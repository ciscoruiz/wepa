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
#ifndef __wepa_persistence_PrimaryKey_hpp
#define __wepa_persistence_PrimaryKey_hpp

#include <vector>
#include <memory>

#include <wepa/adt/RuntimeException.hpp>
#include <wepa/adt/StreamString.hpp>

#include <wepa/dbms/datatype/Set.hpp>

namespace wepa {

namespace dbms {
   namespace datatype {
      class Abstract;
   }
}

namespace persistence {

class PrimaryKeyBuilder;

class PrimaryKey : public dbms::datatype::Set {
public:
   struct EqualSharedPointer {
      bool operator()(const std::shared_ptr<PrimaryKey>& lhs, const std::shared_ptr<PrimaryKey>& rhs) const {
         return lhs->compare(rhs) == 0;
      }
   };
   struct HashSharedPointer {
      size_t operator()(const std::shared_ptr<PrimaryKey>& primaryKey) const { return primaryKey->hash(); }
   };

   PrimaryKey(const PrimaryKeyBuilder& builder);

   PrimaryKey& operator= (const PrimaryKey& other) throw (adt::Exception) { Set::operator=(other); return *this; }

   bool operator== (const PrimaryKey& other) const throw (adt::RuntimeException) { return compare(other) == 0; }
   bool operator< (const PrimaryKey& other) const throw (adt::RuntimeException){ return compare(other) < 0; }

   bool matches(const PrimaryKey& other) const noexcept;

   size_t hash() const noexcept;

   operator adt::StreamString () const noexcept { return asString (); }
   adt::StreamString asString () const noexcept;

private:
   PrimaryKey (const PrimaryKey& other) = delete;
};

} /* namespace persistence */
} /* namespace wepa */
#endif
