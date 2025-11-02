add_library(protobuf-cpp SHARED
  Varint.cpp
)

target_include_directories(protobuf-cpp
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_compile_features(protobuf-cpp PUBLIC cxx_std_20)
