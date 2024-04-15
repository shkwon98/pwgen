# Pwgen - C++ Password Generation Library

## Table of Contents

- [Pwgen - C++ Password Generation Library](#pwgen---c-password-generation-library)
  - [Table of Contents](#table-of-contents)
  - [About ](#about-)
  - [Getting Started ](#getting-started-)
    - [find\_package (Not supported yet)](#find_package-not-supported-yet)
    - [FetchContent](#fetchcontent)
    - [git submodule](#git-submodule)
  - [Installing](#installing)
    - [Build from source](#build-from-source)
      - [System-wide installation](#system-wide-installation)
      - [Local installation](#local-installation)
    - [Package manager (Not supported yet)](#package-manager-not-supported-yet)
  - [Usage ](#usage-)

## About <a name = "about"></a>

Pwgen is an open-source C++ library that generates passwords with a length of 8 base64 characters. It is meant to be a simple and easy-to-use library that can be used in any C++ project that requires password generation.

## Getting Started <a name = "getting_started"></a>

This section describes how to add pwgen as a dependency to your C++ project.
While Pwgen has dependency on the [`Crypto++`](https://www.cryptopp.com/) library, it is not necessary to install it separately as it will be built along with pwgen.
Pwgen supports CMake, so you can easily add it to your project by following the instructions below.

### find_package (Not supported yet)

The canonical way to discover dependencies in CMake is the [`find_package` command](https://cmake.org/cmake/help/latest/command/find_package.html). This way will be supported soon.

```cmake
find_package(pwgen CONFIG REQUIRED)
add_executable(my_exe my_exe.cpp)
target_link_libraries(my_exe pwgen::pwgen)
```

`find_package` can only find software that has already been installed on your system. In practice that means you'll need to install pwgen using cmake first. Pwgen's cmake support will provide the option to install pwgen either system-wide(not recommended) or under a directory prefix in a way that you can later easily use it with the `find_package(pwgen CONFIG REQUIRED)` command.

The following sections describe strategies to automatically build pwgen as part of your project.

### FetchContent
If you are using CMake v3.11 or newer you'd better use CMake's [FetchContent module](https://cmake.org/cmake/help/latest/module/FetchContent.html).
The first time you run CMake in a given build directory, FetchContent will clone the pwgen repository. `FetchContent_MakeAvailable()` also sets up an `add_subdirectory()` rule for you. This causes pwgen to be built as part of your project.

```cmake
cmake_minimum_required(VERSION 3.11)
project(my_project)

include(FetchContent)
FetchContent_Declare(
  pwgen
  GIT_REPOSITORY https://github.com/shkwon98/pwgen)
set(FETCHCONTENT_QUIET OFF)
FetchContent_MakeAvailable(pwgen)

add_executable(my_exe my_exe.cpp)
target_link_libraries(my_exe pwgen::pwgen)
```

### git submodule
If you cannot use FetchContent, another approach is to add the pwgen source tree to your project as a [git submodule](https://git-scm.com/book/en/v2/Git-Tools-Submodules).
You can then add it to your CMake project with `add_subdirectory()`.

```cmake
add_subdirectory(pwgen)
add_executable(my_exe my_exe.cpp)
target_link_libraries(my_exe pwgen::pwgen)
```

## Installing

If you want to install pwgen library, you can use the following methods.

### Build from source

You can build and install pwgen from source by following the instructions below.

```bash
git clone https://github.com/shkwon98/pwgen.git
cd pwgen
```

#### System-wide installation

```bash
cmake -S . -B build
cmake --build build
cmmake --install build
```

#### Local installation

```bash
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=<path_to_install>
cmake --build build
cmmake --install build
```

### Package manager (Not supported yet)

Pwgen might be available in package managers like `vcpkg` or `conan` in the future.

## Usage <a name = "usage"></a>

You can find out how to use the pwgen library by looking at the example codes. Some simple c++ example codes are provided in the [`examples`](examples) directory to show how to use the pwgen library.