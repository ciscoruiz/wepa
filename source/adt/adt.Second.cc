#include <time.h>
#include <sys/time.h>
#include <stdlib.h>
#include <stdio.h>

#include <coffee/config/defines.hpp>

#include <coffee/adt/AsString.hpp>
#include <coffee/adt/Second.hpp>
#include <coffee/adt/Millisecond.hpp>
#include <coffee/adt/Microsecond.hpp>

using namespace std;
using namespace coffee;

#define implement_operator(op) \
   bool adt::Second::operator op (const Millisecond& other) const \
      noexcept\
   {\
      return m_value op (other.m_value / 1000);\
   }\
   bool adt::Second::operator op (const Microsecond& other) const\
      noexcept\
   {\
      return m_value op (other.m_value / 1000000);\
   }

adt::Second::Second (const adt::Millisecond& other) : m_value (other.m_value / 1000) {;}

adt::Second::Second (const adt::Microsecond& other) : m_value (other.m_value / 1000000) {;}

adt::Second& adt::Second::operator= (const adt::Millisecond& other)
   noexcept
{
   m_value = (other.m_value / 1000);
   return *this;
}

adt::Second& adt::Second::operator= (const adt::Microsecond& other)
   noexcept
{
   m_value = (other.m_value / 1000000);
   return *this;
}

implement_operator (==)
implement_operator (!=)
implement_operator (>)
implement_operator (<)

string adt::Second::asDateTime (const char* format) const
   throw (RuntimeException)
{
   char aux [DateTimeSizeString];

   time_t time = m_value;

   struct tm* tt = localtime (&time);

   if (tt == nullptr) {
      COFFEE_THROW_EXCEPTION(m_value << " is a bad time");
   }

   if (strftime (aux, sizeof (aux), format, tt) == 0) {
      COFFEE_THROW_EXCEPTION(m_value << "is a bad date");
   }

   return string(aux);
}

//static
adt::Second adt::Second::getTime ()
   noexcept
{
   struct timeval tv;
   gettimeofday (&tv, NULL);
   return adt::Second (tv.tv_sec);
}

//static
adt::Second adt::Second::getLocalTime ()
   noexcept
{
   return adt::Second (time (NULL));
}

string adt::Second::asString () const
   noexcept
{
   string result (AsString::apply (m_value));
   return result += " sec";
}

//static
adt::Second adt::Second::fromString (const std::string& value)
   throw (adt::RuntimeException)
{
   if (value.find (" sec") == string::npos)
      COFFEE_THROW_EXCEPTION ("String: " << value << " is not a valid second expression");

   return adt::Second (atol (value.c_str()));
}

//static
adt::Second adt::Second::fromString(const std::string& text, const char* format)
   throw(RuntimeException)
{
   tm tt;
   const char* aux;

   coffee_memset(&tt, 0, sizeof(tt));
   if (strptime(text.c_str(), format, &tt) == NULL) {
      COFFEE_THROW_EXCEPTION(text << " is not valid expression for " << format);
   }

   return Second(mktime(&tt));
}

