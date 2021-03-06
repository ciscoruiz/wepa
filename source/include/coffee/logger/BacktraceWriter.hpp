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

#ifndef __coffee_logger_BacktraceWriter_hpp
#define __coffee_logger_BacktraceWriter_hpp

#include <list>

#include <coffee/logger/CircularTraceWriter.hpp>

namespace coffee {
namespace logger {

/**
 * This writer could be used for solve problem with heavy load.
 *
 * It stores N lines with different levels and it will write them only when a
 * trace of error is detected. This way when you get and error you should get
 * the 1024 previous lines of debugging or information previous to that error.
 *
 * \include test/logger/BacktraceWriter_test.cc
 */
class BacktraceWriter : public CircularTraceWriter {
public:
   static const int MaxBacktrackingLength;

   /**
    * Constructor.
    * \param path file path to store the traces.
    * \param maxKbSize Max size of that file expressed in KBytes.
    * \param backtrackingLength Max numbers of lines to show previous to an error.
    */
   BacktraceWriter (const std::string& path, const size_t maxKbSize, const int backtrackingLength);

   /**
    * Set the lowest level of traces to be collected. By default the lowest level would be coffee::logger::Level::Debug.
    */
   void setLowestLeveL (const Level::_v lowestLeveL) noexcept { if (lowestLeveL > Level::Error) m_lowestLevel = lowestLeveL; }

private:
   typedef std::pair <Level::_v, std::string> Line;
   typedef std::list <Line> Lines;

   const int m_backtrackingLength;
   Lines m_lines;
   int m_lineCounter;
   Level::_v m_lowestLevel;

   void apply (const Level::_v level, const std::string& line) noexcept;
   bool wantsToProcess (const Level::_v level) const noexcept { return level <= m_lowestLevel; }
   void backtrace () noexcept;
};

} /* namespace logger */
} /* namespace coffee */

#endif
