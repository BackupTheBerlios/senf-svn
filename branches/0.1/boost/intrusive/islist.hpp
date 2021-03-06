/////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Olaf Krzikalla 2004-2006.
// (C) Copyright Ion Gazta�aga  2006-2007
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/intrusive for documentation.
//
/////////////////////////////////////////////////////////////////////////////

#ifndef BOOST_INTRUSIVE_ISLIST_HPP
#define BOOST_INTRUSIVE_ISLIST_HPP

#include "detail/config_begin.hpp"
#include <boost/utility.hpp>
#include <boost/static_assert.hpp>
#include <boost/assert.hpp>
#include <boost/type_traits/is_convertible.hpp>
#include "islist_hook.hpp"
#include "slist_algorithms.hpp"
#include "detail/pointer_type.hpp"
#include "detail/pointer_to_other.hpp"
#include "linking_policy.hpp"
#include <iterator>
#include <cstddef>

namespace boost {
namespace intrusive {

//! The class template islist is an intrusive container, that encapsulates 
//! a singly-linked list. You can use such a list to squeeze the last bit 
//! of performance from your application. Unfortunately, the little gains 
//! come with some huge drawbacks. A lot of member functions can't be 
//! implemented as efficiently as for standard containers. To overcome 
//! this limitation some other member functions with rather unusual semantics 
//! have to be introduced.
//!
//! The template parameter ValueTraits is called "value traits". It stores
//! information and operations about the type to be stored in the container.
//!
//! If the user specifies ConstantTimeSize as "true", a member of type SizeType
//! will be embedded in the class, that will keep track of the number of stored objects.
//! This will allow constant-time O(1) size() member, instead of default O(N) size.
//! 
//! The iterators of islist are forward iterators. islist provides a static 
//! function called "previous" to compute the previous iterator of a given iterator. 
//! This function has linear complexity. To improve the usability esp. with 
//! the '*_after' functions, ++end() == begin() and previous(begin()) == end() 
//! are defined. In addition, whenever you have an end iterator, 'after this 
//! iterator' means 'at the beginning of the list'. To improve the self-documentation
//! a "before_begin()" function is defined, returning the end() iterator.
template < class ValueTraits
         , bool ConstantTimeSize = true
         , class SizeType        = std::size_t>
class islist
   :  private detail::size_holder<ConstantTimeSize, SizeType>
{
   private:
   typedef islist<ValueTraits, ConstantTimeSize, SizeType>  this_type; 
   typedef typename ValueTraits::node_traits                node_traits;
   typedef detail::size_holder<ConstantTimeSize, SizeType>  size_traits;

   //! This class is
   //! non-copyable
   islist (const islist&);

   //! This class is
   //! non-asignable
   islist &operator =(const islist&);

   //Public typedefs
   public:
   typedef typename ValueTraits::value_type        value_type;
   typedef typename ValueTraits::pointer           pointer;
   typedef typename ValueTraits::const_pointer     const_pointer;
   typedef value_type&                             reference;
   typedef const value_type&                       const_reference;
   typedef SizeType                                size_type;
   typedef typename std::iterator_traits
      <pointer>::difference_type                   difference_type;

   class iterator;
   class const_iterator;
   friend class iterator;
   friend class const_iterator;

   private:
   typedef typename node_traits::node              node;
   typedef typename boost::pointer_to_other
      <pointer, node>::type                        node_ptr;
   typedef typename boost::pointer_to_other
      <pointer, const node>::type                  const_node_ptr;
   typedef slist_algorithms<node_traits>  node_algorithms;
   enum { safemode_or_autounlink  = 
            (int)ValueTraits::linking_policy == (int)auto_unlink   ||
            (int)ValueTraits::linking_policy == (int)safe_mode_link     };

   //Constant-time size is incompatible with auto-unlink hooks!
   BOOST_STATIC_ASSERT(!(ConstantTimeSize && ((int)ValueTraits::linking_policy == (int)auto_unlink)));

   node root;

   node_ptr get_root_node()
   {  return node_ptr(&root);  }

   const_node_ptr get_root_node() const
   {  return const_node_ptr(&root);  }

   static node_ptr uncast(const_node_ptr ptr)
   {
      using boost::get_pointer;
      return node_ptr(const_cast<node*>(get_pointer(ptr)));
   }

   static iterator previous_node(iterator beg, iterator i)
   {
      return iterator
         (node_algorithms::get_previous_node(beg.list_node(), i.list_node()));
   }

   static const_iterator previous_node(const_iterator beg, const_iterator i)
   {
      return const_iterator
         (node_algorithms::get_previous_node(beg.list_node(), i.list_node()));
   }

   //This functor compares a stored value
   //and the one passed as an argument
   class equal_to_value
   {
      const value_type &t_;

      public:
      equal_to_value(const value_type &t)
         :  t_(t)
      {}

      bool operator()(const value_type &t)const
      {  return t_ == t;   }
   };

   public:

   //!The forward iterator of the container
   class iterator
      :  public detail::slist_iterator<value_type, iterator, node_traits>
   {
      private:
      // gcc warns about an ambiguity between iterator::value_type and
      // islist<ValueTraits>::value_type, thus I introduce a unique name:
      typedef typename islist<ValueTraits, ConstantTimeSize, SizeType>::value_type private_vt;
      typedef typename islist<ValueTraits, ConstantTimeSize, SizeType>::pointer    private_pointer;
      typedef typename islist<ValueTraits, ConstantTimeSize, SizeType>::reference  private_reference;
      typedef detail::slist_iterator<private_vt, iterator, node_traits>  inherited;

      public:
      iterator ()
      {}

      private_pointer     operator->() const
      { return  ValueTraits::to_value_ptr(this->list_node()); }

      private_reference   operator*() const
      { return *ValueTraits::to_value_ptr(this->list_node()); }

      node_ptr list_node()const
      {  return inherited::list_node();   }

      private:
      explicit iterator (node_ptr node)
         :  inherited (node)
      {}

      friend class islist<ValueTraits, ConstantTimeSize, SizeType>;
      friend class detail::slist_iterator<private_vt, iterator, node_traits>;
      friend class const_iterator;
   };

   //!The forward const_iterator of the container
   class const_iterator
      :  public detail::slist_iterator<const value_type, const_iterator, node_traits>
   {
      private:
      typedef const typename islist<ValueTraits, ConstantTimeSize, SizeType>::value_type private_vt;
      typedef typename islist<ValueTraits, ConstantTimeSize, SizeType>::const_pointer    private_pointer;
      typedef typename islist<ValueTraits, ConstantTimeSize, SizeType>::const_reference  private_reference;
      typedef detail::slist_iterator<private_vt, const_iterator, node_traits>  inherited;
      
      public: 
      const_iterator()
      {}

      const_iterator(const typename islist::iterator& it)
         :  inherited (it.list_node())
      {}

      const_iterator & operator=(const typename islist::iterator& it)
      {  return inherited::operator=(it.list_node());  }

      private_pointer   operator->()
      { return  ValueTraits::to_value_ptr(this->list_node()); }

      private_reference operator*()
      { return *ValueTraits::to_value_ptr(this->list_node()); }
      
      private:
      explicit const_iterator (const_node_ptr node)
         :  inherited (uncast(node))
      {}

      friend class islist<ValueTraits, ConstantTimeSize, SizeType>;
      friend class detail::slist_iterator<private_vt, const_iterator, node_traits>;
   };

   public:

   //! <b>Effects</b>: constructs an empty list. 
   //! 
   //! <b>Complexity</b>: Constant 
   //! 
   //! <b>Throws</b>: If value_traits::node_traits::node
   //!   constructor throws (this does not happen with predefined Boost.Intrusive hooks).
   islist()
   {
      size_traits::set_size(size_type(0));
      node_algorithms::init(node_ptr(&root)); 
   }

   //! <b>Requires</b>: Dereferencing iterator must yield an lvalue of type value_type.
   //! 
   //! <b>Effects</b>: Constructs a list equal to [first,last).
   //! 
   //! <b>Complexity</b>: Linear in std::distance(b, e). No copy constructors are called.  
   //! 
   //! <b>Throws</b>: If value_traits::node_traits::node
   //!   constructor throws (this does not happen with predefined Boost.Intrusive hooks).
   template<class Iterator>
   islist(Iterator b, Iterator e)
   {
      size_traits::set_size(size_type(0));
      node_algorithms::init(node_ptr(&root));
      insert_after(before_begin(), b, e);
   }

   //! <b>Effects</b>: If it's a safe-mode
   //!   or auto-unlink value, the destructor does nothing
   //!   (ie. no code is generated). Otherwise it detaches all elements from this. 
   //!   In this case the objects in the list are not deleted (i.e. no destructors 
   //!   are called), but the hooks according to the ValueTraits template parameter
   //!   are set to their default value.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements in the list, if 
   //!   it's a safe-mode or auto-unlink value. Otherwise constant.
   ~islist()
   {  this->clear(); }

   //! <b>Effects</b>: Erases all the elements of the container.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements of the list.
   //!   if it's a safe-mode or auto-unlink value_type. Constant time otherwise.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the erased elements.
   void clear()
   {
      if(safemode_or_autounlink){
         this->erase_after(this->before_begin(), this->end()); 
      }
      else{
         node_algorithms::init(node_ptr(&root));
         size_traits::set_size(size_type(0));
      }
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all the elements of the container
   //!   Destroyer::operator()(pointer) is called for the removed elements.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements of the list.
   //! 
   //! <b>Note</b>: Invalidates the iterators to the erased elements.
   template <class Destroyer>
   void clear_and_destroy(Destroyer destroyer)
   {  this->erase_after_and_destroy(this->before_begin(), this->end(), destroyer);   }

   //! <b>Requires</b>: value must be an lvalue.
   //! 
   //! <b>Effects</b>: Inserts the value in the front of the list.
   //!   No copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   void push_front(value_type &value) 
   {
      node_ptr to_insert(ValueTraits::to_node_ptr(value));
      if(safemode_or_autounlink)
         BOOST_ASSERT(node_algorithms::unique(to_insert));
      node_algorithms::link_after(get_root_node(), to_insert); 
      size_traits::increment();
   }

   //! <b>Effects</b>: Erases the first element of the list.
   //!   No destructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the erased element.
   void pop_front() 
   {
      node_ptr to_erase = node_traits::get_next(get_root_node());
      node_algorithms::unlink_after(get_root_node());
      size_traits::decrement();
      if(safemode_or_autounlink)
         node_algorithms::init(to_erase);
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the first element of the list.
   //!   Destroyer::operator()(pointer) is called for the removed element.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Invalidates the iterators to the erased element.
   template<class Destroyer>
   void pop_front_and_destroy(Destroyer destroyer)
   {
      node_ptr to_erase = node_traits::get_next(get_root_node());
      this->pop_front();
      destroyer(ValueTraits::to_value_ptr(to_erase));
   }

   //! <b>Effects</b>: Returns a reference to the first element of the list.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   reference front()
   { return *ValueTraits::to_value_ptr(node_traits::get_next(get_root_node())); }

   //! <b>Effects</b>: Returns a const_reference to the first element of the list.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_reference front() const
   { return *ValueTraits::to_value_ptr(uncast(node_traits::get_next(const_node_ptr(&root)))); }

   //! <b>Effects</b>: Returns an iterator to the first element contained in the list.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   iterator begin() 
   { return iterator (node_traits::get_next(get_root_node())); }

   //! <b>Effects</b>: Returns a const_iterator to the first element contained in the list.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator begin() const 
   { return const_iterator (node_traits::get_next(get_root_node())); }

   //! <b>Effects</b>: Returns an iterator to the end of the list.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   iterator end() 
   { return iterator (get_root_node()); }

   //! <b>Effects</b>: Returns a const_iterator to the end of the list.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator end() const 
   { return const_iterator (get_root_node()); }

   //! <b>Effects</b>: Returns an iterator that points to a position
   //!   before the first element. Equivalent to "end()"
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   iterator before_begin() 
   { return end(); }

   //! <b>Effects</b>: Returns an iterator that points to a position
   //!   before the first element. Equivalent to "end()"
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   const_iterator before_begin() const 
   { return end(); }

   //! <b>Effects</b>: Returns the number of the elements contained in the list.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements contained in the list.
   //!   if ConstantTimeSize is false. Constant time otherwise.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   size_type size() const
   {
      if(ConstantTimeSize)
         return size_traits::get_size();
      else
         return node_algorithms::count(const_node_ptr(&root)) - 1; 
   }

   //! <b>Effects</b>: Returns true if the list contains no elements.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   bool empty() const
   { return node_algorithms::unique(get_root_node()); }

   //! <b>Effects</b>: Swaps the elements of x and *this.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements of both lists.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   void swap(islist& other)
   {
      node_algorithms::swap_nodes(get_root_node(), &other.root);
      if(ConstantTimeSize){
         size_type backup = size_traits::get_size();
         size_traits::set_size(other.get_size());
         other.set_size(backup);
      }
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all the elements from *this
   //!   calling Destroyer::operator()(pointer), clones all the 
   //!   elements from src calling Cloner::operator()(const value_type &)
   //!   and inserts them on *this.
   //!
   //!   If cloner throws, all cloned elements are unlinked and destroyed
   //!   calling Destroyer::operator()(pointer).
   //!   
   //! <b>Complexity</b>: Linear to erased plus inserted elements.
   //! 
   //! <b>Throws</b>: If cloner throws.
   template <class Cloner, class Destroyer>
   void clone_from(const islist &src, Cloner cloner, Destroyer destroyer)
   {  
      clone_and_reverse_from(src, cloner, destroyer);
      this->reverse();
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases all the elements from *this
   //!   calling Destroyer::operator()(pointer), clones all the 
   //!   elements from src calling Cloner::operator()(const value_type &)
   //!   and inserts them on *this in the reverse order than
   //!   the original container.
   //!
   //!   If cloner throws, all cloned elements are unlinked and destroyed
   //!   calling Destroyer::operator()(pointer).
   //!   
   //! <b>Complexity</b>: Linear to erased plus inserted elements.
   //! 
   //! <b>Throws</b>: If cloner throws.
   //! 
   //! <b>Note</b>: This function is more efficient than "clone_from".
   template <class Cloner, class Destroyer>
   void clone_and_reverse_from(const islist &src, Cloner cloner, Destroyer destroyer)
   {
      this->clear_and_destroy(destroyer);
      try{
         const_iterator b(src.begin()), e(src.end());
         for(; b != e; ++b){
            this->push_front(*cloner(*b));
         }
      }
      catch(...){
         clear_and_destroy(destroyer);
         throw;
      }
   }

   //! <b>Requires</b>: value must be an lvalue and prev_p must point to an element
   //!   contained by the list or to end().
   //!
   //! <b>Effects</b>: Inserts the value after the position pointed by prev_p.
   //!    No copy constructor is called.
   //!
   //! <b>Returns</b>: An iterator to the inserted element.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   iterator insert_after(iterator prev_p, value_type &value)
   {
      node_ptr n = ValueTraits::to_node_ptr(value);
      if(safemode_or_autounlink)
         BOOST_ASSERT(node_algorithms::unique(n));
      node_algorithms::link_after(prev_p.list_node(), n);
      size_traits::increment();
      return iterator (n);
   }

   //! <b>Requires</b>: Dereferencing iterator must yield 
   //!   an lvalue of type value_type and prev_p must point to an element
   //!   contained by the list or to the end node.
   //! 
   //! <b>Effects</b>: Inserts the [first, last)
   //!   after the position prev_p.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements inserted.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   template<class Iterator>
   void insert_after(iterator prev_p, Iterator first, Iterator last)
   {
      for (; first != last; ++first)
         prev_p = insert_after(prev_p, *first);
   }

   //! <b>Requires</b>: value must be an lvalue and p must point to an element
   //!   contained by the list or to end().
   //!
   //! <b>Effects</b>: Inserts the value before the position pointed by p.
   //!   No copy constructor is called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements before p. 
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   iterator insert(iterator p, value_type &value)
   {  return insert_after(this->previous(p), value);  }

   //! <b>Requires</b>: Dereferencing iterator must yield 
   //!   an lvalue of type value_type and p must point to an element 
   //!   contained by the list or to the end node.
   //! 
   //! <b>Effects</b>: Inserts the pointed by b and e
   //!   before the position p. No copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements inserted plus linear
   //!   to the elements before b.
   //! 
   //! <b>Note</b>: Does not affect the validity of iterators and references.
   template<class Iterator>
   void insert(iterator p, Iterator b, Iterator e)
   {  return insert_after(this->previous(p), b, e);  }

   //! <b>Effects</b>: Erases the element after the element pointed by prev of 
   //!   the list. No destructors are called.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed elements,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the
   //!   erased element.
   iterator erase_after(iterator prev)
   {
      iterator it(prev); ++it;
      node_ptr to_erase(it.list_node());
      node_algorithms::unlink_after(prev.list_node());
      size_traits::decrement();
      iterator ret(++prev);
      if(safemode_or_autounlink)
         node_algorithms::init(to_erase);
      return ret;
   }

   //! <b>Effects</b>: Erases the range (before_first, last) from
   //!   the list. No destructors are called.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed elements,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Lineal to the elements (last - before_first).
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the
   //!   erased element.
   iterator erase_after(iterator before_first, iterator last)
   {
      iterator first;
      while(++(first = before_first) != last){
         this->erase_after(before_first);
      }
      return last;
   }

   //! <b>Effects</b>: Erases the element pointed by i of the list. 
   //!   No destructors are called.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed element,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the elements before i.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the
   //!   erased element.
   iterator erase(iterator i)
   {  return this->erase_after(this->previous(i));  }

   //! <b>Requires</b>: first and last must be valid iterator to elements in *this.
   //! 
   //! <b>Effects</b>: Erases the range pointed by b and e.
   //!   No destructors are called.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed elements,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements erased plus linear
   //!   to the elements before first.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the
   //!   erased elements.
   iterator erase(iterator first, iterator last)
   {  return erase_after(this->previous(first), last);  }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the element after the element pointed by prev of 
   //!   the list.
   //!   Destroyer::operator()(pointer) is called for the removed element.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed elements,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Invalidates the iterators to the erased element.
   template<class Destroyer>
   iterator erase_after_and_destroy(iterator prev, Destroyer destroyer)
   {
      iterator it(prev); ++it;
      node_ptr to_erase(it.list_node());
      iterator ret(this->erase_after(prev));
      destroyer(ValueTraits::to_value_ptr(to_erase));
      return ret;
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the range (before_first, last) from
   //!   the list.
   //!   Destroyer::operator()(pointer) is called for the removed elements.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed elements,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Lineal to the elements (last - before_first).
   //! 
   //! <b>Note</b>: Invalidates the iterators to the erased element.
   template<class Destroyer>
   iterator erase_after_and_destroy(iterator before_first, iterator last, Destroyer destroyer)
   {
      iterator first;
      while(++(first = before_first) != last){
         this->erase_after_and_destroy(before_first, destroyer);
      }
      return last;
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Erases the element pointed by i of the list. 
   //!   No destructors are called.
   //!   Destroyer::operator()(pointer) is called for the removed element.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed element,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the elements before i.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the
   //!   erased element.
   template<class Destroyer>
   iterator erase_and_destroy(iterator i, Destroyer destroyer)
   {  return this->erase_after_and_destroy(this->previous(i), destroyer);  }

   //! <b>Requires</b>: first and last must be valid iterator to elements in *this.
   //!                  Destroyer::operator()(pointer) shouldn't throw.
   //! 
   //! <b>Effects</b>: Erases the range pointed by b and e.
   //!   No destructors are called.
   //!   Destroyer::operator()(pointer) is called for the removed elements.
   //!
   //! <b>Returns</b>: the first element remaining beyond the removed elements,
   //!   or end() if no such element exists.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements erased plus linear
   //!   to the elements before first.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references) to the
   //!   erased elements.
   template<class Destroyer>
   iterator erase_and_destroy(iterator first, iterator last, Destroyer destroyer)
   {  return erase_after_and_destroy(this->previous(first), last, destroyer);  }

   //! <b>Requires</b>: Dereferencing iterator must yield 
   //!   an lvalue of type value_type.
   //! 
   //! <b>Effects</b>: Clears the list and inserts the range pointed by b and e.
   //!   No destructors or copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements inserted plus
   //!   linear to the elements contained in the list if it's a safe-mode
   //!   or auto-unlink value.
   //!   Linear to the number of elements inserted in the list otherwise.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!   to the erased elements.
   template<class Iterator>
   void assign(Iterator b, Iterator e)
   {
      this->clear();
      this->insert_after(before_begin(), b, e);
   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Requires</b>: Dereferencing iterator must yield 
   //!   an lvalue of type value_type.
   //! 
   //! <b>Effects</b>: Clears the list and inserts the range pointed by b and e.
   //!   No destructors or copy constructors are called.
   //!   Destroyer::operator()(pointer) is called for the removed elements.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements inserted plus
   //!   linear to the elements contained in the list.
   //! 
   //! <b>Note</b>: Invalidates the iterators (but not the references)
   //!   to the erased elements.
   template<class Iterator, class Destroyer>
   void assign_and_destroy(Iterator b, Iterator e, Destroyer destroyer)
   {
      this->clear_and_destroy(destroyer);
      this->insert_after_and_destroy(before_begin(), b, e, destroyer);
   }

   //! <b>Requires</b>: prev is an iterator to an element or x.end()/x.before_begin() in x.
   //! 
   //! <b>Effects</b>: Transfers all the elements of list x to this list, after the
   //! the element pointed by prev. No destructors or copy constructors are called.
   //! 
   //! <b>Returns</b>: The last element inserted of x or prev if x is empty.
   //!   This iterator can be used as new "prev" iterator for a new splice_after call.
   //!   that will splice new values after the previously spliced values.
   //! 
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the elements contained in x
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //! list. Iterators of this list and all the references are not invalidated.
   iterator splice_after(iterator prev, islist &x)
   {
      if (!x.empty()){
         iterator last_x(x.previous(x.end()));
         node_algorithms::transfer_after
            ( prev.list_node()
            , x.end().list_node()
            , last_x.list_node());
         size_traits::set_size(size_traits::get_size() + x.get_size());
         x.set_size(size_type(0));
         return last_x;
      }
      else{
         return prev;
      }
   }

   //! <b>Requires</b>: prev must point to an element contained by this list or
   //!   to the before_begin() element. prev_ele must point to an element contained in list
   //!   x or must be x.before_begin().
   //! 
   //! <b>Effects</b>: Transfers the element after prev_ele, from list x to this list, 
   //!   after the element pointed by prev. No destructors or copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant.
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //! list. Iterators of this list and all the references are not invalidated.
   void splice_after(iterator prev, islist &x, iterator prev_ele)
   {
      iterator nxt = prev_ele;
      ++nxt;
      if (nxt != prev && prev_ele != prev){
         node_algorithms::transfer_after
            (prev.list_node(), prev_ele.list_node(), nxt.list_node());
         size_traits::increment();
         x.decrement();
      }
   }

   //! <b>Requires</b>: prev_pos must be a dereferenceable iterator in *this or be
   //!   before_begin(), and before_first and before_last belong to x and
   //!   ++before_first != x.end() && before_last != x.end(). 
   //! 
   //! <b>Effects</b>: Transfers the range (before_first, before_last] from list x to this
   //!   list, after the element pointed by prev_pos.
   //!   No destructors or copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements transferred
   //!   if ConstantTimeSize is true. Constant-time otherwise.
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //!   list. Iterators of this list and all the references are not invalidated.
   void splice_after(iterator prev_pos, islist &x, iterator before_first, iterator before_last)
   {
      if (before_first != before_last){
         if(ConstantTimeSize){
            size_type increment = std::distance(before_first, before_last);
            node_algorithms::transfer_after
               (prev_pos.list_node(), before_first.list_node(), before_last.list_node());
            size_traits::set_size(size_traits::get_size() + increment);
            x.set_size(x.get_size() - increment);
         }
         else{
            node_algorithms::transfer_after
               (prev_pos.list_node(), before_first.list_node(), before_last.list_node());
         }
      }
   }

   //! <b>Requires</b>: prev_pos must be a dereferenceable iterator in *this or be
   //!   before_begin(), and before_first and before_last belong to x and
   //!   ++before_first != x.end() && before_last != x.end() and
   //!   n == std::distance(before_first, before_last).
   //! 
   //! <b>Effects</b>: Transfers the range (before_first, before_last] from list x to this
   //!   list, after the element pointed by p. No destructors or copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant time.
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //!   list. Iterators of this list and all the references are not invalidated.
   void splice_after(iterator prev_pos, islist &x, iterator before_first, iterator before_last, difference_type n)
   {
      if(n){
         if(ConstantTimeSize){
            BOOST_ASSERT(std::distance(before_first, before_last) == n);
            node_algorithms::transfer_after
               (prev_pos.list_node(), before_first.list_node(), before_last.list_node());
            size_traits::set_size(size_traits::get_size() + n);
            x.set_size(x.get_size() - n);
         }
         else{
            node_algorithms::transfer_after
               (prev_pos.list_node(), before_first.list_node(), before_last.list_node());
         }
      }
   }

   //! <b>Requires</b>: it is an iterator to an element in x.
   //! 
   //! <b>Effects</b>: Transfers all the elements of list x to this list, before the
   //! the element pointed by it. No destructors or copy constructors are called.
   //! 
   //! <b>Returns</b>: The last element inserted of x or the previous element
   //!   of it if x is empty.
   //!   This iterator can be used as new "prev" iterator for a new splice call.
   //!   that will splice new values after the previously spliced values.
   //! 
   //! <b>Throws</b>: Nothing.
   //!
   //! <b>Complexity</b>: Linear to the elements contained in x plus linear to
   //!   the elements before it.
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //! list. Iterators of this list and all the references are not invalidated.
   iterator splice(iterator it, islist &x)
   {  return splice_after(this->previous(it), x);   }

   //! <b>Requires</b>: it p must be a valid iterator of *this.
   //!   elem must point to an element contained in list
   //!   x.
   //! 
   //! <b>Effects</b>: Transfers the element elem, from list x to this list, 
   //!   before the element pointed by pos. No destructors or copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the elements before pos and before elem.
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //! list. Iterators of this list and all the references are not invalidated.
   void splice(iterator pos, islist &x, iterator elem)
   {  return splice_after(this->previous(pos), x, this->previous(elem));  }

   //! <b>Requires</b>: pos must be a dereferenceable iterator in *this
   //!   and first and last belong to x and first and last a valid range on x. 
   //! 
   //! <b>Effects</b>: Transfers the range [first, last) from list x to this
   //!   list, before the element pointed by pos.
   //!   No destructors or copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the sum of elements before pos, first, and last.
   //!   Plus linear to the number of elements transferred if ConstantTimeSize is true.
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //!   list. Iterators of this list and all the references are not invalidated.
   void splice(iterator pos, islist &x, iterator first, iterator last)
   {  return splice_after(this->previous(pos), x, this->previous(first), this->previous(last));  }

   //! <b>Requires</b>: pos must be a dereferenceable iterator in *this
   //!   and first and last belong to x and first and last a valid range on x. 
   //!   n == std::distance(first, last).
   //! 
   //! <b>Effects</b>: Transfers the range [first, last) from list x to this
   //!   list, before the element pointed by pos.
   //!   No destructors or copy constructors are called.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the sum of elements before pos, first, and last.
   //! 
   //! <b>Note</b>: Iterators of values obtained from list x now point to elements of this
   //!   list. Iterators of this list and all the references are not invalidated.
   void splice(iterator pos, islist &x, iterator first, iterator last, difference_type n)
   {  return splice_after(this->previous(pos), x, this->previous(first), this->previous(last), n);  }

   //! <b>Effects</b>: This function sorts the list *this according to std::less<value_type>. 
   //!   The sort is stable, that is, the relative order of equivalent elements is preserved.
   //! 
   //! <b>Throws</b>: If value_traits::node_traits::node
   //!   constructor throws (this does not happen with predefined Boost.Intrusive hooks)
   //!   or the predicate throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: The number of comparisons is approximately N log N, where N
   //!   is the list's size.
   //!
   //! <b>Note</b>: Iterators and references are not invalidated
   template<class Predicate>
   void sort(Predicate p)
   {
      if (!this->empty() &&
            node_traits::get_next(node_traits::get_next(get_root_node()))
               != this->get_root_node()) {
         islist carry;
         islist counter[64];
         int fill = 0;
         iterator last_inserted;
         while(!this->empty()){
            last_inserted = this->begin();
            carry.splice_after(carry.before_begin(), *this, this->before_begin());
            int i = 0;
            while(i < fill && !counter[i].empty()) {
               last_inserted = carry.merge(counter[i++], p);
            }
            BOOST_ASSERT(counter[i].empty());

            iterator last_element(previous_node(last_inserted, carry.end()));
            if(ConstantTimeSize){
               counter[i].splice_after( counter[i].end(), carry
                                      , carry.before_begin(), last_element
                                      , carry.size());
            }
            else{
               counter[i].splice_after( counter[i].end(), carry
                                      , carry.before_begin(), last_element);
            }
            //counter[i].splice_after(counter[i].end(), carry, carry.end(), previous_node(last_inserted, carry.end()));
            //carry.swap(counter[i]);
            if(i == fill)
               ++fill;
         }

         for (int i = 1; i < fill; ++i)
            last_inserted = counter[i].merge(counter[i-1], p);
         //this->swap(counter[fill-1]);
         BOOST_ASSERT(this->empty());

         iterator last_element(previous_node(last_inserted, counter[--fill].end()));
         if(ConstantTimeSize){
            this->splice_after( end(), counter[fill], counter[fill].before_begin()
                              , last_element, counter[fill].size());
         }
         else{
            this->splice_after( end(), counter[fill], counter[fill].before_begin()
                              , last_element);
         }
      }
   }

   //! <b>Requires</b>: p must be a comparison function that induces a strict weak
   //!   ordering and both *this and x must be sorted according to that ordering
   //!   The lists x and *this must be distinct. 
   //! 
   //! <b>Effects</b>: This function removes all of x's elements and inserts them
   //!   in order into *this. The merge is stable; that is, if an element from *this is 
   //!   equivalent to one from x, then the element from *this will precede the one from x. 
   //! 
   //! <b>Throws</b>: If value_traits::node_traits::node
   //!   constructor throws (this does not happen with predefined Boost.Intrusive hooks)
   //!   or std::less<value_type> throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: This function is linear time: it performs at most
   //!   size() + x.size() - 1 comparisons.
   //! 
   //! <b>Note</b>: Iterators and references are not invalidated.
   void sort()
   { this->sort(std::less<value_type>()); }

   //! <b>Requires</b>: p must be a comparison function that induces a strict weak
   //!   ordering and both *this and x must be sorted according to that ordering
   //!   The lists x and *this must be distinct. 
   //! 
   //! <b>Effects</b>: This function removes all of x's elements and inserts them
   //!   in order into *this. The merge is stable; that is, if an element from *this is 
   //!   equivalent to one from x, then the element from *this will precede the one from x. 
   //! 
   //! <b>Returns</b>: An iterator to the last transferred value, end() is x is empty.
   //! 
   //! <b>Throws</b>: If the predicate throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: This function is linear time: it performs at most
   //!   size() + x.size() - 1 comparisons.
   //! 
   //! <b>Note</b>: Iterators and references are not invalidated.
   template<class Predicate>
   iterator merge(islist& x, Predicate p) 
   {
      iterator a(before_begin()), e(end()), ax(x.before_begin());
      iterator last_inserted(e);
      iterator a_next;
      while(++(a_next = a) != e && !x.empty()) {
         iterator ix(ax);
         iterator cx;
         size_type n(0);
         while(++(cx = ix) != ax && p(*cx, *a_next)){
            ++ix; ++n;
         }
         if(ax != ix){
            this->splice_after(a, x, ax, ix, n);
            last_inserted = ix;
         }
         a = a_next;
      }  
      if (!x.empty()){
         last_inserted = this->splice_after(a, x);
      }
      return last_inserted;
   }

   //! <b>Effects</b>: This function removes all of x's elements and inserts them
   //!   in order into *this according to std::less<value_type>. The merge is stable; 
   //!   that is, if an element from *this is equivalent to one from x, then the element 
   //!   from *this will precede the one from x. 
   //! 
   //! <b>Throws</b>: if std::less<value_type> throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: This function is linear time: it performs at most
   //!   size() + x.size() - 1 comparisons.
   //! 
   //! <b>Note</b>: Iterators and references are not invalidated
   void merge(islist& x)
   { this->merge(x, std::less<value_type>()); }

   //! <b>Effects</b>: Reverses the order of elements in the list. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: This function is linear to the contained elements.
   //! 
   //! <b>Note</b>: Iterators and references are not invalidated
   void reverse() 
   {  node_algorithms::reverse(node_ptr(&root));  }

   //! <b>Effects</b>: Removes all the elements that compare equal to value.
   //!   No destructors are called.
   //! 
   //! <b>Throws</b>: If std::equal_to<value_type> throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time. It performs exactly size() comparisons for equality.
   //! 
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid. This function is 
   //!   linear time: it performs exactly size() comparisons for equality.
   void remove(const value_type &value)
   {  remove_if(equal_to_value(value));  }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Removes all the elements that compare equal to value.
   //!   Destroyer::operator()(pointer) is called for every removed element.
   //!
   //! <b>Throws</b>: If std::equal_to<value_type> throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time. It performs exactly size() comparisons for equality.
   //! 
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid.
   template<class Destroyer>
   void remove_and_destroy(const value_type &value, Destroyer destroyer)
   {  remove_and_destroy_if(equal_to_value(value), destroyer);  }

   //! <b>Effects</b>: Removes all the elements for which a specified
   //!   predicate is satisfied. No destructors are called.
   //! 
   //! <b>Throws</b>: If pred throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time. It performs exactly size() calls to the predicate.
   //! 
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid.
   template<class Pred>
   void remove_if(Pred pred)
   {  remove_and_destroy_if(pred, detail::null_destroyer());   }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Removes all the elements for which a specified
   //!   predicate is satisfied.
   //!   Destroyer::operator()(pointer) is called for every removed element.
   //!
   //! <b>Throws</b>: If pred throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time. It performs exactly size() comparisons for equality.
   //!
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid.
   template<class Pred, class Destroyer>
   void remove_and_destroy_if(Pred pred, Destroyer destroyer)
   {
      iterator bcur(this->before_begin()), cur, e(this->end());
      
      while(++(cur = bcur) != e){
         if (pred(*cur)){
            pointer p = cur.operator->();
            this->erase_after(bcur);
            destroyer(p);
         }
         else{
            ++bcur;
         }
      }
   }

   //! <b>Effects</b>: Removes adjacent duplicate elements or adjacent 
   //!   elements that are equal from the list. No destructors are called.
   //! 
   //! <b>Throws</b>: If std::equal_to<value_type> throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time (size()-1) comparisons calls to pred()).
   //! 
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid.
   void unique()
   {  unique_and_destroy(std::equal_to<value_type>(), detail::null_destroyer());  }

   //! <b>Effects</b>: Removes adjacent duplicate elements or adjacent 
   //!   elements that satisfy some binary predicate from the list.
   //!   No destructors are called.
   //! 
   //! <b>Throws</b>: If the predicate throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time (size()-1) comparisons equality comparisons.
   //! 
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid.
   template<class BinaryPredicate>
   void unique(BinaryPredicate pred)
   {  unique_and_destroy(pred, detail::null_destroyer());  }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Removes adjacent duplicate elements or adjacent 
   //!   elements that satisfy some binary predicate from the list.
   //!   Destroyer::operator()(pointer) is called for every removed element.
   //! 
   //! <b>Throws</b>: If std::equal_to<value_type> throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time (size()-1) comparisons equality comparisons.
   //! 
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid.
   template<class Destroyer>
   void unique_and_destroy(Destroyer destroyer)
   {  unique(std::equal_to<value_type>(), destroyer);  }

   //! <b>Requires</b>: Destroyer::operator()(pointer) shouldn't throw.
   //!
   //! <b>Effects</b>: Removes adjacent duplicate elements or adjacent 
   //!   elements that satisfy some binary predicate from the list.
   //!   Destroyer::operator()(pointer) is called for every removed element.
   //! 
   //! <b>Throws</b>: If the predicate throws. Basic guarantee.
   //! 
   //! <b>Complexity</b>: Linear time (size()-1) comparisons equality comparisons.
   //! 
   //! <b>Note</b>: The relative order of elements that are not removed is unchanged,
   //!   and iterators to elements that are not removed remain valid.
   template<class BinaryPredicate, class Destroyer>
   void unique_and_destroy(BinaryPredicate pred, Destroyer destroyer)
   {
      iterator end_n(end());
      iterator cur(begin());
      iterator cur_next;

      if (cur != end_n) {
         while(++(cur_next = cur) != end_n) {
            if (pred(*cur, *cur_next)){
               pointer p = cur_next.operator->();
               this->erase_after(cur);
               destroyer(p);
            }
            else{
               ++cur;
            }
         }
      }
   }

   //! <b>Requires</b>: value must be a reference to a value inserted in a list.
   //! 
   //! <b>Effects</b>: This function returns a const_iterator pointing to the element
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant time.
   //! 
   //! <b>Note</b>: Iterators and references are not invalidated.
   static iterator current(value_type &value) 
   { 
      BOOST_ASSERT (!node_algorithms::unique(ValueTraits::to_node_ptr(value)));
      return iterator (ValueTraits::to_node_ptr(value)); 
   }

   //! <b>Requires</b>: value must be a const reference to a value inserted in a list.
   //! 
   //! <b>Effects</b>: This function returns an iterator pointing to the element.
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Constant time.
   //! 
   //! <b>Note</b>: Iterators and references are not invalidated.
   static const_iterator current(const value_type &value) 
   { 
      BOOST_ASSERT (!node_algorithms::unique(ValueTraits::to_node_ptr(const_cast<value_type&> (value))));
      return const_iterator (ValueTraits::to_node_ptr(const_cast<value_type&> (value))); 
   }

   //! <b>Returns</b>: The iterator to the element before i in the list. 
   //!   Returns the end-iterator, if either i is the begin-iterator or the 
   //!   list is empty. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements before i. 
   iterator previous(iterator i)
   {
      return iterator
         (node_algorithms::get_previous_node
            (before_begin().list_node(), i.list_node()));
   }

   //! <b>Returns</b>: The const_iterator to the element before i in the list. 
   //!   Returns the end-const_iterator, if either i is the begin-const_iterator or 
   //!   the list is empty. 
   //! 
   //! <b>Throws</b>: Nothing.
   //! 
   //! <b>Complexity</b>: Linear to the number of elements before i. 
   const_iterator previous(const_iterator i) const
   {
      return const_iterator
         (node_algorithms::get_previous_node
            (before_begin().list_node(), i.list_node()));
   }
};

template <class V, bool C, class S>
inline bool operator==(const islist<V, C, S>& x, const islist<V, C, S>& y)
{
   if(C && x.size() != y.size()){
      return false;
   }
   typedef typename islist<V, C, S>::const_iterator const_iterator;
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

template <class V, bool C, class S>
inline bool operator<(const islist<V, C, S>& x,
                      const islist<V, C, S>& y)
{  return std::lexicographical_compare(x.begin(), x.end(), y.begin(), y.end());  }

template <class V, bool C, class S>
inline bool operator!=(const islist<V, C, S>& x, const islist<V, C, S>& y) 
{  return !(x == y); }

template <class V, bool C, class S>
inline bool operator>(const islist<V, C, S>& x, const islist<V, C, S>& y) 
{  return y < x;  }

template <class V, bool C, class S>
inline bool operator<=(const islist<V, C, S>& x, const islist<V, C, S>& y) 
{  return !(y < x);  }

template <class V, bool C, class S>
inline bool operator>=(const islist<V, C, S>& x, const islist<V, C, S>& y) 
{  return !(x < y);  }

template <class V, bool C, class S>
inline void swap(islist<V, C, S>& x, islist<V, C, S>& y)
{  x.swap(y);  }

} //namespace intrusive 
} //namespace boost 

#include "detail/config_end.hpp"

#endif //BOOST_INTRUSIVE_ISLIST_HPP
