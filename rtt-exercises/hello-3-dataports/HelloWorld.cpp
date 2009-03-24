
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos Data Ports primitive with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>
#include <rtt/Attribute.hpp>

/**
 * Include this header in order to use data ports.
 */
#include <rtt/Ports.hpp>

#include <ocl/OCL.hpp>
#include <ocl/TaskBrowser.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 3: Read Orocos Component Builder's Manual, Chap 2 sect 3.8; Chap 2 sect 3.1
 *
 * First, compile and run this application and use
 * 'the_data_port' and 'the_buffer_port' of the Hello component.
 * Use the 'pop_helper' attribute to store a Pop'ed value.
 *
 * Next, write a configureHook() in Hello which checks if the buffer port is
 * connected and fails if it is not.
 * Question: how did we force configureHook() to be called in the current flow ?
 *
 * Next, write an updateHook() which reads the_buffer_port and
 * writes the data to 'the_data_port'. Be careful only to write data
 * when a Pop from the buffer was succesful.
 *
 * Finally start the World component ('World.start()'). See how it uses the
 * buffer port in C++. Start and stop the Hello component. See how this
 * influences the buffer's size.
 *
 * Optionally 1: Remove the code from updateHook and write a propram script
 * that does exactly the same, i.e., read data from the buffer port and,
 * if any, write it to the data port. Do this in a while loop.
 *
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
    protected:
        /**
         * @name Input-Output ports
         * @{
         */
        /**
         * DataPorts share data among readers and writers.
         */
        WriteDataPort<double> dataport;
        /**
         * BufferPorts buffer data among readers and writers.
         * A reader reads the data in a FIFO way.
         */
        ReadBufferPort< double > bufferport;
        /** @} */

        /**
         * Since Pop requires an argument, we provide this
         * attribute to hold the pop'ed value.
         */
        Attribute<double> pop_helper;
    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : TaskContext(name, PreOperational),
              // Name, initial value
              dataport("the_data_port", 0.0),
              // Name
              bufferport("the_buffer_port"),
              pop_helper("pop_helper")
        {
            this->attributes()->addAttribute(&pop_helper);
            this->ports()->addPort(&dataport, "Shared Data Port");
            this->ports()->addPort(&bufferport, "Buffered Data Port");
        }

        void updateHook()
        {
        }
    };

	/**
	 * World is the component that shows how to use the interface
	 * of the Hello component.
	 *
	 * This component is a pure producer of buffered values.
	 */
	class World
	: public TaskContext
	{
	protected:
		/**
		 * This port object must be connected to Hello's port.
		 */
		WriteBufferPort<double> my_port;
		/** @} */

		double value;
	public:
		World(std::string name)
		: TaskContext(name),
		my_port("world_port", 10, 0.0),
		value( 0.0 )
		{
            this->ports()->addPort(&my_port, "World's Buffered Data Port");
		}

		void updateHook() {
			my_port.Push( value );
			++value;
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
    hello.setActivity( new PeriodicActivity(1, 0.5, hello.engine() ) );

    log(Info) << "**** Starting the 'Hello' component ****" <<endlog();
    hello.configure();
    // Start the component:
    hello.start();

    log(Info) << "**** Creating the 'World' component ****" <<endlog();
    World world("World");
    // Create the activity which runs the task's engine:
    // 1: Priority
    // 0.5: Period (2Hz)
    // world.engine(): is being executed.
    world.setActivity( new PeriodicActivity(1, 0.5, world.engine() ) );

    log(Info) << "**** Creating the 'Peer' connection ****" <<endlog();
    // This is a bidirectional connection.
    connectPeers(&world, &hello );

    log(Info) << "**** Creating the 'Data Flow' connection ****" <<endlog();
    // The data flow direction is from world to hello.
    world.ports()->getPort("world_port")->connectTo( hello.ports()->getPort("the_buffer_port") );

    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}
