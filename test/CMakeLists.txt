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



list(APPEND CMAKE_MODULE_PATH "${CMAKE_BINARY_DIR}/cmake")
set(CODE_COVERAGE_VERBOSE ON)
include(CodeCoverage)

# Generate the pb.cc and pb.h files from the .proto files
find_package(Protobuf REQUIRED)
file(GLOB_RECURSE PROTO_FILES CONFIGURE_DEPENDS "proto/*.proto")
protobuf_generate_cpp(PROTO_SRCS PROTO_HDRS ${PROTO_FILES})

# Create test executable
file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "*.cpp")
add_executable(test_protobuf-cpp ${TEST_SOURCES} ${PROTO_SRCS})
target_compile_options(test_protobuf-cpp PRIVATE "--coverage")
target_include_directories(test_protobuf-cpp PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_options(test_protobuf-cpp PRIVATE "--coverage")
target_link_libraries(test_protobuf-cpp PRIVATE protobuf-cpp gtest_main protobuf::libprotobuf)
gtest_discover_tests(test_protobuf-cpp)

# Setup code coverage target
# Download CodeCoverage.cmake module
set(CODE_COVERAGE_MODULE_URL
    "https://raw.githubusercontent.com/bilke/cmake-modules/d1e13f6c51554554d391fe6ac48f087aac9f41d8/CodeCoverage.cmake"
)
set(CODE_COVERAGE_MODULE_LOCAL "${CMAKE_BINARY_DIR}/cmake/CodeCoverage.cmake")

# Only download if not already present
if(NOT EXISTS "${CODE_COVERAGE_MODULE_LOCAL}")
  file(DOWNLOAD
    ${CODE_COVERAGE_MODULE_URL}
    ${CODE_COVERAGE_MODULE_LOCAL}
    STATUS _dl_status
    EXPECTED_HASH SHA256=463383198996657017e7b1e7218236a69e0c48da01263134d0f4706721044119
  )
  list(GET _dl_status 0 _dl_code)
  if(NOT _dl_code EQUAL 0)
    message(FATAL_ERROR "Failed to download CodeCoverage module: ${_dl_status}")
  endif()
endif()
set(GCOVR_ADDITIONAL_ARGS "--sonarqube" "coverage.xml" "--html-nested" "coverage.html")
setup_target_for_coverage_gcovr_xml(
    NAME coverage
    EXECUTABLE test_protobuf-cpp
    DEPENDENCIES test_protobuf-cpp
    BASE_DIRECTORY "${PROJECT_SOURCE_DIR}"
	EXCLUDE "${CMAKE_BINARY_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}"
)
