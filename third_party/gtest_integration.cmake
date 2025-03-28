find_package(GTest CONFIG REQUIRED)
add_library(GTest_Integrated INTERFACE)
target_link_libraries(GTest_Integrated INTERFACE
	GTest::gtest GTest::gmock GTest::gtest_main)
