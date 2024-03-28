project(pwd-generator
        LANGUAGES CXX)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME}
    PRIVATE src/main.cpp)
target_include_directories(${PROJECT_NAME}
    PRIVATE include)
target_compile_features(${PROJECT_NAME}
    PRIVATE cxx_std_17)
target_compile_options(${PROJECT_NAME}
    PRIVATE -Wall
            -Wextra
            -Werror)
