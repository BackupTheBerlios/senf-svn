// $Id$
//
// Copyright (C) 2005 
// Fraunhofer Institut fuer offene Kommunikationssysteme (FOKUS)
// Kompetenzzentrum fuer Satelitenkommunikation (SatCom)
//     Stefan Bund <stefan.bund@fokus.fraunhofer.de>
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

// Architecture:
//
// - deque_list manages a std::list of block_ref's.
// - each block_ref references an arbitrary segment of a block
// - each block holds a unique memory reagin.
//
// block's may be shared
// block_ref's are held by value in the deque_list's std::list

#ifndef HH__deque_list_impl_
#define HH__deque_list_impl_ 1

// Custom includes
#include <boost/iterator/iterator_facade.hpp>
#include "managed_pointer.hh"

///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace containers {

namespace mptr = satcom::managed_pointer;
template <class T, class Alloc> class deque_list;

namespace deque_list_impl {

///////////////////////////////////////////////////////////////////////////
// array_allocator<T>

template <typename T>
struct array_allocator 
{
    typedef T value_type;
    typedef T element_type;
    typedef T* block_p;
    typedef unsigned size_type;
    
    static std::pair<block_p,size_type> allocate (size_type size);
    static void deallocate (block_p block, size_type size);
};

///////////////////////////////////////////////////////////////////////////
// block<T,Alloc>

// T requirements:
//   T must be a value type (copyable, default constructible,
//   assignable and should have cheap copy semantics). T instances are
//   always passed by value.
//
// Alloc requirements:
//   See 'array_allocator' for required elements.
//   Alloc must have two static member functions 'allocate' and
//   'deallocate' which are called to create and destroy new
//   blocks. 
//
//   The 'allocate' call is passed the size of the block requested. It
//   must return an std::pair. The first element is of type
//   Alloc::block_p and holds the allocated data block the second
//   element of the pair gives the actual size of the block which may
//   be greater than the requested size. Alloc::block_p is passed
//   by value and must therefore support copy-construction and
//   assignment however the block class ensures, that no two copies of
//   a block_p instance persist.
//
//   The 'deallocate' call is passed the Alloc::block_p object and
//   the blocksize. The 'deallocate' call must not throw.
//
//   The Alloc::block_p object must support operator[] which must
//   return a reference to an object of type T. Furthermore it must be
//   valid to take the address of such object and use this address as
//   an iterator over the allocated block. With other words, the
//   return value must be organized as a single C array which may be
//   wrapped in some manner.

template <typename T, typename Alloc = array_allocator<T> >
class block : public mptr::intrusive_refcount
{
public:
    typedef T value_type;
    typedef T element_type;
    typedef T* pointer;
    typedef T& reference;
    typedef T const & const_reference;
    typedef typename Alloc::size_type size_type;
    typedef Alloc allocator;
    typedef mptr::intrusive_ptr<block> ptr;
    
    // no default constructor
    // my copy constructor
    // disabled copy assignment
    // my destructor
    ~block();

    static ptr create (size_type size);
    ptr clone();

    reference operator[](size_type index) const;
    size_type size() const;

    pointer begin() const;
    pointer end() const;

    bool unique() const;
    
private:
    explicit block(size_type size__);
    block(const block& other__);
    // here to disable
    block& operator=(const block& other__);

    typedef typename allocator::block_p data_t;

    data_t data_;
    size_type size_;
};

///////////////////////////////////////////////////////////////////////////
// block_ref<T,Alloc>

// block_ref references an arbitrary subsegment of a block. It is used
// as value in the deque_lists's std::list
//
// A block_ref may be 'special'. special block_ref's are used to mark
// the beginning/end of the std::list and additionally contain a
// pointer to the container for extended pre-condition checks.

template <class T, class Alloc>
class block_ref
{
public:
    typedef deque_list<T,Alloc> container;
    typedef typename container::block_ptr block_ptr;
    typedef typename container::pointer pointer;
    
    // my default constructor
    // default copy constructor
    // default copy assignment
    // default destructor

    block_ref();
    block_ref(block_ptr block__, pointer begin__, pointer end__);
    explicit block_ref (container* list__);

    bool is_special();
    container* deque_list();

    block_ptr block();
    pointer begin();
    pointer end();

    void set_begin (pointer begin__);
    void set_end (pointer end__);

private:
    block_ptr block_;
    pointer begin_;
    pointer end_;
};

///////////////////////////////////////////////////////////////////////////
// iterator<T,Alloc>

// TODO: make into a (validating) random access iterator
template <class T, class Alloc>
class iterator 
    : public boost::iterator_facade<iterator<T,Alloc>, T, 
                                    std::bidirectional_iterator_tag>
{
public:
    typedef deque_list<T,Alloc> container;
    typedef typename container::pointer pointer;
    typedef typename container::reference reference;    
    typedef typename container::block::size_type size_type;
    
    // default default constructor
    // default copy constructor
    // default copy assignment
    // default destructor

    // here to implement
    iterator();

protected:

private:
    friend class boost::iterator_core_access;
    friend class container;

    typedef typename container::block block;
    typedef typename container::block_list block_list;
    typedef typename block_list::iterator block_list_iterator;

    iterator (block_list_iterator block, pointer index);

    // iterator_facade core functions
    void increment();
    void decrement();
    reference dereference() const;
    bool equal (const iterator& other__) const;

    block_list_iterator block_;
    pointer p_;
};

///////////////////////////////////////////////////////////////////////////
// inserter<T,Alloc>

// Insert data *before* given position

template <class T, class Alloc>
class inserter
    : public boost::iterator_facade<inserter<T,Alloc>, T, 
                                    std::output_iterator_tag>
{
public:
    typedef deque_list<T,Alloc> container;
    typedef T value_type;
    typedef T& reference;
    typedef typename container::size_type size_type;
    typedef typename container::block::pointer pointer;

    // no default constructor
    // default copy constructor
    // default copy assignment
    // default destructor

    void flush();
    void allocate (size_type size);

    // Hmm... this interface should probably be private and be using
    // friends
    size_type size();
    size_type max_size();
    pointer block_begin();
    pointer block_end();

    const static size_type default_size = 256;

protected:

private:
    typedef typename container::block block;
    typedef typename block::ptr block_ptr;
    typedef typename container::size_type size_type;
    typedef typename container::block_list::iterator block_list_iterator;

    struct cache : public mptr::intrusive_refcount
    {
        typedef mptr::intrusive_ptr<cache> ptr;

        // no default constructor
        // default copy constructor
        // default copy assignment
        // my destructor

        cache(size_type size, block_list_iterator place, container& dl);
        ~cache();
        static ptr create(size_type size, block_list_iterator place, container& dl);

        container& container_; 
        block_list_iterator place_;
        block_ptr block_;
        size_type size_;
    };

    typedef typename cache::ptr cache_ptr;

    inserter(block_list_iterator place, container& dl);

    // iterator_facade core functions
    void increment();
    reference dereference() const;

    container* container_; // WARNING: beware lifetime issues !!
    block_list_iterator place_;    
    mutable cache_ptr cache_;

    friend class boost::iterator_core_access;
    friend class container;
};

}
}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "deque_list_impl.cci"
#include "deque_list_impl.ct"
#include "deque_list_impl.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
