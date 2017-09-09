/* ui.h - user interface class */
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

#ifndef __UI_H__
#define __UI_H__

#include "defs.h"

#ifdef __cplusplus

#include <string>

class UI
{
 public:
  byte color_window, color_title, color_chosen, color_status, color_normal;
  bool yesno (char *text, uchar def, uchar *title = (uchar *) "Confirm");
  bool yesno (string text, uchar def = 0, string title = "Confirm");
  void infobox (string title, string text);
  void infobox (char *title, char *text);
  void infobox (string text);
  void infobox (char *text);
  char *get_line (uchar x, uchar y, char *tmp_str, uchar max_len, 
		  bool print_chars);
  void set_colors (byte color_window, byte color_title, byte color_normal,
                   byte color_chosen, byte color_status);
};

extern UI ui;

#endif
#endif
