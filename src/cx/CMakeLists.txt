
add_library (cx_lib STATIC
  $<TARGET_OBJECTS:lace_compat_lib>
  $<TARGET_OBJECTS:lace_lib>
  "alphatab.c"
  "alphatab.h"
  "associa.h"
  "bittable.h"
  "bstree.c"
  "bstree.h"
  "def.h"
  "lgtable.h"
  "rbtree.c"
  "rbtree.h"
  "synhax.h"
  "syscx.c"
  "syscx.h"
  "table.h"
  )

add_executable(bittable_test
  "bittable_test.c")
target_link_libraries(bittable_test cx_lib)
add_test(NAME bittable_test COMMAND bittable_test)

add_executable(lgtable_test
  "lgtable_test.c")
target_link_libraries(lgtable_test cx_lib)
add_test(NAME lgtable_test COMMAND lgtable_test)

add_executable(rbtree_test
  "rbtree_test.c")
target_link_libraries(rbtree_test cx_lib)
add_test(NAME rbtree_test COMMAND rbtree_test)

add_executable(table_test
  "table_test.c")
target_link_libraries(table_test cx_lib)
add_test(NAME table_test COMMAND table_test)

add_executable(tmpdir_test
  "tmpdir_test.c")
target_link_libraries(tmpdir_test cx_lib)
add_test(NAME tmpdir_test COMMAND tmpdir_test)
