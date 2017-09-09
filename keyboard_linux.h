/* keyboard_linux.h */
/* Copyright 1998-1999 Per Lundberg */

#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

#include "defs.h"

#ifdef __cplusplus
#include <string>
#endif

#define INSERT 128
#define CAPSLOCK 64
#define NUMLOCK 32
#define SCROLLOCK 16
#define ALT 8
#define CONTROL 4
#define SHIFT 2
#define ALTGR 1 

#ifdef __cplusplus
class KEYBOARD
{
#ifdef __linux__
  byte where_am_i;   /* KEYBOARD::hit() puts characters in the buffer, so
		        we need to be able to see where we are */
  char buffer[100];  /* used when reading from kbFile */
  
  bool carry;
  uchar the_carry;
#endif
 public:
  uchar *shift_state;
  
  KEYBOARD(void);
  ~KEYBOARD(void);
  void init (void);
  void done (void);
  uchar get(void);
  bool hit(void);
  char *get_line(uchar x, uchar y, char *tmp_str, uchar max_len, 
		 bool print_chars);
};

extern KEYBOARD keyboard;

#endif
#endif
