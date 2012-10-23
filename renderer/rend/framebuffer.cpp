/*
 * framebuffer.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: flamingo
 */

#include "framebuffer.h"

namespace rend
{

void memset32(void *dest, uint32_t data, int count)
{
    asm ("cld\n\t"
         "rep\n\t"
         "stosl"
         :
         : "c" (count), "a" (data), "D" (dest)
         );
}

FrameBuffer::FrameBuffer(int w, int h)
    : m_pixels(0),
      m_zbuffer(0),
      m_width(w),
      m_height(h),
      m_xOrigin(0),
      m_yOrigin(0),
      m_size(0)
{
    m_size = m_width * m_height;
    m_pixels = new rgb[m_size];
    m_zbuffer = new int[m_size];
}

FrameBuffer::~FrameBuffer()
{
    if (m_pixels)
        delete [] m_pixels;
    if (m_zbuffer)
        delete [] m_zbuffer;
}

void FrameBuffer::clear()
{
    memset(m_pixels, 0x00, sizeof(rgb) * m_width * m_height);
    memset32(m_zbuffer, std::numeric_limits<int>::max(), m_width * m_height);
}

void FrameBuffer::resize(int w, int h)
{
    m_width = w;
    m_height = h;
    m_size = w * h;

    if (m_pixels)
        delete [] m_pixels;
    if (m_zbuffer)
        delete [] m_zbuffer;

    m_pixels = new rgb[m_size];
    m_zbuffer = new int[m_size];

    clear();
}

}
