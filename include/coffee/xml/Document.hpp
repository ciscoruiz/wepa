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
#ifndef __coffee_xml_Document_hpp
#define __coffee_xml_Document_hpp

struct _xmlDoc;
struct _xmlValidCtxt;

namespace boost {
   namespace filesystem {
      class path;
   }
}

#include <memory>
#include <coffee/adt/RuntimeException.hpp>

#include <coffee/xml/Wrapper.hpp>

namespace coffee {

namespace adt {
   class DataBlock;
}

namespace xml {

class Node;
class DTD;
class Compiler;

class Document : public Wrapper <_xmlDoc> {
public:
   Document();
   virtual ~Document();

   void parse(const boost::filesystem::path& path) throw(adt::RuntimeException);
   void parse(const adt::DataBlock& buffer) throw(adt::RuntimeException);
   void parse(const char* buffer, const size_t size) throw(adt::RuntimeException);

   void parse(const boost::filesystem::path& path, const DTD& dtd) throw(adt::RuntimeException);
   void parse(const adt::DataBlock& buffer, const DTD& dtd) throw(adt::RuntimeException);
   void parse(const char* buffer, const size_t size, const DTD& dtd) throw(adt::RuntimeException);

   const std::shared_ptr<Node> getRoot() const noexcept { return m_root; }
   std::shared_ptr<Node> getRoot() noexcept { return m_root; }

private:
   std::shared_ptr <Node> m_root;

   void parseFile(const boost::filesystem::path& path) throw(adt::RuntimeException);
   void parseMemory(const adt::DataBlock& buffer) throw(adt::RuntimeException);

   void extractNodes(_xmlDoc* handler) throw(adt::RuntimeException);

   static const char* nameExtractor(const Handler handler) noexcept;

   static void extractNodes(std::shared_ptr<xml::Node>& node) throw(adt::RuntimeException);
   static void extractAttributes(std::shared_ptr<xml::Node>& node) throw(adt::RuntimeException);

   void compile(Compiler& compiler) const throw(adt::RuntimeException);

   friend class Compiler;
};

} /* namespace xml */
} /* namespace coffee */
#endif
