/*
 * jpeg_writer.h
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */

#ifndef SRC_JPEG_WRITER_H_
#define SRC_JPEG_WRITER_H_



#include <stdlib.h>
#include <stdio.h>
#include <memory>

#ifdef __cplusplus
extern "C" {
#endif

#include <jpeglib.h>


#ifdef __cplusplus
}
#endif

#include "image_writer.h"


namespace Magick {
class Blob;
}
namespace mjpeg_maker {


class JPEG_Writer : public ImageWriter {


	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr jerr;

	void init_JPEG();
	void finalize_JPEG();
	//void write_JPEG_file(char * data, AVFrame * curFrame, int quality);
	int write_JPEG_file(unsigned char * dest, unsigned char * src, int stride, int quality);


	std::unique_ptr<Magick::Blob> jpegBlob;

	char * temp;
public:

	JPEG_Writer(int image_width, int image_height, int channel=3, int downSamplingFactor = 1);
	~JPEG_Writer();

	void Initialize(int image_width, int image_height);
	void Finalize();

	//void Write(char * dest, AVFrame * src, int quality);
	int Write(char * dest, char * src, int srcLen, int stride, int quality);


};

}

#endif /* SRC_JPEG_WRITER_H_ */
