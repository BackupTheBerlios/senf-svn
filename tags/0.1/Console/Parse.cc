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
    \brief Parse non-inline non-template implementation */

#include "Parse.hh"
#include "Parse.ih"

// Custom includes
#include <cerrno>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/spirit/iterator/file_iterator.hpp>
#include "../Utils/Exception.hh"

//#include "Parse.mpp"
#define prefix_
///////////////////////////////cc.p////////////////////////////////////////

namespace senf {
namespace console {
namespace detail {

#ifndef DOXYGEN

    struct ParseDispatcher
    {
        ParseCommandInfo * info_;

        struct BindInfo {
            BindInfo( ParseDispatcher & d, ParseCommandInfo & info)
                : dispatcher (d) { dispatcher.info_ = &info; }
            ~BindInfo() { dispatcher.info_ = 0; }

            ParseDispatcher & dispatcher;
        };

        void beginCommand(std::vector<Token> & command)
            { info_->clear();
              info_->command(command); }

        void endCommand()
            { }

        void pushToken(Token const & token)
            { info_->addToken(token); }

        void builtin_cd(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinCD);
              setBuiltinPathArg(path); }

        void builtin_ls(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinLS);
              setBuiltinPathArg(path); }

        void pushDirectory(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinPUSHD);
              setBuiltinPathArg(path); }

        void popDirectory()
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinPOPD); }
        
        void builtin_exit()
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinEXIT); }

        void builtin_help(std::vector<Token> & path)
            { info_->clear();
              info_->builtin(ParseCommandInfo::BuiltinHELP);
              setBuiltinPathArg(path); }

        void setBuiltinPathArg(std::vector<Token> & path)
            {
                pushToken(ArgumentGroupOpenToken());
                for (std::vector<Token>::const_iterator i (path.begin());
                     i != path.end(); ++i)
                    pushToken(*i);
                pushToken(ArgumentGroupCloseToken());
            }
    };

#endif

}}}

///////////////////////////////////////////////////////////////////////////
// senf::console::Token

prefix_ std::ostream & senf::console::operator<<(std::ostream & os, Token const & token)
{
    static char const * tokenTypeName[] = {
        "None",
        "PathSeparator",
        "ArgumentGroupOpen",
        "ArgumentGroupClose",
        "DirectoryGroupOpen",
        "DirectoryGroupClose",
        "CommandTerminator",
        "OtherPunctuation",
        "BasicString",
        "HexString",
        "Word" };
    // The real table is:
    //     static const int bitPosition[32] = {
    //          0,  1, 28,  2, 29, 14, 24,  3, 30, 22, 20, 15, 25, 17,  4,  8, 
    //         31, 27, 13, 23, 21, 19, 16,  7, 26, 12, 18,  6, 11,  5, 10,  9 };
    // However, we have replaced all values >= sizeof(tokenTypeName) with 0
    // and have added 1 to all the remaining values
    static const int bitPosition[32] = {
        1, 2, 0, 3, 0, 0, 0, 4, 0, 0, 0, 0, 0, 0, 5, 9, 
        0, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 7, 0, 6, 0, 10 };
    // We need to check token.type() against 0 explicitly since 0 and 1 will both be mapped to 0
    os << tokenTypeName[ token.type() 
                         ? bitPosition[((token.type() & -token.type()) * 0x077CB531UL) >> 27]
                         : 0 ]
       << "('"
       << token.value()
       << "')";
    return os;
}

///////////////////////////////////////////////////////////////////////////
// senf::console::ParseCommandInfo

prefix_ std::ostream & senf::console::operator<<(std::ostream & stream,
                                                 ParseCommandInfo const & info)
{
    if (info.builtin() == ParseCommandInfo::NoBuiltin) {
        ParseCommandInfo::TokensRange::const_iterator i (info.commandPath().begin());
        ParseCommandInfo::TokensRange::const_iterator const i_end (info.commandPath().end());
        if (i != i_end) {
            for (;;) {
                stream << i->value();
                if ( ++i != i_end ) stream << "/";
                else                break;
            }
        }
    }
    else {
        char const * builtins[] = { 0, "cd", "ls", "pushd", "popd", "exit", "help" };
        stream << "builtin-" << builtins[info.builtin()];
    }
        
    ParseCommandInfo::ArgumentsRange args (info.arguments());
    for (ParseCommandInfo::argument_iterator i (args.begin()); i != args.end(); ++i) {
        ParseCommandInfo::token_iterator j (i->begin());
        stream << " [";
        if ( j != i->end() ) {
            for (;;) {
                stream << "'" << j->value() << "'";
                if ( ++j != i->end() ) stream << ' ';
                else                   break;
            }
        }
        stream << "]";
    }

    return stream;
}

///////////////////////////////////////////////////////////////////////////
// senf::console::ParseCommandInfo::ArgumentIterator

prefix_ void senf::console::ParseCommandInfo::ArgumentIterator::setRange()
    const
{
    if (b_->is(Token::ArgumentGroupOpen)) {
        unsigned level (0);
        e_ = b_;
        for (;;) {
            if (e_->is(Token::ArgumentGroupOpen))
                ++ level;
            else if (e_->is(Token::ArgumentGroupClose)) {
                -- level;
                if (level == 0)
                    break;
            }
            ++e_;
        }
    }
    ++ e_;
}

prefix_ void senf::console::ParseCommandInfo::ArgumentIterator::decrement()
{
    e_ = b_;
    --b_;
    if (b_->is(Token::ArgumentGroupClose)) {
        unsigned level (0);
        for (;;) {
            if (b_->is(Token::ArgumentGroupClose))
                ++ level;
            else if (b_->is(Token::ArgumentGroupOpen)) {
                -- level;
                if (level == 0)
                    break;
            }
            --b_;
        }
    }
}

///////////////////////////////////////////////////////////////////////////
// senf::console::CommandParser

#ifndef DOXYGEN

struct senf::console::CommandParser::Impl
{
    typedef detail::CommandGrammar<detail::ParseDispatcher> Grammar;

    detail::ParseDispatcher dispatcher;
    Grammar::Context context;
    Grammar grammar;

    Impl() : dispatcher(), context(), grammar(dispatcher, context) {}
};

#endif

prefix_ senf::console::CommandParser::CommandParser()
    : impl_ (new Impl())
{}

prefix_ senf::console::CommandParser::~CommandParser()
{}

// This template member is placed here, since it is ONLY called from the implementation.  Otherwise,
// we would need to expose the Impl member to the public, which we don't want to do.

template <class Iterator>
prefix_ Iterator senf::console::CommandParser::parseLoop(Iterator b, Iterator e, Callback cb)
{
    ParseCommandInfo info;
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, info);
    boost::spirit::parse_info<Iterator> result;

    for(;;) {
        result = boost::spirit::parse(
            b, e, * impl().grammar.use_parser<Impl::Grammar::SkipParser>());
        b = result.stop;
        if (b == e) 
            return e;
        info.clear();
        result = boost::spirit::parse(b, e,
                                      impl().grammar.use_parser<Impl::Grammar::CommandParser>(),
                                      impl().grammar.use_parser<Impl::Grammar::SkipParser>());
        if (! result.hit) 
            return b;
        if (! info.empty()) 
            cb(info);
        b = result.stop;
    }
}

prefix_ bool senf::console::CommandParser::parse(std::string const & command, Callback cb)
{
    return parseLoop(command.begin(), command.end(), cb) == command.end();
}

prefix_ bool senf::console::CommandParser::parseFile(std::string const & filename, Callback cb)
{
    boost::spirit::file_iterator<> i (filename);
    if (!i) throw SystemException(ENOENT SENF_EXC_DEBUGINFO);
    boost::spirit::file_iterator<> const i_end (i.make_end());
    
    return parseLoop(i, i_end, cb) == i_end;
}

prefix_ bool senf::console::CommandParser::parseArguments(std::string const & arguments,
                                                          ParseCommandInfo & info)
{
    detail::ParseDispatcher::BindInfo bind (impl().dispatcher, info);
    return boost::spirit::parse( arguments.begin(), arguments.end(), 
                                 impl().grammar.use_parser<Impl::Grammar::ArgumentsParser>(),
                                 impl().grammar.use_parser<Impl::Grammar::SkipParser>()
        ).full;
}

struct senf::console::CommandParser::SetIncremental
{
    SetIncremental(CommandParser & parser) : parser_ (parser) {
        parser_.impl().grammar.incremental = true;
    }

    ~SetIncremental() {
        parser_.impl().grammar.incremental = false;
    }

    CommandParser & parser_;
};

prefix_ std::string::size_type
senf::console::CommandParser::parseIncremental(std::string const & commands, Callback cb)
{
    SetIncremental si (*this);
    return std::distance( commands.begin(), 
                          parseLoop(commands.begin(), commands.end(), cb) );
}

///////////////////////////////////////////////////////////////////////////
// senf::console::SyntaxErrorException

prefix_ char const * senf::console::SyntaxErrorException::what()
    const throw()
{
    return message().empty() ? "syntax error" : message().c_str();
}

///////////////////////////////cc.e////////////////////////////////////////
#undef prefix_
//#include "Parse.mpp"


// Local Variables:
// mode: c++
// fill-column: 100
// comment-column: 40
// c-file-style: "senf"
// indent-tabs-mode: nil
// ispell-local-dictionary: "american"
// compile-command: "scons -u test"
// End:
