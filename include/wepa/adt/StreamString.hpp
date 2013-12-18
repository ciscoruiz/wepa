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
#ifndef _weap_adt_StreamString_H
#define _weap_adt_StreamString_H

#include <string>

#include <stdio.h>

#include <wepa/config/defines.hpp>

namespace wepa  {

namespace adt {

/**
 *  
 *
 */
class StreamString : public std::string {
public:
   /**
    * Flags to tuning behaviour 
    */
   struct Flag { enum _v { None, ShowNull }; };

   /**
    * Constructor.
    * \param flag Set how to manage the null value.
    */
   StreamString (const Flag::_v flag = Flag::None) : a_flags (flag) {;}

   /**
    * Constructor.
    * \param str Buffer to initialize content of the instance. It can be NULL.
    * \param flag Set how to manage the null value.
    */
   StreamString (const char* str, const Flag::_v flag = Flag::None) :
      std::string ((str == NULL) ? ((flag & Flag::ShowNull) ? "<null>": ""): str),
      a_flags (flag)
   {;}

   /**
    * Copy constructor
    * \param other Other instance to duplicate.
    */
   StreamString (const StreamString &other) : std::string (other), a_flags (other.a_flags) {;}

   /**
    * Constructor copia.
    * \param other Buffer to initialize content of the instance. It can be NULL.
    * \param flag Set how to manage the null value.
    */
   StreamString (const std::string& other, const Flag::_v flag = Flag::None) : std::string (other), a_flags (flag) {;}

   /**
    * Convert to upper case.
    */
   void toUpper () throw ();

   /**
    * Convert to lower case.
    */
   void toLower () throw ();

   StreamString& operator = (const char vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const char* vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const int vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const unsigned int vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const bool vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const Integer64 vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const Unsigned64 vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const float vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const double vv) throw () { std::string::clear (); return operator<< (vv); }
   StreamString& operator = (const std::string& vv) throw () { std::string::operator= (vv); return *this; }

   StreamString& operator << (const char vv) throw ();
   StreamString& operator << (const char* vv) throw ();
   StreamString& operator << (const int vv) throw ();
   StreamString& operator << (const unsigned int vv) throw ();
   StreamString& operator << (const bool vv) throw ();
   StreamString& operator << (const Integer64 vv) throw ();
   StreamString& operator << (const Unsigned64 vv) throw ();
   StreamString& operator << (const float vv) throw ();
   StreamString& operator << (const double vv) throw ();
   StreamString& operator << (const std::string& vv) throw () { std::string::append (vv); return *this; }

private:
   Flag::_v a_flags;
};

}
}

#endif