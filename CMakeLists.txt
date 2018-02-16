cmake_minimum_required(VERSION 3.8)
project(static_uint LANGUAGES CXX)

add_library(static_uint INTERFACE)
target_link_libraries(static_uint INTERFACE constd)
target_include_directories(static_uint INTERFACE include/)
target_compile_features(static_uint
  INTERFACE
  cxx_std_17)

add_subdirectory(test)
