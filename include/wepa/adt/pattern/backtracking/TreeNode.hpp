// WEPA - Write Excellent Professional Applications
//
// (c) Copyright 2013 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/wepa
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
//     * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//     * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following disclaimer
// in the documentation and/or other materials provided with the
// distribution.
//     * Neither the name of Google Inc. nor the names of its
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//

#ifndef _wepa_adt_pattern_backtracking_Step_hpp_
#define _wepa_adt_pattern_backtracking_Step_hpp_

#include <boost/ptr_container/ptr_vector.hpp>

#include <wepa/adt/RuntimeException.hpp>

namespace wepa {

namespace adt {

namespace pattern {

namespace backtracking {

template <typename _T> class TreeNode {
   typedef boost::ptr_vector <TreeNode <_T> > Successors;

public:
   typedef typename Successors::iterator successor_iterator;
   typedef typename Successors::const_iterator const_successor_iterator;

   /**
    * Class to operate on every treeNode of the found solution
    */
   class Predicate {
   public:
      /**
       * Destructor
       * @param treeNode
       * @param depth
       */
      virtual ~Predicate () {;}

      /**
       * Apply the predicate on the received treeNode.
       * @param treeNode One solution's treeNode
       * @param depth Depth of this treeNode.
       */
      virtual void apply (const TreeNode <_T>& treeNode, const int depth) const throw () = 0;
   };

   TreeNode (const _T& value) : m_value (value), m_predeccesor (NULL) {;}
   TreeNode (const _T& value, const TreeNode* predeccesor) :  m_value (value), m_predeccesor (predeccesor) {;}
   virtual ~TreeNode () { m_successors.clear (); }

   void add (TreeNode <_T>* treeNode) throw () { m_successors.push_back (treeNode); }

   const _T& getValue () const throw () { return m_value; }

   successor_iterator successor_begin () throw () { return m_successors.begin (); }
   successor_iterator successor_end () throw () { return m_successors.end (); }
   static TreeNode<_T>& get_successor (successor_iterator ii) throw (){ return *ii; }

   const_successor_iterator successor_begin () const throw () { return m_successors.begin (); }
   const_successor_iterator successor_end () const throw () { return m_successors.end (); }
   static const TreeNode<_T>& get_successor (const_successor_iterator ii) throw (){ return *ii; }

   size_t successors_size () const throw () { return m_successors.size (); }

   bool hasSuccessor () const throw () { return m_successors.empty () == false; }

   const TreeNode<_T>* getPredeccessor () const throw () { return m_predeccesor; }
   
   /**
    * Runs over tree with depth-first algorithm and apply the \em _Predicate on every node.
    * \param predicate Operation to apply on every Node.
    * \param depth Depth of this node
    */
   virtual void depthFirst (const Predicate& predicate, const int depth = 0) const throw () {
      predicate.apply (*this, depth);

      for (const_successor_iterator ii = successor_begin (), maxii = successor_end (); ii != maxii; ++ ii) {
         get_successor (ii).depthFirst (predicate, depth + 1);
      }
   }

   /**
    * @return The number of found solutions (which matches with the number of leaf nodes on the tree)
    */
   size_t countSolutions () const throw () {
      int result = 0;

      if (hasSuccessor() == true) {
         for (const_successor_iterator ii = successor_begin (), maxii = successor_end (); ii != maxii; ++ ii) {
            result += get_successor (ii).countSolutions ();
         }
      }
      else
         result = 1;

      return result;
   }

   const TreeNode<_T>* getNextStep (const int index) const
      throw (RuntimeException)
   {
      if (m_successors.size () <= index) {
         WEPA_THROW_EXCEPTION ("Index=" << index << " is out of range (" << m_successors.size () << ")");
      }

      return &get_successor (m_successors.begin () + index);
   }

protected:
   TreeNode () : m_predeccesor (NULL) {;}

   void clear () throw () { m_successors.clear (); }

private:
   _T m_value;
   const TreeNode<_T>* m_predeccesor;
   Successors m_successors;
};


}
}
}
}

#endif
