project(test_coffee_http)

file(GLOB SOURCES "*.cc")

add_executable(test_coffee_http ${SOURCES})

target_link_libraries(test_coffee_http coffee_http coffee_networking coffee_time coffee_app coffee_xml coffee_logger coffee_basis coffee_config -lxml2 -lgtest -lboost_system -lboost_filesystem -lzmq ${CMAKE_THREAD_LIBS_INIT})

include_directories("../../include")



