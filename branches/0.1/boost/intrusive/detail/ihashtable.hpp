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
#ifndef BOOST_INTRUSIVE_IHASHTABLE_HPP
#define BOOST_INTRUSIVE_IHASHTABLE_HPP

#include "config_begin.hpp"
//std C++
#include <functional>
#include <iterator>
#include <utility>
#include <algorithm>
//boost
#include <boost/utility.hpp>
#include <boost/compressed_pair.hpp>
#include <boost/assert.hpp>
#include <boost/static_assert.hpp>
#include <boost/functional/hash.hpp>
//General intrusive utilities
#include "pointer_type.hpp"
#include "pointer_to_other.hpp"
#include "../linking_policy.hpp"
//Implementation utilities
#include "../iunordered_set_hook.hpp"
#include "../islist.hpp"
#include <cstddef>
#include <iterator>

namespace boost {
namespace intrusive {
namespace detail {

template<int Dummy = 0>
struct prime_list_holder
{
   static const std::size_t prime_list[];
   static const std::size_t prime_list_size;
};

template<int Dummy>
const std::size_t prime_list_holder<Dummy>::prime_list[] = {
   53ul, 97ul, 193ul, 389ul, 769ul,
   1543ul, 3079ul, 6151ul, 12289ul, 24593ul,
   49157ul, 98317ul, 196613ul, 393241ul, 786433ul,
   1572869ul, 3145739ul, 6291469ul, 12582917ul, 25165843ul,
   50331653ul, 100663319ul, 201326611ul, 402653189ul, 805306457ul,
   1610612741ul, 3221225473ul, 4294967291ul };

template<int Dummy>
const std::size_t prime_list_holder<Dummy>::prime_list_size
   = sizeof(prime_list)/sizeof(std::size_t);

//! The class template ihashtable is an intrusive hash table container, that
//! is used to construct intrusive unordered_set and unordered_multiset containers. The
//! no-throw guarantee holds only, if the Equal object and Hasher don't throw.
template< class ValueTraits
        , class Hash             = boost::hash<typename ValueTraits::value_type>
        , class Equal            = std::equal_to<typename ValueTraits::value_type>
        , bool  ConstantTimeSize = true
        , class SizeType         = std::size_t
        >
class ihashtable
   :  private detail::size_holder<ConstantTimeSize, SizeType>
{
   private:
   template<class T, class Self, class NodeTraits>
   class hashtable_iterator;

   typedef islist<ValueTraits, false, SizeType> islist_impl;

   struct bucket_type_impl
      :  private islist_impl
   {
      friend class ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>;

      bucket_type_impl()
      {}

      bucket_type_impl(const bucket_type_impl &)
      {}

      bucket_type_impl &operator=(const bucket_type_impl&)
      {  islist_impl::clear();   }
   };

   static islist_impl &islist_from_bucket(bucket_type_impl &b)
   {  return static_cast<islist_impl&>(b);   }

   static const islist_impl &islist_from_bucket(const bucket_type_impl &b)
   {  return static_cast<const islist_impl&>(b);   }

   typedef ihashtable<ValueTraits, Hash, Equal
                     ,ConstantTimeSize, SizeType>           this_type; 
   typedef typename ValueTraits::node_traits                node_traits;
   typedef detail::size_holder<ConstantTimeSize, SizeType>  size_traits;

   //noncopyable
   ihashtable (const ihashtable&);
   ihashtable operator =(const ihashtable&);

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
   typedef Hash                                    hasher;
   typedef Equal                                   key_equal;
   typedef bucket_type_impl                        bucket_type;
   typedef typename boost::pointer_to_other
      <pointer, bucket_type>::type                 bucket_ptr;
   typedef typename islist_impl::iterator          local_iterator;
   typedef typename islist_impl::const_iterator    const_local_iterator;

   class const_iterator;
   class iterator;

   private:
   typedef typename node_traits::node              node;
   typedef typename boost::pointer_to_other
      <pointer, node>::type                        node_ptr;
   typedef typename boost::pointer_to_other
      <node_ptr, const node>::type                 const_node_ptr;
   //typedef hashtable_algorithms<node_traits>       node_algorithms;
   typedef slist_algorithms<node_traits>           node_algorithms;
   enum { safemode_or_autounlink  = 
            (int)ValueTraits::linking_policy == (int)auto_unlink   ||
            (int)ValueTraits::linking_policy == (int)safe_mode_link     };

   //Constant-time size is incompatible with auto-unlink hooks!
   BOOST_STATIC_ASSERT(!(ConstantTimeSize && ((int)ValueTraits::linking_policy == (int)auto_unlink)));

   struct bucket_info_t
   {
      bucket_ptr  buckets_;
      size_type   buckets_len_;
   } ;

   typedef typename boost::pointer_to_other
      <pointer, bucket_info_t>::type               bucket_info_ptr;

   typedef typename boost::pointer_to_other
      <pointer, const bucket_info_t>::type         const_bucket_info_ptr;

   //User scattered boost::compressed pair to get EBO all compilers
   boost::compressed_pair
      <boost::compressed_pair<bucket_info_t, Hash>
      ,Equal> members_;

   const Equal &priv_equal() const
   {  return members_.second();  }

   Equal &priv_equal()
   {  return members_.second();  }

   const_bucket_info_ptr priv_bucket_info() const
   {  return &members_.first().first();  }

   bucket_info_ptr priv_bucket_info()
   {  return &members_.first().first();  }

   const Hash &priv_hasher() const
   {  return members_.first().second();  }

   Hash &priv_hasher()
   {  return members_.first().second();  }

   const bucket_ptr &priv_buckets() const
   {  return members_.first().first().buckets_;  }

   bucket_ptr &priv_buckets()
   {  return members_.first().first().buckets_;  }

   const size_type &priv_buckets_len() const
   {  return members_.first().first().buckets_len_;  }

   size_type &priv_buckets_len()
   {  return members_.first().first().buckets_len_;  }

   static node_ptr uncast(const_node_ptr ptr)
   {
      using boost::get_pointer;
      return node_ptr(const_cast<node*>(get_pointer(ptr)));
   }

   static bucket_info_ptr uncast(const_bucket_info_ptr ptr)
   {
      using boost::get_pointer;
      return bucket_info_ptr(const_cast<bucket_info_t*>(get_pointer(ptr)));
   }

   template<class T, class Self, class NodeTraits>
   class hashtable_iterator
      :  public std::iterator<std::forward_iterator_tag, T>
   {
      protected:
      explicit hashtable_iterator
         (local_iterator ptr, bucket_info_ptr bucket_info, size_type n_bucket)
         :  local_it_ (ptr),   bucket_info_ (bucket_info),   n_bucket_ (n_bucket)
      {}

      public:
      hashtable_iterator ()
      {}

      Self& operator++()
      {
         using boost::get_pointer;
         ++local_it_;
         size_type   buckets_len = bucket_info_->buckets_len_;
         bucket_ptr  buckets     = bucket_info_->buckets_;
         while (local_it_ == islist_from_bucket(buckets[n_bucket_]).end()){
             if (++n_bucket_ == buckets_len) {
                 local_it_ = invalid_local_it(*bucket_info_);
                 break;
             }
             local_it_ = islist_from_bucket(buckets[n_bucket_]).begin();
         }
         return static_cast<Self&> (*this);
      }

      Self operator++(int)
      {
         Self result(local_it_, bucket_info_, n_bucket_);
         ++(*this);
         return result;
      }

      bool operator== (const Self& i) const
      { return local_it_ == i.local(); }

      bool operator!= (const Self& i) const
      { return !operator== (i); }

      Self &set_internals
         (local_iterator ptr, bucket_info_ptr bucket_info, size_type n_bucket)
      {
         local_it_ = ptr;  bucket_info_ = bucket_info;  n_bucket_ = n_bucket;
         return static_cast<Self&>(*this);
      }

      local_iterator local() const
      { return local_it_; }

      bucket_info_ptr priv_bucket_info() const
      { return bucket_info_; }

      size_type bucket_num() const
      { return n_bucket_; }

      private:
      local_iterator    local_it_;
      bucket_info_ptr   bucket_info_;
      size_type         n_bucket_;
   };

   public:

   class iterator
      :  public hashtable_iterator <value_type, iterator, node_traits>
    {
      private:
      typedef typename ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>::value_type   private_vt;
      typedef typename ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>::pointer      private_pointer;
      typedef typename ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>::reference    private_reference;
      typedef hashtable_iterator<private_vt, iterator, node_traits>   inherited;

      public:
      iterator()
      {}

      private_pointer operator->() const
      { return &*this->local(); }

      private_reference operator*() const
      { return *this->local(); }

      private:
      explicit iterator
         (local_iterator local_it, bucket_info_ptr bucket_info, size_type n_bucket)
         :  inherited(local_it, bucket_info, n_bucket)
      {}

      friend class ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>; 
      friend class hashtable_iterator<private_vt, iterator, node_traits>;
      friend class const_iterator;
   };

   class const_iterator
      :  public hashtable_iterator<const value_type, const_iterator, node_traits>
   {
      private:
      typedef const typename ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>::value_type private_vt;
      typedef typename ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>::const_pointer    private_pointer;
      typedef typename ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>::const_reference  private_reference;
      typedef hashtable_iterator<private_vt, const_iterator, node_traits>   inherited;

      public:
      const_iterator()
      {}

      const_iterator(const typename ihashtable::iterator& it)
         :  inherited (it.local(), it.priv_bucket_info(), it.bucket_num())
      {}

      const_iterator & operator=(const typename ihashtable::iterator& it)
      {  return inherited::set_internals(it.local(), it.priv_bucket_info(), it.bucket_num());  }

      private_pointer operator->() const
      { return &*this->local(); }

      private_reference operator*() const
      { return *this->local(); }

      private:
      explicit const_iterator
         (local_iterator ptr, bucket_info_ptr bucket_info, size_type n_bucket)
         :  inherited(ptr, bucket_info, n_bucket)
      {}

      friend class ihashtable<ValueTraits, Hash, Equal, ConstantTimeSize, SizeType>; 
      friend class hashtable_iterator<private_vt, const_iterator, node_traits>;
   };

   //typedef typename node_algorithms::insert_commit_data insert_commit_data;
   struct insert_commit_data
   {
      local_iterator prev_pos;
      size_type   bucket_num;
   };

   ihashtable( bucket_ptr buckets
             , size_type buckets_len
             , const Hash & hasher = Hash()
             , const Equal &equal = Equal()) 
      :  members_(boost::compressed_pair<bucket_info_t, Hash>(hasher)/*hasher*/, equal)
   {
      
      BOOST_ASSERT(buckets_len != 0);
      priv_buckets()       = buckets;
      priv_buckets_len()   = buckets_len;
      priv_clear_buckets();
      size_traits::set_size(size_type(0));
   }

   ~ihashtable() 
   {  this->clear(); }

   iterator begin()
   {
      size_type bucket_num;
      local_iterator local_it = priv_begin(bucket_num);
      return iterator( local_it
                     , this->priv_bucket_info()
                     , bucket_num);
   }

   const_iterator begin() const
   {
      size_type bucket_num;
      local_iterator local_it = priv_begin(bucket_num);
      return const_iterator( local_it
                           , uncast(this->priv_bucket_info())
                           , bucket_num);
   }

   iterator end()
   {
      using boost::get_pointer;
      bucket_info_t *info = get_pointer(this->priv_bucket_info());
      return iterator(invalid_local_it(*info), 0, info->buckets_len_);
   }

   const_iterator end() const
   {
      using boost::get_pointer;
      const bucket_info_t *info = get_pointer(this->priv_bucket_info());
      return const_iterator(invalid_local_it(*info), 0, info->buckets_len_);
   }

   hasher hash_function() const
   {  return this->priv_hasher();  }

   key_equal key_eq() const
   {  return this->priv_equal();   }

   bool empty() const
   {
      if(ConstantTimeSize){
         return !size();
      }
      else{
         using boost::get_pointer;
         size_type buckets_len = this->priv_buckets_len();
         const bucket_type *b = get_pointer(this->priv_buckets());
         for (size_type n = 0; n < buckets_len; ++n, ++b){
            if(!b->empty()){
               return false;
            }
         }
         return true;
      }
   }

   size_type size() const
   {
      if(ConstantTimeSize)
         return size_traits::get_size();
      else{
         size_type len = 0;
         using boost::get_pointer;
         size_type buckets_len = this->priv_buckets_len();
         const bucket_type *b = get_pointer(this->priv_buckets());
         for (size_type n = 0; n < buckets_len; ++n, ++b){
            len += b->size();
         }
         return len;
      }
   }

   void swap(ihashtable& other)
   {
      using std::swap;
      //These can throw
      swap(this->priv_equal(), other.priv_equal());
      swap(this->priv_hasher(), other.priv_hasher());
      //These can't throw
      swap(this->priv_buckets(), other.priv_buckets());
      swap(this->priv_buckets_len(), other.priv_buckets_len());
      if(ConstantTimeSize){
         size_type backup = size_traits::get_size();
         size_traits::set_size(other.get_size());
         other.set_size(backup);
      }
   }

   template <class Cloner, class Destroyer>
   void clone_from(const ihashtable &src, Cloner cloner, Destroyer destroyer)
   {
      this->clear_and_destroy(destroyer);
      if(!ConstantTimeSize || !src.empty()){
         const size_type src_bucket_count = src.bucket_count();
         const size_type dst_bucket_count = this->bucket_count();

         //If src bucket count is bigger or equal, structural copy is possible
         if(src_bucket_count >= dst_bucket_count){
            //First clone the first ones
            const bucket_ptr src_buckets = src.priv_buckets();
            const bucket_ptr dst_buckets = this->priv_buckets();
            size_type constructed;
            try{
               for( constructed = 0
                  ; constructed < dst_bucket_count
                  ; ++constructed){
                  dst_buckets[constructed].clone_and_reverse_from(src_buckets[constructed], cloner, destroyer);
               }
               if(src_bucket_count != dst_bucket_count){
                  //Now insert the remaining ones using the modulo trick
                  for(//"constructed" comes from the previous loop
                     ; constructed < src_bucket_count
                     ; ++constructed){
                     bucket_type &dst_b = dst_buckets[constructed % dst_bucket_count];
                     bucket_type &src_b = src_buckets[constructed];
                     for( local_iterator b(src_b.begin()), e(src_b.end())
                        ; b != e
                        ; ++b){
                        dst_b.push_front(*cloner(*b));
                     }
                  }
               }
            }
            catch(...){
               while(constructed--){
                  dst_buckets[constructed].clear_and_destroy(destroyer);
               }
               throw;
            }
            size_traits::set_size(src.get_size());
         }
         else{
            //Unlike previous cloning algorithm, this can throw
            //if cloner, the hasher or comparison functor throw
            const_iterator b(src.begin()), e(src.end());
            try{
               for(; b != e; ++b){
                  this->insert_equal(*cloner(*b));
               }
            }
            catch(...){
               this->clear_and_destroy(destroyer);
               throw;
            }
         }
      }
   }

   iterator insert_equal(value_type &value)
   {
      size_type bucket_num;
      local_iterator it = priv_find(value, this->priv_hasher(), this->priv_equal(), bucket_num);
      bucket_type &b = this->priv_buckets()[bucket_num];
      if(it == invalid_local_it(*this->priv_bucket_info())){
         it = b.before_begin();
      }
      size_traits::increment();
      return iterator(b.insert_after(it, value), this->priv_bucket_info(), bucket_num);
   }

   template<class Iterator>
   void insert_equal(Iterator b, Iterator e)
   {
      for (; b != e; ++b)
         this->insert_equal(*b);
   }

   std::pair<iterator, bool> insert_unique(value_type &value)
   {
      insert_commit_data commit_data;
      std::pair<iterator, bool> ret = insert_unique_check(value, commit_data);
      if(!ret.second)
         return ret;
      return std::pair<iterator, bool> (insert_unique_commit(value, commit_data), true);
   }

   template<class Iterator>
   void insert_unique(Iterator b, Iterator e)
   {
      for (; b != e; ++b)
         this->insert_unique(*b);
   }

   std::pair<iterator, bool> insert_unique_check
      (const value_type &value, insert_commit_data &commit_data)
   {  return insert_unique_check(value, this->priv_hasher(), this->priv_equal(), commit_data); }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   std::pair<iterator, bool> insert_unique_check
      ( const KeyType &key
      , KeyHasher hasher
      , KeyValueEqual key_value_eq
      , insert_commit_data &commit_data)
   {
      commit_data.prev_pos = 
         priv_find(key, hasher, key_value_eq, commit_data.bucket_num);
      bool success = commit_data.prev_pos == invalid_local_it(*this->priv_bucket_info());
      if(success){
         commit_data.prev_pos = this->priv_buckets()[commit_data.bucket_num].before_begin();
      }
      return std::pair<iterator, bool>
         (iterator(commit_data.prev_pos, this->priv_bucket_info(), commit_data.bucket_num)
         ,success);
   }

   iterator insert_unique_commit(value_type &value, const insert_commit_data &commit_data)
   {
      bucket_type &b = this->priv_buckets()[commit_data.bucket_num];
      size_traits::increment();
      return iterator( b.insert_after(commit_data.prev_pos, value)
                     , this->priv_bucket_info()
                     , commit_data.bucket_num);
   }

   void erase(const_iterator i)
   {  erase_and_destroy(i, detail::null_destroyer());  }

   void erase(const_iterator b, const_iterator e)
   {  erase_and_destroy(b, e, detail::null_destroyer());  }

   size_type erase(const value_type &value)
   {  return this->erase(value, this->priv_hasher(), this->priv_equal());  }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   size_type erase(const KeyType& key, KeyHasher hasher, KeyValueEqual equal)
   {  return erase_and_destroy(key, hasher, equal, detail::null_destroyer()); }

   template<class Destroyer>
   void erase_and_destroy(const_iterator i, Destroyer destroyer)
   {
      local_iterator to_erase(i.local());
      size_type n_bucket = i.bucket_num();
      bucket_type &b = this->priv_buckets()[n_bucket];
      b.erase_after_and_destroy(b.previous(to_erase), destroyer);
      size_traits::decrement();
   }

   template<class Destroyer>
   void erase_and_destroy(const_iterator b, const_iterator e, Destroyer destroyer)
   {
      if(b == e)  return;

      //Get the bucket number and local iterator for both iterators
      size_type first_bucket_num
         = b.bucket_num();
      local_iterator before_first_local_it
         = priv_buckets()[first_bucket_num].previous(b.local());
      size_type last_bucket_num;
      local_iterator last_local_it;

      //For the end iterator, we will assign the end iterator
      //of the last bucket
      if(e == end()){
         last_bucket_num   = this->bucket_count() - 1;
         last_local_it     = priv_buckets()[last_bucket_num].end();
      }
      else{
         last_bucket_num   = e.bucket_num();
         last_local_it     = e.local();
      }

      const bucket_ptr buckets = priv_buckets();
      //First erase the nodes of the first bucket
      {
         bucket_type &first_b = buckets[first_bucket_num];
         local_iterator nxt(before_first_local_it); ++nxt;
         local_iterator end = first_b.end();
         while(nxt != end){
            nxt = first_b.erase_after_and_destroy(before_first_local_it, destroyer);
            size_traits::decrement();
         }
      }

      //Now fully clear the intermediate buckets
      for(size_type i = first_bucket_num+1; i < last_bucket_num; ++i){
         bucket_type &b = buckets[i];
         if(b.empty())
            continue;
         local_iterator b_begin(b.before_begin());
         local_iterator nxt(b_begin); ++nxt;
         local_iterator end = b.end();
         while(nxt != end){
            nxt = b.erase_after_and_destroy(b_begin, destroyer);
            size_traits::decrement();
         }
      }

      //Now erase nodes from the last bucket
      {
         bucket_type &last_b = buckets[last_bucket_num];
         local_iterator b_begin(last_b.before_begin());
         local_iterator nxt(b_begin); ++nxt;
         while(nxt != last_local_it){
            nxt = last_b.erase_after_and_destroy(b_begin, destroyer);
            size_traits::decrement();
         }
      }
   }

   template<class Destroyer>
   size_type erase_and_destroy(const value_type &value, Destroyer destroyer)
   {  return erase_and_destroy(value, priv_hasher(), priv_equal(), destroyer);   }

   template<class KeyType, class KeyHasher, class KeyValueEqual, class Destroyer>
   size_type erase_and_destroy(const KeyType& key, KeyHasher hasher
                  ,KeyValueEqual equal, Destroyer destroyer)
   {
      size_type count(0);

      if(ConstantTimeSize && this->empty()){
         return 0;
      }

      bucket_type &b = this->priv_buckets()[hasher(key) % this->priv_buckets_len()];
      local_iterator it    = b.begin();
      local_iterator prev  = b.before_begin();

      bool found = false;
      //Find equal value
      while(it != b.end()){
         if(equal(key, *it)){
            found = true;
            break;
         }
         ++prev;
         ++it;
      }
   
      if(!found)
         return 0;

      //If found erase all equal values
      for(local_iterator end = b.end(); it != end && equal(key, *it); ++count){
         it = b.erase_after_and_destroy(prev, destroyer);
         size_traits::decrement();
      }
      return count;
   }

   void clear()
   {
      if(safemode_or_autounlink){
         priv_clear_buckets();
      }
      size_traits::set_size(size_type(0));
   }

   template<class Destroyer>
   void clear_and_destroy(Destroyer destroyer)
   {
      if(!ConstantTimeSize || !this->empty()){
         size_type num_buckets = this->bucket_count();
         bucket_ptr b = this->priv_buckets();
         for(; num_buckets--; ++b){
            b->clear_and_destroy(destroyer);
         }
         size_traits::set_size(size_type(0));
      }
   }

   size_type count(const value_type &value) const
   {  return this->count(value, this->priv_hasher(), this->priv_equal());  }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   size_type count(const KeyType &key, const KeyHasher &hasher, const KeyValueEqual &equal) const
   {
      size_type bucket_n1, bucket_n2, count;
      priv_equal_range(key, hasher, equal, bucket_n1, bucket_n2, count);
      return count;
   }

   iterator find(const value_type &value)
   {  return find(value, this->priv_hasher(), this->priv_equal());   }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   iterator find(const KeyType &key, KeyHasher hasher, KeyValueEqual equal)
   {
      size_type bucket_n;
      local_iterator local_it = priv_find(key, hasher, equal, bucket_n);
      return iterator( local_it
                      , this->priv_bucket_info()
                      , bucket_n);
   }

   const_iterator find(const value_type &value) const
   {  return find(value, this->priv_hasher(), this->priv_equal());   }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   const_iterator find
      (const KeyType &key, KeyHasher hasher, KeyValueEqual equal) const
   {
      size_type bucket_n;
      local_iterator local_it = priv_find(key, hasher, equal, bucket_n);
      return const_iterator( local_it
                           , uncast(this->priv_bucket_info())
                           , bucket_n);
   }

   std::pair<iterator,iterator> equal_range(const value_type &value)
   {  return this->equal_range(value, this->priv_hasher(), this->priv_equal());  }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   std::pair<iterator,iterator> equal_range
      (const KeyType &key, KeyHasher hasher, KeyValueEqual equal)
   {
      size_type bucket_n1, bucket_n2, count;
      std::pair<local_iterator, local_iterator> ret
         = priv_equal_range(key, hasher, equal, bucket_n1, bucket_n2, count);
      return std::pair<iterator, iterator>
         (  iterator( ret.first, this->priv_bucket_info(), bucket_n1)
         ,  iterator( ret.second, this->priv_bucket_info(), bucket_n2) );
   }

   std::pair<const_iterator, const_iterator>
      equal_range(const value_type &value) const
   {  return this->equal_range(value, this->priv_hasher(), this->priv_equal());  }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   std::pair<const_iterator,const_iterator> equal_range
      (const KeyType &key, KeyHasher hasher, KeyValueEqual equal) const
   {
      size_type bucket_n1, bucket_n2, count;
      std::pair<local_iterator, local_iterator> ret
         = priv_equal_range(key, hasher, equal, bucket_n1, bucket_n2, count);
      return std::pair<const_iterator, const_iterator>
         (  const_iterator( ret.first, uncast(this->priv_bucket_info()), bucket_n1)
         ,  const_iterator( ret.second, uncast(this->priv_bucket_info()), bucket_n2)  );
   }

   size_type bucket_count() const
   {  return this->priv_buckets_len();   }

   size_type bucket_size(size_type n)
   {  return this->priv_buckets()[n].size();   }

   size_type bucket(const key_type& k)
   {  return this->bucket(k, this->priv_hasher());   }

   template<class KeyType, class KeyHasher>
   size_type bucket(const KeyType& k, const KeyHasher &hasher)
   {  return hasher(k) % this->priv_buckets_len();   }

   bucket_ptr bucket_pointer() const
   {  return this->priv_buckets();   }

   local_iterator begin(size_type n)
   {  return this->priv_buckets()[n].begin();  }

   const_local_iterator begin(size_type n) const
   {  return const_cast<const bucket_type&>(this->priv_buckets()[n]).begin();  }

   local_iterator end(size_type n)
   {  return this->priv_buckets()[n].end();  }

   const_local_iterator end(size_type n) const
   {  return const_cast<const bucket_type&>(this->priv_buckets()[n]).end();  }

   void rehash(bucket_ptr new_buckets, size_type new_buckets_len)
   {
      bucket_ptr old_buckets     = this->priv_buckets();
      size_type  old_buckets_len = this->priv_buckets_len();

      try{
         size_type n = 0;
         bool same_buffer = old_buckets == new_buckets;
         //If we are shrinking the bucket array, just rehash the last nodes
         if(same_buffer && (old_buckets_len > new_buckets_len)){
            n = new_buckets_len;
         }

         //Iterate through nodes
         for(; n < old_buckets_len; ++n){
            bucket_type &old_bucket = old_buckets[n];
            local_iterator before_i(old_bucket.before_begin());
            local_iterator end(old_bucket.end());
            local_iterator i(old_bucket.begin());
            for(;i != end; ++i){
               size_type new_n = this->priv_hasher()(*i) % new_buckets_len;
               //If this is a buffer expansion don't move if it's not necessary
               if(same_buffer && new_n == n){
                  ++before_i;
               }
               else{
                  bucket_type &new_b = new_buckets[new_n];
                  new_b.splice_after(new_b.before_begin(), old_bucket, before_i);
                  i = before_i;
               }
            }
         }

         this->priv_buckets()      = new_buckets;
         this->priv_buckets_len()  = new_buckets_len;
      }
      catch(...){
         for(size_type n = 0; n < new_buckets_len; ++n){
            new_buckets[n].clear();
            old_buckets[n].clear();
         }
         size_traits::set_size(size_type(0));
         throw;
      }
   }

   iterator current(value_type &value)
   {
      return iterator( bucket_type::current(value)
                     , this->priv_bucket_info()
                     , this->priv_hasher()(value) % this->priv_buckets_len());
   }

   const_iterator current(const value_type &value) const
   {
      return const_iterator( bucket_type::current(const_cast<value_type&>(value))
                     , uncast(this->priv_bucket_info())
                     , this->priv_hasher()(value) % this->priv_buckets_len());
   }

   static local_iterator current_local(value_type &value)
   {  return bucket_type::current(value);  }

   static const_local_iterator current_local(const value_type &value)
   {  return bucket_type::current(value);  }

   // no throw
   static size_type suggested_upper_bucket_count(size_type n)
   {
      const std::size_t *primes     = &prime_list_holder<0>::prime_list[0];
      const std::size_t *primes_end = primes + prime_list_holder<0>::prime_list_size;
      size_type const* bound =
            std::lower_bound(primes, primes_end, n);
      if(bound == primes_end)
            bound--;
      return size_type(*bound);
   }

   // no throw
   static size_type suggested_lower_bucket_count(size_type n)
   {
      const std::size_t *primes     = &prime_list_holder<0>::prime_list[0];
      const std::size_t *primes_end = primes + prime_list_holder<0>::prime_list_size;
      size_type const* bound =
            std::upper_bound(primes, primes_end, n);
      if(bound != primes_end)
            bound--;
      return size_type(*bound);
   }

   private:

   static local_iterator invalid_local_it(const bucket_info_t &b)
   {  return b.buckets_->end();  }

   local_iterator priv_begin(size_type &bucket_num) const
   {
      size_type buckets_len = this->priv_buckets_len();
      for (bucket_num = 0; bucket_num < buckets_len; ++bucket_num){
         bucket_type &b = this->priv_buckets()[bucket_num];
         if(!b.empty())
            return b.begin();
      }
      return invalid_local_it(*this->priv_bucket_info());
   }

   void priv_clear_buckets()
   {  priv_clear_buckets(this->priv_buckets(), this->priv_buckets_len());  }

   static void priv_clear_buckets(bucket_ptr buckets_ptr, size_type buckets_len)
   {
      for(; buckets_len--; ++buckets_ptr){
         buckets_ptr->clear();
      }
   }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   local_iterator priv_find
      ( const KeyType &key,  KeyHasher hasher
      , KeyValueEqual equal, size_type &bucket_number) const
   {
      size_type b_len(this->priv_buckets_len());
      bucket_number = hasher(key) % b_len;

      if(ConstantTimeSize && this->empty()){
         return invalid_local_it(*this->priv_bucket_info());
      }
      
      bucket_type &b = this->priv_buckets()[bucket_number];
      local_iterator it = b.begin();

      while(it != b.end()){
         if(equal(key, *it)){
            return it;
         }
         ++it;
      }

      return invalid_local_it(*this->priv_bucket_info());
   }

   template<class KeyType, class KeyHasher, class KeyValueEqual>
   std::pair<local_iterator, local_iterator> priv_equal_range
      ( const KeyType &key
      , KeyHasher hasher
      , KeyValueEqual equal
      , size_type &bucket_number_first
      , size_type &bucket_number_second
      , size_type &count) const
   {
      count = 0;
      //Let's see if the element is present
      std::pair<local_iterator, local_iterator> to_return
         ( priv_find(key, hasher, equal, bucket_number_first)
         , invalid_local_it(*this->priv_bucket_info()));
      if(to_return.first == to_return.second){
         bucket_number_second = bucket_number_first;
         return to_return;
      }
      ++count;
      //If it's present, find the first that it's not equal in
      //the same bucket
      bucket_type &b = this->priv_buckets()[bucket_number_first];
      local_iterator it = to_return.first;
      ++it;

      while(it != b.end()){
         if(!equal(key, *it)){
            to_return.second = it;
            bucket_number_second = bucket_number_first;
            return to_return;
         }
         ++it;
         ++count;
      }
   
      //If we reached the end, find the first, non-empty bucket
      for(bucket_number_second = bucket_number_first+1
         ; bucket_number_second != this->priv_buckets_len()
         ; ++bucket_number_second){
         bucket_type &b = this->priv_buckets()[bucket_number_second];
         if(!b.empty()){
            to_return.second = b.begin();
            return to_return;
         }
      }

      //Otherwise, return the end node
      to_return.second = invalid_local_it(*this->priv_bucket_info());
      return to_return;
   }
};

} //namespace detail
} //namespace intrusive 
} //namespace boost 

#include "config_end.hpp"

#endif //BOOST_INTRUSIVE_IHASHTABLE_HPP
