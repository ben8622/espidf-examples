# This file was automatically generated for projects
# without default 'CMakeLists.txt' file.

REQUIRES esp_wifi

FILE(GLOB_RECURSE app_sources ${CMAKE_SOURCE_DIR}/src/*.*)

idf_component_register(SRCS ${app_sources})
