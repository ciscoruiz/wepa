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
#include <wepa/logger/Logger.hpp>

#include <wepa/logger/Writer.hpp>
#include <wepa/logger/DefaultFormatter.hpp>
#include <wepa/logger/Level.hpp>
#include <wepa/logger/SCCS.hpp>

using namespace wepa;

#ifdef _DEBUG
   logger::Level::_v logger::Logger::m_level = Level::Debug;
#else
   logger::Level::_v logger::Logger::m_level = Level::Warning;
#endif

logger::Logger::FormatterPtr logger::Logger::m_formatter;
logger::Logger::WriterPtr logger::Logger::m_writer;

//static
void logger::Logger::initialize (Writer* writer, Formatter* formatter)
   throw (adt::RuntimeException)
{
   SCCS::activate ();

   m_writer.reset (writer);
   m_writer->initialize ();

   m_formatter.reset (formatter);
}

//static
void logger::Logger::initialize (Writer* writer)
   throw (adt::RuntimeException)
{
   initialize (writer, new DefaultFormatter ());
}

//static
void logger::Logger::write (const Level::_v level, const adt::StreamString& input, const char* function, const char* file, const unsigned lineno)
   throw ()
{
   if (m_writer.get () == NULL || m_formatter.get () == NULL)
      return;

   // Writer will ask 'Logger' about what to do with this level, but it could be different for some kinds of writer.
   if (m_writer->wantsToProcess (level) == false)
      return;

   Formatter::Elements elements (level, input, function, file, lineno);
   m_writer->apply (level, m_formatter->apply (elements));
}

//static
bool logger::Logger::wantsToProcess (const Level::_v level)
   throw ()
{
   if (m_writer.get () == NULL || m_formatter.get () == NULL)
      return false;

   return isActive (level) ? true: m_writer->wantsToProcess(level);
}