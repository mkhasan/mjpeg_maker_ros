/*
 * image_writer.h
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */

#ifndef SRC_IMAGE_WRITER_H_
#define SRC_IMAGE_WRITER_H_



namespace mjpeg_maker {


class ImageWriter {
private:
	int max_data_size;

protected:
	int image_width;
	int image_height;
	int channel;
	bool isInitialized;

	char * buffer;

	//StreamSource * streamSource;

public:
	ImageWriter(int image_width=0, int image_heght=0, int channel=3);
	virtual ~ImageWriter();
	virtual void Initialize();
	virtual void Initialize(int image_width, int image_height)=0;
	virtual void Finalize()=0;
	virtual int Write(char * dest, char * src, int stride, int quality)=0;
	virtual int Write(char *src, int stride, int quality);
	void SetMaxDataSize(int w, int h, int ch);
	void SetMaxDataSize(int w, int h);
	int GetMaxDataSize() const;
	void GetDimenstion(int & width, int & height) const;
	char * GetBuffer();
};

}

#endif /* SRC_IMAGE_WRITER_H_ */
