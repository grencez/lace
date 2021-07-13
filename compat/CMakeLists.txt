add_library(lace_compat_lib STATIC
  "errno.c"
  "fd.c"
  "fd_exclusive.h"
  "file.c"
  "random.c"
  "sh.c"
  "string.c"
  "${CMAKE_SOURCE_DIR}/include/lace_compat_errno.h"
  "${CMAKE_SOURCE_DIR}/include/lace_compat_fd.h"
  "${CMAKE_SOURCE_DIR}/include/lace_compat_file.h"
  "${CMAKE_SOURCE_DIR}/include/lace_compat_random.h"
  "${CMAKE_SOURCE_DIR}/include/lace_compat_sh.h"
  "${CMAKE_SOURCE_DIR}/include/lace_compat_string.h"
  )
target_link_libraries(lace_compat_lib ${CMAKE_THREAD_LIBS_INIT})
