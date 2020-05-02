/*
 * jpeg_writer.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */



#include "mjpeg_maker/jpeg_writer.h"
#include "mjpeg_maker/CStreamer.h"
#include "mjpeg_maker/utils.h"

#include <string.h>


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

	int row_stride;		/* physical row width in image buffer */

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

	cinfo.image_width = image_width; 	/* image width and height, in pixels */
	cinfo.image_height = image_height;
	cinfo.input_components = 3;		/* # of color components per pixel */
	cinfo.in_color_space = JCS_RGB; 	/* colorspace of input image */

	jpeg_set_defaults(&cinfo);

	jpeg_set_quality(&cinfo, quality, TRUE /* limit to baseline-JPEG values */);

	//cinfo.scale_num = 1;
	//cinfo.scale_denom = 2;
	jpeg_start_compress(&cinfo, TRUE);

	row_stride = image_width * 3;	/* JSAMPLEs per row in image_buffer */

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



JPEG_Writer::JPEG_Writer(int _image_width, int _image_height)
	: ImageWriter(_image_width, _image_height)

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
		THROW(mjpeg_maker::RobotException,"Error in initialization");
	}

	image_width = _image_width;
	image_height = _image_height;

	SetMaxDataSize(image_width, image_height);

	init_JPEG();

	buffer = new char[GetMaxDataSize()];
	isInitialized = true;
}


void JPEG_Writer::Finalize() {
	if (isInitialized == false)
		return;
	finalize_JPEG();
	delete[] buffer;
	isInitialized = false;
}

int JPEG_Writer::Write(char * dest, char * src, int stride, int quality) {
	int len = write_JPEG_file((unsigned char *) dest, (unsigned char *) src, stride, quality);

	if (len <= 0)
		THROW(mjpeg_maker::RobotException, "Input data format error");
	return len;
}

void JPEG_Writer::GetInfo(char * data, int data_len, int & width, int & height, int & payloadIndex) {
	int i;

	//ifstream src(filename.c_str(), ios::binary);

	//src.read(buf, 2);

	const char * buf = &data[0];
	const char * p = buf;

	if (!CStreamer::IsSOI(buf[0], buf[1]))
		THROW(mjpeg_maker::RobotException, "Error in getting info");



	buf += 2;
	//src.read(buf, 4);

	if (buf - data > data_len)
		THROW(mjpeg_maker::RobotException, "Error in getting info");


	if(CStreamer::IsAPP0(buf[0], buf[1]))
	{
		int len = CStreamer::GetLength(buf[2], buf[3]);
		if (len < 0)
			THROW(mjpeg_maker::RobotException, "Error in getting info");
		//cout << "APP0 length is " << len << endl;
		buf += 4;

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//src.read(buf, len-2);
		buf += len-2;

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");


		//src.read(buf, 4);

	}


	i = 0;
	while(CStreamer::IsDQT(buf[0], buf[1]))
	{
		int len = CStreamer::GetLength(buf[2], buf[3]);
		if (len < 0)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//cout << "DQT-" << i << " length is " << len << endl;
		buf += 4;

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//src.read(buf, len-2);

		buf += len-2;
		//src.read(buf, 4);
		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		i++;

	}


	if (i == 0)
		THROW(mjpeg_maker::RobotException, "Error in getting info");


	if(CStreamer::IsSOF(buf[0], buf[1]))
	{

		int len = CStreamer::GetLength(buf[2], buf[3]);
		if (len < 0)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//cout << "SOF length is " << len << endl;
		buf += 4;

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//src.read(buf, len-2);

		int val;
		p = buf;
		p++;

		val = (int)*p & 0xff;

		val = val << 8 | ((int)*(p+1) & 0xff);
		//cout << "height is " << val << endl;
		height = val;
		p+= 2;

		val = (int)*p & 0xff;

		val = val << 8 | ((int)*(p+1) & 0xff);
		//cout << "width is " << val << endl;
		width = val;


		buf += len-2;
		//src.read(buf, 4);

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");


	}
	else
		THROW(mjpeg_maker::RobotException, "Error in getting info");

	i = 0;
	while(CStreamer::IsDHT(buf[0], buf[1]))
	{
		int len = CStreamer::GetLength(buf[2], buf[3]);
		if (len < 0)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//cout << "DHT-" << i << " length is " << len << endl;
		buf += 4;

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//src.read(buf, len-2);

		buf += len-2;
		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//src.read(buf, 4);

		i++;

	}

	if (i == 0)
		THROW(mjpeg_maker::RobotException, "Error in getting info");

	if(CStreamer::IsSOS(buf[0], buf[1]))
	{

		int len = CStreamer::GetLength(buf[2], buf[3]);
		if (len < 0)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//cout << "SOS length is " << len << endl;

		buf += 4;

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");

		//src.read(buf, len-2);

		buf += len-2;

		if (buf - data > data_len)
			THROW(mjpeg_maker::RobotException, "Error in getting info");


	}
	else
		THROW(mjpeg_maker::RobotException, "Error in getting info");

/*
	cout << "last 4 bytes are: " << hex << ((int)buf[0] & 0xff) << " " << ((int)buf[1] & 0xff) << " "
			<< ((int)buf[2] & 0xff) << " " <<  ((int)buf[3] & 0xff) << endl;
			*/



	payloadIndex = buf - data;
	if (payloadIndex <= 0)
		THROW(mjpeg_maker::RobotException, "Error in getting info");

}

}
