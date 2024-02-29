#ifndef _RFB_RFBCONFIG_H
#define _RFB_RFBCONFIG_H 1

/* rfb/rfbconfig.h. Generated automatically at end of configure. */
/* rfbconfig.h.  Generated from rfbconfig.h.in by configure.  */
/* rfbconfig.h.in.  Generated from configure.ac by autoheader.  */

/* library authors */
#ifndef LIBVNCASYNC_AUTHORS
#define LIBVNCASYNC_AUTHORS "Jose Angel Caso Sanchez (JACS) altomaltes@yahoo.es"
#endif

#define LIBVNCSERVER_PACKAGE_STRING "LIBVNCSERVER_PACKAGE_STRING"


/* work around when write() returns ENOENT but does not mean it */
/* #undef ENOENT_WORKAROUND */

/* Define to 1 if you have the `crypt' function. */
/* #undef HAVE_CRYPT */

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef HAVE_DLFCN_H
#define HAVE_DLFCN_H 1
#endif

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <endian.h> header file. */
#ifndef HAVE_ENDIAN_H
#define HAVE_ENDIAN_H 1
#endif

/* Define to 1 if you have the <fcntl.h> header file. */
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H 1
#endif


/* Define to 1 if you have the `ftime' function. */
#ifndef HAVE_FTIME
#define HAVE_FTIME 1
#endif



/* GnuTLS library present */
#ifndef HAVE_GNUTLS
#define HAVE_GNUTLS 1
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef HAVE_INTTYPES_H
#define HAVE_INTTYPES_H 1
#endif

/* Define to 1 if you have the `cygipc' library (-lcygipc). */
/* #undef HAVE_LIBCYGIPC */

/* libjpeg support enabled */
#ifndef HAVE_LIBJPEG
#define HAVE_LIBJPEG 1
#endif

/* Define to 1 if you have the `png' library (-lpng). */
#ifndef HAVE_LIBPNG
#define HAVE_LIBPNG 1
#endif

/* openssl libssl library present */
/* #undef HAVE_LIBSSL */

/* Define to 1 if you have the `z' library (-lz). */
#ifndef HAVE_LIBZ
#define HAVE_LIBZ 1
#endif

/* Define to 1 if you have the `memmove' function. */
#ifndef HAVE_MEMMOVE
#define HAVE_MEMMOVE 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef HAVE_MEMORY_H
#define HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the `memset' function. */
#ifndef HAVE_MEMSET
#define HAVE_MEMSET 1
#endif

/* Define to 1 if you have the `mkfifo' function. */
#ifndef HAVE_MKFIFO
#define HAVE_MKFIFO 1
#endif

/* Define to 1 if you have the `mmap' function. */
#ifndef HAVE_MMAP
#define HAVE_MMAP 1
#endif

/* Define to 1 if `stat' has the bug that it succeeds when given the
   zero-length file name argument. */
/* #undef HAVE_STAT_EMPTY_STRING_BUG */

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef HAVE_STDINT_H
#define HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the `strchr' function. */
#ifndef HAVE_STRCHR
#define HAVE_STRCHR 1
#endif

/* Define to 1 if you have the `strcspn' function. */
#ifndef HAVE_STRCSPN
#define HAVE_STRCSPN 1
#endif

/* Define to 1 if you have the `strdup' function. */
#ifndef HAVE_STRDUP
#define HAVE_STRDUP 1
#endif

/* Define to 1 if you have the `strerror' function. */
#ifndef HAVE_STRERROR
#define HAVE_STRERROR 1
#endif

/* Define to 1 if you have the `strftime' function. */
#ifndef HAVE_STRFTIME
#define HAVE_STRFTIME 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef HAVE_STRINGS_H
#define HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef HAVE_STRING_H
#define HAVE_STRING_H 1
#endif

/* Define to 1 if you have the `strstr' function. */
#ifndef HAVE_STRSTR
#define HAVE_STRSTR 1
#endif

/* Define to 1 if you have the <syslog.h> header file. */
#ifndef HAVE_SYSLOG_H
#define HAVE_SYSLOG_H 1
#endif

/* Define to 1 if you have the <sys/endian.h> header file. */
/* #undef HAVE_SYS_ENDIAN_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef HAVE_SYS_STAT_H
#define HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/timeb.h> header file. */
#ifndef HAVE_SYS_TIMEB_H
#define HAVE_SYS_TIMEB_H 1
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef HAVE_SYS_TIME_H
#define HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef HAVE_SYS_TYPES_H
#define HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#ifndef HAVE_SYS_WAIT_H
#define HAVE_SYS_WAIT_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the `vfork' function. */
#ifndef HAVE_VFORK
#define HAVE_VFORK 1
#endif

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `vprintf' function. */
#ifndef HAVE_VPRINTF
#define HAVE_VPRINTF 1
#endif

/* Define to 1 if `fork' works. */
#ifndef HAVE_WORKING_FORK
#define HAVE_WORKING_FORK 1
#endif

/* Define to 1 if `vfork' works. */
#ifndef HAVE_WORKING_VFORK
#define HAVE_WORKING_VFORK 1
#endif

/* Define to 1 if `lstat' dereferences a symlink specified with a trailing
   slash. */
#ifndef LIBVNCASYNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
#define LIBVNCASYNC_LSTAT_FOLLOWS_SLASHED_SYMLINK 1
#endif

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#ifndef LIBVNCASYNC_LT_OBJDIR
#define LIBVNCASYNC_LT_OBJDIR ".libs/"
#endif




/* Enable support for libgcrypt in libvncclient */
#ifndef LIBVNCASYNC_WITH_CLIENT_GCRYPT
#define LIBVNCASYNC_WITH_CLIENT_GCRYPT 1
#endif

/* Disable TightVNCFileTransfer protocol */
/* #undef WITH_TIGHTVNC_FILETRANSFER */

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef inline */
#endif

/* Define to `int' if <sys/types.h> does not define. */
/* #undef pid_t */

/* Define to `unsigned int' if <sys/types.h> does not define. */
/* #undef size_t */

/* Define as `fork' if `vfork' does not work. */
/* #undef vfork */

/* once: _RFB_RFBCONFIG_H */
#endif


typedef unsigned int UINT32;

#include <stdint.h>

#define HAVE_LIBZ
#define BPP 32
#define EXTRA_ARGS , rfbClient * cl

#define O_RDONLY  1
#define O_CREAT   2
#define O_WRONLY  3
#define O_TRUNC   5
