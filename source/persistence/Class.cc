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
#include <coffee/persistence/Class.hpp>
#include <coffee/persistence/ClassBuilder.hpp>
#include <coffee/persistence/Object.hpp>
#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>

#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

using namespace coffee;

persistence::Class::Class(const ClassBuilder& classBuilder) :
     adt::NamedObject(classBuilder.getClassName())

{
   m_primaryKey = classBuilder.getPrimaryKey();
   m_members = classBuilder;
}

std::shared_ptr<persistence::PrimaryKey> persistence::Class::createPrimaryKey() const
   throw ()
{
   PrimaryKeyBuilder builder;

   for (auto ii = m_primaryKey->begin(), maxii = m_primaryKey->end(); ii != maxii; ++ ii) {
      builder.add(PrimaryKey::data(ii)->clone());
   }

   return builder.build();
}

std::shared_ptr<persistence::Object> persistence::Class::createObject(const std::shared_ptr<PrimaryKey>& primaryKey) const
   throw (adt::RuntimeException)
{
   if (!m_primaryKey->matches(*primaryKey.get())) {
      COFFEE_THROW_EXCEPTION(asString() << " primary key does not matches with class");
   }

    dbms::datatype::Set members;

    for(dbms::datatype::Set::const_data_iterator ii = m_members.begin(), maxii = m_members.end(); ii != maxii; ++ ii) {
       members.insert(dbms::datatype::Set::data(ii)->clone());
    }

    return std::make_shared<Object>(*this, primaryKey, members);
}

adt::StreamString persistence::Class::asString() const noexcept {
   adt::StreamString result("persistence.Class { ");
   result << adt::NamedObject::asString();
   result << " | N-members=" << m_members.size();
   return result << " }";
}

std::shared_ptr<xml::Node> persistence::Class::asXML(std::shared_ptr<xml::Node>& parent) const noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("persistence.Class");
   result->createAttribute("Name", this->getName());
   return result;
}

