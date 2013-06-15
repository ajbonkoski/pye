#include "termio.h"

/* Many functions in this file were adapted from a MicroEmacs variant.
   (pEmacs: https://github.com/hughbarney/pEmacs on 6/7/2013) */

#undef CTRL
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>      /* exit */
#include <signal.h>
#include <stdio.h>		 /* puts(3), setbuffer(3), ... */
#include <sys/ioctl.h>   /* to get at the typeahead */

#define	TBUFSIZ	128

static char tobuf[TBUFSIZ];		/* terminal output buffer */
static struct termios ostate, nstate;

/*
 * This function is called once to set up the terminal device streams.
 */
bool ttopen()
{
    // raw input mode stops working after ouputting colors.
    // this setting seems to fix it, but is a bad bad hack...
    setenv("TERM", "xterm-256color", 1);

    /* save terminal flags */
    if ((tcgetattr(0, &ostate) < 0) || (tcgetattr(0, &nstate) < 0)) {
        ERROR("Can't read terminal capabilites\n");
        return false;
    }

    cfmakeraw(&nstate);		/* set raw mode */
    nstate.c_cc[VMIN] = 1;
    nstate.c_cc[VTIME] = 0;	/* block indefinitely for a single char */
    if (tcsetattr(0, TCSADRAIN, &nstate) < 0) {
        ERROR("Can't set terminal mode\n");
        return false;
    }
    /* provide a smaller terminal output buffer so that the type ahead
     * detection works better (more often) */
    setbuffer(stdout, &tobuf[0], TBUFSIZ);
    signal(SIGTSTP, SIG_DFL);

    return true;
}

/*
 * This function gets called just before we go back home to the command
 * interpreter
 */
bool ttclose ()
{
  ttflush();
  if (tcsetattr(0, TCSADRAIN, &ostate) < 0) {
      ERROR("Can't restore terminal flags");
      return false;
  }
  return true;
}

/*
 * Write a character to the display
 */
int ttputc (int c)
{
    return fputc(c, stdout);
}

/*
 * Flush terminal buffer. Does real work where the terminal output is buffered
 * up. A no-operation on systems where byte at a time terminal I/O is done
 */
void ttflush ()
{
    tcdrain(0);
    fflush(stdout);
}

/*
 * Read a character from the terminal, performing no editing and doing no echo
 * at all
 */
int ttgetc ()
{
    return (127 & fgetc (stdin));
}

/* typahead: Check to see if any characters are already in the keyboard buffer
 */
int typahead ()
{
    int x;			/* holds # of pending chars */
    return ((ioctl (0, FIONREAD, &x) < 0) ? 0 : x);
}
