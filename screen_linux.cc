/* screen_linux.cc - screen output routines for Linux console */
/* Copyright 1997-1999 Per Lundberg */

#include "defs.h"
#include "screen_linux.h"
//#include "misc.h"

#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>

Screen screen;
byte line_char[12] = "Ä³Ú¿ÀÙÃ´ÂÁÅ";

Screen::Screen (void)
{
  init ();
}

Screen::~Screen (void)
{
  done ();
}

void Screen::init (void)
{
  char *tty;
  char tmp[40];
  byte i;

  color = 7;
  tty = ttyname (0);
  sprintf (tmp, "/dev/vcsa%d", atoi (tty + 8)); 
  screen_file = open (tmp, O_RDWR);

  if (screen_file == -1)
    {
      fprintf (stderr, "Could not open %s: ", tmp);
      perror ("");
      exit (1);
    }

  lseek (screen_file, 0, 0);
  read (screen_file, &y_max, 1);
  read (screen_file, &x_max, 1);
  mono = FALSE;

  hide_cursor ();
  screens = 0;
  vscreen = new word[y_max * x_max + x_max]; /* this should be y_max * x_max but some routine writes beyond the end of the screen.. */
  memset (vscreen, 0, y_max * y_max * 2);
}

void Screen::done (void)
{
  show_cursor ();
  close (screen_file);
  delete vscreen;
}

word Screen::get (byte x, byte y)
{
  word tmp;

  return vscreen[((y - 1) * x_max + x - 1)];
  return tmp;
}

void Screen::put (byte x, byte y, uchar ch, byte color)
{
  word tmp;
  
  vscreen[((y - 1) * x_max + x - 1)] = (color << 8) + ch;
}

void Screen::put (byte x, byte y, uchar ch)
{
  put (x, y, ch, color);
}

void Screen::put (uchar ch, byte color)
{
  put (x, y, ch, color);
  x++;
}

void Screen::put (byte x, byte y, string s, byte color)
{
  word *tmp = new word[s.length ()];
 
  for (word c = 0; c < s.length() && x + c < screen.x_max + 1; c++)
    vscreen[((y - 1) * x_max + x + c - 1)] = (color << 8) + s[c];

  Screen::y++;

  delete tmp;
}

void Screen::put (byte x, byte y, string s)
{
  put (x, y, s);
}

void Screen::put (byte x, byte y, char *s, byte color)
{
  if (s == NULL) return;
  for (word c = 0; c < strlen (s); c++)
    vscreen[((y - 1) * x_max + x - 1 + c)] = (color << 8) + (byte) s[c];

  Screen::y++;
}

void Screen::put (byte x, byte y, char *s)
{
  put (x, y, s, color);
}

void Screen::put (byte x, byte y, uchar *s, byte color)
{
  put (x, y, (char *) s, color);
}

void Screen::put (string s, byte color)
{
  put (x, y, s, color);
}

void Screen::put (string s)
{
  put (Screen::x, Screen::y, s, color);
}

void Screen::put (char *s)
{
  put (Screen::x, Screen::y, s, color);
}

void Screen::center (byte y, string s, byte color)
{
  put (x_max / 2 - s.length () / 2, y, s, color);
}

void Screen::center (byte y, char *s, byte color)
{
  put (x_max / 2 - strlen (s) / 2, y, s, color);
}

void Screen::center (byte y, string s)
{
  put (x_max / 2 - s.length () / 2, y, s, color);
}

void Screen::center (byte y, char *s)
{
  put (x_max / 2 - strlen (s) / 2, y, s, color);
}

void Screen::gotoxy (byte x, byte y)
{
    char tmp[10];

    Screen::x = x;
    Screen::y = y;
    sprintf (tmp, "\33[%d;%dH", y, x);
    write (STDOUT_FILENO, tmp, strlen(tmp));
}

byte Screen::wherex (void)
{
  return x;
}

byte Screen::wherey (void)
{
  return y;
}

void Screen::hide_cursor (void)
{
  write (STDOUT_FILENO, "\33[?25l", 6);
}

void Screen::show_cursor (void)
{
  write (STDOUT_FILENO, "\33[?25h", 6);
}

void Screen::block_cursor (void)
{
  write (STDOUT_FILENO, "\33[?8c", 5);  
}

void Screen::line_cursor (void)
{
  write (STDOUT_FILENO, "\33[?2c", 5);
}

void Screen::clear (void)
{
  clear (0x720);
}

void Screen::clear (word how)
{
  word c;

  for (c = 0; c < x_max * y_max; c++)
    vscreen[c] = how;
  screen.refresh ();

  gotoxy (1, 1);
}

void Screen::save (void)
{
  old_screen[screens] = new word[x_max * y_max];
  memcpy (old_screen[screens], vscreen, x_max * y_max * 2);
  screens++;
}

void Screen::restore (void)
{
  screens--;
  memcpy (vscreen, old_screen[screens], x_max * y_max * 2);
  delete (old_screen[screens]);
  refresh ();
}

void Screen::window (byte x1, byte y1, byte x2, byte y2, byte color)
{
  byte x, y;
  word tmpbuf[256];
  
  for (x = x1 + 1; x < x2; x++)
  {
    put (x, y1, (char) line_char[0], color);
    put (x, y2, (char) line_char[0], color);
  }

  for (x = 0; x < x2 - x1; x++)
    tmpbuf[x] = (color << 8) + ' ';
 
  for (y = y1 + 1; y < y2; y++)
  {
    put (x1, y, (char) line_char[1], color);
    put (x2, y, (char) line_char[1], color);

    memcpy (&vscreen[((y - 1) * x_max + x1)], &tmpbuf, (x2 - x1 - 1) << 1); 
  }
 
  put (x1, y1, (char) line_char[2], color);
  put (x2, y1, (char) line_char[3], color);
  put (x1, y2, (char) line_char[4], color);
  put (x2, y2, (char) line_char[5], color);
}

void Screen::window (byte x1, byte y1, byte x2, byte y2, byte frame_color,
		     byte title_color, string title)
{
  window (x1, y1, x2, y2, frame_color);

  if (title != "")
    {
      put (x1 + 2, y1, (char) line_char[7], frame_color);
      put (x1 + 3 + title.length () + 2, y1, (char) line_char[6], frame_color);
      put (x1 + 4, y1, title, title_color);
      put (x1 + 3, y1, ' ', title_color);
      put (x1 + 3 + title.length () + 1, y1, ' ', title_color);
    }
}

void Screen::vline (byte x, byte _y1, byte _y2)
{
  word y, c;

  for (y = _y1; y <= _y2; y++)
  {
    c = get (x, y);
    
    if (c % 256 == (byte) line_char[0])
    {
      if (y == _y1)
        put (x, y, (char) line_char[8], color);
      else if (y == _y2)
        put (x, y, (char) line_char[9], color);
      else
        put (x, y, (char) line_char[10], color);
    }
    else if (c % 256 == line_char[8] || c % 256 == line_char[9]);
    else
      put (x, y, (char) line_char[1], color);
  }
}

void Screen::vline (byte x)
{
  vline (x, 1, y_max - 1);
}

void Screen::hline (byte y, byte _x1, byte _x2)
{
  word x, c;

  for (x = _x1; x <= _x2; x++)
  {
    c = get (x, y);
    if (c % 256 == line_char[1])
    {
      if (x == _x1)
        put (x, y, (char) line_char[6], color);
      else if (x == _x2)
        put (x, y, (char) line_char[7], color);
      else
        put (x, y, (char) line_char[10], color);
    }
    else if (c % 256 == line_char[6] || c % 256 == line_char[7]) {}
    else put (x, y, (char) line_char[0], color);
  }
}

void Screen::hline (byte y)
{
  hline (y, 1, x_max);
}

inline void Screen::place (byte x, byte y, uchar ch, byte color) 
{
}

/* Copies the virtual screen buffer to the actual screen */

void Screen::refresh (void)
{
  lseek (screen_file, 4, SEEK_SET);
  write (screen_file, vscreen, x_max * y_max * 2);
}
