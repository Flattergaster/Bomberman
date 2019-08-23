AM_CFLAGS = -I$(top_srcdir)/client/include
AM_CFLAGS += -I$(top_srcdir)/shared/include

bin_PROGRAMS = client
client_SOURCES = client.c
client_LDADD = ../lib/libcore.la
client_LDADD += ../lib/libgui.la
client_LDADD += $(top_builddir)/shared/src/libutils.la
client_LDADD += -lncursesw
