URIParser
=========

uriparser is a small footprint and extremely fast uri parser.

It uses Ragel state machine to parse input. This library does not keep internal state and does not
allocate any dynamic memory.


```C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "uriparser.h"

#define MAX_PARAMS 4

int main(int argc, char const *argv[])
{
  const char* schema;
  size_t schema_len;
  const char* host;
  size_t host_len;
  int port;
  const char* path;
  size_t path_len;
  struct uri_keyvalue params[MAX_PARAMS];
  size_t num_params = MAX_PARAMS;
  const char* fragment;
  size_t fragment_len;

  const char *input = "http://google.com:8080/foo?bar&baz=4#fragm";

  int ret = parse_uri(input, strlen(input), &schema, &schema_len,
    &host, &host_len, &port, &path, &path_len,
    params, &num_params, &fragment, &fragment_len);

  printf("Input URL is %s\n", input);
  printf("schema = %.*s\n", (int)schema_len, schema);
  printf("host = %.*s\n", (int)host_len, host);
  printf("port = %d\n", port);
  printf("path = %.*s\n", (int)path_len, path);
  printf("number of params = %zu\n", num_params);
  for (int i = 0; i < num_params; i++) {
    printf("  param[%d] => %.*s = %.*s\n", i,
      (int)params[i].name_len, params[i].name,
      (int)params[i].value_len, params[i].value);
  }
  printf("fragment = %.*s\n", (int)fragment_len, fragment);


  return 0;
}

```

To compile the example above run:
  gcc -std=c99 -o example example.c uriparser.c
