# Prefer a system-installed GTest (specific minimum version), otherwise fetch it
find_package(GTest 1.12 QUIET)

if (NOT GTest_FOUND)
	include(FetchContent)
	FetchContent_Declare(
		googletest
		GIT_REPOSITORY https://github.com/google/googletest.git
		GIT_TAG release-1.12.0
	)
	FetchContent_MakeAvailable(googletest)
endif()

include(GoogleTest)

add_executable(test_mylib test_stub.cpp)
target_link_libraries(test_mylib PRIVATE stub_lib gtest_main)

gtest_discover_tests(test_mylib)
