
#include "cefFrameBuffer.h"

#include <iostream>     // std::cout
#include <new>
#include <typeinfo>

#include <include/cef_app.h>
#include <include/cef_client.h>
#include <include/cef_render_handler.h>


class CefSimpleRenderHandler : public CefRenderHandler
{

  int width;
  int height;

public:
  CefSimpleRenderHandler(int width, int height) {
    this->width = width;
    this->height = height;
  }

public:
  bool GetViewRect(CefRefPtr<CefBrowser> browser, CefRect &rect) {
    rect = CefRect(0, 0, width, height);
    return true;
  }

public:
  void OnPaint(CefRefPtr<CefBrowser> browser, PaintElementType paintType, const RectList &rects, 
               const void *buffer, int width, int height) {
  }

public:
  IMPLEMENT_REFCOUNTING(CefSimpleRenderHandler);
};

class CefSimpleClient : public CefClient {

  CefRefPtr<CefRenderHandler> renderHandler;

public:
  CefSimpleClient(CefRenderHandler* simpleRenderHandler) {
    renderHandler = simpleRenderHandler;
  }

public:
  ~CefSimpleClient() { }

public:
  CefRefPtr<CefRenderHandler> GetRenderHandler() {
    return renderHandler;
  }

public:
  IMPLEMENT_REFCOUNTING(CefSimpleClient);
};


class CefFrameBuffer : public cef_frame_buffer_t {

  CefSimpleRenderHandler* simpleRenderHandler;

public:
  CefFrameBuffer(const char *url, uint16_t width, uint16_t height) {
    std::cout << "constructed [" << this << "]\n";

    simpleRenderHandler = new CefSimpleRenderHandler(width, height);

    CefSettings settings;
    const CefMainArgs cefMainArgs = const_cast<CefMainArgs> (new CefMainArgs());
    CefInitialize(cefMainArgs, settings, nullptr);

    CefRefPtr<CefClient> client = new CefSimpleClient(
      dynamic_cast<CefRenderHandler*>(&simpleRenderHandler));

    CefBrowserSettings browserSettings;
    CefWindowInfo windowInfo;
    windowInfo.SetAsOffScreen(nullptr);

    CefRefPtr<CefBrowser> browser;
    browser = CefBrowserHost::CreateBrowserSync(windowInfo, client.get(),
                                                url, browserSettings, nullptr);
  };
  
  ~CefFrameBuffer() {
    CefShutdown();
    delete simpleRenderHandler;
  };

  void nextFrame(uint8_t *buffer) {
    CefRunMessageLoop();
  };
};

inline CefFrameBuffer* real(cef_frame_buffer_t* cef_frame_buffer) { return static_cast<CefFrameBuffer*>(cef_frame_buffer); }

cef_frame_buffer_t* cef_frame_buffer_init(const char *url, uint16_t width, uint16_t height) { return new CefFrameBuffer(url, width, height); }
void cef_frame_buffer_get_next_frame(cef_frame_buffer_t *cef_frame_buffer, uint8_t *buffer) { real(cef_frame_buffer)->nextFrame(buffer); }
void cef_frame_buffer_deinit(cef_frame_buffer_t *cef_frame_buffer) { delete real(cef_frame_buffer); }
