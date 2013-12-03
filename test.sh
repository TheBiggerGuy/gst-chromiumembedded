#!/bin/bash

export GST_PLUGIN_PATH="`pwd`/src/.libs:${GST_PLUGIN_PATH}"
export LD_LIBRARY_PATH="`pwd`/src/cef_binary_3.1720.1535_linux64/Debug:${LD_LIBRARY_PATH}"
# autovideoconvert, autovideoconvert
gdb -ex r --args gst-launch-1.0 --verbose --messages chromiumembedded verbose=true ! autovideoconvert ! autovideosink

