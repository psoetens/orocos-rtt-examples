#ifndef OROCOS_CONTROLLER_COMPONENT_HPP
#define OROCOS_CONTROLLER_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include <iostream>
#include <geometry_msgs/typekit/Pose2D.h>
#include <geometry_msgs/typekit/Twist.h>

class Controller
    : public RTT::TaskContext
{
    double radius, steps;
    int curstep;
    bool movingout;
    double gain;
    RTT::InputPort<geometry_msgs::Pose2D> curlocation;
    RTT::OutputPort<geometry_msgs::Twist> cmdvel;
    geometry_msgs::Pose2D startpos;
 public:
    Controller(std::string const& name)
        : TaskContext(name), radius(5.0f), steps(100.0f), curstep(0), gain(10.0)
    {
        std::cout << "Controller constructed !" <<std::endl;
        addPort("curlocation", curlocation);
        addPort("cmdvel", cmdvel);
    }

    bool configureHook() {
        std::cout << "Controller configured !" <<std::endl;
        return true;
    }

    bool startHook() {
        std::cout << "Controller started !" <<std::endl;
        RTT::FlowStatus fs = curlocation.read(startpos);
        movingout = true;
        curstep = 1;
        return fs == RTT::NewData;
    }

    void updateHook() {
        geometry_msgs::Pose2D outpos = startpos;
        geometry_msgs::Pose2D curpos;
        if ( curlocation.read(curpos) != RTT::NewData)
            return;
        if (movingout) {
            outpos.x = startpos.x + radius * double(curstep) / steps;
            curstep++;
            if ( outpos.x >= startpos.x + radius) {
                std::cout << "Moving out done" << std::endl;
                movingout = false;
                curstep = 0;
            }
        } else {
            outpos.x = startpos.x + radius * cos( curstep / 100 );
            outpos.y = startpos.y + radius * sin( curstep / 100 );
            curstep++;
        }
        geometry_msgs::Twist outvel;
        outvel.linear.x = ( outpos.x - curpos.x ) * gain;
        outvel.linear.y = ( outpos.y - curpos.y ) * gain;
        cmdvel.write( outvel );
    }

    void stopHook() {
        std::cout << "Controller executes stopping !" <<std::endl;
    }

    void cleanupHook() {
        std::cout << "Controller cleaning up !" <<std::endl;
    }
};

#endif
