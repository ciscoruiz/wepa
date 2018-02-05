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
#ifndef _coffee_dbms_sqlite_SqliteConnection_hpp_
#define _coffee_dbms_sqlite_SqliteConnection_hpp_

#include <sqlite3.h>
#include <coffee/dbms/Connection.hpp>

namespace coffee {
namespace dbms {
namespace sqlite {

class SqliteConnection : public Connection {
public:
   SqliteConnection(const Database& dbmsDatabase, const std::string& name, const char* user, const char* password);
   virtual ~SqliteConnection();

   ::sqlite3* getImpl() { return impl; }

   void execute(const char* statement) throw(adt::RuntimeException, DatabaseException);

private:
   ::sqlite3* impl;

   void open() throw(DatabaseException);
   void close() noexcept;
   bool isAvailable () const noexcept  { return impl != nullptr;  }
   void do_commit() throw(adt::RuntimeException, DatabaseException) { execute("COMMIT"); }
   void do_rollback() noexcept;
};

} /* namespace sqlite */
} /* namespace dbms */
} /* namespace coffee */

#endif
