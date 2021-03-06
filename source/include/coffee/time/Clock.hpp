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

#ifndef _coffee_time_Clock_hpp_
#define _coffee_time_Clock_hpp_

#include <memory>

#include <coffee/time/TimeEvent.hpp>

namespace coffee {
namespace time {

/**
 * Time event for a periodical time event.
 *
 * It will be re-scheduled every time the timeout has been reached.
 *
 * \include time/Clock_test.cc
 */
class Clock : public TimeEvent {
public:
   /**
    * Fast instantiation for this class
    */
   static std::shared_ptr<Clock> instantiate(const Id id, const std::chrono::milliseconds& timeout) noexcept {
      return std::make_shared<Clock>(id, timeout);
   }

   /**
    * Constructor.
    * \param id Unique identification for this event.
    * \param timeout It will be re-scheduled every time the timeout has been reached.
    */
   Clock(const Id id, const std::chrono::milliseconds& timeout) : TimeEvent(id, timeout) {;}

private:
   bool isPeriodical() const noexcept { return true; }
};

}
}

#endif /* _coffee_time_Clock_hpp_ */
