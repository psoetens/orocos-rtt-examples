
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos 'Property' and 'Attribute' primitives with
 * a 'hello world' example.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Activity.hpp>

/**
 * Include this header in order to use properties or attributes.
 */
#include <rtt/Property.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Component.hpp>

// Optional:
#include <rtt/marsh/Marshalling.hpp>

using namespace std;
using namespace RTT;

/**
 * Exercise 2: Read Orocos Component Builder's Manual, Chap 2 sect. 3.6
 *
 * First, compile and run this application and use 'property' and 'attribute':
 * Change and print their values in the TaskBrowser.
 *
 * Next save the properties of this component to a hello.xml file:
 * You will need to install the 'marshalling' service using the TaskBrowser
 * at runtime:
 * ** TaskBrowser: type from the Deployer: loadService("hello", "marshalling")
 *
 * To make this permanent for your component, add this statement in the start.ops file.
 *
 * Optional: use loadService in C++:
 *    In C++ you need to #include <rtt/marsh/Marshalling.hpp> 
 *    and add to the constructor: this->getProvider<Marshalling>("marshalling");
 *
 *    In the CMakeLists.txt: add rtt-marshalling to the list of components to look for in
 *    find_package(OROCOS-RTT REQUIRED) macro.
 *    See : http://www.orocos.org/wiki/orocos/toolchain/getting-started/cmake-and-building
 *    and use that syntax in the CMakeLists.txt file to link 'HelloWorld' with marshalling.
 *
 *
 * When marshalling is loaded:
 * In order to find out how to write the property to a file using 'marshalling',
 * type 'marshalling' (without quotes) to see the interface of the marshalling
 * task object.
 *
 * Next Open and modify the XML file and read it back in using the marshalling object.
 *
 * For the optional exercises, reead Chap 2, sect 6.1
 * Optional : read the property file from configureHook() and log it's value. You need
 * to make the modifications detailed above in the note.
 * Optional : write the property file in cleanupHook().
 *
 * For ROS users: load the rtt_rosparam service as well and send the properties to the
 * ROS master server instead of to the XML file. 
 * At runtime:
 * ** TaskBrowser: type 'import("rtt_rosnode")' and 'loadService("hello", "rosparam")' in
 *    the deployer
 *
 * Open question: Would you prefer to hard-code this property reading/writing or would
 * you prefer to script it ?
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
        std::string property;
        /**
         * Attributes are C++ variables exported to the interface.
         */
        double attribute;
        /**
         * Constants are not changeable by an outsider, only by us.
         */
        std::string constant;
        /** @} */

    public:
        /**
         * This example sets the interface up in the Constructor
         * of the component.
         */
        Hello(std::string name)
            : TaskContext(name),
              // Name, description, value
              property("Hello World")
        {
            // Now add it to the interface:
            this->addProperty("property", property).doc("This property can contain any friendly string.");

            this->addAttribute("attribute", attribute);
            this->addConstant("constant", constant);

            // Optional:
            this->getProvider<Marshalling>("marshalling");
        }

        bool configureHook() {
            if ( this->getProvider<Marshalling>("marshalling")->readProperties("hello.xml") == true ) {
                log(Info) << "The property value is now: " << property << endlog();
            } else {
                log(Warning) << "No hello.xml property file present yet !" << endlog();
            }
            return true;
        }

        void cleanupHook() {
            this->getProvider<Marshalling>("marshalling")->writeProperties("hello.xml");
        }
    };
}

ORO_CREATE_COMPONENT( Example::Hello )
