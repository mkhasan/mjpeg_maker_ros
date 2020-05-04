/*
 * shm_manager.h
 *
 *  Created on: May 3, 2020
 *      Author: kict
 */

#ifndef CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_SHM_MANAGER_H_
#define CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_SHM_MANAGER_H_



#include <string>
#include <semaphore.h>
#include "client_interface/shm_manager.h"
#include "client_interface/client_interface.h"

namespace mjpeg_maker {

class CameraView;

class ShmManager {

	friend class CameraView;
	friend class JPEG_Writer;
	static const int N_COUNT = 2;
	sem_t *mutex[N_COUNT];
	shm_data * pData[N_COUNT];
	std::string mutexPrefix;
	const std::vector<int> ids {3, 4};
	bool isInitialized;

public:

	static const int IMAGE_WIDTH = 1936;
	static const int IMAGE_HEIGHT = 1464;
	static const int CHANNEL_COUNT = 3;
public:
	static int GetDataSize() {
		return IMAGE_WIDTH*IMAGE_HEIGHT*CHANNEL_COUNT;

	}

	ShmManager();
	void Initialize(const std::string & mutexPrefix);
	void Finalize();
	~ShmManager();
	void GetData(int index, char * data, int & dataLen);

};

}

#endif /* CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_SHM_MANAGER_H_ */
