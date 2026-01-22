#ifndef RFB_PRIVATE_H
#define RFB_PRIVATE_H

/* from cursor.c */

void rfbShowCursor(rfbClient * cl);
void rfbHideCursor(rfbClient * cl);
void rfbRedrawAfterHideCursor(rfbClient * cl,sraRegionPtr updateRegion);

/* from main.c */

rfbClient * rfbClientIteratorHead(rfbClientIteratorPtr i);

/* from tight.c */

#ifdef HAVE_LIBZ
#ifdef HAVE_LIBJPEG
extern void rfbTightCleanup(rfbScreenInfo * screen);
#endif

/* from zlib.c */
extern void rfbZlibCleanup(rfbScreenInfo * screen);

/* from zrle.c */
void rfbFreeZrleData(rfbClient * cl);

#endif


/* from ultra.c */

extern void rfbFreeUltraData(rfbClient * cl);

#endif

