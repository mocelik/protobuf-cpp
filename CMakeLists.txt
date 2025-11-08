cmake_minimum_required(VERSION 3.28)
project(Protobuf-Cpp VERSION 0.1.0 LANGUAGES C CXX)

option(BUILD_TESTING "Build tests" ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(GNUInstallDirs)
add_subdirectory(src)

if(BUILD_TESTING)
	enable_testing()
	add_subdirectory(test)
endif()
