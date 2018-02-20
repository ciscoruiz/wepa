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

#include <iostream>
#include <time.h>

#include <boost/test/unit_test.hpp>

#include <coffee/dbms/datatype/Date.hpp>
#include <coffee/dbms/datatype/Integer.hpp>

using namespace coffee;
using namespace coffee::dbms;


// See http://www.mbari.org/staff/rich/utccalc.htm
BOOST_AUTO_TEST_CASE(date_setter_second)
{
   adt::Second date_01_31_1987_23_59_59(539110799);
   datatype::Date column("from_number");

   column.setValue(date_01_31_1987_23_59_59);

   tm* localTime = column.getLocalTime();

   BOOST_REQUIRE_EQUAL(localTime->tm_mday, 31);
   BOOST_REQUIRE_EQUAL(localTime->tm_mon, 0);
   BOOST_REQUIRE_EQUAL(localTime->tm_year, 1987 - 1900);
   BOOST_REQUIRE_EQUAL(localTime->tm_min, 59);
   BOOST_REQUIRE_EQUAL(localTime->tm_sec, 59);

   BOOST_REQUIRE_EQUAL(column.getValue(), 539110799);
}

BOOST_AUTO_TEST_CASE(date_setter_text)
{
   datatype::Date column("from_text");

   std::string str_date("31/01/1996T22:17:10");

   try {
      column.setValue(str_date, "%d/%m/%YT%T");

      tm* localTime = column.getLocalTime();

      BOOST_REQUIRE_EQUAL(localTime->tm_mday, 31);
      BOOST_REQUIRE_EQUAL(localTime->tm_mon, 0);
      BOOST_REQUIRE_EQUAL(localTime->tm_year, 1996 - 1900);
      BOOST_REQUIRE_EQUAL(localTime->tm_hour, 22); // This test was written under GMT+1
      BOOST_REQUIRE_EQUAL(localTime->tm_min, 17);
      BOOST_REQUIRE_EQUAL(localTime->tm_sec, 10);
   }
   catch(adt::RuntimeException& ex) {
      std::cout << ex.what() << std::endl;
   }

   BOOST_REQUIRE_THROW(column.setValue("12:10", "%d/%m/%YT%T"), adt::RuntimeException);
}

BOOST_AUTO_TEST_CASE(date_is_nulleable)
{
   datatype::Date column("date_is_nulleable", datatype::Constraint::CanBeNull);

   const char* format = "%d/%m/%YT%H:%M:%S";

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   std::string str_date("01/01/1800T12:30:50");
   column.setValue(str_date, format);
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   BOOST_REQUIRE_NO_THROW(column.setValue("01/01/2000T00:00:00", format));
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.clear();
   BOOST_REQUIRE_THROW(column.getValue(), adt::RuntimeException);
   BOOST_REQUIRE_THROW(column.getLocalTime(), adt::RuntimeException);

   str_date = "25/10/2013T02:00:10";

   column.setValue(str_date, "%d/%m/%YT%H:%M:%S");
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   const tm* time_t = column.getLocalTime();
   BOOST_REQUIRE_EQUAL(time_t->tm_year, 2013 - 1900);
   BOOST_REQUIRE_EQUAL(time_t->tm_mon, 9);
   BOOST_REQUIRE_EQUAL(time_t->tm_mday, 25);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
}

BOOST_AUTO_TEST_CASE(date_is_not_nulleable)
{
   datatype::Date column("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   BOOST_REQUIRE_THROW(column.isNull(), adt::RuntimeException);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   BOOST_REQUIRE_EQUAL(column.getValue(), 0);
}

BOOST_AUTO_TEST_CASE(date_downcast)
{
   datatype::Date column("not_nulleable", datatype::Constraint::CanNotBeNull);

   datatype::Abstract& abs = column;

   BOOST_REQUIRE_EQUAL(abs.hasValue(), true);

   const datatype::Date& other = coffee_datatype_downcast(datatype::Date, abs);

   const char* format = "%d/%m/%YT%H:%M:%S";
   std::string str_date("01/01/1800T12:30:50");
   column.setValue(str_date, format);

   BOOST_REQUIRE_EQUAL(other == column, true);

   datatype::Integer zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::Date, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(date_clone)
{
   datatype::Date cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::Date canBeNull("canBeNull", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(cannotBeNull.hasValue(), true);
   BOOST_REQUIRE_EQUAL(canBeNull.hasValue(), false);

   std::shared_ptr<datatype::Abstract> notnull(cannotBeNull.clone());
   std::shared_ptr<datatype::Abstract> null(canBeNull.clone());

   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->hasValue(), false);

   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   cannotBeNull.setValue(adt::Second(5));

   BOOST_REQUIRE_EQUAL(cannotBeNull.getValue(), adt::Second(5));

   notnull = cannotBeNull.clone();
   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   canBeNull.setValue(adt::Second(25));
   null = canBeNull.clone();
   BOOST_REQUIRE_EQUAL(null->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->compare(canBeNull), 0);

   BOOST_REQUIRE_EQUAL(null->compare(cannotBeNull), 20);

   BOOST_REQUIRE_EQUAL(notnull->compare(canBeNull), -20);
}

