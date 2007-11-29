/* Miscellaneous functions, not really specific to GNU tar.

   Copyright 1988, 1992, 1994, 1995, 1996, 1997, 1999, 2000, 2001 Free
   Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation; either version 2, or (at your option) any later
   version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
   Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation, Inc.,
   59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

/* $FreeBSD$ */

#include "system.h"
#include "rmt.h"
#include "common.h"
#include <quotearg.h>
#include <save-cwd.h>

static void call_arg_fatal PARAMS ((char const *, char const *))
     __attribute__ ((noreturn));

/* Handling strings.  */

/* Assign STRING to a copy of VALUE if not zero, or to zero.  If
   STRING was nonzero, it is freed first.  */
void
assign_string (char **string, const char *value)
{
  if (*string)
    free (*string);
  *string = value ? xstrdup (value) : 0;
}

/* Allocate a copy of the string quoted as in C, and returns that.  If
   the string does not have to be quoted, it returns a null pointer.
   The allocated copy should normally be freed with free() after the
   caller is done with it.

   This is used in one context only: generating the directory file in
   incremental dumps.  The quoted string is not intended for human
   consumption; it is intended only for unquote_string.  The quoting
   is locale-independent, so that users needn't worry about locale
   when reading directory files.  This means that we can't use
   quotearg, as quotearg is locale-dependent and is meant for human
   consumption.  */
char *
quote_copy_string (const char *string)
{
  const char *source = string;
  char *destination = 0;
  char *buffer = 0;
  int copying = 0;

  while (*source)
    {
      int character = *source++;

      switch (character)
	{
	case '\n': case '\\':
	  if (!copying)
	    {
	      size_t length = (source - string) - 1;

	      copying = 1;
	      buffer = xmalloc (length + 2 + 2 * strlen (source) + 1);
	      memcpy (buffer, string, length);
	      destination = buffer + length;
	    }
	  *destination++ = '\\';
	  *destination++ = character == '\\' ? '\\' : 'n';
	  break;

	default:
	  if (copying)
	    *destination++ = character;
	  break;
	}
    }
  if (copying)
    {
      *destination = '\0';
      return buffer;
    }
  return 0;
}

/* Takes a quoted C string (like those produced by quote_copy_string)
   and turns it back into the un-quoted original.  This is done in
   place.  Returns 0 only if the string was not properly quoted, but
   completes the unquoting anyway.
 									   
   This is used for reading the saved directory file in incremental
   dumps.  It is used for decoding old `N' records (demangling names).
   But also, it is used for decoding file arguments, would they come
   from the shell or a -T file, and for decoding the --exclude
   argument.  */
int
unquote_string (char *string)
{
  int result = 1;
  char *source = string;
  char *destination = string;

  /* Escape sequences other than \\ and \n are no longer generated by
     quote_copy_string, but accept them for backwards compatibility,
     and also because unquote_string is used for purposes other than
     parsing the output of quote_copy_string.  */

  while (*source)
    if (*source == '\\')
      switch (*++source)
	{
	case '\\':
	  *destination++ = '\\';
	  source++;
	  break;

	case 'n':
	  *destination++ = '\n';
	  source++;
	  break;

	case 't':
	  *destination++ = '\t';
	  source++;
	  break;

	case 'f':
	  *destination++ = '\f';
	  source++;
	  break;

	case 'b':
	  *destination++ = '\b';
	  source++;
	  break;

	case 'r':
	  *destination++ = '\r';
	  source++;
	  break;

	case '?':
	  *destination++ = 0177;
	  source++;
	  break;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	  {
	    int value = *source++ - '0';

	    if (*source < '0' || *source > '7')
	      {
		*destination++ = value;
		break;
	      }
	    value = value * 8 + *source++ - '0';
	    if (*source < '0' || *source > '7')
	      {
		*destination++ = value;
		break;
	      }
	    value = value * 8 + *source++ - '0';
	    *destination++ = value;
	    break;
	  }

	default:
	  result = 0;
	  *destination++ = '\\';
	  if (*source)
	    *destination++ = *source++;
	  break;
	}
    else if (source != destination)
      *destination++ = *source++;
    else
      source++, destination++;

  if (source != destination)
    *destination = '\0';
  return result;
}

/* Return nonzero if NAME contains ".." as a path name component.  */
int
contains_dot_dot (char const *name)
{
  char const *p = name + FILESYSTEM_PREFIX_LEN (name);

  for (;;)
    {
      if (p[0] == '.' && p[1] == '.' && (ISSLASH (p[2]) || !p[2]))
	return 1;

      while (! ISSLASH (*p))
	{
	  if (! *p++)
	    return 0;
	}

      do
	{
	  if (! *p++)
	    return 0;
	}
      while ( ISSLASH (*p));
    }
}

/* File handling.  */

/* Saved names in case backup needs to be undone.  */
static char *before_backup_name;
static char *after_backup_name;

/* Some implementations of rmdir let you remove the working directory.
   Report an error with errno set to zero for obvious cases of this;
   otherwise call rmdir.  */
static int
safer_rmdir (const char *path)
{
  while (path[0] == '.' && ISSLASH (path[1]))
    {
      path++;

      do path++;
      while (ISSLASH (*path));
    }

  if (! path[0] || (path[0] == '.' && ! path[1]))
    {
      errno = 0;
      return -1;
    }

  return rmdir (path);
}

/* Remove PATH.  If PATH is a directory, then if RECURSE is set remove
   it recursively; otherwise, remove it only if it is empty.  Return 0
   on error, with errno set; if PATH is obviously the working
   directory return zero with errno set to zero.  */
int
remove_any_file (const char *path, int recurse)
{
  /* Try unlink first if we are not root, as this saves us a system
     call in the common case where we're removing a non-directory.  */
  if (! we_are_root)
    {
      if (unlink (path) == 0)
	return 1;
      if (errno != EPERM)
	return 0;
    }

  if (safer_rmdir (path) == 0)
    return 1;

  switch (errno)
    {
    case ENOTDIR:
      return we_are_root && unlink (path) == 0;

    case 0:
    case EEXIST:
#if defined ENOTEMPTY && ENOTEMPTY != EEXIST
    case ENOTEMPTY:
#endif
      if (recurse)
	{
	  char *directory = savedir (path);
	  char const *entry;
	  size_t entrylen;

	  if (! directory)
	    return 0;

	  for (entry = directory;
	       (entrylen = strlen (entry)) != 0;
	       entry += entrylen + 1)
	    {
	      char *path_buffer = new_name (path, entry);
	      int r = remove_any_file (path_buffer, 1);
	      int e = errno;
	      free (path_buffer);

	      if (! r)
		{
		  free (directory);
		  errno = e;
		  return 0;
		}
	    }

	  free (directory);
	  return safer_rmdir (path) == 0;
	}
      break;
    }

  return 0;
}

/* Check if PATH already exists and make a backup of it right now.
   Return success (nonzero) only if the backup in either unneeded, or
   successful.  For now, directories are considered to never need
   backup.  If ARCHIVE is nonzero, this is the archive and so, we do
   not have to backup block or character devices, nor remote entities.  */
int
maybe_backup_file (const char *path, int archive)
{
  struct stat file_stat;

  /* Check if we really need to backup the file.  */

  if (archive && _remdev (path))
    return 1;

  if (stat (path, &file_stat))
    {
      if (errno == ENOENT)
	return 1;

      stat_error (path);
      return 0;
    }

  if (S_ISDIR (file_stat.st_mode))
    return 1;

  if (archive && (S_ISBLK (file_stat.st_mode) || S_ISCHR (file_stat.st_mode)))
    return 1;

  assign_string (&before_backup_name, path);

  /* A run situation may exist between Emacs or other GNU programs trying to
     make a backup for the same file simultaneously.  If theoretically
     possible, real problems are unlikely.  Doing any better would require a
     convention, GNU-wide, for all programs doing backups.  */

  assign_string (&after_backup_name, 0);
  after_backup_name = find_backup_file_name (path, backup_type);
  if (! after_backup_name)
    xalloc_die ();

  if (rename (before_backup_name, after_backup_name) == 0)
    {
      if (verbose_option)
	fprintf (stdlis, _("Renaming %s to %s\n"),
		 quote_n (0, before_backup_name),
		 quote_n (1, after_backup_name));
      return 1;
    }
  else
    {
      /* The backup operation failed.  */
      int e = errno;
      ERROR ((0, e, _("%s: Cannot rename to %s"),
	      quotearg_colon (before_backup_name),
	      quote_n (1, after_backup_name)));
      assign_string (&after_backup_name, 0);
      return 0;
    }
}

/* Try to restore the recently backed up file to its original name.
   This is usually only needed after a failed extraction.  */
void
undo_last_backup (void)
{
  if (after_backup_name)
    {
      if (rename (after_backup_name, before_backup_name) != 0)
	{
	  int e = errno;
	  ERROR ((0, e, _("%s: Cannot rename to %s"),
		  quotearg_colon (after_backup_name),
		  quote_n (1, before_backup_name)));
	}
      if (verbose_option)
	fprintf (stdlis, _("Renaming %s back to %s\n"),
		 quote_n (0, after_backup_name),
		 quote_n (1, before_backup_name));
      assign_string (&after_backup_name, 0);
    }
}

/* Depending on DEREF, apply either stat or lstat to (NAME, BUF).  */
int
deref_stat (int deref, char const *name, struct stat *buf)
{
  return deref ? stat (name, buf) : lstat (name, buf);
}

/* A description of a working directory.  */
struct wd
{
  char const *name;
  int saved;
  struct saved_cwd saved_cwd;
};

/* A vector of chdir targets.  wd[0] is the initial working directory.  */
static struct wd *wd;

/* The number of working directories in the vector.  */
static size_t wds;

/* The allocated size of the vector.  */
static size_t wd_alloc;

/* DIR is the operand of a -C option; add it to vector of chdir targets,
   and return the index of its location.  */
int
chdir_arg (char const *dir)
{
  if (wds == wd_alloc)
    {
      wd_alloc = 2 * (wd_alloc + 1);
      wd = xrealloc (wd, sizeof *wd * wd_alloc);
      if (! wds)
	{
	  wd[wds].name = ".";
	  wd[wds].saved = 0;
	  wds++;
	}
    }

  /* Optimize the common special case of the working directory,
     or the working directory as a prefix.  */
  if (dir[0])
    {
      while (dir[0] == '.' && ISSLASH (dir[1]))
	for (dir += 2;  ISSLASH (*dir);  dir++)
	  continue;
      if (! dir[dir[0] == '.'])
	return wds - 1;
    }

  wd[wds].name = dir;
  wd[wds].saved = 0;
  return wds++;
}

/* Change to directory I.  If I is 0, change to the initial working
   directory; otherwise, I must be a value returned by chdir_arg.  */
void
chdir_do (int i)
{
  static int previous;

  if (previous != i)
    {
      struct wd *prev = &wd[previous];
      struct wd *curr = &wd[i];

      if (! prev->saved)
	{
	  prev->saved = 1;
	  if (save_cwd (&prev->saved_cwd) != 0)
	    FATAL_ERROR ((0, 0, _("Cannot save working directory")));
	}

      if (curr->saved)
	{
	  if (restore_cwd (&curr->saved_cwd, curr->name, prev->name))
	    FATAL_ERROR ((0, 0, _("Cannot change working directory")));
	}
      else
	{
	  if (i && ! ISSLASH (curr->name[0]))
	    chdir_do (i - 1);
	  if (chdir (curr->name) != 0)
	    chdir_fatal (curr->name);
	}

      previous = i;
    }
}

/* Decode MODE from its binary form in a stat structure, and encode it
   into a 9-byte string STRING, terminated with a NUL.  */

void
decode_mode (mode_t mode, char *string)
{
  *string++ = mode & S_IRUSR ? 'r' : '-';
  *string++ = mode & S_IWUSR ? 'w' : '-';
  *string++ = (mode & S_ISUID
	       ? (mode & S_IXUSR ? 's' : 'S')
	       : (mode & S_IXUSR ? 'x' : '-'));
  *string++ = mode & S_IRGRP ? 'r' : '-';
  *string++ = mode & S_IWGRP ? 'w' : '-';
  *string++ = (mode & S_ISGID
	       ? (mode & S_IXGRP ? 's' : 'S')
	       : (mode & S_IXGRP ? 'x' : '-'));
  *string++ = mode & S_IROTH ? 'r' : '-';
  *string++ = mode & S_IWOTH ? 'w' : '-';
  *string++ = (mode & S_ISVTX
	       ? (mode & S_IXOTH ? 't' : 'T')
	       : (mode & S_IXOTH ? 'x' : '-'));
  *string = '\0';
}

/* Report an error associated with the system call CALL and the
   optional name NAME.  */
static void
call_arg_error (char const *call, char const *name)
{
  int e = errno;
  ERROR ((0, e, _("%s: Cannot %s"), quotearg_colon (name), call));
}

/* Report a fatal error associated with the system call CALL and
   the optional file name NAME.  */
static void
call_arg_fatal (char const *call, char const *name)
{
  int e = errno;
  FATAL_ERROR ((0, e, _("%s: Cannot %s"), quotearg_colon (name),  call));
}

/* Report a warning associated with the system call CALL and
   the optional file name NAME.  */
static void
call_arg_warn (char const *call, char const *name)
{
  int e = errno;
  WARN ((0, e, _("%s: Warning: Cannot %s"), quotearg_colon (name), call));
}

void
chdir_fatal (char const *name)
{
  call_arg_fatal ("chdir", name);
}

void
chmod_error_details (char const *name, mode_t mode)
{
  int e = errno;
  ERROR ((0, e, _("%s: Cannot change mode to 0%o"),
	  quotearg_colon (name), mode));
}

void
chown_error_details (char const *name, uid_t uid, gid_t gid)
{
  int e = errno;
  ERROR ((0, e, _("%s: Cannot change ownership to uid %lu, gid %lu"),
	  quotearg_colon (name), (unsigned long) uid, (unsigned long) gid));
}

void
close_error (char const *name)
{
  call_arg_error ("close", name);
}

void
close_fatal (char const *name)
{
  call_arg_fatal ("close", name);
}

void
close_warn (char const *name)
{
  call_arg_warn ("close", name);
}

void
exec_fatal (char const *name)
{
  call_arg_fatal ("exec", name);
}

void
link_error (char const *target, char const *source)
{
  int e = errno;
  ERROR ((0, e, _("%s: Cannot hard link to %s"),
	  quotearg_colon (source), quote_n (1, target)));
}

void
mkdir_error (char const *name)
{
  call_arg_error ("mkdir", name);
}

void
mkfifo_error (char const *name)
{
  call_arg_error ("mkfifo", name);
}

void
mknod_error (char const *name)
{
  call_arg_error ("mknod", name);
}

void
open_error (char const *name)
{
  call_arg_error ("open", name);
}

void
open_fatal (char const *name)
{
  call_arg_fatal ("open", name);
}

void
open_warn (char const *name)
{
  call_arg_warn ("open", name);
}

void
read_error (char const *name)
{
  call_arg_error ("read", name);
}

void
read_error_details (char const *name, off_t offset, size_t size)
{
  char buf[UINTMAX_STRSIZE_BOUND];
  int e = errno;
  ERROR ((0, e,
	  _("%s: Read error at byte %s, reading %lu bytes"),
	  quotearg_colon (name), STRINGIFY_BIGINT (offset, buf),
	  (unsigned long) size));
}

void
read_warn_details (char const *name, off_t offset, size_t size)
{
  char buf[UINTMAX_STRSIZE_BOUND];
  int e = errno;
  WARN ((0, e,
	 _("%s: Warning: Read error at byte %s, reading %lu bytes"),
	 quotearg_colon (name), STRINGIFY_BIGINT (offset, buf),
	 (unsigned long) size));
}

void
read_fatal (char const *name)
{
  call_arg_fatal ("read", name);
}

void
read_fatal_details (char const *name, off_t offset, size_t size)
{
  char buf[UINTMAX_STRSIZE_BOUND];
  int e = errno;
  FATAL_ERROR ((0, e,
		_("%s: Read error at byte %s, reading %lu bytes"),
		quotearg_colon (name), STRINGIFY_BIGINT (offset, buf),
		(unsigned long) size));
}

void
readlink_error (char const *name)
{
  call_arg_error ("readlink", name);
}

void
readlink_warn (char const *name)
{
  call_arg_warn ("readlink", name);
}

void
savedir_error (char const *name)
{
  call_arg_error ("savedir", name);
}

void
savedir_warn (char const *name)
{
  call_arg_warn ("savedir", name);
}

void
seek_error (char const *name)
{
  call_arg_error ("seek", name);
}

void
seek_error_details (char const *name, off_t offset)
{
  char buf[UINTMAX_STRSIZE_BOUND];
  int e = errno;
  ERROR ((0, e, _("%s: Cannot seek to %s"),
	  quotearg_colon (name),
	  STRINGIFY_BIGINT (offset, buf)));
}

void
seek_warn (char const *name)
{
  call_arg_warn ("seek", name);
}

void
seek_warn_details (char const *name, off_t offset)
{
  char buf[UINTMAX_STRSIZE_BOUND];
  int e = errno;
  WARN ((0, e, _("%s: Warning: Cannot seek to %s"),
	 quotearg_colon (name),
	 STRINGIFY_BIGINT (offset, buf)));
}

void
symlink_error (char const *contents, char const *name)
{
  int e = errno;
  ERROR ((0, e, _("%s: Cannot create symlink to %s"),
	  quotearg_colon (name), quote_n (1, contents)));
}

void
stat_error (char const *name)
{
  call_arg_error ("stat", name);
}

void
stat_warn (char const *name)
{
  call_arg_warn ("stat", name);
}

void
truncate_error (char const *name)
{
  call_arg_error ("truncate", name);
}

void
truncate_warn (char const *name)
{
  call_arg_warn ("truncate", name);
}

void
unlink_error (char const *name)
{
  call_arg_error ("unlink", name);
}

void
utime_error (char const *name)
{
  call_arg_error ("utime", name);
}

void
waitpid_error (char const *name)
{
  call_arg_error ("waitpid", name);
}

void
write_error (char const *name)
{
  call_arg_error ("write", name);
}

void
write_error_details (char const *name, ssize_t status, size_t size)
{
  if (status < 0)
    write_error (name);
  else
    ERROR ((0, 0, _("%s: Wrote only %lu of %lu bytes"),
	    name, (unsigned long) status, (unsigned long) record_size));
}

void
write_fatal (char const *name)
{
  call_arg_fatal ("write", name);
}

void
write_fatal_details (char const *name, ssize_t status, size_t size)
{
  write_error_details (name, status, size);
  fatal_exit ();
}


/* Fork, aborting if unsuccessful.  */
pid_t
xfork (void)
{
  pid_t p = fork ();
  if (p == (pid_t) -1)
    call_arg_fatal ("fork", _("child process"));
  return p;
}

/* Create a pipe, aborting if unsuccessful.  */
void
xpipe (int fd[2])
{
  if (pipe (fd) < 0)
    call_arg_fatal ("pipe", _("interprocess channel"));
}

/* Return an unambiguous printable representation, allocated in slot N,
   for NAME, suitable for diagnostics.  */
char const *
quote_n (int n, char const *name)
{
  return quotearg_n_style (n, locale_quoting_style, name);
}

/* Return an unambiguous printable representation of NAME, suitable
   for diagnostics.  */
char const *
quote (char const *name)
{
  return quote_n (0, name);
}
