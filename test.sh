#!/bin/bash

export GST_PLUGIN_PATH="`pwd`/src/.libs:${GST_PLUGIN_PATH}"
# autovideoconvert, autovideoconvert
gdb -ex r --args gst-launch-1.0 --verbose --messages chromiumembedded verbose=true ! autovideoconvert ! autovideosink

