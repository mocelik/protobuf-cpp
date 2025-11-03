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

add_executable(test_protobuf-cpp test_varint.cpp test_record.cpp)
target_link_libraries(test_protobuf-cpp PRIVATE protobuf-cpp gtest_main)

gtest_discover_tests(test_protobuf-cpp)
