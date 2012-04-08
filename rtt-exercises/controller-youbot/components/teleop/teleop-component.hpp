#ifndef OROCOS_TELEOP_COMPONENT_HPP
#define OROCOS_TELEOP_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include <iostream>
#include <sensor_msgs/typekit/Joy.h>
#include <geometry_msgs/typekit/Twist.h>

class Teleop
    : public RTT::TaskContext
{
    int linear_x, linear_y;
    int angular_z;
    double l_scale_, a_scale_;
    RTT::InputPort<sensor_msgs::Joy> joystick;
    RTT::OutputPort<geometry_msgs::Twist> cmdvel;
 public:
    Teleop(string const& name)
        : TaskContext(name),
          linear_x(1),linear_y(0),
          angular_z(2), l_scale_(1), a_scale_(1)
    {
        provides()->doc("A component which takes any USB joystick and translates it to teleoperation commands for the Youbot. The default axes are configured for the PlayStation 3 joystick.");

        std::cout << "Teleop constructed !" <<std::endl;
        addEventPort("joystick", joystick);
        addPort("cmdvel", cmdvel);

        /** Optional Exercise: get these parameters from ROS using the RTT rosparam service." */
        addProperty("linear_x",linear_x).doc("The axis for the linear x displacement.");
        addProperty("linear_y",linear_y).doc("The axis for the linear y displacement.");
        addProperty("angular_z",angular_z).doc("The axis for the angular z rotation.");

        addProperty("l_scale",l_scale_).doc("Scales the linear axes to a linear velocity.");
        addProperty("a_scale",a_scale_).doc("Scales the angular axis to an angular velocity.");
    }

    bool configureHook() {
        std::cout << "Teleop configured !" <<std::endl;
        return true;
    }

    bool startHook() {
        std::cout << "Teleop started !" <<std::endl;
        return true;
    }

    void updateHook() {
        geometry_msgs::Twist vel;
        sensor_msgs::Joy joy;
        if ( joystick.read(joy) == RTT::NewData) {
            vel.linear.x = l_scale_ * joy.axes[linear_x];
            vel.linear.y = l_scale_ * joy.axes[linear_y];
            vel.angular.z = a_scale_ * joy.axes[angular_z];
            cmdvel.write(vel);
        }
    }

    void stopHook() {
    }

    void cleanupHook() {
        std::cout << "Teleop cleaning up !" <<std::endl;
    }
};

#endif
