AM_CFLAGS = -I$(top_srcdir)/client/includes

bin_PROGRAMS = client
client_SOURCES = client.c
client_LDADD = ../lib/libcore.a
client_LDADD += ../lib/libgui.a
client_LDADD += $(top_srcdir)/shared/libutils.a
client_LDADD += -lncursesw
