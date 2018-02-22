// MIT License
// 
// Copyright (c) 2018 Francisco Ruiz (francisco.ruiz.rayo@gmail.com)
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//

#include <coffee/adt/pattern/observer/Subject.hpp>
#include <coffee/adt/pattern/observer/Observer.hpp>
#include <coffee/logger/Logger.hpp>

using namespace coffee::adt::pattern;

//static
const observer::Event observer::Subject::AllEvents(-1, 0xffffffff);

observer::Subject::~Subject()
{
   for (auto ii = observer_begin(); ii != observer_end(); ++ ii) {
      get_observer(ii)->isSubscribed(false);
   }
   m_observers.clear();
}

void observer::Subject::attach(std::shared_ptr<Observer> observer, const Event& event)
   throw(RuntimeException)
{
   const std::string& observerName = observer->getName();

   if (m_observers.find(observerName) != observer_end()) {
      return;
   }

   if (observer->isSubscribed()) {
      COFFEE_THROW_EXCEPTION(observer->asString() << " already subscribed");
   }

   m_observers.insert(std::make_pair(observerName, Subscription(observer, event.getBitMask())));
   observer->isSubscribed(true);

   // Put status in common
   observer->update(*this, event);
}

bool observer::Subject::attach(const std::string& observerName)
   noexcept
{
   const auto ii = m_observers.find(observerName);

   if (ii == m_observers.end())
      return false;

   m_observers.erase(ii);

   return true;
}

observer::Event observer::Subject::registerEvent(const Event::Id id)
   throw(RuntimeException)
{
   if (m_events.size() >= sizeof(Event::BitMask) * 8) {
      COFFEE_THROW_EXCEPTION("Too many events for " << asString());
   }

   Event search(id, 0);
   event_iterator ii;

   if ((ii = m_events.find(search)) != event_end())
      return event(ii);

   Event newEvent(id, 1 << m_events.size());

   m_events.insert(newEvent);

   return newEvent;
}

observer::Event observer::Subject::lookupEvent(const Event::Id id) const
   throw(RuntimeException)
{
   Event search(id, 0);
   const_event_iterator ii;

   if ((ii = m_events.find(search)) != event_end())
      return event(ii);

   COFFEE_THROW_EXCEPTION("EventId=" << id << " was not registered");

   // To avoid warning about no return value
   return search;
}

void observer::Subject::notify(const Event& event)
   noexcept
{
   for(auto ii = observer_begin(), maxii = observer_end(); ii != maxii; ++ ii) {
      Event::BitMask bitmask = Subject::bitmask(ii);

      if ((bitmask & event.getBitMask()) != 0)
         observer::Subject::get_observer(ii)->update(*this, event);
   }
}
