# Note: plugindir is set in configure

plugin_LTLIBRARIES = libgstchromiumembedded.la

# sources used to compile this plug-in
libgstchromiumembedded_la_SOURCES = gstchromiumembedded.c gstchromiumembedded.h cefFrameBuffer.cpp cefFrameBuffer.h

libgstchromiumembedded_la_CFLAGS = $(GST_CFLAGS)
libgstchromiumembedded_la_CPPFLAGS = $(GTK_CFLAGS) -I../cef3
libgstchromiumembedded_la_LIBADD = $(GST_LIBS) -L../cef3/bin/cef
libgstchromiumembedded_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) -lcef -lcef_dll_wrapper
libgstchromiumembedded_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstchromiumembedded.h cefFrameBuffer.h
