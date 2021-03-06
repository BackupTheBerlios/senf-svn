/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gazta�aga  2006-2007
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////
#ifndef BOOST_INTRUSIVE_IRBTREE_HPP
#define BOOST_INTRUSIVE_IRBTREE_HPP

#include "config_begin.hpp"
#include <functional>
#include <iterator>
#include <boost/utility.hpp>
#include <boost/compressed_pair.hpp>
#include <utility>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include "pointer_type.hpp"
#include "pointer_to_other.hpp"
#include "../iset_hook.hpp"
#include "rbtree_node.hpp"
#include "ebo_holder.hpp"
#include "../rbtree_algorithms.hpp"
#include "../linking_policy.hpp"
#include <cstddef>
#include <iterator>

namespace boost {
namespace intrusive {
namespace detail {

//! The class template irbtree is an intrusive red-black tree container, that
//! is used to construct intrusive set and tree containers. The no-throw 
//! guarantee holds only, if the Compare object 
//! doesn't throw.
template < class ValueTraits
         , class Compare         = std::less<typename ValueTraits::value_type>
         , bool ConstantTimeSize = true
         , class SizeType = std::size_t
         >
class irbtree
   :  private detail::size_holder<ConstantTimeSize, SizeType>
{
   private:
   typedef irbtree<ValueTraits, Compare
                  ,ConstantTimeSize, SizeType>              this_type; 
   typedef typename ValueTraits::node_traits                node_traits;
   typedef detail::size_holder<ConstantTimeSize, SizeType>    size_traits;

   //noncopyable
   irbtree (const irbtree&);
   irbtree operator =(const irbtree&);

   public:
   typedef typename ValueTraits::value_type        value_type;
   typedef typename ValueTraits::pointer           pointer;
   typedef typename ValueTraits::const_pointer     const_pointer;
   typedef value_type&                             reference;
   typedef const value_type&                       const_reference;
   typedef SizeType                                size_type;
   typedef typename std::iterator_traits
      <pointer>::difference_type                   difference_type;
   typedef value_type                              key_type;
   typedef Compare                                 value_compare;
   class iterator;
   class const_iterator;
   friend class iterator;
   friend class const_iterator;

   private:
   typedef typename node_traits::node              node;
   typedef typename boost::pointer_to_other
      <pointer, node>::type                        node_ptr;
   typedef typename boost::pointer_to_other
      <node_ptr, const node>::type                 const_node_ptr;
   typedef rbtree_algorithms<node_traits>          node_algorithms;
   enum { safemode_or_autounlink  = 
            (int)ValueTraits::linking_policy == (int)auto_unlink   ||
            (int)ValueTraits::linking_policy == (int)safe_mode_link     };

   //Constant-time size is incompatible with auto-unlink hooks!
   BOOST_STATIC_ASSERT(!(ConstantTimeSize && ((int)ValueTraits::linking_policy == (int)auto_unlink)));

   template<class KeyValueCompare>
   struct key_node_ptr_compare
      :  private ebo_holder<KeyValueCompare>
   {
      typedef ebo_holder<KeyValueCompare> base_t;
      key_node_ptr_compare(KeyValueCompare kcomp)
         :  base_t(kcomp)
      {}

      template<class KeyType>
      bool operator()(node_ptr node, const KeyType &key) const
      {  return base_t::get()(*ValueTraits::to_value_ptr(node), key); }

      template<class KeyType>
      bool operator()(const KeyType &key, node_ptr node) const
      {  return base_t::get()(key, *ValueTraits::to_value_ptr(node)); }

      bool operator()(node_ptr node1, node_ptr node2) const
      {
         return base_t::get()
            (*ValueTraits::to_value_ptr(node1), *ValueTraits::to_value_ptr(node2)); 
      }
   };

   //Use boost::compressed_pair to get EBO if possible
   boost::compressed_pair<node, Compare> members_;
   
   const Compare &priv_comp() const
   {  return members_.second();  }

   Compare &priv_comp()
   {  return members_.second();  }

   const node &priv_header() const
   {  return members_.first();  }

   node &priv_header()
   {  return members_.first();  }

   template<class F>
   struct value_to_node_cloner
      :  private ebo_holder<F>
   {
      typedef ebo_holder<F> base_t;
      value_to_node_cloner(F f)
         :  base_t(f)
      {}
      
      node_ptr operator()(node_ptr p)
      {  return ValueTraits::to_node_ptr(*base_t::get()(*ValueTraits::to_value_ptr(p))); }
   };

   template<class F>
   struct value_to_node_destroyer
      :  private ebo_holder<F>
   {
      typedef ebo_holder<F> base_t;
      value_to_node_destroyer(F f)
         :  base_t(f)
      {}

      void operator()(node_ptr p)
      {  base_t::get()(ValueTraits::to_value_ptr(p));   }
   };

   static node_ptr uncast(const_node_ptr ptr)
   {
      using boost::get_pointer;
      return node_ptr(const_cast<node*>(get_pointer(ptr)));
   }

   public:
   typedef typename node_algorithms::insert_commit_data insert_commit_data;

   class iterator
      :  public detail::rbtree_iterator <value_type, iterator, node_traits>
    {
      private:
      typedef typename irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>::value_type   private_vt;
      typedef typename irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>::pointer      private_pointer;
      typedef typename irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>::reference    private_reference;
      typedef detail::rbtree_iterator<private_vt, iterator, node_traits>   inherited;

      public:
      iterator ()
      {}

      private_pointer operator->() const
      { return  ValueTraits::to_value_ptr(this->tree_node()); }

      private_reference operator*() const
      { return *ValueTraits::to_value_ptr(this->tree_node()); }

      private:
      explicit iterator(node_ptr node)
         :  inherited(node)
      {}
      
      friend class irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>; 
      friend class detail::rbtree_iterator<private_vt, iterator, node_traits>;
      friend class const_iterator;
   };

   class const_iterator
      :  public detail::rbtree_iterator<const value_type, const_iterator, node_traits>
   {
      private:
      typedef const typename irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>::value_type private_vt;
      typedef typename irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>::const_pointer    private_pointer;
      typedef typename irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>::const_reference  private_reference;
      typedef detail::rbtree_iterator<private_vt, const_iterator, node_traits>   inherited;

      public:
      const_iterator ()
      {}

      const_iterator(const typename irbtree::iterator& it)
         :  inherited (it.tree_node())
      {}

      const_iterator & operator=(const typename irbtree::iterator& it)
      {  return inherited::operator=(it.tree_node());  }

      private_pointer   operator->()
      { return  ValueTraits::to_value_ptr(this->tree_node()); }

      private_reference operator*()
      { return *ValueTraits::to_value_ptr(this->tree_node()); }

      private:
      explicit const_iterator (const_node_ptr p)
         :  inherited (uncast(p))
      {}

      friend class irbtree<ValueTraits, Compare, ConstantTimeSize, SizeType>; 
      friend class detail::rbtree_iterator<private_vt, const_iterator, node_traits>;
   };

   typedef std::reverse_iterator<iterator>         reverse_iterator;
   typedef std::reverse_iterator<const_iterator>   const_reverse_iterator;

   public:

   //! <b>Effects</b>: Constructs an empty tree. 
   //!   
   //! <b>Complexity</b>: Constant. 
   //! 
   //! <b>Throws</b>: Nothing unless the copy constructor of the Compare object throws. 
   irbtree(Compare cmp = Compare()) 
      :  members_(cmp)
   {  
      node_algorithms::init_header(&priv_header());  
      size_traits::set_size(size_type(0));
   }

   //! <b>Requires</b>: Dereferencing iterator must yield an lvalue of type value_type. 
   //!   cmp must be a comparison function that induces a strict weak ordering.
   //! 
   //! <b>Effects</b>: Constructs an empty tree and inserts elements from 
   //!   [b, e).
   //! 
   //! <b>Complexity</b>: Linear in N if [b, e) is already sorted using 
   //!   comp and otherwise N * log N, where N is last � first.
   //! 
   //! <b>Throws</b>: Nothing unless the copy constructor of the Compare object throws. 
   template<class Iterator>
   irbtree(bool unique, Iterator b, Iterator e, Compare cmp = Compare())
      : members_(cmp)
   {
      node_algorithms::init_header(&priv_header());
      size_traits::set_size(size_type(0));
      if(unique)
         this->insert_unique(b, e);
      else
         this->insert_equal(b, e);
   }

   //! <b>Effects</b>: Detaches all elements from this. The objects in the set 
   //!   are not deleted (i.e. no destructors are called), but the nodes according to 
   //!   the ValueTraits template parameter are reinitialized and thus can be reused. 
   //! 
   //! <b>Complexity</b>: Linear to elements contained in *this. 
   //! 
   //! <b>Throws</b>: Nothing.
   ~irbtree() 
   {  this->clear(); }

   //! <b>Effects</b>: Returns an iterator pointing to the beginning of the tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   iterator begin()
   {  return iterator (node_traits::get_left(node_ptr(&priv_header())));   }

   //! <b>Effects</b>: Returns a const_iterator pointing to the beginning of the tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   const_iterator begin() const
   {  return const_iterator (node_traits::get_left(const_node_ptr(&priv_header())));   }

   //! <b>Effects</b>: Returns an iterator pointing to the end of the tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   iterator end()
   {  return iterator (node_ptr(&priv_header()));  }

   //! <b>Effects</b>: Returns a const_iterator pointing to the end of the tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   const_iterator end() const
   {  return const_iterator (const_node_ptr(&priv_header()));  }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the beginning of the
   //!    reversed tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   reverse_iterator rbegin()
   {  return reverse_iterator(end());  }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the beginning
   //!    of the reversed tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   const_reverse_iterator rbegin() const
   {  return const_reverse_iterator(end());  }

   //! <b>Effects</b>: Returns a reverse_iterator pointing to the end
   //!    of the reversed tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   reverse_iterator rend()
   {  return reverse_iterator(begin());   }

   //! <b>Effects</b>: Returns a const_reverse_iterator pointing to the end
   //!    of the reversed tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   const_reverse_iterator rend() const
   {  return const_reverse_iterator(begin());   }

   //! <b>Effects</b>: Returns the value_compare object used by the tree.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: If value_compare copy-constructor throws.
   value_compare value_comp() const
   {  return priv_comp();   }

   //! <b>Effects</b>: Returns true is the container is empty.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   bool empty() const
   {  return node_algorithms::unique(const_node_ptr(&priv_header()));   }

   //! <b>Effects</b>: Returns the number of elements stored in the tree.
   //! 
   //! <b>Complexity</b>: Linear to elements contained in *this.
   //! 
   //! <b>Throws</b>: Nothing.
   size_type size() const
   {
      if(ConstantTimeSize)
         return size_traits::get_size();
      else
         return empty() ? 0 : node_algorithms::count(node_traits::get_parent(const_node_ptr(&priv_header())));
   }

   //! <b>Effects</b>: Swaps the contents of two multisets.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: If the comparison functor's unspecified swap call throws.
   void swap(irbtree& other)
   {
      //This can throw
      using std::swap;
      swap(priv_comp(), priv_comp());
      //These can't throw
      node_algorithms::swap_tree(node_ptr(&priv_header()), node_ptr(&other.priv_header()));
      if(ConstantTimeSize){
         size_type backup = size_traits::get_size();
         size_traits::set_size(other.get_size());
         other.set_size(backup);
      }
   }

   //! <b>Requires</b>: value must be an lvalue
   //! 
   //! <b>Effects</b>: Inserts value into the tree before the upper bound.
   //! 
   //! <b>Complexity</b>: Average complexity for insert element is at
   //!   most logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   iterator insert_equal_upper_bound(value_type &value)
   {
      key_node_ptr_compare<value_compare> key_node_comp(priv_comp());
      node_ptr to_insert(ValueTraits::to_node_ptr(value));
      if(safemode_or_autounlink)
         BOOST_ASSERT(node_algorithms::unique(to_insert));
      size_traits::increment();
      return iterator(node_algorithms::insert_equal_upper_bound
         (node_ptr(&priv_header()), to_insert, key_node_comp));
   }

   //! <b>Requires</b>: value must be an lvalue
   //! 
   //! <b>Effects</b>: Inserts value into the tree before the lower bound.
   //! 
   //! <b>Complexity</b>: Average complexity for insert element is at
   //!   most logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   iterator insert_equal_lower_bound(value_type &value)
   {
      key_node_ptr_compare<value_compare> key_node_comp(priv_comp());
      node_ptr to_insert(ValueTraits::to_node_ptr(value));
      if(safemode_or_autounlink)
         BOOST_ASSERT(node_algorithms::unique(to_insert));
      size_traits::increment();
      return iterator(node_algorithms::insert_equal_lower_bound
         (node_ptr(&priv_header()), to_insert, key_node_comp));
   }

   //! <b>Requires</b>: value must be an lvalue, and "hint" must be
   //!   a valid iterator.
   //! 
   //! <b>Effects</b>: Inserts x into the tree, using "hint" as a hint to
   //!   where it will be inserted. If "hint" is the upper_bound
   //!   the insertion takes constant time (two comparisons in the worst case)
   //! 
   //! <b>Complexity</b>: Logarithmic in general, but it is amortized
   //!   constant time if t is inserted immediately before hint.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   iterator insert_equal(const_iterator hint, value_type &value)
   {
      key_node_ptr_compare<value_compare> key_node_comp(priv_comp());
      node_ptr to_insert(ValueTraits::to_node_ptr(value));
      if(safemode_or_autounlink)
         BOOST_ASSERT(node_algorithms::unique(to_insert));
      size_traits::increment();
      return iterator(node_algorithms::insert_equal
         (node_ptr(&priv_header()), hint.tree_node(), to_insert, key_node_comp));
   }

   //! <b>Requires</b>: Dereferencing iterator must yield an lvalue 
   //!   of type value_type.
   //! 
   //! <b>Effects</b>: Inserts a each element of a range into the tree
   //!   before the upper bound of the key of each element.
   //! 
   //! <b>Complexity</b>: Insert range is in general O(N * log(N)), where N is the 
   //!   size of the range. However, it is linear in N if the range is already sorted 
   //!   by value_comp().
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   template<class Iterator>
   void insert_equal(Iterator b, Iterator e)
   {
      if(this->empty()){
         iterator end(this->end());
         for (; b != e; ++b)
            this->insert_equal(end, *b);
      }
      else{
         for (; b != e; ++b)
            this->insert_equal_upper_bound(*b);
      }
   }

   //! <b>Requires</b>: value must be an lvalue
   //! 
   //! <b>Effects</b>: Inserts value into the tree if the value
   //!   is not already present.
   //! 
   //! <b>Complexity</b>: Average complexity for insert element is at
   //!   most logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   std::pair<iterator, bool> insert_unique(value_type &value)
   {
      insert_commit_data commit_data;
      std::pair<iterator, bool> ret = insert_unique_check(value, commit_data);
      if(!ret.second)
         return ret;
      return std::pair<iterator, bool> (insert_unique_commit(value, commit_data), true);
   }

   //! <b>Requires</b>: value must be an lvalue, and "hint" must be
   //!   a valid iterator
   //! 
   //! <b>Effects</b>: Tries to insert x into the tree, using "hint" as a hint
   //!   to where it will be inserted.
   //! 
   //! <b>Complexity</b>: Logarithmic in general, but it is amortized
   //!   constant time (two comparisons in the worst case)
   //!   if t is inserted immediately before hint.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   iterator insert_unique(const_iterator hint, value_type &value)
   {
      insert_commit_data commit_data;
      std::pair<iterator, bool> ret = insert_unique_check(hint, value, commit_data);
      if(!ret.second)
         return ret.first;
      return insert_unique_commit(value, commit_data);
   }

   //! <b>Requires</b>: Dereferencing iterator must yield an lvalue 
   //!   of type value_type.
   //! 
   //! <b>Effects</b>: Tries to insert each element of a range into the tree.
   //! 
   //! <b>Complexity</b>: Insert range is in general O(N * log(N)), where N is the 
   //!   size of the range. However, it is linear in N if the range is already sorted 
   //!   by value_comp().
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   //!   No copy-constructors are called.
   template<class Iterator>
   void insert_unique(Iterator b, Iterator e)
   {
      if(this->empty()){
         iterator end(this->end());
         for (; b != e; ++b)
            this->insert_unique(end, *b);
      }
      else{
         for (; b != e; ++b)
            this->insert_unique(*b);
      }
   }

   std::pair<iterator, bool> insert_unique_check
      (const value_type &value, insert_commit_data &commit_data)
   {  return insert_unique_check(value, priv_comp(), commit_data); }

   template<class KeyType, class KeyValueCompare>
   std::pair<iterator, bool> insert_unique_check
      (const KeyType &key, KeyValueCompare key_value_comp, insert_commit_data &commit_data)
   {
      key_node_ptr_compare<KeyValueCompare> comp(key_value_comp);
      std::pair<node_ptr, bool> ret = 
         (node_algorithms::insert_unique_check
            (node_ptr(&priv_header()), key, comp, commit_data));
      return std::pair<iterator, bool>(iterator(ret.first), ret.second);
   }

   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const value_type &value, insert_commit_data &commit_data)
   {  return insert_unique_check(hint, value, priv_comp(), commit_data); }

   template<class KeyType, class KeyValueCompare>
   std::pair<iterator, bool> insert_unique_check
      (const_iterator hint, const KeyType &key
      ,KeyValueCompare key_value_comp, insert_commit_data &commit_data)
   {
      key_node_ptr_compare<KeyValueCompare> comp(key_value_comp);
      std::pair<node_ptr, bool> ret = 
         (node_algorithms::insert_unique_check
            (node_ptr(&priv_header()), hint.tree_node(), key, comp, commit_data));
      return std::pair<iterator, bool>(iterator(ret.first), ret.second);
   }

   iterator insert_unique_commit(value_type &value, const insert_commit_data &commit_data)
   {
      node_ptr to_insert(ValueTraits::to_node_ptr(value));
      if(safemode_or_autounlink)
         BOOST_ASSERT(node_algorithms::unique(to_insert));
      size_traits::increment();
      node_algorithms::insert_unique_commit
               (node_ptr(&priv_header()), to_insert, commit_data);
      return iterator(to_insert);
   }

   //! <b>Effects</b>: Erases the element pointed to by pos. 
   //! 
   //! <b>Complexity</b>: Average complexity for erase element is constant time. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   iterator erase(iterator i)
   {
      iterator ret(i);
      ++ret;
      node_ptr to_erase(i.tree_node());
      if(safemode_or_autounlink)
         BOOST_ASSERT(!node_algorithms::unique(to_erase));
      node_algorithms::erase(&priv_header(), to_erase);
      size_traits::decrement();
      if(safemode_or_autounlink)
         node_algorithms::init(to_erase);
      return ret;
   }

   //! <b>Effects</b>: Erases the range pointed to by b end e. 
   //! 
   //! <b>Complexity</b>: Average complexity for erase range is at most 
   //!   O(log(size() + N)), where N is the number of elements in the range.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   iterator erase(iterator b, iterator e)
   {  size_type n;   return private_erase(b, e, n);   }

   //! <b>Effects</b>: Erases all the elements with the given value.
   //! 
   //! <b>Returns</b>: The number of erased elements.
   //! 
   //! <b>Complexity</b>: O(log(size() + N).
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   size_type erase(const value_type &value)
   {  return this->erase(value, priv_comp());   }

   //! <b>Effects</b>: Erases all the elements with the given key.
   //!   according to the comparison functor "comp".
   //!
   //! <b>Returns</b>: The number of erased elements.
   //! 
   //! <b>Complexity</b>: O(log(size() + N).
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   template<class KeyType, class KeyValueCompare>
   size_type erase(const KeyType& key, KeyValueCompare comp)
   {
      std::pair<iterator,iterator> p = this->equal_range(key, comp);
      size_type n;
      private_erase(p.first, p.second, n);
      return n;
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the element pointed to by pos. 
   //!   Destroyer::operator()(pointer) is called for the removed element.
   //! 
   //! <b>Complexity</b>: Average complexity for erase element is constant time. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators 
   //!    to the erased elements.
   template<class Destroyer>
   iterator erase_and_destroy(iterator i, Destroyer destroyer)
   {
      node_ptr to_erase(i.tree_node());
      iterator ret(this->erase(i));
      destroyer(ValueTraits::to_value_ptr(to_erase));
      return ret;
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the range pointed to by b end e.
   //!   Destroyer::operator()(pointer) is called for the removed elements.
   //! 
   //! <b>Complexity</b>: Average complexity for erase range is at most 
   //!   O(log(size() + N)), where N is the number of elements in the range.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators
   //!    to the erased elements.
   template<class Destroyer>
   iterator erase_and_destroy(iterator b, iterator e, Destroyer destroyer)
   {  size_type n;   return private_erase(b, e, n, destroyer);   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all the elements with the given value.
   //!   Destroyer::operator()(pointer) is called for the removed elements.
   //! 
   //! <b>Returns</b>: The number of erased elements.
   //! 
   //! <b>Complexity</b>: O(log(size() + N).
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   template<class Destroyer>
   size_type erase_and_destroy(const value_type &value, Destroyer destroyer)
   {
      std::pair<iterator,iterator> p = this->equal_range(value);
      size_type n;
      private_erase(p.first, p.second, n, destroyer);
      return n;
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all the elements with the given key.
   //!   according to the comparison functor "comp".
   //!   Destroyer::operator()(pointer) is called for the removed elements.
   //!
   //! <b>Returns</b>: The number of erased elements.
   //! 
   //! <b>Complexity</b>: O(log(size() + N).
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators
   //!    to the erased elements.
   template<class KeyType, class KeyValueCompare, class Destroyer>
   size_type erase_and_destroy(const KeyType& key, KeyValueCompare comp, Destroyer destroyer)
   {
      std::pair<iterator,iterator> p = this->equal_range(key, comp);
      size_type n;
      private_erase(p.first, p.second, n, destroyer);
      return n;
   }

   //! <b>Effects</b>: Erases all of the elements. 
   //! 
   //! <b>Complexity</b>: Linear to the number of elements on the container.
   //!   if it's a safe-mode or auto-unlink value_type. Constant time otherwise.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. No destructors are called.
   void clear()
   {
      if(safemode_or_autounlink){
         while(1){
            node_ptr leftmost
               (node_algorithms::unlink_leftmost_without_rebalance
                  (node_ptr(&priv_header())));
            if(!leftmost)
               break;
            size_traits::decrement();
            if(safemode_or_autounlink)
               node_algorithms::init(leftmost);
         }
      }
      else{
         node_algorithms::init_header(&priv_header());
         size_traits::set_size(0);
      }
   }

   //! <b>Effects</b>: Erases all of the elements calling destroyer(p) for
   //!   each node to be erased.
   //! <b>Complexity</b>: Average complexity for is at most O(log(size() + N)),
   //!   where N is the number of elements in the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!    to the erased elements. Calls N times to destroyer functor.
   template<class Destroyer>
   void clear_and_destroy(Destroyer destroyer)
   {
      while(1){
         node_ptr leftmost
            (node_algorithms::unlink_leftmost_without_rebalance
               (node_ptr(&priv_header())));
         if(!leftmost)
            break;
         size_traits::decrement();
         if(safemode_or_autounlink)
            node_algorithms::init(leftmost);
         destroyer(ValueTraits::to_value_ptr(leftmost));
      }
   }

   //! <b>Effects</b>: Returns the number of contained elements with the given value
   //! 
   //! <b>Complexity</b>: Logarithmic to the number of elements contained plus lineal
   //!   to number of objects with the given value.
   //! 
   //! <b>Throws</b>: Nothing.
   size_type count(const value_type &value) const
   {  return this->count(value, priv_comp());   }

   //! <b>Effects</b>: Returns the number of contained elements with the given key
   //! 
   //! <b>Complexity</b>: Logarithmic to the number of elements contained plus lineal
   //!   to number of objects with the given key.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   size_type count(const KeyType &key, KeyValueCompare comp) const
   {
      std::pair<const_iterator, const_iterator> ret = this->equal_range(key, comp);
      return std::distance(ret.first, ret.second);
   }

   //! <b>Effects</b>: Returns an iterator to the first element whose
   //!   key is not less than k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   iterator lower_bound(const value_type &value)
   {  return this->lower_bound(value, priv_comp());   }

   //! <b>Effects</b>: Returns an iterator to the first element whose
   //!   key is not less than k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   const_iterator lower_bound(const value_type &value) const
   {  return this->lower_bound(value, priv_comp());   }

   //! <b>Effects</b>: Returns an iterator to the first element whose
   //!   key is not less than k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   iterator lower_bound(const KeyType &key, KeyValueCompare comp)
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      return iterator(node_algorithms::lower_bound
         (const_node_ptr(&priv_header()), key, key_node_comp));
   }

   //! <b>Effects</b>: Returns a const iterator to the first element whose
   //!   key is not less than k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   const_iterator lower_bound(const KeyType &key, KeyValueCompare comp) const
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      return const_iterator(node_algorithms::lower_bound
         (const_node_ptr(&priv_header()), key, key_node_comp));
   }

   //! <b>Effects</b>: Returns an iterator to the first element whose
   //!   key is greater than k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   iterator upper_bound(const value_type &value)
   {  return this->upper_bound(value, priv_comp());   }

   //! <b>Effects</b>: Returns an iterator to the first element whose
   //!   key is greater than k according to comp or end() if that element
   //!   does not exist.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   iterator upper_bound(const KeyType &key, KeyValueCompare comp)
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      return iterator(node_algorithms::upper_bound
         (const_node_ptr(&priv_header()), key, key_node_comp));
   }

   //! <b>Effects</b>: Returns an iterator to the first element whose
   //!   key is greater than k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   const_iterator upper_bound(const value_type &value) const
   {  return this->upper_bound(value, priv_comp());   }

   //! <b>Effects</b>: Returns an iterator to the first element whose
   //!   key is greater than k according to comp or end() if that element
   //!   does not exist.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   const_iterator upper_bound(const KeyType &key, KeyValueCompare comp) const
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      return const_iterator(node_algorithms::upper_bound
         (const_node_ptr(&priv_header()), key, key_node_comp));
   }

   //! <b>Effects</b>: Finds an iterator to the first element whose key is 
   //!   k or end() if that element does not exist.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   iterator find(const value_type &value)
   {  return this->find(value, priv_comp()); }

   //! <b>Effects</b>: Finds an iterator to the first element whose key is 
   //!   k or end() if that element does not exist.
   //!
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   iterator find(const KeyType &key, KeyValueCompare comp)
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      return iterator
         (node_algorithms::find(const_node_ptr(&priv_header()), key, key_node_comp));
   }

   //! <b>Effects</b>: Finds a const_iterator to the first element whose key is 
   //!   k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   const_iterator find(const value_type &value) const
   {  return this->find(value, priv_comp()); }

   //! <b>Effects</b>: Finds a const_iterator to the first element whose key is 
   //!   k or end() if that element does not exist.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   const_iterator find(const KeyType &key, KeyValueCompare comp) const
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      return const_iterator
         (node_algorithms::find(const_node_ptr(&priv_header()), key, key_node_comp));
   }

   //! <b>Effects</b>: Finds a range containing all elements whose key is k or
   //!   an empty range that indicates the position where those elements would be
   //!   if they there is no elements with key k.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   std::pair<iterator,iterator> equal_range(const value_type &value)
   {  return this->equal_range(value, priv_comp());   }

   //! <b>Effects</b>: Finds a range containing all elements whose key is k or
   //!   an empty range that indicates the position where those elements would be
   //!   if they there is no elements with key k.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   std::pair<iterator,iterator> equal_range(const KeyType &key, KeyValueCompare comp)
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      std::pair<node_ptr, node_ptr> ret
         (node_algorithms::equal_range(const_node_ptr(&priv_header()), key, key_node_comp));
      return std::pair<iterator, iterator>(iterator(ret.first), iterator(ret.second));
   }

   //! <b>Effects</b>: Finds a range containing all elements whose key is k or
   //!   an empty range that indicates the position where those elements would be
   //!   if they there is no elements with key k.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   std::pair<const_iterator, const_iterator>
      equal_range(const value_type &value) const
   {  return this->equal_range(value, priv_comp());   }

   //! <b>Effects</b>: Finds a range containing all elements whose key is k or
   //!   an empty range that indicates the position where those elements would be
   //!   if they there is no elements with key k.
   //! 
   //! <b>Complexity</b>: Logarithmic.
   //! 
   //! <b>Throws</b>: Nothing.
   template<class KeyType, class KeyValueCompare>
   std::pair<const_iterator, const_iterator>
      equal_range(const KeyType &key, KeyValueCompare comp) const
   {
      key_node_ptr_compare<KeyValueCompare> key_node_comp(comp);
      std::pair<node_ptr, node_ptr> ret
         (node_algorithms::equal_range(const_node_ptr(&priv_header()), key, key_node_comp));
      return std::pair<const_iterator, const_iterator>(const_iterator(ret.first), const_iterator(ret.second));
   }

   template <class Cloner, class Destroyer>
   void clone_from(const irbtree &src, Cloner cloner, Destroyer destroyer)
   {
      this->clear_and_destroy(destroyer);
      if(!src.empty()){
         node_algorithms::clone_tree
            (const_node_ptr(&src.priv_header())
            ,node_ptr(&this->priv_header())
            ,value_to_node_cloner<Cloner>(cloner)
            ,value_to_node_destroyer<Destroyer>(destroyer));
         size_traits::set_size(src.get_size());
      }
   }

   pointer unlink_leftmost_without_rebalance()
   {
      node_ptr to_destroy(node_algorithms::unlink_leftmost_without_rebalance
                           (node_ptr(&priv_header())));
      if(!to_destroy)
         return 0;
      size_traits::decrement();
      if(safemode_or_autounlink)
         node_algorithms::init(to_destroy);
      return ValueTraits::to_value_ptr(to_destroy);
   }

   //! <b>Requires</b>: value must be an lvalue and shall be in a set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //! 
   //! <b>Effects</b>: Returns: a valid iterator i belonging to the set
   //!   that points to the value
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   static iterator current(value_type &value)
   {  return iterator (ValueTraits::to_node_ptr(value));  }

   //! <b>Requires</b>: value must be an lvalue and shall be in a set of
   //!   appropriate type. Otherwise the behavior is undefined.
   //! 
   //! <b>Effects</b>: Returns: a valid const_iterator i belonging to the
   //!   set that points to the value
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Throws</b>: Nothing.
   static const_iterator current(const value_type &value) 
   {  return const_iterator (ValueTraits::to_node_ptr(const_cast<value_type&> (value))); }
/*
   //! <b>Requires</b>: value shall not be in a tree of the appropriate type.
   //! 
   //! <b>Effects</b>: init_node post-constructs the node data in x used by multisets of 
   //! the appropriate type. For the accessors multiset_derived_node and multiset_member_node 
   //! init_node has no effect, since the constructors of multiset_node_d and multiset_node_m 
   //! have already initialized the node data. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant time.
   //! 
   //! <b>Note</b>: This function is meant to be used mainly with the member value_traits, 
   //! where no implicit node initialization during construction occurs.
   static void init_node(reference value)
   { node_algorithms::init(node_ptr(&*ValueTraits::to_node_ptr(value))); }

   //! <b>Effects</b>: removes x from a tree of the appropriate type. It has no effect,
   //! if x is not in such a tree. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant time.
   //! 
   //! <b>Note</b>: This static function is only usable with the "safe mode"
   //! hook and non-constant time size lists. Otherwise, the user must use
   //! the non-static "erase(value_type &)" member. If the user calls
   //! this function with a non "safe mode" or constant time size list
   //! a compilation error will be issued.
   template<class T>
   static void remove_node(T& value)
   {
      //This function is only usable for safe mode hooks and non-constant
      //time lists. 
      //BOOST_STATIC_ASSERT((!(safemode_or_autounlink && ConstantTimeSize)));
      BOOST_STATIC_ASSERT((!ConstantTimeSize));
      BOOST_STATIC_ASSERT((boost::is_convertible<T, value_type>::value));
      node_ptr to_remove(ValueTraits::to_node_ptr(value));
      node_algorithms::unlink_and_rebalance(to_remove);
      if(safemode_or_autounlink)
         node_algorithms::init(to_remove);
   }
*/
   private:
   template<class Destroyer>
   iterator private_erase(iterator b, iterator e, size_type &n, Destroyer destroyer)
   {
      for(n = 0; b != e; ++n)
        this->erase_and_destroy(b++, destroyer);
      return b;
   }

   iterator private_erase(iterator b, iterator e, size_type &n)
   {
      for(n = 0; b != e; ++n)
        this->erase(b++);
      return b;
   }
};

template <class V, class P, bool C, class S>
inline bool operator==(const irbtree<V, P, C, S>& x, const irbtree<V, P, C, S>& y)
{
   if(C && x.size() != y.size()){
      return false;
   }
   typedef typename irbtree<V, P, C, S>::const_iterator const_iterator;
   const_iterator end1 = x.end();

   const_iterator i1 = x.begin();
   const_iterator i2 = y.begin();
   if(C){
      while (i1 != end1 && *i1 == *i2) {
         ++i1;
         ++i2;
      }
      return i1 == end1;
   }
   else{
      const_iterator end2 = y.end();
      while (i1 != end1 && i2 != end2 && *i1 == *i2) {
         ++i1;
         ++i2;
      }
      return i1 == end1 && i2 == end2;
   }
}

template <class V, class P, bool C, class S>
inline bool operator<(const irbtree<V, P, C, S>& x,
                      const irbtree<V, P, C, S>& y)
{  return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());  }

template <class V, class P, bool C, class S>
inline bool operator!=(const irbtree<V, P, C, S>& x, const irbtree<V, P, C, S>& y) 
{  return !(x == y); }

template <class V, class P, bool C, class S>
inline bool operator>(const irbtree<V, P, C, S>& x, const irbtree<V, P, C, S>& y) 
{  return y < x;  }

template <class V, class P, bool C, class S>
inline bool operator<=(const irbtree<V, P, C, S>& x, const irbtree<V, P, C, S>& y) 
{  return !(y < x);  }

template <class V, class P, bool C, class S>
inline bool operator>=(const irbtree<V, P, C, S>& x, const irbtree<V, P, C, S>& y) 
{  return !(x < y);  }

template <class V, class P, bool C, class S>
inline void swap(irbtree<V, P, C, S>& x, irbtree<V, P, C, S>& y)
{  x.swap(y);  }

} //namespace detail
} //namespace intrusive 
} //namespace boost 

#include "config_end.hpp"

#endif //BOOST_INTRUSIVE_IRBTREE_HPP
