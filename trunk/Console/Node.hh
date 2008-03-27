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
    \brief Node public header */

#ifndef HH_Node_
#define HH_Node_ 1

// Custom includes
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/utility.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/typeof/typeof.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include "../Utils/Exception.hh"
#include "../Utils/mpl.hh"
#include "../Utils/Logger/SenfLog.hh"
#include "Parse.hh"

//#include "Node.mpp"
///////////////////////////////hh.p////////////////////////////////////////

#include BOOST_TYPEOF_INCREMENT_REGISTRATION_GROUP()

namespace senf {
namespace console {

    class DirectoryNode;
    class CommandNode;

    /** \brief Config/console node tree base-class

        GenericNode is the base class of all node objects. There are two basic node types derived
        from GenericNode:  DirectoryNode and CommandNode.

        All nodes are dynamically allocated and automatically managed using reference counting.

        All nodes are normally linked into a single tree which root node is
        senf::console::root(). Nodes may however be orphaned (not linked to the tree) either
        directly (the node has no parent) or indirectly (the node has a parent but is part of an
        orphaned subtree which is not linked to the root node).

        Every active (non-orphaned) node (except the root() node) has a non-empty node name. This
        name is assigned to the node when adding the node to the tree.
      */
    class GenericNode 
        : public boost::enable_shared_from_this<GenericNode>
    {
        SENF_LOG_CLASS_AREA();
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<GenericNode> ptr;
        typedef boost::shared_ptr<GenericNode const> cptr;
        typedef boost::weak_ptr<GenericNode> weak_ptr;

        ///////////////////////////////////////////////////////////////////////////

        virtual ~GenericNode();

        std::string const & name() const; ///< Node name
        boost::shared_ptr<DirectoryNode> parent() const; ///< Parent node
                                        /**< May be null, if the node is the root node or if it is
                                             not linked to the tree */

        std::string path() const;       ///< Node path
                                        /**< The node path is built by joining the names of all
                                             parent nodes with '/' chars. */

        ptr unlink();                   ///< Remove node from it's parent directory
                                        /**< You may either discard the return value and thereby
                                             dispose the node or may re-attach the node at some
                                             other place using DirectoryNode::add(). */

        bool active() const;            ///< \c true, if node is attached to the root() node

        void help(std::ostream & output) const; /// Write help info to \a output

        ptr thisptr();                  ///< Get smart pointer to node
        cptr thisptr() const;           ///< Get smart pointer to node (const)

    protected:
        GenericNode();

        void name(std::string const & name);

#ifndef DOXYGEN
    private:
#else
    public:
#endif
        virtual void v_help(std::ostream & output) const = 0;
                                        ///< Provide help information
                                        /**< This member must be implemented in derived classes
                                             to provide node specific help information. */

    private:
        std::string name_;
        DirectoryNode * parent_;

        friend class DirectoryNode;
    };

    class SimpleCommandNode;

    /** \brief Internal: Node creation helper traits
        
        This class is used internally to find out the type of node to create for a specific argument
        type. 
     */
    template <class Object>
    struct NodeCreateTraits
    {
        typedef BOOST_TYPEOF_TPL( senf_console_add_node( 
                                      * static_cast<DirectoryNode *>(0),
                                      * static_cast<std::string const *>(0),
                                      * static_cast<Object const *>(0),
                                      0) ) result_type;

        typedef typename boost::remove_reference<result_type>::type NodeType;

        /// Internal
        struct Creator {
            static NodeType & create(DirectoryNode & node, std::string const & name, 
                                     Object const & ob);
        };
    };

    /** \brief Config/console tree directory node

        This node type provides the internal and root nodes of the tree. It allows to add arbitrary
        children and supports directory traversal.
        
        Nodes are normally not instantiated manually but are created by the DirectoryNode via
        mkdir() or add(). Special add() members however allow externally allocated node objects.

        Nodes may be added to the tree only once, otherwise chaos will ensue. Since nodes are always
        managed dynamically, there is a special ObjectDirectory proxy template which provides a
        DirectoryNode facade. ObjectDirectory is used if a class wants to manage it's own directory
        as a data member.

        Every node is assigned a (new) name when it is added to a directory. If the directory
        already has an entry of that name, the name is made unique by appending a suffix of the form
        '-n' where n is a number starting at 1. If the name is empty, int is set to 'unnamed' and
        then uniquified as above. Automatically providing unique names simplifies adding
        configuration/console support to generic components.
      */
    class DirectoryNode : public GenericNode
    {
        SENF_LOG_CLASS_AREA();
        typedef std::map<std::string, GenericNode::ptr> ChildMap;
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<DirectoryNode> ptr;
        typedef boost::shared_ptr<DirectoryNode const> cptr;
        typedef boost::weak_ptr<DirectoryNode> weak_ptr;

        typedef boost::iterator_range<ChildMap::const_iterator> ChildrenRange;
        typedef ChildMap::const_iterator child_iterator;

        ///////////////////////////////////////////////////////////////////////////
        ///\name Structors and default members
        ///\{

        static ptr create();            ///< Create node object.
                                        /**< You should normally use either mkdir() or
                                             ObjectDirectory instead of create() */

        ///\}
        ///////////////////////////////////////////////////////////////////////////
        ///\name Children
        ///\{

        template <class NodeType>
        NodeType & add(std::string const & name, boost::shared_ptr<NodeType> node);
                                        ///< Add node to tree
                                        /**< Adds the \a node to the tree as a child of \a this
                                             node. The node is given the name \a name. If a node of
                                             that name already exists, a numeric suffix of the form
                                             '-n' is added to the name until the name is unique. If
                                             \a name is empty, it is set to 'unnamed'. */

        template <class Object>
        typename NodeCreateTraits<Object>::NodeType & add (std::string const & name, 
                                                           Object const & ob);
                                        ///< Generic child node factory
                                        /**< This member is used to create a new child node of the
                                             current directory. The type of node created depends on
                                             the type of argument passed.

                                             The node type is selected by the NodeCreateTraits
                                             class. To allow adding a specific node type, you need
                                             to provide an overload for
                                             <tt>senf_console_add_node</tt> which must be visible at
                                             when you register the new node.
                                             \code
                                             MyNodeType & senf_console_add_node(
                                                 DirectoryNode & dir,
                                                 std::string const & name,
                                                 MySpecialObject const & ob,
                                                 int)
                                             {
                                                 return dir.add(name, MyNodeType::create(ob));
                                             }
                                             \endcode
                                             (Do not forget the last unnamed 'int' parameter which
                                             is not used but serves to disambiguate the
                                             overloads). */

        GenericNode::ptr remove(std::string const & name);
                                        ///< Remove node \a name from the tree
                                        /**< The returned pointer may either be discarded, which
                                             will automatically dispose the removed node, or it may
                                             be saved and/or re-attached at some other place in the
                                             tree. */

        DirectoryNode & operator[](std::string const & name) const;
                                        ///< Get directory child node
                                        /**< \throws UnknownNodeNameException if a child \a name
                                                 does not exist. 
                                             \throws std::bad_cast if the child \a name is not a
                                                 directory node. */

        CommandNode & operator()(std::string const & name) const;
                                        ///< Get command child node
                                        /**< \throws UnknownNodeNameException if a child \a name
                                                 does not exist
                                             \throws std::bad_cast if the child \a name is not a
                                                 command node. */

        GenericNode & get(std::string const & name) const;
                                        ///< Get child node
                                        /**< \throws UnknownNodeNameException if a child \a name
                                                 does not exist */

        DirectoryNode & mkdir(std::string const & name);
                                        ///< Create sub-directory node
        
        ChildrenRange children() const;
                                        ///< Return iterator range over all children.
                                        /**< The returned range is sorted by child name. */

        ///\}
        ///////////////////////////////////////////////////////////////////////////

        template <class ForwardRange>
        GenericNode & traverse(ForwardRange const & range);
                                        ///< Traverse node path starting at this node
                                        /**< The <tt>FordwareRange::value_type</tt> must be
                                             (convertible to) std::string. Each range element
                                             constitutes a step along the node traversal.

                                             If the range starts with an empty element, the
                                             traversal is started at the root() node, otherwise it
                                             is started at \a this node. The traversal supports '.',
                                             '..' and ignores further empty elements. */

        DirectoryNode & doc(std::string const & doc);
                                        ///< Set node documentation

        ptr thisptr();
        cptr thisptr() const;

    protected:
        DirectoryNode();

    private:
        void add(GenericNode::ptr node);
        virtual void v_help(std::ostream & output) const;

        ChildMap children_;
        std::string doc_;

        friend DirectoryNode & root();
    };

    BOOST_TYPEOF_REGISTER_TYPE(DirectoryNode);

    /// Exception: Unknown node name
    struct UnknownNodeNameException : public senf::Exception
    { UnknownNodeNameException() : senf::Exception("Unknown node name") {}};

#ifndef DOXYGEN
    template <class Type>
    struct NodeCreateTraits< boost::shared_ptr<Type> >
    {};
#endif

    /** \brief Config/console tree command node

        The CommandNode is the base-class for the tree leaf nodes. Concrete command node
        implementations are derived from this class.

        To execute a command, CommandNode::operator()() is called. This abstract virtual function
        must be implemented in a derived class.
      */
    class CommandNode : public GenericNode
    {
        SENF_LOG_CLASS_AREA();
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<CommandNode> ptr;
        typedef boost::shared_ptr<CommandNode const> cptr;
        typedef boost::weak_ptr<CommandNode> weak_ptr;

        typedef ParseCommandInfo::ArgumentsRange Arguments;

        ///////////////////////////////////////////////////////////////////////////

        virtual void operator()(std::ostream & output, Arguments const & arguments) = 0;
                                        ///< Called to execute the command
                                        /**< \param[in] output stream where result messages may be
                                                 written to
                                             \param[in] arguments command arguments. This is a
                                                 range of ranges of ArgumentToken instances. */

        ptr thisptr();
        cptr thisptr() const;

    protected:
        CommandNode();

    private:
    };

    /** \brief Most simple CommandNode implementation

        This CommandNode implementation simply forwards the \a output and \a arguments arguments to
        an arbitrary callback.
      */
    class SimpleCommandNode : public CommandNode
    {
        SENF_LOG_CLASS_AREA();
    public:
        ///////////////////////////////////////////////////////////////////////////
        // Types

        typedef boost::shared_ptr<SimpleCommandNode> ptr;
        typedef boost::shared_ptr<SimpleCommandNode const> cptr;
        typedef boost::weak_ptr<SimpleCommandNode> weak_ptr;

        typedef boost::function<void (std::ostream &, Arguments const &)> Function;

        ///////////////////////////////////////////////////////////////////////////

        virtual void operator()(std::ostream & output, Arguments const & arguments);

        ptr thisptr();
        cptr thisptr() const;

        static ptr create(Function const & fn);

        SimpleCommandNode & doc(std::string const & doc);

    protected:
        SimpleCommandNode(Function const & fn);

    private:
        virtual void v_help(std::ostream & output) const;

        Function fn_;
        std::string doc_;
    };

#ifndef DOXYGEN
    template <class Function>
    SimpleCommandNode & senf_console_add_node(DirectoryNode & node, std::string const & name, 
                                              Function const & fn, ...);
#endif

    BOOST_TYPEOF_REGISTER_TYPE(SimpleCommandNode);

    DirectoryNode & root();

}}

///////////////////////////////hh.e////////////////////////////////////////
#include "Node.cci"
#include "Node.ct"
#include "Node.cti"
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