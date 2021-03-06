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

#include <gtest/gtest.h>

#include <csignal>

#include <coffee/app/ApplicationServiceStarter.hpp>
#include <coffee/http/HttpClient.hpp>
#include <coffee/http/HttpRequest.hpp>
#include <coffee/http/HttpResponse.hpp>
#include <coffee/http/HttpService.hpp>
#include <coffee/http/HttpServlet.hpp>
#include <coffee/http/url/URLParser.hpp>
#include <coffee/logger/Logger.hpp>
#include <coffee/logger/TraceMethod.hpp>
#include <coffee/logger/UnlimitedTraceWriter.hpp>
#include <coffee/networking/NetworkingService.hpp>
#include <coffee/networking/SocketArguments.hpp>
#include <coffee/xml/Attribute.hpp>
#include <coffee/xml/Document.hpp>
#include <coffee/xml/Node.hpp>

using namespace coffee;

struct HttpServiceFixtureTest : ::testing::Test {
   HttpServiceFixtureTest() : app("TestAppHttpServiceFixtureTest") {;}


   void SetUp();
   void TearDown();

   static void parallelRun(coffee::app::Application& app) {
      app.start();
   }

   app::ApplicationServiceStarter app;
   std::shared_ptr<networking::NetworkingService> networkingService;
   std::shared_ptr<http::HttpService> httpService;
   std::shared_ptr<http::HttpClient> httpClient;
   std::thread thr;
};

class UpperServlet : public http::HttpServlet {
public:
   std::shared_ptr<http::HttpResponse> service(const std::shared_ptr<http::HttpRequest>& request) throw(basis::RuntimeException);
};

class LowerServlet : public http::HttpServlet {
public:
   std::shared_ptr<http::HttpResponse> service(const std::shared_ptr<http::HttpRequest>& request) throw(basis::RuntimeException);
};

class EchoServlet : public http::HttpServlet {
public:
   std::shared_ptr<http::HttpResponse> service(const std::shared_ptr<http::HttpRequest>& request) throw(basis::RuntimeException);
};

void HttpServiceFixtureTest::SetUp()
{
   const char* logFileName = "source/test/http/trace.log";
   unlink (logFileName);
   logger::Logger::initialize(std::make_shared<logger::UnlimitedTraceWriter>(logFileName));
//   logger::Logger::initialize(std::make_shared<logger::TtyWriter>());
   logger::Logger::setLevel(logger::Level::Debug);

   networkingService = networking::NetworkingService::instantiate(app);
   ASSERT_TRUE(networkingService != nullptr);

   httpService = http::HttpService::instantiate(app, networkingService);
   ASSERT_TRUE(httpService != nullptr);

   http::url::URLParser parser("http://127.0.0.1:5555");
   ASSERT_NO_THROW(httpService->createServer(parser.build()));

   ASSERT_NO_THROW(httpService->registerServlet("/upper", std::make_shared<UpperServlet>()));
   ASSERT_NO_THROW(httpService->registerServlet<LowerServlet>("/lower"));
   ASSERT_NO_THROW(httpService->registerServlet<EchoServlet>("/echo"));

   {
      http::url::URLParser parser("http://localhost:5555");
      httpClient = httpService->createClient(parser.build());
   }
   ASSERT_TRUE(httpClient != nullptr);

   thr = std::thread(parallelRun, std::ref(app));
   app.waitUntilRunning();
   networkingService->waitEffectiveRunning();
}

void HttpServiceFixtureTest::TearDown()
{
   LOG_THIS_METHOD();
   app.stop();
   thr.join();
}

std::shared_ptr<http::HttpResponse> UpperServlet::service(const std::shared_ptr<http::HttpRequest>& request)
   throw(basis::RuntimeException)
{
   auto response = http::HttpResponse::instantiate(request);

   if (!request->hasBody()) {
      response->setStatusCode(400);
      return response;
   }

   basis::DataBlock body(request->getBody());
   std::transform(body.begin(), body.end(), body.begin(), [](unsigned char cc) { return std::toupper(cc); });

   response->setBody(body);
   return response;
}

std::shared_ptr<http::HttpResponse> LowerServlet::service(const std::shared_ptr<http::HttpRequest>& request)
   throw(basis::RuntimeException)
{
   basis::DataBlock body(request->getBody());
   std::transform(body.begin(), body.end(), body.begin(), [](unsigned char cc) { return std::tolower(cc); });

   auto response = http::HttpResponse::instantiate(request);
   response->setBody(body);
   return response;
}

std::shared_ptr<http::HttpResponse> EchoServlet::service(const std::shared_ptr<http::HttpRequest>& request)
   throw(basis::RuntimeException)
{
   auto response = http::HttpResponse::instantiate(request);
   response->setBody(request->getBody());
   return response;
}

TEST_F(HttpServiceFixtureTest, http_service_send_response)
{
   {
      auto request = http::HttpRequest::instantiate(http::HttpRequest::Method::Get, "/upper");
      request->setBody("hello world");
      auto response = httpClient->send(request);

      ASSERT_TRUE(response->isOk());
      ASSERT_TRUE(response->hasBody());
      ASSERT_EQ("HELLO WORLD", response->getBody());
   }

   {
      auto request = http::HttpRequest::instantiate(http::HttpRequest::Method::Get, "/lower");
      request->setBody("HELLO WORLD");
      auto response = httpClient->send(request);

      ASSERT_TRUE(response->isOk());
      ASSERT_TRUE(response->hasBody());
      ASSERT_EQ("hello world", response->getBody());
   }

   {
      auto request = http::HttpRequest::instantiate(http::HttpRequest::Method::Get, "/echo");
      request->setBody("Some Text");
      auto response = httpClient->send(request);

      ASSERT_TRUE(response->isOk());
      ASSERT_TRUE(response->hasBody());
      ASSERT_EQ("Some Text", response->getBody());
   }
}

TEST_F(HttpServiceFixtureTest, http_service_send_empty)
{
   auto request = http::HttpRequest::instantiate(http::HttpRequest::Method::Get, "/upper");
   auto response = httpClient->send(request);

   ASSERT_TRUE(!response->isOk());
   ASSERT_TRUE(!response->hasBody());
   ASSERT_EQ(400, response->getStatusCode());
}

TEST_F(HttpServiceFixtureTest, http_service_repeat_path)
{
   ASSERT_THROW(httpService->registerServlet<EchoServlet>("/lower"), basis::RuntimeException);
}

TEST_F(HttpServiceFixtureTest, http_service_nopath)
{
   ASSERT_THROW(httpService->findServlet("/non-exist"), basis::RuntimeException);
}

TEST_F(HttpServiceFixtureTest, http_service_send_unknow_path)
{
   auto request = http::HttpRequest::instantiate(http::HttpRequest::Method::Get, "/non-exist");
   auto response = httpClient->send(request);
   ASSERT_TRUE(!response->isOk());
   ASSERT_TRUE(!response->hasBody());
   ASSERT_EQ(404, response->getStatusCode());
}

class ThrowServlet : public http::HttpServlet {
public:
   std::shared_ptr<http::HttpResponse> service(const std::shared_ptr<http::HttpRequest>& request)
   throw(basis::RuntimeException)
   {
      COFFEE_THROW_EXCEPTION("This operation failed");
   }
};

TEST_F(HttpServiceFixtureTest, http_service_fails)
{
   ASSERT_NO_THROW(httpService->registerServlet<ThrowServlet>("/throw"));

   auto response = httpClient->send(http::HttpRequest::instantiate(http::HttpRequest::Method::Get, "/throw"));
   ASSERT_TRUE(!response->isOk());
   ASSERT_TRUE(!response->hasBody());
   ASSERT_EQ(500, response->getStatusCode());
   ASSERT_EQ("This operation failed", response->getErrorDescription());
}

TEST_F(HttpServiceFixtureTest, http_service_no_http) {
   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
   ASSERT_TRUE(clientSocket != nullptr);
   basis::DataBlock request("this is not a HTTP message");

   http::protocol::HttpProtocolDecoder decoder;
   auto response = std::dynamic_pointer_cast<http::HttpResponse>(decoder.apply(clientSocket->send(request)));

   ASSERT_TRUE(!response->isOk());
   ASSERT_TRUE(!response->hasBody());
   ASSERT_EQ(400, response->getStatusCode());
   ASSERT_EQ("Unable to allocate an HTTP message", response->getErrorDescription());
}

TEST_F(HttpServiceFixtureTest, http_service_request_with_response) {
   networking::SocketArguments arguments;
   auto clientSocket = networkingService->createClientSocket(arguments.addEndPoint("tcp://localhost:5555"));
   ASSERT_TRUE(clientSocket != nullptr);

   auto notRequest = http::HttpResponse::instantiate(1, 1, 200, "OK");
   http::protocol::HttpProtocolEncoder encoder;

   http::protocol::HttpProtocolDecoder decoder;
   auto response = std::dynamic_pointer_cast<http::HttpResponse>(decoder.apply(clientSocket->send(encoder.apply(notRequest))));

   ASSERT_TRUE(!response->isOk());
   ASSERT_TRUE(!response->hasBody());
   ASSERT_EQ(400, response->getStatusCode());
   ASSERT_EQ("HTTP server can not work on HTTP responses", response->getErrorDescription());
}


TEST_F(HttpServiceFixtureTest, networking_write_xml)
{
   app.setOutputContextFilename("source/test/http/context.xml");

   std::raise(SIGUSR1);

   xml::Document document;

   ASSERT_NO_THROW(document.parse(app.getOutputContextFilename()));

   auto root = document.getRoot();

   auto app = root->lookupChild("app.Application");
   auto services = app->lookupChild("Services");

   auto node = services->childAt(0);
   ASSERT_TRUE(node != nullptr);

   node = node->lookupChild("app.Service");
   ASSERT_TRUE(node != nullptr);

   ASSERT_EQ(node->lookupChild("Runnable")->lookupAttribute("Name")->getValue(), "Networking:ZeroMQ");
}
