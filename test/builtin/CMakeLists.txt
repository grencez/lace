
#add_test(NAME elastic_catfile_test COMMAND
#  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
#  ${lace_exe} --
#  "|< zec"
#  "|- waitdo -- rm -f ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- elastic -x ${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt -o ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|- waitdo -- zec ${CMAKE_CURRENT_BINARY_DIR}/elastic_catfile_output.txt"
#  "|> stdout"
#  )

#### add

add_executable(add_float_test "add_float_test.c" "${CMAKE_SOURCE_DIR}/builtin/add.c")
target_link_libraries(add_float_test lace_lib lace_tool_lib)
set_property(TARGET add_float_test
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_BUILTIN_LIBRARY)
add_test(NAME add_float_test COMMAND add_float_test)

#### bestmatch

add_executable(bestmatch_test "bestmatch_test.c")
target_link_libraries(bestmatch_test lace_lib lace_tool_lib)
add_test(NAME bestmatch_test COMMAND
  bestmatch_test "$<TARGET_FILE:bestmatch>")

#### cmp

add_test(NAME cmp_equal_test COMMAND cmp
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt")

add_test(NAME cmp_diff_test COMMAND spawn ! "$<TARGET_FILE:cmp>"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline_x2.txt")

#### elastic

add_test(NAME elastic_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/elastic_cat.lace"
  )

add_test(NAME elastic_tee_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline_x2.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/elastic_tee.lace"
  )

add_executable(elastic_alt_tee_test "elastic_alt_tee_test.c")
target_link_libraries(elastic_alt_tee_test lace_lib lace_tool_lib)
add_test(NAME elastic_pthread_tee_test COMMAND
  elastic_alt_tee_test "$<TARGET_FILE:elastic_pthread>")
if (UNIX)
  add_test(NAME elastic_poll_tee_test COMMAND
    elastic_alt_tee_test "$<TARGET_FILE:elastic_poll>")
  if (NOT APPLE)
    add_test(NAME elastic_aio_tee_test COMMAND
      elastic_alt_tee_test "$<TARGET_FILE:elastic_aio>")
  endif()
endif()

#### godo

add_executable(godo_test "godo_test.c" "${CMAKE_SOURCE_DIR}/src/godo.c")
target_link_libraries(godo_test lace_lib)
set_property(TARGET godo_test
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_BUILTIN_LIBRARY)
add_test(NAME godo_test COMMAND
  godo_test "$<TARGET_FILE:expectish>" "${CMAKE_SOURCE_DIR}/test/expect/hello.txt")

#### lace

add_test(NAME lace_as_add_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/summed_integers.txt"
  "${lace_exe}" -stdin "${CMAKE_CURRENT_SOURCE_DIR}/integers.txt"
  -as add
  )

add_executable(lace_cli_hello_test "lace_cli_hello_test.c")
target_link_libraries(lace_cli_hello_test lace_lib lace_tool_lib)
add_test(NAME lace_cli_hello_test COMMAND lace_cli_hello_test "${lace_exe}")

add_executable(lace_usage_test "lace_usage_test.c")
target_link_libraries(lace_usage_test lace_compat_lib)
add_test(NAME lace_usage_test COMMAND lace_usage_test "${lace_exe}")

#### sshall

add_executable(echocat "echocat.c" "${CMAKE_SOURCE_DIR}/builtin/zec.c")
set_property(TARGET echocat
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_BUILTIN_LIBRARY)
target_link_libraries(echocat lace_lib)

add_executable(sshall_test "sshall_test.c")
target_link_libraries(sshall_test lace_lib lace_tool_lib)
add_test(NAME sshall_test COMMAND sshall_test "$<TARGET_FILE:sshall>" "$<TARGET_FILE:echocat>")

#### time2sec

add_executable(time2sec_test "time2sec_test.c" "${CMAKE_SOURCE_DIR}/builtin/time2sec.c")
target_link_libraries(time2sec_test lace_lib lace_tool_lib)
set_property(TARGET time2sec_test
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_BUILTIN_LIBRARY)
add_test(NAME time2sec_test COMMAND time2sec_test)

#### transpone

add_executable(transpose_test "transpose_test.c" "${CMAKE_SOURCE_DIR}/src/transpose.c")
target_link_libraries(transpose_test cx_lib lace_tool_lib)
set_property(TARGET transpose_test
  APPEND PROPERTY COMPILE_DEFINITIONS LACE_BUILTIN_LIBRARY)
add_test(NAME transpose_test COMMAND transpose_test)

#### ujoin

add_executable(ujoin_test "ujoin_test.c")
target_link_libraries(ujoin_test lace_lib lace_tool_lib)
add_test(NAME ujoin_test COMMAND
  ujoin_test "$<TARGET_FILE:ujoin>")

#### void

add_executable(void_test "void_test.c")
target_link_libraries(void_test lace_lib lace_tool_lib)
add_test(NAME void_test COMMAND void_test "$<TARGET_FILE:void>")

#### waitdo

add_executable(waitdo_test "waitdo_test.c")
target_link_libraries(waitdo_test lace_lib lace_tool_lib)
add_test(NAME waitdo_test COMMAND waitdo_test "$<TARGET_FILE:waitdo>" "$<TARGET_FILE:shout>")

#### xpipe

add_test(NAME xpipe_nested_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/../expect/xpipe_nested.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/xpipe_nested.lace")

#### zec

add_test(NAME zec_cat_test COMMAND
  comparispawn "${CMAKE_CURRENT_SOURCE_DIR}/hello_newline.txt"
  ${lace_exe} -x "${CMAKE_CURRENT_SOURCE_DIR}/zec_cat.lace"
  )

# Only CMake test here that actually uses `spawn`.
# However, Bazel tests rely on it heavily.
add_test(NAME zec_nop_test COMMAND
  spawn ${zec_exe} "/" "/"
  )

add_test(NAME zec_usage_test COMMAND
  spawn ${zec_exe} -h
  )
