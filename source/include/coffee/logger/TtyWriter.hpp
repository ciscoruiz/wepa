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

#ifndef __coffee_logger_TtyWriter_hpp
#define __coffee_logger_TtyWriter_hpp

#include <iostream>

#include <coffee/logger/Writer.hpp>

namespace coffee {
namespace logger {

/**
 * Writer to write the traces on console.
 */
class TtyWriter : public Writer {
public:
   /**
    * Constructor.
    */
   TtyWriter () : Writer ("TtyWriter") {;}

   /**
    * Fast shared creator
    */
   static std::shared_ptr<TtyWriter> instantiate() {
      return std::make_shared<TtyWriter>();
   }

private:
   void initialize () throw (basis::RuntimeException) {;}
   void apply (const Level::_v level, const std::string& line) noexcept {
      std::cout << line << std::endl;
   }
};

} /* namespace logger */
} /* namespace coffee */
#endif
