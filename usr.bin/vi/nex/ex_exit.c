/*-
 * Copyright (c) 1992, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by the University of
 *	California, Berkeley and its contributors.
 * 4. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef lint
static char sccsid[] = "@(#)ex_exit.c	8.7 (Berkeley) 12/10/93";
#endif /* not lint */

#include <sys/types.h>

#include "vi.h"
#include "excmd.h"

/*
 * ex_quit -- :quit[!]
 *	Quit.
 */
int
ex_quit(sp, ep, cmdp)
	SCR *sp;
	EXF *ep;
	EXCMDARG *cmdp;
{
	int force;

	force = F_ISSET(cmdp, E_FORCE);

	/* Check for modifications. */
	if (F_ISSET(ep, F_MODIFIED) && ep->refcnt <= 1 && !force) {
		msgq(sp, M_ERR,
		    "Modified since last write; write or use ! to override.");
		return (1);
	}

	/*
	 * !!!
	 * Historic practice: quit! or two quit's done in succession
	 * (where ZZ counts as a quit) didn't check for other files.
	 *
	 * Also check for related screens; if they exist, quit, the
	 * user will get the message on the last screen.
	 */
	if (!force && sp->ccnt != sp->q_ccnt + 1 &&
	    ep->refcnt <= 1 && file_unedited(sp) != NULL) {
		sp->q_ccnt = sp->ccnt;
		msgq(sp, M_ERR,
	"More files; use \":n\" to go to the next file, \":q!\" to quit.");
		return (1);
	}

	F_SET(sp, force ? S_EXIT_FORCE : S_EXIT);
	return (0);
}
