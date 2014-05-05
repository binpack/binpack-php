/*
  +----------------------------------------------------------------------+
  | BinPack for PHP                                                      |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: Huqiu Liao <liaohuqiu@gmail.com>                             |
  +----------------------------------------------------------------------+
*/

#include <stdio.h>
#include <string.h>

typedef struct {
	char *c;
	size_t len;
	size_t a;
} smart_str;

#define smart_str_0(x) do {											\
	if ((x)->c) {													\
		(x)->c[(x)->len] = '\0';									\
	}																\
} while (0)

#ifndef SMART_STR_PREALLOC
#define SMART_STR_PREALLOC 128
#endif

#ifndef SMART_STR_START_SIZE
#define SMART_STR_START_SIZE 78
#endif

#define SMART_STR_REALLOC(a,b,c) realloc((a),(b))

#define SMART_STR_DO_REALLOC(d, what) \
	(d)->c = SMART_STR_REALLOC((d)->c, (d)->a + 1, (what))

#define smart_str_alloc4(d, n, what, newlen) do {					\
	if (!(d)->c) {													\
		(d)->len = 0;												\
		newlen = (n);												\
		(d)->a = newlen < SMART_STR_START_SIZE 						\
				? SMART_STR_START_SIZE 								\
				: newlen + SMART_STR_PREALLOC;						\
		SMART_STR_DO_REALLOC(d, what);								\
	} else {														\
		newlen = (d)->len + (n);									\
		if (newlen >= (d)->a) {										\
			(d)->a = newlen + SMART_STR_PREALLOC;					\
			SMART_STR_DO_REALLOC(d, what);							\
		}															\
	}																\
} while (0)

#define smart_str_alloc(d, n, what) \
	smart_str_alloc4((d), (n), (what), newlen)

/* wrapper */

#define smart_str_appends_ex(dest, src, what) \
	smart_str_appendl_ex((dest), (src), strlen(src), (what))
#define smart_str_appends(dest, src) \
	smart_str_appendl((dest), (src), strlen(src))

#define smart_str_appendc(dest, c) \
	smart_str_appendc_ex((dest), (c), 0)
#define smart_str_free(s) \
	smart_str_free_ex((s), 0)
#define smart_str_appendl(dest, src, len) \
	smart_str_appendl_ex((dest), (src), (len), 0)
#define smart_str_append(dest, src) \
	smart_str_append_ex((dest), (src), 0)
#define smart_str_append_long(dest, val) \
	smart_str_append_long_ex((dest), (val), 0)
#define smart_str_append_off_t(dest, val) \
	smart_str_append_off_t_ex((dest), (val), 0)
#define smart_str_append_unsigned(dest, val) \
	smart_str_append_unsigned_ex((dest), (val), 0)

#define smart_str_appendc_ex(dest, ch, what) do {					\
	register size_t __nl;											\
	smart_str_alloc4((dest), 1, (what), __nl);						\
	(dest)->len = __nl;												\
	((unsigned char *) (dest)->c)[(dest)->len - 1] = (ch);			\
} while (0)

#define smart_str_free_ex(s, what) do {								\
	smart_str *__s = (smart_str *) (s);								\
	if (__s->c) {													\
		pefree(__s->c, what);										\
		__s->c = NULL;												\
	}																\
	__s->a = __s->len = 0;											\
} while (0)

#define smart_str_appendl_ex(dest, src, nlen, what) do {			\
	register size_t __nl;											\
	smart_str *__dest = (smart_str *) (dest);						\
																	\
	smart_str_alloc4(__dest, (nlen), (what), __nl);					\
	memcpy(__dest->c + __dest->len, (src), (nlen));					\
	__dest->len = __nl;												\
} while (0)


static ssize_t smart_write(void *cookie, const void *data, size_t n)
{
	smart_str *buf = (smart_str *)cookie;
	if (n > 1)
		smart_str_appendl(buf, data, n);
	else if (n == 1)
		smart_str_appendc(buf, *(char *)data);
	return n;
}

static void print_str(char *p, int len)
{
    printf("--------------------\nlen: %d | bit seq | ", len);
    int i = 0;
    for(; i < len; i++)
    {
        printf("%02x ", (unsigned char)p[i]);
    }
    printf("\n");

}

static void print_smart_str(smart_str *str)
{
    print_str(str->c, str->len);
}

#define test(action, reaction, type, value) \
do                                                              \
{                                                               \
    smart_str str = {0};                                \
                                                              \
    bin_packer_t pk;                                                              \
    bin_packer_init(&pk, smart_write, &str, 0);                                                              \
    action(&pk, value);                                                              \
                                                              \
    bin_unpacker_t uk;                                                              \
    bin_unpacker_init(&uk, str.c, str.len, 0);                                                              \
\
    print_smart_str(&str);  \
    type nv;\
    reaction(&uk, &nv); \
    if (value == nv)\
      printf("ok\n");\
    else\
    printf("error %E\n", nv);\
}                                                               \
while(0);
