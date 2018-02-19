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
#ifndef __coffee_balance_Strategy_hpp
#define __coffee_balance_Strategy_hpp

#include <memory>

#include <coffee/adt/NamedObject.hpp>
#include <coffee/balance/ResourceUnavailableException.hpp>

namespace coffee {

namespace xml {
   class Node;
}

namespace balance {

class Resource;
class GuardResourceList;
class ResourceList;

class Strategy : public adt::NamedObject {
public:
   virtual std::shared_ptr<Resource> apply(GuardResourceList& guard) throw (ResourceUnavailableException) = 0;

   virtual adt::StreamString asString () const noexcept;
   virtual std::shared_ptr<xml::Node> asXML (std::shared_ptr<xml::Node>& parent) const noexcept;

   std::shared_ptr<ResourceList>& getResourceList() { return m_resources; }

protected:
   std::shared_ptr<ResourceList> m_resources;

   Strategy(const std::string& name, std::shared_ptr<ResourceList>& resources) : adt::NamedObject(name), m_resources(resources) {;}
   explicit Strategy(const std::string& name) : adt::NamedObject(name) {;}
   void setResourceList(std::shared_ptr<ResourceList>& resources) { m_resources = resources; }
};

}
}

#endif /* __coffee_balance_Strategy_hpp */
