#!/bin/bash

export GST_PLUGIN_PATH="`pwd`/src/.libs:${GST_PLUGIN_PATH}"
gdb -ex r --args gst-launch-1.0 chromiumembedded ! autovideosink

