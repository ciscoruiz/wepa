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
#include <coffee/dbms/datatype/LongBlock.hpp>
#include <coffee/adt/AsHexString.hpp>

using namespace coffee;
using namespace coffee::dbms;

datatype::LongBlock::LongBlock (const char* name, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::LongBlock, 0, constraint),
   m_value ()
{
   datatype::Abstract::setBuffer ((void*) NULL);
}
datatype::LongBlock::LongBlock (const std::string& name, const Constraint::_v constraint) :
   datatype::Abstract (name, Datatype::LongBlock, 0, constraint),
   m_value ()
{
   datatype::Abstract::setBuffer ((void*) NULL);
}

datatype::LongBlock::LongBlock (const LongBlock& other) :
   datatype::Abstract (other),
   m_value ()
{
   m_value = other.m_value;
}

void datatype::LongBlock::setValue (const adt::DataBlock& value)
   throw (adt::RuntimeException)
{
   m_value = value;
   this->isNotNull();
}

adt::StreamString datatype::LongBlock::asString () const
   noexcept
{
   adt::StreamString result ("datatype.LongBlock { ");
   result += datatype::Abstract::asString ();
   result << " | Buffer: " << adt::AsHexString::apply(coffee_ptrnumber_cast (getBuffer()));
   result << " | MaxSize: " << getMaxSize();
   result += " | Size: ";
   if (this->hasValue () == true)
      result << m_value.size ();
   else
      result += "(null)";
   return result += " }";
}

int datatype::LongBlock::do_compare (const datatype::Abstract& other) const
   throw (adt::RuntimeException)
{
   COFFEE_THROW_EXCEPTION(asString () << " | Can not apply");
   return 0;
}
