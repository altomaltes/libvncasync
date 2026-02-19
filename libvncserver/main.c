/*
    This file is called main.c, because it contains most of the new functions
    for use with LibVNCServer.

    LibVNCServer (C) 2001 Johannes E. Schindelin <Johannes.Schindelin@gmx.de>
    Original OSXvnc (C) 2001 Dan McGuirk <mcguirk@incompleteness.net>.
    Original Xvnc (C) 1999 AT&T Laboratories Cambridge.
    All Rights Reserved.

    see GPL (latest version) for full details
*/

#include <string.h>
#include <stdio.h>

#include <rfb/rfbproto.h>
#include <rfb/rfbregion.h>

#include "private.h"

#include <stdarg.h>
#include <errno.h>

#ifndef false
#define false 0
#define true -1
#endif

#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif

#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#include <fcntl.h>
#endif

#include <signal.h>
#include <time.h>


static int rfbEnableLogging=1;

#ifdef WORDS_BIGENDIAN
char rfbEndianTest = (1==0);
#else
char rfbEndianTest = (1==1);
#endif

/*
   Protocol extensions
*/

static rfbProtocolExtension* rfbExtensionHead = NULL;

/*
   This method registers a list of new extensions.
   It avoids same extension getting registered multiple times.
   The order is not preserved if multiple extensions are
   registered at one-go.
*/
void
rfbRegisterProtocolExtension(rfbProtocolExtension* extension)
{ rfbProtocolExtension *head = rfbExtensionHead, *next = NULL;

  if(extension == NULL)
    return;

  next = extension->next;


  while(head != NULL)
  { if(head == extension)
    { rfbRegisterProtocolExtension(next);
      return;
    }

    head = head->next;
  }

  extension->next = rfbExtensionHead;
  rfbExtensionHead = extension;

  rfbRegisterProtocolExtension(next);
}

/*
   This method unregisters a list of extensions.
   These extensions won't be available for any new
   client connection.
*/
void
rfbUnregisterProtocolExtension(rfbProtocolExtension* extension)
{

  rfbProtocolExtension *cur = NULL, *pre = NULL;

  if(extension == NULL)
    return;


  if(rfbExtensionHead == extension)
  { rfbExtensionHead = rfbExtensionHead->next;
    rfbUnregisterProtocolExtension(extension->next);
    return;
  }

  cur = pre = rfbExtensionHead;

  while(cur)
  { if(cur == extension)
    { pre->next = cur->next;
      break;
    }
    pre = cur;
    cur = cur->next;
  }

  rfbUnregisterProtocolExtension(extension->next);
}

rfbProtocolExtension* rfbGetExtensionIterator()
{ return rfbExtensionHead;
}

void rfbReleaseExtensionIterator()
{
}

rfbBool rfbEnableExtension( rfbClient * cl
                            , rfbProtocolExtension* extension
                            , void* data )
{ rfbExtensionData* extData;

  /* make sure extension is not yet enabled.
  */
  for( extData = cl->extensions
                 ; extData
       ; extData = extData->next )
    if(extData->extension == extension)
      return FALSE;

  extData = calloc(sizeof(rfbExtensionData),1);
  extData->extension = extension;
  extData->data = data;
  extData->next = cl->extensions;
  cl->extensions = extData;

  return TRUE;
}

rfbBool rfbDisableExtension( rfbClient * cl, rfbProtocolExtension* extension )
{ rfbExtensionData* extData;
  rfbExtensionData* prevData = NULL;

  for(extData = cl->extensions; extData; extData = extData->next)
  { if(extData->extension == extension)
    { FREE( extData->data );
      if(prevData == NULL)
      { cl->extensions = extData->next;
      }
      else
      { prevData->next = extData->next;
      }
      return TRUE;
    }
    prevData = extData;
  }

  return FALSE;
}

void* rfbGetExtensionClientData(rfbClient * cl, rfbProtocolExtension* extension)
{ rfbExtensionData* data = cl->extensions;

  while(data && data->extension != extension)
    data = data->next;

  if(data == NULL)
  { rfbLog("Extension is not enabled !\n");
    /* rfbCloseClient(cl); */
    return NULL;
  }

  return data->data;
}

/*
   Logging
*/

void rfbLogEnable(int enabled)
{ rfbEnableLogging=enabled;
}

/*
   rfbLog prints a time-stamped message to the log file (stderr).
*/

static void rfbDefaultLog(const char *format, ...)
{ va_list args;
  char buf[256];
  time_t log_clock;

  if(!rfbEnableLogging)
    return;

  va_start(args, format);

  time(&log_clock);
  strftime(buf, 255, "%d/%m/%Y %X ", localtime(&log_clock));
  fprintf(stderr, "%s", buf);

  vfprintf(stderr, format, args);
  fflush(stderr);

  va_end(args);
}

rfbLogProc rfbLog=rfbDefaultLog;
rfbLogProc rfbErr=rfbDefaultLog;

void rfbScheduleCopyRegion( rfbScreenInfo * rfbScreen,sraRegionPtr copyRegion
                            , int dx,int dy )
{ rfbClientIteratorPtr iterator;
  rfbClient * cl;

  iterator=rfbGetClientIterator(rfbScreen);
  while((cl=rfbClientIteratorNext(iterator)))
  { if(cl->useCopyRect)
    { sraRegionPtr modifiedRegionBackup;
      if(!sraRgnEmpty(cl->copyRegion))
      { if(cl->copyDX!=dx || cl->copyDY!=dy)
        { /* if a copyRegion was not yet executed, treat it as a
             modifiedRegion. The idea: in this case it could be
             source of the new copyRect or modified anyway. */
          sraRgnOr(cl->modifiedRegion,cl->copyRegion);
          sraRgnMakeEmpty(cl->copyRegion);
        }
        else
        { /* we have to set the intersection of the source of the copy
             and the old copy to modified. */
          modifiedRegionBackup=sraRgnCreateRgn(copyRegion);
          sraRgnOffset(modifiedRegionBackup,-dx,-dy);
          sraRgnAnd(modifiedRegionBackup,cl->copyRegion);
          sraRgnOr(cl->modifiedRegion,modifiedRegionBackup);
          sraRgnDestroy(modifiedRegionBackup);
        }
      }

      sraRgnOr(cl->copyRegion,copyRegion);
      cl->copyDX = dx;
      cl->copyDY = dy;

      /* if there were modified regions, which are now copied,
        mark them as modified, because the source of these can be overlapped
        either by new modified or now copied regions. */
      modifiedRegionBackup=sraRgnCreateRgn(cl->modifiedRegion);
      sraRgnOffset(modifiedRegionBackup,dx,dy);
      sraRgnAnd(modifiedRegionBackup,cl->copyRegion);
      sraRgnOr(cl->modifiedRegion,modifiedRegionBackup);
      sraRgnDestroy(modifiedRegionBackup);

      /*
         n.b. (dx, dy) is the vector pointing in the direction the
         copyrect displacement will take place.  copyRegion is the
         destination rectangle (say), not the source rectangle.
      */
      if(!cl->enableCursorShapeUpdates)
      { sraRegionPtr cursorRegion;
        int x = cl->cursorX - cl->screen->cursor->xhot;
        int y = cl->cursorY - cl->screen->cursor->yhot;
        int w = cl->screen->cursor->width;
        int h = cl->screen->cursor->height;

        cursorRegion = sraRgnCreateRect(x, y, x + w, y + h);
        sraRgnAnd(cursorRegion, cl->copyRegion);
        if(!sraRgnEmpty(cursorRegion))
        { /*
             current cursor rect overlaps with the copy region *dest*,
             mark it as modified since we won't copy-rect stuff to it.
          */
          sraRgnOr(cl->modifiedRegion, cursorRegion);
        }
        sraRgnDestroy(cursorRegion);

        cursorRegion = sraRgnCreateRect(x, y, x + w, y + h);
        /* displace it to check for overlap with copy region source: */
        sraRgnOffset(cursorRegion, dx, dy);
        sraRgnAnd(cursorRegion, cl->copyRegion);

        /*
           current cursor rect overlaps with the copy region *source*,
           mark the *displaced* cursorRegion as modified since we
           won't copyrect stuff to it.
        */

        if (!sraRgnEmpty(cursorRegion))
        { sraRgnOr(cl->modifiedRegion, cursorRegion);
        }
        sraRgnDestroy(cursorRegion);
      }
    }
    else
    { sraRgnOr(cl->modifiedRegion,copyRegion);
    }
  }

  rfbReleaseClientIterator(iterator);
}

void rfbDoCopyRegion( rfbScreenInfo * screen
                      , sraRegionPtr copyRegion
                      , int dx,int dy )
{ sraRectangleIterator* i;
  sraRect rect;
  int j,widthInBytes,bpp=screen->window.serverFormat.bitsPerPixel/8,
                     rowstride=screen->window.paddedWidthInBytes;
  char *in,*out;

  /* copy it, really */
  i = sraRgnGetReverseIterator(copyRegion,dx<0,dy<0);

  while(sraRgnIteratorNext(i,&rect))
  { widthInBytes = (rect.x2-rect.x1)*bpp;
    out = screen->window.frameBuffer+rect.x1*bpp+rect.y1*rowstride;
    in  = screen->window.frameBuffer+(rect.x1-dx)*bpp+(rect.y1-dy)*rowstride;

    if ( dy<0 )
      for(j=rect.y1; j<rect.y2; j++,out+=rowstride,in+=rowstride)
        memmove(out,in,widthInBytes);
    else
    { out += rowstride*(rect.y2-rect.y1-1);
      in += rowstride*(rect.y2-rect.y1-1);
      for(j=rect.y2-1; j>=rect.y1; j--,out-=rowstride,in-=rowstride)
        memmove(out,in,widthInBytes);
    }
  }
  sraRgnReleaseIterator(i);

  rfbScheduleCopyRegion(screen,copyRegion,dx,dy);
}

void rfbDoCopyRect(rfbScreenInfo * screen,int x1,int y1,int x2,int y2,int dx,int dy)
{ sraRegionPtr region = sraRgnCreateRect(x1,y1,x2,y2);
  rfbDoCopyRegion(screen,region,dx,dy);
  sraRgnDestroy(region);
}

void rfbScheduleCopyRect(rfbScreenInfo * screen,int x1,int y1,int x2,int y2,int dx,int dy)
{ sraRegionPtr region = sraRgnCreateRect(x1,y1,x2,y2);
  rfbScheduleCopyRegion(screen,region,dx,dy);
  sraRgnDestroy(region);
}

void rfbMarkRegionAsModified( rfbScreenInfo * screen
                              , sraRegionPtr modRegion )
{ rfbClientIteratorPtr iterator;
  rfbClient * cl;

  iterator= rfbGetClientIterator( screen );

  while(( cl= rfbClientIteratorNext( iterator ) ))
  { sraRgnOr( cl->modifiedRegion
              , modRegion);
  }

  rfbReleaseClientIterator( iterator );
}

void rfbScaledScreenUpdate( ScreenAtom * screen
                          , int x1, int y1
                          , int x2, int y2);

void rfbMarkRectAsModified( rfbScreenInfo * screen
                          , int x1, int y1
                          , int x2, int y2 )
{ sraRegionPtr region;
  int i;

  if ( x1>x2)
  { i=x1;
    x1=x2;
    x2=i;
  }

  if ( x1<0 )
  { x1=0;
  }

  if ( x2>screen->window.width) x2=screen->window.width;
  if ( x1==x2) return;

  if ( y1>y2)
  { i=y1;
    y1=y2;
    y2=i;
  }

  if ( y1<0) y1=0;

  if ( y2>screen->window.height)
  { y2=screen->window.height;
  }
  if ( y1==y2) return;

  /* update scaled copies for this rectangle */
  rfbScaledScreenUpdate(screen,x1,y1,x2,y2);

  region = sraRgnCreateRect(x1,y1,x2,y2);
  rfbMarkRegionAsModified(screen,region);
  sraRgnDestroy(region);
}

void rfbStartOnHoldClient(rfbClient * cl)
{ cl->onHold = FALSE;
}


void rfbRefuseOnHoldClient(rfbClient * cl)
{ rfbCloseClient(cl);
  rfbClientConnectionGone(cl);
}

static void rfbDefaultKbdAddEvent(rfbBool down, rfbKeySym keySym, rfbClient * cl)
{
}

void rfbDefaultPtrAddEvent( int buttonMask
                          , int x, int y
                          , rfbClient * cl )
{ rfbClientIteratorPtr iterator;
  rfbClient * other_client;
  rfbScreenInfo * s = cl->screen;

  if (x != s->cursorX || y != s->cursorY)
  { s->cursorX = x;
    s->cursorY = y;

    /* The cursor was moved by this client, so don't send CursorPos. */
    if (cl->enableCursorPosUpdates)
      cl->cursorWasMoved = FALSE;

    /* But inform all remaining clients about this cursor movement.
    */
    iterator = rfbGetClientIterator(s);
    while ((other_client = rfbClientIteratorNext(iterator)) != NULL)
    { if (other_client != cl && other_client->enableCursorPosUpdates)
      { other_client->cursorWasMoved = TRUE;
      }
    }
    rfbReleaseClientIterator(iterator);
  }
}

static void rfbDefaultSetXCutText(char* text, int len, rfbClient * cl)
{
}

/* TODO: add a nice VNC or RFB cursor */

#if defined(WIN32) || defined(sparc) || !defined(NO_STRICT_ANSI)
static rfbCursor myCursor =
{ FALSE, FALSE, FALSE, FALSE,
  (unsigned char*)"\000\102\044\030\044\102\000",
  (unsigned char*)"\347\347\176\074\176\347\347",
  8, 7, 3, 3,
  0, 0, 0,
  0xffff, 0xffff, 0xffff,
  NULL
};
#else
static rfbCursor myCursor =
{ cleanup:
  FALSE,
cleanupSource:
  FALSE,
cleanupMask:
  FALSE,
cleanupRichSource:
  FALSE,
source: "\000\102\044\030\044\102\000"
  ,
mask:   "\347\347\176\074\176\347\347"
  ,
  width: 8, height: 7, xhot: 3, yhot: 3,
  foreRed: 0, foreGreen: 0, foreBlue: 0,
  backRed: 0xffff, backGreen: 0xffff, backBlue: 0xffff,
richSource:
  NULL
};
#endif

static rfbCursorPtr rfbDefaultGetCursorPtr(rfbClient * cl)
{ return(cl->screen->cursor);
}

/* response is cl->authChallenge vncEncrypted with passwd */
static rfbBool rfbDefaultPasswordCheck(rfbClient * cl,const char* response,int len)
{ int i;
  char *passwd=rfbDecryptPasswdFromFile(cl->screen->authPasswdData);

  if (!passwd)
  { rfbErr("Couldn't read password file: %s\n",cl->screen->authPasswdData);
    return(FALSE);
  }

  rfbEncryptBytes(cl->authChallenge, passwd);

  /* Lose the password from memory */
  for (i = strlen(passwd); i >= 0; i--)
  { passwd[i] = '\0';
  }

  FREE( passwd );

  if (memcmp(cl->authChallenge, response, len) != 0)
  { rfbErr("authProcessClientMessage: authentication failed from %s\n",
           "cl->host" );
    return(FALSE);
  }

  return(TRUE);
}

/* for this method, authPasswdData is really a pointer to an array
   of char*'s, where the last pointer is 0. */
rfbBool rfbCheckPasswordByList(rfbClient * cl,const char* response,int len)
{ char **passwds;
  int i=0;

  for(passwds=(char**)cl->screen->authPasswdData; *passwds; passwds++,i++)
  { uint8_t auth_tmp[CHALLENGESIZE];
    memcpy((char *)auth_tmp, (char *)cl->authChallenge, CHALLENGESIZE);
    rfbEncryptBytes(auth_tmp, *passwds);

    if (memcmp(auth_tmp, response, len) == 0)
    { if(i>=cl->screen->authPasswdFirstViewOnly)
        cl->viewOnly=TRUE;
      return(TRUE);
    }
  }

  rfbErr("authProcessClientMessage: authentication failed from %s\n",
         "cl->host" );
  return(FALSE);
}

rfbBool rfbCheckPasswordSingle( rfbClient * cl
                              , const char * response, int len )
{ uint8_t auth_tmp[CHALLENGESIZE];

  memcpy((char *)auth_tmp, (char *)cl->authChallenge, CHALLENGESIZE);
  rfbEncryptBytes( auth_tmp, cl->screen->authPasswdData );

  if ( !memcmp(auth_tmp, response, len ))
  { if ( ! cl->screen->authPasswdFirstViewOnly )
    { cl->viewOnly=TRUE;
    }
    return(TRUE);
  }

  rfbErr( "rfbCheckPasswordSingle: authentication failed from %s\n"
        , "cl->host" );

  return(FALSE);
}


void rfbDoNothingWithClient(rfbClient * cl)
{
}

static enum rfbNewClientAction rfbDefaultNewClientHook(rfbClient * cl)
{ return RFB_CLIENT_ACCEPT;
}

static int rfbDefaultNumberOfExtDesktopScreens(rfbClient * cl)
{ return 1;
}

static rfbBool rfbDefaultGetExtDesktopScreen(int seqnumber, rfbExtDesktopScreen* s, rfbClient * cl)
{ if (seqnumber != 0)
    return FALSE;

  /* Populate the provided rfbExtDesktopScreen structure */
  s->id = 1;
  s->width = cl->scaledScreen->window.width;
  s->height = cl->scaledScreen->window.height;
  s->x = 0;
  s->y = 0;
  s->flags = 0;

  return TRUE;
}

static int rfbDefaultSetDesktopSize(int width, int height, int numScreens, rfbExtDesktopScreen* extDesktopScreens, rfbClient * cl)
{ return rfbExtDesktopSize_ResizeProhibited;
}

/*
   Update server's pixel format in screenInfo structure. This
   function is called from rfbGetScreen() and rfbNewFramebuffer().
*/

static void rfbInitServerFormat( rfbScreenInfo * screen
                               , int bitsPerSample )
{ rfbPixelFormat* format=&screen->window.serverFormat;

  format->bitsPerPixel = screen->window.bitsPerPixel;
  format->depth = screen->depth;
  format->bigEndian = rfbEndianTest?FALSE:TRUE;
  format->trueColour = TRUE;
  screen->colourMap.count = 0;
  screen->colourMap.is16 = 0;
  screen->colourMap.data.bytes = NULL;

  if (format->bitsPerPixel == 8)
  { format->redMax = 7;
    format->greenMax = 7;
    format->blueMax = 3;
    format->redShift = 0;
    format->greenShift = 3;
    format->blueShift = 6;
  }
  else
  { format->redMax  =
      format->greenMax=
        format->blueMax = (1 << bitsPerSample) - 1;

    if ( rfbEndianTest )
    { format->redShift  = bitsPerSample * 2;  // JACS, windows compatible
      format->greenShift= bitsPerSample;
      format->blueShift = 0;
    }
    else
    { if(format->bitsPerPixel==8*3)
      { format->redShift = bitsPerSample*2;
        format->greenShift = bitsPerSample;
        format->blueShift = 0;
      }
      else
      { format->redShift  = bitsPerSample*3;
        format->greenShift= bitsPerSample*2;
        format->blueShift = bitsPerSample;
} } } }

//                                     , int width,int height
//                                   , int bitsPerSample, int bytesPerPixel, int lpad );

int setVncEvents( struct _rfbScreenInfo * screen
                , VncPushFun   pusher   // pusher
                , VncPopPtrFun doPtr
                , VncPopKeyFun doKey )
{ if ( screen )
  { screen->streamPusher= pusher;
    screen->ptrAddEvent= doPtr;
    screen->kbdAddEvent= doKey;
    return( 0 );
  }

  return( 1 );
}

int setVncAuth( rfbScreenInfo * screen
              , const char * name             // desktop name
              , const char * pass
              , rfbPasswordCheckProcPtr proc )
{ if ( screen )
  { screen->alwaysShared  = TRUE;
    screen->authPasswdData= pass;
    screen->passwordCheck = proc;
    screen->desktopName   = name;
    return( 0 );
  }

  return( 1 );
}


int  getVncHandler ( rfbClient * cl )
{ if ( cl )
  { return( cl->fd ) ;
  }

  return( sizeof( rfbClient )); /* Overload with client sise */
}



rfbScreenInfo * rfbGetScreen( void * frameBuffer
                             , int width, int height
                             , int bitsPerSample
                             , int  stride               // samplesPerPixel1 JACS, nos standard stride
                             , int bytesPerPixel )
{ rfbScreenInfo * screen= calloc(sizeof(rfbScreenInfo),1);

  if ( width & 3 )
  { rfbErr("WARNING: Width (%d) is not a multiple of 4. VncViewer has problems with that.\n",width);
  }

  screen->window.frameBuffer=   frameBuffer;
 // screen->autoPort=      FALSE;
  screen->clientHead=    NULL;
  screen->pointerClient= NULL;

//  screen->inetdInitDone = FALSE;
//  screen->udpClient=NULL;
  screen->fdQuota = 0.5;
  screen->desktopName    = "AsyncServerVNC";
  screen->alwaysShared   = FALSE;
  screen->neverShared    = FALSE;
  screen->dontDisconnect = FALSE;
  screen->authPasswdData = NULL;
  screen->authPasswdFirstViewOnly = 1;

  screen->window.width = width;
  screen->window.height = height;
  screen->window.bitsPerPixel = screen->depth = 8*bytesPerPixel;

  screen->passwordCheck = rfbDefaultPasswordCheck;
  screen->ignoreSIGPIPE = TRUE;

  screen->progressiveSliceHeight = 0; /* disable progressive updating per default */
  screen->deferUpdateTime= 5;
  screen->maxRectsPerUpdate= 50;

  screen->handleEventsEagerly = FALSE;

  screen->protocolMajorVersion = rfbProtocolMajorVersion;
  screen->protocolMinorVersion = rfbProtocolMinorVersion;

  screen->permitFileTransfer = FALSE;

//   if ( !rfbProcessArguments( screen
//                            , argc
//                            , argv ) )
//   { FREE( screen );
//     return NULL;
//   }

#ifdef _WIN32

  { unsigned int dummy=255;
    GetComputerNameA(screen->thisHost,&dummy);
    // strcpy
  }
#else
  gethostname( screen->thisHost, 255);
#endif

//  screen->window.paddedWidthInBytes = width*bytesPerPixel;
  screen->window.paddedWidthInBytes= ( stride < 0  ? -stride : width ) * bytesPerPixel;  /*  JACS, partial surface support */

  /* format */

  rfbInitServerFormat(screen, bitsPerSample);

  /* cursor */

  screen->cursorX=screen->cursorY=screen->underCursorBufferLen=0;
  screen->underCursorBuffer=NULL;
  screen->dontConvertRichCursorToXCursor = FALSE;
  screen->cursor = &myCursor;

  /* proc's and hook's */

  screen->kbdAddEvent         = rfbDefaultKbdAddEvent;
  screen->kbdReleaseAllKeys   = rfbDoNothingWithClient;
  screen->ptrAddEvent         = rfbDefaultPtrAddEvent;
  screen->setXCutText         = rfbDefaultSetXCutText;
  screen->getCursorPtr        = rfbDefaultGetCursorPtr;
// NULL  screen->setModified         = rfbMarkRectAsModified;
  screen->setTranslateFunction= rfbSetTranslateFunction;
  screen->newClientHook       = rfbDefaultNewClientHook;
  screen->displayHook         = NULL;
  screen->displayFinishedHook = NULL;
  screen->xvpHook             = NULL;
  screen->setDesktopSizeHook  = rfbDefaultSetDesktopSize;
  screen->getKeyboardLedStateHook= NULL;
  screen->getExtDesktopScreenHook= rfbDefaultGetExtDesktopScreen;
  screen->numberOfExtDesktopScreensHook= rfbDefaultNumberOfExtDesktopScreens;

  /* initialize client list and iterator mutex */
  rfbClientListInit( screen );

  return(screen);
}

/**
 *  Switch to another framebuffer (maybe of different size and color
 *  format). Clients supporting NewFBSize pseudo-encoding will change
 *  their local framebuffer dimensions if necessary.
 *  NOTE: Rich cursor data should be converted to new pixel format by
 *  the caller.
 */
void rfbNewFramebuffer( rfbScreenInfo * screen, char *framebuffer
                      , int width, int height
                      , int bitsPerSample, int samplesPerPixel
                      , int bytesPerPixel )
{ rfbPixelFormat old_format;
  rfbBool format_changed = FALSE;
  rfbClientIteratorPtr iterator;
  rfbClient * cl;

  /* Update information in the screenInfo structure */

  old_format = screen->window.serverFormat;

  if (width & 3)
    rfbErr("WARNING: New width (%d) is not a multiple of 4.\n", width);

  screen->window.width = width;
  screen->window.height = height;
  screen->window.bitsPerPixel = screen->depth = 8*bytesPerPixel;
  screen->window.paddedWidthInBytes = width*bytesPerPixel;

  rfbInitServerFormat(screen, bitsPerSample);

  if (memcmp(&screen->window.serverFormat, &old_format,
             sizeof(rfbPixelFormat)) != 0)
  { format_changed = TRUE;
  }

  screen->window.frameBuffer= framebuffer;

  /* Adjust pointer position if necessary */

  if (screen->cursorX >= width)
  { screen->cursorX = width - 1;
  }

  if (screen->cursorY >= height)
  { screen->cursorY = height - 1;
  }

  /* For each client: */
  iterator = rfbGetClientIterator(screen);

  while ((cl = rfbClientIteratorNext(iterator)))
  { if (format_changed) /* Re-install color translation tables if necessary */
    { screen->setTranslateFunction(cl);
    }

    /* Mark the screen contents as changed, and schedule sending
       NewFBSize message if supported by this client. */

    sraRgnDestroy(cl->modifiedRegion);
    cl->modifiedRegion = sraRgnCreateRect(0, 0, width, height);
    sraRgnMakeEmpty(cl->copyRegion);
    cl->copyDX = 0;
    cl->copyDY = 0;

    if (cl->useNewFBSize)
      cl->newFBSizePending = TRUE;

  }
  rfbReleaseClientIterator(iterator);
}

/* hang up on all clients and free all reserved memory */

void rfbScreenCleanup(rfbScreenInfo * screen)
{ rfbClientIteratorPtr i=rfbGetClientIterator(screen);
  rfbClient * cl
  , * cl1= rfbClientIteratorNext(i);

  while(cl1)
  { cl=rfbClientIteratorNext(i);
    rfbClientConnectionGone(cl1);
    cl1=cl;
  }
  rfbReleaseClientIterator(i);

#define FREE_IF(x) if(screen->x) free(screen->x)
  FREE_IF(colourMap.data.bytes);
  FREE_IF(underCursorBuffer);
  if(screen->cursor && screen->cursor->cleanup)
    rfbFreeCursor(screen->cursor);

#ifdef HAVE_LIBZ
  rfbZlibCleanup(screen);
#ifdef HAVE_LIBJPEG
  rfbTightCleanup(screen);
#endif

  /** free all 'scaled' versions of this screen
   */
  while (screen->window.scaledScreenNext )
  { rfbScreenInfo * ptr;
    ptr = screen->window.scaledScreenNext;
    screen->window.scaledScreenNext = ptr->window.scaledScreenNext;
    FREE( ptr->window.frameBuffer );
    FREE( ptr );
  }

#endif
  FREE( screen );
}

void rfbInitServer(rfbScreenInfo * screen)
{
#ifndef WIN32
  if ( screen->ignoreSIGPIPE )
    signal(SIGPIPE,SIG_IGN);
#endif
}

void rfbShutdownServer(rfbScreenInfo * screen,rfbBool disconnectClients)
{ if(disconnectClients)
  { rfbClientIteratorPtr iter = rfbGetClientIterator(screen);
    rfbClient * nextCl
    , * currentCl = rfbClientIteratorNext(iter);

    while(currentCl)
    { nextCl = rfbClientIteratorNext(iter);

      rfbClientConnectionGone(currentCl);

      currentCl = nextCl;
    }

    rfbReleaseClientIterator(iter);
  }

//  rfbShutdownSockets(screen);
// rfbHttpShutdownSockets(screen);
}

#ifdef _WIN32

#include <windows.h>
//#include <fcntl.h>
//#include <conio.h>
//#include <sys/timeb.h>

int  gettimeofday123( struct timeval*  tv,char *  dummy )
{ SYSTEMTIME t;

  GetSystemTime( &t );
  tv->tv_sec=  t.wHour*3600+t.wMinute*60+t.wSecond;
  tv->tv_usec= t.wMilliseconds*1000;

  return( 0 );
}

#endif

extern rfbClientIteratorPtr rfbGetClientIteratorWithClosed(rfbScreenInfo * rfbScreen);
// JACS
rfbBool rfbUpdateClients( rfbScreenInfo * screen )
{ rfbClient * clPrev;
  rfbClientIteratorPtr i= rfbGetClientIteratorWithClosed(screen);
  rfbClient * cl= rfbClientIteratorHead(i);
  rfbBool result=FALSE;

  while(cl)
  { result = rfbUpdateClient(cl);
    clPrev=cl;
    cl=rfbClientIteratorNext(i);
  }
  rfbReleaseClientIterator(i);
  return( result );
}


rfbBool rfbProcessEvents( rfbScreenInfo * screen
                        , rfbLong usec)
{ rfbClientIteratorPtr i;
  rfbClient * cl
          , * clPrev;
  rfbBool result=FALSE;

  if( usec < 0 )
  { usec= screen->deferUpdateTime * 1000;
  }

  i = rfbGetClientIteratorWithClosed(screen);
  cl=rfbClientIteratorHead(i);

  while(cl)
  { result = rfbUpdateClient(cl);
    clPrev=cl;
    cl=rfbClientIteratorNext(i);
  }
  rfbReleaseClientIterator(i);

  return result;
}

rfbBool rfbUpdateClient( rfbClient * cl )
{ struct timeval tv;
  rfbBool result=FALSE;
  rfbScreenInfo * screen = cl->screen;

  if ( !cl->onHold
       && FB_UPDATE_PENDING(cl)
       && !sraRgnEmpty(cl->requestedRegion))
  { result=TRUE;
    if ( screen->deferUpdateTime == 0)
    { rfbSendFramebufferUpdate(cl,cl->modifiedRegion);
    }

    else if(cl->startDeferring.tv_usec == 0)
    { gettimeofday(&cl->startDeferring,NULL);
      if(cl->startDeferring.tv_usec == 0)
        cl->startDeferring.tv_usec++;
    }

    else
    { gettimeofday(&tv,NULL);
      if ( tv.tv_sec < cl->startDeferring.tv_sec /* at midnight */
           || ((tv.tv_sec-cl->startDeferring.tv_sec)*1000
               +(tv.tv_usec-cl->startDeferring.tv_usec)/1000)
           > screen->deferUpdateTime)
      { cl->startDeferring.tv_usec = 0;
        rfbSendFramebufferUpdate( cl,cl->modifiedRegion );
  } } }

  if (!cl->viewOnly && cl->lastPtrX >= 0)
  { if( cl->startPtrDeferring.tv_usec == 0)
    { gettimeofday(&cl->startPtrDeferring,NULL);

      if(cl->startPtrDeferring.tv_usec == 0)
        cl->startPtrDeferring.tv_usec++;
    }

    else
    { struct timeval tv;
      gettimeofday(&tv,NULL);
      if(tv.tv_sec < cl->startPtrDeferring.tv_sec /* at midnight */
          || ((tv.tv_sec-cl->startPtrDeferring.tv_sec)*1000
              +(tv.tv_usec-cl->startPtrDeferring.tv_usec)/1000)
          > cl->screen->deferPtrUpdateTime)
      { cl->startPtrDeferring.tv_usec = 0;
        cl->screen->ptrAddEvent(cl->lastPtrButtons,
                                cl->lastPtrX,
                                cl->lastPtrY, cl);
        cl->lastPtrX = -1;
  } } }

  return result;
}

rfbBool rfbIsActive(rfbScreenInfo * screenInfo)
{ return screenInfo->clientHead!=NULL;
}

/*

  void rfbRunEventLoop(rfbScreenInfo * screen, long usec, rfbBool runInBackground)
  { if(runInBackground)
  { rfbErr("Can't run in background, because I don't have PThreads!\n");
    return;
  }

  if(usec<0)
    usec=screen->deferUpdateTime*1000;

  while(rfbIsActive(screen))
    rfbProcessEvents(screen,usec);
  }

*/
void rfbLogPerror( const char * s )
{ fputs( s, stderr );
}


