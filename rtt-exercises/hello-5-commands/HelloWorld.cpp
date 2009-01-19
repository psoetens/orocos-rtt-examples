
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos 'Command' primitive with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>

/**
 * Include this header in order to use commands.
 */
#include <rtt/Command.hpp>

#include <ocl/OCL.hpp>
#include <ocl/TaskBrowser.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 5: Read Orocos Component Builder's Manual, Chap 2 sect 3.12
 *
 * First, compile and run this application and use 'the_command' in the TaskBrowser.
 * Stop the Hello component and try to use 'the_command' again,
 * does it still work ?
 * Now start the World component ('World.start()'). See how it uses the
 * command in C++.
 *
 * Next, add a second function 'bool multiply(double d1, double d2)'
 * Which uses log(Info) to display the product and
 * add this function to the command interface of this class.
 * Remember that your completion condition needs to have
 * the same function signature as your multiply function.
 * Optionally 1: let the command fail if d1 or d2 == 0
 * Optionally 2: let the command never complete when d1*d2 > 10.0
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
         * @name Command
         * @{
         */
        /**
         * Commands take a number of arguments and
         * return true or false. They are asynchronous
         * and executed in the thread of the receiver.
         */
        Command<bool(std::string)> command;

        /**
         * The command function executed by the receiver.
         */
        bool mycommand(std::string arg) {
            log() << "Hello receives Command: "<< arg << endlog();
            if ( arg == "World" )
                return true;
            else
                return false;
        }

        /**
         * The completion condition checked by the sender.
         */
        bool mycomplete(std::string arg) {
            log() << "Checking Command: "<< arg <<endlog();
            return true;
        }
        /** @} */

    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : RTT::TaskContext(name),
              command("the_command", &Hello::mycommand, &Hello::mycomplete, this)
        {
            // Check if all initialisation was ok:
            assert( command.ready() );

            this->commands()->addCommand(&command, "'the_command' Description",
                                         "the_arg", "Use 'World' as argument to make the command succeed.");

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
		 * This command object serves to store the
		 * call to the Hello component.
		 * It is best practice to have this object as
		 * a member variable of your class.
		 */
		Command<bool(std::string)> hello_command;
		/** @} */

	public:
		World(std::string name)
		: TaskContext(name, PreOperational)
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

			// It is best practice to lookup methods of peers in
			// your configureHook.
			hello_command = peer->commands()->getCommand<bool(std::string)>("the_command");
			if ( !hello_command.ready() ) {
				log(Error) << "Could not find Hello.hello_command Command!"<<endlog();
				return false;
			}
			return true;
		}

		void updateHook() {
			Logger::In in("World");
			log(Info) << "Last command was: "
                      << (hello_command.accepted() ? "accepted" : "not accepted")
                      << (hello_command.executed() ? ", executed" : ", not executed")
                      << (hello_command.valid() ? ", valid" : ", not valid")
                      << (hello_command.done() ? " and done!" : " and not done!") <<endlog();
			// the_command expects the 'World' argument, so it will always return 'false'.
			// However, the return value of the command itself is true as
			// long as the Hello component *accepts* the command.
			if ( hello_command.ready() )
				log(Info) << "Sending to 'Hello': " << hello_command("This is World Speeking.") <<endlog();
			else
				log(Warning) << "Previous command not done. Waiting..." <<endlog();
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

    // Let World lookup Hello's command.
    world.configure();

    log(Info) << "**** Using the 'Hello' component    ****" <<endlog();

    // Do some 'client' calls :
    log(Info) << "**** Sending a Command:             ****" <<endlog();
    TaskContext* peer = &hello;
    Command<bool(std::string)> c = peer->commands()->getCommand<bool(std::string)>("the_command");
    assert( c.ready() );
    log(Info) << "     Sending Command : " << c("World")<<endlog();


    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}
