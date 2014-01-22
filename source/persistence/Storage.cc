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
#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/adt/AsHexString.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/dbms/datatype/Abstract.hpp>

#include <wepa/persistence/Object.hpp>
#include <wepa/persistence/Loader.hpp>
#include <wepa/persistence/Recorder.hpp>
#include <wepa/persistence/Eraser.hpp>
#include <wepa/persistence/Storage.hpp>
#include <wepa/persistence/GuardClass.hpp>

using namespace wepa;

auto_enum_assign(persistence::Storage::AccessMode) = { "ReadOnly", "ReadWrite", NULL };

persistence::Storage::Storage (const char* name, const AccessMode::_v accessMode) :
   adt::NamedObject (name),
   m_accessMode(accessMode)
{
   m_hitCounter = m_faultCounter = 0;
}

persistence::Storage::~Storage ()
{
   for (entry_type ii : m_objects) {
      object (ii)->releaseDependences ();
      delete object(ii);
   }

   m_objects.clear ();
}

persistence::Object& persistence::Storage::load (dbms::Connection& connection, GuardClass& _class, persistence::Loader& loader)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   Object* result = NULL;

   const PrimaryKey& primaryKey (loader.getPrimaryKey());

   LOG_DEBUG (getName () << " | Loading=" << primaryKey);

   entry_iterator ii = m_objects.find (primaryKey);

   if (ii == m_objects.end ()) {
      Entry entry;

      m_faultCounter ++;

      try {
         entry.m_object = _class.createObject();
         dbms::ResultCode resultCode = loader.apply (connection, _class, std::ref (*entry.m_object));

         if (resultCode.successful() == false)
            WEPA_THROW_NAME_DB_EXCEPTION(loader.getName(), resultCode);

         entry.m_useCounter = 1;
         m_objects [primaryKey] = entry;
         std::pair <entry_iterator, bool> rr = m_objects.insert (std::make_pair(primaryKey, entry));
         entry.m_object->setPrimaryKey(Storage::primaryKey (rr.first));
         result = entry.m_object;
      }
      catch (adt::Exception& ex) {
         delete entry.m_object;
         throw;
      }
   }
   else {
      m_hitCounter ++;

      Entry& entry = Storage::entry(ii);
      try {
         result = instanciateAccessMode(m_accessMode).refresh(connection, _class, loader, entry);
         entry.m_useCounter ++;
      }
      catch (adt::Exception& ex) {
         logger::Logger::write(ex);
         delete entry.m_object;
         m_objects.erase(ii);
      }
   }

   if (result == NULL) {
      WEPA_THROW_EXCEPTION(loader << " | There is not any match for the loader");
   }

   LOG_DEBUG (loader << " | ObjectId=" << result->getInternalId() << " | Result=" << result->asString ());

   return std::ref (*result);
}

void persistence::Storage::save (dbms::Connection& connection, GuardClass& _class, Recorder& recorder)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   exceptionWhenReadOnly(recorder);

   Object &object = recorder.getObject();

   dbms::ResultCode resultCode = recorder.apply (connection, _class, object);

   if (resultCode.successful() == false)
      WEPA_THROW_NAME_DB_EXCEPTION(recorder.getName (), resultCode);

   LOG_DEBUG (recorder << " | ObjectId=" << object.getInternalId() << " | " << resultCode);
}

void persistence::Storage::erase (dbms::Connection& connection, GuardClass& _class, Eraser& eraser)
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   exceptionWhenReadOnly(eraser);

   Object& object = eraser.getObject();

   const PrimaryKey& primaryKey = object.getPrimaryKey();

   LOG_DEBUG (getName () << " | Erasing =" << primaryKey);

   entry_iterator ii = m_objects.find (primaryKey);

   if (ii == m_objects.end ()) {
      LOG_WARN(primaryKey << " is not loaded in storage '" << getName () << "'");
   }
   else {
      Entry& entry = Storage::entry(ii);

      if (entry.hasMultipleReferences () == true) {
         WEPA_THROW_EXCEPTION(eraser.getName () << " can not erase primary key " << primaryKey << " due to multiple references");
      }
   }

   const std::string internalId = object.getInternalId();

   dbms::ResultCode resultCode = eraser.apply (connection, _class, object);

   if (resultCode.successful() == false)
      WEPA_THROW_NAME_DB_EXCEPTION(eraser.getName (), resultCode);

   if (ii != m_objects.end ()) {
      Entry& entry = Storage::entry(ii);
      entry.m_useCounter = 1;
      release (_class, object);
   }

   LOG_DEBUG (eraser << " | ObjectId=" << internalId << " | " << resultCode);
}

bool persistence::Storage::release (GuardClass& _class, Object& object) noexcept
{
   bool result = false;

   try {
      const PrimaryKey& primaryKey = object.getPrimaryKey();

      LOG_DEBUG (getName () << " | Releasing=" << primaryKey);
      entry_iterator ii = m_objects.find (primaryKey);

      if (ii != m_objects.end ()) {
         Entry& entry = Storage::entry(ii);

         LOG_DEBUG (getName () << " | ObjectId=" << entry.m_object->getInternalId() << " | Uses=" << entry.m_useCounter);

         if (-- entry.m_useCounter <= 0) {
            object.clearPrimaryKey();
            delete entry.m_object;
            m_objects.erase (ii);
            result = true;
         }
      }
   }
   catch (adt::RuntimeException& ex) {
      LOG_ERROR (ex.asString ());
   }

   LOG_DEBUG (getName () << " | Result=" << result);

   return result;
}

adt::StreamString persistence::Storage::asString () const
   noexcept
{
   adt::StreamString result (adt::StreamString::Flag::ShowNull);

   result = "persistence.Storage {";
   result << adt::NamedObject::asString();
   result << " | Mode=" << AccessMode::enumName (m_accessMode);
   result << " | Hit=" << m_hitCounter;
   result << " | Fault=" << m_faultCounter;
   result << " | N-size=" << m_objects.size ();
   return result += " }";
}

xml::Node& persistence::Storage::asXML (xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild("persistence.Storage");

   result.createAttribute("Name", getName ());
   result.createAttribute ("Mode", AccessMode::enumName(m_accessMode));
   result.createAttribute ("N-size", m_objects.size ());

   xml::Node& access = result.createChild ("Access");
   access.createAttribute ("Hit", m_hitCounter);
   access.createAttribute ("Fault", m_faultCounter);

   return std::ref (result);
}

void persistence::Storage::exceptionWhenReadOnly (const Accessor& accessor) const
   throw (adt::RuntimeException)
{
   if (instanciateAccessMode(m_accessMode).canWrite() == false) {
      WEPA_THROW_EXCEPTION("Accessor '" << accessor.getName () << "' can not write on a read-only storage '" << getName () << "'");
   }
}

//static
const persistence::Storage::AccessMode& persistence::Storage::instanciateAccessMode (const AccessMode::_v accessMode)
   throw (adt::RuntimeException)
{
   static AccessModeReadOnly readOnly;
   static AccessModeReadWrite readWrite;

   static const AccessMode* st_accessMode [2] = { &readOnly, &readWrite };

   if (accessMode == AccessMode::None)
      WEPA_THROW_EXCEPTION("There is not define any AccessMode");

   return std::ref (*st_accessMode [accessMode]);
}

persistence::Object* persistence::Storage::AccessMode::reload (dbms::Connection& connection, GuardClass& _class, Loader& loader, Entry& entry) const
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   Object& object = std::ref (*entry.m_object);

   bool hasToRefresh = loader.hasToRefresh(connection, _class, object);

   LOG_DEBUG (object << " | HasToRefresh=" << hasToRefresh);

   if (hasToRefresh == true) {
      LOG_DEBUG ("Release dependences for " << entry.m_object->asString ());
      object.releaseDependences();

      LOG_DEBUG ("Reloading data for " << entry.m_object->asString ());
      dbms::ResultCode resultCode = loader.apply (connection, _class, object);

      if (resultCode.successful() == false)
         WEPA_THROW_NAME_DB_EXCEPTION(loader.getName(), resultCode);
   }

   LOG_DEBUG ("Result=" << entry.m_object->asString ());

   return entry.m_object;
}


persistence::Object* persistence::Storage::AccessModeReadOnly::refresh(dbms::Connection&, GuardClass& _class, persistence::Loader& loader, Storage::Entry& entry) const
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   LOG_DEBUG("Loader=" << loader.getName () << " | ObjectId=" << entry.m_object->getInternalId());
   return entry.m_object;
}

persistence::Object* persistence::Storage::AccessModeReadWrite::refresh(dbms::Connection& connection, GuardClass& _class, persistence::Loader& loader, Storage::Entry& entry) const
   throw (adt::RuntimeException, dbms::DatabaseException)
{
   persistence::Object* result = this->reload (connection,_class, loader, entry);
   LOG_DEBUG("Loader=" << loader.getName () << " | ObjectId=" << result->getInternalId());
   return result;
}

