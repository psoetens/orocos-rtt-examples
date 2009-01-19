
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos 'Method' primitive with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>

/**
 * Include this header in order to use methods.
 */
#include <rtt/Method.hpp>

#include <ocl/OCL.hpp>
#include <ocl/TaskBrowser.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 4: Read Orocos Component Builder's Manual, Chap 2 sect 3.9
 *
 * First, compile and run this application and use 'the_method'.
 * Stop the Hello component and try to use 'the_method' again,
 * does it still work ?
 * Start the World component ('World.start()') and see
 * how it uses the_method.
 *
 * Next, add to Hello a second method void 'sayIt(const std::string& word)'
 * which uses log(Info) to display a sentence.
 * Add this function to the method interface of this class.
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
         * @name Method
         * @{
         */
        /**
         * Methods take a number of arguments and
         * return a value. The are executed in the
         * thread of the caller.
         */
        Method< std::string(void) > method;

        /**
         * The method function is executed by
         * the method object:
         */
        std::string mymethod() {
            return "Hello World";
        }
        /** @} */

    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : TaskContext(name),
              // Name, function pointer, object
              method("the_method", &Hello::mymethod, this)
        {
            // Check if all initialisation was ok:
            assert( method.ready() );

            this->methods()->addMethod(&method, "'the_method' Description");
        }

    };

    /**
     * World is the component that shows how to use the interface
     * of the Hello component.
     */
    class World
		: public TaskContext
    {
    protected:
    	/**
    	 * This method object serves to store the
    	 * call to the Hello component.
    	 * It is best practice to have this object as
    	 * a member variable of your class.
    	 */
    	Method< std::string(void) > hello_method;

    	/** @} */

    public:
    	World(std::string name)
			: TaskContext(name, PreOperational)
    	{
    	}

    	bool configureHook() {

    		// Lookup the Hello component.
    	    TaskContext* peer = this->getPeer("Hello");
    	    if ( !peer ) {
    	    	log(Error) << "Could not find Hello component!"<<endlog();
    	    	return false;
    	    }

    	    // It is best practice to lookup methods of peers in
    	    // your configureHook.
    	    hello_method = peer->methods()->getMethod<std::string(void)>("the_method");
    	    if ( !hello_method.ready() ) {
    	    	log(Error) << "Could not find Hello.hello_method Method!"<<endlog();
    	    	return false;
    	    }
    	    return true;
    	}

    	void updateHook() {
    		log(Info) << "Receiving from 'Hello': " << hello_method() <<endlog();
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
    log(Info) << "**** Starting the 'hello' component ****" <<endlog();
    // Start the component's activity:
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

    // Let World lookup Hello's method.
    world.configure();

    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}
