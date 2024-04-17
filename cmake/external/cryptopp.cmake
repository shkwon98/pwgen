set(THIRD_PARTY_PATH "${CMAKE_CURRENT_BINARY_DIR}/_deps"
    CACHE STRING "A path setting third party libraries download & build directories.")
set(THIRD_PARTY_CACHE_PATH "${CMAKE_CURRENT_SOURCE_DIR}"
    CACHE STRING "A path cache third party source code to avoid repeated download.")

set(THIRD_PARTY_BUILD_TYPE Release)

if(${CMAKE_VERSION} VERSION_GREATER "3.5.2")
    set(SHALLOW_CLONE "GIT_SHALLOW TRUE") # adds --depth=1 arg to git clone of External_Projects
endif()

########################### include cryptopp according to flags ###############################

include(ExternalProject)

set(CRYPTOPP_PREFIX_DIR  ${THIRD_PARTY_PATH}/cryptopp)
set(CRYPTOPP_INSTALL_DIR ${CRYPTOPP_PREFIX_DIR}/install)
set(CRYPTOPP_INCLUDE_DIR "${CRYPTOPP_INSTALL_DIR}/include" CACHE PATH "cryptopp include directory." FORCE)
set(CRYPTOPP_REPOSITORY  https://github.com/weidai11/cryptopp.git)
set(CRYPTOPP_TAG         CRYPTOPP_8_2_0)

if(WIN32)
  set(CRYPTOPP_LIBRARIES "${CRYPTOPP_INSTALL_DIR}/lib/cryptopp-static.lib" CACHE FILEPATH "cryptopp library." FORCE)
  set(CRYPTOPP_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")
  set(CompilerFlags
        CMAKE_CXX_FLAGS
        CMAKE_CXX_FLAGS_DEBUG
        CMAKE_CXX_FLAGS_RELEASE)
  foreach(CompilerFlag ${CompilerFlags})
    string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
  endforeach()
else(WIN32)
  set(CRYPTOPP_LIBRARIES "${CRYPTOPP_INSTALL_DIR}/lib/libcryptopp.a" CACHE FILEPATH "cryptopp library." FORCE)
  set(CRYPTOPP_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
endif(WIN32)

set(CRYPTOPP_CMAKE_ARGS ${COMMON_CMAKE_ARGS}
                        -DBUILD_SHARED=ON
                        -DBUILD_STATIC=ON
                        -DBUILD_TESTING=OFF
                        -DCMAKE_INSTALL_LIBDIR=${CRYPTOPP_INSTALL_DIR}/lib
                        -DCMAKE_INSTALL_PREFIX=${CRYPTOPP_INSTALL_DIR}
                        -DCMAKE_BUILD_TYPE=${THIRD_PARTY_BUILD_TYPE}
                        -DCMAKE_CXX_FLAGS=${CRYPTOPP_CMAKE_CXX_FLAGS}
                        -DCMAKE_CXX_FLAGS_RELEASE=${CMAKE_CXX_FLAGS_RELEASE}
                        -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER})

include_directories(${CRYPTOPP_INCLUDE_DIR})

ExternalProject_Add(
    extern_cryptopp
    ${EXTERNAL_PROJECT_LOG_ARGS}
    ${SHALLOW_CLONE}
    GIT_REPOSITORY ${CRYPTOPP_REPOSITORY}
    GIT_TAG        ${CRYPTOPP_TAG}
    PREFIX         ${CRYPTOPP_PREFIX_DIR}
    SOURCE_DIR     ${CRYPTOPP_SOURCE_DIR}
    UPDATE_COMMAND ""
    PATCH_COMMAND
    COMMAND ${CMAKE_COMMAND} -E remove_directory "<SOURCE_DIR>/cmake/"
    COMMAND git clone https://github.com/noloader/cryptopp-cmake "<SOURCE_DIR>/cmake"
    COMMAND cd "<SOURCE_DIR>/cmake" && git checkout tags/${CRYPTOPP_TAG} -b ${CRYPTOPP_TAG}
    COMMAND ${CMAKE_COMMAND} -E copy_directory "<SOURCE_DIR>/cmake/" "<SOURCE_DIR>/"
    INSTALL_DIR ${CRYPTOPP_INSTALL_DIR}
    CMAKE_ARGS ${CRYPTOPP_CMAKE_ARGS}
    CMAKE_CACHE_ARGS -DCMAKE_INSTALL_PREFIX:PATH=${CRYPTOPP_INSTALL_DIR}
                     -DCMAKE_POSITION_INDEPENDENT_CODE:BOOL=ON
                     -DCMAKE_BUILD_TYPE:STRING=${THIRD_PARTY_BUILD_TYPE})

add_library(cryptopp STATIC IMPORTED GLOBAL)
set_property(TARGET cryptopp PROPERTY IMPORTED_LOCATION ${CRYPTOPP_LIBRARIES})
add_dependencies(cryptopp extern_cryptopp)