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
#include <stdlib.h>
#include <locale.h>

#include <string>
#include <algorithm>

#include <coffee/config/defines.hpp>

#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/Logger.hpp>

#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/dbms/Statement.hpp>
#include <coffee/dbms/Connection.hpp>

#include <coffee/dbms/FailRecoveryHandler.hpp>
#include <coffee/dbms/StatementTranslator.hpp>
#include <coffee/dbms/SCCS.hpp>

#include <coffee/dbms/internal/DummyApplication.hpp>
#include "../../include/coffee/dbms/Database.hpp"

using namespace std;
using namespace coffee;


dbms::Database::Database(app::Application& app, const char* className, const char* dbmsName) :
   app::Engine(app, className),
   m_name((dbmsName == NULL) ? "local": dbmsName),
   m_type((dbmsName == NULL) ? Type::Local: Type::Remote),
   m_failRecoveryHandler(NULL),
   m_statementTranslator(NULL)
{
   dbms::SCCS::activate();
}

dbms::Database::Database(const char* rdbmsName, const char* dbmsName) :
   dbms::Database(internal::DummyApplication::getInstance(), rdbmsName, dbmsName)
{;}

dbms::Database::~Database()
{
   if(this->isStopped() == false)
      stop();
}

void dbms::Database::externalInitialize()
   throw(adt::RuntimeException)
{
   initialize();
}

void dbms::Database::externalStop()
   throw(adt::RuntimeException)
{
   app::Application& application(getApplication());
   app::Application& dummy = internal::DummyApplication::getInstance();

   if(&dummy != &application) {
      COFFEE_THROW_EXCEPTION(asString() << " | This method can't be applied to a database with associated application");
   }

   stop();
}

void dbms::Database::do_initialize()
   throw(adt::RuntimeException)
{
   LOG_THIS_METHOD();

   int counter(0);
   bool error = false;

   for(connection_iterator iic = connection_begin(), maxiic = connection_end(); iic != maxiic; iic ++) {
      try {
         connection(iic)->open();
         counter ++;
      }
      catch(adt::Exception& ex) {
         logger::Logger::write(ex);
         error = true;
      }
   }

   if(counter == 0 && error == true) {
      COFFEE_THROW_EXCEPTION(asString() << " | There is not connection available");
   }

   LOG_INFO(asString() << " | It is initialized");
}

void dbms::Database::do_stop()
   noexcept
{
   LOG_THIS_METHOD();

   int counter = 0;

   for(connection_iterator iic = connection_begin(), maxiic = connection_end(); iic != maxiic; ++ iic) {
      std::shared_ptr<Connection>& _connection = connection(iic);
      try {
         _connection->close();
         ++ counter;
      }
      catch(adt::Exception& ex) {
         logger::Logger::write(ex);
      }
   }

   LOG_DEBUG(asString() << " | Closed " << counter << " of " << m_connections.size());

   m_connections.clear();
   m_statements.clear();
}

std::shared_ptr<dbms::Connection> dbms::Database::createConnection(const char* name, const char* user, const char* password)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   LOG_DEBUG("Name=" << name << " | User=" << user);

   if(m_connections.size() >= MaxConnection) {
      COFFEE_THROW_EXCEPTION(asString() << " Can not create more than " << MaxConnection);
   }

   for(connection_iterator ii = connection_begin(), maxii = connection_end(); ii != maxii; ii ++) {
      if(connection(ii)->getName() == name) {
         COFFEE_THROW_EXCEPTION(asString() << " | Connection=" << name << " is already defined");
      }
   }

   string strname(name);

   std::shared_ptr<Connection> result = allocateConnection(strname, user, password);

   LOG_DEBUG(result->asString());

   if(this->isRunning() == true) {
      try {
         result->open();
         m_connections.push_back(result);
      }
      catch(adt::Exception& ex) {
         logger::Logger::write(ex);
         result.reset();
         throw;
      }
   }
   else
      m_connections.push_back(result);

   LOG_DEBUG(result->asString());

   return result;
}

std::shared_ptr<dbms::Connection>& dbms::Database::findConnection(const char* name)
   throw(adt::RuntimeException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   LOG_DEBUG("Name=" << name);

   for(connection_iterator ii = connection_begin(), maxii = connection_end(); ii != maxii; ii ++) {
      if(connection(ii)->getName() == name) {
         return connection(ii);
      }
   }

   COFFEE_THROW_EXCEPTION(asString() << " | Connection='" << name << "' is not defined");
}

std::shared_ptr<dbms::Statement> dbms::Database::createStatement(const char* name, const char* expression, const ActionOnError::_v actionOnError)
   throw(adt::RuntimeException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   for(statement_iterator ii = statement_begin(), maxii = statement_end(); ii != maxii; ++ ii) {
      if(statement(ii)->getName() == name)
         COFFEE_THROW_EXCEPTION("Sentence '" << name << "' is already defined");
   }

   if(m_statementTranslator)
      expression = m_statementTranslator->apply(expression);

   std::shared_ptr<Statement> result = allocateStatement(name, expression, actionOnError);

   LOG_DEBUG(result->asString());

   m_statements.push_back(result);

   return result;
}

std::shared_ptr<dbms::Statement>& dbms::Database::findStatement(const char* name)
   throw(adt::RuntimeException)
{
   logger::TraceMethod ttmm(logger::Level::Local7, COFFEE_FILE_LOCATION);

   Statement* result(NULL);

   LOG_DEBUG("Name=" << name);

   for(statement_iterator ii = statement_begin(), maxii = statement_end(); ii != maxii; ii ++) {
      if(statement(ii)->getName() == name) {
         return statement(ii);
      }
   }

   COFFEE_THROW_EXCEPTION("Statement '" << name << "' was not found");
}

void dbms::Database::notifyRecoveryFail(dbms::Connection& connection) const
   throw(adt::RuntimeException)
{
   LOG_WARN(connection.asString());

   if(m_failRecoveryHandler)
      m_failRecoveryHandler->apply(connection);
}

adt::StreamString dbms::Database::asString() const
   noexcept
{
   adt::StreamString result("dbms::Database { ");

   result << app::Engine::asString();

   if(m_type == Type::Local)
      result += " | Type: Local";
   else {
      result += " | Type: Remote | Name: ";
      result += m_name;
   }

   if(m_statementTranslator)
      result << " | StatementTranslator=" << m_statementTranslator->getName();

   return result += " }";
}

std::shared_ptr<xml::Node> dbms::Database::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("dbms.Database");

   app::Engine::asXML(result);

   result->createAttribute("Type",(m_type == Type::Local) ? "Local": "Remote");

   if(m_type != Type::Local)
      result->createAttribute("Name", m_name);

   if(m_statementTranslator)
      result->createAttribute("Translator", m_statementTranslator->getName());

   std::shared_ptr<xml::Node> connections = result->createChild("Connections");
   for(const_connection_iterator ii = connection_begin(), maxii = connection_end(); ii != maxii; ii ++)
      connection(ii)->asXML(connections);

   std::shared_ptr<xml::Node> statements = result->createChild("Statements");
   for(const_statement_iterator ii = statement_begin(), maxii = statement_end(); ii != maxii; ii ++)
      statement(ii)->asXML(statements);

   return result;
}
