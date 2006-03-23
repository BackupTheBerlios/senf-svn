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

// This header combines all managed pointer classes into a single
// namespace. By default, the implementations are just imported with
// using declarations. In the future, a debug version can be
// implemented with extendet debug checking (dereferencing of 0
// pointers etc)
//
// The slave_ptr is a wrapper which behaves like an ordinary pointer
// with no additional semantics (only some restrictions like missing
// arithmetic operators). A slave_ptr is used primarily to clarify the
// type of pointer in the code. In debug builds, the slave_ptr may be
// replaced by a weak pointer and so automatically assert() if a wild
// slave_ptr is dereferenced.

///////////////////////////////////////////////////////////////////////////

#ifndef HH__managed_pointer_
#define HH__managed_pointer_ 1

// Custom includes
#include <memory>
#include <ostream>
#include <boost/scoped_ptr.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/intrusive_ptr.hpp>

///////////////////////////////hh.p////////////////////////////////////////

namespace satcom {
namespace managed_pointer {

using std::auto_ptr;
using boost::scoped_ptr;
using boost::shared_ptr;
using boost::intrusive_ptr;

///////////////////////////////////////////////////////////////////////////
// slave_ptr

template <typename T>
class slave_ptr
{
public:
    typedef T element_type;
    typedef T value_type;
    typedef T* pointer;
    typedef T& reference;
    
    // default default constructor
    // default copy constructor
    // default copy assignment
    // default destructor
    
    // here to define
    slave_ptr();
    template <typename U> explicit slave_ptr (U* p);
    template <typename U> slave_ptr (slave_ptr<U> const & other);
    
    reference operator*() const;
    pointer operator->() const;
    pointer get() const;
    
    operator bool() const;
    bool operator !() const;
    
private:
    pointer p_;
};

template <typename T, typename U> slave_ptr<T> static_pointer_cast (slave_ptr<U> const & p);
template <typename T, typename U> slave_ptr<T> const_pointer_cast (slave_ptr<U> const & p);
template <typename T, typename U> slave_ptr<T> dynamic_pointer_cast (slave_ptr<U> const & p);
template <typename T> typename slave_ptr<T>::pointer get_pointer (slave_ptr<T> const & p);

template <typename E, typename T, typename U> 
std::basic_ostream<E,T>& operator<<(std::basic_ostream<E,T>& os, slave_ptr<U> const & p);

///////////////////////////////////////////////////////////////////////////
// intrusive_refcount

class intrusive_refcount
{
public:
    // protected default constructor
    // protected copy constructor
    // my copy assignment
    // default destructor
    intrusive_refcount& operator=(const intrusive_refcount& other__);

    unsigned refcount() const;

protected:
    intrusive_refcount();
    intrusive_refcount(const intrusive_refcount& other__);

private:
    mutable unsigned refcount_;

public:
    void inc_ref() const;
    bool dec_ref() const;
};

// found only (!) via ADL
template <class T> void intrusive_ptr_add_ref (T* p);
template <class T> void intrusive_ptr_release (T* p);

}
}

///////////////////////////////hh.e////////////////////////////////////////
#include "managed_pointer.cci"
//#include "managed_pointer.ct"
#include "managed_pointer.cti"
#endif


// Local Variables:
// c-file-style: "satcom"
// mode: c++
// End:
