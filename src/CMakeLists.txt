add_library(stub_lib SHARED
  stub.cpp
)

target_include_directories(stub_lib
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_compile_features(stub_lib PUBLIC cxx_std_17)
