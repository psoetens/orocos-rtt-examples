
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos Input/Output Ports primitives with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Activity.hpp>
#include <rtt/Attribute.hpp>

/**
 * Include this header in order to use data ports.
 */
#include <rtt/Port.hpp>

#include <ocl/OCL.hpp>
#include <ocl/TaskBrowser.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 3: Read Orocos Component Builder's Manual, Chap 2 sect 3.4; Chap 2 sect 3.1
 *
 * First, compile and run this application and use
 * 'the_output_port' and 'the_input_port' of the Hello component.
 * Use the 'read_helper' attribute to store a read value.
 *
 * Next, write a configureHook() in Hello which checks if the input port is
 * connected and fails if it is not.
 * Question: how did we force configureHook() to be called in the current flow ?
 *
 * Next, write an updateHook() which reads the_input_port and
 * writes the data to 'the_output_port'. Be careful only to write data
 * when a read from the input was succesful.
 *
 * Finally start the World component ('World.start()'). See how it uses the
 * input port in C++. Start and stop the Hello component. See how this
 * influences the input's size.
 *
 * Optionally 1: Remove the code from updateHook and write a propram script
 * that does exactly the same, i.e., read data from the input port and,
 * if any, write it to the output port. Do this in a while loop.
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
         * OutputPorts publish data.
         */
        OutputPort<double> outputport;
        /**
         * InputPorts read data.
         */
        InputPort< double > inputport;
        /** @} */

        /**
         * Since read() requires an argument, we provide this
         * attribute to hold the read value.
         */
        double read_helper;
    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : TaskContext(name, PreOperational),
              // Name, initial value
              outputport("the_output_port", 0.0),
              // Name
              inputport("the_input_port")
        {
            this->addAlias("read_helper", read_helper);
            this->ports()->addPort(&outputport, "Shared output Port");
            this->ports()->addPort(&inputport, "inputed Data Port");
        }

        void updateHook()
        {
        }
    };

	/**
	 * World is the component that shows how to use the interface
	 * of the Hello component.
	 *
	 * This component is a pure producer of values.
	 */
	class World
	: public TaskContext
	{
	protected:
		/**
		 * This port object must be connected to Hello's port.
		 */
		OutputPort<double> my_port;
		/** @} */

		double value;
	public:
		World(std::string name)
		: TaskContext(name),
		my_port("world_port"),
		value( 0.0 )
		{
            this->ports()->addPort(&my_port, "World's data producing port.");
		}

		void updateHook() {
			my_port.write( value );
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
    hello.setActivity( new Activity(1, 0.5 ) );

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
    world.setActivity( new Activity(1, 0.5 ) );

    log(Info) << "**** Creating the 'Peer' connection ****" <<endlog();
    // This is a bidirectional connection.
    connectPeers(&world, &hello );

    log(Info) << "**** Creating the 'Data Flow' connection ****" <<endlog();
    // The data flow direction is from world to hello.
    world.ports()->getPort("world_port")->connectTo( *hello.ports()->getPort("the_input_port") );

    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}
