#ifndef __KVL
#define __KVL
struct key_value {
  char *key;
  char *value;
  struct key_value *next;
};
#endif
