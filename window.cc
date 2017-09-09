/* window.cc - windowing routines, totally platform independent */
/* Copyright 1996-1999 Per Lundberg */

#include "ui.h"
#include "window.h"
#include "screen_linux.h"

#include <string>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

void Window::put (byte x, byte y, char ch, byte color)
{
  screen.put (x1 + x, y1 + y, ch, color);
}

void Window::put (byte x, byte y, char ch)
{
  screen.put (x1 + x, y1 + y, ch, color);
}

void Window::put (byte x, byte y, string s, byte color)
{
  screen.put (x1 + x, y1 + y, s, color);
}

void Window::put (byte x, byte y, char *s, byte color)
{
  screen.put (x1 + x, y1 + y, s, color);
}

void Window::put (byte x, byte y, string s)
{
  screen.put (x1 + x, y1 + y, s, color);
}

void Window::put (byte x, byte y, char *s)
{
  screen.put (x1 + x, y1 + y, s, color);
}

void Window::put (byte x, byte y, dword c, byte color)
{
  string tmpstr;

  tmpstr = c;
  screen.put (x1 + x, y1 + y, tmpstr, color);
}

void Window::put (byte x, byte y, dword c)
{
  string tmpstr;

  tmpstr = c;
  screen.put (x1 + x, y1 + y, tmpstr, color);
}

void Window::put (char *s)
{
  screen.put (x1 + x, y1 + y, s, color);
  y++;
  //  if (y > y_size - 2)
  // scroll_window ();
}

void Window::put (string s)
{
  screen.put (x1 + x, y1 + y, s, color);
  y++;
  //  if (y > y_size - 2)
  // scroll_window ();
}

void Window::gotoxy (byte x, byte y)
{
  Window::x = x;
  Window::y = y;
}

void Window::center (byte y, string s, byte color)
{
  screen.put (x1 + x_size / 2 - s.length () / 2, y1 + y, s, color);
}

void Window::center (byte y, char *s, byte color)
{
  screen.put (x1 + x_size / 2 - strlen (s) / 2, y1 + y, s, color);
}

void Window::center (byte y, string s)
{
  screen.put (x1 + x_size / 2 - s.length () / 2, y1 + y, s, color);
}

void Window::center (byte y, char *s)
{
  screen.put (x1 + x_size / 2 - strlen (s) / 2, y1 + y, s, color);
}

Window::Window (byte x, byte y, byte x_size, byte y_size, 
		byte frame_color, byte title_color, string title)
{
  if (!x) x = screen.x_max / 2 - x_size / 2;
  if (!y) y = screen.y_max / 2 - y_size / 2;
  Window::x1 = x;
  Window::y1 = y;
  Window::x_size = x_size;
  Window::y_size = y_size;
  Window::x = 2;
  Window::y = 1;
  Window::title = title;
  Window::frame_color = frame_color;
  Window::title_color = title_color;
 
  open_window ();
}

Window::~Window (void)
{
  close_window ();
}
 
void Window::open_window (void)
{
  screen.save ();
  screen.window (x1, y1, x1 + x_size - 1, y1 + y_size - 1, frame_color,
		 title_color, title);
  color = ui.color_normal;
}
 
void Window::close_window(void)
{
  screen.restore ();
}

void Window::vline (byte x, byte _y1, byte _y2)
{
  screen.color = frame_color;
  screen.vline (x1 + x, y1 + _y1, y1 + _y2);
}

void Window::vline (byte x)
{
  vline (x1 + x, y1, y1 + y_size - 1);
}

void Window::hline (byte y, byte _x1, byte _x2)
{
  screen.color = frame_color;
  screen.hline (y1 + y, x1 + _x1, x1 + _x2);
}

void Window::hline (byte y)
{
  screen.color = frame_color;
  screen.hline (y1 + y, x1, x1 + x_size - 1);
}

char *Window::get_line (byte x, byte y, char *tmp_str, byte max_len, 
			bool print_chars)
{
  return ui.get_line (x1 + x, y1 + y, tmp_str, max_len, print_chars);
}
