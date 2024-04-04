set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)
find_package(Threads REQUIRED)

list(APPEND PWGEN_EXAMPLES
    01_check_duplication
    02_generate_password_table
    03_generate_password)

foreach(example ${PWGEN_EXAMPLES})
    add_executable(${example})
    target_sources(${example}
        PRIVATE ${example}.cpp)
    target_link_libraries(${example}
        PRIVATE pwgen::pwgen
                Threads::Threads)
endforeach()