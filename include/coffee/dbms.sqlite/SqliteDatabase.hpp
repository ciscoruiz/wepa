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
#ifndef _coffee_dbms_sqlite_SqliteDatabase_hpp_
#define _coffee_dbms_sqlite_SqliteDatabase_hpp_

#include <boost/filesystem.hpp>

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/ErrorCodeInterpreter.hpp>

namespace coffee {

namespace dbms  {

namespace sqlite {

class SqliteDatabase : public Database {
public:
   SqliteDatabase(const boost::filesystem::path& dbFile);
   ~SqliteDatabase();

private:
   class SqliteErrorCodeInterpreter : public dbms::ErrorCodeInterpreter {
   public:
      SqliteErrorCodeInterpreter() {;}
   private:
      bool notFound(const int errorCode) const throw(adt::RuntimeException);
      bool successful(const int errorCode) const throw(adt::RuntimeException);
      bool locked(const int errorCode) const throw(adt::RuntimeException);
      bool lostConnection(const int errorCode) const throw(adt::RuntimeException);
   };

   std::shared_ptr<Connection> allocateConnection(const std::string& name, const char* user, const char* password)
      throw(adt::RuntimeException);

   std::shared_ptr<Statement> allocateStatement(const char* name, const std::string& expression, const ActionOnError::_v actionOnError)
      throw(adt::RuntimeException);

   std::shared_ptr<binder::Input> allocateInputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException);
   std::shared_ptr<binder::Output> allocateOutputBind(std::shared_ptr<datatype::Abstract> data) const
      throw(adt::RuntimeException);

};

}
}
}

#endif
