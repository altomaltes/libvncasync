#ifndef RFBPROTO_H
#define RFBPROTO_H

/*
    Copyright (C) 2009-2010 D. R. Commander. All Rights Reserved.
    Copyright (C) 2005 Rohit Kumar, Johannes E. Schindelin
    Copyright (C) 2004-2008 Sun Microsystems, Inc. All Rights Reserved.
    Copyright (C) 2000-2002 Constantin Kaplinsky.  All Rights Reserved.
    Copyright (C) 2000 Tridia Corporation.  All Rights Reserved.
    Copyright (C) 1999 AT&T Laboratories Cambridge.  All Rights Reserved.

    This is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This software is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this software; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
    USA.
*/

/*
   rfbproto.h - header file for the RFB protocol version 3.3

   Uses types CARD<n> for an n-bit unsigned integer, INT<n> for an n-bit signed
   integer (for n = 8, 16 and 32).

   All multiple byte integers are in big endian (network) order (most
   significant byte first).  Unless noted otherwise there is no special
   alignment of protocol structures.


   Once the initial handshaking is done, all messages start with a type byte,
   (usually) followed by message-specific data.  The order of definitions in
   this file is as follows:

    (1) Structures used in several types of message.
    (2) Structures used in the initial handshaking.
    (3) Message types.
    (4) Encoding types.
    (5) For each message type, the form of the data following the type byte.
        Sometimes this is defined by a single structure but the more complex
        messages have to be explained by comments.
*/

#include <stdint.h>
#include <stdlib.h>

#include <rfb/rfb.h>


#define FREE( item ) if ( item ) { free( item ); item= NULL; }

#if defined(WIN32) && !defined(__MINGW32__)
#define WORDS_BIGENDIAN
#include <sys/timeb.h>
#endif

#ifdef BUILDING_VNCASYNC           /* Internal use during build */
#if defined( WIN32 )
#include <common/rfb-config.h>
#else
#include <common/rfb-config.h>
#endif

#if HAVE_ENDIAN_H
# include <endian.h>
# if __BYTE_ORDER == __BIG_ENDIAN
#  define WORDS_BIGENDIAN 1
# endif
#endif

/* MS compilers don't have strncasecmp
*/
#ifdef _MSC_VER
#define strncasecmp _strnicmp
#endif

#endif

#ifdef HAVE_LIBZ
#include <zlib.h>
#ifdef __CHECKER__
#undef Z_NULL
#define Z_NULL NULL
#endif
#endif

#define rfbMax(a,b) (((a)>(b))?(a):(b))

#if !defined(WIN32) || defined(__MINGW32__)
#ifdef HAVE_SYS_TIME_H
#include <sys/time.h>
#endif


#undef FALSE
#define FALSE 0
#undef TRUE
#define TRUE -1
#endif

#define MAX_ENCODINGS 64

/*****************************************************************************

   Structures used in several messages

 *****************************************************************************/

/*-----------------------------------------------------------------------------
   Structure used to specify a rectangle.  This structure is a multiple of 4
   bytes so that it can be interspersed with 32-bit pixel data without
   affecting alignment.
*/

typedef struct
{ uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
} rfbRectangle;

#define sz_rfbRectangle 8


/*-----------------------------------------------------------------------------
   Structure used to specify pixel format.
*/

typedef struct
{

  uint8_t bitsPerPixel;   /* 8,16,32 only */

  uint8_t depth;    /* 8 to 32 */

  uint8_t bigEndian;    /* True if multi-byte pixels are interpreted
           as big endian, or if single-bit-per-pixel
           has most significant bit of the byte
           corresponding to first (leftmost) pixel. Of
           course this is meaningless for 8 bits/pix */

  uint8_t trueColour;   /* If false then we need a "colour map" to
           convert pixels to RGB.  If true, xxxMax and
           xxxShift specify bits used for red, green
           and blue */

  /* the following fields are only meaningful if trueColour is true */

  uint16_t redMax;    /* maximum red value (= 2^n - 1 where n is the
           number of bits used for red). Note this
           value is always in big endian order. */

  uint16_t greenMax;    /* similar for green */

  uint16_t blueMax;   /* and blue */

  uint8_t redShift;   /* number of shifts needed to get the red
           value in a pixel to the least significant
           bit. To find the red value from a given
           pixel, do the following:
           1) Swap pixel value according to bigEndian
              (e.g. if bigEndian is false and host byte
              order is big endian, then swap).
           2) Shift right by redShift.
           3) AND with redMax (in host byte order).
           4) You now have the red value between 0 and
              redMax. */

  uint8_t greenShift;   /* similar for green */

  uint8_t blueShift;    /* and blue */

  uint8_t pad1;
  uint16_t pad2;

} rfbPixelFormat;

#define sz_rfbPixelFormat 16

/* UltraVNC: Color settings values */
#define rfbPFFullColors   0
#define rfbPF256Colors     1
#define rfbPF64Colors     2
#define rfbPF8Colors       3
#define rfbPF8GreyColors  4
#define rfbPF4GreyColors  5
#define rfbPF2GreyColors  6


/*****************************************************************************

   Initial handshaking messages

 *****************************************************************************/

/*-----------------------------------------------------------------------------
   Protocol Version

   The server always sends 12 bytes to start which identifies the latest RFB
   protocol version number which it supports.  These bytes are interpreted
   as a string of 12 ASCII characters in the format "RFB xxx.yyy\n" where
   xxx and yyy are the major and minor version numbers (for version 3.3
   this is "RFB 003.003\n").

   The client then replies with a similar 12-byte message giving the version
   number of the protocol which should actually be used (which may be different
   to that quoted by the server).

   It is intended that both clients and servers may provide some level of
   backwards compatibility by this mechanism.  Servers in particular should
   attempt to provide backwards compatibility, and even forwards compatibility
   to some extent.  For example if a client demands version 3.1 of the
   protocol, a 3.0 server can probably assume that by ignoring requests for
   encoding types it doesn't understand, everything will still work OK.  This
   will probably not be the case for changes in the major version number.

   The format string below can be used in sprintf or sscanf to generate or
   decode the version string respectively.
*/

#define rfbProtocolVersionFormat "RFB %03d.%03d\n"
#define rfbProtocolMajorVersion 3
#define rfbProtocolMinorVersion 8
/* UltraVNC Viewer examines rfbProtocolMinorVersion number (4, and 6)
   to identify if the server supports File Transfer
*/

typedef char rfbProtocolVersionMsg[ 13 ]; /* allow extra byte for null */

#define sz_rfbProtocolVersionMsg 12

/*
   Negotiation of the security type (protocol version 3.7)

   Once the protocol version has been decided, the server either sends a list
   of supported security types, or informs the client about an error (when the
   number of security types is 0).  Security type rfbSecTypeTight is used to
   enable TightVNC-specific protocol extensions.  The value rfbSecTypeVncAuth
   stands for classic VNC authentication.

   The client selects a particular security type from the list provided by the
   server.
*/

#define rfbSecTypeInvalid 0
#define rfbSecTypeNone 1
#define rfbSecTypeVncAuth 2


/*-----------------------------------------------------------------------------
   Authentication

   Once the protocol version has been decided, the server then sends a 32-bit
   word indicating whether any authentication is needed on the connection.
   The value of this word determines the authentication scheme in use.  For
   version 3.0 of the protocol this may have one of the following values:
*/

#define rfbConnFailed 0
#define rfbNoAuth 1
#define rfbVncAuth 2

#define rfbRA2       5
#define rfbRA2ne     6
#define rfbSSPI      7
#define rfbSSPIne    8
#define rfbTight    16
#define rfbUltra    17
#define rfbTLS      18
#define rfbVeNCrypt 19
#define rfbSASL     20
#define rfbARD      30
#define rfbMSLogon 0xfffffffa

#define rfbVeNCryptPlain     256
#define rfbVeNCryptTLSNone   257
#define rfbVeNCryptTLSVNC    258
#define rfbVeNCryptTLSPlain  259
#define rfbVeNCryptX509None  260
#define rfbVeNCryptX509VNC   261
#define rfbVeNCryptX509Plain 262
#define rfbVeNCryptX509SASL  263
#define rfbVeNCryptTLSSASL   264

/*
   rfbConnFailed: For some reason the connection failed (e.g. the server
        cannot support the desired protocol version).  This is
        followed by a string describing the reason (where a
        string is specified as a 32-bit length followed by that
        many ASCII characters).

   rfbNoAuth:   No authentication is needed.

   rfbVncAuth:    The VNC authentication scheme is to be used.  A 16-byte
        challenge follows, which the client encrypts as
        appropriate using the password and sends the resulting
        16-byte response.  If the response is correct, the
        server sends the 32-bit word rfbVncAuthOK.  If a simple
        failure happens, the server sends rfbVncAuthFailed and
        closes the connection. If the server decides that too
        many failures have occurred, it sends rfbVncAuthTooMany
        and closes the connection.  In the latter case, the
        server should not allow an immediate reconnection by
        the client.
*/

#define rfbVncAuthOK      0
#define rfbVncAuthFailed  1
#define rfbVncAuthTooMany 2


/*-----------------------------------------------------------------------------
   Client Initialisation Message

   Once the client and server are sure that they're happy to talk to one
   another, the client sends an initialisation message.  At present this
   message only consists of a boolean indicating whether the server should try
   to share the desktop by leaving other clients connected, or give exclusive
   access to this client by disconnecting all other clients.
*/

typedef struct
{ uint8_t shared;
} rfbClientInitMsg;

#define sz_rfbClientInitMsg 1


/*-----------------------------------------------------------------------------
   Server Initialisation Message

   After the client initialisation message, the server sends one of its own.
   This tells the client the width and height of the server's framebuffer,
   its pixel format and the name associated with the desktop.
*/

typedef struct
{ uint16_t framebufferWidth;
  uint16_t framebufferHeight;
  rfbPixelFormat format;  /* the server's preferred pixel format */
  uint32_t nameLength;
  /* followed by char name[nameLength] */
} rfbServerInitMsg;

#define sz_rfbServerInitMsg (8 + sz_rfbPixelFormat)


/*
   Following the server initialisation message it's up to the client to send
   whichever protocol messages it wants.  Typically it will send a
   SetPixelFormat message and a SetEncodings message, followed by a
   FramebufferUpdateRequest.  From then on the server will send
   FramebufferUpdate messages in response to the client's
   FramebufferUpdateRequest messages.  The client should send
   FramebufferUpdateRequest messages with incremental set to true when it has
   finished processing one FramebufferUpdate and is ready to process another.
   With a fast client, the rate at which FramebufferUpdateRequests are sent
   should be regulated to avoid hogging the network.
*/



/*****************************************************************************

   Message types

 *****************************************************************************/

/* server -> client */

#define rfbFramebufferUpdate 0
#define rfbSetColourMapEntries 1
#define rfbBell 2
#define rfbServerCutText 3
/* Modif sf@2002 */
#define rfbResizeFrameBuffer 4
#define rfbPalmVNCReSizeFrameBuffer 0xF

/* client -> server */

#define rfbSetPixelFormat 0
#define rfbFixColourMapEntries 1  /* not currently supported */
#define rfbSetEncodings 2
#define rfbFramebufferUpdateRequest 3
#define rfbKeyEvent 4
#define rfbPointerEvent 5
#define rfbClientCutText 6  /* Modif sf@2002 - actually bidirectionnal */
#define rfbFileTransfer 7   /* Modif sf@2002 */
#define rfbSetScale 8       /* Modif rdv@2002 */
#define rfbSetServerInput 9 /* Modif rdv@2002 */
#define rfbSetSW  10        /* Modif sf@2002 - TextChat - Bidirectionnal */
#define rfbTextChat 11      /* Modif cs@2005 *//* PalmVNC 1.4 & 2.0 SetScale Factor message */
#define rfbPalmVNCSetScaleFactor 0xF
/* Xvp message - bidirectional */
#define rfbXvp 250
/* SetDesktopSize client -> server message */
#define rfbSetDesktopSize 251




/*****************************************************************************

   Encoding types

 *****************************************************************************/

#define rfbEncodingRaw      0
#define rfbEncodingCopyRect 1
#define rfbEncodingRRE      2
#define rfbEncodingCoRRE    4
#define rfbEncodingHextile  5
#define rfbEncodingZlib     6
#define rfbEncodingTight    7
#define rfbEncodingTightPng 0xFFFFFEFC /* -260 */
#define rfbEncodingZlibHex  8
#define rfbEncodingUltra    9
#define rfbEncodingTRLE    15
#define rfbEncodingZRLE    16
#define rfbEncodingZYWRLE  17

#define rfbEncodingH264    0x48323634

/* Cache & XOR-Zlib - rdv@2002 */
#define rfbEncodingCache                 0xFFFF0000
#define rfbEncodingCacheEnable           0xFFFF0001
#define rfbEncodingXOR_Zlib              0xFFFF0002
#define rfbEncodingXORMonoColor_Zlib     0xFFFF0003
#define rfbEncodingXORMultiColor_Zlib    0xFFFF0004
#define rfbEncodingSolidColor            0xFFFF0005
#define rfbEncodingXOREnable             0xFFFF0006
#define rfbEncodingCacheZip              0xFFFF0007
#define rfbEncodingSolMonoZip            0xFFFF0008
#define rfbEncodingUltraZip              0xFFFF0009

/* Xvp pseudo-encoding */
#define rfbEncodingXvp       0xFFFFFECB

/*
   Special encoding numbers:
     0xFFFFFD00 .. 0xFFFFFD05 -- subsampling level
     0xFFFFFE00 .. 0xFFFFFE64 -- fine-grained quality level (0-100 scale)
     0xFFFFFF00 .. 0xFFFFFF0F -- encoding-specific compression levels;
     0xFFFFFF10 .. 0xFFFFFF1F -- mouse cursor shape data;
     0xFFFFFF20 .. 0xFFFFFF2F -- various protocol extensions;
     0xFFFFFF30 .. 0xFFFFFFDF -- not allocated yet;
     0xFFFFFFE0 .. 0xFFFFFFEF -- quality level for JPEG compressor;
     0xFFFFFFF0 .. 0xFFFFFFFF -- cross-encoding compression levels.
*/

#define rfbEncodingFineQualityLevel0   0xFFFFFE00
#define rfbEncodingFineQualityLevel100 0xFFFFFE64
#define rfbEncodingSubsamp1X           0xFFFFFD00
#define rfbEncodingSubsamp4X           0xFFFFFD01
#define rfbEncodingSubsamp2X           0xFFFFFD02
#define rfbEncodingSubsampGray         0xFFFFFD03
#define rfbEncodingSubsamp8X           0xFFFFFD04
#define rfbEncodingSubsamp16X          0xFFFFFD05

#define rfbEncodingCompressLevel0  0xFFFFFF00
#define rfbEncodingCompressLevel1  0xFFFFFF01
#define rfbEncodingCompressLevel2  0xFFFFFF02
#define rfbEncodingCompressLevel3  0xFFFFFF03
#define rfbEncodingCompressLevel4  0xFFFFFF04
#define rfbEncodingCompressLevel5  0xFFFFFF05
#define rfbEncodingCompressLevel6  0xFFFFFF06
#define rfbEncodingCompressLevel7  0xFFFFFF07
#define rfbEncodingCompressLevel8  0xFFFFFF08
#define rfbEncodingCompressLevel9  0xFFFFFF09

#define rfbEncodingXCursor         0xFFFFFF10
#define rfbEncodingRichCursor      0xFFFFFF11
#define rfbEncodingPointerPos      0xFFFFFF18

#define rfbEncodingLastRect           0xFFFFFF20
#define rfbEncodingNewFBSize          0xFFFFFF21
#define rfbEncodingExtDesktopSize     0xFFFFFECC

#define rfbEncodingQualityLevel0   0xFFFFFFE0
#define rfbEncodingQualityLevel1   0xFFFFFFE1
#define rfbEncodingQualityLevel2   0xFFFFFFE2
#define rfbEncodingQualityLevel3   0xFFFFFFE3
#define rfbEncodingQualityLevel4   0xFFFFFFE4
#define rfbEncodingQualityLevel5   0xFFFFFFE5
#define rfbEncodingQualityLevel6   0xFFFFFFE6
#define rfbEncodingQualityLevel7   0xFFFFFFE7
#define rfbEncodingQualityLevel8   0xFFFFFFE8
#define rfbEncodingQualityLevel9   0xFFFFFFE9


/* LibVNCServer additions.   We claim 0xFFFE0000 - 0xFFFE00FF */
#define rfbEncodingKeyboardLedState   0xFFFE0000
#define rfbEncodingSupportedMessages  0xFFFE0001
#define rfbEncodingSupportedEncodings 0xFFFE0002
#define rfbEncodingServerIdentity     0xFFFE0003


/*****************************************************************************

   Server -> client message definitions

 *****************************************************************************/


/*-----------------------------------------------------------------------------
   FramebufferUpdate - a block of rectangles to be copied to the framebuffer.

   This message consists of a header giving the number of rectangles of pixel
   data followed by the rectangles themselves.  The header is padded so that
   together with the type byte it is an exact multiple of 4 bytes (to help
   with alignment of 32-bit pixels):
*/

typedef struct
{ uint8_t type;     /* always rfbFramebufferUpdate */
  uint8_t pad;
  uint16_t nRects;
  /* followed by nRects rectangles */
} rfbFramebufferUpdateMsg;

#define sz_rfbFramebufferUpdateMsg 4

/*
   Each rectangle of pixel data consists of a header describing the position
   and size of the rectangle and a type word describing the encoding of the
   pixel data, followed finally by the pixel data.  Note that if the client has
   not sent a SetEncodings message then it will only receive raw pixel data.
   Also note again that this structure is a multiple of 4 bytes.
*/

typedef struct
{ rfbRectangle r;
  uint32_t encoding;  /* one of the encoding types rfbEncoding... */
} rfbFramebufferUpdateRectHeader;

#define sz_rfbFramebufferUpdateRectHeader (sz_rfbRectangle + 4)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Supported Messages Encoding.  This encoding does not contain any pixel data.
   Instead, it contains 2 sets of bitflags.  These bitflags indicate what messages
   are supported by the server.
   rect->w contains byte count
*/

typedef struct
{ uint8_t client2server[32]; /* maximum of 256 message types (256/8)=32 */
  uint8_t server2client[32]; /* maximum of 256 message types (256/8)=32 */
} rfbSupportedMessages;

#define sz_rfbSupportedMessages 64

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Supported Encodings Encoding.  This encoding does not contain any pixel data.
   Instead, it contains a list of (uint32_t) Encodings supported by this server.
   rect->w contains byte count
   rect->h contains encoding count
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Server Identity Encoding.  This encoding does not contain any pixel data.
   Instead, it contains a text string containing information about the server.
   ie: "x11vnc: 0.8.1 lastmod: 2006-04-25 (libvncserver 0.9pre)\0"
   rect->w contains byte count
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Raw Encoding.  Pixels are sent in top-to-bottom scanline order,
   left-to-right within a scanline with no padding in between.
*/

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   KeyboardLedState Encoding.  The X coordinate contains the Locked Modifiers
   so that a remote troubleshooter can identify that the users 'Caps Lock'
   is set...   (It helps a *lot* when the users are untrained)
*/
#define rfbKeyboardMaskShift        1
#define rfbKeyboardMaskCapsLock     2
#define rfbKeyboardMaskControl      4
#define rfbKeyboardMaskAlt          8
#define rfbKeyboardMaskMeta        16
#define rfbKeyboardMaskSuper       32
#define rfbKeyboardMaskHyper       64
#define rfbKeyboardMaskNumLock    128
#define rfbKeyboardMaskScrollLock 256
#define rfbKeyboardMaskAltGraph   512

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   CopyRect Encoding.  The pixels are specified simply by the x and y position
   of the source rectangle.
*/

typedef struct
{ uint16_t srcX;
  uint16_t srcY;
} rfbCopyRect;

#define sz_rfbCopyRect 4


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   RRE - Rise-and-Run-length Encoding.  We have an rfbRREHeader structure
   giving the number of subrectangles following.  Finally the data follows in
   the form [<bgpixel><subrect><subrect>...] where each <subrect> is
   [<pixel><rfbRectangle>].
*/

typedef struct
{ uint32_t nSubrects;
} rfbRREHeader;

#define sz_rfbRREHeader 4


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   CoRRE - Compact RRE Encoding.  We have an rfbRREHeader structure giving
   the number of subrectangles following.  Finally the data follows in the form
   [<bgpixel><subrect><subrect>...] where each <subrect> is
   [<pixel><rfbCoRRERectangle>].  This means that
   the whole rectangle must be at most 255x255 pixels.
*/

typedef struct
{ uint8_t x;
  uint8_t y;
  uint8_t w;
  uint8_t h;
} rfbCoRRERectangle;

#define sz_rfbCoRRERectangle 4


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Hextile Encoding.  The rectangle is divided up into "tiles" of 16x16 pixels,
   starting at the top left going in left-to-right, top-to-bottom order.  If
   the width of the rectangle is not an exact multiple of 16 then the width of
   the last tile in each row will be correspondingly smaller.  Similarly if the
   height is not an exact multiple of 16 then the height of each tile in the
   final row will also be smaller.  Each tile begins with a "subencoding" type
   byte, which is a mask made up of a number of bits.  If the Raw bit is set
   then the other bits are irrelevant; w*h pixel values follow (where w and h
   are the width and height of the tile).  Otherwise the tile is encoded in a
   similar way to RRE, except that the position and size of each subrectangle
   can be specified in just two bytes.  The other bits in the mask are as
   follows:

   BackgroundSpecified - if set, a pixel value follows which specifies
      the background colour for this tile.  The first non-raw tile in a
      rectangle must have this bit set.  If this bit isn't set then the
      background is the same as the last tile.

   ForegroundSpecified - if set, a pixel value follows which specifies
      the foreground colour to be used for all subrectangles in this tile.
      If this bit is set then the SubrectsColoured bit must be zero.

   AnySubrects - if set, a single byte follows giving the number of
      subrectangles following.  If not set, there are no subrectangles (i.e.
      the whole tile is just solid background colour).

   SubrectsColoured - if set then each subrectangle is preceded by a pixel
      value giving the colour of that subrectangle.  If not set, all
      subrectangles are the same colour, the foreground colour;  if the
      ForegroundSpecified bit wasn't set then the foreground is the same as
      the last tile.

   The position and size of each subrectangle is specified in two bytes.  The
   Pack macros below can be used to generate the two bytes from x, y, w, h,
   and the Extract macros can be used to extract the x, y, w, h values from
   the two bytes.
*/

#define rfbHextileRaw     (1 << 0)
#define rfbHextileBackgroundSpecified (1 << 1)
#define rfbHextileForegroundSpecified (1 << 2)
#define rfbHextileAnySubrects   (1 << 3)
#define rfbHextileSubrectsColoured  (1 << 4)

#define rfbHextilePackXY(x,y) (((x) << 4) | (y))
#define rfbHextilePackWH(w,h) ((((w)-1) << 4) | ((h)-1))
#define rfbHextileExtractX(byte) ((byte) >> 4)
#define rfbHextileExtractY(byte) ((byte) & 0xf)
#define rfbHextileExtractW(byte) (((byte) >> 4) + 1)
#define rfbHextileExtractH(byte) (((byte) & 0xf) + 1)

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   zlib - zlib compressed Encoding.  We have an rfbZlibHeader structure
   giving the number of bytes following.  Finally the data follows is
   zlib compressed version of the raw pixel data as negotiated.
   (NOTE: also used by Ultra Encoding)
*/

typedef struct
{ uint32_t nBytes;
} rfbZlibHeader;

#define sz_rfbZlibHeader 4

#ifdef HAVE_LIBZ

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   Tight and TightPng Encoding.

  -- TightPng is like Tight but basic compression is not used, instead PNG
     data is sent.

  -- The first byte of each Tight-encoded rectangle is a "compression control
     byte". Its format is as follows (bit 0 is the least significant one):

     bit 0:    if 1, then compression stream 0 should be reset;
     bit 1:    if 1, then compression stream 1 should be reset;
     bit 2:    if 1, then compression stream 2 should be reset;
     bit 3:    if 1, then compression stream 3 should be reset;
     bits 7-4: if 1000 (0x08), then the compression type is "fill",
               if 1001 (0x09), then the compression type is "jpeg",
               (Tight only) if 1010 (0x0A), then the compression type is
                 "basic" and no Zlib compression was used,
               (Tight only) if 1110 (0x0E), then the compression type is
                 "basic", no Zlib compression was used, and a "filter id" byte
                 follows this byte,
               (TightPng only) if 1010 (0x0A), then the compression type is
                 "png",
               if 0xxx, then the compression type is "basic" and Zlib
                 compression was used,
               values greater than 1010 are not valid.

   If the compression type is "basic" and Zlib compression was used, then bits
   6..4 of the compression control byte (those xxx in 0xxx) specify the
   following:

     bits 5-4:  decimal representation is the index of a particular zlib
                stream which should be used for decompressing the data;
     bit 6:     if 1, then a "filter id" byte is following this byte.

  -- The data that follows after the compression control byte described
   above depends on the compression type ("fill", "jpeg", "png" or "basic").

  -- If the compression type is "fill", then the only pixel value follows, in
   client pixel format (see NOTE 1). This value applies to all pixels of the
   rectangle.

  -- If the compression type is "jpeg" or "png", the following data stream
   looks like this:

     1..3 bytes:  data size (N) in compact representation;
     N bytes:     JPEG or PNG image.

   Data size is compactly represented in one, two or three bytes, according
   to the following scheme:

    0xxxxxxx                    (for values 0..127)
    1xxxxxxx 0yyyyyyy           (for values 128..16383)
    1xxxxxxx 1yyyyyyy zzzzzzzz  (for values 16384..4194303)

   Here each character denotes one bit, xxxxxxx are the least significant 7
   bits of the value (bits 0-6), yyyyyyy are bits 7-13, and zzzzzzzz are the
   most significant 8 bits (bits 14-21). For example, decimal value 10000
   should be represented as two bytes: binary 10010000 01001110, or
   hexadecimal 90 4E.

  -- If the compression type is "basic" and bit 6 of the compression control
   byte was set to 1, then the next (second) byte specifies "filter id" which
   tells the decoder what filter type was used by the encoder to pre-process
   pixel data before the compression. The "filter id" byte can be one of the
   following:

     0:  no filter ("copy" filter);
     1:  "palette" filter;
     2:  "gradient" filter.

  -- If bit 6 of the compression control byte is set to 0 (no "filter id"
   byte), or if the filter id is 0, then raw pixel values in the client
   format (see NOTE 1) will be compressed. See below details on the
   compression.

  -- The "gradient" filter pre-processes pixel data with a simple algorithm
   which converts each color component to a difference between a "predicted"
   intensity and the actual intensity. Such a technique does not affect
   uncompressed data size, but helps to compress photo-like images better.
   Pseudo-code for converting intensities to differences is the following:

     P[i,j] := V[i-1,j] + V[i,j-1] - V[i-1,j-1];
     if (P[i,j] < 0) then P[i,j] := 0;
     if (P[i,j] > MAX) then P[i,j] := MAX;
     D[i,j] := V[i,j] - P[i,j];

   Here V[i,j] is the intensity of a color component for a pixel at
   coordinates (i,j). MAX is the maximum value of intensity for a color
   component.

  -- The "palette" filter converts true-color pixel data to indexed colors
   and a palette which can consist of 2..256 colors. If the number of colors
   is 2, then each pixel is encoded in 1 bit, otherwise 8 bits is used to
   encode one pixel. 1-bit encoding is performed such way that the most
   significant bits correspond to the leftmost pixels, and each raw of pixels
   is aligned to the byte boundary. When "palette" filter is used, the
   palette is sent before the pixel data. The palette begins with an unsigned
   byte which value is the number of colors in the palette minus 1 (i.e. 1
   means 2 colors, 255 means 256 colors in the palette). Then follows the
   palette itself which consist of pixel values in client pixel format (see
   NOTE 1).

  -- The pixel data is compressed using the zlib library. But if the data
   size after applying the filter but before the compression is less then 12,
   then the data is sent as is, uncompressed. Four separate zlib streams
   (0..3) can be used and the decoder should read the actual stream id from
   the compression control byte (see NOTE 2).

   If the compression is not used, then the pixel data is sent as is,
   otherwise the data stream looks like this:

     1..3 bytes:  data size (N) in compact representation;
     N bytes:     zlib-compressed data.

   Data size is compactly represented in one, two or three bytes, just like
   in the "jpeg" compression method (see above).

  -- NOTE 1. If the color depth is 24, and all three color components are
   8-bit wide, then one pixel in Tight encoding is always represented by
   three bytes, where the first byte is red component, the second byte is
   green component, and the third byte is blue component of the pixel color
   value. This applies to colors in palettes as well.

  -- NOTE 2. The decoder must reset compression streams' states before
   decoding the rectangle, if some of bits 0,1,2,3 in the compression control
   byte are set to 1. Note that the decoder must reset zlib streams even if
   the compression type is "fill", "jpeg" or "png".

  -- NOTE 3. The "gradient" filter and "jpeg" compression may be used only
   when bits-per-pixel value is either 16 or 32, not 8.

  -- NOTE 4. The width of any Tight-encoded rectangle cannot exceed 2048
   pixels. If a rectangle is wider, it must be split into several rectangles
   and each one should be encoded separately.

*/

#define rfbTightExplicitFilter         0x04
#define rfbTightFill                   0x08
#define rfbTightJpeg                   0x09
#define rfbTightNoZlib                 0x0A
#define rfbTightPng                    0x0A
#define rfbTightMaxSubencoding         0x0A

/* Filters to improve compression efficiency */
#define rfbTightFilterCopy             0x00
#define rfbTightFilterPalette          0x01
#define rfbTightFilterGradient         0x02

#endif

/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   XCursor encoding. This is a special encoding used to transmit X-style
   cursor shapes from server to clients. Note that for this encoding,
   coordinates in rfbFramebufferUpdateRectHeader structure hold hotspot
   position (r.x, r.y) and cursor size (r.w, r.h). If (w * h != 0), two RGB
   samples are sent after header in the rfbXCursorColors structure. They
   denote foreground and background colors of the cursor. If a client
   supports only black-and-white cursors, it should ignore these colors and
   assume that foreground is black and background is white. Next, two bitmaps
   (1 bits per pixel) follow: first one with actual data (value 0 denotes
   background color, value 1 denotes foreground color), second one with
   transparency data (bits with zero value mean that these pixels are
   transparent). Both bitmaps represent cursor data in a byte stream, from
   left to right, from top to bottom, and each row is byte-aligned. Most
   significant bits correspond to leftmost pixels. The number of bytes in
   each row can be calculated as ((w + 7) / 8). If (w * h == 0), cursor
   should be hidden (or default local cursor should be set by the client).
*/

typedef struct
{ uint8_t foreRed;
  uint8_t foreGreen;
  uint8_t foreBlue;
  uint8_t backRed;
  uint8_t backGreen;
  uint8_t backBlue;
} rfbXCursorColors;

#define sz_rfbXCursorColors 6


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   RichCursor encoding. This is a special encoding used to transmit cursor
   shapes from server to clients. It is similar to the XCursor encoding but
   uses client pixel format instead of two RGB colors to represent cursor
   image. For this encoding, coordinates in rfbFramebufferUpdateRectHeader
   structure hold hotspot position (r.x, r.y) and cursor size (r.w, r.h).
   After header, two pixmaps follow: first one with cursor image in current
   client pixel format (like in raw encoding), second with transparency data
   (1 bit per pixel, exactly the same format as used for transparency bitmap
   in the XCursor encoding). If (w * h == 0), cursor should be hidden (or
   default local cursor should be set by the client).
*/


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   ZRLE - encoding combining Zlib compression, tiling, palettisation and
   run-length encoding.
*/

typedef struct
{ uint32_t length;
} rfbZRLEHeader;

#define sz_rfbZRLEHeader 4

#define rfbZRLETileWidth 64
#define rfbZRLETileHeight 64


/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   ZLIBHEX - zlib compressed Hextile Encoding.  Essentially, this is the
   hextile encoding with zlib compression on the tiles that can not be
   efficiently encoded with one of the other hextile subencodings.  The
   new zlib subencoding uses two bytes to specify the length of the
   compressed tile and then the compressed data follows.  As with the
   raw sub-encoding, the zlib subencoding invalidates the other
   values, if they are also set.
*/

#define rfbHextileZlibRaw   (1 << 5)
#define rfbHextileZlibHex   (1 << 6)
#define rfbHextileZlibMono    (1 << 7)


/*-----------------------------------------------------------------------------
   SetColourMapEntries - these messages are only sent if the pixel
   format uses a "colour map" (i.e. trueColour false) and the client has not
   fixed the entire colour map using FixColourMapEntries.  In addition they
   will only start being sent after the client has sent its first
   FramebufferUpdateRequest.  So if the client always tells the server to use
   trueColour then it never needs to process this type of message.
*/

typedef struct
{ uint8_t type;     /* always rfbSetColourMapEntries */
  uint8_t pad;
  uint16_t firstColour;
  uint16_t nColours;

  /* Followed by nColours * 3 * uint16_t
     r1, g1, b1, r2, g2, b2, r3, g3, b3, ..., rn, bn, gn */

} rfbSetColourMapEntriesMsg;

#define sz_rfbSetColourMapEntriesMsg 6



/*-----------------------------------------------------------------------------
   Bell - ring a bell on the client if it has one.
*/

typedef struct
{ uint8_t type;     /* always rfbBell */
} rfbBellMsg;

#define sz_rfbBellMsg 1



/*-----------------------------------------------------------------------------
   ServerCutText - the server has new text in its cut buffer.
*/

typedef struct
{ uint8_t type;     /* always rfbServerCutText */
  uint8_t pad1;
  uint16_t pad2;
  uint32_t length;
  /* followed by char text[length] */
} rfbServerCutTextMsg;

#define sz_rfbServerCutTextMsg 8


/*-----------------------------------------------------------------------------
   //  Modif sf@2002
   FileTransferMsg - The client sends FileTransfer message.
   Bidirectional message - Files can be sent from client to server & vice versa
*/

typedef struct _rfbFileTransferMsg
{ uint8_t type;     /* always rfbFileTransfer */
  uint8_t contentType;  /*  See defines below */
  uint8_t contentParam;/*  Other possible content classification (Dir or File name, etc..) */
  uint8_t pad;         /* It appears that UltraVNC *forgot* to Swap16IfLE(contentParam) */
  uint32_t size;    /*  FileSize or packet index or error or other  */
  /*  uint32_t sizeH;    Additional 32Bits params to handle big values. Only for V2 (we want backward compatibility between all V1 versions) */
  uint32_t length;
  /* followed by data char text[length] */
} rfbFileTransferMsg;

#define sz_rfbFileTransferMsg 12

#define rfbFileTransferVersion  2 /*  v1 is the old FT version ( <= 1.0.0 RC18 versions) */

/*  FileTransfer Content types and Params defines
*/
#define rfbDirContentRequest    1 /*  Client asks for the content of a given Server directory */
#define rfbDirPacket              2 /*  Full directory name or full file name. */
/*  Null content means end of Directory */
#define rfbFileTransferRequest  3 /*  Client asks the server for the transfer of a given file */
#define rfbFileHeader            4 /*  First packet of a file transfer, containing file's features */
#define rfbFilePacket            5 /*  One chunk of the file */
#define rfbEndOfFile              6 /*  End of file transfer (the file has been received or error) */
#define rfbAbortFileTransfer    7 /*  The file transfer must be aborted, whatever the state */
#define rfbFileTransferOffer    8 /*  The client offers to send a file to the server */
#define rfbFileAcceptHeader     9 /*  The server accepts or rejects the file */
#define rfbCommand                10 /*  The Client sends a simple command (File Delete, Dir create etc...) */
#define rfbCommandReturn        11 /*  The Client receives the server's answer about a simple command */
#define rfbFileChecksums        12 /*  The zipped checksums of the destination file (Delta Transfer) */
#define rfbFileTransferAccess 14 /*  Request FileTransfer authorization */

/*  rfbDirContentRequest client Request - content params  */
#define rfbRDirContent      1 /*  Request a Server Directory contents */
#define rfbRDrivesList      2 /*  Request the server's drives list */
#define rfbRDirRecursiveList  3 /*  Request a server directory content recursive sorted list */
#define rfbRDirRecursiveSize  4 /*  Request a server directory content recursive size */

/*  rfbDirPacket & rfbCommandReturn  server Answer - content params */
#define rfbADirectory     1  /* Reception of a directory name */
#define rfbAFile            2  /* Reception of a file name  */
#define rfbADrivesList    3  /* Reception of a list of drives */
#define rfbADirCreate     4  /* Response to a create dir command  */
#define rfbADirDelete     5  /* Response to a delete dir command  */
#define rfbAFileCreate    6  /* Response to a create file command  */
#define rfbAFileDelete    7  /* Response to a delete file command  */
#define rfbAFileRename    8  /* Response to a rename file command  */
#define rfbADirRename     9  /* Response to a rename dir command  */
#define rfbADirRecursiveListItem  10
#define rfbADirRecursiveSize    11

/*  rfbCommand Command - content params */
#define rfbCDirCreate     1 /*  Request the server to create the given directory */
#define rfbCDirDelete     2 /*  Request the server to delete the given directory */
#define rfbCFileCreate    3 /*  Request the server to create the given file */
#define rfbCFileDelete    4 /*  Request the server to delete the given file */
#define rfbCFileRename    5 /*  Request the server to rename the given file  */
#define rfbCDirRename     6 /*  Request the server to rename the given directory */

/*  Errors - content params or "size" field */
#define rfbRErrorUnknownCmd     1  /*  Unknown FileTransfer command. */
#define rfbRErrorCmd      0xFFFFFFFF/*  Error when a command fails on remote side (ret in "size" field) */

#define sz_rfbBlockSize     8192  /*  Size of a File Transfer packet (before compression) */
#define rfbZipDirectoryPrefix   "!UVNCDIR-\0" /*  Transferred directory are zipped in a file with this prefix. Must end with "-" */
#define sz_rfbZipDirectoryPrefix 9
#define rfbDirPrefix      "[ "
#define rfbDirSuffix      " ]"



/*-----------------------------------------------------------------------------
   Modif sf@2002
   TextChatMsg - Utilized to order the TextChat mode on server or client
   Bidirectional message
*/

typedef struct _rfbTextChatMsg
{ uint8_t type;     /* always rfbTextChat */
  uint8_t pad1;         /*  Could be used later as an additionnal param */
  uint16_t pad2;    /*  Could be used later as text offset, for instance */
  uint32_t length;      /*  Specific values for Open, close, finished (-1, -2, -3) */
  /* followed by char text[length] */
} rfbTextChatMsg;

#define sz_rfbTextChatMsg 8

#define rfbTextMaxSize    4096
#define rfbTextChatOpen   0xFFFFFFFF
#define rfbTextChatClose  0xFFFFFFFE
#define rfbTextChatFinished 0xFFFFFFFD


/*-----------------------------------------------------------------------------
   Xvp Message
   Bidirectional message
   A server which supports the xvp extension declares this by sending a message
   with an Xvp_INIT xvp-message-code when it receives a request from the client
   to use the xvp Pseudo-encoding. The server must specify in this message the
   highest xvp-extension-version it supports: the client may assume that the
   server supports all versions from 1 up to this value. The client is then
   free to use any supported version. Currently, only version 1 is defined.

   A server which subsequently receives an xvp Client Message requesting an
   operation which it is unable to perform, informs the client of this by
   sending a message with an Xvp_FAIL xvp-message-code, and the same
   xvp-extension-version as included in the client's operation request.

   A client supporting the xvp extension sends this to request that the server
   initiate a clean shutdown, clean reboot or abrupt reset of the system whose
   framebuffer the client is displaying.
*/


typedef struct
{ uint8_t type;     /* always rfbXvp */
  uint8_t pad;
  uint8_t version;  /* xvp extension version */
  uint8_t code;       /* xvp message code */
} rfbXvpMsg;

#define sz_rfbXvpMsg (4)

/* server message codes */
#define rfbXvp_Fail 0
#define rfbXvp_Init 1
/* client message codes */
#define rfbXvp_Shutdown 2
#define rfbXvp_Reboot 3
#define rfbXvp_Reset 4

/*-----------------------------------------------------------------------------
   ExtendedDesktopSize server -> client message

   Informs the client of (re)size of framebuffer, provides information about
   physical screens attached, and lets the client knows it can request
   resolution changes using SetDesktopSize.
*/

typedef struct rfbExtDesktopSizeMsg
{ uint8_t numberOfScreens;
  uint8_t pad[3];

  /* Followed by rfbExtDesktopScreen[numberOfScreens] */
} rfbExtDesktopSizeMsg;

typedef struct rfbExtDesktopScreen
{ uint32_t id;
  uint16_t x;
  uint16_t y;
  uint16_t width;
  uint16_t height;
  uint32_t flags;
} rfbExtDesktopScreen;

#define sz_rfbExtDesktopSizeMsg (4)
#define sz_rfbExtDesktopScreen (16)

/* x - reason for the change */
#define rfbExtDesktopSize_GenericChange 0
#define rfbExtDesktopSize_ClientRequestedChange 1
#define rfbExtDesktopSize_OtherClientRequestedChange 2

/* y - status code for change */
#define rfbExtDesktopSize_Success 0
#define rfbExtDesktopSize_ResizeProhibited 1
#define rfbExtDesktopSize_OutOfResources 2
#define rfbExtDesktopSize_InvalidScreenLayout 3

/*-----------------------------------------------------------------------------
   SetDesktopSize client -> server message

   Allows the client to request that the framebuffer and physical screen
   resolutions are changed.
*/

typedef struct rfbSetDesktopSizeMsg
{ uint8_t type;                       /* always rfbSetDesktopSize */
  uint8_t pad1;
  uint16_t width;
  uint16_t height;
  uint8_t numberOfScreens;
  uint8_t pad2;

  /* Followed by rfbExtDesktopScreen[numberOfScreens] */
} rfbSetDesktopSizeMsg;

#define sz_rfbSetDesktopSizeMsg (8)


/*-----------------------------------------------------------------------------
   Modif sf@2002
   ResizeFrameBuffer - The Client must change the size of its framebuffer
*/

typedef struct _rfbResizeFrameBufferMsg
{ uint8_t type;     /* always rfbResizeFrameBuffer */
  uint8_t pad1;
  uint16_t framebufferWidth;  /*  FrameBuffer width */
  uint16_t framebufferHeigth; /*  FrameBuffer height */
} rfbResizeFrameBufferMsg;

#define sz_rfbResizeFrameBufferMsg 6


/*-----------------------------------------------------------------------------
   Copyright (C) 2001 Harakan Software
   PalmVNC 1.4 & 2.? ResizeFrameBuffer message
   ReSizeFrameBuffer - tell the RFB client to alter its framebuffer, either
   due to a resize of the server desktop or a client-requested scaling factor.
   The pixel format remains unchanged.
*/

typedef struct
{ uint8_t type;     /* always rfbReSizeFrameBuffer */
  uint8_t pad1;
  uint16_t desktop_w; /* Desktop width */
  uint16_t desktop_h; /* Desktop height */
  uint16_t buffer_w;  /* FrameBuffer width */
  uint16_t buffer_h;  /* Framebuffer height */
  uint16_t pad2;

} rfbPalmVNCReSizeFrameBufferMsg;

#define sz_rfbPalmVNCReSizeFrameBufferMsg (12)




/*-----------------------------------------------------------------------------
   Union of all server->client messages.
*/

typedef union
{ uint8_t type;
  rfbFramebufferUpdateMsg fu;
  rfbSetColourMapEntriesMsg scme;
  rfbBellMsg b;
  rfbServerCutTextMsg sct;
  rfbResizeFrameBufferMsg rsfb;
  rfbPalmVNCReSizeFrameBufferMsg prsfb;
  rfbFileTransferMsg ft;
  rfbTextChatMsg tc;
  rfbXvpMsg xvp;
  rfbExtDesktopSizeMsg eds;
} rfbServerToClientMsg;



/*- - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
   RDV Cache Encoding.
   special is not used at this point, can be used to reset cache or other specials
   just put it to make sure we don't have to change the encoding again.
*/

typedef struct
{ uint16_t special;
} rfbCacheRect;

#define sz_rfbCacheRect 2




/*****************************************************************************

   Message definitions (client -> server)

 *****************************************************************************/


/*-----------------------------------------------------------------------------
   SetPixelFormat - tell the RFB server the format in which the client wants
   pixels sent.
*/

typedef struct
{ uint8_t type;     /* always rfbSetPixelFormat */
  uint8_t pad1;
  uint16_t pad2;
  rfbPixelFormat format;
} rfbSetPixelFormatMsg;

#define sz_rfbSetPixelFormatMsg (sz_rfbPixelFormat + 4)


/*-----------------------------------------------------------------------------
   FixColourMapEntries - when the pixel format uses a "colour map", fix
   read-only colour map entries.

 *    ***************** NOT CURRENTLY SUPPORTED *****************
*/

typedef struct
{ uint8_t type;     /* always rfbFixColourMapEntries */
  uint8_t pad;
  uint16_t firstColour;
  uint16_t nColours;

  /* Followed by nColours * 3 * uint16_t
     r1, g1, b1, r2, g2, b2, r3, g3, b3, ..., rn, bn, gn */

} rfbFixColourMapEntriesMsg;

#define sz_rfbFixColourMapEntriesMsg 6


/*-----------------------------------------------------------------------------
   SetEncodings - tell the RFB server which encoding types we accept.  Put them
   in order of preference, if we have any.  We may always receive raw
   encoding, even if we don't specify it here.
*/

typedef struct
{ uint8_t type;     /* always rfbSetEncodings */
  uint8_t pad;
  uint16_t nEncodings;
  /* followed by nEncodings * uint32_t encoding types */
} rfbSetEncodingsMsg;

#define sz_rfbSetEncodingsMsg 4


/*-----------------------------------------------------------------------------
   FramebufferUpdateRequest - request for a framebuffer update.  If incremental
   is true then the client just wants the changes since the last update.  If
   false then it wants the whole of the specified rectangle.
*/

typedef struct
{ uint8_t type;     /* always rfbFramebufferUpdateRequest */
  uint8_t incremental;
  uint16_t x;
  uint16_t y;
  uint16_t w;
  uint16_t h;
} rfbFramebufferUpdateRequestMsg;

#define sz_rfbFramebufferUpdateRequestMsg 10


/*-----------------------------------------------------------------------------
   KeyEvent - key press or release

   Keys are specified using the "keysym" values defined by the X Window System.
   For most ordinary keys, the keysym is the same as the corresponding ASCII
   value.  Other common keys are:

   BackSpace        0xff08
   Tab              0xff09
   Return or Enter  0xff0d
   Escape           0xff1b
   Insert           0xff63
   Delete           0xffff
   Home     0xff50
   End      0xff57
   Page Up    0xff55
   Page Down    0xff56
   Left     0xff51
   Up     0xff52
   Right    0xff53
   Down     0xff54
   F1     0xffbe
   F2     0xffbf
   ...      ...
   F12      0xffc9
   Shift    0xffe1
   Control    0xffe3
   Meta     0xffe7
   Alt      0xffe9
*/

typedef struct
{ uint8_t type;     /* always rfbKeyEvent */
  uint8_t down;     /* true if down (press), false if up */
  uint16_t pad;
  uint32_t key;     /* key is specified as an X keysym */
} rfbKeyEventMsg;

#define sz_rfbKeyEventMsg 8


/*-----------------------------------------------------------------------------
   PointerEvent - mouse/pen move and/or button press.
*/

typedef struct
{ uint8_t type;     /* always rfbPointerEvent */
  uint8_t buttonMask;   /* bits 0-7 are buttons 1-8, 0=up, 1=down */
  uint16_t x;
  uint16_t y;
} rfbPointerEventMsg;

#define rfbButton1Mask 1
#define rfbButton2Mask 2
#define rfbButton3Mask 4
#define rfbButton4Mask 8
#define rfbButton5Mask 16
/* RealVNC 335 method */
#define rfbWheelUpMask   rfbButton4Mask
#define rfbWheelDownMask rfbButton5Mask

#define sz_rfbPointerEventMsg 6



/*-----------------------------------------------------------------------------
   ClientCutText - the client has new text in its cut buffer.
*/

typedef struct
{ uint8_t type;     /* always rfbClientCutText */
  uint8_t pad1;
  uint16_t pad2;
  uint32_t length;
  /* followed by char text[length] */
} rfbClientCutTextMsg;

#define sz_rfbClientCutTextMsg 8



/*-----------------------------------------------------------------------------
   sf@2002 - Set Server Scale
   SetServerScale - Server must change the scale of the client buffer.
*/

typedef struct _rfbSetScaleMsg
{ uint8_t type;     /* always rfbSetScale */
  uint8_t scale;    /* Scale value 1<sv<n */
  uint16_t pad;
} rfbSetScaleMsg;

#define sz_rfbSetScaleMsg 4


/*-----------------------------------------------------------------------------
   Copyright (C) 2001 Harakan Software
   PalmVNC 1.4 & 2.? SetScale Factor message
   SetScaleFactor - tell the RFB server to alter the scale factor for the
   client buffer.
*/
typedef struct
{ uint8_t type;     /* always rfbPalmVNCSetScaleFactor */

  uint8_t scale;    /* Scale factor (positive non-zero integer) */
  uint16_t pad2;
} rfbPalmVNCSetScaleFactorMsg;

#define sz_rfbPalmVNCSetScaleFactorMsg (4)


/*-----------------------------------------------------------------------------
   rdv@2002 - Set input status
   SetServerInput - Server input is dis/enabled
*/

typedef struct _rfbSetServerInputMsg
{ uint8_t type;     /* always rfbSetScale */
  uint8_t status;   /* Scale value 1<sv<n */
  uint16_t pad;
} rfbSetServerInputMsg;

#define sz_rfbSetServerInputMsg 4

/*-----------------------------------------------------------------------------
   rdv@2002 - Set SW
   SetSW - Server SW/full desktop
*/

typedef struct _rfbSetSWMsg
{ uint8_t type;     /* always rfbSetSW */
  uint8_t status;
  uint16_t x;
  uint16_t y;
} rfbSetSWMsg;

#define sz_rfbSetSWMsg 6



/*-----------------------------------------------------------------------------
   Union of all client->server messages.
*/

typedef union
{ uint8_t type;
  rfbSetPixelFormatMsg spf;
  rfbFixColourMapEntriesMsg fcme;
  rfbSetEncodingsMsg se;
  rfbFramebufferUpdateRequestMsg fur;
  rfbKeyEventMsg ke;
  rfbPointerEventMsg pe;
  rfbClientCutTextMsg cct;
  rfbSetScaleMsg ssc;
  rfbPalmVNCSetScaleFactorMsg pssf;
  rfbSetServerInputMsg sim;
  rfbFileTransferMsg ft;
  rfbSetSWMsg sw;
  rfbTextChatMsg tc;
  rfbXvpMsg xvp;
  rfbSetDesktopSizeMsg sdm;
} rfbClientToServerMsg;

/*
   vncauth.h - describes the functions provided by the vncauth library.
*/

#define MAXPWLEN 8
#define CHALLENGESIZE 16

extern int   rfbEncryptAndStorePasswd(char *passwd, char *fname);
extern char *rfbDecryptPasswdFromFile(char *fname);
extern void  rfbRandomBytes( unsigned char *bytes);
extern void  rfbEncryptBytes( unsigned char *bytes, char *passwd);



/* JACS, originally from rfb.h. isolate caller from called, ie. application from library
 */

#ifdef HAVE_SYS_TYPES_H
  #include <sys/types.h>
#endif


/* end of stuff for autoconf */


enum rfbNewClientAction
{ RFB_CLIENT_ACCEPT
, RFB_CLIENT_ON_HOLD
, RFB_CLIENT_REFUSE
};

typedef void   ( * rfbKbdAddEventProcPtr       ) ( rfbBool down, rfbKeySym keySym, struct _rfbClient* cl);
typedef void   ( * rfbKbdReleaseAllKeysProcPtr ) ( struct _rfbClient* cl);
typedef void   ( * rfbPtrAddEventProcPtr       ) ( int buttonMask, int x, int y, struct _rfbClient* cl);
typedef void   ( * rfbSetXCutTextProcPtr       ) ( char* str,int len, struct _rfbClient* cl);
typedef void   ( * rfbSetModifiedProcPtr       ) ( struct _rfbScreenInfo *,int x1,int y1,int x2,int y2 );


typedef struct rfbCursor* (*rfbGetCursorProcPtr) (struct _rfbClient* pScreen);

typedef enum rfbNewClientAction (*rfbNewClientHookPtr)(struct _rfbClient* cl);
typedef void    (*rfbDisplayHookPtr)(struct _rfbClient* cl);
typedef void    (*rfbDisplayFinishedHookPtr)(struct _rfbClient* cl, int result);
/** support the capability to view the caps/num/scroll states of the X server */
typedef int     (*rfbGetKeyboardLedStateHookPtr)(struct _rfbScreenInfo* screen);
typedef rfbBool (*rfbXvpHookPtr)(struct _rfbClient* cl, uint8_t, uint8_t);
typedef int     (*rfbSetDesktopSizeHookPtr)(int width, int height, int numScreens, struct rfbExtDesktopScreen* extDesktopScreens, struct _rfbClient* cl);
typedef int     (*rfbNumberOfExtDesktopScreensPtr)(struct _rfbClient* cl);
typedef rfbBool (*rfbGetExtDesktopScreenPtr)(int seqnumber, struct rfbExtDesktopScreen *extDesktopScreen, struct _rfbClient* cl);
/**
 * If x==1 and y==1 then set the whole display
 * else find the window underneath x and y and set the framebuffer to the dimensions
 * of that window
 */
typedef void (*rfbSetSingleWindowProcPtr) (struct _rfbClient* cl, int x, int y);
/**
 * Status determines if the X11 server permits input from the local user
 * status==0 or 1
 */
typedef void (*rfbSetServerInputProcPtr) (struct _rfbClient* cl, int status);
/**
 * Permit the server to allow or deny filetransfers.   This is defaulted to deny
 * It is called when a client initiates a connection to determine if it is permitted.
 */
typedef int  (*rfbFileTransferPermitted) (struct _rfbClient* cl);
/** Handle the textchat messages */
typedef void (*rfbSetTextChat) (struct _rfbClient* cl, int length, char *string);

typedef struct
{ uint32_t count;
  rfbBool is16; /**< is the data format short? */
  union {
    uint8_t* bytes;
    uint16_t* shorts;
  } data; /**< there have to be count*3 entries */
} rfbColourMap;

/**
 * Security handling (RFB protocol version 3.7)
 */

typedef struct _rfbSecurity
{ uint8_t type;
	 void (*handler)(struct _rfbClient* cl);
	 struct _rfbSecurity* next;
} rfbSecurityHandler;

/**
 * Protocol extension handling.
 */

typedef struct _rfbProtocolExtension
{ rfbBool ( *newClient)( struct _rfbClient* client, void** data); /** returns FALSE if extension should be deactivated for client.   if newClient == NULL, it is always deactivated. */
 	rfbBool ( *init     )( struct _rfbClient* client, void*  data); /** returns FALSE if extension should be deactivated for client. if init == NULL, it stays activated. */

	int *pseudoEncodings; 	/** if pseudoEncodings is not NULL, it contains a 0 terminated  list of the pseudo encodings handled by this extension. */

	rfbBool (*enablePseudoEncoding)(struct _rfbClient* client,	void** data, int encodingNumber);                           	/** returns TRUE if that pseudo encoding is handled by the extension.  encodingNumber==0 means "reset encodings". */
	/** returns TRUE if message was handled */
	rfbBool (*handleMessage)(struct _rfbClient* client,				void* data,		const rfbClientToServerMsg* message);
	void (*close)(struct _rfbClient* client, void* data);
	void (*usage)(void);
	/** processArguments returns the number of handled arguments */
	int (*processArgument)(int argc, char *argv[]);
	struct _rfbProtocolExtension* next;
} rfbProtocolExtension;

typedef struct _rfbExtensionData
{ rfbProtocolExtension* extension;
	 void* data;
	 struct _rfbExtensionData* next;
} rfbExtensionData;

/**
 * Per-screen (framebuffer) structure.  There can be as many as you wish,
 * each serving different clients. However, you have to call
 * rfbProcessEvents for each of these.
 */

typedef struct _rfbScreenInfo
{ struct _rfbScreenInfo *scaledScreenNext;  /** this structure has children that are scaled versions of this screen */
  int scaledScreenRefCount;

  int width;
  int paddedWidthInBytes;
  int height;
  int depth;
  int bitsPerPixel;
 //   int sizeInBytes;

  rfbPixel blackPixel;
  rfbPixel whitePixel;

    /**  JACS, async stuff
      */
 // int (*streamPusher)( int sk, void */*StackFun */, void * userData
   //                  , const void * src, size_t sz );

  VncPushFun streamPusher;


/**
  *  some screen specific data can be put into a struct where screenData points to.
  * You need this if you have more than one screen at the
  * same time while using the same functions.
  */
  void * screenData;

    /* additions by libvncserver */

  rfbPixelFormat serverFormat;
  rfbColourMap colourMap; /**< set this if rfbServerFormat.trueColour==FALSE */
  const char* desktopName;
  char thisHost[255];

//  rfbBool autoPort;
//  rfbBool inetdInitDone;

//  struct _rfbClient* udpClient;
  rfbBool udpSockConnected;

  int maxClientWait;

    /* http stuff */
 //   rfbBool httpInitDone;
//    rfbBool httpEnableProxyConnect;

  rfbPasswordCheckProcPtr passwordCheck;
  void* authPasswdData;


  int authPasswdFirstViewOnly; /** If rfbAuthPasswdData is given a list, this is the first view only password. */


  int maxRectsPerUpdate;     /** send only this many rectangles in one update */
    /** this is the amount of milliseconds to wait at least before sending
     * an update. */
  int deferUpdateTime;
#ifdef TODELETE
    char* screen;
#endif
  rfbBool alwaysShared;
  rfbBool neverShared;
  rfbBool dontDisconnect;
  struct _rfbClient * clientHead;
  struct _rfbClient * pointerClient;  /**< "Mutex" for pointer events */


    /* cursor */
  int cursorX, cursorY,underCursorBufferLen;
  char* underCursorBuffer;
  rfbBool dontConvertRichCursorToXCursor;
  struct rfbCursor* cursor;

    /**
     * the frameBuffer has to be supplied by the serving process.
     * The buffer will not be freed by
     */
  char* frameBuffer;

  rfbKbdAddEventProcPtr          kbdAddEvent;
  rfbKbdReleaseAllKeysProcPtr    kbdReleaseAllKeys;
  rfbPtrAddEventProcPtr          ptrAddEvent;
  rfbSetXCutTextProcPtr          setXCutText;
  rfbGetCursorProcPtr            getCursorPtr;

//  rfbSetModifiedProcPtr          setModified;

  rfbSetTranslateFunctionProcPtr setTranslateFunction;
  rfbSetSingleWindowProcPtr      setSingleWindow;
  rfbSetServerInputProcPtr       setServerInput;
  rfbFileTransferPermitted       getFileTransferPermission;
  rfbSetTextChat                 setTextChat;


  rfbNewClientHookPtr newClientHook;     /** newClientHook is called just after a new client is created */
  rfbDisplayHookPtr displayHook;     /** displayHook is called just before a frame buffer update */
  rfbGetKeyboardLedStateHookPtr getKeyboardLedStateHook;     /** These hooks are called to pass keyboard state back to the client */
  rfbBool ignoreSIGPIPE;     /** if TRUE, an ignoring signal handler is installed for SIGPIPE */

    /** if not zero, only a slice of this height is processed every time
     * an update should be sent. This should make working on a slow
     * link more interactive. */
  int progressiveSliceHeight;

//    in_addr_t listenInterface;    JACS
  int deferPtrUpdateTime;

  rfbBool handleEventsEagerly; /** handle as many input events as possible (default off) */
  char *versionString;     /** rfbEncodingServerIdentity */

    /** What does the server tell the new clients which version it supports */
  int protocolMajorVersion;
  int protocolMinorVersion;

    /** command line authorization of file transfers */
  rfbBool permitFileTransfer;

    /** displayFinishedHook is called just after a frame buffer update */
  rfbDisplayFinishedHookPtr displayFinishedHook;
    /** xvpHook is called to handle an xvp client message */
  rfbXvpHookPtr xvpHook;
  char *sslkeyfile;
  char *sslcertfile;
// JACS    int ipv6port; /**< The port to listen on when using IPv6.  */
// JACS    char* listen6Interface;
    /* We have an additional IPv6 listen socket since there are systems that
       don't support dual binding sockets under *any* circumstances, for
       instance OpenBSD */
    /** hook to let client set resolution */
  rfbSetDesktopSizeHookPtr setDesktopSizeHook;
    /** Optional hooks to query ExtendedDesktopSize screen information.
     * If not set it is assumed only one screen is present spanning entire fb */
  rfbNumberOfExtDesktopScreensPtr numberOfExtDesktopScreensHook;
  rfbGetExtDesktopScreenPtr getExtDesktopScreenHook;

/** This value between 0 and 1.0 defines which fraction of the maximum number
	* of file descriptors LibVNCServer uses before denying new client connections.
	* It is set to 0.5 per default.
 */
  float fdQuota;

}  rfbScreenInfo
, *rfbScreenInfoPtr;


/**
 * rfbTranslateFnType is the type of translation functions.
 */

typedef void (*rfbTranslateFnType)( char *table, rfbPixelFormat *in
                                  , rfbPixelFormat *out
                                  , char *iptr, char *optr
                                  , int bytesBetweenInputLines
                                  , int width, int height);


/* region stuff */

struct sraRegion;
typedef struct sraRegion* sraRegionPtr;

/*
 * Per-client structure.
 */

typedef void (*ClientGoneHookPtr)(struct _rfbClient* cl);
typedef void (*ClientFramebufferUpdateRequestHookPtr)(struct _rfbClient* cl, rfbFramebufferUpdateRequestMsg* furMsg);

typedef struct _rfbFileTransferData
{ int fd;
  int compressionEnabled;
  int fileSize;
  int numPackets;
  int receiving;
  int sending;
} rfbFileTransferData;


typedef struct _rfbStatList
{ uint32_t type;
  uint32_t sentCount;
  uint32_t bytesSent;
  uint32_t bytesSentIfRaw;
  uint32_t rcvdCount;
  uint32_t bytesRcvd;
  uint32_t bytesRcvdIfRaw;
  struct _rfbStatList *Next;
} rfbStatList;

typedef struct _rfbSslCtx rfbSslCtx;
typedef struct _wsCtx wsCtx;



typedef struct _rfbClient
{ rfbScreenInfoPtr screen;     /** back pointer to the screen */


// JACS, async versio
  int fd;
  char * recvPtr; int bytesLeft;   /* JACS, jun 2017, async reads */

  rfbScreenInfoPtr scaledScreen; /** points to a scaled version of the screen buffer in cl->scaledScreenList */
  rfbBool PalmVNC; /** how did the client tell us it wanted the screen changed?  Ultra style or palm style? */


    /** private data. You should put any application client specific data
     * into a struct and let clientData point to it. Don't forget to
     * free the struct via clientGoneHook!
     *
     * This is useful if the IO functions have to behave client specific.
     */
  void   * clientData;
  void ( * clientCode )( void * args, int butt, int x, int y );

  ClientGoneHookPtr clientGoneHook;


  int protocolMajorVersion;   /* RFB protocol minor version number */
  int protocolMinorVersion;


/**   Note that the RFB_INITIALISATION_SHARED state is provided to support
 *  clients that under some circumstances do not send a ClientInit message.
 *  In particular the Mac OS X built-in VNC client (with protocolMinorVersion
 *  == 889) is one of those.  However, it only requires this support under
 *  special circumstances that can only be determined during the initial
 *  authentication.  If the right conditions are met this state will be
 *  set (see the auth.c file) when rfbProcessClientInitMessage is called.
 *
 *  If the state is RFB_INITIALISATION_SHARED we should not expect to receive
 *  any ClientInit message, but instead should proceed to the next stage
 *  of initialisation as though an implicit ClientInit message was received
 *  with a shared-flag of true.  (There is currently no corresponding
 *  RFB_INITIALISATION_NOTSHARED state to represent an implicit ClientInit
 *  message with a shared-flag of false because no known existing client
 *  requires such support at this time.)
 *
 *  Note that software using LibVNCServer to provide a VNC server will only
 *  ever have a chance to see the state field set to
 *  RFB_INITIALISATION_SHARED if the software is multi-threaded and manages
 *  to examine the state field during the extremely brief window after the
 *  'None' authentication type selection has been received from the built-in
 *  OS X VNC client and before the rfbProcessClientInitMessage function is
 *  called -- control cannot return to the caller during this brief window
 *  while the state field is set to RFB_INITIALISATION_SHARED.
 */

                                /** Possible client states: */
  enum
  { RFB_PROTOCOL_VERSION,   /**< establishing protocol version */
   	RFB_SECURITY_TYPE,      /**< negotiating security (RFB v.3.7) */
    RFB_AUTHENTICATION,     /**< authenticating */
    RFB_INITIALISATION,     /**< sending initialisation messages */
    RFB_NORMAL,             /**< normal protocol messages */

        /* Ephemeral internal-use states that will never be seen by software
         * using LibVNCServer to provide services: */
    RFB_INITIALISATION_SHARED /**< sending initialisation messages with implicit shared-flag already true */
  } state;

  rfbBool reverseConnection;
  rfbBool onHold;
  rfbBool readyForSetColourMapEntries;
  rfbBool useCopyRect;
  int preferredEncoding;
  int correMaxWidth, correMaxHeight;

  rfbBool viewOnly;

    /* The following member is only used during VNC authentication */
  uint8_t authChallenge[CHALLENGESIZE];

    /* The following members represent the update needed to get the client's
       framebuffer from its present state to the current state of our
       framebuffer.

       If the client does not accept CopyRect encoding then the update is
       simply represented as the region of the screen which has been modified
       (modifiedRegion).

       If the client does accept CopyRect encoding, then the update consists of
       two parts.  First we have a single copy from one region of the screen to
       another (the destination of the copy is copyRegion), and second we have
       the region of the screen which has been modified in some other way
       (modifiedRegion).

       Although the copy is of a single region, this region may have many
       rectangles.  When sending an update, the copyRegion is always sent
       before the modifiedRegion.  This is because the modifiedRegion may
       overlap parts of the screen which are in the source of the copy.

       In fact during normal processing, the modifiedRegion may even overlap
       the destination copyRegion.  Just before an update is sent we remove
       from the copyRegion anything in the modifiedRegion. */

    sraRegionPtr copyRegion;	/**< the destination region of the copy */
    int copyDX, copyDY;		/**< the translation by which the copy happens */

    sraRegionPtr modifiedRegion;

    /** As part of the FramebufferUpdateRequest, a client can express interest
       in a subrectangle of the whole framebuffer.  This is stored in the
       requestedRegion member.  In the normal case this is the whole
       framebuffer if the client is ready, empty if it's not. */

    sraRegionPtr requestedRegion;

    /** The following member represents the state of the "deferred update" timer
       - when the framebuffer is modified and the client is ready, in most
       cases it is more efficient to defer sending the update by a few
       milliseconds so that several changes to the framebuffer can be combined
       into a single update. */

      struct timeval startDeferring;
      struct timeval startPtrDeferring;

      int lastPtrX;
      int lastPtrY;
      int lastPtrButtons;

    /** translateFn points to the translation function which is used to copy
       and translate a rectangle from the framebuffer to an output buffer. */

    rfbTranslateFnType translateFn;
    char *translateLookupTable;
    rfbPixelFormat format;

    /**
     * UPDATE_BUF_SIZE must be big enough to send at least one whole line of the
     * framebuffer.  So for a max screen width of say 2K with 32-bit pixels this
     * means 8K minimum.
     */

#define UPDATE_BUF_SIZE 30000

    char updateBuf[UPDATE_BUF_SIZE];
    int ublen;

    /* statistics */
    struct _rfbStatList *statEncList;
    struct _rfbStatList *statMsgList;
    int rawBytesEquivalent;
    int bytesSent;

#ifdef HAVE_LIBZ
    /* zlib encoding -- necessary compression state info per client */

    struct z_stream_s compStream;
    rfbBool compStreamInited;
    uint32_t zlibCompressLevel;
#endif

#if defined(HAVE_LIBZ) || defined(HAVE_LIBPNG)
    /** the quality level is also used by ZYWRLE and TightPng */
    int tightQualityLevel;

#ifdef HAVE_LIBJPEG
    /* tight encoding -- preserve zlib streams' state for each client */

    z_stream zsStruct[4];
    rfbBool zsActive[4];
    int zsLevel[4];
    int tightCompressLevel;

#endif

#endif

    /* Ultra Encoding support */
    rfbBool compStreamInitedLZO;
    char *lzoWrkMem;

    rfbFileTransferData fileTransfer;

    int     lastKeyboardLedState;     /**< keep track of last value so we can send *change* events */
    rfbBool enableSupportedMessages;  /**< client supports SupportedMessages encoding */
    rfbBool enableSupportedEncodings; /**< client supports SupportedEncodings encoding */
    rfbBool enableServerIdentity;     /**< client supports ServerIdentity encoding */
    rfbBool enableKeyboardLedState;   /**< client supports KeyboardState encoding */
    rfbBool enableLastRectEncoding;   /**< client supports LastRect encoding */
    rfbBool enableCursorShapeUpdates; /**< client supports cursor shape updates */
    rfbBool enableCursorPosUpdates;   /**< client supports cursor position updates */
    rfbBool useRichCursorEncoding;    /**< rfbEncodingRichCursor is preferred */
    rfbBool cursorWasChanged;         /**< cursor shape update should be sent */
    rfbBool cursorWasMoved;           /**< cursor position update should be sent */
    int cursorX,cursorY;	             /**< the coordinates of the cursor,	 if enableCursorShapeUpdates = FALSE */

    rfbBool useNewFBSize;             /**< client supports NewFBSize encoding */
    rfbBool newFBSizePending;         /**< framebuffer size was changed */

    struct _rfbClient *prev;
    struct _rfbClient *next;


#ifdef HAVE_LIBZ
    void* zrleData;
    int zywrleLevel;
    int zywrleBuf[rfbZRLETileWidth * rfbZRLETileHeight];
#endif

    /** if progressive updating is on, this variable holds the current
     * y coordinate of the progressive slice. */
    int progressiveSliceY;

    rfbExtensionData* extensions;

    /** for threaded zrle */
    char *zrleBeforeBuf;
    void *paletteHelper;

    /** for thread safety for rfbSendFBUpdate() */

  /* buffers to hold pixel data before and after encoding.
     per-client for thread safety */
  char *beforeEncBuf;
  int beforeEncBufSize;
  char *afterEncBuf;
  int afterEncBufSize;
  int afterEncBufLen;
#if defined(HAVE_LIBZ) || defined(HAVE_LIBPNG)
    uint32_t tightEncoding;  /* rfbEncodingTight or rfbEncodingTightPng */
  #ifdef HAVE_LIBJPEG /* TurboVNC Encoding support (extends TightVNC) */
    int turboSubsampLevel;
    int turboQualityLevel;  /* 1-100 scale */
  #endif
#endif
    rfbSslCtx *sslctx;
    wsCtx     *wsctx;
    char *wspath;                          /* Requests path component */

    /**
     * clientFramebufferUpdateRequestHook is called when a client requests a frame
     * buffer update.
     */
    ClientFramebufferUpdateRequestHookPtr clientFramebufferUpdateRequestHook;

    rfbBool useExtDesktopSize;
    int requestedDesktopSizeChange;
    int lastDesktopSizeChangeError;
} rfbClient;

/**
 * This macro is used to test whether there is a framebuffer update needing to
 * be sent to the client.
 */

#define FB_UPDATE_PENDING(cl)                                              \
     (((cl)->enableCursorShapeUpdates && (cl)->cursorWasChanged) ||        \
     (((cl)->enableCursorShapeUpdates == FALSE &&                          \
       ((cl)->cursorX != (cl)->screen->cursorX ||                          \
	(cl)->cursorY != (cl)->screen->cursorY))) ||                       \
     ((cl)->useNewFBSize && (cl)->newFBSizePending) ||                     \
     ((cl)->enableCursorPosUpdates && (cl)->cursorWasMoved) ||             \
     !sraRgnEmpty((cl)->copyRegion) || !sraRgnEmpty((cl)->modifiedRegion))

/*
 * Macros for endian swapping.
 */

#define Swap16(s) ((((s) & 0xff) << 8) | (((s) >> 8) & 0xff))

#define Swap24(l) ((((l) & 0xff) << 16) | (((l) >> 16) & 0xff) | \
                   (((l) & 0x00ff00)))

#define Swap32(l) ((((l) >> 24) & 0x000000ff)| \
                   (((l) & 0x00ff0000) >> 8)  | \
                   (((l) & 0x0000ff00) << 8)  | \
                   (((l) & 0x000000ff) << 24))


extern char rfbEndianTest;

#define Swap16IfLE(s) (rfbEndianTest ? Swap16(s) : (s))
#define Swap24IfLE(l) (rfbEndianTest ? Swap24(l) : (l))
#define Swap32IfLE(l) (rfbEndianTest ? Swap32(l) : (l))

/* UltraVNC uses some windows structures unmodified, so the viewer expects LittleEndian Data */
#define Swap16IfBE(s) (rfbEndianTest ? (s) : Swap16(s))
#define Swap24IfBE(l) (rfbEndianTest ? (l) : Swap24(l))
#define Swap32IfBE(l) (rfbEndianTest ? (l) : Swap32(l))

int rfbPushClientStream( rfbClient * cl, const void * data, size_t sz );  // JACS, client data sourcer


/* rfbserver.c */

/* Routines to iterate over the client list in a thread-safe way.
   Only a single iterator can be in use at a time process-wide. */
typedef struct rfbClientIterator *rfbClientIteratorPtr;

extern void rfbClientListInit(rfbScreenInfoPtr rfbScreen);
extern rfbClientIteratorPtr rfbGetClientIterator(rfbScreenInfoPtr rfbScreen);
extern rfbClient * rfbClientIteratorNext(rfbClientIteratorPtr iterator);
extern void rfbReleaseClientIterator(rfbClientIteratorPtr iterator);
extern void rfbIncrClientRef(rfbClient * cl);
extern void rfbDecrClientRef(rfbClient * cl);

extern void rfbNewClientConnection(rfbScreenInfoPtr rfbScreen,int sock);

extern char *rfbProcessFileTransferReadBuffer(rfbClient * cl, uint32_t length);
extern char *rfbProcessFileTransferReadBuffer(rfbClient * cl, uint32_t length);


extern void * getStreamBytes(              rfbClient *, size_t sz );
extern void    rfbClientConnFailed(        rfbClient *, const char *reason);
extern rfbBool rfbSendFramebufferUpdate(   rfbClient *, sraRegionPtr updateRegion);
extern rfbBool rfbSendRectEncodingRaw(     rfbClient *, int x,int y,int w,int h);
extern rfbBool rfbSendUpdateBuf(           rfbClient *);
extern rfbBool rfbSendCopyRegion(          rfbClient *,sraRegionPtr reg,int dx,int dy);
extern rfbBool rfbSendLastRectMarker(      rfbClient *);
extern rfbBool rfbSendNewFBSize(           rfbClient *, int w, int h);
extern rfbBool rfbSendExtDesktopSize(      rfbClient *, int w, int h);
extern rfbBool rfbSendSetColourMapEntries( rfbClient *, int firstColour, int nColours);
extern rfbBool rfbSendFileTransferChunk(   rfbClient * cl);
extern rfbBool rfbSendDirContent(          rfbClient * cl, int length, char *buffer);
extern rfbBool rfbSendFileTransferMessage( rfbClient * cl, uint8_t contentType, uint8_t contentParam, uint32_t size, uint32_t length, const char *buffer);
extern rfbBool rfbProcessFileTransfer(     rfbClient * cl, uint8_t contentType, uint8_t contentParam, uint32_t size, uint32_t length);

extern void rfbSendBell(          rfbScreenInfoPtr );
extern void rfbProcessUDPInput(   rfbScreenInfoPtr );
extern void rfbSendServerCutText( rfbScreenInfoPtr ,char *str, int len);
       void rfbGotXCutText(       rfbScreenInfoPtr , char *str, int len);



/* translate.c */

extern rfbBool rfbEconomicTranslate;

extern void rfbTranslateNone(char *table, rfbPixelFormat *in,
                             rfbPixelFormat *out,
                             char *iptr, char *optr,
                             int bytesBetweenInputLines,
                             int width, int height);
extern rfbBool rfbSetTranslateFunction(rfbClient * cl);
extern rfbBool rfbSetClientColourMap(rfbClient * cl, int firstColour, int nColours);
extern void rfbSetClientColourMaps(rfbScreenInfoPtr rfbScreen, int firstColour, int nColours);



/* auth.c */

extern void rfbAuthNewClient(rfbClient * cl);
extern void rfbProcessClientSecurityType(rfbClient * cl);
extern void rfbAuthProcessClientMessage(rfbClient * cl);
extern void rfbRegisterSecurityHandler(   rfbSecurityHandler* );
extern void rfbUnregisterSecurityHandler( rfbSecurityHandler* );

/* rre.c */

extern rfbBool rfbSendRectEncodingRRE(rfbClient * cl, int x,int y,int w,int h);


/* corre.c */

extern rfbBool rfbSendRectEncodingCoRRE(rfbClient * cl, int x,int y,int w,int h);


/* hextile.c */

extern rfbBool rfbSendRectEncodingHextile(rfbClient * cl, int x, int y, int w, int h);

/* ultra.c */

/* Set maximum ultra rectangle size in pixels.  Always allow at least
 * two scan lines.
 */
#define ULTRA_MAX_RECT_SIZE (128*256)
#define ULTRA_MAX_SIZE(min) ((( min * 2 ) > ULTRA_MAX_RECT_SIZE ) ? \
                            ( min * 2 ) : ULTRA_MAX_RECT_SIZE )

extern rfbBool rfbSendRectEncodingUltra(rfbClient * cl, int x,int y,int w,int h);

#ifdef HAVE_LIBZ /* zlib.c */

/** Minimum zlib rectangle size in bytes.  Anything smaller will
 * not compress well due to overhead.
 */
#define VNC_ENCODE_ZLIB_MIN_COMP_SIZE ( 17 )

/* Set maximum zlib rectangle size in pixels.  Always allow at least
 * two scan lines.
 */
#define ZLIB_MAX_RECT_SIZE (128*256)
#define ZLIB_MAX_SIZE(min) ((( min * 2 ) > ZLIB_MAX_RECT_SIZE ) ? \
			    ( min * 2 ) : ZLIB_MAX_RECT_SIZE )

extern rfbBool rfbSendRectEncodingZlib(rfbClient * cl, int x, int y, int w,
				    int h);

#ifdef HAVE_LIBJPEG /* tight.c */

#define TIGHT_DEFAULT_COMPRESSION  6
#define TURBO_DEFAULT_SUBSAMP 0

extern rfbBool rfbTightDisableGradient;
extern int rfbNumCodedRectsTight(rfbClient * cl, int x,int y,int w,int h);

 rfbBool rfbSendRectEncodingTight(rfbClient * cl, int x,int y,int w,int h);
 rfbBool rfbSendTightHeader(rfbClient * cl, int x, int y, int w, int h);
 rfbBool rfbSendCompressedDataTight( rfbClient * cl, char *buf , int compressedLen);

  #if defined(HAVE_LIBPNG)
     rfbBool rfbSendRectEncodingTightPng(rfbClient * cl, int x,int y,int w,int h);
  #endif

#endif
#endif

/* cursor.c */



typedef struct rfbCursor
{ rfbBool cleanup, cleanupSource, cleanupMask, cleanupRichSource; /** set this to true if LibVNCServer has to free this cursor */
  unsigned char *source;	               /**< points to bits */
  unsigned char *mask;			                      /**< points to bits */
  unsigned short width, height, xhot, yhot;    /**< metrics */
  unsigned short foreRed, foreGreen, foreBlue; /**< device-independent colour */
  unsigned short backRed, backGreen, backBlue; /**< device-independent colour */
  unsigned char *richSource;                   /**< source bytes for a rich cursor */
  unsigned char *alphaSource; /**< source for alpha blending info */
  rfbBool alphaPreMultiplied; /**< if richSource already has alpha applied */
} rfbCursor, *rfbCursorPtr;

extern unsigned char rfbReverseByte[0x100];

extern rfbBool rfbSendCursorShape(rfbClient * cl/*, rfbScreenInfoPtr pScreen*/);
extern         rfbBool rfbSendCursorPos(rfbClient * cl);
extern void    rfbConvertLSBCursorBitmapOrMask(int width,int height,unsigned char* bitmap);
extern         rfbCursorPtr rfbMakeXCursor(int width,int height,char* cursorString,char* maskString);
extern char*   rfbMakeMaskForXCursor(int width,int height,char* cursorString);
extern char*   rfbMakeMaskFromAlphaSource(int width,int height,unsigned char* alphaSource);
extern void    rfbMakeXCursorFromRichCursor( rfbScreenInfoPtr, rfbCursorPtr cursor);
extern void    rfbMakeRichCursorFromXCursor( rfbScreenInfoPtr, rfbCursorPtr cursor);
extern void    rfbSetCursor(                 rfbScreenInfoPtr, rfbCursorPtr c);

extern void    rfbFreeCursor(rfbCursorPtr cursor);

/** cursor handling for the pointer */
extern void rfbDefaultPtrAddEvent(int buttonMask,int x,int y,rfbClient * cl);

/* zrle.c */
#ifdef HAVE_LIBZ
extern rfbBool rfbSendRectEncodingZRLE(rfbClient * cl, int x, int y, int w,int h);
#endif

/* stats.c */

extern void rfbResetStats(rfbClient * cl);
extern void rfbPrintStats(rfbClient * cl);

/* font.c */

typedef struct rfbFontData
{ unsigned char* data;
  /**
    metaData is a 256*5 array:
    for each character
    (offset,width,height,x,y)
  */
  int* metaData;
} rfbFontData,* rfbFontDataPtr;

int rfbDrawChar(rfbScreenInfoPtr rfbScreen,rfbFontDataPtr font,int x,int y,unsigned char c,rfbPixel colour);
void rfbDrawString(rfbScreenInfoPtr rfbScreen,rfbFontDataPtr font,int x,int y,const char* string,rfbPixel colour);
/** if colour==backColour, background is transparent */
int rfbDrawCharWithClip(rfbScreenInfoPtr rfbScreen,rfbFontDataPtr font,int x,int y,unsigned char c,int x1,int y1,int x2,int y2,rfbPixel colour,rfbPixel backColour);
void rfbDrawStringWithClip(rfbScreenInfoPtr rfbScreen,rfbFontDataPtr font,int x,int y,const char* string,int x1,int y1,int x2,int y2,rfbPixel colour,rfbPixel backColour);
int rfbWidthOfString(rfbFontDataPtr font,const char* string);
int rfbWidthOfChar(rfbFontDataPtr font,unsigned char c);
void rfbFontBBox(rfbFontDataPtr font,unsigned char c,int* x1,int* y1,int* x2,int* y2);
/** this returns the smallest box enclosing any character of font. */
void rfbWholeFontBBox(rfbFontDataPtr font,int *x1, int *y1, int *x2, int *y2);

/** dynamically load a linux console font (4096 bytes, 256 glyphs a 8x16 */
rfbFontDataPtr rfbLoadConsoleFont(char *filename);
/** free a dynamically loaded font */
void rfbFreeFont(rfbFontDataPtr font);

/* draw.c */

void rfbFillRect(  rfbScreenInfoPtr s,int x1,int y1,int x2,int y2,rfbPixel col);
void rfbDrawPixel( rfbScreenInfoPtr s,int x,int y,rfbPixel col);
void rfbDrawLine(  rfbScreenInfoPtr s,int x1,int y1,int x2,int y2,rfbPixel col);

/* selbox.c */

/** this opens a modal select box. list is an array of strings, the end marked
   with a NULL.
   It returns the index in the list or -1 if cancelled or something else
   wasn't jalal. */

typedef void (*SelectionChangedHookPtr)(int _index);

extern int rfbSelectBox( rfbScreenInfoPtr
                       ,	rfbFontDataPtr, char** list
                       ,	int x1, int y1, int x2, int y2
                       ,	rfbPixel foreColour, rfbPixel backColour,	int border
                       , SelectionChangedHookPtr selChangedHook);


/* cargs.c */

extern void rfbUsage(void);
extern void rfbPurgeArguments(int* argc,int* position,int count,char *argv[]);
extern rfbBool rfbProcessArguments(rfbScreenInfoPtr rfbScreen,int* argc, char *argv[]);
extern rfbBool rfbProcessSizeArguments(int* width,int* height,int* bpp,int* argc, char *argv[]);

/* main.c */

extern void rfbLogEnable(int enabled);
extern void rfbLogPerror(const char *str);

typedef void (*rfbLogProc)(const char *format, ...);
extern rfbLogProc rfbLog, rfbErr;

void rfbScheduleCopyRect(     rfbScreenInfoPtr, int x1,int y1,int x2,int y2,int dx,int dy);
void rfbScheduleCopyRegion(   rfbScreenInfoPtr, sraRegionPtr copyRegion,int dx,int dy);
void rfbDoCopyRect(           rfbScreenInfoPtr, int x1,int y1,int x2,int y2,int dx,int dy);
void rfbDoCopyRegion(         rfbScreenInfoPtr, sraRegionPtr copyRegion,int dx,int dy);
void rfbMarkRegionAsModified( rfbScreenInfoPtr, sraRegionPtr modRegion);

void rfbDoNothingWithClient(  rfbClient * cl);
enum rfbNewClientAction defaultNewClientHook(rfbClient * cl);
void rfbRegisterProtocolExtension(   rfbProtocolExtension* extension);
void rfbUnregisterProtocolExtension( rfbProtocolExtension* extension);

struct _rfbProtocolExtension* rfbGetExtensionIterator();
void rfbReleaseExtensionIterator();
rfbBool rfbEnableExtension(rfbClient * cl, rfbProtocolExtension* extension,	void* data);
rfbBool rfbDisableExtension(rfbClient * cl, rfbProtocolExtension* extension);
void* rfbGetExtensionClientData(rfbClient * cl, rfbProtocolExtension* extension);

extern void rfbInitServer(     rfbScreenInfoPtr );
extern void rfbShutdownServer( rfbScreenInfoPtr ,rfbBool disconnectClients);
extern void rfbNewFramebuffer( rfbScreenInfoPtr ,char *framebuffer,
 int width,int height, int bitsPerSample,int samplesPerPixel,
 int bytesPerPixel);

extern void rfbScreenCleanup(rfbScreenInfoPtr screenInfo);
extern void rfbSetServerVersionIdentity(rfbScreenInfoPtr screen, char *fmt, ...);

/* functions to accept/refuse a client that has been put on hold
   by a NewClientHookPtr function. Must not be called in other
   situations. */
extern void rfbStartOnHoldClient(rfbClient * cl);
extern void rfbRefuseOnHoldClient(rfbClient * cl);

/* call one of these two functions to service the vnc clients.
 usec are the microseconds the select on the fds waits.
 if you are using the event loop, set this to some value > 0, so the
 server doesn't get a high load just by listening.
 rfbProcessEvents() returns TRUE if an update was pending. */

extern void rfbRunEventLoop(rfbScreenInfoPtr screenInfo, long usec, rfbBool runInBackground);
extern rfbBool rfbProcessEvents(rfbScreenInfoPtr screenInfo,long usec);
extern rfbBool rfbIsActive(rfbScreenInfoPtr screenInfo);

/* TightVNC file transfer extension */
void rfbRegisterTightVNCFileTransferExtension();
void rfbUnregisterTightVNCFileTransferExtension();

/* Statistics */
extern char *messageNameServer2Client(uint32_t type, char *buf, int len);
extern char *messageNameClient2Server(uint32_t type, char *buf, int len);
extern char *encodingName(uint32_t enc, char *buf, int len);

extern rfbStatList *rfbStatLookupEncoding(rfbClient * cl, uint32_t type);
extern rfbStatList *rfbStatLookupMessage( rfbClient * cl, uint32_t type);

/* Each call to rfbStatRecord* adds one to the rect count for that type */
extern void rfbStatRecordEncodingSent(    rfbClient * , uint32_t type, int byteCount, int byteIfRaw );
extern void rfbStatRecordEncodingSentAdd( rfbClient * , uint32_t type, int byteCount); /* Specifically for tight encoding */
extern void rfbStatRecordEncodingRcvd(    rfbClient * , uint32_t type, int byteCount, int byteIfRaw );
extern void rfbStatRecordMessageSent(     rfbClient * , uint32_t type, int byteCount, int byteIfRaw );
extern void rfbStatRecordMessageRcvd(     rfbClient * , uint32_t type, int byteCount, int byteIfRaw );
extern void rfbResetStats(                rfbClient * );
extern void rfbPrintStats(                rfbClient * );
extern  int rfbStatGetSentBytes(          rfbClient * );
extern  int rfbStatGetSentBytesIfRaw(     rfbClient * );
extern  int rfbStatGetRcvdBytes(          rfbClient * );
extern  int rfbStatGetRcvdBytesIfRaw(     rfbClient * );
extern  int rfbStatGetMessageCountSent(   rfbClient * , uint32_t type );
extern  int rfbStatGetMessageCountRcvd(   rfbClient * , uint32_t type );
extern  int rfbStatGetEncodingCountSent(  rfbClient * , uint32_t type );
extern  int rfbStatGetEncodingCountRcvd(  rfbClient * , uint32_t type );

/** Set which version you want to advertise 3.3, 3.6, 3.7 and 3.8 are currently supported*/
extern void rfbSetProtocolVersion(rfbScreenInfoPtr rfbScreen, int major_, int minor_);

/** send a TextChat message to a client */
extern rfbBool rfbSendTextChatMessage(rfbClient * cl, uint32_t length, char *buffer);




#endif
