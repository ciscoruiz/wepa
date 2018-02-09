// COFFEE - COmpany eFFEEctive Platform
//
// (c) Copyright 2018 Francisco Ruiz Rayo
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
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Author: cisco.tierra@gmail.com
//
#ifndef __wepm_logger_TraceMethod_hpp
#define __wepm_logger_TraceMethod_hpp

#include <coffee/logger/Level.hpp>
#include <coffee/logger/Logger.hpp>

namespace coffee {

namespace logger {

class TraceMethod  {
public:
   TraceMethod (const char* methodName, const char* fromFile, const int fromLine) :
      m_level (Level::Debug),
      m_methodName (methodName),
      m_fromFile (fromFile),
      m_ok (false)
   {
      if ((m_ok = Logger::wantsToProcess (m_level)) == true)
         Logger::write (m_level, "+++ begin +++", methodName, fromFile, fromLine);
   }

   TraceMethod (const Level::_v level, const char* methodName, const char* fromFile, const int fromLine) :
      m_level (level),
      m_methodName (methodName),
      m_fromFile (fromFile),
      m_ok (false)
   {
      if ((m_ok = Logger::wantsToProcess (m_level)) == true)
         Logger::write (m_level, "+++ begin +++", methodName, fromFile, fromLine);
   }

   /**
      Destructor.
   */
   ~TraceMethod () {
      if (m_ok == true && Logger::wantsToProcess(m_level) == true) {
         Logger::write (m_level, "+++ end +++", m_methodName, m_fromFile, 0);
      }
   }

private:
   const Level::_v m_level;
   const char* m_methodName;
   const char* m_fromFile;
   bool m_ok;
};

}
}

#define LOG_THIS_METHOD() coffee::logger::TraceMethod __traceMethod__ (COFFEE_LOG_LOCATION)

#endif