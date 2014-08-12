dnl $Id$
dnl config.m4 for extension binpack

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_WITH(binpack, for binpack support,
[  --with-binpack             Include binpack support])

dnl Otherwise use enable:

PHP_ARG_ENABLE(binpack, whether to enable binpack support,
[  --enable-binpack           Enable binpack support])

if test "$PHP_BINPACK" != "no"; then
  dnl Write more examples of tests here...

  dnl # --with-binpack -> check with-path
  dnl SEARCH_PATH="/usr/local /usr"     # you might want to change this
  dnl SEARCH_FOR="/include/binpack.h"  # you most likely want to change this
  dnl if test -r $PHP_BINPACK/$SEARCH_FOR; then # path given as parameter
  dnl   BINPACK_DIR=$PHP_BINPACK
  dnl else # search default path list
  dnl   AC_MSG_CHECKING([for binpack files in default path])
  dnl   for i in $SEARCH_PATH ; do
  dnl     if test -r $i/$SEARCH_FOR; then
  dnl       BINPACK_DIR=$i
  dnl       AC_MSG_RESULT(found in $i)
  dnl     fi
  dnl   done
  dnl fi
  dnl
  dnl if test -z "$BINPACK_DIR"; then
  dnl   AC_MSG_RESULT([not found])
  dnl   AC_MSG_ERROR([Please reinstall the binpack distribution])
  dnl fi

  dnl # --with-binpack -> add include path
  dnl PHP_ADD_INCLUDE($BINPACK_DIR/include)

  dnl # --with-binpack -> check for lib and symbol presence
  dnl LIBNAME=binpack # you may want to change this
  dnl LIBSYMBOL=binpack # you most likely want to change this 

  dnl PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  dnl [
  dnl   PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $BINPACK_DIR/lib, BINPACK_SHARED_LIBADD)
  dnl   AC_DEFINE(HAVE_BINPACKLIB,1,[ ])
  dnl ],[
  dnl   AC_MSG_ERROR([wrong binpack lib version or lib not found])
  dnl ],[
  dnl   -L$BINPACK_DIR/lib -lm
  dnl ])
  dnl
  dnl PHP_SUBST(BINPACK_SHARED_LIBADD)

  AC_CHECK_HEADERS([arpa/inet.h])
  PHP_NEW_EXTENSION(binpack, bin_pack.c php_binpack.c, $ext_shared)

  ifdef([PHP_INSTALL_HEADERS],
  [
    PHP_INSTALL_HEADERS([bin_pack.h], [php_binpack.h], [bin_pack_endian.h])
  ], [
    PHP_ADD_MAKEFILE_FRAGMENT
  ])

fi
