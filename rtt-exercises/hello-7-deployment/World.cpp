
/**
 * @file World.cpp
 * This file demonstratess the Orocos component loading and XML with
 * a 'hello world' example.
 *
 * Exercise: This file (World.cpp) serves to demonstrate a fully
 * functional, loadable Orocos component. Extend the Hello.cpp file
 * in order to make the Example::Hello component loadable as well.
 *
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>

#include <rtt/Property.hpp>
#include <rtt/Port.hpp>

using namespace std;
using namespace RTT;

namespace Example
{

    /**
     * This component will be dynamically loaded in your application.
     */
    class World
        : public TaskContext
    {
    protected:
        /**
         * @name Name-Value parameters
         * @{
         */
        /**
         * Properties take a name, a value and a description
         * and are suitable for XML.
         */
        Property<std::string> property;
        /** @} */

        /**
         * @name Input-Output ports
         * @{
         */
        /**
         * OutputPorts produce data
         */
        OutputPort<std::string> outport;
        /**
         * InputPorts consume data.
         */
        InputPort<std::string> inport;
        /** @} */

        Method <string(void)> mymethod;
        Method <bool(string)> sayWorld;
    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        World(std::string name)
            : TaskContext(name, PreOperational),
              // Name, description, value
              property("world_property", "the_property Description", "Example"),
              // Name, initial value
              outport("outport"),
              // Name, buffer size, initial value
              inport("inport"),
              mymethod("the_method"),
              sayWorld("the_command")
        {
            // Check if all initialisation was ok:
            assert( property.ready() );

            // Now add it to the interface:
            this->addProperty(&property);

            this->ports()->addPort(&outport);
            this->ports()->addPort(&inport);

            this->requires()->addMethod(mymethod);
            this->requires()->addMethod(sayWorld);
        }

        bool configureHook() {
            Logger::In in("World");
            // Lookup the Hello component.
            TaskContext* peer = this->getPeer("Hello");
            if ( !peer ) {
                log(Error) << "Could not find Hello component!"<<endlog();
                return false;
            }
            return true;
        }

    };
}

#include <ocl/ComponentLoader.hpp>
ORO_CREATE_COMPONENT( Example::World );
