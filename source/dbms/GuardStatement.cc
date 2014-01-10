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

#include <wepa/dbms/Statement.hpp>
#include <wepa/dbms/GuardStatement.hpp>
#include <wepa/dbms/GuardConnection.hpp>

using namespace wepa;

dbms::GuardStatement::GuardStatement(GuardConnection& guardConnection, Statement& statement)  throw (adt::RuntimeException) :
   m_guardConnection (guardConnection),
   m_statement (statement)
{
   guardConnection.linkStatement();
   statement.lock();
}

dbms::GuardStatement::GuardStatement(GuardConnection& guardConnection, Statement* statement) throw (adt::RuntimeException) :
   m_guardConnection (guardConnection),
   m_statement (*statement)
{
   if (statement == NULL)
      WEPA_THROW_EXCEPTION("Statement can not be NULL");

   guardConnection.linkStatement();

   statement->lock();
}

dbms::GuardStatement::~GuardStatement()
{
   m_statement.unlock();
   m_guardConnection.unlinkStatement();
}

dbms::datatype::Abstract& dbms::GuardStatement::getInputData (const int pos)
   throw (adt::RuntimeException)
{
   return m_statement.getInputData(pos);
}

const dbms::datatype::Abstract& dbms::GuardStatement::getOutputData (const int pos) const
   throw (adt::RuntimeException)
{
   return m_statement.getOutputData(pos);
}

dbms::ResultCode dbms::GuardStatement::execute ()
   throw (adt::RuntimeException, DatabaseException)
{
   return m_guardConnection.execute (m_statement);
}

bool dbms::GuardStatement::fetch ()
   throw (adt::RuntimeException, DatabaseException)
{
   return m_statement.fetch();
}

void dbms::GuardStatement::setRequiresCommit (const bool requiresCommit) noexcept
{
   m_statement.setRequiresCommit(requiresCommit);
}


