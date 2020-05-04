/*
 * camera_view.h
 *
 *  Created on: May 3, 2020
 *      Author: kict
 */

#ifndef CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_CAMERA_VIEW_H_
#define CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_CAMERA_VIEW_H_

#include "stream_source.h"
#include "shm_manager.h"

namespace mjpeg_maker {


class CameraView : public StreamSource {

public:
	static const int WIDTH = ShmManager::IMAGE_WIDTH;
	static const int HEIGHT = ShmManager::IMAGE_HEIGHT;


private:

	bool quit;
	pthread_t tid;

public:
	const int SLEEP_IN_US = 40000;

	struct info_struct {
		bool * quit;
		StreamSource * source;
	} info;

	CameraView(CStreamer * streamer,int streamID);
	~CameraView();
	static void * stream_generator(void * arg);

};

}


#endif /* CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_CAMERA_VIEW_H_ */
