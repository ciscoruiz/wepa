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
#ifndef _wepa_adt_Millisecond_h
#define _wepa_adt_Millisecond_h

#include <time.h>

#include <string>

#include <wepa/config/defines.hpp>
#include <wepa/adt/RuntimeException.hpp>

namespace wepa {

namespace adt {

class Second;
class Microsecond;

class Millisecond {
public:
   typedef Integer64 type_t;

   /**
    * Constructor
    */
   Millisecond() : m_value (0) {;}

   /**
    * Constructor.
    * \param value Valor inicial de esta instancia.
    */
   explicit Millisecond(const type_t value) : m_value (value) {;}

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Millisecond (const Millisecond& other) : m_value (other.m_value) {;}

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Millisecond (const Second& other);

   /**
    * Constructor copia.
    * \param other Instancia de la que copiar.
    */
   Millisecond (const Microsecond& other);

   operator type_t () const throw () { return m_value; }

   Millisecond& operator= (const type_t other) throw () { m_value = other; return *this; }

   Millisecond& operator= (const Millisecond& other) throw () { m_value = other.m_value; return *this; }

   Millisecond& operator= (const Second& other) throw ();

   Millisecond& operator= (const Microsecond& other) throw ();

   bool operator== (const Millisecond& other) const throw () { return m_value == other.m_value; }

   bool operator== (const Second& other) const throw ();

   bool operator== (const Microsecond& other) const throw ();

   bool operator!= (const Millisecond& other) const throw () { return m_value != other.m_value; }

   bool operator!= (const Second& other) const throw ();

   bool operator!= (const Microsecond& other) const throw ();

   bool operator> (const Millisecond& other) const throw () { return m_value > other.m_value; }

   bool operator> (const Second& other) const throw ();

   bool operator> (const Microsecond& other) const throw ();

   bool operator< (const Millisecond& other) const throw () { return m_value < other.m_value; }

   bool operator< (const Second& other) const throw ();

   bool operator< (const Microsecond& other) const throw ();

   bool operator>= (const Millisecond& other) const throw () { return m_value >= other.m_value; }

   bool operator>= (const Second& other) const throw () { return (operator==(other) == true) ? true: operator>(other); }

   bool operator>= (const Microsecond& other) const throw () { return (operator==(other) == true) ? true: operator>(other); }

   bool operator<= (const Millisecond& other) const throw () { return m_value <= other.m_value; }

   bool operator<= (const Second& other) const throw () { return (operator==(other) == true) ? true: operator<(other); }

   bool operator<= (const Microsecond& other) const throw () { return (operator==(other) == true) ? true: operator<(other); }

   Millisecond& operator+= (const Millisecond& other) throw () { m_value += other.m_value; return *this; }

   Millisecond& operator-= (const Millisecond& other) throw () { (m_value > other.m_value) ? (m_value -= other.m_value): (m_value = 0); return *this; }

   type_t getValue () const throw () { return m_value; }

   timeval* getTimeVal (timeval& tv) const throw ();

   static Millisecond getTime () throw ();

   std::string asString () const throw ();

   static Millisecond fromString (const std::string& value) throw (RuntimeException);

private:
   type_t m_value;

   friend class Second;
   friend class Microsecond;

   friend class Millisecond operator / (const Millisecond& left, const Millisecond& right) throw ();
   friend class Millisecond operator + (const Millisecond& left, const Millisecond& right) throw ();
   friend class Millisecond operator - (const Millisecond& left, const Millisecond& right) throw ();
   friend class Millisecond operator / (const Millisecond& left, const int right) throw ();
   friend class Millisecond operator / (const Millisecond& left, const unsigned int right) throw ();
   friend class Millisecond operator * (const Millisecond& left, const int right) throw ();
};

inline Millisecond operator / (const Millisecond& left, const Millisecond& right)
   throw ()
{
   return Millisecond (left.m_value / right.m_value);
}

inline Millisecond operator + (const Millisecond& left, const Millisecond& right)
   throw ()
{
   return Millisecond (left.m_value + right.m_value);
}

inline Millisecond operator - (const Millisecond& left, const Millisecond& right)
   throw ()
{
   return Millisecond (left.m_value - right.m_value);
}

inline Millisecond operator / (const Millisecond& left, const int right)
   throw ()
{
   return Millisecond (left.m_value / right);
}

inline Millisecond operator / (const Millisecond& left, const unsigned int right)
   throw ()
{
   return Millisecond (left.m_value / right);
}

inline Millisecond operator * (const Millisecond& left, const int right)
   throw ()
{
   return Millisecond (left.m_value * right);
}

}
}

#endif