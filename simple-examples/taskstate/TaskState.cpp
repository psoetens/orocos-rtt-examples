#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>
#include <rtt/Property.hpp>
#include <rtt/Ports.hpp>
#include <rtt/os/main.h>
#include <rtt/TimeService.hpp>

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
    // Read-Write buffer port:
    BufferPort<double> rwbufPort;

    /**
     * Task's Properties.
     */
    Property<int> bufSize;
    Property<double> initial_value;
    Property<bool> read_propfile;
    Attribute<double> runtime;
    TimeService::ticks stamp;
public:
    /**
     * Constructor. Sets up the interface of this TaskContext.
     */
    MyTask(std::string name) 
        : RTT::TaskContext(name, PreOperational),
          rwbufPort("Packets", 30),
          bufSize("BufSize","Configurable buffer size of Packets",30),
          initial_value("InitialValue","Value used to write into Packet Buffer", 1.23 ),
          read_propfile("ReadPropFile","Set to true if properties must be read from disk upon configure().", true),
          runtime("RunTime", 0.0 )
    {
        /**
         * Export ports to interface:
         * See also the 'dataflow-task' example !
         */
        this->ports()->addPort( &rwbufPort );

        /**
         * Attribute/Property Interface
         */
        this->properties()->addProperty( &bufSize );
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

        if ( read_propfile.get() == true ) {
            if ( this->marshalling()->updateProperties("TaskState.cpf") == false )
                return false;
        } else
            log(Info) << "Skipping TaskState.cpf -- change ReadPropFile to true for reading XML file."<<endlog();
        
        if ( bufSize.get() < 0 ) {
            log(Error) << "A negative buffer size is not accepted."<<endlog();
            return false;
        }

        if ( rwbufPort.connected() == false ) {
            log(Info) << "Setting Buffer size to "<< bufSize.get() <<endlog();
            rwbufPort.setBufferSize( bufSize.get() ); // setup unconnected port
        } else {
            log(Warning) << rwbufPort.getName() << " port already connected, can not change buffer size without breaking up connection."<<endlog();
            log(Warning) << "Call cleanup() in order to disconnect."<<endlog();
            //rwbufPort.connection()->disconnect(); 
            //rwbufPort.setBufferSize( bufSize.get() );
        }
        
        return true;
    }

    /**
     * This function is for the application's startup code.
     * Return false to abort startup.
     */
    bool startHook() {
        // ...
        if ( rwbufPort.ready() == false ) {
            log(Warning)<<"Packet port not connected !"<<endlog();
        }
        // start recording runtime.
        stamp = TimeService::Instance()->getTicks();
        return true;
    }

    /**
     * This function is periodically called.
     */
    void updateHook() {
        // Your algorithm for periodic execution goes inhere
        // gradually fill and empty at once if full.

        double value = 0;
        if ( rwbufPort.full() )
            while ( !rwbufPort.empty() )
                rwbufPort.Pop( value );

        // set the attribute with the elapsed time since startHook()
        runtime.set( TimeService::Instance()->secondsSince( stamp ) );

        rwbufPort.Push( initial_value.get() + runtime.get() );
    }

    /**
     * This function is called when the task is stopped.
     */
    void stopHook() {
        // Your stop code after last updateHook()

        // empty remaining items.
        double value;
        while ( rwbufPort.ready() && !rwbufPort.empty() )
            rwbufPort.Pop( value );

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
        if ( rwbufPort.connected() ) {
            log(Info) << "Disconnecting Buffer port."<<endlog();
            rwbufPort.disconnect();
        }
        assert( rwbufPort.connected() == false );
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
    a_task.setActivity( new PeriodicActivity(OS::HighestPriority, 0.01 ) );

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
