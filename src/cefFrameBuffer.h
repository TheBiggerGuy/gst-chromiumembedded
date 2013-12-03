
#ifndef __CEF_FRAMEBUFFER_H__
#define __CEF_FRAMEBUFFER_H__

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct cef_frame_buffer_t { };
typedef struct cef_frame_buffer_t cef_frame_buffer_t;

cef_frame_buffer_t* cef_frame_buffer_init(const char *url, uint16_t width, uint16_t height);
void cef_frame_buffer_get_next_frame(cef_frame_buffer_t *cef_frame_buffer, uint8_t *buffer);
void cef_frame_buffer_deinit(cef_frame_buffer_t *cef_frame_buffer);

#ifdef __cplusplus
}
#endif

#endif /* __CEF_FRAMEBUFFER_H__ */