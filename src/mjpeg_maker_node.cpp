/*
 * mjpeg_maker_node.cpp
 *
 *  Created on: May 1, 2020
 *      Author: kict
 */





#include "mjpeg_maker/mjpeg_maker.h"

#include "client_interface/shm_manager.h"

#include "mjpeg_maker/client_acceptor.h"

#include "ros/ros.h"
#include "std_msgs/String.h"


#include <ace/Shared_Memory_SV.h>

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <assert.h>

#include <mjpeg_maker/mjpeg_maker.h>

using namespace std;
using namespace client_interface;


//extern int quit;
//extern ACE_Reactor reactor;
shm_data *pData;

sem_t *mutex;
sem_t *debugLock;

pthread_t cam_viewer_thread;

string mutexPrefix;


//extern void * mjpeg_maker::main1(void * ptr);






void mySigintHandler(int sig)
{

	ROS_WARN("Signal %d caught", sig);

	if(sig == SIGINT) {
		ROS_WARN("SIGINT caught");


	}

	mjpeg_maker::Finish();
	pthread_join( cam_viewer_thread, NULL);



	ros::Duration(1.0).sleep();
	ros::shutdown();
}




int main(int argc, char **argv) {


	ros::init(argc, argv, "mjpeg_maker_node", ros::init_options::NoSigintHandler);

	ros::NodeHandle nh;

	/*
	nh.param("/CAM_VIEWER/delay_on_start", delay_on_start, 0);
	//n.param("/CAM_VIEWER/my_id", id, 0);
	nh.param("/ROBOT/MUTEX_PREFIX", mutexPrefix, string("kict_mp_camera00_"));

	ROS_ERROR("MUTEX PREFIX IS %s", mutexPrefix.c_str());

	*/

	if( ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Debug) ) {
	   ros::console::notifyLoggerLevelsChanged();
	}



	signal(SIGTERM, mySigintHandler);
	signal(SIGINT, mySigintHandler);          // caught in a different way fo$
	signal(SIGHUP, mySigintHandler);
	signal(SIGKILL, mySigintHandler);
	signal(SIGTSTP, mySigintHandler);


	int iret = pthread_create( &cam_viewer_thread, NULL, mjpeg_maker::main1, NULL);
    if(iret) {
        ROS_ERROR("Error in creating cam viewer");
        return -1;
    }

	while (ros::ok()) {


		ros::spinOnce();

		ROS_DEBUG("Waiting....");

		ros::Duration(5.0).sleep();
	}

	return 0;
}


