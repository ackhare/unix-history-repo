/*
 * FreeBSD install - a package for the installation and maintainance
 * of non-core utilities.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * Jordan K. Hubbard
 * 23 Aug 1993
 *
 * Various display routines for the info module.
 *
 */

#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include "lib.h"
#include "info.h"
#include <err.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <md5.h>

void
show_file(const char *title, const char *fname)
{
    FILE *fp;
    char line[1024];
    int n;

    if (!Quiet)
	printf("%s%s", InfoPrefix, title);
    fp = fopen(fname, "r");
    if (!fp)
	printf("ERROR: show_file: Can't open '%s' for reading!\n", fname);
    else {
	while ((n = fread(line, 1, 1024, fp)) != 0)
	    fwrite(line, 1, n, stdout);
	fclose(fp);
    }
    printf("\n");	/* just in case */
}

void
show_index(const char *title, const char *fname)
{
    FILE *fp;
    char line[MAXINDEXSIZE+2];

    if (!Quiet)
        printf("%s%s", InfoPrefix, title);
    fp = fopen(fname, "r");
    if (!fp) {
        warnx("show_file: can't open '%s' for reading", fname);
        return;
    }
    if(fgets(line, MAXINDEXSIZE+1, fp)) {
	if(line[MAXINDEXSIZE-1] != '\n')
          line[MAXINDEXSIZE] = '\n';
	line[MAXINDEXSIZE+1] = 0;
	fputs(line, stdout);
    }
    fclose(fp);
}

/* Show a packing list item type.  If showall is TRUE, show all */
void
show_plist(const char *title, Package *plist, plist_t type, Boolean showall)
{
    PackingList p;
    Boolean ign = FALSE;

    if (!Quiet)
	printf("%s%s", InfoPrefix, title);
    p = plist->head;
    while (p) {
	if (p->type != type && showall != TRUE) {
	    p = p->next;
	    continue;
	}
	switch(p->type) {
	case PLIST_FILE:
	    if (ign) {
		printf(Quiet ? "%s\n" : "File: %s (ignored)\n", p->name);
		ign = FALSE;
	    }
	    else
		printf(Quiet ? "%s\n" : "File: %s\n", p->name);
	    break;

	case PLIST_CWD:
	    printf(Quiet ? "@cwd %s\n" : "\tCWD to %s\n", p->name);
	    break;

	case PLIST_SRC:
	    printf(Quiet ? "@srcdir %s\n" : "\tSRCDIR to %s\n", p->name);
	    break;

	case PLIST_CMD:
	    printf(Quiet ? "@exec %s\n" : "\tEXEC '%s'\n", p->name);
	    break;

	case PLIST_UNEXEC:
	    printf(Quiet ? "@unexec %s\n" : "\tUNEXEC '%s'\n", p->name);
	    break;

	case PLIST_CHMOD:
	    printf(Quiet ? "@chmod %s\n" : "\tCHMOD to %s\n",
		   p->name ? p->name : "(clear default)");
	    break;

	case PLIST_CHOWN:
	    printf(Quiet ? "@chown %s\n" : "\tCHOWN to %s\n",
		   p->name ? p->name : "(clear default)");
	    break;

	case PLIST_CHGRP:
	    printf(Quiet ? "@chgrp %s\n" : "\tCHGRP to %s\n",
		   p->name ? p->name : "(clear default)");
	    break;

	case PLIST_COMMENT:
	    printf(Quiet ? "@comment %s\n" : "\tComment: %s\n", p->name);
	    break;

	case PLIST_IGNORE:
	    ign = TRUE;
	    break;

	case PLIST_IGNORE_INST:
	    printf(Quiet ? "@ignore_inst ??? doesn't belong here.\n" :
		   "\tIgnore next file installation directive (doesn't belong)\n");
	    ign = TRUE;
	    break;

	case PLIST_NAME:
	    printf(Quiet ? "@name %s\n" : "\tPackage name: %s\n", p->name);
	    break;

	case PLIST_DISPLAY:
	    printf(Quiet ? "@display %s\n" : "\tInstall message file: %s\n", p->name);
	    break;

	case PLIST_PKGDEP:
	    printf(Quiet ? "@pkgdep %s\n" : "\t%s\n", p->name);
	    break;

	case PLIST_MTREE:
	    printf(Quiet ? "@mtree %s\n" : "\tPackage mtree file: %s\n", p->name);
	    break;

	case PLIST_DIR_RM:
	    printf(Quiet ? "@dirrm %s\n" : "\tDeinstall directory remove: %s\n", p->name);
	    break;

	case PLIST_OPTION:
	    printf(Quiet ? "@option %s\n" :
		"\tOption \"%s\" controlling package installation behaviour\n",
		p->name);
	    break;

	default:
	    cleanup(0);
	    errx(2, __FUNCTION__ ": unknown command type %d (%s)", p->type, p->name);
	    break;
	}
	p = p->next;
    }
}

/* Show all files in the packing list (except ignored ones) */
void
show_files(const char *title, Package *plist)
{
    PackingList p;
    Boolean ign = FALSE;
    const char *dir = ".";

    if (!Quiet)
	printf("%s%s", InfoPrefix, title);
    p = plist->head;
    while (p) {
	switch(p->type) {
	case PLIST_FILE:
	    if (!ign)
		printf("%s/%s\n", dir, p->name);
	    ign = FALSE;
	    break;

	case PLIST_CWD:
	    dir = p->name;
	    break;

	case PLIST_IGNORE:
	    ign = TRUE;
	    break;

        /* Silence GCC in the -Wall mode */
	default:
	    break;
	}
	p = p->next;
    }
}

/* Calculate and show size of all installed package files (except ignored ones) */
void
show_size(const char *title, Package *plist)
{
    PackingList p;
    Boolean ign = FALSE;
    const char *dir = ".";
    struct stat sb;
    char tmp[FILENAME_MAX];
    unsigned long size = 0;
    long blksize;
    int headerlen;
    char *descr;

    descr = getbsize(&headerlen, &blksize);
    if (!Quiet)
	printf("%s%s", InfoPrefix, title);
    for (p = plist->head; p != NULL; p = p->next) {
	switch (p->type) {
	case PLIST_FILE:
	    if (!ign) {
		snprintf(tmp, FILENAME_MAX, "%s/%s", dir, p->name);
		if (!lstat(tmp, &sb)) {
		    size += sb.st_size;
		    if (Verbose)
			printf("%lu\t%s\n", (unsigned long) howmany(sb.st_size, blksize), tmp);
		}
	    }
	    ign = FALSE;
	    break;

	case PLIST_CWD:
	    dir = p->name;
	    break;

	case PLIST_IGNORE:
	    ign = TRUE;
	    break;

	/* Silence GCC in the -Wall mode */	    
	default:
	    break;
	}
    }
    if (!Quiet)
	printf("%lu\t(%s)\n", howmany(size, blksize), descr);
    else
	printf("%lu\n", size);
}

/* Show files that don't match the recorded checksum */
void
show_cksum(const char *title, Package *plist)
{
    PackingList p;
    const char *dir = ".";
    char tmp[FILENAME_MAX];

    if (!Quiet)
	printf("%s%s", InfoPrefix, title);

    for (p = plist->head; p != NULL; p = p->next)
	if (p->type == PLIST_CWD) 
	    dir = p->name;
	else if (p->type == PLIST_FILE) {
	    snprintf(tmp, FILENAME_MAX, "%s/%s", dir, p->name);
	    if (!fexists(tmp))
		warnx("%s doesn't exist\n", tmp);
	    else if (p->next && p->next->type == PLIST_COMMENT && !strncmp(p->next->name, "MD5:", 4)) {
		char *cp, buf[33];
		if ((cp = MD5File(tmp, buf)) != NULL) {
		    if (strcmp(cp, p->next->name + 4))
			printf("%s fails the original MD5 checksum\n", tmp);
		    else if (Verbose)
			printf("%s matched the original MD5 checksum\n", tmp);
		}
	    }
	}
}

/* Show an "origin" path (usually category/portname) */
void
show_origin(const char *title, Package *plist)
{
    PackingList p;

    if (!Quiet)
	printf("%s%s", InfoPrefix, title);
    for (p = plist->head; p != NULL; p = p->next)
	if (p->type == PLIST_COMMENT && !strncmp(p->name, "ORIGIN:", 7)) {
	    printf("%s\n", p->name + 7);
	    break;
	}
}

/* Show revision number of the packing list */
void
show_fmtrev(const char *title, Package *plist)
{
    if (!Quiet)
	printf("%s%s", InfoPrefix, title);
    printf("%d.%d\n", plist->fmtver_maj, plist->fmtver_mnr);
}
