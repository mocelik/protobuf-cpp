file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS "*.cpp")

add_library(protobuf-cpp SHARED
  ${SRCS}
)


target_include_directories(protobuf-cpp
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

target_compile_features(protobuf-cpp PUBLIC cxx_std_23)
