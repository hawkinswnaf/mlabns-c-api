/* vim: set expandtab ts=2: */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "json_object.h"

struct json_object *new_object_array(struct json_object *first) {
  EMPTY_JSON_OBJECT(new_object);  

  new_object->type = JSON_TYPE_ARRAY;
  new_object->a_value = first;
  
  return new_object;
}

struct json_object *new_object_single(char *value) {
  EMPTY_JSON_OBJECT(new_object);  

  new_object->type = JSON_TYPE_SINGLE;
  new_object->s_value = (char*)malloc(sizeof(char*)*(strlen(value) + 1));

  memset(new_object->s_value, 0, sizeof(char*)*(strlen(value) + 1));
  memcpy(new_object->s_value, value, strlen(value));

  return new_object;
}

struct json_object *new_object_object(struct json_object *object) {
  EMPTY_JSON_OBJECT(new_object);  

  new_object->type = JSON_TYPE_OBJECT;
  new_object->a_value = object;

  return new_object;
}


void print_json_object(struct json_object *obj) {
  struct json_object *obj_iterator = obj;
  char *type = NULL, *value = NULL, *name = NULL;

  name = obj_iterator->name ? obj_iterator->name : "NULL";

  if (obj_iterator->type == JSON_TYPE_ARRAY) {
    type = "ARRAY";
    value = "NULL";
  } else if (obj_iterator->type == JSON_TYPE_SINGLE) {
    type = "SINGLE";
    value = obj_iterator->s_value ? obj_iterator->s_value : "NULL";
  } else if (obj_iterator->type == JSON_TYPE_OBJECT) {
    type = "OBJECT";
    value = "NULL";
  }
  printf("(%s)%s:%s\n", type, name, value);

  if (obj_iterator->type == JSON_TYPE_ARRAY) {
    struct json_object *array_obj_iterator = obj_iterator->a_value;
    for (; array_obj_iterator; array_obj_iterator = array_obj_iterator->a_value) {
      print_json_object(array_obj_iterator);
    }
  }
}
