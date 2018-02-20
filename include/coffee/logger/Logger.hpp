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

#ifndef coffee_logger_Logger_hpp
#define coffee_logger_Logger_hpp

#include <memory>
#include <vector>

#include <coffee/logger/Level.hpp>

#include <coffee/adt/RuntimeException.hpp>

namespace coffee {

namespace adt {
   class StreamString;
}

namespace logger {

class Writer;
class Formatter;

class Logger {
   typedef std::vector<std::shared_ptr<Writer> > Writers;
   typedef Writers::iterator writer_iterator;

public:
   static void initialize(std::shared_ptr<Writer> writer, std::shared_ptr<Formatter> formatter) throw(adt::RuntimeException);
   static void initialize(std::shared_ptr<Writer> writer) throw(adt::RuntimeException);
   static void add(std::shared_ptr<Writer> writer) throw (adt::RuntimeException);

   static void write(const Level::_v level, const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept;

   static void setLevel(const Level::_v level) noexcept { m_level = level; }
   static Level::_v getLevel() noexcept { return m_level; }

   static bool isActive(const Level::_v level) noexcept { return(level <= Level::Error) ? true:(level <= m_level); }
   static bool wantsToProcess(const Level::_v level) noexcept;

   static void critical(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Critical, streamString, function, file, line); }
   static void error(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Error, streamString, function, file, line); }
   static void warning(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Warning, streamString, function, file, line); }
   static void notice(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Notice, streamString, function, file, line); }
   static void info(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Information, streamString, function, file, line); }
   static void debug(const adt::StreamString& streamString, const char* function, const char* file, const unsigned line) noexcept { write(Level::Debug, streamString, function, file, line); }

   static void write(const adt::Exception& ex) {
      error(ex.what(), ex.getMethod(), ex.getFile(), ex.getLine());
   }

private:
   static Level::_v m_level;
   static Writers m_writers;
   static std::shared_ptr<Formatter> m_formatter;

   Logger() = delete;
   Logger(const Logger&) = delete;
};

#ifdef COFFEE_LOG_LOCATION
#undef COFFEE_LOG_LOCATION
#endif
#define COFFEE_LOG_LOCATION __func__,__FILE__,__LINE__

#define LOG_CRITICAL(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Critical)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::critical(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_ERROR(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Error)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::error(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_WARN(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Warning)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::warning(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_NOTICE(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Notice)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::notice(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_INFO(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Information)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::info(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_DEBUG(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Debug)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::debug(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

#define LOG_LOCAL7(args)\
   do {\
   if(coffee::logger::Logger::wantsToProcess(coffee::logger::Level::Local7)) { \
      coffee::adt::StreamString msg; \
      coffee::logger::Logger::debug(msg << args, COFFEE_LOG_LOCATION); \
   } \
   } while(false);

}
}

#endif
