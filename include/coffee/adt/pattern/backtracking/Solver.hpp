// COFFEE - COmpany eFFEEctive Platform
//
//(c) Copyright 2018 Francisco Ruiz Rayo
//
// https://github.com/ciscoruiz/coffee
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
//(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//

#ifndef _coffee_adt_pattern_backtracking_Solver_hpp_
#define _coffee_adt_pattern_backtracking_Solver_hpp_

#include <coffee/adt/pattern/backtracking/TreeNode.hpp>

namespace coffee {

namespace adt {

namespace pattern {

namespace backtracking {

/**
 * This is the solver using Backtracking techniques.
 *
 * \param _T Data type for scope for the problem's solutions
 *
 * \see http://en.wikipedia.org/wiki/Backtracking
 * \see http://www.cs.uiuc.edu/~jeffe/teaching/algorithms/notes/02-backtracking.pdf
 */
template <typename _T> class Solver : public TreeNode<_T> {
public:
   typedef TreeNode <_T> Solution;
   typedef typename std::shared_ptr<Solution> shared_solution;

   /**
    * Constructor.
    */
   Solver() {;}

   /**
    * Destructor.
    */
   virtual ~Solver() { ; }

   /**
    *
    * @param problem
    * @return Solutions for the received problem. It will erased when the Solver goes out of scope.
    */
   bool apply() noexcept {
      this->clear();


      for(_T ii = first(0, getStartingPoint()); stop(0, ii) == false; ii = next(0, ii)) {
         shared_solution solution = std::make_shared<Solution>(ii);

         if(exploreSolutions(1, solution) == true)
            this->add(solution);
      }

      return this->hasSuccessor();
   }

   shared_solution getSolution(const int index) const throw(RuntimeException) { return this->getNextStep(index); }

protected:
   virtual _T getStartingPoint() const noexcept = 0;

   virtual _T first(const int depth, _T value) const noexcept = 0;
   virtual bool stop(const int depth, _T value) const noexcept = 0;
   virtual _T next(const int depth, _T value) const noexcept = 0;

   virtual bool reject(const shared_solution& solution, const _T candidate) const noexcept = 0;
   virtual bool accept(const shared_solution& solution, const _T candidate) const noexcept = 0;

private:
   bool exploreSolutions(const int depth, shared_solution& onTestSolution) throw () {
      bool hasSolution = false;

      for(_T ii = first(depth, onTestSolution->getValue()); stop(depth, ii) == false; ii = next(depth, ii)) {
         if(backtracking(depth, onTestSolution, ii) == true)
            hasSolution = true;
      }

      return hasSolution;
   }

   bool backtracking(const int depth, shared_solution& onTestSolution, const _T candidate) noexcept {
      if(reject(onTestSolution, candidate) == true) return false;

      // onTestSolution + candidate become to a real Solution
      if(accept(onTestSolution, candidate) == true) {
         onTestSolution->add(std::make_shared<Solution>(candidate, onTestSolution));
         return true;
      }

      shared_solution partialSolution = std::make_shared<Solution>(candidate, onTestSolution);

      if(exploreSolutions(depth + 1, partialSolution) == false) {
         return false;
      }

      onTestSolution->add(partialSolution);

      return true;
   }

};

}
}
}
}

#endif


