#ifndef OROCOS_AREADETECTION_COMPONENT_HPP
#define OROCOS_AREADETECTION_COMPONENT_HPP

#include <rtt/RTT.hpp>
#include <iostream>

/**
 * Exercise: include the correct header in order to
 * speed up compilation of this component.
 */
#include <geometry_msgs/Pose2D.h>

class Areadetection
    : public RTT::TaskContext
{
    RTT::InputPort<geometry_msgs::Pose2D> curlocation;
    /**
     * The safe area.
     */
    geometry_msgs::Pose2D safeleftcorner, saferightcorner;
    /**
     * The slow area.
     */
    geometry_msgs::Pose2D slowleftcorner, slowrightcorner;
 public:
    Areadetection(std::string const& name)
        : TaskContext(name)
    {
        std::cout << "Areadetection constructed !" <<std::endl;
        addPort("curlocation", curlocation).doc("Receives the current location.");
        addProperty("safeleft", safeleftcorner).doc("Safe top left corner.");
        addProperty("saferight", saferightcorner).doc("Safe bottom right corner.");
        addProperty("slowleft", slowleftcorner).doc("Slow top left corner.");
        addProperty("slowright", slowrightcorner).doc("Slow bottom right corner.");

        safeleftcorner.x = -5;
        safeleftcorner.y = -5;
        saferightcorner.x = 5;
        saferightcorner.y = 5;

        slowleftcorner.x = -10;
        slowleftcorner.y = -10;
        slowrightcorner.x = 10;
        slowrightcorner.y = 10;
    }

    bool configureHook() {
        std::cout << "Areadetection configured !" <<std::endl;
        bool status = true;
        if ( safeleftcorner.x >= saferightcorner.x ) {
            std::cout << "Error: safeleft must be left of saferight." << std::endl;
            status = false;
        }
        if ( slowleftcorner.x >= slowrightcorner.x ) {
            std::cout << "Error: slowleft must be left of slowright." << std::endl;
            status = false;
        }
        if ( safeleftcorner.y >= saferightcorner.y ) {
            std::cout << "Error: safeleft must be above saferight." << std::endl;
            status = false;
        }
        if ( slowleftcorner.y >= slowrightcorner.y ) {
            std::cout << "Error: slowleft must be above slowright." << std::endl;
            status = false;
        }
        
        return status;
    }

    bool startHook() {
        std::cout << "Areadetection started !" <<std::endl;
        return true;
    }

    void updateHook() {
        // Exercise: Emit events when curlocation is traversing zones.
        // You can do this by caching the previous location of the robot
        // and compare it with the current location. Use the utility functions
        // below to ease your work.
    }

    bool isSafe(geometry_msgs::Pose2D pos) {
        return pos.x < saferightcorner.x && pos.x > safeleftcorner.x 
            && pos.y < saferightcorner.y && pos.y > safeleftcorner.y;
    }

    bool isSlow(geometry_msgs::Pose2D pos) {
        return !isSafe(pos) && pos.x < slowrightcorner.x && pos.x > slowleftcorner.x 
            && pos.y < slowrightcorner.y && pos.y > slowleftcorner.y;
    }

    bool isOut(geometry_msgs::Pose2D pos) {
        return !isSafe(pos) && !isSlow(pos);
    }

    void stopHook() {
        std::cout << "Areadetection executes stopping !" <<std::endl;
    }

    void cleanupHook() {
        std::cout << "Areadetection cleaning up !" <<std::endl;
    }
};

#endif
