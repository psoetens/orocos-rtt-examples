
/**
 * @file Hello.cpp
 * This file demonstratess the Orocos component loading and XML with
 * a 'hello world' example.
 */

/*
 * Exercise 7: This file contains a fully functional Orocos component.
 * It's your task to extend the hello.xml file in this directory such
 * that you can load, configure and start this component from the deployer
 * application.
 *
 * NOTE: See the bottom of this file for the first exercise.
 * NOTE: See the hello.xml file for the rest of the exercise.
 */

#include <rtt/os/main.h>

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>

#include <rtt/Property.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Method.hpp>
#include <rtt/Command.hpp>
#include <rtt/Event.hpp>
#include <rtt/Ports.hpp>

using namespace std;
using namespace RTT;

namespace Example
{

    /**
     * This component will be dynamically loaded in your application.
     */
    class Hello
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
        /**
         * Attributes take a name and contain changing values.
         */
        Attribute<std::string> attribute;
        /**
         * Constants take a name and contain a constant value.
         */
        Constant<std::string> constant;
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
         * @name Method
         * @{
         */
        /**
         * Methods take a number of arguments and
         * return a value. The are executed in the
         * thread of the caller.
         */
        Method<std::string(void)> method;

        /**
         * The method function is executed by
         * the method object:
         */
        std::string mymethod() {
            return "Hello World";
        }
        /** @} */

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
            log() << "Hello Command: "<< arg << endlog();
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

        /**
         * @name Event
         * @{
         */
        /**
         * The event takes a payload which is distributed
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
              // Name, description, value
              property("the_property", "the_property Description", "Hello World"),
              // Name, value
              attribute("the_attribute", "Hello World"),
              // Name, value
              constant("the_constant", "Hello World"),
              // Name, initial value
              dataport("hello_data_port","World"),
              // Name, buffer size, initial value
              bufferport("hello_buffer_port",13, "World"),
              // Name, function pointer, object
              method("the_method", &Hello::mymethod, this),
              // Name, command function pointer, completion condition function pointer, object
              command("the_command", &Hello::mycommand, &Hello::mycomplete, this),
              // Name
              event("the_event")
        {
            // Check if all initialisation was ok:
            assert( property.ready() );
            assert( attribute.ready() );
            assert( constant.ready() );
            assert( method.ready() );
            assert( command.ready() );
            assert( event.ready() );

            // Now add it to the interface:
            this->properties()->addProperty(&property);

            this->attributes()->addAttribute(&attribute);
            this->attributes()->addConstant(&constant);

            this->ports()->addPort(&dataport);
            this->ports()->addPort(&bufferport);

            this->methods()->addMethod(&method, "'the_method' Description");
        
            this->commands()->addCommand(&command, "'the_command' Description",
                                         "the_arg", "Use 'World' as argument to make the command succeed.");

            this->events()->addEvent(&event, "'the_event' Description",
                                     "the_data", "The data of this event.");
        }
    };
}

/**
 * See Orocos Deployment Component Manual (An OCL component).
 * Exercise: Add the necessary OCL header include and C macro such that the
 * Example::Hello class becomes a loadable Orocos component.
 */

