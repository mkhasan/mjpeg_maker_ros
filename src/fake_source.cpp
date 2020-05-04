/*
 * fake_source.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */

#include "mjpeg_maker/utils.h"
#include "mjpeg_maker/fake_source.h"
#include "mjpeg_maker/jpeg_writer.h"

#include <pthread.h>
#include <unistd.h>

#ifdef __cplusplus

extern "C" {
#endif

#include <jpeglib.h>



#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>



#ifdef __cplusplus
}
#endif

#include "client_interface/client_interface.h"

#include <assert.h>


using namespace std;

namespace mjpeg_maker {



FakeSource::FakeSource(CStreamer * streamer, int streamID)
	: StreamSource(WIDTH, HEIGHT, streamID, streamer, new JPEG_Writer(WIDTH, HEIGHT))
	//, filename(f_name)
	, quit(false), tid(NULL)
{
	info.quit = &quit;
	info.streamer = streamer;
	info.writer = writer;
	info.filename = filename;

	int err = pthread_create(&tid, NULL, &stream_generator, (void *) &info);
	if (err != 0)
	{
		stringstream ss;
		ss << "can't create thread :[" << err << "]";
		THROW(RobotException, ss.str().c_str());
	}

	printf("FakeSource created \n");

}

FakeSource::~FakeSource() {

	printf("In FakeSource Destructor \n");
	if (tid != NULL && quit == false) {
		quit = true;
		printf("waiting for thread to finish \n");
		pthread_join(tid, NULL);
		printf("done \n");
	}

	delete writer;
}


void * FakeSource::stream_generator(void * arg) {
	AVFormatContext *pFormatCtx = NULL;
	int             i, videoStream;
	AVCodecContext  *pCodecCtx = NULL;
	AVCodec         *pCodec = NULL;
	AVFrame         *pFrame = NULL;
	AVFrame         *pFrameRGB = NULL;
	AVPacket        packet;
	int             frameFinished;
	int             numBytes;
	uint8_t         *buffer = NULL;

	AVDictionary    *optionsDict = NULL;
	struct SwsContext      *sws_ctx = NULL;


	info_struct *info = (info_struct *) arg;

	av_register_all();

	if(avformat_open_input(&pFormatCtx, info->filename.c_str(), NULL, NULL)!=0) {
		fprintf(stderr, "Couldn't open file \n");
	    return NULL;
	}

	// Retrieve stream information
	if(avformat_find_stream_info(pFormatCtx, NULL)<0) {
		fprintf(stderr, "Couldn't find stream information \n");
		return NULL;
	}

	av_dump_format(pFormatCtx, 0, info->filename.c_str(), 0);


	videoStream=-1;
	for(i=0; i<pFormatCtx->nb_streams; i++) {
		if(pFormatCtx->streams[i]->codec->codec_type==AVMEDIA_TYPE_VIDEO) {
			videoStream=i;
			break;
	    }
	}

	if(videoStream==-1) {
		fprintf(stderr, "Didn't find a video stream \n");
		return NULL;
	}


	pCodecCtx=pFormatCtx->streams[videoStream]->codec;


	pCodec=avcodec_find_decoder(pCodecCtx->codec_id);
	if(pCodec==NULL) {
		fprintf(stderr, "Unsupported codec \n");
		return NULL; // Codec not found
	}
	// Open codec
	if(avcodec_open2(pCodecCtx, pCodec, &optionsDict)<0) {
		fprintf(stderr, "Could not open codec \n");
		return NULL;
	}

	// Allocate video frame
	pFrame=av_frame_alloc();

	// Allocate an AVFrame structure
	pFrameRGB=av_frame_alloc();
	if(pFrameRGB==NULL) {
		fprintf(stderr, "Could not get frame \n");
		return NULL;
	}

	// Determine required buffer size and allocate buffer
	numBytes=avpicture_get_size(AV_PIX_FMT_RGB24, pCodecCtx->width,
				  pCodecCtx->height);
	buffer=(uint8_t *)av_malloc(numBytes*sizeof(uint8_t));

	sws_ctx =
			sws_getContext
			(
				pCodecCtx->width,
				pCodecCtx->height,
				pCodecCtx->pix_fmt,
				pCodecCtx->width,
				pCodecCtx->height,
				AV_PIX_FMT_RGB24,
				SWS_BILINEAR,
				NULL,
				NULL,
				NULL
			);

	// Assign appropriate parts of buffer to image planes in pFrameRGB
	// Note that pFrameRGB is an AVFrame, but AVFrame is a superset
	// of AVPicture

	avpicture_fill((AVPicture *)pFrameRGB, buffer, AV_PIX_FMT_RGB24,
		 pCodecCtx->width, pCodecCtx->height);

	// Read frames and save first five frames to disk

	//init_JPEG();


	int w, h;
	info->writer->GetDimenstion(w, h);

	if (!(w == pCodecCtx->width && h == pCodecCtx->height)) {
		printf("(%d x %d) \n", pCodecCtx->width, pCodecCtx->height);
		assert(w == pCodecCtx->width && h == pCodecCtx->height);
	}
	printf("before data read\n");



	info->writer->Initialize();

	while(av_read_frame(pFormatCtx, &packet)>=0 && (info->streamer->finished == 0) && *(info->quit) == false) {

		//printf("data read\n");
	    // Is this a packet from the video stream?
	    if(packet.stream_index==videoStream) {
	      // Decode video frame
	    	avcodec_decode_video2(pCodecCtx, pFrame, &frameFinished,
				   &packet);

			// Did we get a video frame?
			if(frameFinished) {
	    	  // Convert the image from its native format to RGB
				sws_scale
				(
					sws_ctx,
					(uint8_t const * const *)pFrame->data,
					pFrame->linesize,
					0,
					pCodecCtx->height,
					pFrameRGB->data,
					pFrameRGB->linesize
				);


				if(info->streamer->streamStarted && *(info->quit) == false)
				{
					//SaveFrame(pFrameRGB, pCodecCtx->width, pCodecCtx->height,
						//1, info->streamer->data, (int) info->streamer->GetQualityFactor(), info->writer);

					int data_len = info->writer->Write((char *)pFrameRGB->data[0], pCodecCtx->width*pCodecCtx->height*3, pFrameRGB->linesize[0], (int) info->streamer->GetQualityFactor());


					printf("width %d height %d stride %d: ", pCodecCtx->width, pCodecCtx->height, pFrameRGB->linesize[0]);
					//int w, h, offset;
					//JPEG_Writer::GetInfo(info->writer->GetBuffer(), data_len, w, h, offset);
					//assert(w == pCodecCtx->width && h == pCodecCtx->height);

					char * p = info->writer->GetBuffer();
					info->streamer->StreamImage(info->writer->GetBuffer(), data_len, pCodecCtx->width, pCodecCtx->height);
					//info->streamer->StreamImage(&p[offset], data_len-offset, w, h);

					//printf("sent \n");


				}
			}

			//printf("value of index %d \n", i++);
	    }

	    // Free the packet that was allocated by av_read_frame
	    av_free_packet(&packet);
	    usleep(40000);

	    //  cout<< "tsleep : "<<tsleep<<" twakeup : "<<twakeup<<" t1 :"<<t1<<endl;

	}

	*(info->quit) = true;

	return NULL;
}


void SaveFrame(AVFrame *pFrame, int width, int height, int iFrame, char * data, int qualityFactor, ImageWriter * writer) {
  FILE *pFile;
  char szFilename[32];
  int  y;


  if (iFrame == 1)
  {
	  //curFrame = pFrame;
	  //image_height = height;
	  //image_width = width;
	  //write_JPEG_file(data, qualityFactor);
	  //writer->Write(data, (char *)pFrame->data[0], pFrame->linesize[0], qualityFactor);

  }

}

}
