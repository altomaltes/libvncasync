/* Functions to compute MD5 message digest of files or memory blocks.
   according to the definition of MD5 in RFC 1321 from April 1992.
   Copyright (C) 1995,1996,1997,1999,2000,2001,2005
    Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

/* Written by Ulrich Drepper <drepper@gnu.ai.mit.edu>, 1995.  */


#include <rfb/rfb.h>

#ifndef WIN32
  #include <errno.h>  
#endif

/*
 * rfbProcessClientProtocolVersion is called when the client sends its
 * protocol version.
 */


void rfbLogPerror(const char *str)
{
#ifdef WIN32
//    wchar_t *s = NULL;
//    FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
//                   NULL, errno, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
//                   (LPWSTR)&s, 0, NULL);
//    rfbErr("%s: %S\n", str, s);
//    LocalFree(s);
#else
    rfbErr("%s: %s\n", str, strerror(errno));
#endif
}

void rfbCloseClient( rfbClientPtr cl )
{ cl->bytesLeft= 0;                      /* Kill stream */ 
}

void * getStreamBytes( rfbClientPtr cl, size_t sz )
{ char * oldPtr= cl->recvPtr;

  if ( cl->bytesLeft < sz  )
  { rfbLogPerror("rfbProcessClientProtocolVersion: read");
    rfbCloseClient( cl );
    return( NULL );
  }
  cl->bytesLeft-= sz; cl->recvPtr+= sz; // Step pointer 
  return( oldPtr );                     // Received stream
}

int WriteToRFBServer( rfbClientPtr * cl, char * c, int len ) 
{
}




