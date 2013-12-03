# Note: plugindir is set in configure

plugin_LTLIBRARIES = libgstchromiumembedded.la

# sources used to compile this plug-in
libgstchromiumembedded_la_SOURCES = gstchromiumembedded.c gstchromiumembedded.h cefFrameBuffer.cpp cefFrameBuffer.h

# compiler and linker flags used to compile this plugin, set in configure.ac
#libgstchromiumembedded_la_CFLAGS = $(GST_CFLAGS) $(GTK_CFLAGS) $(GLIB_CFLAGS) $(GTHREADS_CFLAGS) -I/home/guy/Code/git/gst-chromiumembedded/src/cef_binary_3.1720.1535_linux64
libgstchromiumembedded_la_CFLAGS = $(GST_CFLAGS)
libgstchromiumembedded_la_CPPFLAGS = $(GTK_CFLAGS) -I/home/guy/Code/git/gst-chromiumembedded/src/cef_binary_3.1720.1535_linux64
#libgstchromiumembedded_la_LIBADD = $(GST_LIBS) $(GTK_LIBS) $(GLIB_LIBS) $(GTHREADS_LIBS)
libgstchromiumembedded_la_LIBADD = $(GST_LIBS)
#libgstchromiumembedded_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) $(GTK_LDFLAGS) $(GLIB_LDFLAGS) $(GTHREADS_LDFLAGS) -L/home/guy/Code/git/gst-chromiumembedded/src/cef_binary_3.1720.1535_linux64/Debug -lcef3
libgstchromiumembedded_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) -L/home/guy/Code/git/gst-chromiumembedded/src/cef_binary_3.1720.1535_linux64/Debug -lcef3
libgstchromiumembedded_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstchromiumembedded.h
