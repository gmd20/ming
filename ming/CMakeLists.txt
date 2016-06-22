cmake_minimum_required(VERSION 2.8)

project(ming)

get_filename_component(ming_root ${CMAKE_SOURCE_DIR} DIRECTORY)
message (STATUS "ming_root = ${ming_root}")
include_directories("${ming_root}")

if (MSVC)
	# add_compile_options("/MT")
elseif (UNIX)
endif()

file(GLOB ming_source "*.cpp")
file(GLOB ming_header "*.h")

set (folly_source
     "folly/TimeoutQueue.cpp"
     )

# list(APPEND ming_source "${folly_source}" )

add_library(ming STATIC ${ming_source} ${ming_header})
