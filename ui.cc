/* ui.cc - user interface class */
/* Copyright 1998-1999 Per Lundberg */

/* This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA */

#include <string>

#include "defs.h"
#include "ui.h"
#include "window.h"
#include "screen_linux.h"
#include "keyboard_linux.h"

#include <memory.h>
#include <stdio.h>
#include <unistd.h>

UI ui;

/* Set colors used by the other methods in the class. This *must* be run
   before any of those are to be used; otherwise the text output from these
   methods will not be visible */

void UI::set_colors (byte color_window, byte color_title, byte color_normal,
                     byte color_chosen, byte color_status)
{
  UI::color_window = color_window;
  UI::color_title = color_title;
  UI::color_normal = color_normal;
  UI::color_chosen = color_chosen;
  UI::color_status = color_status;
}

/* Put up a confirmation box */

bool UI::yesno (char *text, uchar def, uchar *title)
{
  return yesno (string (text), def, string ((char *) title));
}

bool UI::yesno (string text, uchar def, string title)
{
  Window *win;
  uchar active = def;
  bool done = FALSE;
  uchar c;
  
  win = new Window (screen.x_max / 2 - (text.length () + 4) / 2,
                    screen.y_max / 2 - 2, text.length () + 4, 4, 
                    color_normal, color_title, title);
  win->center (1, text, color_normal);

  do
  {
    win->put ((text.length () + 4) / 2 - 5, 2, " Yes ", active ? color_chosen : color_normal);
    win->put ((text.length () + 4) / 2, 2, " No ", !active ? color_chosen : color_normal);
    screen.refresh ();
    c = keyboard.get();

    switch (toupper (c))
    {
      case 0:
        c = keyboard.get();
        switch(c)
        {
          case 75: /* Left Arrow */
          case 77: /* Right Arrow */
            active = 1 - active;
            break;
        }
        break;
      case 13: /* Return, exit */
        done = TRUE;
        break;
      case 27: /* Escape, abort */
        active = 0;
        done = TRUE;
        break;
      case 'Y':
        done = TRUE;
        active = 1;
        break;
      case 'N':
        done = TRUE;
        active = 0;
        break;
      default: /* do nothing */
        break;
    }
  } while(!done);
  delete win;
  win = NULL;
  return active;
}

/* Read a line from the keyboard at the specified position */

char *UI::get_line (uchar x, uchar y, char *tmp_str, uchar max_len, 
		    bool print_chars)
{
  char *old_str = new char[256];
  char *tmp_str2;
  uchar c, l = strlen (tmp_str), m = l, help = 0;
  sword c2, c3;
  bool insert = TRUE;
  
  strcpy (old_str, tmp_str);
  screen.gotoxy (x + l, y);
  
  screen.show_cursor ();  

  do
  {
    c = keyboard.get ();
    switch (c)
    {
      case 0:
        c = keyboard.get ();
        switch (c)
        {
          case 59: /* F1 */
            help = 1;
            break;

          case 71: /* Home */
            m = 0;
            screen.gotoxy (x, y);
            break;

          case 75: /* Left arrow */
            if (m) m--;
            screen.gotoxy (x + m, y);
            break;

          case 77: /* Right arrow */
            if (m < l) m++;
            screen.gotoxy (x + m, y);
            break;

          case 79: /* End */
            m = l;
            screen.gotoxy (x + m, y);
            break;

          case 82: /* Insert */
            insert = insert ^ 1;
            if (insert) 
              screen.line_cursor ();
            else 
              screen.block_cursor ();
            break;

          case 83: /* Delete */
            if (m < l)
            {
              memcpy (tmp_str + m, tmp_str + m + 1, l - m);
              screen.put (x, y, tmp_str, color_normal);
              screen.put (x + l - 1, y, 0x20, color_normal);
              screen.refresh ();
              l--;
            }
            break;
        }
        break;

      case 1: /* Ctrl-A */
        m = 0;
        screen.gotoxy (x, y);
        break;

      case 4: /* Ctrl-D */
        if (m < l)
        {
          memcpy (tmp_str + m, tmp_str + m + 1, l - m);
          screen.put (x, y, tmp_str, color_normal);
          screen.put (x + l - 1, y, 0x20, color_normal);
          screen.refresh ();
          l--;
        }
        break;

      case 5: /* Ctrl-E */
        m = l;
        screen.gotoxy (x + m, y); 
        break;

      case 8: /* Backspace; Ctrl-H */
        if (m)
        {
          if (m != l) memcpy (tmp_str + m - 1, tmp_str + m, l - m);
          tmp_str[l - 1] = 0;
          l--;
          m--;
          if (print_chars)
          {
            screen.put (x, y, tmp_str, color_normal);
            screen.put (x + l, y, 0x20, color_normal);
            screen.gotoxy (x + m, y);
            screen.refresh ();
          }
        }
        break;

      case 11: /* Ctrl-K */
        memset (tmp_str + m, 0x20, l - m);
        screen.put (x, y, tmp_str, color_normal);
        screen.refresh ();
        memset (tmp_str + m, l - m, 0);
        l = m;
        break;

      case 13: /* Enter */
        break;

      case 27: /* Escape */
        c2 = strlen (tmp_str) - strlen (old_str);
        memset (tmp_str, 0, max_len + 1);
        strcpy (tmp_str, old_str);
        screen.put (x, y, tmp_str, color_normal);
        if (c2 > 0)
          for (c3 = 0; c3 < c2; c3++)
            screen.put (x + strlen (tmp_str) + c3, y, 0x20, color_normal);
        screen.refresh ();
        break;

      default:
        if (l < max_len)
        {
          if (m < l) 
          {
            if (insert)
            {
              tmp_str2 = new char[256];
              strcpy (tmp_str2, tmp_str + m);
              strcpy (tmp_str + m + 1, tmp_str2);
              delete tmp_str2;
              tmp_str2 = NULL;
            }
          }
          tmp_str[m] = c;
          if (insert)
            l++;
          m++;

          if (print_chars)
          {
            screen.put (x, y, tmp_str, color_normal);
            screen.gotoxy (x + m, y);
            screen.refresh ();
          }
        }
    }
  } while (!((c == 13) || (c == 27) || (help == 1)));
  screen.hide_cursor ();
  delete old_str;
  old_str = NULL;
  return tmp_str;
}

void UI::infobox (string title, string text)
{
  Window *win; 
  win = new Window (screen.x_max / 2 - (text.length () + 8) / 2, 
                    screen.y_max / 2 - 2, text.length () + 8, 4, color_window, color_title, title);
  win->center (1, text, color_normal);
  win->center (2, " OK ", color_chosen);
  screen.refresh ();
  while (keyboard.get () != 13);
  delete win;  
  win = NULL;
}

void UI::infobox (char *title, char *text)
{
  infobox (string (title), string (text));
}

void UI::infobox (string text)
{
  infobox (string (""), text);
}

void UI::infobox (char *text)
{
  infobox (string (text));
}
