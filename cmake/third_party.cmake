include(ExternalProject)
# Creat a target named "third_party", which can compile external dependencies on all platform(windows/linux/mac)

set(THIRD_PARTY_PATH
    "${CMAKE_CURRENT_BINARY_DIR}/third_party"
    CACHE STRING
          "A path setting third party libraries download & build directories.")
set(THIRD_PARTY_CACHE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}"
    CACHE STRING
          "A path cache third party source code to avoid repeated download.")

set(THIRD_PARTY_BUILD_TYPE Release)
set(third_party_deps)

if(${CMAKE_VERSION} VERSION_GREATER "3.5.2")
    set(SHALLOW_CLONE "GIT_SHALLOW TRUE") # adds --depth=1 arg to git clone of External_Projects
endif()

########################### include third_party according to flags ###############################

include(external/cryptopp) # download, build, install cryptopp
list(APPEND third_party_deps extern_cryptopp)
add_definitions(-DPADDLE_WITH_CRYPTO)

add_custom_target(third_party ALL DEPENDS ${third_party_deps})