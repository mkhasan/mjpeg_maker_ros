/*
 * mjpeg_maker_node.cpp
 *
 *  Created on: May 1, 2020
 *      Author: kict
 */





#include "mjpeg_maker/mjpeg_maker.h"
#include "client_interface/shm_manager.h"
#include "mjpeg_maker/client_acceptor.h"
#include "spectator/get_str.h"
#include "mjpeg_maker/utils.h"
#include "mjpeg_maker/shm_manager.h"
#include "mjpeg_maker/mjpeg_maker.h"

#include "ros/ros.h"
#include "std_msgs/String.h"


#include <ace/Shared_Memory_SV.h>

#include <iostream>
#include <sstream>
#include <string>
#include <string.h>
#include <assert.h>



using namespace std;
//using namespace client_interface;
using namespace mjpeg_maker;



pthread_t mjpeg_maker_thread;




//extern void * mjpeg_maker::main1(void * ptr);






void mySigintHandler(int sig)
{

	ROS_WARN("Signal %d caught", sig);

	if(sig == SIGINT) {
		ROS_WARN("SIGINT caught");


	}

	mjpeg_maker::finish();
	pthread_join( mjpeg_maker_thread, NULL);



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


	if( ros::console::set_logger_level(ROSCONSOLE_DEFAULT_NAME, ros::console::levels::Info) ) {
	   ros::console::notifyLoggerLevelsChanged();
	}

	string mutexPrefix;
	nh.param("/ROBOT/MUTEX_PREFIX", mutexPrefix, string("kict_mp_camera00_"));


	signal(SIGTERM, mySigintHandler);
	signal(SIGINT, mySigintHandler);          // caught in a different way fo$
	signal(SIGHUP, mySigintHandler);
	signal(SIGKILL, mySigintHandler);
	signal(SIGTSTP, mySigintHandler);


	ros::ServiceClient shm_observer = nh.serviceClient<spectator::get_str>("client_interface/is_shm_initialized");

	spectator::get_str srv;

	int tryCount=0;
	for (tryCount=0; tryCount<10; tryCount++) {
		if (shm_observer.call(srv)) {
			ROS_INFO("is shm initialiaed %s", srv.response.str.c_str());
			if (srv.response.str == "true")
				break;
		}
		else {
			ROS_ERROR("client_interface not found");


		}

		ros::Duration(1.0).sleep();
	}

	if (tryCount == 10) {
		ROS_ERROR("Shm is not initialized");
		return 1;
	}


	int iret = pthread_create( &mjpeg_maker_thread, NULL, mjpeg_maker::mjpeg_maker_func, (void *)&mutexPrefix);
    if(iret) {
        ROS_ERROR("Error in creating cam viewer");
        return -1;
    }


    ServiceContainer::Instance()->set<ShmManager>();
	while (ros::ok()) {


		ros::spinOnce();

		ROS_DEBUG("Waiting....");

		ros::Duration(5.0).sleep();
	}

	return 0;
}


