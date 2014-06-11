/*
  +----------------------------------------------------------------------+
  | PHP Version 5                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2013 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_exceptions.h"

#include "ext/standard/info.h"
#include "ext/standard/php_smart_str.h"			/* for smart_str */

#include "php_binpack.h"			
#include "bin_pack.h"

#include <stdio.h>

/* If you declare any globals in php_binpack.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(binpack)
*/

/* True global resources - no need for thread safety here */
static int le_binpack;

ZEND_BEGIN_ARG_INFO_EX(arginfo_bin_encode, 0, 0, 1)
	ZEND_ARG_INFO(0, value)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_bin_decode, 0, 0, 1)
	ZEND_ARG_INFO(0, str)
	ZEND_ARG_INFO(1, len)
ZEND_END_ARG_INFO()


/* {{{ binpack_functions[]
 *
 * Every user visible function must have an entry in binpack_functions[].
 */
zend_function_entry binpack_functions[] = {
	PHP_FE(bin_encode, arginfo_bin_encode)
	PHP_FE(bin_decode, arginfo_bin_decode)
	{NULL, NULL, NULL}	/* Must be the last line in binpack_functions[] */
};
/* }}} */

/* {{{ binpack_module_entry
 */
zend_module_entry binpack_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"binpack",
	binpack_functions,
	PHP_MINIT(binpack),
	PHP_MSHUTDOWN(binpack),
	NULL,		/* Replace with NULL if there's nothing to do at request start */
	NULL,		/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(binpack),
#if ZEND_MODULE_API_NO >= 20010901
	"0.1", /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_BINPACK
ZEND_GET_MODULE(binpack)
#endif

/* {{{ PHP_INI
 */
/* Remove comments and fill if you need to have entries in php.ini
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("binpack.global_value",      "42", PHP_INI_ALL, OnUpdateLong, global_value, zend_binpack_globals, binpack_globals)
    STD_PHP_INI_ENTRY("binpack.global_string", "foobar", PHP_INI_ALL, OnUpdateString, global_string, zend_binpack_globals, binpack_globals)
PHP_INI_END()
*/
/* }}} */

/* {{{ php_binpack_init_globals
 */
/* Uncomment this function if you have INI entries
static void php_binpack_init_globals(zend_binpack_globals *binpack_globals)
{
	binpack_globals->global_value = 0;
	binpack_globals->global_string = NULL;
}
*/
/* }}} */

/* {{{ PHP_MINIT_FUNCTION
 */
PHP_MINIT_FUNCTION(binpack)
{
	/* If you have INI entries, uncomment these lines 
	REGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(binpack)
{
	/* uncomment this line if you have INI entries
	UNREGISTER_INI_ENTRIES();
	*/
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(binpack)
{
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(binpack)
{
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(binpack)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "binpack support", "enabled");
	php_info_print_table_end();

	/* Remove comments if you have entries in php.ini
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/*
 * some function declare here
 */
static void binpack_do_encode(bin_packer_t *pk, zval *val TSRMLS_DC);
static int binpack_do_decode(bin_unpacker_t *uk, zval **val TSRMLS_DC);

#define SPECIAL(p, val, Base, digits)							\
		case Base:												\
				do {											\
						*--(p) = (digits)[(val) % Base];		\
				} while (((val) /= Base) != 0);					\
				break

#define ITOA(p, val, base, digits)		do {					\
		switch (base)											\
		{														\
		SPECIAL(p, val, 10, digits);							\
		SPECIAL(p, val, 16, digits);							\
		SPECIAL(p, val, 8, digits);								\
		default: do {											\
						*--(p) = (digits)[(val) % (base)];		\
				} while (((val) /= (base)) != 0);				\
		}														\
} while (0)

static inline char *_itoa(char *bufend, uintmax_t value)
{
		const char *digits = "0123456789";
		ITOA(bufend, value, 10, digits);
		return bufend;
}

inline static int binpack_check_ht_is_map(zval *array TSRMLS_DC) 
{
    int count = zend_hash_num_elements(Z_ARRVAL_P(array));

    if (count != (Z_ARRVAL_P(array))->nNextFreeElement) {
        return 1;
    } else {
        int i;
        HashPosition pos = {0};
        zend_hash_internal_pointer_reset_ex(Z_ARRVAL_P(array), &pos);
        for (i = 0; i < count; i++) {
            if (zend_hash_get_current_key_type_ex(Z_ARRVAL_P(array), &pos) != HASH_KEY_IS_LONG) {
                return 1;
            }
            zend_hash_move_forward_ex(Z_ARRVAL_P(array), &pos);
        }
    }
    return 0;
}

static ssize_t binpack_write_buffer(void *buf, const void *data, size_t len)
{
	smart_str *str = (smart_str *)buf;
	if (len == 1)
		smart_str_appendc(str, *(char *)data);
	else
		smart_str_appendl(str, data, len);
	return len;
}

static void binpack_encode_array(bin_packer_t *pk, zval *arr TSRMLS_DC)
{
	HashTable *ht = Z_ARRVAL_P(arr);
	bool is_dict = false;
	int num;

	if (Z_TYPE_P(arr) == IS_ARRAY) {
		ht = HASH_OF(arr);
		is_dict = binpack_check_ht_is_map(arr TSRMLS_CC);
	} else {
		ht = Z_OBJPROP_P(arr);
		is_dict = true;
	}

	if (ht->nApplyCount > 1)
	{
		php_error_docref(NULL TSRMLS_CC, E_RECOVERABLE_ERROR, "[binpack] (binpack_encode_array) circular references is unsupported");
		bin_pack_null(pk);
		return;
	}

	ht->nApplyCount++;

	num = ht ? zend_hash_num_elements(ht) : 0;

	if (is_dict)
		bin_pack_open_dict(pk);
	else
		bin_pack_open_list(pk);

	if (num > 0)
	{
		ulong idx;
		char *key;
		uint key_len;
		HashPosition pos;
		HashTable *tmp_ht;
		zval **data;

		zend_hash_internal_pointer_reset_ex(ht, &pos);
		for (; true; zend_hash_move_forward_ex(ht, &pos))
		{
			int x = zend_hash_get_current_key_ex(ht, &key, &key_len, &idx, 0, &pos);
			if (x == HASH_KEY_NON_EXISTANT)
				break;

			if (zend_hash_get_current_data_ex(ht, (void **) &data, &pos) != SUCCESS)
				continue;

			tmp_ht = HASH_OF(*data);
			if (tmp_ht)
				tmp_ht->nApplyCount++;

			if (is_dict)
			{
				if (x == HASH_KEY_IS_STRING)
				{
					if (key[0] == '\0' && Z_TYPE_P(arr) == IS_OBJECT)
					{
						/* Skip protected and private members. */
						if (tmp_ht)
							tmp_ht->nApplyCount--;
						continue;
					}
					/* The key length including the trailing NUL character. */
					bin_pack_lstring(pk, key, (key_len - 1));
				}
				else
				{
					bin_pack_integer(pk, idx);
				}
			}

			binpack_do_encode(pk, *data TSRMLS_CC);

			if (tmp_ht)
				tmp_ht->nApplyCount--;
		}
	}
	bin_pack_closure(pk);

	ht->nApplyCount--;
}

static bool binpack_make_list(bin_unpacker_t *uk, zval *val TSRMLS_DC)
{
	array_init(val);
	while (true)
	{
		zval *item;
		int type = binpack_do_decode(uk, &item TSRMLS_CC);
		if (type == BIN_TYPE_CLOSURE)
		{
			break;
		}
		if (type == -1)
		{
			zend_error(E_WARNING, "unexpect type when decode value for list, type: 0x%02x line: %d", type, __LINE__);
			zval_dtor(val);

			return false;
		}
		add_next_index_zval(val, item);
	}
	return true;
}

static bool binpack_make_dict(bin_unpacker_t *uk, zval *dict TSRMLS_DC)
{
	array_init(dict);
	while (true)
	{
		uintmax_t num;
		int key_type = bin_unpack_type(uk, &num);
		if (key_type == BIN_TYPE_CLOSURE)
		{
			break;
		}
		if (key_type == -1)
		{
			zend_error(E_WARNING, "unexpect type when decode key for dictionary, type: 0x%02x, line: %d", key_type, __LINE__);
			zval_dtor(dict);
			return false;
		}

		if (key_type == BIN_TYPE_STRING)
		{
			char *key = uk->buf + uk->pos;
			uk->pos += num;

			zval *dict_val;
			int val_type = binpack_do_decode(uk, &dict_val TSRMLS_CC);
			if (val_type == BIN_TYPE_CLOSURE || val_type == -1)
			{
				zval_dtor(dict);
				zend_error(E_WARNING, "unexpect type when decode value for dictionary, type: 0x%02x, line: %d", val_type, __LINE__);
				return false;
			}

			char tmp_ch = key[num];
			key[num] = 0;
			add_assoc_zval(dict, key, dict_val);
			key[num] = tmp_ch;
		}
		else
		{
			zval *dict_val;
			int val_type = binpack_do_decode(uk, &dict_val TSRMLS_CC);
			if (val_type == BIN_TYPE_CLOSURE || val_type == -1)
			{
				zval_dtor(dict);
				zend_error(E_WARNING, "unexpect type when decode value for dictionary, type: 0x%02x, line: %d", val_type, __LINE__);
				return false;
			}

			int sign = key_type & BIN_MASK_INTEGER_SIGN;
			// if (num <= LONG_MAX || (sign && num <= LONG_MAX + 1))
			if (num <= LONG_MAX + sign)
			{
				if (sign)
					add_index_zval(dict, -num, dict_val);
				else
					add_index_zval(dict, num, dict_val);
			}
			else
			{
				char tmp[40], *key;
				char *end = tmp + sizeof(tmp) - 1;
				*end = 0;
				key = _itoa(end, num);
				if (sign)
				{
					*--key = '-';
				}
				add_assoc_zval(dict, key, dict_val);
			}
		}
	}
	return true;
}

static void binpack_do_encode(bin_packer_t *pk, zval *val TSRMLS_DC)
{
	switch(Z_TYPE_P(val)) {
		case IS_STRING:
			bin_pack_lstring(pk, Z_STRVAL_P(val), Z_STRLEN_P(val));
			break;
			
		case IS_LONG:
			bin_pack_integer(pk, Z_LVAL_P(val));
			break;

		case IS_BOOL:
			bin_pack_bool(pk, Z_BVAL_P(val));
			break;

		case IS_NULL:
			bin_pack_null(pk);
			break;
		
		case IS_DOUBLE:
			bin_pack_float_double(pk, Z_DVAL_P(val));
			break;

		case IS_OBJECT:
			/* TODO: pack blob
			 * as in php, there is no difference between string and blob,
			 * we encode blob as string
			 *
			if (zend_get_class_entry(val TSRMLS_CC) == bin_ce_BlobObject)
			{
				blob_object *intern = (blob_object *)zend_object_store_get_object(val TSRMLS_CC);
				bin_pack_blob(pk, Z_STRVAL_P(intern->value), Z_STRLEN_P(intern->value));
				break;
			}
			*/
			/* fall through */
		case IS_ARRAY:
			binpack_encode_array(pk, val TSRMLS_CC);
			break;

		default:
			zend_error(E_WARNING, "[binpack] (binpack_do_encode) unknown type(%d), encoded as null.", Z_TYPE_P(val));
			bin_pack_null(pk);
	}
}

static int binpack_do_decode(bin_unpacker_t *uk, zval **val TSRMLS_DC)
{
	uintmax_t num;
	int type = bin_unpack_type(uk, &num);
	if (type == BIN_TYPE_CLOSURE)
	{
		return type;
	}
	else
	{
		ALLOC_INIT_ZVAL(*val);

		if (type == BIN_TYPE_INTEGER || type == BIN_TYPE_INTEGER_NEGATIVE)
		{
			int sign = type & BIN_MASK_INTEGER_SIGN;
			// if (num <= LONG_MAX || (sign && num <= LONG_MAX + 1))
			// more efficent
			if (num <= LONG_MAX + sign)
			{
				if (sign)
				{
					ZVAL_LONG(*val, -num);
				}
				else
				{
					ZVAL_LONG(*val, num);
				}

				type = BIN_TYPE_INTEGER;
			}
			else
			{
				// reach long limit, convert to string
				char tmp[40], *s;
				char *end = tmp + sizeof(tmp);
				s = _itoa(end, num);
				if (sign)
				{
					*--s = '-';
				}
				ZVAL_STRINGL(*val, s, end - s, 1);

				type = BIN_TYPE_STRING;
			}
		}
		else if (type == BIN_TYPE_STRING)
		{
			ZVAL_STRINGL(*val, uk->buf + uk->pos, num, 1);
			uk->pos += num;
		}
		else if (type == BIN_TYPE_BLOB)
		{
			/* in php, blob is string */
			ZVAL_STRINGL(*val, uk->buf + uk->pos, num, 1);
			uk->pos += num;
		}
		else if (type == BIN_TYPE_BOOL || type == BIN_TYPE_BOOL_FALSE)
		{
			ZVAL_BOOL(*val, type == BIN_TYPE_BOOL); 
			type = BIN_TYPE_BOOL;
		}
		else if (type == BIN_TYPE_FLOAT_DOUBLE)
		{
			double value = bin_make_double(uk);
			ZVAL_DOUBLE(*val, value);
		}
		else if (type == BIN_TYPE_FLOAT_SINGLE)
		{
			double value = (double)bin_make_float(uk);
			ZVAL_DOUBLE(*val, value);

			type = BIN_TYPE_FLOAT_DOUBLE;;
		}
		else if (type == BIN_TYPE_LIST)
		{
			bool make_list = binpack_make_list(uk, *val TSRMLS_CC);
			if (!make_list)
			{
				goto error;
			}
		}
		else if (type == BIN_TYPE_DICT)
		{
			bool make_dict = binpack_make_dict(uk, *val TSRMLS_CC);
			if (!make_dict)
			{
				goto error;
			}
		}
		else if (type == BIN_TYPE_NULL)
		{
			ZVAL_NULL(*val);
		}
		else if (type == BIN_TYPE_CLOSURE)
		{
			// do nothing, will never hit here unless someone do somthing nasty
			goto error;
		}
		else
		{
			goto error;
		}
		return type;
	}
error:
	FREE_ZVAL(*val);
	return -1;
}

/* 
 * bin_encode, let us start here.
 *
 * {{{ proto: string bin_encode(mixed $value)
 *   return string after encode
 */
PHP_FUNCTION(bin_encode)
{
	zval *val;
	smart_str buf = {0};

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "z", &val) == FAILURE) {
		return;
	}

	bin_packer_t pk;
	bin_packer_init(&pk, binpack_write_buffer, &buf);

	binpack_do_encode(&pk, val TSRMLS_CC);

	ZVAL_STRINGL(return_value, buf.c, buf.len, 1);																  
	smart_str_free(&buf);
}
/* }}} */
/* 
 * end of bin_encode
 */


/*
 * bin_decode
 * {{{ proto mixed bin_decode(string $str)
 */
PHP_FUNCTION(bin_decode)
{
	char *str;
	int str_len;
	zval *len;

	zval *ret;
	bin_unpacker_t uk;

	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "s|z", &str, &str_len, &len) == FAILURE) 
	{
		return;
	}

	bin_unpacker_init(&uk, str, str_len);
	int result = binpack_do_decode(&uk, &ret TSRMLS_CC);
	if (result == -1) 
	{
		ZVAL_NULL(return_value);
		return;
	}
	else
	{
		*return_value = *ret;

		FREE_ZVAL(ret);
	}
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
