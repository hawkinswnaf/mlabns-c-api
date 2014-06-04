#ifndef __JSON
#define __JSON

#define JSON_TYPE_OBJECT 1
#define JSON_TYPE_ARRAY 2
#define JSON_TYPE_SINGLE 3

#define EMPTY_JSON_OBJECT(a) \
  struct json_object *a = (struct json_object*) \
    malloc(sizeof(struct json_object)); \
  memset(a, 0, sizeof(struct json_object));

struct json_object {
  int type;
  char *name;
  struct json_object *a_value;
  char *s_value;
  struct json_object *next;
};

void print_json_object(struct json_object *);
struct json_object *new_object_object(struct json_object *);
struct json_object *new_object_array(struct json_object *);
struct json_object *new_object_single(char *);
#endif
