/*	time.c	4.3	83/02/03	*/

#include "sh.h"

/*
 * C Shell - routines handling process timing and niceing
 */
struct	tms times0;
struct	tms timesdol;

settimes()
{
	struct rusage ruch;

	gettimeofday(&time0, (struct timezone *)0);
	getrusage(RUSAGE_SELF, &ru0);
	getrusage(RUSAGE_CHILDREN, &ruch);
	ruadd(&ru0, &ruch);
}

/*
 * dotime is only called if it is truly a builtin function and not a
 * prefix to another command
 */
dotime()
{
	struct timeval timedol;
	struct rusage ru1, ruch;

	getrusage(RUSAGE_SELF, &ru1);
	getrusage(RUSAGE_CHILDREN, &ruch);
	ruadd(&ru1, &ruch);
	gettimeofday(&timedol, (struct timezone *)0);
	prusage(&ru0, &ru1, &timedol, &time0);
}

/*
 * donice is only called when it on the line by itself or with a +- value
 */
donice(v)
	register char **v;
{
	register char *cp;

	v++, cp = *v++;
	if (cp == 0) {
#ifndef V6
		nice(20);
		nice(-10);
#endif
		nice(4);
	} else if (*v == 0 && any(cp[0], "+-")) {
#ifndef V6
		nice(20);
		nice(-10);
#endif
		nice(getn(cp));
	}
}

ruadd(ru, ru2)
	register struct rusage *ru, *ru2;
{
	register long *lp, *lp2;
	register int cnt;

	tvadd(&ru->ru_utime, &ru2->ru_utime);
	tvadd(&ru->ru_stime, &ru2->ru_stime);
	if (ru2->ru_maxrss > ru->ru_maxrss)
		ru->ru_maxrss = ru2->ru_maxrss;
	cnt = &ru->ru_last - &ru->ru_first + 1;
	lp = &ru->ru_first; lp2 = &ru2->ru_first;
	do
		*lp++ += *lp2++;
	while (--cnt > 0);
}

prusage(r0, r1, e, b)
	register struct rusage *r0, *r1;
	struct timeval *e, *b;
{
	register time_t t =
	    (r1->ru_utime.tv_sec-r0->ru_utime.tv_sec)*100+
	    (r1->ru_utime.tv_usec-r0->ru_utime.tv_usec)/10000+
	    (r1->ru_stime.tv_sec-r0->ru_stime.tv_sec)*100+
	    (r1->ru_stime.tv_usec-r0->ru_stime.tv_usec)/10000;
	register char *cp;
	register int i;
	register struct varent *vp = adrof("time");
	int ms =
	    (e->tv_sec-b->tv_sec)*100 + (e->tv_usec-b->tv_usec)/10000;

	cp = "%Uu %Ss %E %P %X+%Dk %I+%Oio %Fpf+%Ww";
	if (vp && vp->vec[0] && vp->vec[1])
		cp = vp->vec[1];
	for (; *cp; cp++)
	if (*cp != '%')
		putchar(*cp);
	else if (cp[1]) switch(*++cp) {

	case 'U':
		pdeltat(&r1->ru_utime, &r0->ru_utime);
		break;

	case 'S':
		pdeltat(&r1->ru_stime, &r0->ru_stime);
		break;

	case 'E':
		psecs(ms / 100);
		break;

	case 'P':
		printf("%d%%", (int) (t*100 / ((ms ? ms : 1))));
		break;

	case 'W':
		i = r1->ru_nswap - r0->ru_nswap;
		printf("%d", i);
		break;

	case 'X':
		printf("%d", t == 0 ? 0 : (r1->ru_ixrss-r0->ru_ixrss)/t);
		break;

	case 'D':
		printf("%d", t == 0 ? 0 :
		    (r1->ru_idrss+r1->ru_isrss-(r0->ru_idrss+r0->ru_isrss))/t);
		break;

	case 'K':
		printf("%d", t == 0 ? 0 :
		    ((r1->ru_ixrss+r1->ru_isrss+r1->ru_idrss) -
		    (r0->ru_ixrss+r0->ru_idrss+r0->ru_isrss))/t);
		break;

	case 'M':
		printf("%d", r1->ru_maxrss/2);
		break;

	case 'F':
		printf("%d", r1->ru_majflt-r0->ru_majflt);
		break;

	case 'R':
		printf("%d", r1->ru_minflt-r0->ru_minflt);
		break;

	case 'I':
		printf("%d", r1->ru_inblock-r0->ru_inblock);
		break;

	case 'O':
		printf("%d", r1->ru_oublock-r0->ru_oublock);
		break;
	}
	putchar('\n');
}

pdeltat(t1, t0)
	struct timeval *t1, *t0;
{
	struct timeval td;

	tvsub(&td, t1, t0);
	printf("%d.%01d", td.tv_sec, td.tv_usec/100000);
}

tvadd(tsum, t0)
	struct timeval *tsum, *t0;
{

	tsum->tv_sec += t0->tv_sec;
	tsum->tv_usec += t0->tv_usec;
	if (tsum->tv_usec > 1000000)
		tsum->tv_sec++, tsum->tv_usec -= 1000000;
}

tvsub(tdiff, t1, t0)
	struct timeval *tdiff, *t1, *t0;
{

	tdiff->tv_sec = t1->tv_sec - t0->tv_sec;
	tdiff->tv_usec = t1->tv_usec - t0->tv_usec;
	if (tdiff->tv_usec < 0)
		tdiff->tv_sec--, tdiff->tv_usec += 1000000;
}
