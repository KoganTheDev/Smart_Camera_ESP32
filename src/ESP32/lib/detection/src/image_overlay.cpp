#include "image_overlay.h"

void ImageOverlay::set_pixel(uint16_t* buffer, int width, int height, int x, int y, uint16_t color)
{
    // Bounds check
    if (x < 0 || x >= width || y < 0 || y >= height)
    {
        return;
    }

    buffer[y * width + x] = color;
}

void ImageOverlay::draw_crosshair(uint16_t* buffer, int width, int height, int center_x, int center_y, int size,
                                  uint16_t color)
{
    // Horizontal line
    for (int x = center_x - size; x <= center_x + size; x++)
    {
        set_pixel(buffer, width, height, x, center_y, color);
    }

    // Vertical line
    for (int y = center_y - size; y <= center_y + size; y++)
    {
        set_pixel(buffer, width, height, center_x, y, color);
    }
}

void ImageOverlay::draw_circle(uint16_t* buffer, int width, int height, int center_x, int center_y, int radius,
                               uint16_t color)
{
    // Midpoint circle algorithm
    int x = 0;
    int y = radius;
    int d = 3 - 2 * radius; // Decision parameter

    while (x <= y)
    {
        // Draw 8 octants
        set_pixel(buffer, width, height, center_x + x, center_y + y, color);
        set_pixel(buffer, width, height, center_x - x, center_y + y, color);
        set_pixel(buffer, width, height, center_x + x, center_y - y, color);
        set_pixel(buffer, width, height, center_x - x, center_y - y, color);
        set_pixel(buffer, width, height, center_x + y, center_y + x, color);
        set_pixel(buffer, width, height, center_x - y, center_y + x, color);
        set_pixel(buffer, width, height, center_x + y, center_y - x, color);
        set_pixel(buffer, width, height, center_x - y, center_y - x, color);

        if (d < 0)
        {
            d = d + 4 * x + 6;
        } else
        {
            d = d + 4 * (x - y) + 10;
            y--;
        }

        x++;
    }
}

void ImageOverlay::draw_line(uint16_t* buffer, int width, int height, int x1, int y1, int x2, int y2, uint16_t color)
{
    // Bresenham's line algorithm
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    int x = x1;
    int y = y1;

    while (true)
    {
        set_pixel(buffer, width, height, x, y, color);

        if (x == x2 && y == y2)
        {
            break;
        }

        int e2 = 2 * err;
        if (e2 > -dy)
        {
            err -= dy;
            x += sx;
        }
        if (e2 < dx)
        {
            err += dx;
            y += sy;
        }
    }
}
