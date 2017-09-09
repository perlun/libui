/* screen_linux.h - this is -*- c++ -*- code */
/* Copyright 1998-1999 Per Lundberg */

#ifndef __SCREEN_H__
#define __SCREEN_H__

#include "defs.h"

#ifdef __cplusplus
#include <string>
#endif

#ifdef __cplusplus
class Screen
{
  byte x, y;
  word *image;
  dword *old_images[16]; /* Used by the ncurses routines only */
public:
  bool mono;
  byte color;
  byte x_max, y_max;
  word *old_screen[16];
  byte screens;
  word *vscreen;
#ifdef __linux__
  int screen_file;
#endif
  
  Screen (void);
  ~Screen (void);
  void init (void);
  void done (void);
  word get (byte x, byte y);
  void put (uchar ch, byte color);
  void put (byte x, byte y, uchar ch, byte color);
  void put (byte x, byte y, uchar ch);
  void put (byte x, byte y, string s, byte color);
  void put (byte x, byte y, string s);
  void put (byte x, byte y, char *s, byte color);
  void put (byte x, byte y, char *s);
  void put (byte x, byte y, uchar *s, byte color);
  void put (string s, byte color);
  void put (string s);
  void put (char *s);
  void center (byte y, string s, byte color);
  void center (byte y, char *s, byte color);
  void center (byte y, string s);
  void center (byte y, char *s);
  void gotoxy (byte x, byte y);
  void clear (void);
  void clear (word how);
  void hide_cursor (void);
  void show_cursor (void);
  void block_cursor (void);
  void line_cursor (void);
  byte wherex (void);
  byte wherey (void);
  void save (void);
  void restore (void);
  void window (byte x1, byte y1, byte x2, byte y2, byte frame_color,
	       byte title_color, string title);
  void window (byte x1, byte y1, byte x2, byte y2, byte frame_color);
  void hline (byte y, byte _x1, byte _x2);
  void hline (byte y);
  void vline (byte x, byte _y1, byte _y2);
  void vline (byte x);
  inline void place (byte x, byte y, uchar ch, byte color);
  void refresh (void);
};

extern Screen screen;

#endif
#endif
