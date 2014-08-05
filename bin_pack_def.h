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

#ifndef BIN_PACK_DEF_H_
#define BIN_PACK_DEF_H_ 1

#ifndef __bool_true_false_are_defined
    #ifdef _Bool
        #define bool                        _Bool
    #else
        #define bool                        int
    #endif
    #define true                            1
    #define false                           0
    #define __bool_true_false_are_defined   1
#endif

/*
 * for long max
 */
#if LONG_MAX == 0x7FFFFFFFL
    #define BIN_LONG_MAX 0x7FFFFFFFUL
    #define BIN_LONG_MIN 0x80000000UL
#else
    #define BIN_LONG_MAX 0X7FFFFFFFFFFFFFFFUL
    #define BIN_LONG_MIN 0X8000000000000000UL
#endif

/*
 * Use inlined functions for supported systems.
 */
#if defined(__GNUC__) || defined(__DMC__) || defined(__POCC__) || defined(__WATCOMC__) || defined(__SUNPRO_C)
#define BINPACK_INLINE inline
#elif defined(__BORLANDC__) || defined(_MSC_VER) || defined(__LCC__)
#define BINPACK_INLINE __inline
#else 
#define BINPACK_INLINE
#endif

#endif
