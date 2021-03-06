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

#ifndef __GST_CHROMIUMEMBEDDED_H__
#define __GST_CHROMIUMEMBEDDED_H__

#include <stdio.h>

#include <gst/gst.h>

#include <gst/base/gstpushsrc.h>

#include <gst/video/gstvideometa.h>
#include <gst/video/video.h>

#include "cefFrameBuffer.h" 

G_BEGIN_DECLS

/* #defines don't like whitespacey bits */
#define GST_TYPE_CHROMIUMEMBEDDED \
  (gst_chromium_embedded_get_type())
#define GST_CHROMIUMEMBEDDED(obj) \
  (G_TYPE_CHECK_INSTANCE_CAST((obj),GST_TYPE_CHROMIUMEMBEDDED,GstChromiumEmbedded))
#define GST_CHROMIUMEMBEDDED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_CAST((klass),GST_TYPE_CHROMIUMEMBEDDED,GstChromiumEmbeddedClass))
#define GST_IS_CHROMIUMEMBEDDED(obj) \
  (G_TYPE_CHECK_INSTANCE_TYPE((obj),GST_TYPE_CHROMIUMEMBEDDED))
#define GST_IS_CHROMIUMEMBEDDED_CLASS(klass) \
  (G_TYPE_CHECK_CLASS_TYPE((klass),GST_TYPE_CHROMIUMEMBEDDED))

typedef struct _GstChromiumEmbedded      GstChromiumEmbedded;
typedef struct _GstChromiumEmbeddedClass GstChromiumEmbeddedClass;

struct _GstChromiumEmbedded
{
  GstPushSrc element;

  /*< private >*/

  /* verbose logging */
  gboolean verbose;

  cef_frame_buffer_t *cef_frame_buffer;

  const char *url;

  /* video state */
  GstVideoInfo info;

  gint64 timestamp_offset;              /* base offset */

  /* running time and frames for current caps */
  GstClockTime running_time;    /* total running time */
  gint64 n_frames;              /* total frames sent */

  /* previous caps running time and frames */
  GstClockTime accum_rtime;    /* accumulated running_time */
  gint64 accum_frames;         /* accumulated frames */

  /* test file */
  FILE *fp;
};

struct _GstChromiumEmbeddedClass 
{
  GstPushSrcClass parent_class;
};

GType gst_chromium_embedded_get_type (void);

G_END_DECLS

#endif /* __GST_CHROMIUMEMBEDDED_H__ */
