/*
 * shm_manager.cpp
 *
 *  Created on: May 3, 2020
 *      Author: kict
 */




#include "mjpeg_maker/shm_manager.h"

#include "ros/ros.h"

#include <sstream>

using namespace std;

namespace mjpeg_maker {

ShmManager::ShmManager() : isInitialized(false)
{

}

ShmManager::~ShmManager() {
	if(isInitialized) {
		Finalize();
	}
}

void ShmManager::Initialize(const std::string & _mutexPrefix) {
	mutexPrefix = _mutexPrefix;

	ROS_DEBUG("mutex prefix is %s", _mutexPrefix.c_str());
	if (ids.size() < N_COUNT) {
		THROW(RobotException, "ids size is too small");

	}

	string mutexName;

	for (int k=0; k<ids.size(); ++k){

		stringstream ss;
		ss << ids[k];
		mutexName = mutexPrefix+ss.str();

		sem_t * m = sem_open(mutexName.c_str(), O_RDWR);
		if (m == SEM_FAILED) {
			stringstream msg;
			msg << "Error in initializing mutex " << ids[k] << " mutex name " << mutexName;
			THROW(RobotException, msg.str().c_str());
		}

		mutex[k] = m;

	    ACE_Shared_Memory_SV shm_client(client_interface::ShmManager::SHM_KEY_START+ids[k], sizeof (shm_data));

	    char *shm = (char *) shm_client.malloc ();

	    if (shm == NULL) {
	    	sem_close(m);

			stringstream msg;
			msg << "Error in initializing shm " << ids[k];
			THROW(RobotException, msg.str().c_str());
	    }

	    pData[k] = new (shm) shm_data;


	}

	isInitialized = true;
	ROS_DEBUG("ShmManager::Initilize done");
}

void ShmManager::Finalize() {

	if (!isInitialized)
		return;

    for (unsigned int k = 0; k < N_COUNT; k++)
    {
		sem_close(mutex[k]);
    }

    isInitialized = false;
    ROS_DEBUG("ShmManager::Finalize done");

}

void ShmManager::GetData(int index, char * data, int & dataLen){

	if (index < 0 || index > N_COUNT) {
		THROW(RobotException, "Index out of range");
	}

	int len=0;
	sem_wait(mutex[index]);

	len = pData[index]->len;
	ROS_DEBUG("data len is %d", len);

	if (len < IMAGE_WIDTH*IMAGE_HEIGHT*CHANNEL_COUNT || pData[index]->width != IMAGE_WIDTH || pData[index]->height != IMAGE_HEIGHT) {

		sem_post(mutex[index]);
		len = -1;
	}
	else {

		memcpy(data, pData[index]->data, pData[index]->len);
		sem_post(mutex[index]);
	}

	if (len < 0) {
		THROW(RobotException, "Data format error");
	}

	dataLen = len;


}

}
