/*-
 * Copyright (c) 1993, 1994
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
static char sccsid[] = "@(#)ex_util.c	9.4 (Berkeley) 11/13/94";
#endif /* not lint */

#include <sys/types.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <sys/time.h>

#include <bitstring.h>
#include <errno.h>
#include <limits.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#include "compat.h"
#include <db.h>
#include <regex.h>

#include "vi.h"
#include "excmd.h"

/*
 * ex_cbuild --
 *	Build an EX command structure.
 */
void
ex_cbuild(cmdp, cmd_id, naddr, lno1, lno2, force, ap, a, arg)
	EXCMDARG *cmdp;
	int cmd_id, force, naddr;
	recno_t lno1, lno2;
	ARGS *ap[2], *a;
	char *arg;
{
	memset(cmdp, 0, sizeof(EXCMDARG));
	cmdp->cmd = &cmds[cmd_id];
	cmdp->addrcnt = naddr;
	cmdp->addr1.lno = lno1;
	cmdp->addr2.lno = lno2;
	cmdp->addr1.cno = cmdp->addr2.cno = 1;
	if (force)
		cmdp->flags |= E_FORCE;
	if ((a->bp = arg) == NULL) {
		cmdp->argc = 0;
		a->len = 0;
	} else {
		cmdp->argc = 1;
		a->len = strlen(arg);
	}
	ap[0] = a;
	ap[1] = NULL;
	cmdp->argv = ap;
}

/*
 * ex_getline --
 *	Return a line from the terminal.
 */
int
ex_getline(sp, fp, lenp)
	SCR *sp;
	FILE *fp;
	size_t *lenp;
{
	EX_PRIVATE *exp;
	size_t off;
	int ch;
	char *p;

	exp = EXP(sp);
	for (errno = 0, off = 0, p = exp->ibp;;) {
		if (off >= exp->ibp_len) {
			BINC_RET(sp, exp->ibp, exp->ibp_len, off + 1);
			p = exp->ibp + off;
		}
		if ((ch = getc(fp)) == EOF && !feof(fp)) {
			if (errno == EINTR) {
				errno = 0;
				clearerr(fp);
				continue;
			}
			return (1);
		}
		if (ch == EOF || ch == '\n') {
			if (ch == EOF && !off)
				return (1);
			*lenp = off;
			return (0);
		}
		*p++ = ch;
		++off;
	}
	/* NOTREACHED */
}

/*
 * ex_comment --
 *	Skip the first comment.
 */
int
ex_comment(sp)
	SCR *sp;
{
	recno_t lno;
	size_t len;
	char *p;

	for (lno = 1;
	    (p = file_gline(sp, lno, &len)) != NULL && len == 0; ++lno);
	if (p == NULL || len <= 1 || memcmp(p, "/*", 2))
		return (0);
	do {
		for (; len; --len, ++p)
			if (p[0] == '*' && len > 1 && p[1] == '/') {
				sp->lno = lno;
				return (0);
			}
	} while ((p = file_gline(sp, ++lno, &len)) != NULL);
	return (0);
}

/*
 * ex_sleave --
 *	Save the terminal/signal state, screen modification time.
 * 	Specific to ex/filter.c and ex/ex_shell.c.
 */
int
ex_sleave(sp)
	SCR *sp;
{
	struct stat sb;
	EX_PRIVATE *exp;

	/* Ignore sessions not using tty's. */
	if (!F_ISSET(sp->gp, G_STDIN_TTY))
		return (1);

	exp = EXP(sp);
	if (tcgetattr(STDIN_FILENO, &exp->leave_term)) {
		msgq(sp, M_SYSERR, "tcgetattr");
		return (1);
	}
	if (tcsetattr(STDIN_FILENO,
	    TCSANOW | TCSASOFT, &sp->gp->original_termios)) {
		msgq(sp, M_SYSERR, "tcsetattr");
		return (1);
	}
	/*
	 * The process may write to the terminal.  Save the access time
	 * (read) and modification time (write) of the tty; if they have
	 * changed when we restore the modes, will have to refresh the
	 * screen.
	 */
	if (fstat(STDIN_FILENO, &sb)) {
		msgq(sp, M_SYSERR, "stat: stdin");
		exp->leave_atime = exp->leave_mtime = 0;
	} else {
		exp->leave_atime = sb.st_atime;
		exp->leave_mtime = sb.st_mtime;
	}
	return (0);
}

/*
 * ex_rleave --
 *	Return the terminal/signal state, not screen modification time.
 * 	Specific to ex/filter.c and ex/ex_shell.c.
 */
void
ex_rleave(sp)
	SCR *sp;
{
	EX_PRIVATE *exp;
	struct stat sb;

	exp = EXP(sp);

	/* Restore the terminal modes. */
	if (tcsetattr(STDIN_FILENO, TCSANOW | TCSASOFT, &exp->leave_term))
		msgq(sp, M_SYSERR, "tcsetattr");

	/* If the terminal was used, refresh the screen. */
	if (fstat(STDIN_FILENO, &sb) || exp->leave_atime == 0 ||
	    exp->leave_atime != sb.st_atime || exp->leave_mtime != sb.st_mtime)
		F_SET(sp, S_SCR_REFRESH);
}

/*
 * ex_ncheck --
 *	Check for more files to edit.
 */
int
ex_ncheck(sp, force)
	SCR *sp;
	int force;
{
	/*
	 * !!!
	 * Historic practice: quit! or two quit's done in succession
	 * (where ZZ counts as a quit) didn't check for other files.
	 */
	if (!force && sp->ccnt != sp->q_ccnt + 1 &&
	    sp->cargv != NULL && sp->cargv[1] != NULL) {
		sp->q_ccnt = sp->ccnt;
		msgq(sp, M_ERR,
"170|More files to edit; use n[ext] to go to the next file, q[uit]! to quit");
		return (1);
	}
	return (0);
}

/*
 * ex_message --
 *	Display a few common messages.
 */
void
ex_message(sp, cmdp, which)
	SCR *sp;
	const EXCMDLIST *cmdp;
	enum exmtype which;
{
	switch (which) {
	case EXM_INTERRUPTED:
		msgq(sp, M_INFO, "134|Interrupted");
		break;
	case EXM_NOPREVRE:
		msgq(sp, M_ERR, "230|No previous regular expression");
		break;
	case EXM_NORC:
		msgq(sp, M_ERR,
	"103|The %s command requires that a file have already been read in",
		    cmdp->name);
		break;
	case EXM_USAGE:
		msgq(sp, M_ERR, "130|Usage: %s", cmdp->usage);
		break;
	}
}
