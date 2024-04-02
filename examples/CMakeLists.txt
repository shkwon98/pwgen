add_executable(check_duplication)
target_sources(check_duplication
    PRIVATE check_duplication.cpp)
target_link_libraries(check_duplication
    PRIVATE pwdgen)