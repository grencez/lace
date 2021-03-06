
#include "kve.h"
#include <string.h>

size_t ksize_LaceKVE_size(size_t x) {
  unsigned i;
  if (0 == (x & high_size_bit(0))) {
    return x;
  }
  for (i = 1; i < sizeof(size_t)-1; ++i) {
    if (0 == (x & high_size_bit(i*CHAR_BIT))) {
      return x & (high_size_bit(i*CHAR_BIT)-1);
    }
  }
  return x & (((size_t)1 << CHAR_BIT)-1);
}

size_t splitksize_LaceKVE_size(size_t x) {
  unsigned i;
  size_t y;
  if (0 == get_splitkexists_bit_LaceKVE_size(x)) {
    return 0;
  }
  if (0 == get_splitvexists_bit_LaceKVE_size(x)) {
    y = shiftmaskhi_size(x, 2, CHAR_BIT-2);
  } else {
    y = shiftmaskhi_size(x, 3, CHAR_BIT-3);
  }
  for (i = 1; i < sizeof(size_t)-1; ++i) {
    if (0 == (x & high_size_bit(i*CHAR_BIT))) {
      break;
    }
    y <<= CHAR_BIT-1;
    y |= shiftmaskhi_size(x, i*CHAR_BIT+1, CHAR_BIT-1);
  }
  return y;
}


/** Assuming the node is empty, populate it with a key and value.
 *
 * This only overwrites data, leaving the node's connectivity intact.
 **/
  void
populate_empty_LaceKVE(LaceKVE* e,
                       size_t ksize, const void* k,
                       size_t vsize, const void* v)
{
  bool kdirect;
  /* Assuming no split data, so we can clear e->size.*/
  e->size = ksize;

  if (vsize > 0 && v) {
    set1_vexists_bit_LaceKVE(e);
    if (vsize <= sizeof(e->kv[1])) {
      set1_vdirect_bit_LaceKVE(e);
      memcpy(&e->kv[1], v, vsize);
    } else {
      set0_vdirect_bit_LaceKVE(e);
      e->kv[1] = (uintptr_t) v;
    }
    kdirect = (ksize <= sizeof(e->kv[0]));
  } else {
    set0_vexists_bit_LaceKVE(e);
    set0_vdirect_bit_LaceKVE(e);
    kdirect = (ksize <= sizeof(e->kv[0]) + sizeof(e->kv[1]));
  }
  if (kdirect) {
    memcpy(&e->kv[0], k, ksize);
  } else {
    e->kv[0] = (uintptr_t)k;
  }
}


  bool
populate_splitkv_LaceKVE(LaceKVE* e,
                         size_t ksize, const void* k,
                         size_t vsize, const void* v)
{
  const bool vexists = (vsize > 0 && v);
  bool kdirect;
  size_t tmp_ksize = ksize;
  unsigned i;
  if (vexists) {
    tmp_ksize >>= (CHAR_BIT-3);
  } else {
    tmp_ksize >>= (CHAR_BIT-2);
  }
  if (0 != shiftmaskhi_size(e->size, 0, CHAR_BIT)) {
    return false;
  }

  /* Counting from high.*/
  for (i = 1; i < sizeof(size_t)-1 && tmp_ksize > 0; ++i) {
    if (0 != shiftmaskhi_size(e->size, i*CHAR_BIT, CHAR_BIT)) {
      return false;
    }
    tmp_ksize >>= CHAR_BIT-1;
  }
  if (tmp_ksize > 0) {
    return false;
  }
  if (i < sizeof(size_t)-1 && 0 != (e->size & high_size_bit(i*CHAR_BIT))) {
    return false;
  }

  tmp_ksize = ksize;
  /* Counting from low.*/
  for (i = sizeof(size_t)-i; i < sizeof(size_t); ++i) {
    const size_t z = high_byte_bit(0) | (tmp_ksize & (high_byte_bit(0)-1));
    e->size |= z << (i*CHAR_BIT);
    tmp_ksize >>= CHAR_BIT-1;
  }

  if (vexists) {
    set1_splitvexists_bit_LaceKVE(e);
    if (vsize <= sizeof(e->split[1])) {
      set1_splitvdirect_bit_LaceKVE(e);
      memcpy(&e->split[1], v, vsize);
    } else {
      set0_splitvdirect_bit_LaceKVE(e);
      e->split[1] = (uintptr_t) v;
    }
    kdirect = (ksize <= sizeof(e->split[0]));
  } else {
    set0_splitvexists_bit_LaceKVE(e);
    kdirect = (ksize <= sizeof(e->split[0]) + sizeof(e->split[1]));
  }
  if (kdirect) {
    memcpy(&e->split[0], k, ksize);
  } else {
    e->split[0] = (uintptr_t)k;
  }
  return true;
}

  int
cmp_LaceKVE_(size_t keysize, const void* key,
            size_t ejoint, size_t esize,
            const uintptr_t ekv[2])
{
  const size_t actual_size = ksize_LaceKVE_size(esize);
  if (keysize != actual_size) {
    return (keysize < actual_size ? -1 : 1);
  }
  if (kdirect_LaceKVE_joint(ejoint, actual_size)) {
    return memcmp(key, direct_k_LaceKVE_kv(ekv), actual_size);
  } else {
    return memcmp(key, indirect_k_LaceKVE_kv(ekv), actual_size);
  }
}

  int
cmp_split_LaceKVE_(size_t keysize, const void* key,
                   size_t esize, const uintptr_t esplit[2])
{
  const size_t actual_size = splitksize_LaceKVE_size(esize);
  if (keysize != actual_size) {
    return (keysize < actual_size ? -1 : 1);
  }
  if (splitkdirect_LaceKVE_size(esize, actual_size)) {
    return memcmp(key, direct_splitk_LaceKVE_split(esplit), actual_size);
  } else {
    return memcmp(key, indirect_splitk_LaceKVE_split(esplit), actual_size);
  }
}
