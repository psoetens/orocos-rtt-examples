#ifndef OROCOS_NOBOT_COMPONENT_HPP
#define OROCOS_NOBOT_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include <iostream>
#include <geometry_msgs/typekit/Twist.h>
#include <geometry_msgs/typekit/Pose2D.h>

/**
 * Simplistic implementation of a Youbot base.
 */
class Nobot
    : public RTT::TaskContext
{
    RTT::InputPort<geometry_msgs::Twist> cmdvel;
    RTT::OutputPort<geometry_msgs::Pose2D> curpos;
    geometry_msgs::Pose2D mcurpos;
    double inertia;
 public:
    Nobot(std::string const& name)
        : TaskContext(name), inertia(1.0)
    {
        std::cout << "Nobot constructed !" <<std::endl;
        addPort("cmdvel",cmdvel);
        addPort("curpos",curpos);
        mcurpos.x = 0;
        mcurpos.y = 0;
        mcurpos.theta = 0;
    }

    bool configureHook() {
        curpos.write( mcurpos );
        return true;
    }

    bool startHook() {
        curpos.write( mcurpos );
        return true;
    }

    void updateHook() {
        geometry_msgs::Twist vel;
        RTT::FlowStatus fs = cmdvel.read( vel );
        if ( fs == RTT::NewData ) {
            mcurpos.x += this->engine()->getActivity()->getPeriod()*vel.linear.x /inertia;
            mcurpos.y += this->engine()->getActivity()->getPeriod()*vel.linear.y /inertia;
            mcurpos.theta += this->engine()->getActivity()->getPeriod()*vel.angular.z /inertia;
        }
        curpos.write(mcurpos);
    }

    void stopHook() {
        std::cout << "Nobot executes stopping !" <<std::endl;
    }

    void cleanupHook() {
        std::cout << "Nobot cleaning up !" <<std::endl;
    }
};

#endif
