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
 * gst-launch -v -m chromiumembedded verbose=true ! fakesink
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <stdint.h>
#include <glib.h>

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
  PROP_VERBOSE,
  PROP_URL
};

/* the capabilities of the inputs and outputs. */
#define VTS_VIDEO_CAPS GST_VIDEO_CAPS_MAKE ("BGRA")

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

static gboolean gst_chromium_embedded_setcaps (GstBaseSrc * bsrc, GstCaps * caps);
static GstCaps *gst_chromium_embedded_src_fixate (GstBaseSrc * parent, GstCaps * caps);
static gboolean gst_chromium_embedded_is_seekable (GstBaseSrc * psrc);

static GstFlowReturn gst_chromium_embedded_fill (GstPushSrc * psrc, GstBuffer * buffer);

/* GObject vmethod implementations */

/* 
 * initialize the chromiumembedded's class
 * Initialise the class only once
 *   (specifying what signals, arguments and virtual
 *    functions the class has and setting up global state)
 */
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

  g_object_class_install_property (gobject_class, PROP_VERBOSE,
      g_param_spec_boolean ("verbose", "Verbose", "Produce verbose output",
          FALSE, G_PARAM_READWRITE));

    g_object_class_install_property (gobject_class, PROP_URL,
      g_param_spec_string ("url", "URL", "URL to video",
          "http://www.google.com/", G_PARAM_READWRITE));

  gst_element_class_set_details_simple(gstelement_class,
    "ChromiumEmbedded",
    "Source/Video",
    "Creates a Chromium Embedded video stream",
    "Guy Taylor <thebigguy.co.uk@gmail.com>");

  gst_element_class_add_pad_template (gstelement_class,
      gst_static_pad_template_get (&src_factory));

  gstbasesrc_class->set_caps = gst_chromium_embedded_setcaps;
  gstbasesrc_class->fixate = gst_chromium_embedded_src_fixate;
  gstbasesrc_class->is_seekable = gst_chromium_embedded_is_seekable;

  gstpushsrc_class->fill = gst_chromium_embedded_fill;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 *
 * initialise a specific instance of this type
 */
static void
gst_chromium_embedded_init (GstChromiumEmbedded * src)
{
  /* custom props */
  src->verbose = TRUE;

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
  gst_structure_fixate_field_nearest_fraction (structure, "framerate", 25, 1);

  caps = GST_BASE_SRC_CLASS (parent_class)->fixate (bsrc, caps);

  return caps;
}

static void
gst_chromium_embedded_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
  GstChromiumEmbedded *src = GST_CHROMIUMEMBEDDED (object);

  switch (prop_id) {
    case PROP_VERBOSE:
      src->verbose = g_value_get_boolean (value);
      break;
    case PROP_URL:
      src->url = g_value_get_string (value);
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
  GstChromiumEmbedded *src = GST_CHROMIUMEMBEDDED (object);

  switch (prop_id) {
    case PROP_VERBOSE:
      g_value_set_boolean (value, src->verbose);
      break;
    case PROP_URL:
      g_value_set_string (value, src->url);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static gboolean
gst_chromium_embedded_setcaps (GstBaseSrc * bsrc, GstCaps * caps)
{
  GstChromiumEmbedded *src;
  GstVideoInfo info;

  src = GST_CHROMIUMEMBEDDED (bsrc);

  /* we can use the parsing code */
  if (!gst_video_info_from_caps (&info, caps))
    goto parse_failed;

  /* looks ok here */
  src->info = info;

  GST_DEBUG_OBJECT (src, "size %dx%d, %d/%d fps",
      info.width, info.height, info.fps_n, info.fps_d);

  src->n_frames = 0;

  // temp
  if (src->fp != NULL) {
    fclose(src->fp);
    src->fp = NULL;
  }
  src->fp = fopen("/home/guytay/Videos/big_buck_bunny.bgra_320x240_25fps.raw", "rb");

  if (src->cef_frame_buffer != NULL) {
    cef_frame_buffer_deinit(src->cef_frame_buffer);
    src->cef_frame_buffer = NULL;
  }
  //src->cef_frame_buffer = cef_frame_buffer_init(src->url, info.width, info.height);
  

  return TRUE;

  /* ERRORS */
parse_failed:
  {
    GST_DEBUG_OBJECT (bsrc, "failed to parse caps");
    return FALSE;
  }
}

static gboolean
gst_chromium_embedded_is_seekable (GstBaseSrc * psrc)
{
  /* we're are NOT seekable... */
  return FALSE;
}

/* this function does the actual processing */
static GstFlowReturn
gst_chromium_embedded_fill (GstPushSrc * psrc, GstBuffer * buffer)
{
  GstChromiumEmbedded *src;
  GstClockTime next_time;
  GstVideoFrame frame_on_the_stack;
  GstVideoFrame* frame;
  GstVideoInfo *info;
  gconstpointer pal;
  gsize palsize;
  guint h;
  guint w;
  guint d;
  guint n;
  GstVideoFormatPack pack_func;
  uint8_t *buf;

  src = GST_CHROMIUMEMBEDDED (psrc);

  if (G_UNLIKELY (GST_VIDEO_INFO_FORMAT (&src->info) ==
          GST_VIDEO_FORMAT_UNKNOWN))
    goto not_negotiated;

  if (src->fp == NULL)
    goto not_negotiated;

  /* 0 framerate and we are at the second frame, eos */
  if (G_UNLIKELY (src->info.fps_n == 0 && src->n_frames == 1))
    goto eos;

  if (feof(src->fp)) {
    /* EOS when file error or file eof */
    goto eos;
  }

  GST_LOG_OBJECT (src,
       "creating buffer from pool for frame %d", (gint) src->n_frames);

  if (!gst_video_frame_map (&frame_on_the_stack, &src->info, buffer, GST_MAP_WRITE))
    goto invalid_frame;
  frame = &frame_on_the_stack;

  GST_BUFFER_DTS (buffer) = src->accum_rtime + src->timestamp_offset + src->running_time;
  GST_BUFFER_PTS (buffer) = GST_BUFFER_DTS (buffer);

  gst_object_sync_values (GST_OBJECT (psrc), GST_BUFFER_DTS (buffer));

  info = &frame->info;
  w = GST_VIDEO_INFO_WIDTH(info);
  h = GST_VIDEO_INFO_HEIGHT(info);
  n = GST_VIDEO_INFO_N_COMPONENTS(info);
  d = GST_VIDEO_FORMAT_INFO_DEPTH (gst_video_format_get_info (info->finfo->unpack_format), 0) / 8;
  pack_func = info->finfo->pack_func;

  // TODO: Keep the same buffer, only remalloc on size change
  buf = g_malloc(w * h * n * d);

  //cef_frame_buffer_get_next_frame(src->cef_frame_buffer, buf);

  if (fread(buf, 1, w * h * n * d, src->fp) != w * h * n * d)
    goto error;
  pack_func(info->finfo, GST_VIDEO_PACK_FLAG_NONE,
    buf, 0,
    frame->data, (*info).stride, (*info).chroma_site,
    0, w * h);
  g_free (buf);

  if ((pal = gst_video_format_get_palette (GST_VIDEO_FRAME_FORMAT (frame),
              &palsize))) {
    memcpy (GST_VIDEO_FRAME_PLANE_DATA (frame, 1), pal, palsize);
  }

  gst_video_frame_unmap (frame);

  GST_DEBUG_OBJECT (src, "Timestamp: %" GST_TIME_FORMAT " = accumulated %"
      GST_TIME_FORMAT " + offset: %"
      GST_TIME_FORMAT " + running time: %" GST_TIME_FORMAT,
      GST_TIME_ARGS (GST_BUFFER_PTS (buffer)), GST_TIME_ARGS (src->accum_rtime),
      GST_TIME_ARGS (src->timestamp_offset), GST_TIME_ARGS (src->running_time));

  GST_BUFFER_OFFSET (buffer) = src->accum_frames + src->n_frames;
  src->n_frames++;

  GST_BUFFER_OFFSET_END (buffer) = GST_BUFFER_OFFSET (buffer) + 1;
  if (src->info.fps_n) {
    next_time = gst_util_uint64_scale_int (src->n_frames * GST_SECOND,
        src->info.fps_d, src->info.fps_n);
    GST_BUFFER_DURATION (buffer) = next_time - src->running_time;
  } else {
    next_time = src->timestamp_offset;
    /* NONE means forever */
    GST_BUFFER_DURATION (buffer) = GST_CLOCK_TIME_NONE;
  }

  src->running_time = next_time;

  return GST_FLOW_OK;

not_negotiated:
  {
    GST_DEBUG_OBJECT (src, "Not negotiated");
    return GST_FLOW_NOT_NEGOTIATED;
  }
eos:
  {
    GST_DEBUG_OBJECT (src, "eos: 0 framerate, frame %d", (gint) src->n_frames);
    return GST_FLOW_EOS;
  }
invalid_frame:
  {
    GST_DEBUG_OBJECT (src, "invalid frame");
    return GST_FLOW_OK;
  }
error:
  {
    GST_DEBUG_OBJECT (src, "Error");
    return GST_FLOW_ERROR;
  }
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
