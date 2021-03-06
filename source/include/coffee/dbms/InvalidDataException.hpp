// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#ifndef _coffee_dbms_InvalidDataException_hpp
#define _coffee_dbms_InvalidDataException_hpp

#include <coffee/basis/RuntimeException.hpp>

namespace coffee {

namespace dbms {

/**
 * Defines exception used for this library.
 *
 * @see http://www.boost.org/doc/libs/1_39_0/libs/exception/doc/exception_types_as_simple_semantic_tags.html
 */
class InvalidDataException : public basis::RuntimeException {
public:
   InvalidDataException (const std::string& str, const char* fromMethod, const char* fromFile, const unsigned fromLine) :
      basis::RuntimeException (str, fromMethod, fromFile, fromLine)
   {;}

   InvalidDataException (const InvalidDataException& other) : basis::RuntimeException (other) {;}

   std::string asString () const noexcept {
      basis::StreamString str (this->filePosition());
      str << what ();
      return str;
   }
};

}
}


#endif

