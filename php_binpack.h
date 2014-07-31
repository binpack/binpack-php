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

#ifndef PHP_BINPACK_H
#define PHP_BINPACK_H

#define BINPACK_EXTENSION_VERSION "1.0.1"

extern zend_module_entry binpack_module_entry;
#define phpext_binpack_ptr &binpack_module_entry

#ifdef PHP_WIN32
#	define PHP_BINPACK_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_BINPACK_API __attribute__ ((visibility("default")))
#else
#	define PHP_BINPACK_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(binpack);
PHP_MSHUTDOWN_FUNCTION(binpack);
PHP_RINIT_FUNCTION(binpack);
PHP_RSHUTDOWN_FUNCTION(binpack);
PHP_MINFO_FUNCTION(binpack);

/* 
 * proto: string bin_encode(mixed $val)
 * Return a encoded string.
 */
PHP_FUNCTION(bin_encode);

/* 
 * proto: mixed bin_decode(string $str [, int &$used])
 * Return the decoded value 
 * If error, return NULL (and $used is 0).
 */
PHP_FUNCTION(bin_decode);


/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(binpack)
	long  global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(binpack)
*/

/* In every utility function you add that needs to use variables 
   in php_binpack_globals, call TSRMLS_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMLS_CC
   after the last function argument and declare your utility function
   with TSRMLS_DC after the last declared argument.  Always refer to
   the globals in your function as BINPACK_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define BINPACK_G(v) TSRMG(binpack_globals_id, zend_binpack_globals *, v)
#else
#define BINPACK_G(v) (binpack_globals.v)
#endif

#endif	/* PHP_BINPACK_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
