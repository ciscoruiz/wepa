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

#ifndef __coffee_xml_Document_hpp
#define __coffee_xml_Document_hpp

struct _xmlDoc;

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

   const Document& parse(const boost::filesystem::path& path) throw(adt::RuntimeException);
   const Document& parse(const adt::DataBlock& buffer) throw(adt::RuntimeException);

   const Document& parse(const boost::filesystem::path& path, const DTD& dtd) throw(adt::RuntimeException);
   const Document& parse(const adt::DataBlock& buffer, const DTD& dtd) throw(adt::RuntimeException);

   const std::shared_ptr<Node> getRoot() const noexcept { return m_root; }
   std::shared_ptr<Node> getRoot() noexcept { return m_root; }

private:
   std::shared_ptr <Node> m_root;

   void parseFile(const boost::filesystem::path& path) throw(adt::RuntimeException);
   void parseMemory(const adt::DataBlock& buffer) throw(adt::RuntimeException);

   void extractNodes(_xmlDoc* handler) throw(adt::RuntimeException);

   const char* readName(const Handler handler) const noexcept;

   static void extractNodes(std::shared_ptr<xml::Node>& node) throw(adt::RuntimeException);
   static void extractAttributes(std::shared_ptr<xml::Node>& node) throw(adt::RuntimeException);

   void compile(Compiler& compiler) const throw(adt::RuntimeException);

   friend class Compiler;
};

} /* namespace xml */
} /* namespace coffee */
#endif
