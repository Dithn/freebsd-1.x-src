/* Data structures associated with breakpoints in GDB.
   Copyright (C) 1992 Free Software Foundation, Inc.

This file is part of GDB.

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */

#if !defined (BREAKPOINT_H)
#define BREAKPOINT_H 1

#include "frame.h"
#include "value.h"

/* This is the maximum number of bytes a breakpoint instruction can take.
   Feel free to increase it.  It's just used in a few places to size
   arrays that should be independent of the target architecture.  */

#define	BREAKPOINT_MAX	16

/* Type of breakpoint. */
/* FIXME In the future, we should fold all other breakpoint-like things into
   here.  This includes:

   * single-step (for machines where we have to simulate single stepping)
      (probably, though perhaps it is better for it to look as much as
      possible like a single-step to wait_for_inferior).  */

enum bptype {
  bp_breakpoint,		/* Normal breakpoint */
  bp_until,			/* used by until command */
  bp_finish,			/* used by finish command */
  bp_watchpoint,		/* Watchpoint */
  bp_longjmp,			/* secret breakpoint to find longjmp() */
  bp_longjmp_resume,		/* secret breakpoint to escape longjmp() */

  /* Used by wait_for_inferior for stepping over subroutine calls, for
     stepping over signal handlers, and for skipping prologues.  */
  bp_step_resume,

  /* The breakpoint at the end of a call dummy.  */
  /* FIXME: What if the function we are calling longjmp()s out of the
     call, or the user gets out with the "return" command?  We currently
     have no way of cleaning up the breakpoint in these (obscure) situations.
     (Probably can solve this by noticing longjmp, "return", etc., it's
     similar to noticing when a watchpoint on a local variable goes out
     of scope (with hardware support for watchpoints)).  */
  bp_call_dummy
};

/* States of enablement of breakpoint. */

enum enable { disabled, enabled};

/* Disposition of breakpoint.  Ie: what to do after hitting it. */

enum bpdisp {
  delete,			/* Delete it */
  disable,			/* Disable it */
  donttouch			/* Leave it alone */
};

/* Note that the ->silent field is not currently used by any commands
   (though the code is in there if it was to be, and set_raw_breakpoint
   does set it to 0).  I implemented it because I thought it would be
   useful for a hack I had to put in; I'm going to leave it in because
   I can see how there might be times when it would indeed be useful */

/* This is for a breakpoint or a watchpoint.  */

struct breakpoint
{
  struct breakpoint *next;
  /* Type of breakpoint. */
  enum bptype type;
  /* Zero means disabled; remember the info but don't break here.  */
  enum enable enable;
  /* What to do with this breakpoint after we hit it. */
  enum bpdisp disposition;
  /* Number assigned to distinguish breakpoints.  */
  int number;

  /* Address to break at, or NULL if not a breakpoint.  */
  CORE_ADDR address;

  /* Line number of this address.  Only matters if address is
     non-NULL.  */

  int line_number;

  /* Source file name of this address.  Only matters if address is
     non-NULL.  */

  char *source_file;

  /* Non-zero means a silent breakpoint (don't print frame info
     if we stop here). */
  unsigned char silent;
  /* Number of stops at this breakpoint that should
     be continued automatically before really stopping.  */
  int ignore_count;
  /* "Real" contents of byte where breakpoint has been inserted.
     Valid only when breakpoints are in the program.  Under the complete
     control of the target insert_breakpoint and remove_breakpoint routines.
     No other code should assume anything about the value(s) here.  */
  char shadow_contents[BREAKPOINT_MAX];
  /* Nonzero if this breakpoint is now inserted.  Only matters if address
     is non-NULL.  */
  char inserted;
  /* Nonzero if this is not the first breakpoint in the list
     for the given address.  Only matters if address is non-NULL.  */
  char duplicate;
  /* Chain of command lines to execute when this breakpoint is hit.  */
  struct command_line *commands;
  /* Stack depth (address of frame).  If nonzero, break only if fp
     equals this.  */
  FRAME_ADDR frame;
  /* Conditional.  Break only if this expression's value is nonzero.  */
  struct expression *cond;

  /* String we used to set the breakpoint (malloc'd).  Only matters if
     address is non-NULL.  */
  char *addr_string;
  /* String form of the breakpoint condition (malloc'd), or NULL if there
     is no condition.  */
  char *cond_string;
  /* String form of exp (malloc'd), or NULL if none.  */
  char *exp_string;

  /* The expression we are watching, or NULL if not a watchpoint.  */
  struct expression *exp;
  /* The largest block within which it is valid, or NULL if it is
     valid anywhere (e.g. consists just of global symbols).  */
  struct block *exp_valid_block;
  /* Value of the watchpoint the last time we checked it.  */
  value val;
  /* Thread number for thread-specific breakpoint, or -1 if don't care */
  int thread;
};

/* The following stuff is an abstract data type "bpstat" ("breakpoint status").
   This provides the ability to determine whether we have stopped at a
   breakpoint, and what we should do about it.  */

typedef struct bpstat *bpstat;

/* Interface:  */
/* Clear a bpstat so that it says we are not at any breakpoint.
   Also free any storage that is part of a bpstat.  */
extern void bpstat_clear PARAMS ((bpstat *));

/* Return a copy of a bpstat.  Like "bs1 = bs2" but all storage that
   is part of the bpstat is copied as well.  */
extern bpstat bpstat_copy PARAMS ((bpstat));

/* FIXME:  prototypes uses equivalence between FRAME_ADDR and CORE_ADDR */
extern bpstat bpstat_stop_status PARAMS ((CORE_ADDR *, CORE_ADDR, int));

/* This bpstat_what stuff tells wait_for_inferior what to do with a
   breakpoint (a challenging task).  */

enum bpstat_what_main_action {
  /* Perform various other tests; that is, this bpstat does not
     say to perform any action (e.g. failed watchpoint and nothing
     else).  */
  BPSTAT_WHAT_KEEP_CHECKING,

  /* Rather than distinguish between noisy and silent stops here, it
     might be cleaner to have bpstat_print make that decision (also
     taking into account stop_print_frame and source_only).  But the
     implications are a bit scary (interaction with auto-displays, etc.),
     so I won't try it.  */
     
  /* Stop silently.  */
  BPSTAT_WHAT_STOP_SILENT,

  /* Stop and print.  */
  BPSTAT_WHAT_STOP_NOISY,

  /* Remove breakpoints, single step once, then put them back in and
     go back to what we were doing.  It's possible that this should be
     removed from the main_action and put into a separate field, to more
     cleanly handle BPSTAT_WHAT_CLEAR_LONGJMP_RESUME_SINGLE.  */
  BPSTAT_WHAT_SINGLE,

  /* Set longjmp_resume breakpoint, remove all other breakpoints,
     and continue.  The "remove all other breakpoints" part is required
     if we are also stepping over another breakpoint as well as doing
     the longjmp handling.  */
  BPSTAT_WHAT_SET_LONGJMP_RESUME,

  /* Clear longjmp_resume breakpoint, then handle as
     BPSTAT_WHAT_KEEP_CHECKING.  */
  BPSTAT_WHAT_CLEAR_LONGJMP_RESUME,

  /* Clear longjmp_resume breakpoint, then handle as BPSTAT_WHAT_SINGLE.  */
  BPSTAT_WHAT_CLEAR_LONGJMP_RESUME_SINGLE,

  /* This is just used to keep track of how many enums there are.  */
  BPSTAT_WHAT_LAST
};

struct bpstat_what {
  enum bpstat_what_main_action main_action;

  /* Did we hit the step resume breakpoint?  This is separate from the
     main_action to allow for it to be combined with any of the main
     actions.  */
  int step_resume;

  /* Did we hit a call dummy breakpoint?  This only goes with a main_action
     of BPSTAT_WHAT_STOP_SILENT or BPSTAT_WHAT_STOP_NOISY (the concept of
     continuing from a call dummy without popping the frame is not a
     useful one).  */
  int call_dummy;
};

/* Tell what to do about this bpstat.  */
struct bpstat_what bpstat_what PARAMS ((bpstat));

/* Find the bpstat associated with a breakpoint.  NULL otherwise. */
bpstat bpstat_find_breakpoint PARAMS ((bpstat, struct breakpoint *));

/* Nonzero if a signal that we got in wait() was due to circumstances
   explained by the BS.  */
/* Currently that is true if we have hit a breakpoint, or if there is
   a watchpoint enabled.  */
#define bpstat_explains_signal(bs) ((bs) != NULL)

/* Nonzero if we should step constantly (e.g. watchpoints on machines
   without hardware support).  This isn't related to a specific bpstat,
   just to things like whether watchpoints are set.  */
extern int bpstat_should_step PARAMS ((void));

/* Print a message indicating what happened.  Returns nonzero to
   say that only the source line should be printed after this (zero
   return means print the frame as well as the source line).  */
extern int bpstat_print PARAMS ((bpstat));

/* Return the breakpoint number of the first breakpoint we are stopped
   at.  *BSP upon return is a bpstat which points to the remaining
   breakpoints stopped at (but which is not guaranteed to be good for
   anything but further calls to bpstat_num).
   Return 0 if passed a bpstat which does not indicate any breakpoints.  */
extern int bpstat_num PARAMS ((bpstat *));

/* Perform actions associated with having stopped at *BSP.  Actually, we just
   use this for breakpoint commands.  Perhaps other actions will go here
   later, but this is executed at a late time (from the command loop).  */
extern void bpstat_do_actions PARAMS ((bpstat *));

/* Modify BS so that the actions will not be performed.  */
extern void bpstat_clear_actions PARAMS ((bpstat));

/* Implementation:  */
struct bpstat
{
  /* Linked list because there can be two breakpoints at the
     same place, and a bpstat reflects the fact that both have been hit.  */
  bpstat next;
  /* Breakpoint that we are at.  */
  struct breakpoint *breakpoint_at;
  /* Commands left to be done.  */
  struct command_line *commands;
  /* Old value associated with a watchpoint.  */
  value old_val;

  /* Nonzero if this breakpoint tells us to print the frame.  */
  char print;

  /* Nonzero if this breakpoint tells us to stop.  */
  char stop;

  /* Function called by bpstat_print to print stuff associated with
     this element of the bpstat chain.  Returns 0 or 1 just like
     bpstat_print, or -1 if it can't deal with it.  */
  int (*print_it) PARAMS((bpstat bs));
};

/* Prototypes for breakpoint-related functions.  */

#ifdef __STDC__		/* Forward declarations for prototypes */
struct frame_info;
#endif

extern int
breakpoint_here_p PARAMS ((CORE_ADDR));

extern int
breakpoint_thread_match PARAMS ((CORE_ADDR, int));

extern void
until_break_command PARAMS ((char *, int));

extern void
breakpoint_re_set PARAMS ((void));

extern void
clear_momentary_breakpoints PARAMS ((void));

/* FIXME:  Prototype uses equivalence of "struct frame_info *" and FRAME */
extern struct breakpoint *
set_momentary_breakpoint PARAMS ((struct symtab_and_line,
				  struct frame_info *,
				  enum bptype));

extern void
set_ignore_count PARAMS ((int, int, int));

extern void
set_default_breakpoint PARAMS ((int, CORE_ADDR, struct symtab *, int));

extern void
mark_breakpoints_out PARAMS ((void));

extern void
breakpoint_init_inferior PARAMS ((void));

extern void
delete_breakpoint PARAMS ((struct breakpoint *));

extern void
breakpoint_auto_delete PARAMS ((bpstat));

extern void
breakpoint_clear_ignore_counts PARAMS ((void));

extern void
break_command PARAMS ((char *, int));

extern int
insert_breakpoints PARAMS ((void));

extern int
remove_breakpoints PARAMS ((void));

extern void
enable_longjmp_breakpoint PARAMS ((void));

extern void
disable_longjmp_breakpoint PARAMS ((void));

extern void
set_longjmp_resume_breakpoint PARAMS ((CORE_ADDR, FRAME));
 
/* The following are for displays, which aren't really breakpoints, but
   here is as good a place as any for them.  */

extern void
disable_current_display PARAMS ((void));

extern void
do_displays PARAMS ((void));

extern void
disable_display PARAMS ((int));

extern void
clear_displays PARAMS ((void));

#endif /* !defined (BREAKPOINT_H) */
