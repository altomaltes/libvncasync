# $Id: Makefile.am,v 1.1.1.1 2004/08/05 21:47:57 caso Exp $

AUTOMAKE_OPTIONS = foreign
ACLOCAL_AMFLAGS = -I m4
CCOPTS= -DNO_FORK
EXTRA_DIST = configure.ac

lib_LTLIBRARIES= libvncasync.la
AM_CPPFLAGS= -I$(top_srcdir)/common -I$(top_srcdir)/libvncserver ${JPEG_CFLAGS} -DBUILDING_VNCASYNC


library_includedir=$(includedir)/rfb
library_include_HEADERS= rfb/rfb.h rfb/rfbproto.h

libvncasync_la_SOURCES= libvncserver/translate.c libvncserver/auth.c libvncserver/cargs.c libvncserver/corre.c libvncserver/cursor.c libvncserver/cutpaste.c libvncserver/draw.c libvncserver/font.c libvncserver/hextile.c libvncserver/main.c libvncserver/rfbregion.c libvncserver/rfbserver.c libvncserver/rre.c libvncserver/scale.c libvncserver/selbox.c libvncserver/stats.c libvncserver/tight.c libvncserver/ultra.c libvncserver/zlib.c libvncserver/zrlepalettehelper.c  libvncserver/ws_decode.c libvncserver/zrle.c libvncserver/zrleoutstream.c
libvncasync_la_SOURCES+= common/d3des.c common/md5.c common/minilzo.c common/rfbcrypto_included.c common/sha1.c  common/turbojpeg.c common/vncauth.c common/base64.c

libvncasync_la_LDFLAGS= $(JPEG_LIBS) $(LIBPNG_LIBS)
libvncasync_la_LDFLAGS+= -release $(VERSION) -shared

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libvncasync.pc

if WINDOWS
  libvncasync_la_LDFLAGS+= -lws2_32 -no-undefined
endif

if LINUX
  libvncasync_la_LDFLAGS+= -ldl
endif

if BUILDING_SLACK

slack:
	make install DESTDIR=$(abs_builddir)/pkg.${build_cpu}
	$(MKDIR_P) $(abs_builddir)/pkg.${build_cpu}/install
	cp ${top_srcdir}/slack-desc $(abs_builddir)/pkg.${build_cpu}/install
	cd $(abs_builddir)/pkg.${build_cpu}; ${SLACKBUILD} -l y -c n $(abs_builddir)/${PACKAGE}-${VERSION}-${build_cpu}-${SVN_BUILD}.txz

else

slack:
	echo "No slackware package requested ${PACKAGE}-${VERSION}-${SVN_BUILD}"
endif


