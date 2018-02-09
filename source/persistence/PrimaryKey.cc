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
#include <memory>

#include <coffee/dbms/datatype/Abstract.hpp>

#include <coffee/persistence/PrimaryKey.hpp>
#include <coffee/persistence/PrimaryKeyBuilder.hpp>

using namespace coffee;

persistence::PrimaryKey::PrimaryKey(const PrimaryKeyBuilder& builder) :
   dbms::datatype::Set(builder)
{
}

bool persistence::PrimaryKey::matches(const PrimaryKey& other) const
   noexcept
{
   if (size() != other.size())
      return false;

   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ii ++) {
      if (other.constains(PrimaryKey::name(ii)) == false)
         return false;
   }

   for (const_data_iterator ii = other.begin(), maxii = other.end(); ii != maxii; ii ++) {
      if (constains(PrimaryKey::name(ii)) == false)
         return false;
   }

   return true;
}

size_t persistence::PrimaryKey::hash() const noexcept
{
   size_t result = size();

   for(PrimaryKey::const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      result ^= PrimaryKey::data(ii)->hash() << 1;
   }

   return result;
}

adt::StreamString persistence::PrimaryKey::asString() const noexcept
{
   adt::StreamString result("persistence.PrimaryKey { ");

   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      result += data(ii)->asString();
      result += "  ";
   }

   return result += "}";
}