#include <coffee/xml/Node.hpp>
#include <coffee/xml/Attribute.hpp>

#include <coffee/adt/AsHexString.hpp>

#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>

#include <coffee/dbms/datatype/Abstract.hpp>

#include <coffee/persistence/Object.hpp>
#include <coffee/persistence/Loader.hpp>
#include <coffee/persistence/Recorder.hpp>
#include <coffee/persistence/Eraser.hpp>
#include <coffee/persistence/Storage.hpp>
#include <coffee/persistence/Class.hpp>

#include <coffee/dbms/GuardConnection.hpp>
#include <coffee/dbms/GuardStatement.hpp>

using namespace coffee;
using namespace coffee::persistence;

Storage::Storage(const std::string& name, const int maxCacheSize) :
   adt::NamedObject(name),
   m_cache(maxCacheSize)
{
   m_hitCounter = m_faultCounter = 0;
}

Storage::~Storage()
{
}

Accessor::TheObject Storage::load(Accessor::TheConnection& connection, Loader& loader)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   Accessor::TheObject result;

   const Accessor::ThePrimaryKey& primaryKey(loader.getPrimaryKey());

   LOG_DEBUG(asString() << " | Loading=" << primaryKey->asString());

   Cache::pair_iterator vv = m_cache.find(primaryKey);

   dbms::GuardConnection guardConnection(connection);
   dbms::GuardStatement guardStatement(guardConnection, loader.getStatement());

   if(vv == m_cache.end()) {
      m_faultCounter ++;

      result = loader.getClass()->createObject(primaryKey);

      dbms::ResultCode resultCode = loader.apply(guardStatement, result);

      if(resultCode.successful() == false)
         COFFEE_THROW_NAME_DB_EXCEPTION(loader.getName(), resultCode);

      m_cache.set(primaryKey, result);
   }
   else {
      m_hitCounter ++;

      result = m_cache.value(vv);

      if (loader.hasToRefresh(guardStatement, result)) {
         dbms::ResultCode resultCode = loader.apply(guardStatement, result);

         if(resultCode.successful() == false)
            COFFEE_THROW_NAME_DB_EXCEPTION(loader.getName(), resultCode);
      }
   }

   LOG_DEBUG(loader << " | ObjectId=" << result->getInternalId() << " | Result=" << result->asString());

   return result;
}

void Storage::save(Accessor::TheConnection& connection, Recorder& recorder)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();
   dbms::GuardConnection guardConnection(connection);
   save(guardConnection, recorder);
}

void Storage::save(dbms::GuardConnection& guardConnection, Recorder& recorder)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();

   dbms::GuardStatement guardStatement(guardConnection, recorder.getStatement());

   Accessor::TheObject object = recorder.getObject();

   dbms::ResultCode resultCode = recorder.apply(guardStatement);

   if(resultCode.successful() == false)
      COFFEE_THROW_NAME_DB_EXCEPTION(recorder.getName(), resultCode);

   if (recorder.hasAutoCommit()) {
      guardConnection.commit();
   }

   LOG_DEBUG(recorder << " | ObjectId=" << object->getInternalId() << " | " << resultCode);
}

void Storage::erase(Accessor::TheConnection& connection, Eraser& eraser)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   LOG_THIS_METHOD();
   dbms::GuardConnection guardConnection(connection);
   erase(guardConnection, eraser);
}

void Storage::erase(dbms::GuardConnection& guardConnection, Eraser& eraser)
   throw(adt::RuntimeException, dbms::DatabaseException)
{
   const Accessor::ThePrimaryKey& primaryKey(eraser.getPrimaryKey());

   LOG_DEBUG(asString() << " | Erasing =" << primaryKey->asString());
   
   if(!m_cache.erase(primaryKey)) {
      LOG_DEBUG(primaryKey->asString() << " is not loaded in storage '" << getName() << "'");
      m_faultCounter ++;
   }
   else {      
      m_hitCounter ++;
   }

   dbms::GuardStatement guardStatement(guardConnection, eraser.getStatement());

   dbms::ResultCode resultCode = eraser.apply(guardStatement);

   if(resultCode.successful() == false)
      COFFEE_THROW_NAME_DB_EXCEPTION(eraser.getName(), resultCode);

   if (eraser.hasAutoCommit()) {
      guardConnection.commit();
   }

   LOG_DEBUG(eraser << " | " << primaryKey->asString() << " | " << resultCode);
}

adt::StreamString Storage::asString() const
   noexcept
{
   adt::StreamString result(adt::StreamString::Flag::ShowNull);

   result = "persistence.Storage {";
   result << adt::NamedObject::asString();
   result << " | Hit=" << m_hitCounter;
   result << " | Fault=" << m_faultCounter;
   result << " | " << m_cache;
   return result += " }";
}

std::shared_ptr<xml::Node> Storage::asXML(std::shared_ptr<xml::Node>& parent) const
   noexcept
{
   std::shared_ptr<xml::Node> result = parent->createChild("persistence.Storage");

   result->createAttribute("Name", adt::NamedObject::getName());
   std::shared_ptr<xml::Node> access = result->createChild("Access");
   access->createAttribute("Hit", m_hitCounter);
   access->createAttribute("Fault", m_faultCounter);

   std::shared_ptr<xml::Node> cache = result->createChild("Cache(LRU)");
   access->createAttribute("Size", adt::AsString::apply((int) m_cache.size()));

   return result;
}

