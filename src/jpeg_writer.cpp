/*
 * jpeg_writer.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */



#include "mjpeg_maker/jpeg_writer.h"
#include "mjpeg_maker/CStreamer.h"
#include "mjpeg_maker/utils.h"
#include "client_interface/client_interface.h"
#include "mjpeg_maker/shm_manager.h"

#include <Magick++.h>
#include <Magick++/Blob.h>

#include "ros/ros.h"
#include <string.h>




using namespace Magick;

namespace mjpeg_maker {

/* abstructing the c interface */

void
JPEG_Writer::init_JPEG()
{

	/* More stuff */
		/* target file */
  /* Step 1: allocate and initialize JPEG compression object */

	cinfo.err = jpeg_std_error(&jerr);
	/* Now we can initialize the JPEG compression object. */
	jpeg_create_compress(&cinfo);

}


void
JPEG_Writer::finalize_JPEG()
{
	jpeg_destroy_compress(&cinfo);
}

/*
void JPEG_Writer::write_JPEG_file(char * data, AVFrame * curFrame, int quality) {

}

*/

//#include "config.h"


int
JPEG_Writer::write_JPEG_file(unsigned char * dest, unsigned char * src, int stride, int quality)
{
	FILE * outfile;

	//int row_stride;		/* physical row width in image buffer */

	JSAMPROW row_pointer[1];	/* pointer to JSAMPLE row[s] */

	/*
	if ((outfile = fopen(filename, "wb")) == NULL) {
			fprintf(stderr, "can't open %s\n", filename);
			exit(1);
		}

		*/

	int size = GetMaxDataSize();
	memset(dest, 0, size);

	if ((outfile = fmemopen(dest, size, "wb")) == NULL) {
			fprintf(stderr, "can't open %s\n", "mem file");
			exit(1);
		}



	//printf("widht is %d and height is %d \n", image_width, image_height);
	jpeg_stdio_dest(&cinfo, outfile);

	int w = (ShmManager::IMAGE_WIDTH/downSamplingFactor)/8;
	w *= 8;

	cinfo.image_width = w; 	/* image width and height, in pixels */
	cinfo.image_height = image_height/downSamplingFactor;
	cinfo.input_components = channel;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);

	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	//cinfo.scale_num = 1;
	//cinfo.scale_denom = 2;
	jpeg_start_compress(&cinfo, TRUE);

	//row_stride = (image_width/downSamplingFactor) * 3;	/* JSAMPLEs per row in image_buffer */

	int y=0;
	while (cinfo.next_scanline < cinfo.image_height) {
		/* row_pointer[0] = curFrame->data[0]+y*curFrame->linesize[0];////& image_buffer[cinfo.next_scanline * row_stride]; */
		row_pointer[0] = &src[y*stride];////& image_buffer[cinfo.next_scanline * row_stride];
		(void) jpeg_write_scanlines(&cinfo, row_pointer, 1);

		y++;
	}

  /* Step 6: Finish compression */

	jpeg_finish_compress(&cinfo);
	/* After finish_compress, we can close the output file. */

	fseek(outfile,0,SEEK_END);
	size = ftell(outfile);

	//printf("size is %d \n", size);
	fclose(outfile);

	return size;


	/* Step 7: release JPEG compression object */

	/* This is an important step since it will release a good deal of memory. */

}



JPEG_Writer::JPEG_Writer(int _image_width, int _image_height, int _channel, int _downSamplingFactor)
	: ImageWriter(_image_width, _image_height, _channel, _downSamplingFactor)

{

}

JPEG_Writer::~JPEG_Writer() {
	if (isInitialized) {
		Finalize();
	}

	isInitialized = false;
	printf("JPEG_Writer desctuctor \n");
}

void JPEG_Writer::Initialize(int _image_width, int _image_height) {

	if (isInitialized) {
		printf("Already initialized \n");
		return;

	}

	if (_image_width == 0 || _image_height == 0 || channel == 0) {
		THROW(RobotException,"Error in initialization");
	}

	image_width = _image_width;
	image_height = _image_height;

	ROS_DEBUG("w=%d, h=%d, ch=%d", image_width,  image_height, channel);
	SetMaxDataSize(image_width, image_height);


	init_JPEG();
	buffer = new char[GetMaxDataSize()];

	jpegBlob = std::make_unique<Blob>();

	isInitialized = true;
}


void JPEG_Writer::Finalize() {
	if (isInitialized == false)
		return;


	finalize_JPEG();
	delete[] buffer;

	jpegBlob.reset();
	isInitialized = false;
}


int JPEG_Writer::Write(char * dest, char * src, int srcLen, int stride, int quality) {

	static int first = 1;
	if (downSamplingFactor == 1) {
		int len = write_JPEG_file((unsigned char *) dest, (unsigned char *) src, stride, quality);

		if (len <= 0)
			THROW(RobotException, "Input data format error");

		ROS_DEBUG("stride is %d", stride);
		return len;
	}
	else {
		//Blob blob(src, srcLen);

		//Magick::Image image(blob);
		const Magick::Geometry g(ShmManager::IMAGE_WIDTH/downSamplingFactor, ShmManager::IMAGE_HEIGHT/downSamplingFactor);
		//image.resize(g);
		Image image( ShmManager::IMAGE_WIDTH, ShmManager::IMAGE_HEIGHT, "RGB", Magick::CharPixel, src );




		image.resize(g);

		int w = (image_width/downSamplingFactor)/8;
		w *= 8;

		ROS_DEBUG("trimmed width is %d", w);

		static char buf[ShmManager::IMAGE_WIDTH*ShmManager::IMAGE_HEIGHT*3];
		image.write(8, 0, w, ShmManager::IMAGE_HEIGHT/downSamplingFactor, "RGB", Magick::CharPixel, (void *)buf);


		int len = write_JPEG_file((unsigned char *) dest, (unsigned char *) buf, w*3, 0x5e);//jpegBlob->length();
		//memcpy(dest, jpegBlob->data(), len);

		if (first) {
			//image.write("test3.jpg");
			ofstream wf("out.jpg", ios::out | ios::binary);
			wf.write(dest, len);
			wf.close();

			ROS_DEBUG("size is *********************** %d ****************", len);
			first = 0;


		}
		return len;
		/*
		image.write(jpegBlob.get(), "jpg");
		int len = jpegBlob->length();

		//dest = jpegBlob->data();
		 *

		*/
		//static char temp[484*366*3+1024];
		//image.write( 0, 0, image_width/4, image_height/4, "RGB",(const StorageType)0, (void *)temp);

		//memcpy(dest, (char *)jpegBlob->data(), len);
		//int len = write_JPEG_file((unsigned char *) dest, (unsigned char *) temp, 3*484, 0x5e);

		return 0;

	}
}


}
