#include "syscx.h"
#include "alphatab.h"
#include "fileb.h"

int main(int argc, char** argv) {
  AlphaTab tmppath = cons1_AlphaTab("lace");
  mktmppath_sysCx(&tmppath);
  if (!rmdir_sysCx(ccstr_of_AlphaTab(&tmppath))) {
    failout_sysCx("failed to create temporary directory");
  }
  lose_AlphaTab(&tmppath);
  lose_sysCx();
  return 0;
}
