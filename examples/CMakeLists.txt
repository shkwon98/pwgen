list(APPEND PWDGEN_EXAMPLES
    01_check_duplication
    02_generate_password_table)

foreach(example ${PWDGEN_EXAMPLES})
    add_executable(${example})
    target_sources(${example}
        PRIVATE ${example}.cpp)
    target_link_libraries(${example}
        PRIVATE pwdgen)
endforeach()