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

#ifndef BINPACK_ENDIAN_H_
#define BINPACK_ENDIAN_H_


/*
 * use ntohl() and bsswap_64() [or the other corresponding function in other platform] to swap bit order by __LITTLE_ENDIAN
 * 

http://forums.codeguru.com/showthread.php?298741-C-General-What-do-ntohl()-and-htonl()-actually-do

  1. 'ntohs()', 'ntohl()', 'htons()', and 'htonl()' are not part of the C standard, and thus do not guarentee portability. 
  2. The POSIX implementations of 'ntohs()', 'ntohl()', 'htons()' and 'htonl()' 
     take arguments of 'uint16_t' and 'uint32_t' argument types and can be found in the header file netinet/in.h. 
  3. The Windows implementations use 'unsigned short' and 'unsigned long' and can be found in the header file winsock2.h.
  4. Other variants of 'ntoht()' and 'htont()' may exist on some sytems, such as 'ntohi()'/'htoni()' or 'ntohll()'/'htonll()'.

*/


/* for uint16_t / uint32_t / uint64_t */
#if defined(_MSC_VER) && _MSC_VER < 1600
typedef __int8 int8_t;
typedef unsigned __int8 uint8_t;
typedef __int16 int16_t;
typedef unsigned __int16 uint16_t;
typedef __int32 int32_t;
typedef unsigned __int32 uint32_t;
typedef __int64 int64_t;
typedef unsigned __int64 uint64_t;
#elif defined(_MSC_VER)     /* && _MSC_VER >= 1600 */
#include <stdint.h>         
#else
#include <stdint.h>
#endif

#ifdef _WIN32
#include <winsock2.h>

#ifdef __cplusplus
/* numeric_limits<T>::min,max */
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#endif

#else
#include <endian.h>
#endif


#if __BYTE_ORDER == __LITTLE_ENDIAN

#define _binpack_be32(x) ntohl(x)

/* windows */
#if defined(_byteswap_uint64) || _MSC_VER >= 1400
#  define _binpack_be64(x) (_byteswap_uint64(x))
/* linux */
#elif defined(bswap_64)
#  define _binpack_be64(x) bswap_64(x)
/* Mac OS */
#elif defined(__DARWIN_OSSwapInt64) 
#  define _binpack_be64(x) __DARWIN_OSSwapInt64(x)
#else
#define _binpack_be64(x) \
	( ((((uint64_t)x) << 56) & 0xff00000000000000ULL ) | \
	  ((((uint64_t)x) << 40) & 0x00ff000000000000ULL ) | \
	  ((((uint64_t)x) << 24) & 0x0000ff0000000000ULL ) | \
	  ((((uint64_t)x) <<  8) & 0x000000ff00000000ULL ) | \
	  ((((uint64_t)x) >>  8) & 0x00000000ff000000ULL ) | \
	  ((((uint64_t)x) >> 24) & 0x0000000000ff0000ULL ) | \
	  ((((uint64_t)x) >> 40) & 0x000000000000ff00ULL ) | \
	  ((((uint64_t)x) >> 56) & 0x00000000000000ffULL ) )
#endif

#elif __BYTE_ORDER == __BIG_ENDIAN
#define _binpack_be32(x) (x)
#define _binpack_be64(x) (x)
#else
#error Not supported __BYTE_ORDER
#endif

#endif
