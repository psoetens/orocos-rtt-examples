#include <rtt/TaskContext.hpp>
#include <rtt/Activity.hpp>
#include <rtt/Property.hpp>
#include <rtt/Port.hpp>
#include <rtt/os/main.h>
#include <rtt/os/TimeService.hpp>

#include <ocl/TaskBrowser.hpp>

using namespace Orocos;
using namespace std;


/**
 * This class shows how you can use the TaskContext
 * state semantics to configure your components.
 */
class MyTask
    : public RTT::TaskContext
{
    /**
     * Task's Data Ports.
     */

    InputPort<double> inPort;
    OutputPort<double> outPort;

    /**
     * Task's Properties.
     */
    Property<double> initial_value;
    Property<bool> read_propfile;
    Attribute<double> runtime;
    os::TimeService::ticks stamp;
public:
    /**
     * Constructor. Sets up the interface of this TaskContext.
     */
    MyTask(std::string name) 
        : RTT::TaskContext(name, PreOperational),
          inPort("Packets", ConnPolicy::buffer(30) ),
          outPort("Results" ),
          initial_value("InitialValue","Value used to write into Packet Buffer", 1.23 ),
          read_propfile("ReadPropFile","Set to true if properties must be read from disk upon configure().", true),
          runtime("RunTime", 0.0 )
    {
        /**
         * Export ports to interface:
         * See also the 'dataflow-task' example !
         */
        this->ports()->addPort( &inPort );
        this->ports()->addPort( &outPort );

        /**
         * Attribute/Property Interface
         */
        this->properties()->addProperty( &initial_value );
        this->properties()->addProperty( &read_propfile );
        this->attributes()->addAttribute( &runtime );

    }
  
    /**
     * This function is for the configuration code.
     * We first read our properties, then try to reconfigure
     * the buffer.
     * Return false to abort configuration.
     */
    bool configureHook()
    {

	// For demonstration purposes, we roundtrip our output to our input:
	outPort.connectTo( inPort );

        if ( read_propfile.get() == true ) {
            if ( this->marshalling()->updateProperties("TaskState.cpf") == false )
                return false;
        } else
            log(Info) << "Skipping TaskState.cpf -- change ReadPropFile to true for reading XML file."<<endlog();
        
        return true;
    }

    /**
     * This function is for the application's startup code.
     * Return false to abort startup.
     */
    bool startHook() {
        // ...
        if ( inPort.connected() == false ) {
            log(Warning)<<"Packet port not connected !"<<endlog();
        }
        // start recording runtime.
        stamp = os::TimeService::Instance()->getTicks();
        return true;
    }

    /**
     * This function is periodically called.
     */
    void updateHook() {
        // Your algorithm for periodic execution goes inhere
        // gradually fill and empty at once if full.

        double value = 0;
	while ( inPort.read(value) ); // empty port

        // set the attribute with the elapsed time since startHook()
        runtime.set( os::TimeService::Instance()->secondsSince( stamp ) );

        outPort.write( initial_value.get() + runtime.get() );
    }

    /**
     * This function is called when the task is stopped.
     */
    void stopHook() {
        // Your stop code after last updateHook()

        // empty remaining items.
        double value;
        while ( inPort.read( value ) );

        stamp = 0;
        runtime.set( 0.0 );
    }

    /**
     * This function is called when the task is being deconfigured.
     */
    void cleanupHook() {
        // Your configuration cleanup code
        // Write the configuration state to a file.
        this->marshalling()->writeProperties("TaskState.cpf");

        // Disconnect this port, this may confuse the TaskBrowser slightly.
        if ( inPort.connected() ) {
            log(Info) << "Disconnecting Ports."<<endlog();
            inPort.disconnect();
            outPort.disconnect();
        }
        assert( inPort.connected() == false );
    }
};

/**
 * main() function
 */
int ORO_main(int arc, char* argv[])
{
    Logger::In in("TaskState");
    // Set log level more verbose than default,
    // such that we can see output :
    if ( log().getLogLevel() < Logger::Info ) {
        log().setLogLevel( Logger::Info );
        log( Info ) << argv[0] << " raises LogLevel to 'Info' (5). See also file 'orocos.log'."<<endlog();
    }

    MyTask a_task("ATask");

    // ... make task periodic
    a_task.setActivity( new Activity(os::HighestPriority, 0.01 ) );

    a_task.configure();

    a_task.start();


    TaskBrowser browser( &a_task );

    /**
     * Start the console reader.
     */
    browser.loop();

    /**
     * End of C++ examples, start TaskBrowser to let user play:
     */

    a_task.stop();

    a_task.cleanup();

    return 0;
}
