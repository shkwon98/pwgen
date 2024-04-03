cmake_minimum_required(VERSION 3.10)

project(pwgen
        LANGUAGES CXX)

include(FetchContent)

# Get the cryptopp CMakeLists.txt file for cryptopp package
set(CRYPTOPP_CMAKE "cryptopp-cmake")
FetchContent_Declare(
    ${CRYPTOPP_CMAKE}
    GIT_REPOSITORY https://github.com/noloader/cryptopp-cmake
    GIT_TAG        CRYPTOPP_8_2_0
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE)

FetchContent_GetProperties(${CRYPTOPP_CMAKE})
if(NOT ${CRYPTOPP_CMAKE}_POPULATED)
    FetchContent_Populate(${CRYPTOPP_CMAKE})
endif()

# Get the cryptopp package
set(CRYPTOPP "cryptopp")
FetchContent_Declare(
    ${CRYPTOPP}
    GIT_REPOSITORY https://github.com/weidai11/cryptopp
    GIT_TAG        CRYPTOPP_8_2_0
    GIT_PROGRESS   TRUE
    GIT_SHALLOW    TRUE)

FetchContent_GetProperties(${CRYPTOPP})
if(NOT ${CRYPTOPP}_POPULATED)
    FetchContent_Populate(${CRYPTOPP})
endif()

file(COPY ${${CRYPTOPP_CMAKE}_SOURCE_DIR}/CMakeLists.txt DESTINATION ${${CRYPTOPP}_SOURCE_DIR})
add_subdirectory(${${CRYPTOPP}_SOURCE_DIR} ${${CRYPTOPP}_BINARY_DIR})
file(GLOB CRYPTOPP_HEADERS "${cryptopp_SOURCE_DIR}/*.h")
file(MAKE_DIRECTORY ${cryptopp_SOURCE_DIR}/include/cryptopp)
file(COPY ${CRYPTOPP_HEADERS} DESTINATION ${cryptopp_SOURCE_DIR}/include/cryptopp)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif()

add_library(pwgen INTERFACE)
target_include_directories(pwgen
    INTERFACE ${cryptopp_SOURCE_DIR}/include
              include)
target_link_libraries(pwgen
    INTERFACE cryptopp-static)
target_compile_features(pwgen
    INTERFACE cxx_std_17)
target_compile_options(pwgen
    INTERFACE -Wall
              -Wextra
              -Werror)
add_library(pwgen::pwgen ALIAS pwgen)

add_subdirectory(examples)