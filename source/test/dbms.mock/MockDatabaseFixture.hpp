/*
 * MockDatabaseFixture.hpp
 *
 *  Created on: Mar 4, 2018
 *      Author: cisco
 */

#ifndef TEST_DBMS_MOCK_MOCKFIXTURE_HPP_
#define TEST_DBMS_MOCK_MOCKFIXTURE_HPP_

#include <coffee/dbms/Database.hpp>
#include <coffee/dbms/Connection.hpp>
#include <coffee/dbms/ConnectionParameters.hpp>
#include <coffee/app/ApplicationServiceStarter.hpp>

template <class _T> struct MockDatabaseFixture : public ::testing::Test {
   coffee::app::ApplicationServiceStarter app;
   std::shared_ptr<_T> database;
   std::shared_ptr<coffee::dbms::Connection> connection;
   std::thread thr;

   explicit MockDatabaseFixture(const char* appName) : app(appName) { ; }

   virtual void SetUp() {
      database = _T::instantiate(app);
      thr = std::thread(dbmsParallelRun, std::ref(app));
      app.waitUntilRunning();
      ASSERT_TRUE(database->isRunning());
      const coffee::dbms::ConnectionParameters parameters("0", "0");
      connection = database->createConnection("0", parameters);
   }

   virtual void TearDown() {
      app.stop();
      thr.join();
   }

   static void dbmsParallelRun(coffee::app::Application& app) {
      app.start();
   }
};


#endif /* TEST_DBMS_MOCK_MOCKFIXTURE_HPP_ */
