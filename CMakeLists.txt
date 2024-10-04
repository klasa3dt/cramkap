cmake_minimum_required(VERSION 3.25)

file(GLOB_RECURSE SRCS CONFIGURE_DEPENDS src/*.cc)

project(cramkap)

add_executable(cramkap)
target_sources(cramkap PRIVATE ${SRCS})
target_include_directories(cramkap PRIVATE src/)
target_compile_features(cramkap PRIVATE cxx_std_20)