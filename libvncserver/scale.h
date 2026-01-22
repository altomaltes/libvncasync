
int              ScaleX                   ( rfbScreenInfo * from, rfbScreenInfo * to, int x);
int              ScaleY                   ( rfbScreenInfo * from, rfbScreenInfo * to, int y);
void             rfbScaledCorrection      ( rfbScreenInfo * from, rfbScreenInfo * to, int *x, int *y, int *w, int *h, const char *function);
void             rfbScaledScreenUpdateRect( rfbScreenInfo * screen, rfbScreenInfo * ptr, int x0, int y0, int w0, int h0);
void             rfbScaledScreenUpdate    ( rfbScreenInfo * screen, int x1, int y1, int x2, int y2);

rfbScreenInfo * rfbScaledScreenAllocate( rfbClient *, int width, int height);
rfbScreenInfo * rfbScalingFind         ( rfbClient *, int width, int height);
void             rfbScalingSetup       ( rfbClient *, int width, int height);
int              rfbSendNewScaleSize   ( rfbClient * );
