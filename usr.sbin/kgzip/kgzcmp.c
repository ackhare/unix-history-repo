/*
 * Copyright (c) 1999 Global Technology Associates, Inc.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
 * OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT
 * OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * $FreeBSD$
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

#include <err.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <a.out.h>
#include <elf.h>

#include "elfhdr.h"
#include "kgzip.h"

#define KGZOFF (sizeof(struct kgz_elfhdr) + sizeof(struct kgz_hdr))

#define F_AOUT	1		/* Input format: a.out */
#define F_ELF	2		/* Input format: ELF32 */

static void mk_data(const struct iodesc *i, const struct iodesc *,
		    struct kgz_hdr *);
static int ld_elf(const struct iodesc *, const struct iodesc *,
		  struct kgz_hdr *, const Elf32_Ehdr *);
static int ld_aout(const struct iodesc *, const struct iodesc *,
		   struct kgz_hdr *, const struct exec *);

/*
 * Compress executable and output it in relocatable object format.
 */
void
kgzcmp(struct kgz_hdr *kh, const char *f1, const char *f2)
{
    struct iodesc idi, ido;
    struct kgz_elfhdr ehdr;

    if ((idi.fd = open(idi.fname = f1, O_RDONLY)) == -1)
	err(1, "%s", idi.fname);
    if ((ido.fd = open(ido.fname = f2, O_CREAT | O_TRUNC | O_WRONLY,
		       0666)) == -1)
	err(1, "%s", ido.fname);
    kh->ident[0] = KGZ_ID0;
    kh->ident[1] = KGZ_ID1;
    kh->ident[2] = KGZ_ID2;
    kh->ident[3] = KGZ_ID3;
    xseek(&ido, KGZOFF);
    mk_data(&idi, &ido, kh);
    kh->dload &= 0xffffff;
    kh->entry &= 0xffffff;
    xseek(&ido, 0);
    ehdr = elfhdr;
    ehdr.st[KGZ_ST_KGZ_NDATA].st_size = kh->nsize;
    ehdr.sh[KGZ_SH_DATA].sh_size += kh->nsize;
    xwrite(&ido, &ehdr, sizeof(ehdr));
    xwrite(&ido, kh, sizeof(*kh));
    xclose(&ido);
    xclose(&idi);
}

/*
 * Make encoded (compressed) data.
 */
static void
mk_data(const struct iodesc * idi, const struct iodesc * ido,
	struct kgz_hdr * kh)
{
    union {
	struct exec ex;
	Elf32_Ehdr ee;
    } hdr;
    struct stat sb;
    struct iodesc idp;
    int fd[2];
    pid_t pid;
    size_t n;
    int fmt, status, e;

    n = xread(idi, &hdr, sizeof(hdr), 0);
    fmt = 0;
    if (n >= sizeof(hdr.ee) && IS_ELF(hdr.ee))
	fmt = F_ELF;
    else if (n >= sizeof(hdr.ex) && N_GETMAGIC(hdr.ex) == ZMAGIC)
	fmt = F_AOUT;
    if (!fmt)
	errx(1, "%s: Format not supported", idi->fname);
    if (pipe(fd))
	err(1, NULL);
    switch (pid = fork()) {
    case -1:
	err(1, NULL);
    case 0:
	close(fd[1]);
	dup2(fd[0], STDIN_FILENO);
	close(fd[0]);
	close(idi->fd);
	dup2(ido->fd, STDOUT_FILENO);
	close(ido->fd);
	execlp("gzip", "gzip", "-9", NULL);
	warn(NULL);
	_exit(1);
    default:
	close(fd[0]);
	idp.fname = "(pipe)";
	idp.fd = fd[1];
	e = fmt == F_ELF  ? ld_elf(idi, &idp, kh, &hdr.ee) :
	    fmt == F_AOUT ? ld_aout(idi, &idp, kh, &hdr.ex) : -1;
	close(fd[1]);
	if ((pid = waitpid(pid, &status, 0)) == -1)
	    err(1, NULL);
	if (WIFSIGNALED(status) || WEXITSTATUS(status))
	    exit(1);
    }
    if (e)
	errx(1, "%s: Invalid format", idi->fname);
    if (fstat(ido->fd, &sb))
	err(1, "%s", ido->fname);
    kh->nsize = sb.st_size - KGZOFF;
}

/*
 * "Load" an ELF-format executable.
 */
static int
ld_elf(const struct iodesc * idi, const struct iodesc * ido,
       struct kgz_hdr * kh, const Elf32_Ehdr * e)
{
    Elf32_Phdr p;
    size_t load, addr, n;
    unsigned x, i;

    load = addr = n = 0;
    for (x = i = 0; i < e->e_phnum; i++) {
	if (xread(idi, &p, sizeof(p),
		  e->e_phoff + i * e->e_phentsize) != e->e_phentsize)
	    return -1;
	if (p.p_type != PT_LOAD)
	    continue;
	if (!x)
	    load = addr = p.p_vaddr;
	else {
	    if (p.p_vaddr < addr)
		return -1;
	    n = p.p_vaddr - addr;
	    if (n) {
		xzero(ido, n);
		addr += n;
	    }
	}
	if (p.p_memsz < p.p_filesz)
	    return -1;
	n = p.p_memsz - p.p_filesz;
	xcopy(idi, ido, p.p_filesz, p.p_offset);
	addr += p.p_filesz;
	x++;
    }
    if (!x)
	return -1;
    kh->dload = load;
    kh->dsize = addr - load;
    kh->isize = kh->dsize + n;
    kh->entry = e->e_entry;
    return 0;
}

/*
 * "Load" an a.out-format executable.
 */
static int
ld_aout(const struct iodesc * idi, const struct iodesc * ido,
	struct kgz_hdr * kh, const struct exec * a)
{
    size_t load, addr;

    load = addr = N_TXTADDR(*a);
    xcopy(idi, ido, a->a_text, N_TXTOFF(*a));
    addr += a->a_text;
    if (N_DATADDR(*a) != addr)
	return -1;
    xcopy(idi, ido, a->a_data, N_DATOFF(*a));
    addr += a->a_data;
    kh->dload = load;
    kh->dsize = addr - load;
    kh->isize = kh->dsize + a->a_bss;
    kh->entry = a->a_entry;
    return 0;
}
