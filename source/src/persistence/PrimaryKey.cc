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
      const_data_iterator oo = other.search(PrimaryKey::name(ii));

      if (oo == other.end())
         return false;

      if (PrimaryKey::data(oo)->getType() != PrimaryKey::data(ii)->getType())
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

basis::StreamString persistence::PrimaryKey::asString() const noexcept
{
   basis::StreamString result("persistence.PrimaryKey { ");

   bool first = true;
   for (const_data_iterator ii = begin(), maxii = end(); ii != maxii; ++ ii) {
      if (first == false) {
         result += ",";
      }
      result += data(ii)->asString();
      first = false;
   }

   return result += "}";
}
