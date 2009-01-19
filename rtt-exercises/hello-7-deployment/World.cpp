
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
#include <rtt/Event.hpp>
#include <rtt/Ports.hpp>

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
         * DataPorts share data among readers and writers.
         * A reader always reads the most recent data.
         */
        DataPort<std::string> dataport;
        /**
         * BufferPorts buffer data among readers and writers.
         * A reader reads the data in a FIFO way.
         */
        BufferPort<std::string> bufferport;
        /** @} */

        /**
         * @name Event
         * @{
         */
        /**
         * Stores the connection between 'event' and 'mycallback'.
         */
        Handle h;

        /**
         * An event callback (or subscriber) function.
         */
        void mycallback( std::string data )
        {
            log() << "Receiving Event: " << data << endlog();
        }
        /** @} */

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
              dataport("world_data_port","World"),
              // Name, buffer size, initial value
              bufferport("world_buffer_port",13, "World")
        {
            // Check if all initialisation was ok:
            assert( property.ready() );

            // Now add it to the interface:
            this->properties()->addProperty(&property);

            this->ports()->addPort(&dataport);
            this->ports()->addPort(&bufferport);

        }

        bool configureHook() {
            Logger::In in("World");
            // Lookup the Hello component.
            TaskContext* peer = this->getPeer("Hello");
            if ( !peer ) {
                log(Error) << "Could not find Hello component!"<<endlog();
                return false;
            }

            // It is best practice to lookup events of peers in
            // your configureHook().
            // Setup an asynchronous (note the use of 'this->engine()' ) call-back
            // to 'the_event' of 'peer':
            h = peer->events()->setupConnection("the_event").callback(this, &World::mycallback, this->engine()->events() ).handle();

            // now use h to connect or disconnect the callback function.
            assert( !h.connected() );
            // if connect() returns true, 'mycallback' was effectively connected to 'the_event'
            // and will be executed each time 'the_event' is fired.
            return h.connect();
        }

    };
}

#include <ocl/ComponentLoader.hpp>
ORO_CREATE_COMPONENT( Example::World );
