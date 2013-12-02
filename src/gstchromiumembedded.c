/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2013 Guy Taylor <thebigguy.co.uk@gmail.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-chromiumembedded
 *
 * FIXME:Describe chromiumembedded here.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! chromiumembedded ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>

#include "gstchromiumembedded.h"

GST_DEBUG_CATEGORY_STATIC (gst_chromium_embedded_debug);
#define GST_CAT_DEFAULT gst_chromium_embedded_debug

#define DEFAULT_IS_LIVE            TRUE

/* Filter signals and args */
enum
{
  /* FILL ME */
  LAST_SIGNAL
};

enum
{
  PROP_0,
  PROP_SILENT
};

/* the capabilities of the inputs and outputs. */
#define VTS_VIDEO_CAPS GST_VIDEO_CAPS_MAKE (GST_VIDEO_FORMATS_ALL) ";" \
  "video/x-raw, format=(string) { gbra }, "                            \
  "width = " GST_VIDEO_SIZE_RANGE ", "                                 \
  "height = " GST_VIDEO_SIZE_RANGE ", "                                \
  "framerate = " GST_VIDEO_FPS_RANGE

/* Fix sublime syntax " */

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS (VTS_VIDEO_CAPS)
    );

#define gst_chromium_embedded_parent_class parent_class
G_DEFINE_TYPE (GstChromiumEmbedded, gst_chromium_embedded, GST_TYPE_PUSH_SRC);

static void gst_chromium_embedded_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_chromium_embedded_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static GstCaps *gst_chromium_embedded_src_fixate (GstBaseSrc * parent, GstCaps * caps);
static gboolean gst_chromium_embedded_start (GstBaseSrc * parent);
static gboolean gst_chromium_embedded_stop (GstBaseSrc * parent);

static GstFlowReturn gst_chromium_embedded_fill (GstPushSrc * psrc, GstBuffer * buffer);

/* GObject vmethod implementations */

/* initialize the chromiumembedded's class */
static void
gst_chromium_embedded_class_init (GstChromiumEmbeddedClass * klass)
{
  GObjectClass *gobject_class;
  GstElementClass *gstelement_class;
  GstBaseSrcClass *gstbasesrc_class;
  GstPushSrcClass *gstpushsrc_class;

  gobject_class = (GObjectClass *) klass;
  gstelement_class = (GstElementClass *) klass;
  gstbasesrc_class = (GstBaseSrcClass *) klass;
  gstpushsrc_class = (GstPushSrcClass *) klass;

  gobject_class->set_property = gst_chromium_embedded_set_property;
  gobject_class->get_property = gst_chromium_embedded_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
          FALSE, G_PARAM_READWRITE));

  gst_element_class_set_details_simple(gstelement_class,
    "ChromiumEmbedded",
    "Source/Video",
    "Creates a Chromium Embedded video stream",
    "Guy Taylor <thebigguy.co.uk@gmail.com>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));

  gstbasesrc_class->fixate = gst_chromium_embedded_src_fixate;
  gstbasesrc_class->start = gst_chromium_embedded_start;
  gstbasesrc_class->start = gst_chromium_embedded_stop;

  gstpushsrc_class->fill = gst_chromium_embedded_fill;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_chromium_embedded_init (GstChromiumEmbedded * src)
{
  /* custom props */
  src->silent = FALSE;

  /* we operate in time */
  gst_base_src_set_format (GST_BASE_SRC (src), GST_FORMAT_TIME);
  gst_base_src_set_live (GST_BASE_SRC (src), DEFAULT_IS_LIVE);
}

static GstCaps *
gst_chromium_embedded_src_fixate (GstBaseSrc * bsrc, GstCaps * caps)
{
  GstStructure *structure;

  caps = gst_caps_make_writable (caps);

  structure = gst_caps_get_structure (caps, 0);

  gst_structure_fixate_field_nearest_int (structure, "width", 320);
  gst_structure_fixate_field_nearest_int (structure, "height", 240);
  gst_structure_fixate_field_nearest_fraction (structure, "framerate", 30, 1);

  caps = GST_BASE_SRC_CLASS (parent_class)->fixate (bsrc, caps);

  return caps;
}

static void
gst_chromium_embedded_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstChromiumEmbedded *filter = GST_CHROMIUMEMBEDDED (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_chromium_embedded_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
  GstChromiumEmbedded *filter = GST_CHROMIUMEMBEDDED (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_chromium_embedded_parse_caps (const GstCaps * caps,
    gint * width, gint * height, gint * fps_n, gint * fps_d,
    GstVideoColorimetry * colorimetry, gint * x_inv, gint * y_inv)
{
  const GstStructure *structure;
  GstPadLinkReturn ret;
  const GValue *framerate;

  GST_DEBUG ("parsing caps");

  structure = gst_caps_get_structure (caps, 0);

  ret = gst_structure_get_int (structure, "width", width);
  ret &= gst_structure_get_int (structure, "height", height);
  framerate = gst_structure_get_value (structure, "framerate");

  if (framerate) {
    *fps_n = gst_value_get_fraction_numerator (framerate);
    *fps_d = gst_value_get_fraction_denominator (framerate);
  } else
    goto no_framerate;

  return ret;

  /* ERRORS */
no_framerate:
  {
    GST_DEBUG ("chromiumembedded no framerate given");
    return FALSE;
  }
}

/* GstElement vmethod implementations */


/* chain function
 * this function does the actual processing
 */
static GstFlowReturn
gst_chromium_embedded_fill (GstPushSrc * psrc, GstBuffer * buffer)
{
  GstChromiumEmbedded *src;

  src = GST_CHROMIUMEMBEDDED (psrc);

 if (G_UNLIKELY (GST_VIDEO_INFO_FORMAT (&src->info) ==
          GST_VIDEO_FORMAT_UNKNOWN))
    goto not_negotiated;

  /* 0 framerate and we are at the second frame, eos */
  if (G_UNLIKELY (src->info.fps_n == 0 && src->n_frames == 1))
    goto eos;

  if (G_UNLIKELY (src->n_frames == -1)) {
    /* EOS for reverse playback */
    goto eos;
  }


  if (src->silent == FALSE)
    g_print ("Not finished yet so EOS\n");

  return GST_FLOW_EOS;

not_negotiated:
  {
    return GST_FLOW_NOT_NEGOTIATED;
  }
eos:
  {
    GST_DEBUG_OBJECT (src, "eos: 0 framerate, frame %d", (gint) src->n_frames);
    return GST_FLOW_EOS;
  }
}

static gboolean
gst_chromium_embedded_start (GstBaseSrc * basesrc)
{
  GstChromiumEmbedded *src = GST_CHROMIUMEMBEDDED (basesrc);

  GST_DEBUG_OBJECT (src, "start");

  src->n_frames = 0;

  gst_video_info_init (&src->info);

  return TRUE;
}

static gboolean
gst_chromium_embedded_stop (GstBaseSrc * basesrc)
{
  GstChromiumEmbedded *src = GST_CHROMIUMEMBEDDED (basesrc);

  GST_DEBUG_OBJECT (src, "stop");

  return TRUE;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean
chromiumembedded_init (GstPlugin * chromiumembedded)
{
  /* debug category for fltering log messages
   *
   * exchange the string 'Template chromiumembedded' with your description
   */
  GST_DEBUG_CATEGORY_INIT (gst_chromium_embedded_debug, "chromiumembedded",
      0, "Template chromiumembedded");

  return gst_element_register (chromiumembedded, "chromiumembedded", GST_RANK_NONE,
      GST_TYPE_CHROMIUMEMBEDDED);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "chromiumembedded"
#endif

GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    chromiumembedded,
    "Template chromiumembedded",
    chromiumembedded_init,
    VERSION,
    "LGPL",
    PACKAGE,
    "http://www.thebiggerguy.com/"
)
