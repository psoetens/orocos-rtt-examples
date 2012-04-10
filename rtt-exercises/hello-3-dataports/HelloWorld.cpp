
/**
 * @file HelloWorld.cpp
 * This file demonstrates the Orocos Input/Output Ports primitives with
 * a 'hello world' example.
 */

#include <rtt/Logger.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Attribute.hpp>

/**
 * Include this header in order to use data ports.
 */
#include <rtt/Port.hpp>
#include <rtt/Component.hpp>


using namespace std;
using namespace RTT;

/**
 * Exercise 3: Read Orocos Component Builder's Manual, Chap 2 sect 3.4; Chap 2 sect 3.1
 *
 * Reading and writing Ports.
 *
 * First, compile and run this application and use
 * 'the_output_port' and 'the_input_port' of the Hello component.
 * Use the 'read_helper' attribute to store a read value. Clearly, you should
 * notice that no data is consumed or produced. Check the TaskBrowser documentation
 * on how you can create data ports to a live component.
 *
 * So, write a configureHook() in Hello which checks if the input port is
 * connected and fails if it is not.
 * Question: how did Hello specify it requires a configureHook() call ?
 *
 * Next, write an updateHook() which reads the_input_port and
 * writes the data to 'the_output_port'. Be careful only to write data
 * when a read from the input was succesful.
 *
 * Finally start the World component ('world.start()'). See how it uses the
 * input port in C++. Start and stop the Hello component. See how this
 * influences the input's size.
 *
 * Connection policies.
 *
 * We did not specify yet if connections should be buffered or not. Modify at the
 * bottom of this file the connectTo statement to indicate a buffering policy,
 * buffer size 10 and using locks. Set the period of World's activity to 0.1s
 * and modify updateHook in Hello to keep reading as long as NewData is available
 * and print the results.
 *
 * Analysing real-time behaviour:
 *
 * Create a struct 'Data' that holds an std::vector<double> and logs in
 * the constructor, destructor, copy constructor and operator=. Replace
 * the ports in this example with ports holding <Data>. Re-run the program,
 * Explain when and why data is copied, constructed or destructed in both the
 * default 'data' connection policy and in the 'buffered' connection policy.
 *
 * Now find out how to initialise a connection such that copies are real-time
 * safe with respect to copying the vector of doubles.
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
              inputport("the_input_port"),
              read_helper(0.0)
        {
            this->addAttribute("read_helper", read_helper);
            this->ports()->addPort( outputport ).doc("Data producing port.");
            this->ports()->addPort( inputport ).doc("Data consuming port.");
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
            this->ports()->addPort( my_port ).doc("World's data producing port.");
		}

		void updateHook() {
			my_port.write( value );
			++value;
		}
	};

}

ORO_CREATE_COMPONENT_LIBRARY()
ORO_LIST_COMPONENT_TYPE( Example::Hello )
ORO_LIST_COMPONENT_TYPE( Example::World )

