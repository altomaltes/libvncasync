#ifdef __STRICT_ANSI__
#define _BSD_SOURCE
#endif
#include <time.h>
#include <stdarg.h>
#include <rfb/rfb.h>
#include <rfb/rfbclient.h>

#ifndef HAVE_LIBPTHREAD
#error This test need pthread support (otherwise the client blocks the client)
#endif

#define ALL_AT_ONCE
/*#define VERY_VERBOSE*/


typedef struct { int id; char* str; } encoding_t;
static encoding_t testEncodings[]={
        { rfbEncodingRaw, "raw" },
	{ rfbEncodingRRE, "rre" },
	{ rfbEncodingCoRRE, "corre" },
	{ rfbEncodingHextile, "hextile" },
	{ rfbEncodingUltra, "ultra" },
#ifdef HAVE_LIBZ
	{ rfbEncodingZlib, "zlib" },
	{ rfbEncodingZlibHex, "zlibhex" },
	{ rfbEncodingZRLE, "zrle" },
	{ rfbEncodingZYWRLE, "zywrle" },
#ifdef HAVE_LIBJPEG
	{ rfbEncodingTight, "tight" },
#endif
#endif
	{ 0, NULL }
};

#define NUMBER_OF_ENCODINGS_TO_TEST (sizeof(testEncodings)/sizeof(encoding_t)-1)
/*#define NUMBER_OF_ENCODINGS_TO_TEST 1*/

/* Here come the variables/functions to handle the test output */

static const int width=400,height=300;
static unsigned int statistics[2][NUMBER_OF_ENCODINGS_TO_TEST];
static unsigned int totalFailed,totalCount;
static unsigned int countGotUpdate;

static void initStatistics(void) {
	memset(statistics[0],0,sizeof(int)*NUMBER_OF_ENCODINGS_TO_TEST);
	memset(statistics[1],0,sizeof(int)*NUMBER_OF_ENCODINGS_TO_TEST);
	totalFailed=totalCount=0;
}


static void updateStatistics(int encodingIndex,rfbBool failed) {
	if(failed) {
		statistics[1][encodingIndex]++;
		totalFailed++;
	}
	statistics[0][encodingIndex]++;
	totalCount++;
	countGotUpdate++;
}

/* Here begin the functions for the client. They will be called in a
 * pthread. */

/* maxDelta=0 means they are expected to match exactly;
 * maxDelta>0 means that the average difference must be lower than maxDelta */
static rfbBool doFramebuffersMatch(rfbScreenInfo* server,rfbClient* client,
		int maxDelta)
{
	int i,j,k;
	unsigned int total=0,diff=0;
	if(server->width!=client->width || server->height!=client->height)
		return FALSE;
	/* TODO: write unit test for colour transformation, use here, too */
	for(i=0;i<server->width;i++)
		for(j=0;j<server->height;j++)
			for(k=0;k<3/*server->serverFormat.bitsPerPixel/8*/;k++) {
				unsigned char s=server->frameBuffer[k+i*4+j*server->paddedWidthInBytes];
				unsigned char cl=client->frameBuffer[k+i*4+j*client->width*4];

				if(maxDelta==0 && s!=cl) {
					return FALSE;
				} else {
					total++;
					diff+=(s>cl?s-cl:cl-s);
				}
			}
	if(maxDelta>0 && diff/total>=maxDelta)
		return FALSE;
	return TRUE;
}

static rfbBool resize(rfbClient* cl) {
	if(cl->frameBuffer)
		free(cl->frameBuffer);
	cl->frameBuffer=malloc(cl->width*cl->height*cl->format.bitsPerPixel/8);
	if(!cl->frameBuffer)
		return FALSE;
	SendFramebufferUpdateRequest(cl,0,0,cl->width,cl->height,FALSE);
	return TRUE;
}

typedef struct clientData {
	int encodingIndex;
	rfbScreenInfo* server;
	char* display;
} clientData;

static void update(rfbClient* client,int x,int y,int w,int h) {
#ifndef VERY_VERBOSE

	static const char* progress="|/-\\";
	static int counter=0;

	if(++counter>sizeof(progress)) counter=0;
	fprintf(stderr,"%c\r",progress[counter]);
#else
	clientData* cd=(clientData*)client->clientData;
	rfbClientLog("Got update (encoding=%s): (%d,%d)-(%d,%d)\n",
			testEncodings[cd->encodingIndex].str,
			x,y,x+w,y+h);
#endif
}

static void update_finished(rfbClient* client) {
	clientData* cd=(clientData*)client->clientData;
        int maxDelta=0;

#ifdef HAVE_LIBZ
	if(testEncodings[cd->encodingIndex].id==rfbEncodingZYWRLE)
		maxDelta=5;
#ifdef HAVE_LIBJPEG
	if(testEncodings[cd->encodingIndex].id==rfbEncodingTight)
		maxDelta=5;
#endif
#endif
	updateStatistics(cd->encodingIndex,
			!doFramebuffersMatch(cd->server,client,maxDelta));
}


static void* clientLoop(void* data) {
	rfbClient* client=(rfbClient*)data;
	clientData* cd=(clientData*)client->clientData;

	client->appData.encodingsString=strdup(testEncodings[cd->encodingIndex].str);
	client->appData.qualityLevel = 7; /* ZYWRLE fails the test with standard settings */

	sleep(1);
	rfbClientLog("Starting client (encoding %s, display %s)\n",
			testEncodings[cd->encodingIndex].str,
			cd->display);
	if(!rfbInitClient(client,NULL,NULL)) {
		rfbClientErr("Had problems starting client (encoding %s)\n",
				testEncodings[cd->encodingIndex].str);
		updateStatistics(cd->encodingIndex,TRUE);
		return NULL;
	}
//	while(1) {
	//	if(WaitForMessage(client,50)>=0)
//			if(!HandleRFBServerMessage(client))
//				break;
//	}
	free(((clientData*)client->clientData)->display);
	free(client->clientData);
	client->clientData = NULL;
	if(client->frameBuffer)
		free(client->frameBuffer);
	rfbClientCleanup(client);
	return NULL;
}

static pthread_t all_threads[NUMBER_OF_ENCODINGS_TO_TEST];
static int thread_counter;

static void startClient(int encodingIndex,rfbScreenInfo* server) {
	rfbClient* client=rfbGetClient(8,3,4);
	clientData* cd;

	client->clientData=malloc(sizeof(clientData));
	client->MallocFrameBuffer=resize;
	client->GotFrameBufferUpdate=update;
	client->FinishedFrameBufferUpdate=update_finished;

	cd=(clientData*)client->clientData;
	cd->encodingIndex=encodingIndex;
	cd->server=server;
	cd->display=(char*)malloc(6);
//	sprintf(cd->display,":%d",server->port-5900);

	pthread_create(&all_threads[thread_counter++],NULL,clientLoop,(void*)client);
}

/* Here begin the server functions */

static void idle(rfbScreenInfo* server)
{
	int c;
	rfbBool goForward;

#ifdef ALL_AT_ONCE
	goForward=(countGotUpdate==NUMBER_OF_ENCODINGS_TO_TEST);
#else
	goForward=(countGotUpdate==1);
#endif

	if(!goForward)
	  return;
	countGotUpdate=0;

	{
		int i,j;
		int x1=(rand()%(server->width-1)),x2=(rand()%(server->width-1)),
		y1=(rand()%(server->height-1)),y2=(rand()%(server->height-1));
		if(x1>x2) { i=x1; x1=x2; x2=i; }
		if(y1>y2) { i=y1; y1=y2; y2=i; }
		x2++; y2++;
		for(c=0;c<3;c++) {
			for(i=x1;i<x2;i++)
				for(j=y1;j<y2;j++)
					server->frameBuffer[i*4+c+j*server->paddedWidthInBytes]=255*(i-x1+j-y1)/(x2-x1+y2-y1);
		}
		rfbMarkRectAsModified(server,x1,y1,x2,y2);

#ifdef VERY_VERBOSE
		rfbLog("Sent update (%d,%d)-(%d,%d)\n",x1,y1,x2,y2);
#endif
	}
}

/* log function (to show what messages are from the client) */

static void
rfbTestLog(const char *format, ...)
{
	va_list args;
	char buf[256];
	time_t log_clock;

	if(!rfbEnableClientLogging)
		return;

	va_start(args, format);

	time(&log_clock);
	strftime(buf, 255, "%d/%m/%Y %X (client) ", localtime(&log_clock));
	fprintf(stderr,"%s",buf);

	vfprintf(stderr, format, args);
	fflush(stderr);

	va_end(args);
}

/* the main function */

int main(int argc,char** argv)
{
	int i,j;
	time_t t;
	rfbScreenInfoPtr server;

	rfbClientLog=rfbTestLog;
	rfbClientErr=rfbTestLog;

	/* Initialize server */
	server=rfbGetScreen(&argc,argv,width,height,8,3,4);
        if(!server)
          return 0;

	server->frameBuffer=malloc(400*300*4);
	server->cursor=NULL;
	for(j=0;j<400*300*4;j++)
		server->frameBuffer[j]=j;
	rfbInitServer(server);
	rfbProcessEvents(server,0);

	initStatistics();

#ifndef ALL_AT_ONCE
	for(i=0;i<NUMBER_OF_ENCODINGS_TO_TEST;i++) {
#else
	/* Initialize clients */
	for(i=0;i<NUMBER_OF_ENCODINGS_TO_TEST;i++)
#endif
		startClient(i,server);

	t=time(NULL);
	/* test 20 seconds */
	while(time(NULL)-t<20) {

		idle(server);

		rfbProcessEvents(server,1);
	}
	rfbLog("%d failed, %d received\n",totalFailed,totalCount);
#ifndef ALL_AT_ONCE
	{
		rfbClientPtr cl;
		rfbClientIteratorPtr iter=rfbGetClientIterator(server);
		while((cl=rfbClientIteratorNext(iter)))
			rfbCloseClient(cl);
		rfbReleaseClientIterator(iter);
	}
	}
#endif

	/* shut down server, disconnecting all clients */
	rfbShutdownServer(server, TRUE);

	for(i=0;i<thread_counter;i++)
		pthread_join(all_threads[i], NULL);

	free(server->frameBuffer);
	rfbScreenCleanup(server);

	rfbLog("Statistics:\n");
	for(i=0;i<NUMBER_OF_ENCODINGS_TO_TEST;i++)
		rfbLog("%s encoding: %d failed, %d received\n",
				testEncodings[i].str,statistics[1][i],statistics[0][i]);
	if(totalFailed)
		return 1;
	return(0);
}


