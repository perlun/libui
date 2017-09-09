/* keyboard_linux.cc - Linux console version of the KEYBOARD class */
/* Copyright 1998-1999 Per Lundberg */

#include "defs.h"
#include "keyboard_linux.h"
#include "screen_linux.h"

#include <stdio.h>
#include <memory.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <linux/kd.h>
#include <linux/vt.h>

KEYBOARD keyboard;

struct termios oldTermios;
int kb_file;
fd_set set;

/* The following tables are used to return the same scan codes in Linux and
   MSDOS */

const uchar key_map[] =
{
  0, 27, '1', '2', '3', '4', '5', '6',    /* 0-7 */
  '7', '8', '9', '0', '+', 180, 8, 9,     /* 8-15 */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', /* 16-23 */
  'o', 'p', 134, 168, 13, 0, 'a', 's',    /* 24-31 */
  'd', 'f', 'g', 'h', 'j', 'k', 'l', 148, /* 32-39 */
  132, 167, 0, '\'', 'z', 'x', 'c', 'v',  /* 40-47 */
  'b', 'n', 'm', ',', '.', '-', 0, '*',   /* 48-55 */
  0, 32, 0, 0, 0, 0, 0, 0,                /* 56-63 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 64-71 */
  0, 0, '-', 0, 0, 0, '+', 0,             /* 72-79 */
  0, 0, 0, 0, 0, 0, '<', 0,               /* 80-87 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 88-95 */
  0                                       /* 96 */
};

const uchar shift_key_map[] =
{
  0, 27, '!', '"', '#', '$', '%', '&',    /* 0-7 */
  '/', '(', ')', '=', '?', '`', 127, 9,   /* 8-15 */
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', /* 16-23 */
  'O', 'P', 143, '^', 13, 0, 'A', 'S',    /* 24-31 */
  'D', 'F', 'G', 'H', 'J', 'K', 'L', 153, /* 32-39 */
  142, 171, 0, '*', 'Z', 'X', 'C', 'V',   /* 40-47 */
  'B', 'N', 'M', ';', ':', '_', 0, '*',   /* 48-55 */
  0, 32, 0, 0, 0, 0, 0, 0,                /* 56-63 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 64-71 */
  0, 0, '-', 0, 0, 0, '+', 0,             /* 72-79 */ 
  0, 0, 0, 0, 0, 0, '>', 0,               /* 80-87 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 88-95 */
  0                                       /* 96 */
};

const uchar alt_key_map[] =
{
  0, 0, 0, '@', '£', '$', 0, 0,           /* 0-7 */
  '{', '[', ']', '}', '\\', 0, 0, 0,      /* 8-15 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 16-23 */ 
  0, 0, 0, '~', 13, 0, 0, 0,              /* 24-31 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 32-39 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 40-47 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 48-55 */ 
  0, 32, 0, 0, 0, 0, 0, 0,                /* 56-63 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 64-71 */
  0, 0, 0, 155, 0, 157, 0, 0,             /* 72-79 */
  0, 0, 0, 0, 0, 0, '|', 0,               /* 80-87 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 88-95 */
  0                                       /* 96 */
};

const uchar ctrl_key_map[] =
{
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 0-7 */
  0, 0, 0, '0', 0, 0, 127, 0,             /* 8-15 */
  16, 23, 5, 17, 19, 25, 20, 9,           /* 16-23 */
  14, 15, 0, 0, 10, 0, 1, 18,             /* 24-31 */
  4, 6, 7, 8, 10, 11, 12, 0,              /* 32-39 */
  0, 0, 0, 0, 26, 24, 3, 22,              /* 40-47 */
  2, 14, 13, 0, 0, 0, 0, 0,               /* 48-55 */
  0, 32, 0, 0, 0, 0, 0, 0,                /* 56-63 */
  0, 0, 0, 0, 0, 0, 0, 117,               /* 64-71 */
  0, 0, 0, 115, 0, 116, 0, 119,           /* 72-79 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 80-87 */
  0, 0, 0, 0, 0, 0, 0, 0,                 /* 88-95 */
  0                                       /* 96 */
};

extern "C"  void sighandler (int SIGNUM);

extern "C" 
void sighandler (int SIGNUM)
{
  if (SIGNUM == SIGSEGV || SIGNUM == SIGKILL || SIGNUM == SIGTERM ||
      SIGNUM == SIGFPE)
  {
    ioctl (kb_file, KDSKBMODE, K_XLATE);

    tcsetattr (kb_file, TCSAFLUSH, &oldTermios);
    close (kb_file);
    //    screen.clear ();
    screen.done ();

    switch (SIGNUM)
      {
      case SIGSEGV:
	printf ("\nSegmentation violation caught. Aborting.\n");
	break;
      case SIGKILL:
	printf ("\nKilled.\n");
	break;
      case SIGTERM:
	printf ("\nTerminated.\n");
	break;
      case SIGFPE:
	printf ("\nFloating point exception. Aborting.\n");
	break;
      }
    exit (1);
  }
}

KEYBOARD::KEYBOARD (void)
{
}

KEYBOARD::~KEYBOARD (void)
{
  done ();
}

/* Purpose: Initalize the keyboard handling */

void KEYBOARD::init (void)
{
  struct termios t;
  
  shift_state = new uchar[1];
  *shift_state = 0;

  where_am_i = 0;
  carry = 0;
  kb_file = open ("/dev/tty", O_RDWR);

  if (kb_file == -1)
  {
    perror ("Couldn't open /dev/tty");
    exit (1);
  }

  tcgetattr (kb_file, &oldTermios);
  tcgetattr (kb_file, &t);

  t.c_lflag &= ~(ICANON | ECHO | ISIG);
  t.c_iflag = 0;
  t.c_cc[VMIN] = 0;
  t.c_cc[VTIME] = 0;
  tcsetattr (kb_file, TCSAFLUSH, &t);

  /* Enable mediumraw mode, so that we can receive raw scan codes and convert
     them ourselves */

  ioctl (kb_file, KDSKBMODE, K_MEDIUMRAW);

  FD_ZERO (&set);
  FD_SET (kb_file, &set);

  /* Since we put the keyboard in RAW mode, making a new handler for SIGSEGV
     is a good idea. (Otherwise we may end up with an unusable keyboard) */

  signal (SIGSEGV, &sighandler);
  signal (SIGKILL, &sighandler);
  signal (SIGTERM, &sighandler);
  signal (SIGFPE, &sighandler);
}

/* Purpose: De-init the keyboard stuff */

void KEYBOARD::done (void)
{
  ioctl (kb_file, KDSKBMODE, K_XLATE);

  tcsetattr (kb_file, TCSAFLUSH, &oldTermios);
  close (kb_file);
  signal (SIGSEGV, SIG_DFL);
  signal (SIGKILL, SIG_DFL);
  signal (SIGTERM, SIG_DFL);
  signal (SIGFPE, SIG_DFL);

  delete shift_state;
}

/* Purpose: Get a character from the keyboard file, or the buffer */

uchar KEYBOARD::get (void)
{
  byte c = 0;
  
  if (carry)
  {
    carry = false;
    return the_carry;
  }
  
  if (where_am_i)
  {
    c = buffer[0];
    where_am_i--;
    if (where_am_i) memcpy (buffer, buffer + 1, where_am_i);
    return c;
  }
  else
    do
    {
      select (FD_SETSIZE, &set, NULL, NULL, NULL);
      read (kb_file, &c, 1);
      
      switch (c)
      {
	case 29: /* Left Control */
	case 97: /* Right Control */
	  *shift_state |= CONTROL;
	  break;
	case 128 + 29:
	case 128 + 97:
	  *shift_state &= ~CONTROL;
	  break;
	case 42: /* Left Shift */
	case 54: /* Right Shift */
	  *shift_state |= SHIFT;
	  break;
	case 42 + 128:
	case 54 + 128:
	  *shift_state &= ~SHIFT;
	  break;
	case 56:  /* Alt */
	  *shift_state |= ALT;
	  break;
	case 100: /* Alt Gr */
	  *shift_state |= ALTGR;
	  break;
	case 56 + 128:
	  *shift_state &= ~ALT;
	  break;
	case 100 + 128:
	  *shift_state &= ~ALTGR;
	  break;
	case 59: /* F1 */
	case 60: /* F2 */
	case 61: /* F3 */
	case 62: /* F4 */
	case 63: /* F5 */
	case 64: /* F6 */
	case 65: /* F7 */
	case 66: /* F8 */
	case 67: /* F9 */
	case 68: /* F10 */
	  if (*shift_state & ALT) 
	  {
	    *shift_state = *shift_state & (256 - ALT - CONTROL);
	    ioctl(kb_file, VT_ACTIVATE, c - 58);
	    ioctl(kb_file, VT_WAITACTIVE, c - 58);
	    *shift_state = 0;
	  }
	  else buffer[where_am_i++] = c;
	  break;
	case 102: /* Home */
	  carry = true;
	  the_carry = 71;
	  return 0;
	  break;
	case 103: /* Up Arrow */
	  carry = true;
	  the_carry = 72;
	  return 0;
	  break;
	case 104: /* Page Up */
	  carry = true;
	  the_carry = 73;
	  return 0;
	  break;
	case 105: /* Left Arrow */
	  carry = true;
	  the_carry = 75;
	  return 0;
	  break;
	case 106: /* Right Arrow */
	  carry = true;
	  the_carry = 77;
	  return 0;
	  break;
	case 107: /* End */
	  carry = true;
	  the_carry = 79;
	  return 0;
	  break;
	case 108: /* Down Arrow */
	  carry = true;
	  the_carry = 80;
	  return 0;
	  break;
	case 109: /* Page Down */
	  carry = true;
	  the_carry = 81;
	  return 0;
	  break;
	case 110: /* Insert */
	  carry = true;
	  the_carry = 82;
	  return 0;
	  break;
	case 111: /* Delete */
	  carry = true;
	  the_carry = 83;
	  return 0;
	  break;
	default:
	  if (*shift_state & ALTGR)
	  {
	    c = alt_key_map[c];
	    if (c) return c;
	  }
	  else
	    if (c < 128) buffer[where_am_i++] = c;
	  break;
      }
    } while (!where_am_i);
  where_am_i--;
  if (*shift_state & SHIFT)
    c = shift_key_map[buffer[0]];
  else
    if (*shift_state & ALT)
      c = 0;
    else
      if (*shift_state & CONTROL)
      {
	if (ctrl_key_map[buffer[0]] < 32) /* Ctrl-A to Ctrl-Z etc */
	  c = ctrl_key_map[buffer[0]];
	else
	{
	  c = 0;
	  carry = true;
	  the_carry = ctrl_key_map[buffer[0]];
	}
      }
      else
	c = key_map[buffer[0]];
  
  if (where_am_i)
    memcpy (buffer, buffer + 1, where_am_i);
  
  if (c == 0 && !carry) 
  {
    carry = true;
    if (buffer[0] == 87)
      the_carry = 113;
    else
      if (buffer[0] == 88)
	the_carry = 114;
      else
	the_carry = buffer[0];
  }
  return c;
}

/* Purpose: check to see whether a key has been pressed
   Returns: TRUE if key available (KEYBOARD::get() returns the key),
   FALSE otherwise */

bool KEYBOARD::hit (void)
{
  uchar c;
  
  if (where_am_i) return TRUE;
  read (kb_file, &c, 1);
  if (c == 191) return FALSE;
  do
  {
    if ((c & 128) == 0) buffer[where_am_i++] = c;
    read (kb_file, &c, 1);
  } while (c != 191);
  return TRUE;
}

