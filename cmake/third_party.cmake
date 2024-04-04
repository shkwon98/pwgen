# Copyright (c) 2018 PaddlePaddle Authors. All Rights Reserved.
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
# http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

include(ExternalProject)
# Creat a target named "third_party", which can compile external dependencies on all platform(windows/linux/mac)

set(THIRD_PARTY_PATH
    "${CMAKE_BINARY_DIR}/third_party"
    CACHE STRING
          "A path setting third party libraries download & build directories.")
set(THIRD_PARTY_CACHE_PATH
    "${CMAKE_SOURCE_DIR}"
    CACHE STRING
          "A path cache third party source code to avoid repeated download.")

set(THIRD_PARTY_BUILD_TYPE Release)
set(third_party_deps)

if(${CMAKE_VERSION} VERSION_GREATER "3.5.2")
  set(SHALLOW_CLONE "GIT_SHALLOW TRUE"
  )# adds --depth=1 arg to git clone of External_Projects
endif()

########################### include third_party according to flags ###############################

include(external/cryptopp) # download, build, install cryptopp
list(APPEND third_party_deps extern_cryptopp)
add_definitions(-DPADDLE_WITH_CRYPTO)

add_custom_target(third_party ALL DEPENDS ${third_party_deps})