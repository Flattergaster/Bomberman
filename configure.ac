AC_INIT([Bomberman], [0.1.0])
AM_INIT_AUTOMAKE([-Wall -Werror foreign])

m4_ifdef([AM_PROG_AR], [AM_PROG_AR])

#AC_CONFIG_MACRO_DIRS([m4])

AC_PROG_CC
AC_PROG_CC_STDC
AC_PROG_LIBTOOL

AC_CHECK_LIB(socket, connect)
AC_CHECK_LIB(pthread, pthread_create)

AC_CONFIG_HEADERS([config.h])

AC_HEADER_STDC
AC_CHECK_HEADERS([ncurses.h pthread.h])

AC_CONFIG_FILES([
	Makefile

	server/Makefile
	server/src/Makefile
	server/lib/Makefile
	
	client/Makefile
	client/src/Makefile
	client/lib/Makefile

	shared/Makefile
	shared/src/Makefile
	])

AC_OUTPUT
