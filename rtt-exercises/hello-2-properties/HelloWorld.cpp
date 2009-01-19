
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos 'Property' and 'Attribute' primitives with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>

/**
 * Include this header in order to use properties or attributes.
 */
#include <rtt/Property.hpp>
#include <rtt/Attribute.hpp>

#include <ocl/OCL.hpp>
#include <ocl/TaskBrowser.hpp>

using namespace std;
using namespace RTT;
using namespace Orocos;

/**
 * Exercise 2: Read Orocos Component Builder's Manual, Chap 2 sect. 3.11
 *
 * First, compile and run this application and use 'the_property' and 'the_attribute':
 * Change and print their values.
 *
 * Next save the properties of this component to a hello.xml file.
 * In order to find out how to write the property to a file using 'marshalling',
 * type 'marshalling' (without quotes) to see the interface of the marshalling
 * task object.
 *
 * Next Open and modify the XML file and read it back in using the marshalling object.
 *
 * Optional : read the property file from configureHook() and log it's value.
 * Optional : write the property file in cleanupHook().
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
         * @name Name-Value parameters
         * @{
         */
        /**
         * Properties take a name, a value and a description
         * and are suitable for XML.
         */
        Property<std::string> property;
        /**
         * Attributes take a name and contain changing values.
         */
        Attribute<double> attribute;
        /**
         * Constants take a name and contain a constant value.
         */
        Constant<std::string> constant;
        /** @} */

    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : TaskContext(name),
              // Name, description, value
              property("the_property", "the_property Description", "Hello World"),
              // Name, value
              attribute("the_attribute", 5.0),
              // Name, value
              constant("the_constant", "Hello World")
        {
            // Check if all initialization was OK:
            assert( property.ready() );
            assert( attribute.ready() );
            assert( constant.ready() );

            // Now add it to the interface:
            this->properties()->addProperty(&property);

            this->attributes()->addAttribute(&attribute);
            this->attributes()->addConstant(&constant);

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
    PeriodicActivity act(1, 0.5, hello.engine() );

    log(Info) << "**** Starting the 'Hello' component ****" <<endlog();
    // Start the component:
    hello.start();

    log(Info) << "**** Using the 'Hello' component    ****" <<endlog();

    // Do some 'client' calls :
    log(Info) << "**** Reading a Property:            ****" <<endlog();
    Property<std::string> p = hello.properties()->getProperty<std::string>("the_property");
    assert( p.ready() );
    log(Info) << "     "<<p.getName() << " = " << p.value() <<endlog();

    log(Info) << "**** Starting the TaskBrowser       ****" <<endlog();
    // Switch to user-interactive mode.
    TaskBrowser browser( &hello );

    // Accept user commands from console.
    browser.loop();

    return 0;
}
