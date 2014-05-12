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

#ifndef BIN_PACK_H_
#define BIN_PACK_H_ 1

#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
	BIN_TYPE_CLOSURE	            = 0x01,	
	BIN_TYPE_LIST   	            = 0x02,
	BIN_TYPE_DICT   	            = 0x03,
	BIN_TYPE_BOOL   	            = 0x04,     /* 0000 0100 T */
    BIN_TYPE_BOOL_FALSE             = 0x05,     /* 0000 0101 F */

	BIN_TYPE_FLOAT_DOUBLE           = 0x06,     /* 0010 0110   */
    BIN_TYPE_FLOAT_SINGLE           = 0x07,     /* 0000 0111   */

	BIN_TYPE_NULL   	            = 0x0f,

	BIN_TYPE_BLOB   	            = 0x10,		/* 0001 xxxx   */
	BIN_TYPE_STRING   	            = 0x20,		/* 0010 xxxx   */

	BIN_TYPE_INTEGER 	            = 0x40,		/* 010x xxxx + */
	BIN_TYPE_INTEGER_NEGATIVE    	= 0x60,     /* 011x xxxx - */
} bin_type_t;

#define BIN_INTEGER_TYPE_64                 0x00 << 3   /* default implementation */
#define BIN_INTEGER_TYPE_8                  0x01 << 3
#define BIN_INTEGER_TYPE_16                 0x02 << 3
#define BIN_INTEGER_TYPE_32                 0x03 << 3

#define BIN_INTEGER_SUBTYPE_MASK            0x03 << 3

#define BIN_INTEGER_NEGATVIE_MASK           0x20        /* 001x xxxx, 2 bits for type, 3 bits for packing */

#define BIN_TAG_PACK_INTERGER               0x08        /* 0000 0xxx */
#define BIN_TAG_PACK_NUM                    0x10        /* 0000 xxxx */

#define bin_bool_byte(value)	((unsigned char)((value) ? BIN_TYPE_BOOL : BIN_TYPE_BOOL_FALSE))
#define bin_null_byte()		((unsigned char)BIN_TYPE_NULL)
#define bin_closure_byte()	((unsigned char)BIN_TYPE_CLOSURE)
#define bin_list_byte()		((unsigned char)BIN_TYPE_LIST)
#define bin_dict_byte()		((unsigned char)BIN_TYPE_DICT)

#define bin_pack_bool(packer, value) do {       \
    char byte = bin_bool_byte(value);           \
    (packer)->write((packer)->buf, (&byte), 1); \
}while(0)

#define bin_pack_null(packer) do {              \
    char byte = bin_null_byte();                \
    (packer)->write((packer)->buf, (&byte), 1); \
}while(0)

#define bin_pack_open_list(packer) do {         \
    char byte = bin_list_byte();                \
    (packer)->write((packer)->buf, (&byte), 1); \
}while(0)

#define bin_pack_open_dict(packer) do {         \
    char byte = bin_dict_byte();                \
    (packer)->write((packer)->buf, (&byte), 1); \
}while(0)

#define bin_pack_closure(packer) do {           \
    char byte = bin_closure_byte();             \
    (packer)->write((packer)->buf, (&byte), 1); \
}while(0)

#define bin_packer_init(PACKER, WRITE, BUF)	do {	\
		(PACKER)->write = (WRITE);				    \
		(PACKER)->buf = (BUF);				        \
} while(0)

#define bin_unpacker_init(UK, BUF, SIZE)	do {    \
		(UK)->buf = (BUF);					        \
		(UK)->size = (SIZE);					    \
		(UK)->pos = 0;						        \
		(UK)->error = 0;				        	\
} while(0)


size_t bin_int_buffer(char *buf, intmax_t value);
size_t bin_uint_buffer(char *buf, uintmax_t value);
size_t bin_real_buffer(char *buf, double value);

size_t bin_strhead_buffer(char *buf, size_t strlen);
size_t bin_blobhead_buffer(char *buf, size_t bloblen);


typedef struct bin_packer_t bin_packer_t;
struct bin_packer_t {
	ssize_t (*write)(void *buf, const void *data, size_t size);
	void *buf;
	int error;
};


typedef struct bin_unpacker_t bin_unpacker_t;
struct bin_unpacker_t {
	char *buf;
	size_t size;
	size_t pos;
	int error;
};

/* 
 * Return 0 if success. 
 * Return -1 if error.
 */
int bin_pack_integer(bin_packer_t *packer, intmax_t value);
int bin_pack_uinteger(bin_packer_t *packer, uintmax_t value);
int bin_pack_lstring(bin_packer_t *packer, const char *str, size_t len);
int bin_pack_string(bin_packer_t *packer, const char *str);
int bin_pack_blob(bin_packer_t *packer, const void *data, size_t len);
int bin_pack_float_double(bin_packer_t *packer, double value);
int bin_pack_float_single(bin_packer_t *packer, float value);


int bin_unpack_type(bin_unpacker_t *job, uintmax_t *p_value);
int bin_unpack_integer(bin_unpacker_t *job, intmax_t *p_value);
int bin_unpack_uinteger(bin_unpacker_t *job, uintmax_t *p_value);
int bin_unpack_lstring(bin_unpacker_t *job, char **p_str, size_t *p_len);
int bin_unpack_blob(bin_unpacker_t *job, void **p_data, size_t *p_len);
int bin_unpack_float_double(bin_unpacker_t *job, double *p_value);
int bin_unpack_float_single(bin_unpacker_t *job, float *p_value);
int bin_unpack_bool(bin_unpacker_t *job, bool *p_value);
int bin_unpack_null(bin_unpacker_t *job);

double bin_make_double(bin_unpacker_t *packer);
float bin_make_float(bin_unpacker_t *packer);
#ifdef __cplusplus
}
#endif

#endif
