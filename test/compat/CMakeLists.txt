
add_executable(fd_move_off_stdio_test "fd_move_off_stdio_test.c")
target_link_libraries(fd_move_off_stdio_test lace_compat_lib)
add_test(NAME fd_move_off_stdio_test COMMAND fd_move_off_stdio_test)

add_executable(random_bytes_test "random_bytes_test.c")
target_link_libraries(random_bytes_test lace_compat_lib)
add_test(NAME random_bytes_test COMMAND random_bytes_test)

add_executable(sh_escape_argv_test "sh_escape_argv_test.c")
target_link_libraries(sh_escape_argv_test lace_compat_lib)
add_test(NAME sh_escape_argv_test COMMAND sh_escape_argv_test)

add_executable(string_byte_translate_test "string_byte_translate_test.c")
target_link_libraries(string_byte_translate_test lace_compat_lib)
add_test(NAME string_byte_translate_test COMMAND string_byte_translate_test)
