#ifndef WINDOWSVIEWPORT_H
#define WINDOWSVIEWPORT_H

#include "renderer.h"
#include <windows.h>

class WindowsApplication;

DECLARE_EXCEPTION(WindowsException)

class WindowsViewport : public rend::Viewport
{
    HWND m_hWnd;
    HDC m_hDC;

public:
    WindowsViewport(WindowsApplication *app,
                    int width, int height,
                    boost::shared_ptr<rend::Camera> camera);
    ~WindowsViewport();

    void frameBegin();
    void flush(unsigned char *pixels);
};

#endif // WINDOWSVIEWPORT_H
