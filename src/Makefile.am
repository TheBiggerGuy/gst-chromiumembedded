# Note: plugindir is set in configure

plugin_LTLIBRARIES = libgstchromiumembedded.la

# sources used to compile this plug-in
libgstchromiumembedded_la_SOURCES = gstchromiumembedded.c gstchromiumembedded.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstchromiumembedded_la_CFLAGS = $(GST_CFLAGS)
libgstchromiumembedded_la_LIBADD = $(GST_LIBS)
libgstchromiumembedded_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
libgstchromiumembedded_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstchromiumembedded.h
