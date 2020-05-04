cmake_minimum_required(VERSION 2.8.3)

project(mjpeg_maker)

add_compile_options(-std=c++14)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -ggdb -Wall -D_GNU_SOURCE=1 -D_REENTRANT")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall -D_GNU_SOURCE=1 -D_REENTRANT")

find_package(catkin REQUIRED COMPONENTS
  roscpp
  rospy
  std_msgs
  client_interface
)

generate_messages(
   DEPENDENCIES
   std_msgs
 )




include_directories(
  include
  /usr/local/include
  /usr/local/include/ImageMagick-7
  ${catkin_INCLUDE_DIRS}
)

find_library(JPEG_LIB jpeg)
find_library(AVUTIL avutil HINTS $ENV{FFMPEG_ROOT}/libavutil)
#find_library(SWRESAMPLE swresample HINTS $ENV{FFMPEG_ROOT}/libswresample)
find_library(AVCODEC avcodec HINTS $ENV{FFMPEG_ROOT}/libavcodec)
find_library(AVFORMAT avformat HINTS $ENV{FFMPEG_ROOT}/libavformat)
find_library(SWSCALE swscale HINTS $ENV{FFMPEG_ROOT}/libswscale)
#find_library(AVFILTER avfilter HINTS $ENV{FFMPEG_ROOT}/libavfilter)
#find_library(AVDEVICE avdevice HINTS $ENV{FFMPEG_ROOT}/libavdevice)
find_library(MAGIC_HDRI Magick++-7.Q16HDRI HINTS "/usr/local/lib")
find_library(ACE ACE)


set ( MJPEG_MAKER_SRC
	
	src/${PROJECT_NAME}.cpp
	src/client_handler.cpp
	src/CStreamer.cpp
	src/CRtspSession.cpp
	src/image_writer.cpp
	src/jpeg_writer.cpp
	src/stream_source.cpp
	src/fake_source.cpp
	src/shm_manager.cpp
	src/camera_view.cpp
	src/utils.cpp
	
)			

add_library(${PROJECT_NAME}
  
 	${MJPEG_MAKER_SRC}
)

catkin_package(
   INCLUDE_DIRS include
   LIBRARIES mjpeg_maker
   CATKIN_DEPENDS roscpp rospy std_msgs client_interface LIBRARIES
)

add_dependencies(${PROJECT_NAME} ${catkin_EXPORTED_TARGETS})
 
target_link_libraries(${PROJECT_NAME} ${catkin_LIBRARIES} ${AVCODEC} ${AVFORMAT} ${AVUTIL} ${SWSCALE} ${JPEG_LIB} ${MAGIC_HDRI} ${ACE})

add_executable(${PROJECT_NAME}_node src/${PROJECT_NAME}_node.cpp)

add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

target_link_libraries(${PROJECT_NAME}_node ${PROJECT_NAME} ${catkin_LIBRARIES})

