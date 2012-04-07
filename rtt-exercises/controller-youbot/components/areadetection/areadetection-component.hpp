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
    Areadetection(string const& name)
        : TaskContext(name)
    {
        std::cout << "Areadetection constructed !" <<std::endl;
        addPort("curlocation", curlocation).doc("Receives the current location.");
        addProperty("safeleft", safeleftcorner).doc("Safe top left corner.");
        addProperty("saferight", saferightcorner).doc("Safe bottom right corner.");
        addProperty("slowleft", slowleftcorner).doc("Slow top left corner.");
        addProperty("slowright", slowrightcorner).doc("Slow bottom right corner.");
    }

    bool configureHook() {
        std::cout << "Areadetection configured !" <<std::endl;
        bool status = true;
        if ( (status &= safeleftcorner.x >= saferightcorner.x) )
            cout << "Error: safeleft must be left of saferight." << endl;
        if ( (status &= slowleftcorner.x >= slowrightcorner.x) )
            cout << "Error: slowleft must be left of slowright." << endl;
        if ( (status &= safeleftcorner.y >= saferightcorner.y) )
            cout << "Error: safeleft must be above saferight." << endl;
        if ( (status &= slowleftcorner.y >= slowrightcorner.y) )
            cout << "Error: slowleft must be above slowright." << endl;
        
        return status;
    }

    bool startHook() {
        std::cout << "Areadetection started !" <<std::endl;
        return true;
    }

    void updateHook() {
        // Exercise: Emit events when curlocation is traversing zones.

    }

    void stopHook() {
        std::cout << "Areadetection executes stopping !" <<std::endl;
    }

    void cleanupHook() {
        std::cout << "Areadetection cleaning up !" <<std::endl;
    }
};

#endif
