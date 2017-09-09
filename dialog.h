/* dialog.h - routines for dialog boxes */
/* Copyright 1998-1999 Per Lundberg */

#ifndef __DIALOG_H__
#define __DIALOG_H__

#include "window.h"

#ifdef __cplusplus
#include <string>

class LISTBOX
{
  char *text[1024], *help_text[1024];
  char *data[1024];
  uchar data_item[1024], data_x[1024], data_y[1024], hbar[5], vbar[5];
  string title;
  unsigned int items, data_items, hbars, vbars;
  word x, y, x_size, y_size, x_max;
  bool init_run;
  bool done_run;
 public:
  Window *win;
  void insert (string text, string help_text = "", FPTR function = NULL,
	       uchar attributes = 0);
  void insert (string text, FPTR function = NULL, uchar attributes = 0);
  void insert (char *text, char *help_text = NULL, FPTR function = NULL, 
	       uchar attributes = 0);
  void insert (char *text, FPTR function = NULL, uchar attributes = 0);
  void insert_data (char *text, uchar x, uchar y, uchar item);
  void insert_data (char *text, uchar x, uchar y);
  void insert_hbar (uchar y);
  void insert_vbar (uchar x);
  void init (void);
  sword play (void);
  sword play (word);
  void done (void);
  LISTBOX (string title = "");
  LISTBOX (char *title);
  LISTBOX (uchar x, uchar y, string title = "");
  LISTBOX (uchar x, uchar y, char *title = NULL);
  LISTBOX (uchar x, uchar y, uchar x_size, uchar y_size, string title = "");
  LISTBOX (uchar x, uchar y, uchar x_size, uchar y_size, char *title = NULL);
  ~LISTBOX(void);
};

#endif
#endif
