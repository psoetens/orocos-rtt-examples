
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos Scripting service with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Activity.hpp>

#include <rtt/Method.hpp>
#include <rtt/Port.hpp>

#include <ocl/OCL.hpp>
#include <ocl/TaskBrowser.hpp>

#include <boost/lambda/lambda.hpp>
#include <algorithm>

using namespace std;
using namespace boost;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 6: Read Orocos Component Builder's Manual, Chap 3
 *
 * In this exercise, we will write some basic Orocos program scripts and
 * state machines. The details for the exercise are in program.ops and
 * statemachine.osd. This file contains the ready-to-be-used components
 * in which our scripts will run.
 *
 * Optional 1: Modify Hello and World such that they load the program.ops
 * and statemachine.osd scripts respectively in their configureHook() and fail if they
 * are not present or contain an error.
 */
namespace Example
{
    typedef std::vector<double> Data;

	/**
	 * This component offers some operations,
	 * an input port and an output port. We'll implement
	 * the behaviour of this component in a program script.
     */
    class Hello
        : public TaskContext
    {
    protected:
        Data data;
        /**
         * @name Operations
         * @{
         */

        Data& multiply(Data& v, double factor) {
            for_each(v.begin(), v.end(), lambda::_1 *= factor);
            return v;
        }

        Data& divide(Data& v, double number) {
            for_each(v.begin(), v.end(), lambda::_1 /= number);
            return v;
        }

        /** @} */

        InputPort<Data> input;
        OutputPort<Data> output;

    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : TaskContext(name), input("input"), output("output")
        {
            this->provides()->addOperation("multiply", &Hello::multiply, this).doc("Multiplies.")
                    .arg("v","Data").arg("f", "Factor");
            this->provides()->addOperation("divide", &Hello::multiply, this).doc("Divides.")
                    .arg("v","Data").arg("f", "Factor");

            this->ports()->addPort(&input);
            this->ports()->addPort(&output);
        }
    };

    /**
     * World contains two ports. The rest of the
     * behaviour is implemented in statemachine.osd.
     */
    class World
		: public TaskContext
    {
    protected:
		InputPort<Data> input;
		OutputPort<Data> output;
    public:
    	World(std::string name)
			: TaskContext(name, PreOperational),
			  input("input"),output("output")
    	{
    	    // Add input and output as ports, but add input as an event generating port.
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
    hello.setActivity( new Activity(1, 0.5 ) );
    log(Info) << "**** Starting the 'hello' component ****" <<endlog();
    // Start the component:
    hello.start();

    log(Info) << "**** Creating the 'World' component ****" <<endlog();
    World world("World");
    // Create the activity which runs the task's engine:
    // 1: Priority
    // 0.5: Period (2Hz)
    world.setActivity( new Activity(1, 0.5 ) );

    log(Info) << "**** Creating the 'Peer' connection ****" <<endlog();
    // This is a bidirectional connection.
    connectPeers(&world, &hello );

    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}
