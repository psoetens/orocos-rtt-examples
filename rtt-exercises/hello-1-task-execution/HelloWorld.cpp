
/**
 * @file HelloWorld.cpp
 * This file demonstratess the Orocos TaskContext execution with
 * a 'hello world' example.
 */

#include <rtt/RTT.hpp>
#include <rtt/Component.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 1: Read Orocos Component Builder's Manual, Chap 2 sect 3 and 3.1
 *
 * First, compile and run this application and try to start the component
 * from the TaskBrowser console. How often is updateHook() executed ? Why ?
 *
 * Tip: In order to find out which functions this component has, type 'ls', and
 * for detailed information, type 'help this' (i.e. print the interface of the 'this'
 * task object).
 *
 * Next, Set the period of the component in configureHook to 0.5 seconds and
 * make start() succeed when the period of the component indeed equals 0.5 seconds.
 *
 * Next, add functions which use the log(Info) construct to display
 * a notice when the configureHook(), startHook(), stopHook() and cleanupHook()
 * are executed. (Note: not all these functions return a bool!)
 *
 * Recompile and restart this application and try to configure, start, stop
 * and cleanup the component.
 *
 * Optional: Let the Hello component be created in the 'PreOperational' mode.
 * What effect does this have on the acceptance of the start() method ?
 * Optional: Replace the Activity with a SlaveActivity. What are
 * the effects of trigger and update in comparison with the other activity types ?
 */
namespace Example
{

    /**
     * Every component inherits from the 'TaskContext' class.  This base
     * class allow a user to add a primitive to the interface and contain
     * an ExecutionEngine which executes application code.
     */
    class Hello
        : public TaskContext
    {
    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : RTT::TaskContext(name, PreOperational)
        {
        }

        void updateHook()
        {
        	log(Info) << "Update !" <<endlog();
        }
        bool configureHook()
        {
        	log(Info) << "Configure !" <<endlog();
            this->setPeriod(0.5);
        	return true;
        }
        bool startHook()
        {
        	log(Info) << "Start !" <<endlog();
            return this->getPeriod() == 0.5;
        }
        void stopHook()
        {
        	log(Info) << "Stop !" <<endlog();
        }
        void cleanupHook()
        {
        	log(Info) << "Cleanup !" <<endlog();
        }
    };
}

ORO_CREATE_COMPONENT( Example::Hello )
