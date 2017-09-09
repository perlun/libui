/* dialog.cc - general dialog routines */
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

#include "defs.h"
#include "dialog.h"
#include "screen_linux.h"
#include "keyboard_linux.h"

LISTBOX::LISTBOX (string title)
{
  items = data_items = hbars = vbars = 0;
  LISTBOX::title = title;
  LISTBOX::x = 0;
  LISTBOX::y = 0;
  LISTBOX::x_size = 0;
  LISTBOX::y_size = 0;
  init_run = FALSE;
  done_run = FALSE;
}

LISTBOX::LISTBOX (char *title)
{
  items = data_items = hbars = vbars = 0;
  LISTBOX::title = title;
  LISTBOX::x = 0;
  LISTBOX::y = 0;
  LISTBOX::x_size = 0;
  LISTBOX::y_size = 0;
  init_run = FALSE;
  done_run = FALSE;
}

LISTBOX::LISTBOX (uchar x, uchar y, string title)
{
  items = data_items = hbars = vbars = 0;
  LISTBOX::title = title;
  LISTBOX::x = x;
  LISTBOX::y = y;
  LISTBOX::x_size = 0;
  LISTBOX::y_size = 0;
  init_run = FALSE;
  done_run = FALSE;
}

LISTBOX::LISTBOX (uchar x, uchar y, char *title)
{
  items = data_items = hbars = vbars = 0;
  LISTBOX::title = title;
  LISTBOX::x = x;
  LISTBOX::y = y;
  LISTBOX::x_size = 0;
  LISTBOX::y_size = 0;
  init_run = FALSE;
  done_run = FALSE;
}

LISTBOX::LISTBOX (uchar x, uchar y, uchar x_size, uchar y_size, string title)
{
  items = data_items = hbars = vbars = 0;
  LISTBOX::title = title;
  LISTBOX::x = x;
  LISTBOX::y = y;
  LISTBOX::x_size = x_size;
  LISTBOX::y_size = y_size;
  init_run = FALSE;
  done_run = FALSE;
}

LISTBOX::LISTBOX (uchar x, uchar y, uchar x_size, uchar y_size, char *title)
{
  items = data_items = hbars = vbars = 0;
  LISTBOX::title = title;
  LISTBOX::x = x;
  LISTBOX::y = y;
  LISTBOX::x_size = x_size;
  LISTBOX::y_size = y_size;
  init_run = FALSE;
  done_run = FALSE;
}

LISTBOX::~LISTBOX (void)
{
  if (!done_run) done ();
}

void LISTBOX::insert (char *text, char *help_text, FPTR function,
		      uchar attributes)
{
  if (text)
  {
    LISTBOX::text[items] = new char[strlen (text) + 1];
    strcpy (LISTBOX::text[items], text);
  }
  else
    LISTBOX::text[items] = text;

  if (help_text)
  {
    LISTBOX::help_text[items] = new char[strlen (help_text) + 1];
    strcpy (LISTBOX::help_text[items], help_text);
  }
  else
    LISTBOX::help_text[items] = help_text;

  items++;
}

void LISTBOX::insert (string text, string help_text, FPTR function,
                      uchar attributes)
{
  insert (text, help_text, function, attributes);
}

void LISTBOX::insert (string text, FPTR function, uchar attributes)
{
  insert (text.data (), function, attributes);
}

void LISTBOX::insert (char *text, FPTR function, uchar attributes)
{
  insert (text, (char *) NULL, function, attributes);
}

void LISTBOX::insert_data (char *text, uchar x, uchar y, uchar item)
{
  data[data_items] = text;
  data_x[data_items] = x;
  data_y[data_items] = y;
  data_item[data_items] = item;
  data_items++;
}

void LISTBOX::insert_data (char *text, uchar x, uchar y)
{
  insert_data (text, x, y, 255);
}

void LISTBOX::insert_hbar (uchar y)
{
  hbar[hbars++] = y;
}

void LISTBOX::insert_vbar (uchar x)
{
  vbar[vbars++] = x;
}

void LISTBOX::init (void)
{
  unsigned int c = 0, d;
  
  for (d = 0; d < items; d++)
    if (text[d] != NULL && strlen (text[d]) > c) c = strlen (text[d]);

  x_max = c;
  if (title.length () + 4 > c) c = title.length () + 4;

  for (d = 0; d < data_items; d++)
    if (data[d] != NULL && data_x[d] + strlen (data[d]) > c)
      c = data_x[d] + strlen (data[d]);
 
  if (!x_size) x_size = c + 4; 
  if (!y_size) y_size = items + 2;
  if (y_size > screen.y_max) 
  {
    y_size = screen.y_max - 1;
    y = 1;
  }
  if (!x) x = screen.x_max / 2 - x_size / 2;
  if (!y) y = screen.y_max / 2 - y_size / 2;
  win = new Window (x, y, x_size, y_size, ui.color_window, ui.color_title, title);
  
  if (data_items)
    for (d = 0; d < data_items; d++)
      win->put (data_x[d], data_y[d], data[d], ui.color_normal);
  
  if (hbars)
    for (d = 0; d < hbars; d++)
      win->hline (hbar[d], 0, win->x_size - 1);

  if (vbars)
    for (d = 0; d < vbars; d++)
      win->vline (vbar[d], 0, win->y_size - 1);
  init_run = TRUE;
}

sword LISTBOX::play (void)
{
  return play (1);
}

sword LISTBOX::play (word start_item)
{
  sword done = 0;
  dword active = start_item - 1, c = 0, d, delta = 0;

  if (active > y_size - 3)
  {
    if (start_item + y_size / 2 - 1 > items) /* we're near the end */
    {
      delta = items - y_size - 3;
      active = start_item - delta;
    }
    else
    {
      active = y_size / 2 - 1;
      delta = start_item - active - 1;
    }
  }

  if (!init_run) init ();
  
  do
  {
    if (help_text[active] != NULL)
    {
      screen.put (1, screen.y_max, ' ', ui.color_status);
      screen.put (2, screen.y_max, help_text[active + delta], ui.color_status);
      if (help_text[active + delta] != NULL)
	for (d = strlen (help_text[active + delta]) + 2; d <= screen.x_max + 3;
	     d++)
	  screen.put (d, screen.y_max, ' ', ui.color_status);
    }

    for (d = 0; (d < items) && (d < y_size - 2); d++)
    {
      if (text[d + delta] != NULL && strlen (text[d + delta]))
      {
	if (d == active) 
	{
	  win->color = ui.color_chosen;
	  screen.gotoxy (x, y + d + 1);
	}
	else
	  win->color = ui.color_normal;
	win->put (1, 1 + d, ' ');
	win->put (2, 1 + d, text[d + delta]);
	for (c = strlen (text[d + delta]) + 2; c < x_max + 3; c++)
	  win->put (c, 1 + d, ' ');
      }  
    }

    screen.refresh ();

    c = keyboard.get ();
      
    switch (c)
    {
      case 0:
	c = keyboard.get ();
	switch (c)
	{
	  case 71: /* Home */
	    active = 0;
	    delta = 0;
	    break;
	  case 79: /* End */
	    if (items > y_size - 2)
	    {
	      delta = items - (y_size - 2);
	      active = y_size - 2;
	    }
	    else
	    {
	      active = items - 1;
	    }
	    break;
	  case 72: /* Up Arrow */
	    if(active) 
	      do 
	      {
		active--;
	      } while (text[active] == NULL);
	    else
	      if (delta)
		delta--;
	    break;
	  case 80: /* Down Arrow */
	    if (active + delta < items - 1) 
	      do 
	      {
		active++;
	      } while (text[active] == NULL);
	    if (active > y_size - 3)
	    {
	      active--;
	      delta++;
	    }
	    break;
	}
	break;
      case 13: /* Enter */
	done = active + delta + 1;
	break;
      case 27: /* Escape */
	done = ABORT;
	break; 
      default:
	break;
    }
  } while (!done);
  
  return done;
}

void LISTBOX::done (void)
{
  word d;

  for (d = 0; d < items; d++)
  {
    if (text[d]) delete text[d];
    if (help_text[d]) delete help_text[d];
  }
  delete win;
  done_run = TRUE;
}
