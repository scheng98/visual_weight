#include <ros/ros.h>
#include <visualization_msgs/Marker.h>
#include "spunc/Loadcell.h"
#include "spunc/spunc.h"

static uint16_t s16p_decipounds[8] = {1};
//
// void spunc_msg_callback
// When visual_wight node receives a "spunc_rx" message, it invokes this callback function
//
void spunc_msg_callback(const spunc::Loadcell& msg)
{
    ROS_INFO("%s: I heard", __FUNCTION__);

    if (msg.RH.u8_type == 0x3A /* SPUC_SERIAL_REC_LOADCELL */)
    {
        ROS_INFO("%s: got LOADCELL first two readings, %d, %d", 
                 __FUNCTION__, msg.s16p_decipounds[0], msg.s16p_decipounds[1]);
        memcpy((uint16_t *)s16p_decipounds, (uint16_t *)&msg.s16p_decipounds[0], 8);
    }

}

int main( int argc, char** argv )
{
  ros::init(argc, argv, "visual_weight");
  ros::NodeHandle n;
  ros::Rate r(1);
  ros::Publisher marker_pub = n.advertise<visualization_msgs::Marker>("visualization_marker", 1);
  ros::Subscriber sub = n.subscribe("spunc_rx", 1000, spunc_msg_callback);

  // Set our initial shape type to be a cube
  uint32_t shape = visualization_msgs::Marker::CYLINDER;

  while (ros::ok())
  {
    visualization_msgs::Marker marker;
    // Set the frame ID and timestamp.  See the TF tutorials for information on these.
    marker.header.frame_id = "/my_frame";
    marker.header.stamp = ros::Time::now();

    // Set the namespace and id for this marker.  This serves to create a unique ID
    // Any marker sent with the same namespace and id will overwrite the old one
    marker.ns = "basic_shapes";
    marker.id = 0;

    // Set the marker type.  Initially this is CUBE, and cycles between that and SPHERE, ARROW, and CYLINDER
    marker.type = shape;

    // Set the marker action.  Options are ADD, DELETE, and new in ROS Indigo: 3 (DELETEALL)
    marker.action = visualization_msgs::Marker::ADD;

    // Set the pose of the marker.  This is a full 6DOF pose relative to the frame/time specified in the header
    marker.pose.position.x = 0;
    marker.pose.position.y = 0;
    marker.pose.position.z = 0;
    marker.pose.orientation.x = 0.0;
    marker.pose.orientation.y = 0.0;
    marker.pose.orientation.z = 0.0;
    marker.pose.orientation.w = 1.0;

    // Set the scale of the marker -- 1x1x1 here means 1m on a side
    marker.scale.x = 1.0*s16p_decipounds[0];
    marker.scale.y = 1.0*s16p_decipounds[0];
    marker.scale.z = 1.0;

    // Set the color -- be sure to set alpha to something non-zero!
    marker.color.r = 0.0f;
    marker.color.g = (1.0f);
    marker.color.b = (0.1f);
    marker.color.a = 1.0;

    marker.lifetime = ros::Duration();

    // Publish the marker
    while (marker_pub.getNumSubscribers() < 1)
    {
      if (!ros::ok())
      {
        return 0;
      }
      ROS_WARN_ONCE("Please create a subscriber to the marker");
      sleep(1);
    }
    marker_pub.publish(marker);

    r.sleep();
  }
}
