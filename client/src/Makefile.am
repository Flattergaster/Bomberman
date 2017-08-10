AM_CFLAGS = -I$(top_srcdir)/client/includes

bin_PROGRAMS = client
client_SOURCES = client.c
client_LDADD = ../lib/libcore.la
client_LDADD += ../lib/libgui.la
client_LDADD += $(top_srcdir)/shared/src/libutils.la
client_LDADD += -lncursesw
