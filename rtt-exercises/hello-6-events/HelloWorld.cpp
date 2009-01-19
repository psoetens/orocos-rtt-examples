
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos 'Event' primitive with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>

/**
 * Include this header in order to use events.
 */
#include <rtt/Event.hpp>

#include <ocl/OCL.hpp>
#include <ocl/TaskBrowser.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 6: Read Orocos Component Builder's Manual, Chap 2 sect. 3.13
 *
 * First, compile and run this application and use 'the_event' in the TaskBrowser.
 * Note that the 'World' component is running and is receiving the event.
 * Stop the 'World' component and use 'the_event' again. What happens?
 *
 * Next, write a second call-back function in 'World' 'void other_callback(std::string)'
 * and attach it synchronously to the event. Compare the reaction times
 * between the new synchronous and the existing asynchronous call-back.
 *
 * Next, stop the 'World' component and call 'the_event' again. Which call-back is
 * processed and which isn't ?
 *
 * Optional : emit 'the_event' from Hello's updateHook() and start/stop the component
 * Optional : add to 'World' a method 'void stop_event()' which disconnects the call-backs
 * and a method 'void start_event()' which connects the call-backs. Both
 * use the Handle objects for this purpose. Stop/start receiving events while
 * the World component keeps running.
 */
namespace Example
{

    /**
     * Every component inherits from the 'TaskContext' class.  This base
     * class allow a user to add a primitive to the interface and contain
     * an ExecutionEngine which executes application code.
     */
    class Hello
        : public RTT::TaskContext
    {
    protected:
        /**
         * @name Event
         * @{
         */
        /**
         * The event takes a pay load which is distributed
         * to anonymous receivers. Distribution can happen
         * synchronous and asynchronous.
         */
        Event<void(std::string)> event;
        /** @} */

    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : TaskContext(name),
              // Name
              event("the_event")
        {
            // Check if all initialization was OK:
            assert( event.ready() );

            this->events()->addEvent(&event, "'the_event' Description",
                                     "the_data", "The data of this event.");
        }
    };

    	/**
    	 * World is the component that shows how to use the interface
    	 * of the Hello component.
    	 */
        class World
            : public RTT::TaskContext
        {
        protected:

            /**
             * Stores the connection between one 'event' and one 'mycallback'.
             */
            Handle h;

            /**
             * An event call-back (i.e. subscriber) function.
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
                : TaskContext(name)
            {
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

using namespace Example;

int ORO_main(int argc, char** argv)
{
    Logger::In in("main()");

    // Set log level more verbose than default,
    // such that we can see output :
    if ( log().getLogLevel() < Logger::Info ) {
        log().setLogLevel( Logger::Info );
        log(Info) << argv[0] << " manually raises LogLevel to 'Info' (5). See also file 'orocos.log'."<<endlog();
    }

    log(Info) << "**** Creating the 'Hello' component ****" <<endlog();
    // Create the task:
    Hello hello("Hello");
    // Create the activity which runs the task's engine:
    // 1: Priority
    // 0.5: Period (2Hz)
    // hello.engine(): is being executed.
    PeriodicActivity h_act(1, 0.5, hello.engine() );

    log(Info) << "**** Starting the 'Hello' component ****" <<endlog();
    // Start the component:
    hello.start();

    log(Info) << "**** Creating the 'World' component ****" <<endlog();
    World world("World");
    // Create the activity which runs the task's engine:
    // 1: Priority
    // 0.5: Period (2Hz)
    // hello.engine(): is being executed.
    PeriodicActivity w_act(1, 0.5, world.engine() );

    log(Info) << "**** Creating the 'Peer' connection ****" <<endlog();
    // This is a bidirectional connection.
    connectPeers(&world, &hello );

    log(Info) << "**** Starting the 'World' component ****" <<endlog();
    // Start the component:
    world.start();

    log(Info) << "**** Using the 'Hello' component    ****" <<endlog();

    log(Info) << "**** Emitting an Event:             ****" <<endlog();
    Event<void(std::string)> e = hello.events()->getEvent<void(std::string)>("the_event");
    assert( e.ready() );

    e("Hello World");

    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}
