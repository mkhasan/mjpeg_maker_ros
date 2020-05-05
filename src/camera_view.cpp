/*
 * camera_view.cpp
 *
 *  Created on: May 3, 2020
 *      Author: kict
 */

#include "mjpeg_maker/camera_view.h"
#include "mjpeg_maker/utils.h"
#include "mjpeg_maker/jpeg_writer.h"
#include "mjpeg_maker/shm_manager.h"



#include "ros/ros.h"

namespace mjpeg_maker {

CameraView::CameraView(CStreamer * streamer, int streamID)
	: StreamSource(WIDTH, HEIGHT, streamID, streamer, new JPEG_Writer(WIDTH, HEIGHT, 3, 4))
	//, filename(f_name)
	, quit(false), tid(NULL)
{
	info.quit = &quit;
	info.source = this;


	int err = pthread_create(&tid, NULL, &stream_generator, (void *) &info);
	if (err != 0)
	{
		stringstream ss;
		ss << "can't create thread :[" << err << "]";
		THROW(RobotException, ss.str().c_str());
	}

	printf("CameraView created \n");

}

CameraView::~CameraView() {

	printf("In FakeSource Destructor \n");
	if (tid != NULL && quit == false) {
		quit = true;
		printf("waiting for thread to finish \n");
		pthread_join(tid, NULL);
		printf("done \n");
	}

	delete writer;
}

void * CameraView::stream_generator(void * arg) {

	static int first = 1;
	info_struct *info = (info_struct *) arg;



	int w, h;
	info->source->GetWriter()->GetDimenstion(w, h);
	if (w != WIDTH || h != HEIGHT) {
		THROW(RobotException, "dimension mismatch");
	}

	ROS_DEBUG("CameraView: before data read\n");

	ShmManager * shmManager;
	try {

		info->source->GetWriter()->Initialize();
		shmManager = ServiceContainer::Instance()->get<ShmManager>();
		if (info->source->GetWriter()->GetMaxDataSize() < ShmManager::GetDataSize()) {
			ROS_ERROR("Data sizes (%d vs %d) mismatch ", info->source->GetWriter()->GetMaxDataSize(), ShmManager::GetDataSize());
			throw std::exception();
		}

	}
	catch (exception & e) {
		THROW(RobotException, e.what());
	}

	const int SIZE = ShmManager::IMAGE_WIDTH*ShmManager::IMAGE_HEIGHT*3;
	char * buffer = new char[SIZE+1024];
	while((info->source->GetStreamer()->finished == 0) && *(info->quit) == false) {

		int streamID = info->source->GetStreamID();
		if (streamID >= 0 && streamID < ShmManager::N_COUNT && info->source->GetStreamer()->streamStarted) {
			try {
				//char * p = info->source->GetWriter()->GetBuffer();
				int len;
				shmManager->GetData(streamID, buffer, len);
				ROS_DEBUG("Data len is %d SIZE is %d gap is %d", len, SIZE, (len-SIZE));




				int jpegLen = info->source->GetWriter()->Write(&buffer[len-SIZE], SIZE, 0, 0);
				ROS_DEBUG("Jpeg len is %d", jpegLen);

				if (first) {

					 //ofstream wf("cam_view.jpg", ios::out | ios::binary);
					 //wf.write(info->source->GetWriter()->GetBuffer(), jpegLen);
					 //wf.close();

					first = 0;
				}

				//int w, h;
				//info->source->GetWriter()->GetDimenstion(w, h);
				//int factor = info->source->GetWriter()->GetDownSamplingFactor();
				//w = w/factor;
				//h = h/factor;
				char * p = info->source->GetWriter()->GetBuffer();
				int downSamplingFactor = info->source->GetWriter()->GetDownSamplingFactor();

				int w = (ShmManager::IMAGE_WIDTH/downSamplingFactor)/8;
				w *= 8;
				info->source->GetStreamer()->StreamImage(p, jpegLen, w, (ShmManager::IMAGE_HEIGHT/downSamplingFactor));
				ROS_DEBUG("data copied");
			}
			catch (exception & e) {
				ROS_ERROR("Error in getting data %s", e.what());
				//continue;
			}



		}
	    usleep(40000);

	    //  cout<< "tsleep : "<<tsleep<<" twakeup : "<<twakeup<<" t1 :"<<t1<<endl;

	}

	delete buffer;
	*(info->quit) = true;

	return NULL;
}

}



