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
    bool do_demo;
    int demo_counter;
    RTT::InputPort<sensor_msgs::Joy> joystick;
    RTT::OutputPort<geometry_msgs::Twist> cmdvel;
 public:
    Teleop(std::string const& name)
        : TaskContext(name),
          linear_x(1),linear_y(0),
          angular_z(2), l_scale_(1), a_scale_(1), 
          do_demo(false), demo_counter(0)
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

        addOperation("demo", &Teleop::demo, this).arg("on_off", "Turn the demo mode on (true) or off (false)" ).doc("Turns on or off 'demo' mode, which allows you to simulate a joystick in case you have no access to one.");
    }


    void demo( bool on_off ) {
        do_demo = on_off;
        this->trigger(); // start updateHook()
    }

    bool startHook() {
        geometry_msgs::Twist vel;
        cmdvel.write(vel);
        demo_counter = 0;
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
        } else {
            if (do_demo) {
                demo_counter++;
                if (demo_counter == 100)
                    vel.linear.x = l_scale_ * 1;
                else if (demo_counter < 200)
                    vel.linear.z = a_scale_ * 1;
                else if (demo_counter < 300)
                    vel.linear.y = l_scale_ * 1;
                else
                    demo_counter = 0;
                // hacked sleep-and-trigger logic. RTT should offer this out of the box !
                cmdvel.write(vel);
                TIME_SPEC ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 1000*1000*20; // 20ms
                rtos_nanosleep(&ts, 0);
                this->trigger();
            }
        }
    }

    void stopHook() {
        geometry_msgs::Twist vel;
        cmdvel.write(vel);
    }
};

#endif
