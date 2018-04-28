// MIT License
//
// Copyright(c) 2018 Francisco Ruiz(francisco.ruiz.rayo@gmail.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files(the "Software"), to deal
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

#include <boost/test/unit_test.hpp>

#include <coffee/http/HttpHeader.hpp>
#include <coffee/http/HttpCustomHeader.hpp>

using namespace coffee;

using coffee::http::HttpHeader;

BOOST_AUTO_TEST_CASE( header_predefined_encode )
{
   HttpHeader header(HttpHeader::Type::LastModified);
   header.setValue("my value");
   BOOST_REQUIRE_EQUAL(header.encode(), "Last-Modified:my value");
}

BOOST_AUTO_TEST_CASE( header_instantiate_encode )
{
   auto header = HttpHeader::instantiate(HttpHeader::Type::AcceptLanguage);
   header->setValue("havax");
   basis::StreamString ss(HttpHeader::Type::asString(HttpHeader::Type::AcceptLanguage));
   BOOST_REQUIRE_EQUAL(header->encode(), ss << ":havax");
}

BOOST_AUTO_TEST_CASE( header_exception_encode )
{
   HttpHeader header(http::HttpHeader::Type::Custom);
   header.setValue("my value");
   BOOST_REQUIRE_THROW(header.encode(), basis::RuntimeException);
}

BOOST_AUTO_TEST_CASE( header_custom_encode )
{
   auto header = http::HttpCustomHeader::instantiate("MyCustomHeader");
   BOOST_REQUIRE(header->getType() == HttpHeader::Type::Custom);
   header->setValue("my value");
   BOOST_REQUIRE_EQUAL(header->encode(), "MyCustomHeader:my value");
}

BOOST_AUTO_TEST_CASE( header_multivalue )
{
   auto header = http::HttpCustomHeader::instantiate("MyCustomHeader");
   BOOST_REQUIRE(header->getType() == HttpHeader::Type::Custom);
   header->setValue(123);
   header->setValue("after");
   BOOST_REQUIRE_EQUAL(header->encode(), "MyCustomHeader:123,after");
}