
int              ScaleX                   ( ScreenAtom * from, ScreenAtom * to, int x);
int              ScaleY                   ( ScreenAtom * from, ScreenAtom * to, int y);
void             rfbScaledCorrection      ( ScreenAtom * from, ScreenAtom * to, int *x, int *y, int *w, int *h, const char *function);
void             rfbScaledScreenUpdateRect( ScreenAtom * screen, ScreenAtom * ptr, int x0, int y0, int w0, int h0);
void             rfbScaledScreenUpdate    ( rfbScreenInfo * screen, int x1, int y1, int x2, int y2);

ScreenAtom * rfbScaledScreenAllocate( rfbClient *, int width, int height);
ScreenAtom * rfbScalingFind         ( rfbClient *, int width, int height);
void            rfbScalingSetup        ( rfbClient *, int width, int height);
int             rfbSendNewScaleSize    ( rfbClient * );
