/*
 * fake_source.h
 *
 *  Created on: Apr 24, 2020
 *      Author: hasan
 */

#ifndef SRC_FAKE_SOURCE_H_
#define SRC_FAKE_SOURCE_H_

#include "stream_source.h"
#include <string>

namespace mjpeg_maker {


class FakeSource : public StreamSource {

public:
	static const int WIDTH = 768;
	static const int HEIGHT = 576;


private:
	const std::string filename = "/home/kict/KICT_MP/vtest.avi";
	bool quit;
	pthread_t tid;

public:
	const int SLEEP_IN_US = 40000;

	struct info_struct {
		bool * quit;
		CStreamer * streamer;
		ImageWriter * writer;
		std::string filename;
	} info;

	FakeSource(CStreamer * streamer,int streamID);
	~FakeSource();
	static void * stream_generator(void * arg);

};

}

#endif /* SRC_FAKE_SOURCE_H_ */
