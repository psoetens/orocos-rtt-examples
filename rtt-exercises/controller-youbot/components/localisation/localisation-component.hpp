#ifndef OROCOS_LOCALISATION_COMPONENT_HPP
#define OROCOS_LOCALISATION_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include <iostream>
#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/TransformStamped.h>
#include <kdl/frames.hpp>

class Localisation
    : public RTT::TaskContext
{
    geometry_msgs::TransformStamped tfs;
    RTT::OutputPort<geometry_msgs::Pose2D> outpose;
 public:
    Localisation(std::string const& name)
        : TaskContext(name, PreOperational)
    {
        addPort("pose", outpose).doc("Contains latest Youbot Pose.");
    }

    bool configureHook() {
        /** Check in here that we can call rtt_tf */
        return false;
    }

    void updateHook() {
        try {
            /* tfs = ...call rtt_tf and query the transform /odom to /base_footprint ... ; */
        } catch (...) {
            return;
        }
        geometry_msgs::Pose2D pose;
        pose.x = tfs.transform.translation.x;
        pose.y = tfs.transform.translation.y;
        geometry_msgs::Quaternion qu = tfs.transform.rotation;
        KDL::Rotation rot = KDL::Rotation::Quaternion(qu.x, qu.y, qu.z, qu.w);
        KDL::Vector v;
        pose.theta = rot.GetRotAngle(v);
        outpose.write(pose);
    }
};

#endif
