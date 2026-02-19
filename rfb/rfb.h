#ifndef RFB_H
#define RFB_H

/**
 * @file rfb.h
 */

/**
 *  Copyright (C) 2005 Rohit Kumar <rokumar@novell.com>,
 *                     Johannes E. Schindelin <johannes.schindelin@gmx.de>
 *  Copyright (C) 2002 RealVNC Ltd.
 *  OSXvnc Copyright (C) 2001 Dan McGuirk <mcguirk@incompleteness.net>.
 *  Original Xvnc code Copyright (C) 1999 AT&T Laboratories Cambridge.
 *  All Rights Reserved.
 *
 *  This is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this software; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307,
 *  USA.
 */


/*
 * Additions for Qt event loop integration
 * Original idea taken from vino.
 */

 typedef   signed char rfbBool;
 typedef   signed int  rfbLong;
 typedef unsigned int  rfbDword;

#ifdef  BUILDING_VNCASYNC      /* Isolate library from caller */

 struct _rfbClient;
 struct _rfbScreenInfo;

#else

 #define _rfbClient     rfbClient
 #define _rfbScreenInfo rfbScreenInfo

#endif

typedef uint32_t rfbKeySym;
typedef uint32_t rfbPixel;

typedef rfbBool( * rfbSetTranslateFunctionProcPtr) ( struct _rfbClient * cl );


typedef void * (* VncPushFun   ) ( int sk
                                 , int ( *StackFun )( int sk
                                                    , void * userData
                                                    , time_t date, void * result
                                                    , int sz )
                                 , void * userData
                                 , const void * src, size_t sz );

typedef void   (* VncPopPtrFun ) ( int    b, int x, int y , struct _rfbClient * cl );
typedef void   (* VncPopKeyFun ) ( int attr, rfbKeySym key, struct _rfbClient * cl );

/**
 * to check against plain passwords
 */
typedef
  rfbBool(*rfbPasswordCheckProcPtr)( struct _rfbClient *, const char * resp, int len );
  rfbBool rfbCheckPasswordByList   ( struct _rfbClient *, const char * resp, int len );
  rfbBool rfbCheckPasswordSingle   ( struct _rfbClient *, const char * resp, int len );

/**
 * functions to make a vnc server
 */
void rfbMarkRectAsModified( struct _rfbScreenInfo *
                          , int x1, int y1
                          , int x2, int y2 );


struct _rfbScreenInfo * rfbGetScreen( void * frameBuffer
                                    , int width, int height
                                    , int bitsPerSample, int samplesPerPixel
                                    , int bytesPerPixel );

int      setVncEvents( struct _rfbScreenInfo *
                     , VncPushFun   // pusher
                     , VncPopPtrFun
                     , VncPopKeyFun );

int      setVncAuth  ( struct _rfbScreenInfo *
                     , const char * name             // desktop name
                     , const char * pass
                     , rfbPasswordCheckProcPtr );

int      getVncHandler ( struct _rfbClient * );


void rfbClientConnectionGone( struct _rfbClient * );
void rfbProcessClientMessage( struct _rfbClient * );
int  rfbSinkClientStream(     struct _rfbClient *, void *, size_t ); // JACS, client data sinker

struct _rfbClient * rfbNewStreamClient( struct _rfbScreenInfo *
                                      , struct _rfbClient     *
                                      , int fd );



void    rfbCloseClient(   struct _rfbClient     * );  // JACS, client data sinker
rfbBool rfbUpdateClient(  struct _rfbClient     * );
rfbBool rfbUpdateClients( struct _rfbScreenInfo * ); // JACS





/**
 @page libvncserver_doc LibVNCServer Documentation
 @section create_server Creating a server instance
 To make a server, you just have to initialise a server structure using the
 function rfbGetScreen(), like
 @code
   rfbScreenInfo * screen =
     rfbGetScreen(argc,argv,screenwidth,screenheight,8,3,bpp);
 @endcode
 where byte per pixel should be 1, 2 or 4. If performance doesn't matter,
 you may try bpp=3 (internally one cannot use native data types in this
 case; if you want to use this, look at pnmshow24.c).

 You then can set hooks and io functions (see @ref making_it_interactive) or other
 options (see @ref server_options).

 And you allocate the frame buffer like this:
 @code
     screen->window.frameBuffer = (char*)malloc(screenwidth*screenheight*bpp);
 @endcode
 After that, you initialize the server, like
 @code
   rfbInitServer(screen);
 @endcode
 You can use a blocking event loop, a background (pthread based) event loop,
 or implement your own using the rfbProcessEvents() function.

 @subsection server_options Optional Server Features

 These options have to be set between rfbGetScreen() and rfbInitServer().

 If you already have a socket to talk to, just set rfbScreenInfo::inetdSock
 (originally this is for inetd handling, but why not use it for your purpose?).

 To also start an HTTP server (running on port 5800+display_number), you have
 to set rfbScreenInfo::httpDir to a directory containing vncviewer.jar and
 index.vnc (like the included "webclients" directory).

 @section making_it_interactive Making it interactive

 Whenever you draw something, you have to call
 @code
  rfbMarkRectAsModified(screen,x1,y1,x2,y2).
 @endcode
 This tells LibVNCServer to send updates to all connected clients.

 There exist the following IO functions as members of rfbScreen:
 rfbScreenInfo::kbdAddEvent(), rfbScreenInfo::kbdReleaseAllKeys(), rfbScreenInfo::ptrAddEvent() and rfbScreenInfo::setXCutText()

 rfbScreenInfo::kbdAddEvent()
   is called when a key is pressed.
 rfbScreenInfo::kbdReleaseAllKeys()
   is not called at all (maybe in the future).
 rfbScreenInfo::ptrAddEvent()
   is called when the mouse moves or a button is pressed.
   WARNING: if you want to have proper cursor handling, call
	rfbDefaultPtrAddEvent()
   in your own function. This sets the coordinates of the cursor.
 rfbScreenInfo::setXCutText()
   is called when the selection changes.

 There are only two hooks:
 rfbScreenInfo::newClientHook()
   is called when a new client has connected.
 rfbScreenInfo::displayHook()
   is called just before a frame buffer update is sent.

 You can also override the following methods:
 rfbScreenInfo::getCursorPtr()
   This could be used to make an animated cursor (if you really want ...)
 rfbScreenInfo::setTranslateFunction()
   If you insist on colour maps or something more obscure, you have to
   implement this. Default is a trueColour mapping.

 @section cursor_handling Cursor handling

 The screen holds a pointer
  rfbScreenInfo::cursor
 to the current cursor. Whenever you set it, remember that any dynamically
 created cursor (like return value from rfbMakeXCursor()) is not free'd!

 The rfbCursor structure consists mainly of a mask and a source. The rfbCursor::mask
 describes, which pixels are drawn for the cursor (a cursor needn't be
 rectangular). The rfbCursor::source describes, which colour those pixels should have.

 The standard is an XCursor: a cursor with a foreground and a background
 colour (stored in backRed,backGreen,backBlue and the same for foreground
 in a range from 0-0xffff). Therefore, the arrays "mask" and "source"
 contain pixels as single bits stored in bytes in MSB order. The rows are
 padded, such that each row begins with a new byte (i.e. a 10x4
 cursor's mask has 2x4 bytes, because 2 bytes are needed to hold 10 bits).

 It is however very easy to make a cursor like this:
 @code
 char* cur="    "
           " xx "
	   " x  "
	   "    ";
 char* mask="xxxx"
            "xxxx"
            "xxxx"
            "xxx ";
 rfbCursorPtr c=rfbMakeXCursor(4,4,cur,mask);
 @endcode
 You can even set rfbCursor::mask to NULL in this call and LibVNCServer will calculate
 a mask for you (dynamically, so you have to free it yourself).

 There is also an array named rfbCursor::richSource for colourful cursors. They have
 the same format as the frameBuffer (i.e. if the server is 32 bit,
 a 10x4 cursor has 4x10x4 bytes).

 @section screen_client_difference What is the difference between rfbScreenInfo * and rfbClient *?

 The rfbScreenInfo * is a pointer to a rfbScreenInfo structure, which
 holds information about the server, like pixel format, io functions,
 frame buffer etc. The rfbClient * is a pointer to an rfbClientRec structure, which holds
 information about a client, like pixel format, socket of the
 connection, etc. A server can have several clients, but needn't have any. So, if you
 have a server and three clients are connected, you have one instance
 of a rfbScreenInfo and three instances of rfbClientRec's.

 The rfbClientRec structure holds a member rfbClientRec::screen which points to the server.
 So, to access the server from the client structure, you use client->screen.

 To access all clients from a server be sure to use the provided iterator
  rfbGetClientIterator()
 with
  rfbClientIteratorNext()
 and
  rfbReleaseClientIterator()
 to prevent thread clashes.

 @section example_code Example Code

 There are two documented examples included:
  - example.c, a shared scribble sheet
  - pnmshow.c, a program to show PNMs (pictures) over the net.

 The examples are not too well documented, but easy straight forward and a
 good starting point.

 Try example.c: it outputs on which port it listens (default: 5900), so it is
 display 0. To view, call @code	vncviewer :0 @endcode
 You should see a sheet with a gradient and "Hello World!" written on it. Try
 to paint something. Note that every time you click, there is some bigger blot,
 whereas when you drag the mouse while clicked you draw a line. The size of the
 blot depends on the mouse button you click. Open a second vncviewer with
 the same parameters and watch it as you paint in the other window. This also
 works over internet. You just have to know either the name or the IP of your
 machine. Then it is @code vncviewer machine.where.example.runs.com:0 @endcode
 or similar for the remote client. Now you are ready to type something. Be sure
 that your mouse sits still, because every time the mouse moves, the cursor is
 reset to the position of the pointer! If you are done with that demo, press
 the down or up arrows. If your viewer supports it, then the dimensions of the
 sheet change. Just press Escape in the viewer. Note that the server still
 runs, even if you closed both windows. When you reconnect now, everything you
 painted and wrote is still there. You can press "Page Up" for a blank page.

 The demo pnmshow.c is much simpler: you either provide a filename as argument
 or pipe a file through stdin. Note that the file has to be a raw pnm/ppm file,
 i.e. a truecolour graphics. Only the Escape key is implemented. This may be
 the best starting point if you want to learn how to use LibVNCServer. You
 are confronted with the fact that the bytes per pixel can only be 8, 16 or 32.
*/

#endif
