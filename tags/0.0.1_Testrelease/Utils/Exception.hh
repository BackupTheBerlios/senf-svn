// $Id$
//
// Copyright (C) 2006
// Fraunhofer Institute for Open Communication Systems (FOKUS)
// Competence Center NETwork research (NET), St. Augustin, GERMANY
//     Stefan Bund <g0dil@berlios.de>
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

/** \file
    \brief Exception public header */

#ifndef HH_Exception_
#define HH_Exception_ 1

// Custom includes
#include <exception>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/preprocessor/repeat.hpp>
#include <boost/preprocessor/cat.hpp>
#include <boost/utility.hpp>

//#include "Exception.mpp"
///////////////////////////////hh.p////////////////////////////////////////

/** \defgroup exception Exception classes

    All exceptions in %senf are derived from senf::Exception. This class adds the possibility to
    extend the exception description while it is processed:

    \code
    try {

        // Some code which might raise an arbitrary senf exception

    }
    catch (senf::Exception & e) {
        e << "\handling user " << user;
        throw;
    }
    \endcode

    This will add the user information to any %senf exception thrown. The Exception is however not a
    stream. If you need to do more extensive formating, either use an intermediate string-stream or
    use <a href="http://www.boost.org/libs/format/doc/format.html">Boost.Format</a>:

    \code
    try { 
        // ...
    }
    catch (senf::Exception & e) {
        e << boost::format("\ncall id 0x%04x@%s") % id % address;
    }
    \endcode

    senf::SystemException is thrown for all operating system errors (failures which result in the
    operating system setting the errno value). It is also derived from senf::Exception and can
    therefore be extended as well.

    Defining your own exception classes derived from senf::Exception is very simple:

    \code
    struct FooException : public senf::Exception
    { FooException() : senf::Exception("Foo hit the fan") {} };
    \endcode
 */

namespace senf {

    /** \brief Generic exception base-class

        Exception is a generic exception base-class which allows the exception to be later extended
        by catching and re-throwing it (See example in \ref exception).

        \ingroup exception
      */
    class Exception
        : public std::exception
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        virtual ~Exception() throw();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        virtual char const * what() const throw();

        template <class Arg>
        Exception & operator<<(Arg const & arg); ///< Extend exception description
                                        /**< Adds \a arg converted to string to the end of the
                                             exception description string. This operator allows to
                                             use Exception instances like streams. The conversion is
                                             performed using <code>boost::lexical_cast</code> and is
                                             therefor identical to a streaming operation. 
                                             \see \ref exception */

    protected:
        Exception(std::string const & description = ""); ///< Initialize exception with string
                                        /**< \a description is the initial error description
                                             string. This should probably be a string constant
                                             describing the exception for most derived
                                             exceptions. */

    private:
        std::string message_;
    };


    /** \brief Exception handling standard UNIX errors (errno)

        This exception is thrown to signal generic \c errno failures. Normally the \c errno value is
        automatically taken from the \c errno variable but it may also be specified explicitly:

        \code
        // Standard usage: Take \c errno from environment
        throw senf::SystemException("::open()") 
            << " while opening configuration file: " << filename;

        // You may however explicitly specify the errno value
        throw senf::SystemException("::open()", ENOFILE)

        // Or leave the location information empty
        throw senf::SystemException(ENOFILE);
        throw senf::SystemException();
        \endcode

        \ingroup exception
     */
    class SystemException : public Exception
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit SystemException(std::string const & where = ""); 
        explicit SystemException(int code);
        SystemException(std::string const & where, int code);

        virtual ~SystemException() throw();

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        int errorNumber() const;        ///< Error code (\c errno number)
        char const * description() const; ///< Error description (\c strerror() value)

        bool anyOf(int c0, int c1=0, int c2=0, int c3=0, int c4=0, int c5=0, 
                   int c6=0, int c7=0, int c8=0, int c9=0);
                                        ///< \c true, if errorNumber() is one of \a c0 ... \a c9



    private:
        void init(std::string const & where, int code);
        
        int code_;
    };

}

///////////////////////////////hh.e////////////////////////////////////////
#include "Exception.cci"
//#include "Exception.ct"
#include "Exception.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// comment-column: 40
// End:
