/* window.h - prototypes for the Window class. This is -*- c++ -*- */
/* Copyright 1998-1999 Per Lundberg */

#ifndef __WINDOW_H__
#define __WINDOW_H__

#include "ui.h"

#ifdef __cplusplus

#include <string>

class Window
{
public:
  
  byte frame_color;
  byte title_color;
  byte color;
  byte x, y;			/* current cursor position */
  byte x1, y1;
  byte x_size, y_size;
  string title;

  Window (byte x, byte y, byte x_size, byte y_size, byte frame_color = 3,
	  byte title_color = 11, string title = "");
  ~Window (void);
  void open_window (void);
  void close_window (void);
  void put (byte x, byte y, char ch, byte color);
  void put (byte x, byte y, char ch);
  void put (byte x, byte y, string s, byte color);
  void put (byte x, byte y, char *s, byte color);
  void put (byte x, byte y, string s);
  void put (byte x, byte y, char *s);
  void put (byte x, byte y, dword c, byte color);
  void put (byte x, byte y, dword c);
  void put (char *s);
  void put (string s);
  void gotoxy (byte x, byte y);
  void center (byte y, string s, byte color);
  void center (byte y, char *s, byte color);
  void center (byte y, string s);
  void center (byte y, char *s);
  void hline (byte y, byte _x1, byte _x2);
  void hline (byte y);
  void vline (byte x, byte _y1, byte _y2);
  void vline (byte x);
  char *get_line (byte, byte, char *, byte, bool);
};
#endif /* __cplusplus */
#endif /* !__WINDOW_H__ */
