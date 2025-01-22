#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    // TODO: Request a service and pass the velocities to it to drive the robot
  ROS_INFO_STREAM("Moving the bot to the ball");

  ball_chaser::DriveToTarget srv;
  srv.request.linear_x = lin_x;
  srv.request.angular_z = ang_z;

  if (!client.call(srv)){
    ROS_ERROR("Failed to call ball_chaser/command_robot");
  }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
   

    // TODO: Loop through each pixel in the image and check if there's a bright white one
    int white_location = -255;
   //it is done three at a time because each pixel has red blue and green values
    for (int i = 0; i < (img.height * img.step); i = i+3){
		if ((img.data[i] == white_pixel) && (img.data[i+1] == white_pixel) && (img.data[i+2] == white_pixel)){
			white_location = i;
			break;
		}
		if(i == img.height * img.step){//if we no longer see the the pixel we stop
			white_location = -255;
			break;
		}
	}
    // Then, identify if this pixel falls in the left, mid, or right side of the image
    float left_side = (img.step / 3);
    float right_side = 2 * (img.step / 3);
    float horizontal_position = white_location % img.step;
	
    // Depending on the white ball position, call the drive_bot function and pass velocities to it
    if(white_location >= 0){
		if (horizontal_position < left_side)
			drive_robot(0.0, 0.1); //left
		else if(horizontal_position > right_side)
			drive_robot(0.0, -0.1); //right
		else{
			drive_robot(0.1, 0.0); //forward
		}

    // Request a stop when there's no white ball seen by the camera
    }else{
		drive_robot(0.0, 0.0);
     }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
