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
#include <wepa/balance/Resource.hpp>
#include <wepa/balance/ResourceList.hpp>

using namespace wepa;

#include <mutex>

#include <wepa/adt/AsString.hpp>

#include <wepa/logger/Logger.hpp>
#include <wepa/logger/TraceMethod.hpp>

#include <wepa/xml/Node.hpp>
#include <wepa/xml/Attribute.hpp>

#include <wepa/balance/SCCS.hpp>

using namespace wepa;

void balance::ResourceList::initialize ()
   throw (adt::RuntimeException)
{
   LOG_THIS_METHOD();

   SCCS::activate();

   LockGuard guard(*this);

   for (resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
      try {
         resource(ii)->initialize();
         LOG_DEBUG(resource(ii)->asString());
      }
      catch (adt::RuntimeException& ex) {
         LOG_ERROR(resource (ii)->getName () << " | " << ex.asString());
      }
   }

   if (m_resources.empty ())
      LOG_WARN (asString() << " does not have any resource");

   if (countAvailableResources(guard) == 0)
      LOG_WARN (asString() << " does not have any available resource");
}

bool balance::ResourceList::add (const std::shared_ptr<Resource>& resource)
   throw (adt::RuntimeException)
{
    logger::TraceMethod tm (logger::Level::Local7, WEPA_FILE_LOCATION);

   if (!resource) {
      WEPA_THROW_EXCEPTION(asString() << " can not add an empty resource");
   }

   bool result = true;

   if (true) { // Minimize critical section
	   LockGuard guard(*this);

      for (const_resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
         if (ResourceList::resource(ii)->getName() == resource->getName()) {
            result = false;
            break;
         }
      }

      if (result == true) {
         m_resources.push_back (resource);
      }
   }

   LOG_INFO (asString() << " | Add: " << resource->asString() << " | Result=" << adt::AsString::apply (result));

   return result;
}

size_t balance::ResourceList::countAvailableResources (balance::ResourceList::LockGuard& guard) const
   noexcept
{
   size_t result = 0;

   for (const_resource_iterator ii = resource_begin(guard), maxii = resource_end(guard); ii != maxii; ++ ii) {
      if (resource(ii)->isAvailable())
         result ++;
   }

   return result;
}

balance::ResourceList::resource_iterator balance::ResourceList::next(balance::ResourceList::LockGuard& guard, balance::ResourceList::resource_iterator ii)
   noexcept
{
   ii ++;

   if (ii == this->resource_end(guard))
      ii = this->resource_begin(guard);

   return ii;
}

//virtual
adt::StreamString balance::ResourceList::asString () const
   noexcept
{
   adt::StreamString result ("balance.ResourceList { ");
   result += adt::NamedObject::asString();
   result += " | Available = ";

   LockGuard fakeGuard(*this);
   result += adt::AsString::apply(countAvailableResources(fakeGuard));
   result.append (" of ").append (adt::AsString::apply (size (fakeGuard)));
   return result.append (" }");
}

//virtual
xml::Node& balance::ResourceList::asXML (xml::Node& parent) const
   noexcept
{
   xml::Node& result = parent.createChild (this->getName());

   LockGuard fakeGuard(*this);
   for (const_resource_iterator ii = resource_begin(fakeGuard), maxii = resource_end(fakeGuard); ii != maxii; ++ ii) {
      resource(ii)->asXML(result);
   }

   return std::ref (result);
}
