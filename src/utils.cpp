/*
 * utils.cpp
 *
 *  Created on: May 3, 2020
 *      Author: kict
 */

#include "mjpeg_maker/utils.h"


int mjpeg_maker::ServiceContainer::mLastTypeId = 0;

mjpeg_maker::ServiceContainer * mjpeg_maker::ServiceContainer::Instance() {
	static ServiceContainer serviceContainer;
	return &serviceContainer;
}




