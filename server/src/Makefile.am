AM_CFLAGS = -I$(top_srcdir)/server/includes

bin_PROGRAMS = server
server_SOURCES = main.c
server_LDADD = ../lib/libserver.a
server_LDADD += $(top_srcdir)/shared/libutils.a