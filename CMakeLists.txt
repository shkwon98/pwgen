cmake_minimum_required(VERSION 3.10)

project(pwgen
        LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(third_party)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(pwgen INTERFACE)
target_include_directories(pwgen
    INTERFACE ${CRYPTOPP_INCLUDE_DIR}
              include)
target_link_libraries(pwgen
    INTERFACE cryptopp)
target_compile_features(pwgen
    INTERFACE cxx_std_17)
target_compile_options(pwgen
    INTERFACE $<$<CXX_COMPILER_ID:MSVC>:/W4 /WX>
              $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wextra -Wpedantic -Werror>)
add_library(pwgen::pwgen ALIAS pwgen)

add_subdirectory(examples)