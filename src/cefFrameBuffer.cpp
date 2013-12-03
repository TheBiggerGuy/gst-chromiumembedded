
#include "cefFrameBuffer.h"

#include <iostream>     // std::cout
#include <string>

#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_render_handler.h>


class CefSimpleRenderHandler : public CefRenderHandler
{
private:
  int width;
  int height;
  uint8_t *frame_buffer;

public:
  CefSimpleRenderHandler(int width, int height, uint8_t *frame_buffer) {
    this->width = width;
    this->height = height;
    this->frame_buffer = frame_buffer;
  }

  bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
    rect = CefRect(0, 0, width, height);
    return true;
  }

  void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType paintType, const RectList &rects, 
               const void *buffer, int width, int height) {
    memcpy(frame_buffer, buffer, width * height * 4 * 1);
  }

  IMPLEMENT_REFCOUNTING(CefSimpleRenderHandler);
};

class CefSimpleClient : public CefClient {
private:
  CefRefPtr<CefRenderHandler> renderHandler;

public:
  CefSimpleClient(CefRenderHandler* simpleRenderHandler) {
    this->renderHandler = simpleRenderHandler;
  }

  ~CefSimpleClient() { }

  CefRefPtr<CefRenderHandler> GetRenderHandler() {
    return renderHandler;
  }

  IMPLEMENT_REFCOUNTING(CefSimpleClient);
};


class CefFrameBuffer : public cef_frame_buffer_t {
private:
  CefSimpleRenderHandler* simpleRenderHandler;
  uint8_t *frame_buffer;

public:
  CefFrameBuffer(const char *url, uint16_t width, uint16_t height) {
    std::cout << "constructed [" << this << "]\n";

    this->simpleRenderHandler = new CefSimpleRenderHandler(width, height, frame_buffer);

    CefSettings settings;
    const CefMainArgs* cefMainArgs = new CefMainArgs(0, nullptr);
    CefInitialize(*cefMainArgs, settings, nullptr);
    delete cefMainArgs;

    CefRefPtr<CefClient> client(new CefSimpleClient(simpleRenderHandler));

    CefBrowserSettings browserSettings;
    CefWindowInfo windowInfo;
    windowInfo.SetAsOffScreen(nullptr);

    CefRefPtr<CefBrowser> browser;
    browser = CefBrowserHost::CreateBrowserSync(windowInfo, client.get(),
                                                url, browserSettings);
  }
  
  ~CefFrameBuffer() {
    CefShutdown();
    delete simpleRenderHandler;
  }

  void nextFrame(uint8_t *buffer) {
    CefRunMessageLoop();
    buffer = frame_buffer;
  }
};

inline CefFrameBuffer* real(cef_frame_buffer_t* cef_frame_buffer) { return static_cast<CefFrameBuffer*>(cef_frame_buffer); }

cef_frame_buffer_t* cef_frame_buffer_init(const char *url, uint16_t width, uint16_t height) { return new CefFrameBuffer(url, width, height); }
void cef_frame_buffer_get_next_frame(cef_frame_buffer_t *cef_frame_buffer, uint8_t *buffer) { real(cef_frame_buffer)->nextFrame(buffer); }
void cef_frame_buffer_deinit(cef_frame_buffer_t *cef_frame_buffer) { delete real(cef_frame_buffer); }
