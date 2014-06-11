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

#include <stdlib.h>
#include <stdint.h>         
#include <limits.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>

#include "bin_pack.h"
#include "bin_pack_endian.h"

#include <stdio.h>

#ifdef CUBE_LIB_RCSID
static const char rcsid[] = "$Id: bin_pack.c, huqiu Exp $";
#endif

#define TMPBUF_SIZE	24

#ifndef SSIZE_MAX
#define SSIZE_MAX	(SIZE_MAX/2)
#endif

static const char *binpack_tpnames[] = {
    "UNKNOWN",	/*  0 */
    "CLOSURE",	/*  1 */
    "LIST",		/*  2 */
    "DICT",		/*  3 */
    "BOOL",		/*  4 */
    "DOUBLE",	/*  5 */
    "FLOAT",	/*  5 */
    "NULL",		/*  7 */
    "STRING",	/*  8 */
    "DOUBLE",	/*  9 */
    "INTEGER",	/* 10 */
};

static BINPACK_INLINE size_t do_pack_intstr(char *buf, int type, uintmax_t num)
{
    char *p = buf;
    while (num >= BIN_TAG_PACK_INTERGER)
    {
        *p++ = 0x80 | num;
        num >>= 7;
    }

    *p++ = type | num;
    return (p - buf);
}

static BINPACK_INLINE size_t do_pack_uint_len(char *buf, int type, uintmax_t num)
{
    char *p = buf;
    /* 0001 xxxx */
    while (num >= BIN_TAG_PACK_UINT_LEN)
    {
        *p++ = 0x80 | num;
        num >>= 7;
    }
    *p++ = type | num;
    return (p - buf);
}

static BINPACK_INLINE size_t do_pack_tag(char *buf, int type, uintmax_t num)
{
    char *p = buf;
    if (num)
    {
        do 
        {
            *p++ = 0x80 | num;
            num >>= 7;
        } while (num);
    }

    *p++ = type;
    return (p - buf);
}

BINPACK_INLINE size_t bin_int_buffer(char *buf, intmax_t value)
{
    return value >= 0 ? do_pack_intstr(buf, BIN_TYPE_INTEGER, value)
        : do_pack_intstr(buf, BIN_TYPE_INTEGER_NEGATIVE, -value);
}

BINPACK_INLINE size_t bin_uint_buffer(char *buf, uintmax_t value)
{
    return do_pack_intstr(buf, BIN_TYPE_INTEGER, value);
}

BINPACK_INLINE size_t bin_strhead_buffer(char *buf, size_t strlen)
{
    assert(strlen <= SSIZE_MAX);
    return do_pack_uint_len(buf, BIN_TYPE_STRING, strlen);
}

BINPACK_INLINE size_t bin_blobhead_buffer(char *buf, size_t bloblen)
{
    assert(bloblen <= SSIZE_MAX);
    return do_pack_uint_len(buf, BIN_TYPE_BLOB, bloblen);
}

size_t bin_float_double_buffer(char *buf, double value)
{
    union { double v; uint64_t c; } f;
	f.v = value;
    
    uint64_t x = do_binpack_be64(f.c);

    *buf++ = BIN_TYPE_FLOAT_DOUBLE;
    memcpy(buf, &x, 8);

    return 9;
}

size_t bin_float_single_buffer(char *buf, float value)
{
    *buf++ = BIN_TYPE_FLOAT_SINGLE;

    union { float v; uint32_t c; } f;
	f.v = value;
    
    uint32_t x = do_binpack_be32(f.c);
    memcpy(buf, &x, 4);

    return 5;
}

int bin_unpack_type(bin_unpacker_t *packer, uintmax_t *p_num)
{
    if (packer->pos < packer->size)
    {
        char *p = packer->buf + packer->pos;
        uintmax_t num = 0;
        int type = 0;

        int x = *(unsigned char *)p++;
        int shift = 0;

        /* 1xxx xxxx */
        if (x >= 0x80)
        {
            int left = sizeof(uintmax_t) * 8 ;
            while (x >= 0x80)
            {
                x &= 0x7f;
                num |= ((uintmax_t)x) << shift;

                shift += 7;
                left -= 7;

                if (left <= 0)
                {
                    packer->error = __LINE__;
                    return -1;
                }

                x = *(unsigned char *)p++;
            }
        }

        /* 0001 xxxx */
        if (x < 0x10)
        {
            type = x;
        }
        else
        {
            /* pack:    000x xxxx
             * type:    0xxx x000, integer, bit 5 & 6 directive sub-type inf
             */
            if (x >= BIN_TYPE_INTEGER)
            {
                type = x & BIN_MASK_TYPE_INTEGER;
                num |= (uintmax_t)(x & BIN_MASK_LAST_INTEGER) << shift;
            }
            /* pack:    0000 xxxx, only 4 bits to pack data
             * type:    00xx 0000, string / blob
             */
            else
            {
                type = x & BIN_MASK_TYPE_STRING_OR_BLOB;
                num |= (uintmax_t)(x & BIN_MASK_LAST_UINT_LEN) << shift;
            }
        }

        packer->pos = p - packer->buf;
        *p_num = num;
        return type;
    }

    packer->error = __LINE__;
    return -1;
}

static BINPACK_INLINE int do_unpack_int(bin_unpacker_t *packer, intmax_t *p_value)
{
    int type = bin_unpack_type(packer, (uintmax_t*)p_value);
    int sign = type & BIN_MASK_INTEGER_SIGN;

    if (type < BIN_TYPE_INTEGER)
    {
        packer->error = __LINE__;
        return -1;
    }

    if (!sign)
    {
        if (*p_value < 0)
        {
            packer->error = __LINE__;
            return -1;
        }
    }
    else
    {
        *p_value = -*p_value;
        if (*p_value > 0)
        {
            packer->error = __LINE__;
            return -1;
        }
    }

    return 0;
}

static BINPACK_INLINE int do_unpack_unit(bin_unpacker_t *packer, intmax_t *p_value)
{
    uintmax_t num;

    int type = bin_unpack_type(packer, (uintmax_t*)p_value);
    int sign = type & BIN_MASK_INTEGER_SIGN;

    if (type < BIN_TYPE_INTEGER || sign)
    {
        packer->error = __LINE__;
        return -1;
    }

    *p_value = num;
    return 0;
}


int bin_pack_integer(bin_packer_t *packer, intmax_t value)
{
    char tmpbuf[TMPBUF_SIZE];
    size_t n = bin_int_buffer(tmpbuf, value);
    if (packer->write(packer->buf, tmpbuf, n) != n)
    {
        packer->error = __LINE__;
        return -1;
    }
    return 0;
}

int bin_pack_uinteger(bin_packer_t *packer, uintmax_t value)
{
    char tmpbuf[TMPBUF_SIZE];
    size_t n = bin_uint_buffer(tmpbuf, value);
    if (packer->write(packer->buf, tmpbuf, n) != n)
    {
        packer->error = __LINE__;
        return -1;
    }
    return 0;
}

BINPACK_INLINE int bin_pack_lstring(bin_packer_t *packer, const char *str, size_t len)
{
    char tmpbuf[TMPBUF_SIZE];
    size_t n = bin_strhead_buffer(tmpbuf, len);
    if (packer->write(packer->buf, tmpbuf, n) != n)
    {
        packer->error = __LINE__;
        return -1;
    }

    if (len > 0 && packer->write(packer->buf, str, len) != len)
    {
        packer->error = __LINE__;
        return -1;
    }
    return 0;
}

int bin_pack_string(bin_packer_t *packer, const char *str)
{
    return bin_pack_lstring(packer, str, strlen(str));
}

int bin_pack_blob(bin_packer_t *packer, const void *data, size_t len)
{
    char tmpbuf[TMPBUF_SIZE];
    size_t n = bin_blobhead_buffer(tmpbuf, len);
    if (packer->write(packer->buf, tmpbuf, n) != n)
    {
        packer->error = __LINE__;
        return -1;
    }

    if (len > 0 && packer->write(packer->buf, data, len) != len)
    {
        packer->error = __LINE__;
        return -1;
    }
    return 0;
}

int bin_pack_float_double(bin_packer_t *packer, double value)
{
    char tmpbuf[9];
    size_t n = bin_float_double_buffer(tmpbuf, value);
    if (packer->write(packer->buf, tmpbuf, n) != n)
    {
        packer->error = __LINE__;
        return -1;
    }
    return 0;
}

int bin_pack_float_single(bin_packer_t *packer, float value)
{
    char tmpbuf[5];
    size_t n = bin_float_single_buffer(tmpbuf, value);
    if (packer->write(packer->buf, tmpbuf, n) != n)
    {
        packer->error = __LINE__;
        return -1;
    }
    return 0;
}


int bin_unpack_integer(bin_unpacker_t *packer, intmax_t *p_value)
{
    return do_unpack_int(packer, p_value);
}

int bin_unpack_uinteger(bin_unpacker_t *packer, uintmax_t *p_value)
{
    return do_unpack_unit(packer, p_value);
}

BINPACK_INLINE int bin_unpack_lstring(bin_unpacker_t *packer, char **p_str, size_t *p_len)
{
    uintmax_t num;

    if (BIN_TYPE_STRING != bin_unpack_type(packer, &num) || num > SSIZE_MAX)
    {
        packer->error = __LINE__;
        return -1;
    }

    if (num > packer->size - packer->pos)
    {
        packer->error = __LINE__;
        return -1;
    }

    *p_str = packer->buf + packer->pos;
    *p_len = num;

    packer->pos += num;
    return 0;
}

int bin_unpack_blob(bin_unpacker_t *packer, void **p_data, size_t *p_len)
{
    uintmax_t num;

    if (BIN_TYPE_BLOB != bin_unpack_type(packer, &num) || num > SSIZE_MAX)
    {
        packer->error = __LINE__;
        return -1;
    }

    if (num > packer->size - packer->pos)
    {
        packer->error = __LINE__;
        return -1;
    }

    *p_data = packer->buf + packer->pos;
    *p_len = num;
    packer->pos += num;
    return 0;
}

double bin_make_double(bin_unpacker_t *packer)
{
    union { uint64_t c; double v;} f;

    char *p = packer->buf + packer->pos;
    f.c = do_binpack_be64(*(uint64_t *)p);
    packer->pos += 8;
    return f.v;
}

float bin_make_float(bin_unpacker_t *packer)
{
    union { uint32_t c; float v;} f;
    char *p = packer->buf + packer->pos;

    f.c = do_binpack_be32(*(uint32_t *)p);

    return f.v;
}

int bin_unpack_float_double(bin_unpacker_t *packer, double *p_value)
{
    uintmax_t num;
    if (BIN_TYPE_FLOAT_DOUBLE != bin_unpack_type(packer, &num))
    {
        packer->error = __LINE__;
        return -1;
    }

    *p_value = bin_make_double(packer);
    return 0;
}

int bin_unpack_float_single(bin_unpacker_t *packer, float *p_value)
{
    uintmax_t num;
    if (BIN_TYPE_FLOAT_SINGLE != bin_unpack_type(packer, &num))
    {
        packer->error = __LINE__;
        return -1;
    }

    *p_value = bin_make_float(packer);

    return 0;
}

int bin_unpack_bool(bin_unpacker_t *packer, bool *p_value)
{
    if (packer->pos >= packer->size)
    {
        packer->error = __LINE__;
        return -1;
    }

    /* xxxx x0x0, any x is not 0 */
    if ((packer->buf[packer->pos] & ~0x05) != 0)
    {
        packer->error = __LINE__;
        return -1;
    }

    *p_value = (packer->buf[packer->pos] == BIN_TYPE_BOOL);
    packer->pos++;
    return 0;
}

static BINPACK_INLINE int do_unpack_verify_simple_tag(bin_unpacker_t *packer, int tag)
{
    if (packer->pos >= packer->size || packer->buf[packer->pos] != tag)
    {
        packer->error = __LINE__;
        return -1;
    }

    packer->pos++;
    return 0;
}

int bin_unpack_null(bin_unpacker_t *packer)
{
    return do_unpack_verify_simple_tag(packer, BIN_TYPE_NULL);
}
