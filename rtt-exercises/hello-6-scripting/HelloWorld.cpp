
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos Scripting service with
 * a 'hello world' example.
 */

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>

#include <rtt/OperationCaller.hpp>
#include <rtt/Port.hpp>
#include <rtt/scripting/Scripting.hpp>
#include <rtt/Component.hpp>

#include <boost/lambda/lambda.hpp>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace boost;
using namespace RTT;

/**
 * Exercise 6: Read Orocos Component Builder's Manual, Chap 3
 *
 * In this exercise, we will write some basic Orocos program scripts and
 * state machines. The details for the exercise are in program.ops and
 * statemachine.osd. This file contains the almost-ready-to-be-used components
 * in which our scripts will run. Also check the World class below for a small
 * required addition.
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

        // useful for debugging our scripts.
        void say(const string& who, const string& message) {
            cout <<' '<<who<<" :" <<message << endl;
        }

        /** @} */

        InputPort<Data> input;
        OutputPort<Data> output;

        Property<int>    max_data;
        Property<double> gain;
        Property<Data>   gains;
    public:
        /**
         * We initialize the ports, the operations and the properties.
         */
        Hello(std::string name)
            : TaskContext(name),
              input("input"), output("output", true), // keeps last written value.
              max_data("max_data","The maximum number of elements in our Data struct.", 10),
              gain("gain","A single gain.",1.0),
              gains("gains","A vector of gains",Data(max_data.get(),1.0))
        {
            addOperation("multiply", &Hello::multiply, this).doc("Multiplies.")
                    .arg("v","Data").arg("f", "Factor");
            addOperation("divide", &Hello::multiply, this).doc("Divides.")
                    .arg("v","Data").arg("f", "Factor");
            addOperation("say", &Hello::say, this).doc("Logs to cout.")
                    .arg("sender","Who's saying it.").arg("message","The message.");

            ports()->addPort( input );
            ports()->addPort( output );

            addProperty( gain);
            addProperty( gains);
            addProperty( max_data);

            // Installs the scripting service from C++ code:
            this->getProvider<Scripting>("scripting");
        }

        bool configureHook() {
            if (max_data.get() < 1 ) {
                log(Error) <<"Invalid number of max data elements."<<endlog();
                return false;
            }
            // resizing gains property
            Data new_sample = gains.get();
            new_sample.resize(max_data.get(),gain.get());
            gains.set( new_sample );

            // setting output port
            output.setDataSample( gains.get() );
            return true;
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
    	    // Exercise: Add input and output as ports, but add input as an event generating port.
    	}
    };
}

ORO_CREATE_COMPONENT_LIBRARY()
ORO_LIST_COMPONENT_TYPE( Example::Hello )
ORO_LIST_COMPONENT_TYPE( Example::World )
