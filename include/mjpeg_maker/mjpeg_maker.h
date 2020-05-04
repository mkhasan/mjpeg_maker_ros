/*
 * mjpeg_maker.h
 *
 *  Created on: May 1, 2020
 *      Author: kict
 */

#ifndef CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_MJPEG_MAKER_H_
#define CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_MJPEG_MAKER_H_

#include <string>
#include <semaphore.h>
#include "client_interface/shm_manager.h"
#include "client_interface/client_interface.h"

namespace mjpeg_maker {



void * mjpeg_maker_func (void *);
void finish();



}


#endif /* CATKIN_WS_SRC_MJPEG_MAKER_INCLUDE_MJPEG_MAKER_MJPEG_MAKER_H_ */
