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
#ifndef __coffee_xml_Compiler_hpp
#define __coffee_xml_Compiler_hpp

#include <memory>

#include <coffee/adt/RuntimeException.hpp>
#include <coffee/adt/DataBlock.hpp>

#include <coffee/xml/Wrapper.hpp>

struct _xmlTextWriter;
struct _xmlBuffer;

namespace coffee {
namespace xml {

class Document;
class Node;

class Compiler : public Wrapper <_xmlTextWriter> {
public:
   Compiler ();
   virtual ~Compiler ();

   std::string apply (const Document& document) throw (adt::RuntimeException);
   std::string apply (const std::shared_ptr<Node>& node) throw (adt::RuntimeException);

   const char* encode (const char* text) throw (adt::RuntimeException);
   const char* encode (const std::string& text) throw (adt::RuntimeException) { return encode (text.c_str ()); }

   void setEncoding (const char* encoding) noexcept { m_encoding = encoding; }

private:
   const char* m_encoding;
   char* m_encodedBuffer;
   int  m_encodedReservedSize;

   Compiler (const Compiler&);

   class Buffer : public Wrapper <_xmlBuffer> {
   public:
      Buffer ();
      ~Buffer () {;}
      const char* getValue () const noexcept;
   };
};

} /* namespace xml */
} /* namespace coffee */
#endif