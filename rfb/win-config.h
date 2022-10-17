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

/* build date */
#ifndef LIBVNCASYNC_BUILD_DATE
#define LIBVNCASYNC_BUILD_DATE "Tue Nov 28 11:33:11 CET 2017"
#endif

/* work around when write() returns ENOENT but does not mean it */
/* #undef ENOENT_WORKAROUND */

/* Define to 1 if you have the `crypt' function. */
/* #undef HAVE_CRYPT */

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef LIBVNCASYNC_HAVE_DLFCN_H
#define LIBVNCASYNC_HAVE_DLFCN_H 1
#endif

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
/* #undef HAVE_DOPRNT */

/* Define to 1 if you have the <endian.h> header file. */
#ifndef LIBVNCASYNC_HAVE_ENDIAN_H
#define LIBVNCASYNC_HAVE_ENDIAN_H 1
#endif

/* Define to 1 if you have the <fcntl.h> header file. */
#ifndef LIBVNCASYNC_HAVE_FCNTL_H
#define LIBVNCASYNC_HAVE_FCNTL_H 1
#endif

/* Define to 1 if you have the `fork' function. */
#ifndef LIBVNCASYNC_HAVE_FORK
#define LIBVNCASYNC_HAVE_FORK 1
#endif

/* Define to 1 if you have the `ftime' function. */
#ifndef LIBVNCASYNC_HAVE_FTIME
#define LIBVNCASYNC_HAVE_FTIME 1
#endif

/* Define to 1 if you have the `gethostbyname' function. */
#ifndef LIBVNCASYNC_HAVE_GETHOSTBYNAME
#define LIBVNCASYNC_HAVE_GETHOSTBYNAME 1
#endif

/* Define to 1 if you have the `gethostname' function. */
#ifndef LIBVNCASYNC_HAVE_GETHOSTNAME
#define LIBVNCASYNC_HAVE_GETHOSTNAME 1
#endif

/* Define to 1 if you have the `gettimeofday' function. */
#ifndef LIBVNCASYNC_HAVE_GETTIMEOFDAY
#define LIBVNCASYNC_HAVE_GETTIMEOFDAY 1
#endif

/* GnuTLS library present */
#ifndef LIBVNCASYNC_HAVE_GNUTLS
#define LIBVNCASYNC_HAVE_GNUTLS 1
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef LIBVNCASYNC_HAVE_INTTYPES_H
#define LIBVNCASYNC_HAVE_INTTYPES_H 1
#endif

/* libcrypt library present */
#ifndef LIBVNCASYNC_HAVE_LIBCRYPT
#define LIBVNCASYNC_HAVE_LIBCRYPT 1
#endif

/* openssl libcrypto library present */
#ifndef LIBVNCASYNC_HAVE_LIBCRYPTO
#define LIBVNCASYNC_HAVE_LIBCRYPTO 1
#endif

/* Define to 1 if you have the `cygipc' library (-lcygipc). */
/* #undef HAVE_LIBCYGIPC */

/* libjpeg support enabled */
#ifndef LIBVNCASYNC_HAVE_LIBJPEG
#define LIBVNCASYNC_HAVE_LIBJPEG 1
#endif

/* Define to 1 if you have the `png' library (-lpng). */
#ifndef LIBVNCASYNC_HAVE_LIBPNG
#define LIBVNCASYNC_HAVE_LIBPNG 1
#endif

/* openssl libssl library present */
/* #undef HAVE_LIBSSL */

/* Define to 1 if you have the `z' library (-lz). */
#ifndef LIBVNCASYNC_HAVE_LIBZ
#define LIBVNCASYNC_HAVE_LIBZ 1
#endif

/* Define to 1 if you have the `memmove' function. */
#ifndef LIBVNCASYNC_HAVE_MEMMOVE
#define LIBVNCASYNC_HAVE_MEMMOVE 1
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef LIBVNCASYNC_HAVE_MEMORY_H
#define LIBVNCASYNC_HAVE_MEMORY_H 1
#endif

/* Define to 1 if you have the `memset' function. */
#ifndef LIBVNCASYNC_HAVE_MEMSET
#define LIBVNCASYNC_HAVE_MEMSET 1
#endif

/* Define to 1 if you have the `mkfifo' function. */
#ifndef LIBVNCASYNC_HAVE_MKFIFO
#define LIBVNCASYNC_HAVE_MKFIFO 1
#endif

/* Define to 1 if you have the `mmap' function. */
#ifndef LIBVNCASYNC_HAVE_MMAP
#define LIBVNCASYNC_HAVE_MMAP 1
#endif

/* Define to 1 if `stat' has the bug that it succeeds when given the
   zero-length file name argument. */
/* #undef HAVE_STAT_EMPTY_STRING_BUG */

/* Define to 1 if you have the <stdint.h> header file. */
#ifndef LIBVNCASYNC_HAVE_STDINT_H
#define LIBVNCASYNC_HAVE_STDINT_H 1
#endif

/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef LIBVNCASYNC_HAVE_STDLIB_H
#define LIBVNCASYNC_HAVE_STDLIB_H 1
#endif

/* Define to 1 if you have the `strchr' function. */
#ifndef LIBVNCASYNC_HAVE_STRCHR
#define LIBVNCASYNC_HAVE_STRCHR 1
#endif

/* Define to 1 if you have the `strcspn' function. */
#ifndef LIBVNCASYNC_HAVE_STRCSPN
#define LIBVNCASYNC_HAVE_STRCSPN 1
#endif

/* Define to 1 if you have the `strdup' function. */
#ifndef LIBVNCASYNC_HAVE_STRDUP
#define LIBVNCASYNC_HAVE_STRDUP 1
#endif

/* Define to 1 if you have the `strerror' function. */
#ifndef LIBVNCASYNC_HAVE_STRERROR
#define LIBVNCASYNC_HAVE_STRERROR 1
#endif

/* Define to 1 if you have the `strftime' function. */
#ifndef LIBVNCASYNC_HAVE_STRFTIME
#define LIBVNCASYNC_HAVE_STRFTIME 1
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef LIBVNCASYNC_HAVE_STRINGS_H
#define LIBVNCASYNC_HAVE_STRINGS_H 1
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef LIBVNCASYNC_HAVE_STRING_H
#define LIBVNCASYNC_HAVE_STRING_H 1
#endif

/* Define to 1 if you have the `strstr' function. */
#ifndef LIBVNCASYNC_HAVE_STRSTR
#define LIBVNCASYNC_HAVE_STRSTR 1
#endif

/* Define to 1 if you have the <syslog.h> header file. */
#ifndef LIBVNCASYNC_HAVE_SYSLOG_H
#define LIBVNCASYNC_HAVE_SYSLOG_H 1
#endif

/* Define to 1 if you have the <sys/endian.h> header file. */
/* #undef HAVE_SYS_ENDIAN_H */

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef LIBVNCASYNC_HAVE_SYS_STAT_H
#define LIBVNCASYNC_HAVE_SYS_STAT_H 1
#endif

/* Define to 1 if you have the <sys/timeb.h> header file. */
#ifndef LIBVNCASYNC_HAVE_SYS_TIMEB_H
#define LIBVNCASYNC_HAVE_SYS_TIMEB_H 1
#endif

/* Define to 1 if you have the <sys/time.h> header file. */
#ifndef LIBVNCASYNC_HAVE_SYS_TIME_H
#define LIBVNCASYNC_HAVE_SYS_TIME_H 1
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef LIBVNCASYNC_HAVE_SYS_TYPES_H
#define LIBVNCASYNC_HAVE_SYS_TYPES_H 1
#endif

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#ifndef LIBVNCASYNC_HAVE_SYS_WAIT_H
#define LIBVNCASYNC_HAVE_SYS_WAIT_H 1
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef LIBVNCASYNC_HAVE_UNISTD_H
#define LIBVNCASYNC_HAVE_UNISTD_H 1
#endif

/* Define to 1 if you have the `vfork' function. */
#ifndef LIBVNCASYNC_HAVE_VFORK
#define LIBVNCASYNC_HAVE_VFORK 1
#endif

/* Define to 1 if you have the <vfork.h> header file. */
/* #undef HAVE_VFORK_H */

/* Define to 1 if you have the `vprintf' function. */
#ifndef LIBVNCASYNC_HAVE_VPRINTF
#define LIBVNCASYNC_HAVE_VPRINTF 1
#endif

/* Define to 1 if `fork' works. */
#ifndef LIBVNCASYNC_HAVE_WORKING_FORK
#define LIBVNCASYNC_HAVE_WORKING_FORK 1
#endif

/* Define to 1 if `vfork' works. */
#ifndef LIBVNCASYNC_HAVE_WORKING_VFORK
#define LIBVNCASYNC_HAVE_WORKING_VFORK 1
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

/* Name of package */
#ifndef LIBVNCASYNC_PACKAGE
#define LIBVNCASYNC_PACKAGE "libvncasync"
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef LIBVNCASYNC_PACKAGE_BUGREPORT
#define LIBVNCASYNC_PACKAGE_BUGREPORT "https://svn.code.sf.net/p/libhandlers/code/nomad/vnc"
#endif

/* Define to the full name of this package. */
#ifndef LIBVNCASYNC_PACKAGE_NAME
#define LIBVNCASYNC_PACKAGE_NAME "libvncasync"
#endif

/* Define to the full name and version of this package. */
#ifndef LIBVNCASYNC_PACKAGE_STRING
#define LIBVNCASYNC_PACKAGE_STRING "libvncasync 0.0.1"
#endif

/* Define to the one symbol short name of this package. */
#ifndef LIBVNCASYNC_PACKAGE_TARNAME
#define LIBVNCASYNC_PACKAGE_TARNAME "libvncasync"
#endif

/* Define to the home page for this package. */
#ifndef LIBVNCASYNC_PACKAGE_URL
#define LIBVNCASYNC_PACKAGE_URL ""
#endif

/* Define to the version of this package. */
#ifndef LIBVNCASYNC_PACKAGE_VERSION
#define LIBVNCASYNC_PACKAGE_VERSION "0.0.1"
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef LIBVNCASYNC_STDC_HEADERS
#define LIBVNCASYNC_STDC_HEADERS 1
#endif

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#ifndef LIBVNCASYNC_TIME_WITH_SYS_TIME
#define LIBVNCASYNC_TIME_WITH_SYS_TIME 1
#endif

/* Version number of package */
#ifndef LIBVNCASYNC_VERSION
#define LIBVNCASYNC_VERSION "0.0.1"
#endif

/* libvncasync major version */
#ifndef LIBVNCASYNC_VERSION_MAJOR
#define LIBVNCASYNC_VERSION_MAJOR 0
#endif

/* libvncasync minor version */
#ifndef LIBVNCASYNC_VERSION_MINOR
#define LIBVNCASYNC_VERSION_MINOR 0
#endif

/* libvncasync patchlevel */
#ifndef LIBVNCASYNC_VERSION_PATCHLEVEL
#define LIBVNCASYNC_VERSION_PATCHLEVEL 1
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

#define LIBVNCSERVER_HAVE_LIBZ
#define BPP 32
#define EXTRA_ARGS , rfbClientPtr cl

#define O_RDONLY  1
#define O_CREAT   2
#define O_WRONLY  3
#define O_TRUNC   5
