#include <ros/ros.h>
#include <ros/console.h>
#include <lcm_to_ros/Path.h>
#include <nav_msgs/Path.h>
#include <lcm_to_ros/PoseStamped.h>
#include <nav_msgs/PoseStamped.h>
#include <stdlib.h>

ros::Publisher pub;
std::string lcm_topic, ros_topic, direction;

void rosCallback(const nav_msgs::Path::ConstPtr& msg)
{
  lcm_to_ros::Path bridge_message;

  bridge_message.header.seq = msg->header.seq;
  bridge_message.header.frame_id = msg->header.frame_id;
  bridge_message.header.stamp = msg->header.stamp;

  bridge_message.nposes = msg->poses.size();

  for(int i = 0; i < msg->poses.size(); i++){
    lcm_to_ros::PoseStamped pstamp;

    pstamp.header.seq = msg->transforms[i].header.seq;
    pstamp.header.frame_id = msg->transforms[i].header.frame_id;
    pstamp.header.stamp = msg->transforms[i].header.stamp;

    pstamp.pose.position.x = msg->poses[i].pose.position.x;
    pstamp.pose.position.y = msg->poses[i].pose.position.y;
    pstamp.pose.position.z = msg->poses[i].pose.position.z;

    pstamp.pose.orientation.x = msg->poses[i].pose.orientation.x;
    pstamp.pose.orientation.y = msg->poses[i].pose.orientation.y;
    pstamp.pose.orientation.z = msg->poses[i].pose.orientation.z;
    pstamp.pose.orientation.w = msg->poses[i].pose.orientation.w;

    bridge_message.poses.push_back(pstamp);
  }

  pub.publish(bridge_message);
}

void lcmCallback(const lcm_to_ros::Path::ConstPtr& msg)
{
  nav_msgs::Path bridge_message;

  for(int i = 0; i < msg->transforms.size(); i++){
    geometry_msgs::TransformStamped tform;

    bridge_message.header.seq = msg->header.seq;
    bridge_message.header.frame_id = msg->header.frame_id;
    bridge_message.header.stamp = msg->header.stamp;

    for(int i = 0; i < msg->poses.size(); i++){
      lcm_to_ros::PoseStamped pstamp;

      pstamp.header.seq = msg->transforms[i].header.seq;
      pstamp.header.frame_id = msg->transforms[i].header.frame_id;
      pstamp.header.stamp = msg->transforms[i].header.stamp;

      pstamp.pose.position.x = msg->poses[i].pose.position.x;
      pstamp.pose.position.y = msg->poses[i].pose.position.y;
      pstamp.pose.position.z = msg->poses[i].pose.position.z;

      pstamp.pose.orientation.x = msg->poses[i].pose.orientation.x;
      pstamp.pose.orientation.y = msg->poses[i].pose.orientation.y;
      pstamp.pose.orientation.z = msg->poses[i].pose.orientation.z;
      pstamp.pose.orientation.w = msg->poses[i].pose.orientation.w;

      bridge_message.poses.push_back(pstamp);
    }

  pub.publish(bridge_message);
}

int main(int argc, char **argv) {
  ros::init(argc, argv, "path_remapper");
  ros::NodeHandle nh("~");
  ros::Subscriber sub;

  if(nh.getParam("ros_topic", ros_topic) && nh.getParam("lcm_topic", lcm_topic) && nh.getParam("direction", direction)){
     if(!direction.compare("ros2lcm")){
       ROS_INFO("(%s) LCM <-- ROS (%s)", lcm_topic.c_str(), ros_topic.c_str());
    }
    else{
      ROS_INFO("(%s) LCM --> ROS (%s)", lcm_topic.c_str(), ros_topic.c_str());
    }

    if(!direction.compare("ros2lcm")){
      pub = nh.advertise<lcm_to_ros::Path>(lcm_topic, 10);
      sub = nh.subscribe(ros_topic, 10, rosCallback);
    }
    else{
      pub = nh.advertise<nav_msgs::Path>(ros_topic, 10);
      sub = nh.subscribe(lcm_topic, 10, lcmCallback);
    }

    ros::spin();
  }
  else{
    ROS_ERROR("Missing Parameters!");
  }

  return 0;

}