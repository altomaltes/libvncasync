
int              ScaleX( rfbScreenInfoPtr from, rfbScreenInfoPtr to, int x);
int              ScaleY( rfbScreenInfoPtr from, rfbScreenInfoPtr to, int y);
void             rfbScaledCorrection(        rfbScreenInfoPtr from, rfbScreenInfoPtr to, int *x, int *y, int *w, int *h, const char *function);
void             rfbScaledScreenUpdateRect(  rfbScreenInfoPtr screen, rfbScreenInfoPtr ptr, int x0, int y0, int w0, int h0);
void             rfbScaledScreenUpdate(      rfbScreenInfoPtr screen, int x1, int y1, int x2, int y2);

rfbScreenInfoPtr rfbScaledScreenAllocate( rfbClient *, int width, int height);
rfbScreenInfoPtr rfbScalingFind(          rfbClient *, int width, int height);
void             rfbScalingSetup(         rfbClient *, int width, int height);
int              rfbSendNewScaleSize(     rfbClient * );
