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

#include <chrono>

#include <coffee/basis/pattern/observer/Subject.hpp>
#include <coffee/basis/pattern/observer/Event.hpp>
#include <coffee/basis/pattern/observer/Observer.hpp>

#include <gtest/gtest.h>

using namespace std;
using namespace coffee::basis;
using namespace coffee::basis::pattern;

using std::chrono::milliseconds;

const int iiInitialValue = 55;
const int uuInitialValue = 88;
const char* ssInitialValue = "MyFirstValue";
const char* finalValue = "Cleared";

class TheSubject : public observer::Subject {
public:
   struct Events { enum _v { ChangeII = 0, ChangeSS, ChangeUU, Clean}; };

   TheSubject() : observer::Subject("TheSubject") {
      m_ii = iiInitialValue;
      m_uu = uuInitialValue;
      m_ss = ssInitialValue;
      m_events[Events::ChangeII] = observer::Event(Events::ChangeII);
      m_events[Events::ChangeSS] = observer::Event(Events::ChangeSS);
      m_events[Events::ChangeUU] = observer::Event(Events::ChangeUU);
   }

   const observer::Event& getEvent(const Events::_v nn) const noexcept { return m_events[nn]; }

   void setII(const int ii) noexcept { m_ii = ii; notify(m_events[Events::ChangeII]); }
   void setSS(const std::string& ss) { m_ss = ss; notify(m_events[Events::ChangeSS]); }
   void setUU(const unsigned int uu) noexcept { m_uu = uu; notify(m_events[Events::ChangeUU]); }

   int getII() const noexcept { return m_ii; }
   const std::string& getSS() const noexcept { return m_ss; }
   unsigned int getUU() const noexcept { return m_uu; }

   void clear() {
      m_ii = iiInitialValue;
      m_ss = finalValue;
      m_uu = uuInitialValue;
      notify(observer::Event(Events::Clean));
   }

private:
   observer::Event m_events[3];
   int m_ii;
   std::string m_ss;
   unsigned m_uu;
};

class TheObserverII : public observer::Observer {
public:
   TheObserverII() : observer::Observer("TheObserverII"), m_value(0) {}
   int getValue() const noexcept { return m_value; }

private:
   int m_value;

   void attached(const observer::Subject& subject) noexcept {
      m_value = static_cast<const TheSubject&>(subject).getII();
   }

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      if (event.getId() == TheSubject::Events::ChangeII) {
         m_value = static_cast<const TheSubject&>(subject).getII();
      }
   }
};

class TheObserverSS : public observer::Observer {
public:
   TheObserverSS() : observer::Observer("TheObserverSS") {}

   const std::string& getValue() const noexcept { return m_value; }

private:
   std::string m_value;

   void attached(const observer::Subject& subject) noexcept {
      m_value = static_cast<const TheSubject&>(subject).getSS();
   }

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      if (event.getId() == TheSubject::Events::ChangeSS) {
         m_value = static_cast<const TheSubject&>(subject).getSS();
      }
   }
};

class ObserverUUAndII : public observer::Observer {
public:
   ObserverUUAndII() : observer::Observer("ObserverUUAndII"), m_ii(0), m_uu(0) {}

   unsigned int sumUpUUAndII() const noexcept { return m_uu + m_ii; }

private:
   int m_ii;
   unsigned m_uu;

   void attached(const observer::Subject& subject) noexcept {
      m_ii = static_cast<const TheSubject&>(subject).getII();
      m_uu = static_cast<const TheSubject&>(subject).getUU();
   }

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      if (event.getId() == TheSubject::Events::ChangeUU || event.getId() == TheSubject::Events::ChangeII) {
         auto mySubject = static_cast<const TheSubject&>(subject);
         m_uu =  mySubject.getUU();
         m_ii = mySubject.getII();
      }
   }
};

class TimedObserver : public observer::Observer {
public:
   TimedObserver() : observer::Observer("OptimalObserver") {}

   int getII() const noexcept { return m_ii.first; }
   const milliseconds& getIITime() { return m_ii.second; }

   unsigned int getUU() const noexcept { return m_uu.first; }
   const milliseconds& getUUTime() { return m_uu.second; }

private:
   pair<int, milliseconds> m_ii;
   pair<unsigned,milliseconds> m_uu;

   void attached(const observer::Subject& subject) noexcept {
      const TheSubject& mySubject = static_cast<const TheSubject&>(subject);

      m_uu.first =  mySubject.getUU();
      m_uu.second = getTime();

      m_ii.first = mySubject.getII();
      m_ii.second = getTime();
   }

   void update(const observer::Subject& subject, const observer::Event& event) noexcept {
      const TheSubject& mySubject = static_cast<const TheSubject&>(subject);

      if (event.getId() == TheSubject::Events::ChangeUU) {
         m_uu.first =  mySubject.getUU();
         m_uu.second = getTime();
      }
      if (event.getId() == TheSubject::Events::ChangeII) {
         m_ii.first = mySubject.getII();
         m_ii.second = getTime();
      }
   }

   static milliseconds getTime() {
      // See https://stackoverflow.com/questions/9089842/c-chrono-system-time-in-milliseconds-time-operations
      return std::chrono::duration_cast<milliseconds>(std::chrono::system_clock::now().time_since_epoch());
   }
};

TEST(PatternObserverTest,by_event)
{
   TheSubject subject;
   shared_ptr<TheObserverII> observer = make_shared<TheObserverII>();

   subject.attach(observer);

   ASSERT_EQ(iiInitialValue, observer->getValue());

   subject.setII(100);

   ASSERT_EQ(100, observer->getValue());
}

TEST(PatternObserverTest,initial_value)
{
   TheSubject subject;
   shared_ptr<TheObserverSS> observer = make_shared<TheObserverSS>();

   subject.attach(observer);

   ASSERT_EQ(ssInitialValue, observer->getValue());

   subject.setSS("Wow!");

   ASSERT_EQ("Wow!", observer->getValue());
}

class NullObserver : public observer::Observer {
public:
   NullObserver() : observer::Observer("NullObserver") {;}
   void attached(const observer::Subject& subject) noexcept {}
   void update(const observer::Subject& subject, const observer::Event& event) noexcept {}
};

TEST(PatternObserverTest,detach)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   TheSubject subject;
   subject.attach(observer);
   ASSERT_TRUE(observer->isSubscribed());
   ASSERT_TRUE(subject.detach(observer->getName()));
   ASSERT_FALSE(observer->isSubscribed());
}

TEST(PatternObserverTest,isnot_attached)
{
   TheSubject subject;
   ASSERT_FALSE(subject.detach("undefinedObserver"));
}

TEST(PatternObserverTest,repeat_attach)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   TheSubject subject;
   subject.attach(observer);
   ASSERT_TRUE(subject.countObservers() == 1);

   subject.attach(observer);
   ASSERT_TRUE(subject.countObservers() == 1);
}

TEST(PatternObserverTest,compare_both)
{
   TheSubject subject;
   shared_ptr<TheObserverII> iiObserver = make_shared<TheObserverII>();

   subject.attach(iiObserver);

   shared_ptr<ObserverUUAndII> uuiiObserver = make_shared<ObserverUUAndII>();
   subject.attach(uuiiObserver);

   ASSERT_EQ(uuiiObserver->sumUpUUAndII(), uuInitialValue + iiInitialValue);

   subject.setII(100);

   ASSERT_EQ(100, iiObserver->getValue());
   ASSERT_EQ(uuInitialValue + 100, uuiiObserver->sumUpUUAndII());

   subject.setUU(10);

   ASSERT_EQ(100, iiObserver->getValue());
   ASSERT_EQ(10 + 100, uuiiObserver->sumUpUUAndII());
}

TEST(PatternObserverTest, optimal_subscription)
{
   TheSubject subject;
   shared_ptr<TimedObserver> observer = make_shared<TimedObserver>();

   subject.attach(observer);

   subject.setII(100);

   usleep(1100);

   subject.setUU(3000);

   ASSERT_EQ(100, observer->getII());
   ASSERT_EQ(3000, observer->getUU());
   ASSERT_GT(observer->getUUTime().count(), observer->getIITime().count());
}

TEST(PatternObserverTest, promicuous_observer)
{
   TheSubject subject1;
   TheSubject subject2;

   shared_ptr<NullObserver> observer = make_shared<NullObserver>();
   subject1.attach(observer);

   ASSERT_THROW(subject2.attach(observer), coffee::basis::RuntimeException);
}

TEST(PatternObserverTest,out_of_scope)
{
   shared_ptr<NullObserver> observer = make_shared<NullObserver>();

   if (true) {
      TheSubject subject;
      subject.attach(observer);
      ASSERT_EQ(subject.countObservers(), 1);
      ASSERT_TRUE(observer->isSubscribed());
   }

   ASSERT_FALSE(observer->isSubscribed());
}

TEST(PatternObserverTest,asstring)
{
   TheSubject subject;
   shared_ptr<TheObserverII> iiObserver = make_shared<TheObserverII>();
   shared_ptr<ObserverUUAndII> uuiiObserver = make_shared<ObserverUUAndII>();

   subject.attach(iiObserver);
   subject.attach(uuiiObserver);

   ASSERT_TRUE(subject.asString().find("#Observers=2") != std::string::npos);
}
