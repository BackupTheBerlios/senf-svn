// $Id$
//
// Copyright (C) 2007
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
    \brief FileTarget public header */

#ifndef HH_FileTarget_
#define HH_FileTarget_ 1

// Custom includes
#include <boost/utility.hpp>
#include <fstream>
#include "IOStreamTarget.hh"

//#include "FileTarget.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace log {

    /** \brief Log target writing to a %log file.

        The FileTarget will save all %log messages in the given file. Messages will be appended at
        the end of the file.

        After %log files have been rotated, the reopen() member should be called to create a new %log
        file.

        \ingroup targets
      */
    class FileTarget 
        : private boost::base_from_member<std::ofstream>,
          public IOStreamTarget
    {
        typedef boost::base_from_member<std::ofstream> ofstream_t;

    public:
        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        explicit FileTarget(std::string file); ///< Construct FileTarget writing to \a file

        ///@}
        ///////////////////////////////////////////////////////////////////////////

        void reopen();                  ///< Reopen %log after log-file rotation
        void reopen(std::string file);  ///< Reopen %log under a new name

    private:
        std::string file_;
    };

}}

///////////////////////////////hh.e////////////////////////////////////////
//#include "FileTarget.cci"
//#include "FileTarget.ct"
//#include "FileTarget.cti"
#endif


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
