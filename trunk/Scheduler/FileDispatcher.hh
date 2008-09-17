// $Id$
//
// Copyright (C) 2008 
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
    \brief FileDispatcher public header */

#ifndef HH_FileDispatcher_
#define HH_FileDispatcher_ 1

// Custom includes
#include <map>
#include "FdManager.hh"
#include "FIFORunner.hh"
#include "FdDispatcher.hh"

//#include "FileDispatcher.mpp"
///////////////////////////////hh.p////////////////////////////////////////

namespace senf {
namespace scheduler {

    /** \brief Scheduler dispatcher managing disc files

        This dispatcher manages file descriptors which are connected to disc files. Since disc files
        do not support select() / poll() / epoll(), they are considered to be always ready (which is
        especially untrue for remote files e.g. vie NFS).

        The FileDispatcher will change the FdManager's event timeout value to 0 (from -1) whenever
        there is at least one file registered.
      */
    class FileDispatcher
    {
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::function<void (int)> Callback;

        enum Events { 
            EV_READ = FdManager::EV_READ, EV_WRITE = FdManager::EV_WRITE,
            EV_HUP = FdManager::EV_HUP, EV_ERR = FdManager::EV_ERR,
            EV_ALL = FdManager::EV_READ | FdManager::EV_WRITE
        };

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///@{

        FileDispatcher(FdManager & manager, FIFORunner & runner);
        ~FileDispatcher();

        ///@}
        ///////////////////////////////////////////////////////////////////////////
        
        void add(std::string const & name, int fd, Callback const & cb, int events = EV_ALL);
                                        ///< Add file descriptor callback
                                        /**< There is always one active callback for each
                                             combination of file descriptor and event. Registering a
                                             new callback will overwrite the old callback.
                                             \param[in] name descriptive name
                                             \param[in] fd file descriptor
                                             \param[in] cb callback
                                             \param[in] events Events to call \a cb for */

        void remove(int fd, int events = EV_ALL);
                                        /**< \param[in] fd file descriptor
                                             \param[in] events Events for which to remove the
                                                 callback */

        void prepareRun();              ///< Prepare tasks
                                        /**< This must be called after the FdManager returns before
                                             running the runnable tasks. */

        void timeout(int t);            ///< Change FdManager timeout
                                        /**< Since the FileDispatcher must be able to change the
                                             timeout value, the value must be set here and not
                                             directly in the FdManager. */
        int timeout() const;            ///< Retrieve current timeout value

        bool empty() const;             ///< \c true, if no files are registered.

    protected:

    private:
        /// Internal: Disk file event
        struct FileEvent
            : public detail::FdTask<0, FileEvent>,
              public detail::FdTask<1, FileEvent>
        {
            typedef detail::FdTask<0, FileEvent> ReadTask;
            typedef detail::FdTask<1, FileEvent> WriteTask;

            explicit FileEvent(std::string const & name)
                : ReadTask (name), WriteTask (name) {}

            int activeEvents() const;
            int events;
        };

        typedef std::map<int, FileEvent> FileMap;

        FileMap files_;
        FdManager & manager_;
        FIFORunner & runner_;
        int managerTimeout_;
    };


}}

///////////////////////////////hh.e////////////////////////////////////////
#include "FileDispatcher.cci"
//#include "FileDispatcher.ct"
//#include "FileDispatcher.cti"
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
