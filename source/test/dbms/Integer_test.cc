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

#include <coffee/dbms/datatype/Integer.hpp>
#include <coffee/dbms/datatype/Float.hpp>

using namespace coffee;
using namespace coffee::dbms;


BOOST_AUTO_TEST_CASE(integer_is_nulleable)
{
   datatype::Integer column("nulleable", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   column.clear();

   BOOST_REQUIRE_EQUAL(column.hasValue(), false);

   BOOST_REQUIRE_THROW(column.getValue(), basis::RuntimeException);

   column.setValue(10);
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getValue(),10);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), false);
}

BOOST_AUTO_TEST_CASE(integer_is_not_nulleable)
{
   datatype::Integer column("not_nulleable", datatype::Constraint::CanNotBeNull);

   BOOST_REQUIRE_EQUAL(column.hasValue(), true);

   column.setValue(11);
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
   BOOST_REQUIRE_EQUAL(column.getValue(), 11);

   column.clear();
   BOOST_REQUIRE_EQUAL(column.hasValue(), true);
}

BOOST_AUTO_TEST_CASE(integer_downcast)
{
   datatype::Integer column("integer_downcast");

   datatype::Abstract& abs = column;

   const datatype::Integer& other = coffee_datatype_downcast(datatype::Integer, abs);
   column.setValue(1234);

   BOOST_REQUIRE_EQUAL(other == column, true);

   datatype::Float zzz("zzz");

   BOOST_REQUIRE_THROW(coffee_datatype_downcast(datatype::Integer, zzz), dbms::InvalidDataException);
}

BOOST_AUTO_TEST_CASE(integer_clone)
{
   datatype::Integer cannotBeNull("cannotBeNull", datatype::Constraint::CanNotBeNull);
   datatype::Integer canBeNull("canBeNull", datatype::Constraint::CanBeNull);

   BOOST_REQUIRE_EQUAL(cannotBeNull.hasValue(), true);
   BOOST_REQUIRE_EQUAL(canBeNull.hasValue(), false);

   auto notnull(cannotBeNull.clone());
   auto null(canBeNull.clone());

   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->hasValue(), false);

   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   cannotBeNull.setValue(5.0);

   BOOST_REQUIRE_EQUAL(cannotBeNull.getValue(), 5.0);

   notnull = cannotBeNull.clone();
   BOOST_REQUIRE_EQUAL(notnull->hasValue(), true);
   BOOST_REQUIRE_EQUAL(notnull->compare(cannotBeNull), 0);

   canBeNull.setValue(25);
   null = canBeNull.clone();
   BOOST_REQUIRE_EQUAL(null->hasValue(), true);
   BOOST_REQUIRE_EQUAL(null->compare(canBeNull), 0);

   BOOST_REQUIRE_EQUAL(null->compare(cannotBeNull), 20);

   BOOST_REQUIRE_EQUAL(notnull->compare(canBeNull), -20);
}

BOOST_AUTO_TEST_CASE(integer_instantiate) {
   auto data = datatype::Integer::instantiate("nulleable");
   BOOST_REQUIRE(data->hasValue());

   data = datatype::Integer::instantiate("not-nulleable", datatype::Constraint::CanBeNull);
   BOOST_REQUIRE(!data->hasValue());
}