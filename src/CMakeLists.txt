
add_library(lace_lib STATIC
  $<TARGET_OBJECTS:lace_compat_lib>
  "infile.c"
  "instream.c"
  "kve.c"
  "kve.h"
  "log.c"
  "outfile.c"
  "outstream.c"
  "string.c"
  "${CMAKE_SOURCE_DIR}/include/lace.h"
  )


add_subdirectory(cx)


set(LACE_EXTRA_ELASTIC_AIO_LIBS)
set(LACE_EXTRA_ELASTIC_SOURCES)
if (UNIX)
  if (APPLE)
    list(APPEND LACE_EXTRA_ELASTIC_SOURCES "elastic_poll.c")
    list(APPEND LACE_EXTRA_ELASTIC_AIO_LIBS "c")
  else()
    list(APPEND LACE_EXTRA_ELASTIC_SOURCES "elastic_aio.c" "elastic_poll.c")
    list(APPEND LACE_EXTRA_ELASTIC_AIO_LIBS "rt")
  endif()
endif()

add_executable(lace
  "lace.c"
  "utilace.h"
  "best-match.c"
  ${LACE_EXTRA_ELASTIC_SOURCES}
  "execfd.c"
  "godo.c"
  "ssh-all.c"
  "transpose.c"
  "ujoin.c"
  "waitdo.c"
  "xpipe.c"
  "${CMAKE_SOURCE_DIR}/builtin/add.c"
  "${CMAKE_SOURCE_DIR}/builtin/cmp.c"
  "${CMAKE_SOURCE_DIR}/builtin/elastic_pthread.c"
  "${CMAKE_SOURCE_DIR}/builtin/seq.c"
  "${CMAKE_SOURCE_DIR}/builtin/time2sec.c"
  "${CMAKE_SOURCE_DIR}/builtin/void.c"
  "${CMAKE_SOURCE_DIR}/builtin/zec.c"
  "${CMAKE_SOURCE_DIR}/include/lace_builtin.h"
  "${CMAKE_SOURCE_DIR}/include/lace_posix_thread.h"
  )

set_property(TARGET lace APPEND PROPERTY COMPILE_DEFINITIONS LACE_BUILTIN_LIBRARY)
if (WIN32)
  # Nothing special.
elseif (APPLE)
  set_property(TARGET lace APPEND PROPERTY COMPILE_DEFINITIONS
    LACE_BUILTIN_PERMIT_ELASTIC_POLL)
else()
  set_property(TARGET lace
    APPEND PROPERTY COMPILE_DEFINITIONS
    LACE_BUILTIN_PERMIT_ELASTIC_AIO
    LACE_BUILTIN_PERMIT_ELASTIC_POLL)
endif()

target_link_libraries(lace cx_lib ${CMAKE_THREAD_LIBS_INIT} ${LACE_EXTRA_ELASTIC_AIO_LIBS})


add_executable(bestmatch "best-match.c")
target_link_libraries(bestmatch lace_lib)

add_executable(execfd "execfd.c" "utilace.h")
target_link_libraries(execfd cx_lib)

add_executable(godo "godo.c" "utilace.h")
target_link_libraries(godo cx_lib)

add_executable(sshall "ssh-all.c")
target_link_libraries(sshall lace_lib)

add_executable(transpose "transpose.c")
target_link_libraries(transpose cx_lib)

add_executable(ujoin "ujoin.c")
target_link_libraries(ujoin cx_lib)

add_executable(waitdo "waitdo.c" "utilace.h")
target_link_libraries(waitdo cx_lib)

if (UNIX)
  add_executable(elastic_poll "elastic_poll.c")
  target_link_libraries (elastic_poll cx_lib)
endif ()

if (UNIX AND NOT APPLE)
  add_executable(elastic_aio "elastic_aio.c")
  target_link_libraries (elastic_aio cx_lib ${LACE_EXTRA_ELASTIC_AIO_LIBS})
  set_property (TARGET elastic_aio
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=199309L)

  add_executable(chatty "chatty.c")
  target_link_libraries(chatty lace_lib ${LACE_EXTRA_ELASTIC_AIO_LIBS})
  set_property(TARGET chatty
    APPEND PROPERTY COMPILE_DEFINITIONS _POSIX_C_SOURCE=201112L)
endif ()


install(TARGETS lace
  RUNTIME DESTINATION bin)
