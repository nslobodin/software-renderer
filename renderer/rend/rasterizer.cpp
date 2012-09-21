/*
 * rasterizer.cpp
 *
 *  Created on: Mar 10, 2012
 *      Author: flamingo
 */

#include "rasterizer.h"

#include "vec2.h"
#include "poly.h"
#include "viewport.h"

namespace rend
{

// helpers
enum TriangleType
{
    TT_FLAT_TOP,
    TT_FLAT_BOTTOM,
    TT_FLAT_GENERAL
};

void Rasterizer::drawBottomTriangle(int x1, int y1,
                                    int x2, int /*y2*/,
                                    int x3, int y3,
                                    const Color3 &color)
{
    // make ccw order
    if (x3 < x2)
        std::swap(x2, x3);

    double height = y3 - y1;
    double dxLeft = (x2 - x1) / height;
    double dxRight = (x3 - x1) / height;
    double xs = x1;
    double xe = x1;

    int iy1 = 0, iy3 = 0;

    // top of screen clipping
    if (y1 < m_fb.yorig())
    {
        xs = xs + dxLeft * (-y1 + m_fb.yorig());
        xe = xe + dxRight * (-y1 + m_fb.yorig());

        y1 = m_fb.yorig();
        iy1 = m_fb.yorig();     // ceiling
    }
    else
    {
        iy1 = y1;

        xs = xs + dxLeft * (iy1 - y1);
        xe = xe + dxRight * (iy1 - y1);
    }

    // bottom screen clipping
    if (y3 > m_fb.height())
    {
        y3 = m_fb.height();
        iy3 = y3 - 1;
    }
    else
    {
        iy3 = y3 - 1;
    }

    // if no x clipping (left right screen borders)
    if (x1 >= m_fb.xorig() && x1 <= m_fb.width() &&
        x2 >= m_fb.xorig() && x2 <= m_fb.width() &&
        x3 >= m_fb.xorig() && x3 <= m_fb.width())
    {
        for (int y = y1; y <= iy3; y++)
        {
            m_fb.wscanline(xs, xe, y, color);

            xs += dxLeft;
            xe += dxRight;
        }
    }
    else    // have x clipping
    {
        for (int y = y1; y <= iy3; y++)
        {
            double left = xs, right = xe;

            xs += dxLeft;
            xe += dxRight;

            if (left < m_fb.xorig())
            {
                left = m_fb.xorig();

                if (right < m_fb.xorig())
                    continue;
            }

            if (right > m_fb.width())
            {
                right = m_fb.width();

                if (left > m_fb.width())
                    continue;
            }

            m_fb.wscanline(left, right, y, color);
        }
    }
}

void Rasterizer::drawTopTriangle(int x1, int y1,
                                 int x2, int /*y2*/,
                                 int x3, int y3,
                                 const Color3 &color)
{
    if (x2 < x1)
    {
        std::swap(x1, x2);
    }

    double height = y3 - y1;

    double dx_left  = (x3 - x1) / height;
    double dx_right = (x3 - x2) / height;
    double xs = x1;
    double xe = x2;

    int iy1 = 0, iy3 = 0;

    if (y1 < m_fb.yorig())
    {
        xs = xs + dx_left * (-y1 + m_fb.yorig());
        xe = xe + dx_right * (-y1 + m_fb.yorig());

        y1 = m_fb.yorig();

        iy1 = y1;
    }
    else
    {
        iy1 = y1;

        xs = xs + dx_left * (iy1 - y1);
        xe = xe + dx_right * (iy1 - y1);
    }

    if (y3 > m_fb.height())
    {
        y3 = m_fb.height();

        iy3 = y3 - 1;
    }
    else
    {
        iy3 = y3 - 1;
    }

    if (x1 >= m_fb.xorig() && x1 <= m_fb.width() &&
        x2 >= m_fb.xorig() && x2 <= m_fb.width() &&
        x3 >= m_fb.xorig() && x3 <= m_fb.width())
    {
        for (int y = iy1; y <= iy3; y++)
        {

            m_fb.wscanline(xs, xe, y, color);

            xs += dx_left;
            xe += dx_right;
        }

    }
    else
    {
        for (int y = iy1; y <= iy3; y++)
        {
            double left = xs;
            double right = xe;

            xs += dx_left;
            xe += dx_right;

            if (left < m_fb.xorig())
            {
                left = m_fb.xorig();

                if (right < m_fb.xorig())
                    continue;
            }

            if (right > m_fb.width())
            {
                right = m_fb.width();

                if (left > m_fb.width())
                   continue;
            }

            m_fb.wscanline(left, right, y, color);
        }
    }
}

void Rasterizer::drawBottomTriangleGouraud(math::vertex &v1, math::vertex &v2, math::vertex &v3)
{
    // make ccw order
    if (v3.p.x < v2.p.x )
        std::swap(v2, v3);

    double height = v3.p.y - v1.p.y;
    double dx_left  = (v2.p.x - v1.p.x) / height;
    double dx_right = (v3.p.x - v1.p.x) / height;
    double dr_left  = ((int)v2.color[RED] - (int)v1.color[RED]) / height;
    double dr_right = ((int)v3.color[RED] - (int)v1.color[RED]) / height;
    double dg_left  = ((int)v2.color[GREEN] - (int)v1.color[GREEN]) / height;
    double dg_right = ((int)v3.color[GREEN] - (int)v1.color[GREEN]) / height;
    double db_left  = ((int)v2.color[BLUE] - (int)v1.color[BLUE]) / height;
    double db_right = ((int)v3.color[BLUE] - (int)v1.color[BLUE]) / height;

    double xs = v1.p.x;
    double xe = v1.p.x;
    double rs = v1.color[RED];
    double re = v1.color[RED];
    double gs = v1.color[GREEN];
    double ge = v1.color[GREEN];
    double bs = v1.color[BLUE];
    double be = v1.color[BLUE];


    int iy1 = 0, iy3 = 0;

    // top of screen clipping
    if (v1.p.y < m_fb.yorig())
    {
        xs = xs + dx_left * (-v1.p.y  + m_fb.yorig());
        xe = xe + dx_right * (-v1.p.y  + m_fb.yorig());

        v1.p.y  = m_fb.yorig();
        iy1 = m_fb.yorig();     // ceiling
    }
    else
        iy1 = v1.p.y ;

    // bottom screen clipping
    if (v3.p.y  > m_fb.height())
    {
        v3.p.y = m_fb.height();
        iy3 = v3.p.y - 1;
    }
    else
    {
        iy3 = v3.p.y - 1;
    }

    // if no x clipping (left right screen borders)
    if (v1.p.x >= m_fb.xorig() && v1.p.x <= m_fb.width() &&
        v2.p.x >= m_fb.xorig() && v2.p.x <= m_fb.width() &&
        v3.p.x >= m_fb.xorig() && v3.p.x <= m_fb.width())
    {
        for (int y = iy1; y <= iy3; y++)
        {
            int ri = rs, gi = gs, bi = bs;

            double dx = xe - xs;
            double dr = (re - rs) / dx;
            double dg = (ge - gs) / dx;
            double db = (be - bs) / dx;

            for (int x = xs; x < xe; x++)
            {
                ri += dr;
                gi += dg;
                bi += db;

//                syslog << "r:" << ri << "g:" << gi << "b:" << bi << logdebug;

                m_fb.wpixel(x, y, Color3(ri, gi, bi));
            }

//            m_fb.wscanline(xs, xe, y, Color3(rs, gs, bs));

            xs += dx_left;
            xe += dx_right;
            rs += dr_left;
            re += dr_right;
            gs += dg_left;
            ge += dg_right;
            bs += db_left;
            be += db_right;
        }
    }
    else    // have x clipping
    {
        for (int y = iy1; y <= iy3; y++)
        {
            double left = xs, right = xe;

            xs += dx_left;
            xe += dx_right;

            if (left < m_fb.xorig())
            {
                left = m_fb.xorig();

                if (right < m_fb.xorig())
                    continue;
            }

            if (right > m_fb.width())
            {
                right = m_fb.width();

                if (left > m_fb.width())
                    continue;
            }

            m_fb.wscanline(left, right, y, v1.color);
        }
    }
}

void Rasterizer::drawTopTriangleGouraud(math::vertex &v1, math::vertex &v2, math::vertex &v3)
{
    if (v2.p.x < v1.p.x)
    {
        std::swap(v1, v2);
    }

    double height = v3.p.y - v1.p.y;

    double dx_left  = (v3.p.x - v1.p.x) / height;
    double dx_right = (v3.p.x - v2.p.x) / height;
    double dr_left  = ((int)v3.color[RED] - (int)v1.color[RED]) / height;
    double dr_right = ((int)v3.color[RED] - (int)v2.color[RED]) / height;
    double dg_left  = ((int)v3.color[GREEN] - (int)v1.color[GREEN]) / height;
    double dg_right = ((int)v3.color[GREEN] - (int)v2.color[GREEN]) / height;
    double db_left  = ((int)v3.color[BLUE] - (int)v1.color[BLUE]) / height;
    double db_right = ((int)v3.color[BLUE] - (int)v2.color[BLUE]) / height;

    double xs = v1.p.x;
    double xe = v2.p.x;
    double rs = v1.color[RED];
    double re = v2.color[RED];
    double gs = v1.color[GREEN];
    double ge = v2.color[GREEN];
    double bs = v1.color[BLUE];
    double be = v2.color[BLUE];

    int iy1 = 0, iy3 = 0;

    if (v1.p.y < m_fb.yorig())
    {
        double dy = (m_fb.yorig() - v1.p.y);

        xs = xs + dx_left * dy;
        xe = xe + dx_right *dy;

        rs = rs + dr_left * dy;
        re = re + dr_right * dy;
        gs = gs + dg_left * dy;
        ge = ge + dg_right * dy;
        bs = bs + db_left * dy;
        be = be + db_right * dy;

        v1.p.y = m_fb.yorig();

        iy1 = v1.p.y;
    }
    else
    {
        iy1 = v1.p.y;
    }

    if (v3.p.y > m_fb.height())
    {
        v3.p.y  = m_fb.height();

        iy3 = v3.p.y - 1;
    }
    else
    {
        iy3 = v3.p.y - 1;
    }

    if (v1.p.x >= m_fb.xorig() && v1.p.x <= m_fb.width() &&
        v2.p.x >= m_fb.xorig() && v2.p.x <= m_fb.width() &&
        v3.p.x >= m_fb.xorig() && v3.p.x <= m_fb.width())
    {
        for (int y = iy1; y <= iy3; y++)
        {
            int ri = rs, gi = gs, bi = bs;

            double dx = xe - xs;
            double dr = (re - rs) / dx;
            double dg = (ge - gs) / dx;
            double db = (be - bs) / dx;

            for (int x = xs; x < xe; x++)
            {
                ri += dr;
                gi += dg;
                bi += db;

//                syslog << "r:" << ri << "g:" << gi << "b:" << bi << logdebug;

                m_fb.wpixel(x, y, Color3(ri, gi, bi));
            }

//            m_fb.wscanline(xs, xe, y, Color3(rs, gs, bs));

            xs += dx_left;
            xe += dx_right;
            rs += dr_left;
            re += dr_right;
            gs += dg_left;
            ge += dg_right;
            bs += db_left;
            be += db_right;
        }

    }
    else
    {
        for (int y = iy1; y <= iy3; y++)
        {
            double left = xs;
            double right = xe;
            double lr = rs;
            double rr = re;
            double lg = gs;
            double rg = ge;
            double lb = bs;
            double rb = be;

            xs += dx_left;
            xe += dx_right;
            lr += dr_left;
            rr += dr_right;
            lg += dg_left;
            rg += dg_right;
            lb += db_left;
            rb += db_right;

            if (left < m_fb.xorig())
            {
                left = m_fb.xorig();

                if (right < m_fb.xorig())
                    continue;
            }

            if (right > m_fb.width())
            {
                right = m_fb.width();

                if (left > m_fb.width())
                   continue;
            }

            m_fb.wscanline(left, right, y, v1.color);
        }
    }
}

bool Rasterizer::clipLine(math::vec3 &p1, math::vec3 &p2)
{
    // internal clipping codes
    const int CLIP_CODE_C = 0x0000;
    const int CLIP_CODE_N = 0x0008;
    const int CLIP_CODE_S = 0x0004;
    const int CLIP_CODE_E = 0x0002;
    const int CLIP_CODE_W = 0x0001;

    const int CLIP_CODE_NE = 0x000a;
    const int CLIP_CODE_SE = 0x0006;
    const int CLIP_CODE_NW = 0x0009;
    const int CLIP_CODE_SW = 0x0005;

    int xc1 = static_cast<int>(p1.x),
        yc1 = static_cast<int>(p1.y),
        xc2 = static_cast<int>(p2.x),
        yc2 = static_cast<int>(p2.y);

    int p1_code = 0, p2_code = 0;

    // determine codes for p1 and p2
    if (p1.y < m_fb.yorig())
        p1_code |= CLIP_CODE_N;
    else
        if (p1.y > m_fb.height())
            p1_code |= CLIP_CODE_S;

    if (p1.x < m_fb.xorig())
        p1_code |= CLIP_CODE_W;
    else
        if (p1.x > m_fb.width())
            p1_code |= CLIP_CODE_E;

    if (p2.y < m_fb.yorig())
        p2_code |= CLIP_CODE_N;
    else
        if (p2.y > m_fb.height())
            p2_code |= CLIP_CODE_S;

    if (p2.x < m_fb.xorig())
        p2_code |= CLIP_CODE_W;
    else
        if (p2.x > m_fb.width())
            p2_code |= CLIP_CODE_E;

    // try and trivially reject
    if ((p1_code & p2_code))
        return false;

    // test for totally visible, if so leave points untouched
    if (p1_code == 0 && p2_code == 0)
        return true;

    // determine end clip point for p1
    switch(p1_code)
    {
    case CLIP_CODE_C: break;

    case CLIP_CODE_N:
        yc1 = m_fb.yorig();
        xc1 = p1.x + 0.5 + (m_fb.yorig() - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);

        break;
    case CLIP_CODE_S:
        yc1 = m_fb.height();
        xc1 = p1.x + 0.5 + (m_fb.height() - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);

        break;
    case CLIP_CODE_W:
        xc1 = m_fb.xorig();
        yc1 = p1.y + 0.5 + (m_fb.xorig() - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);

        break;
    case CLIP_CODE_E:
        xc1 = m_fb.width();
        yc1 = p1.y + 0.5 + (m_fb.width() - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);

        break;
    // these cases are more complex, must compute 2 intersections
    case CLIP_CODE_NE:
        // north hline intersection
        yc1 = m_fb.yorig();
        xc1 = p1.x + 0.5 + (m_fb.yorig() - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc1 < m_fb.xorig() || xc1 > m_fb.width())
        {
            // east vline intersection
            xc1 = m_fb.width();
            yc1 = p1.y + 0.5 + (m_fb.width() - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
        }

        break;
    case CLIP_CODE_SE:
        // south hline intersection
        yc1 = m_fb.height();
        xc1 = p1.x + 0.5 + (m_fb.height() - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc1 < m_fb.xorig() || xc1 > m_fb.width())
        {
            // east vline intersection
            xc1 = m_fb.width();
            yc1 = p1.y + 0.5 + (m_fb.width() - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
        } // end if

        break;
    case CLIP_CODE_NW:
        // north hline intersection
        yc1 = m_fb.yorig();
        xc1 = p1.x + 0.5 + (m_fb.yorig() - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc1 < m_fb.xorig() || xc1 > m_fb.width())
        {
            xc1 = m_fb.xorig();
            yc1 = p1.y + 0.5 + (m_fb.xorig() - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
        } // end if

        break;
    case CLIP_CODE_SW:
        // south hline intersection
        yc1 = m_fb.height();
        xc1 = p1.x + 0.5 + (m_fb.height() - p1.y) * (p2.x - p1.x) / (p2.y - p1.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc1 < m_fb.xorig() || xc1 > m_fb.width())
        {
            xc1 = m_fb.xorig();
            yc1 = p1.y + 0.5 + (m_fb.xorig() - p1.x) * (p2.y - p1.y) / (p2.x - p1.x);
        }

        break;

    default:
        break;
    }

    // determine clip point for p2
    switch(p2_code)
    {
    case CLIP_CODE_C:

        break;
    case CLIP_CODE_N:
        yc2 = m_fb.yorig();
        xc2 = p2.x + (m_fb.yorig() - p2.y) * (p1.x - p2.x) / (p1.y - p2.y);

        break;
    case CLIP_CODE_S:
        yc2 = m_fb.height();
        xc2 = p2.x + (m_fb.height() - p2.y) * (p1.x - p2.x) / (p1.y - p2.y);

        break;
    case CLIP_CODE_W:
        xc2 = m_fb.xorig();
        yc2 = p2.y + (m_fb.xorig() - p2.x) * (p1.y - p2.y) / (p1.x - p2.x);

        break;
    case CLIP_CODE_E:
        xc2 = m_fb.width();
        yc2 = p2.y + (m_fb.width() - p2.x) * (p1.y - p2.y) / (p1.x - p2.x);

        break;
        // these cases are more complex, must compute 2 intersections
    case CLIP_CODE_NE:
        // north hline intersection
        yc2 = m_fb.yorig();
        xc2 = p2.x + 0.5 + (m_fb.yorig() - p2.y) * (p1.x - p2.x) / (p1.y - p2.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc2 < m_fb.xorig() || xc2 > m_fb.width())
        {
            // east vline intersection
            xc2 = m_fb.width();
            yc2 = p2.y + 0.5 + (m_fb.width() - p2.x) * (p1.y - p2.y) / (p1.x - p2.x);
        }

        break;
    case CLIP_CODE_SE:
        // south hline intersection
        yc2 = m_fb.height();
        xc2 = p2.x + 0.5 + (m_fb.height() - p2.y) * (p1.x - p2.x) / (p1.y - p2.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc2 < m_fb.xorig() || xc2 > m_fb.width())
        {
            // east vline intersection
            xc2 = m_fb.width();
            yc2 = p2.y + 0.5 + (m_fb.width() - p2.x) * (p1.y - p2.y) / (p1.x - p2.x);
        }

        break;
    case CLIP_CODE_NW:
        // north hline intersection
        yc2 = m_fb.yorig();
        xc2 = p2.x + 0.5 + (m_fb.yorig() - p2.y) * (p1.x - p2.x) / (p1.y - p2.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc2 < m_fb.xorig() || xc2 > m_fb.width())
        {
            xc2 = m_fb.xorig();
            yc2 = p2.y + 0.5 + (m_fb.xorig() - p2.x) * (p1.y - p2.y) / (p1.x - p2.x);
        }

        break;
    case CLIP_CODE_SW:
        // south hline intersection
        yc2 = m_fb.height();
        xc2 = p2.x + 0.5 + (m_fb.height() - p2.y) * (p1.x - p2.x) / (p1.y - p2.y);

        // test if intersection is valid, of so then done, else compute next
        if (xc2 < m_fb.xorig() || xc2 > m_fb.width())
        {
            xc2 = m_fb.xorig();
            yc2 = p2.y + 0.5 + (m_fb.xorig() - p2.x) * (p1.y - p2.y) / (p1.x - p2.x);
        }

        break;
    default:
        break;
    }

    // do bounds check
    if ((xc1 < m_fb.xorig()) || (xc1 > m_fb.width()) ||
        (yc1 < m_fb.yorig()) || (yc1 > m_fb.height())||
        (xc2 < m_fb.xorig()) || (xc2 > m_fb.width()) ||
        (yc2 < m_fb.yorig()) || (yc2 > m_fb.height()))
    {
        return false;
    }

    // store vars back
    p1.x = xc1;
    p1.y = yc1;
    p2.x = xc2;
    p2.y = yc2;

    return true;
}

void Rasterizer::drawLine(const math::vertex &p1, const math::vertex &p2)
{
    math::vec3 pc1(p1.p), pc2(p2.p);
    const Color3 &color = p1.color;

    int cols = m_fb.width();

    if (clipLine(pc1, pc2))
    {
        int error;
        int x0 = pc1.x, x1 = pc2.x;
        int y0 = pc1.y, y1 = pc2.y;
        int pixNum;

        pixNum = y0 * cols + x0;

        int dx = x1 - x0;
        int dy = y1 - y0;

        int xInc, yInc;
        if (dx >= 0)
            xInc = 1;
        else
        {
            xInc = -1;
            dx = -dx;
        }

        if (dy >= 0)
            yInc = cols;
        else
        {
            yInc = -cols;
            dy = -dy;
        }

        int dx2 = dx << 1;
        int dy2 = dy << 1;

        if (dx > dy)
        {
            error = dy2 - dx;

            for (int index = 0; index <= dx; index++)
            {
                m_fb.wpixel(pixNum, color);

                if (error >= 0)
                {
                    error -= dx2;
                    pixNum += yInc;
                }

                error += dy2;

                pixNum += xInc;
            }
        }
        else
        {
            error = dx2 - dy;

            for (int index = 0; index <= dy; index++)
            {
                m_fb.wpixel(pixNum, color);

                if (error >= 0)
                {
                    error -= dy2;
                    pixNum += xInc;
                }
                error += dx2;
                pixNum += yInc;
            }
        }
    }
}

void Rasterizer::drawFillTriangle(const math::vertex &p1,
                                  const math::vertex &p2,
                                  const math::vertex &p3)
{
    int x1 = p1.p.x, x2 = p2.p.x, x3 = p3.p.x;
    int y1 = p1.p.y, y2 = p2.p.y, y3 = p3.p.y;
    const Color3 &color = p1.color;

    // check degenerate triangle
    if ((x1 == x2 && x2 == x3) || (y1 == y2 && y2 == y3))
        return;

    // sort vertices (CCW order: p1 (top), p2, p3 (bottom))
    if (y2 < y1)
    {
        std::swap(x1, x2);
        std::swap(y1, y2);
    }

    if (y3 < y1)
    {
        std::swap(x1, x3);
        std::swap(y1, y3);
    }

    if (y3 < y2)
    {
        std::swap(x2, x3);
        std::swap(y2, y3);
    }

    // if triangle isn't on a screen
    if (y3 < m_fb.yorig() || y1 > m_fb.height()
            || (x1 < m_fb.xorig() && x2 < m_fb.xorig() && x3 < m_fb.xorig())
            || (x1 > m_fb.width() && x2 > m_fb.width() && x3 > m_fb.width()))
        return;

    if (y1 == y2)
    {
        drawTopTriangle(x1, y1, x2, y2, x3, y3, color);
    }
    else if (y2 == y3)
    {
        drawBottomTriangle(x1, y1, x2, y2, x3, y3, color);
    }
    else
    {
        double newX = x1 + (y2 - y1) * (x3 - x1) / (y3 - y1);   // ctg * dy

        drawBottomTriangle(x1, y1, newX, y2, x2, y2, color);
        drawTopTriangle(x2, y2, newX, y2, x3, y3, color);
    }
}

void Rasterizer::drawFillTriangle(const math::Triangle &tr)
{
    drawFillTriangle(tr.v(0), tr.v(1), tr.v(2));
}

void Rasterizer::drawTriangle(const math::vertex &p1, const math::vertex &p2, const math::vertex &p3)
{
    drawLine(p1, p2);
    drawLine(p2, p3);
    drawLine(p3, p1);
}

void Rasterizer::drawTriangle(const math::Triangle &tr)
{
    drawTriangle(tr.v(0), tr.v(1), tr.v(2));
}

void Rasterizer::drawGouraudTriangle(const math::vertex &v1, const math::vertex &v2, const math::vertex &v3)
{
}

void Rasterizer::drawGouraudTriangle(const math::Triangle &tr)
{
    drawGouraudTriangle(tr.v(0), tr.v(1), tr.v(2));
}

Rasterizer::Rasterizer(const int width, const int height)
    : m_fb(width, height)
{
}

void Rasterizer::rasterize(const RenderList &rendlist)
{
    auto &trias = rendlist.triangles();

    // painter's algorithm
    BOOST_REVERSE_FOREACH(const math::Triangle &t, trias)
    {
        if (!t.getMaterial())
        {
            syslog << "Material has not been setted for this triangle" << logdebug;
            continue;
        }

        switch(t.getMaterial()->shadeMode)
        {
        case Material::SM_WIRE:
            drawTriangle(t);
            break;

        case Material::SM_PLAIN_COLOR:
            drawFillTriangle(t);
            break;

        case Material::SM_FLAT:
            drawFillTriangle(t);
            break;

        case Material::SM_GOURAUD:
            drawGouraudTriangle(t);
            break;

        default:
            syslog << "Unsupported shading mode." << logdebug;
            break;
        }
    }
}

void Rasterizer::beginFrame(sptr(Viewport) /*viewport*/)
{
    m_fb.clear();
}

void Rasterizer::endFrame(sptr(Viewport) viewport)
{
    viewport->flush(m_fb);
}

void Rasterizer::resize(int w, int h)
{
    m_fb.resize(w, h);
}

}
