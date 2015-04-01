/* use `make test` to run the test */
/*
 * Copyright (c) 2015 Anatol Pomozov
 * Copyright (c) 2009-2014 Kazuho Oku, Tokuhiro Matsuno, Daisuke Murase,
 *                         Shigeo Mitsunari
 *
 * The software is licensed under either the MIT License (below) or the Perl
 * license.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "picotest/picotest.h"
#include "uriparser.h"

static int bufis(const char* s, size_t l, const char* t)
{
  return strlen(t) == l && memcmp(s, t, l) == 0;
}

static void test_path(void)
{
  const char* path;
  size_t path_len;
  struct uri_keyvalue params[4];
  size_t num_params;
  const char* frag;
  size_t frag_len;

#define PARSE(s, exp, comment)                                   \
  do {                                                           \
    note(comment);                                               \
    num_params = sizeof(params) / sizeof(params[0]);             \
    ok(parse_uri_path(s, sizeof(s) - 1, &path, &path_len,        \
                      params, &num_params, &frag, &frag_len)     \
       == exp);                                                  \
  } while (0)

  PARSE("/hello/world", 0, "simple");
  ok(bufis(path, path_len, "/hello/world"));
  ok(num_params == 0);
  ok(frag == NULL);
  ok(frag_len == 0);

  PARSE("/hello/world?", 0, "question");
  ok(bufis(path, path_len, "/hello/world"));
  ok(num_params == 0);

  PARSE("/reddit?hello=world!&eeee&bar=", 0, "params");
  ok(bufis(path, path_len, "/reddit"));
  ok(num_params == 3);
  ok(bufis(params[0].name, params[0].name_len, "hello"));
  ok(bufis(params[0].value, params[0].value_len, "world!"));
  ok(bufis(params[1].name, params[1].name_len, "eeee"));
  ok(params[1].value == NULL);
  ok(bufis(params[2].name, params[2].name_len, "bar"));
  ok(bufis(params[2].value, params[2].value_len, ""));

  PARSE("/hello/world?a=b&a=c&a=d&a=e&e=4", -2, "toomanyparams");

  PARSE("/hello/world?a=b&a=c&a=d&a=e", 0, "lotofparams");
  ok(bufis(path, path_len, "/hello/world"));
  ok(num_params == 4);
  ok(bufis(params[0].name, params[0].name_len, "a"));
  ok(bufis(params[0].value, params[0].value_len, "b"));
  ok(bufis(params[1].name, params[1].name_len, "a"));
  ok(bufis(params[1].value, params[1].value_len, "c"));
  ok(bufis(params[2].name, params[2].name_len, "a"));
  ok(bufis(params[2].value, params[2].value_len, "d"));
  ok(bufis(params[3].name, params[3].name_len, "a"));
  ok(bufis(params[3].value, params[3].value_len, "e"));

  PARSE("@#$@sdfs", -1, "parseerror");

  PARSE("/tut?#thefrag", 0, "fragment");
  ok(bufis(path, path_len, "/tut"));
  ok(num_params == 0);
  ok(bufis(frag, frag_len, "thefrag"));

#undef PARSE
}

static void test_uri(void)
{
  const char* schema;
  size_t schema_len;
  const char* host;
  size_t host_len;
  int port;
  const char* path;
  size_t path_len;
  struct uri_keyvalue params[4];
  size_t num_params;
  const char* frag;
  size_t frag_len;

#define PARSE(s, exp, comment)                                   \
  do {                                                           \
    note(comment);                                               \
    num_params = sizeof(params) / sizeof(params[0]);             \
    ok(parse_uri(s, sizeof(s) - 1, &schema, &schema_len,    \
                      &host, &host_len, &port, &path, &path_len, \
                      params, &num_params, &frag, &frag_len)     \
       == exp);                                                  \
  } while (0)

  PARSE("https://google.com", 0, "simple");
  ok(bufis(schema, schema_len, "https"));
  ok(bufis(host, host_len, "google.com"));
  ok(port == -1);
  ok(path == NULL);
  ok(num_params == 0);
  ok(frag == NULL);

  PARSE("http://hello.com:890/foobar", 0, "withportandpath");
  ok(bufis(schema, schema_len, "http"));
  ok(bufis(host, host_len, "hello.com"));
  ok(port == 890);
  ok(bufis(path, path_len, "/foobar"));

  PARSE("http://hello.com:1/justbar/eeee?key=val#fragment", 0, "longuri");
  ok(bufis(schema, schema_len, "http"));
  ok(bufis(host, host_len, "hello.com"));
  ok(port == 1);
  ok(bufis(path, path_len, "/justbar/eeee"));
  ok(num_params == 1);
  ok(bufis(params[0].name, params[0].name_len, "key"));
  ok(bufis(params[0].value, params[0].value_len, "val"));
  ok(bufis(frag, frag_len, "fragment"));

#undef PARSE
}

int main(int argc, char **argv)
{
  subtest("path", test_path);
  subtest("uri", test_uri);
  return done_testing();
}
