 AC_INIT(engine3, 1.0)
AC_CANONICAL_SYSTEM

AC_CONFIG_AUX_DIR(build/unix/config)
AC_CONFIG_HEADER(config.h)

AM_INIT_AUTOMAKE([subdir-objects])

CXXFLAGS="$CXXFLAGS $C_EXTRA_FLAGS "

AC_PROG_CXX
#AC_PROG_RANLIB
AC_PROG_LIBTOOL

# checking for library dependencies.
#AC_CHECK_HEADER(lua.h, [], [AC_MSG_ERROR([*** lua.h is required, install lua header files])])	 AC_CHECK_HEADER([mysql/mysql.h], [], [AC_MSG_ERROR([*** mysql.h is required, install mysql header files])])
#AC_CHECK_HEADER(mysql/mysql.h, [], [AC_MSG_ERROR([*** mysql.h is required, install mysql header files])])	 AC_CHECK_HEADER([mysql/mysql.h], [], [AC_MSG_ERROR([*** mysql.h is required, install mysql header files])])

case "$host_cpu-$host_os" in
    *freebsd*)
      SYSTEM_INCLUDES="-I/usr/local/include"
      SYSTEM_LIBS="-lpthread -lexecinfo"
      USER_INCLUDES_PATH="/usr/local/include"
      USER_LIBS_PATH="/usr/local/lib"
      ;;
     *darwin*)
      SYSTEM_INCLUDES="-I/opt/local/include"
      SYSTEM_LIBS="-lpthread"
      USER_INCLUDES_PATH="/opt/local/include"
      USER_LIBS_PATH="/opt/local/lib"
      ;;
    *cygwin*)
      SYSTEM_INCLUDES=""
      SYSTEM_LIBS="-lpthread -lwsock32"
      USER_INCLUDES_PATH="/usr/local/include"
      USER_LIBS_PATH="/usr/local/lib"
      ;;
    *mingw*)
      SYSTEM_INCLUDES=""
      SYSTEM_LIBS="-lpthread-win32 -lwsock32"
      USER_INCLUDES_PATH="/local/include"
      USER_LIBS_PATH="/local/lib"
      ;;
    *686*linux*)
      SYSTEM_INCLUDES="" 
      SYSTEM_LIBS="-lrt -ldl -lpthread -lexecinfo"
      USER_INCLUDES_PATH="/usr/local/include"
      USER_LIBS_PATH="/usr/local/lib"
	  CXXFLAGS="$CXXFLAGS -m32 "
      ;;
    *)
      SYSTEM_INCLUDES="" 
      SYSTEM_LIBS="-lrt -ldl -lpthread -lexecinfo"
      USER_INCLUDES_PATH="/usr/local/include"
      USER_LIBS_PATH="/usr/local/lib"
esac

AM_CONDITIONAL(DEBUG, test "$enable_debug" = yes)

AC_SUBST(CFLAGS)
AC_SUBST(CXXFLAGS)

AC_SUBST(SYSTEM_INCLUDES)
AC_SUBST(SYSTEM_LIBS)

AC_SUBST(USER_INCLUDES_PATH)
AC_SUBST(USER_LIBS_PATH)
#AC_CONFIG_SUBDIRS([src/testsuite3])

AC_OUTPUT([Makefile src/Makefile])