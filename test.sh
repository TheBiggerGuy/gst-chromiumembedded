#!/bin/bash

CEF_VERSION="cef_binary_3.1547.1406_linux"

export GST_PLUGIN_PATH="`pwd`/src/.libs:${GST_PLUGIN_PATH}"
export LD_LIBRARY_PATH="`pwd`/cef3/bin/cef:${GST_PLUGIN_PATH}:${LD_LIBRARY_PATH}"
# autovideoconvert, autovideoconvert
gdb -ex r --args gst-launch-1.0 --verbose --messages chromiumembedded verbose=true url="http://www.google.com/" ! autovideoconvert ! autovideosink

