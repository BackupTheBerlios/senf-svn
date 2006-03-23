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

///////////////////////////////////////////////////////////////////////////

// A deque_list is a container allowing efficient random access
// iterators but also allows for efficient front, back and middle
// insertions. Copying and slicing a deque_list is relatively cheap
// and uses COW semantics.

// TODO: Optimize implementation removing one indirection level
//       i.e. make block hold the data block directly not via a
//       pointer.  How can we than use pools wihtout making the
//       interface to complicated?

///////////////////////////////////////////////////////////////////////////

#ifndef HH__deque_list_
#define HH__deque_list_ 1

// Custom includes
#include <list>

///////////////////////////////hh.p////////////////////////////////////////

#include "deque_list_impl.hh"

namespace satcom {
namespace containers {

template <typename T, typename Alloc = deque_list_impl::array_allocator<T> >
class deque_list
{
public:
    typedef T value_type;
    typedef T element_type;
    typedef T* pointer;
    typedef T& reference;
    typedef T const & const_reference;
    typedef typename Alloc::size_type size_type;
    typedef int difference_type;
    typedef Alloc allocator;

    typedef deque_list_impl::iterator<T,Alloc> iterator;
    typedef deque_list_impl::inserter<T,Alloc> inserter;
    
    // my default constructor
    // default copy constructor
    // default copy assignment
    // default destructor

    deque_list();

    iterator begin();
    iterator end();

    // Insert data *before* p
    inserter inserter_at (iterator p);

protected:

public: // Hrmpf ... either gcc gets this wrong or I am stupid ...

    typedef deque_list_impl::block<T,Alloc> block;
    typedef typename block::ptr block_ptr;
    typedef deque_list_impl::block_ref<T,Alloc> block_ref;

    typedef std::list<block_ref> block_list;
    typedef typename block_list::iterator block_list_iterator;
    typedef typename block_list::const_iterator const_block_list_iterator;

private:
    void insert_block_before (block_ptr block, pointer begin, pointer end,
                              block_list_iterator place);
    void split_block (block_list_iterator block, pointer place);
    
    block_list blocks_;

    friend class deque_list_impl::iterator<T,Alloc>;
    friend class deque_list_impl::inserter<T,Alloc>;
    friend class deque_list_impl::inserter<T,Alloc>::cache;

};

}
}

///////////////////////////////hh.e////////////////////////////////////////
//#include "deque_list.cci"
#include "deque_list.ct"
#include "deque_list.cti"
#endif


// Local Variables:
// mode: c++
// c-file-style: "satcom"
// End:
