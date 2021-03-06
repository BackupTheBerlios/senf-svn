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
    \brief Executor non-inline non-template implementation */

#include "Executor.hh"
//#include "Executor.ih"

// Custom includes
#include <boost/utility.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/bind.hpp>
#include "../Utils/senfassert.hh"
#include "../Utils/Range.hh"
#include "../Utils/String.hh"

//#include "Executor.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace {

    struct TraverseTokens {
        typedef std::string const & result_type;
        result_type operator()(senf::console::Token const & token) const {
            return token.value();
        }
    };
    
}

///////////////////////////////////////////////////////////////////////////
// senf::console::Executor

prefix_ senf::console::DirectoryNode & senf::console::Executor::cwd()
    const
{
    SENF_ASSERT( ! cwd_.empty() );
    while (cwd_.size()>1 && (cwd_.back().expired() || ! cwd_.back().lock()->active()))
        cwd_.pop_back();
    return * cwd_.back().lock();
}

prefix_ std::string senf::console::Executor::cwdPath()
    const
{
    if (skipping())
        return "";
    return "/" + senf::stringJoin(
        senf::make_transform_range(
            boost::make_iterator_range(boost::next(cwd_.begin()), cwd_.end()),
            boost::bind(&DirectoryNode::name, boost::bind(&DirectoryNode::weak_ptr::lock, _1))),
        "/" );
}

prefix_ void senf::console::Executor::execute(std::ostream & output,
                                              ParseCommandInfo const & command)
{
    SENF_LOG(( "Executing: " << command ));

    if (! skipping())
        (void) cwd(); // Prune the cwd path of expired entries

    try {
        switch(command.builtin()) {
        case ParseCommandInfo::NoBuiltin : 
            if (skipping())
                return;
            exec(output, command);
            break;

        case ParseCommandInfo::BuiltinCD :
            if (skipping())
                break;
            try {
                // The parser ensures, we have exactly one argument
                cd(*command.arguments().begin());
            }
            catch (IgnoreCommandException &) {
                throw SyntaxErrorException(
                    "'cd' cannot be skipped (don't use 'cd' in conf-files)");
            }
            break;
            
        case ParseCommandInfo::BuiltinLS :
            if (skipping())
                break;
            // The parser ensures, we have either one or no argument
            ls( output,
                command.tokens().empty() ? command.tokens() : *command.arguments().begin() );
            break;

        case ParseCommandInfo::BuiltinPUSHD :
            // The parser ensures, we have exactly one argument
            pushd( *command.arguments().begin() );
            break;
            
        case ParseCommandInfo::BuiltinPOPD :
            // The parser ensures, we have no arguments
            popd();
            break;
            
        case ParseCommandInfo::BuiltinEXIT :
            if (skipping())
                break;
            // The parser ensures, we have no arguments
            exit();
            break;

        case ParseCommandInfo::BuiltinHELP :
            if (skipping())
                break;
            // The parser ensures, we have either one or no arguments
            help( output,
                  command.tokens().empty() ? command.tokens() : *command.arguments().begin() );
            break;

        }
    }
    catch (InvalidPathException &) {
        throw SyntaxErrorException("invalid path");
    }
    catch (InvalidDirectoryException &) {
        throw SyntaxErrorException("invalid directory");
    }
    catch (InvalidCommandException &) {
        throw SyntaxErrorException("invalid command");
    }
    catch (IgnoreCommandException &) {}
}

prefix_ void senf::console::Executor::exec(std::ostream & output,
                                           ParseCommandInfo const & command)
{
    GenericNode & node ( traverseNode(command.commandPath()) );
    DirectoryNode * dir ( dynamic_cast<DirectoryNode*>(&node) );
    if ( dir ) {
        if (autocd_ && command.tokens().empty()) {
            cd( boost::make_iterator_range(
                    command.commandPath().begin(),
                    command.commandPath().end()) );
        } else
            throw InvalidCommandException();
    } else {
        dynamic_cast<CommandNode &>(node)(output, command);
    }
}


prefix_ void senf::console::Executor::cd(ParseCommandInfo::TokensRange dir)
{
    if (dir.size() == 1 && *dir.begin() == WordToken("-")) {
        cwd_.swap(oldCwd_);
        (void) cwd(); // Prune any expired items
    }
    else {
        // We need to use a temporary so an error somewhere while traversing the dir does not cause
        // the current directory to change.
        Path newDir (cwd_);
        traverseDirectory(dir, newDir);
        oldCwd_.swap(cwd_);
        cwd_.swap(newDir);
    }
}

prefix_ void senf::console::Executor::ls(std::ostream & output,
                                         ParseCommandInfo::TokensRange path)
{
    Path dir (cwd_);
    traverseDirectory(path, dir);
    DirectoryNode & node (*dir.back().lock());
    DirectoryNode::child_iterator i (node.children().begin());
    DirectoryNode::child_iterator const i_end (node.children().end());
    for (; i != i_end; ++i) {
        output << i->first;
        if (boost::dynamic_pointer_cast<DirectoryNode>(i->second))
            output << "/";
        output << "\n";
    }
}

prefix_ void senf::console::Executor::pushd(ParseCommandInfo::TokensRange dir)
{
    Path newDir (cwd_);
    if (! skipping()) {
        try {
            traverseDirectory(dir, newDir);
        }
        catch (IgnoreCommandException &) {
            newDir.clear();
        }
    }
    dirstack_.push_back(Path());
    dirstack_.back().swap(cwd_);
    cwd_.swap(newDir);
}

prefix_ void senf::console::Executor::popd()
{
    if (! dirstack_.empty()) {
        cwd_.swap(dirstack_.back());
        dirstack_.pop_back();
    }
}

prefix_ void senf::console::Executor::exit()
{
    throw ExitException();
}

prefix_ void senf::console::Executor::help(std::ostream & output,
                                           ParseCommandInfo::TokensRange path)
{
    GenericNode const & node (traverseNode(path));
    output << prettyName(typeid(node)) << " at " << node.path() << "\n\n";
    node.help(output);
    output << std::flush;
}

prefix_ senf::console::GenericNode &
senf::console::Executor::traverseNode(ParseCommandInfo::TokensRange const & path)
{
    if (path.empty())
        return *cwd_.back().lock();
    try {
        Path dir (cwd_);
        traverseDirectory(boost::make_iterator_range(
                              path.begin(),
                              boost::prior(path.end())),
                          dir);
        DirectoryNode & base (*dir.back().lock());
        std::string const & name (complete(base, boost::prior(path.end())->value()));
        if (policy_)
            policy_( base, name );
        return dir.back().lock()->get(name);
    }
    catch (UnknownNodeNameException &) {
        throw InvalidPathException();
    }
}

prefix_ void
senf::console::Executor::traverseDirectory(ParseCommandInfo::TokensRange const & path,
                                           Path & dir)
{
    try {
        ParseCommandInfo::TokensRange::const_iterator i (path.begin());
        ParseCommandInfo::TokensRange::const_iterator const i_end (path.end());
        for (; i != i_end; ++i) {
            if (*i == NoneToken()) {
                if (i == path.begin()) {
                    dir.clear();
                    dir.push_back(root_);
                }
            }
            else if (*i == WordToken("..")) {
                if (dir.size() > 1)
                    dir.pop_back();
            }
            else if (*i ==  WordToken("."))
                ;
            else {
                DirectoryNode & base (*dir.back().lock());
                std::string name (complete(base, i->value()));
                if (policy_)
                    policy_( base, name );
                dir.push_back(base[name].thisptr());
            }
        }
    }
    catch (std::bad_cast &) {
        throw InvalidDirectoryException();
    }
    catch (UnknownNodeNameException &) {
        throw InvalidDirectoryException();
    }
}

prefix_ std::string senf::console::Executor::complete(DirectoryNode & dir,
                                                      std::string const & name)
{
    if (! dir.hasChild(name)) {
        DirectoryNode::ChildrenRange completions (dir.completions(name));
        if (completions.size() == 1)
            return completions.begin()->first;
    }
    return name;
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Executor.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
