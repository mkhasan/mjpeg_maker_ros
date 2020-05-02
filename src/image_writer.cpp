/*
 * image_writer.cpp
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */

#include "mjpeg_maker/image_writer.h"
#include "mjpeg_maker/utils.h"

#include <stdio.h>

namespace mjpeg_maker {

ImageWriter::ImageWriter(int _image_width, int _image_height, int _channel)
	: image_width(_image_width)
	, image_height(_image_height)
	, channel(_channel)
	, buffer(NULL)
	, isInitialized(false)

{
	SetMaxDataSize(image_width, image_height, channel);
}

ImageWriter::~ImageWriter() {

	printf("ImageWriter desctuctor \n");
}

void ImageWriter::Initialize() {
	Initialize(image_width, image_height);
}

void ImageWriter::SetMaxDataSize(int w, int h, int ch) {
	max_data_size = w*h*ch+1024;
}

void ImageWriter::SetMaxDataSize(int w, int h) {
	SetMaxDataSize(w, h, channel);
}

int ImageWriter::GetMaxDataSize() const{
	return max_data_size;
}

void ImageWriter::GetDimenstion(int & _image_width, int & _image_height) const {
	_image_width = image_width;
	_image_height = image_height;
}

char * ImageWriter::GetBuffer() {
	return buffer;
}

int ImageWriter::Write(char * src, int stride, int quality) {
	return Write(buffer, src, stride, quality);

}

}
