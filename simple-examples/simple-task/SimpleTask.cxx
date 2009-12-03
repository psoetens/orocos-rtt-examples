#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Method.hpp>
#include <rtt/Command.hpp>
#include <rtt/Ports.hpp>
#include <iostream>
#include <rtt/os/main.h>

#include <ocl/TaskBrowser.hpp>

using namespace Orocos;
using namespace std;


/**
 * See event example below.
 */
class Reactor {
public:
    void callback( double a1, double a2, double a3 ) {
        cout <<"Callback called with args: "<<a1<<", "<< a2<<", " << a3 <<endl;
        // ... read a1, a2, a3
    }
};

/**
 * This class implements the examples given in
 * The Orocos Task Context Manual.
 */
class MyTask 
    : public RTT::TaskContext
{
    /**
     * Task's Data Ports.
     * See also the 'dataflow-task' example !
     */
    // Read-only data port:
    ReadDataPort<double> indatPort;
    // Read-Write data port:
    WriteDataPort<double> outdatPort;
    // Read-only buffer port:
    ReadBufferPort<double> inbufPort;
    // Write-only buffer port:
    WriteBufferPort<double> outbufPort;
    // Read-Write buffer port:
    BufferPort<double> rwbufPort;

    /**
     * Task's Properties.
     */
    Property<std::string> param;
    Property<double> value;

    /**
     * Task's Attributes
     */
    Attribute<bool> aflag;
    Attribute<double> max;
    Attribute<std::vector<double> > d_vector;
    Constant<double> pi;

    /**
     * Task's Events.
     */
    // An event with a bool argument:
    Event< void(bool) > turnSwitch;
    // An event with three arguments:
    Event< void(double, double, double) > moveAxis;

    /**
     * Task's Methods.
     */
    void reset() { cout <<"Method reset() called!"<<endl; }
    string getName() { return "Task"; }
    double changeParameter(double f) { double old = value; value = f; return old; }
    void passByArgument(int& a1, vector<double>& v2) { a1 = 3; v2.push_back(3.0); }

    /**
     * Task's Commands.
     */
    /**
     * The first command starts a cycle.
     */
    bool startCycle() { cout <<"Command startCycle() called!"<<endl; return true; }
    bool cycleDone() const { cout <<"Condition cycleDone() const called!"<<endl; return true; }

    /**
     * Another command cleans stuff up.
     */
    bool cleanupMess(double f) { cout <<"Command cleanupMess(double f) called!"<<endl; return true; }
    bool isMessCleaned() const { cout <<"Condition isMessCleaned() const called!"<<endl; return true; }
public:
    /**
     * Constructor. Sets up the interface of this TaskContext.
     */
    MyTask(std::string name) 
        : RTT::TaskContext(name, PreOperational),
          indatPort("Data_R"),
          outdatPort("Data_W"),
          inbufPort("SetPoint_X"),
          outbufPort("Packet_1", 15), // note: buffer size
          rwbufPort("Packet_2", 30),
          param("Param","Param Description","The String"),
          value("Value","Value Description", 1.23 ),
          aflag("aflag", false), max("max",1.0), 
          d_vector("d_vector", std::vector<double>(5, 1.0) ),
          pi("pi", 3.14),
          turnSwitch("turnSwitch"),
          moveAxis("move")
    {
        /**
         * Export ports to interface:
         * See also the 'dataflow-task' example !
         */
        this->ports()->addPort( &indatPort );
        this->ports()->addPort( &outdatPort );
        this->ports()->addPort( &inbufPort );
        this->ports()->addPort( &outbufPort );
        this->ports()->addPort( &rwbufPort );

        /**
         * Method Factory Interface.
         */

        this->methods()->addMethod( method( "reset", &MyTask::reset, this), "Reset the system."  );
        this->methods()->addMethod( method( "name", &MyTask::getName, this),
                           "Read out the name of the system."  );
        this->methods()->addMethod( method( "changeP", &MyTask::changeParameter, this),
                           "Change a parameter, return the old value.",
                           "New Value", "The new value for the parameter."  ); 
        this->methods()->addMethod( method( "passByArgument", &MyTask::passByArgument, this),
                           "Method which passes results by argument.",
                           "I","Integer passed by reference",
                           "V","Vector passed by reference"  ); 

        /**
         * Attribute/Property Interface
         */
        this->attributes()->addAttribute( &aflag );
        this->attributes()->addAttribute( &max );
        this->attributes()->addAttribute( &d_vector );
        this->attributes()->addConstant( &pi );
      
        this->properties()->addProperty( &param );
        this->properties()->addProperty( &value );

        /**
         * Command Interface
         */

        this->commands()->addCommand( command( "startCycle", &MyTask::startCycle,
                             &MyTask::cycleDone, this),
                             "Start a new cycle."  );
        this->commands()->addCommand( command( "cleanupMess", &MyTask::cleanupMess,
                             &MyTask::isMessCleaned, this),
                             "Start cleanup operation.",
                             "factor", "A factor denoting the thoroughness."  );

        /**
         * Event Interface
         */
        this->events()->addEvent( &turnSwitch, "Emitted when a switch is turned.",
                                  "Value", "The new value of the switch: true or false."); 
        this->events()->addEvent( &moveAxis, "Emitted when an axis starts moving.",
                                  "Position", "The target position.",
                                  "Velocity", "The maximum velocity.",
                                  "Accel", "The maximum acceleration.");
    }
  
    /**
     * This function is for the configuration code.
     * Return false to abort configuration.
     */
    bool configureHook() {
        // ...
        return true;
    }

    /**
     * This function is for the application's startup code.
     * Return false to abort startup.
     */
    bool startHook() {
        // ...
        return true;
    }

    /**
     * This function is periodically called.
     */
    void updateHook() {
        // Your algorithm for periodic execution goes inhere
    }

    /**
     * This function is called when the task is stopped.
     */
    void stopHook() {
        // Your stop code after last updateHook()
    }

    /**
     * This function is called when the task is being deconfigured.
     */
    void cleanupHook() {
        // Your configuration cleanup code
    }
};

/**
 * main() function
 */
int ORO_main(int arc, char* argv[])
{
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

    /**
     * Begin of C++ examples. Use the Task's interface without 'scripting'.
     */

    /**
     * Execute methods:
     */
    {
        // create a method:
        Method<void(void)> reset_meth = a_task.methods()->getMethod<void(void)>("reset");

        // Call 'reset' of a_task:
        reset_meth();  

        // used to store the return value:
        string name;
        Method<std::string(void)> name_meth = 
            a_task.methods()->getMethod<std::string(void)>("name");

        // Call 'getName' of a_task:
        name = name_meth(); 

        cout << "Name was: " << name << endl; 

        // store return value.
        double oldvalue;

        Method<double(double)> mychange_1 =
            a_task.methods()->getMethod<double(double)>("changeP");

        // Call 'changeParameter' of a_task with argument '1.0'
        oldvalue = mychange_1( 1.0 );
        // oldvalue now contains previous value.

        // Call 'changeParameter' of a_task, reads contents of d_arg.
        double d_arg = 5.0;
        mychange_1( d_arg );
    }

    /**
     * Check Attributes and Properties:
     */
    {
        Attribute<bool> attrb;
        attrb = a_task.attributes()->getValue( "aflag" );

        bool result = attrb.get();
        assert( result == false );
        attrb.set(true);
        assert( attrb.get() == a_task.attributes()->getAttribute<bool>("aflag")->get() );

        Property<std::string> prop
            = a_task.properties()->getProperty<std::string>( "Param" );

        assert( prop.get() == "The String" );
        prop.set("New String");
        assert( prop.get() == a_task.properties()->getProperty<string>("Param")->get() );
    }

    /**
     * Execute Commands:
     */
    {
        Command<bool(void)> mycom = a_task.commands()->getCommand<bool(void)>("startCycle");

        // Send 'startCycle' to a_task.
        bool result = mycom();
        // next, check its status:
        bool accepted = mycom.accepted(); // accepted by execution engine?
        bool valid = mycom.valid();       // command was valid (well-formed)?
        bool done = mycom.done();     // command was done?
    
        // const argument (note: 'argC'):
        Command<bool(double)> mycleanupMess_1 =
            a_task.commands()->getCommand<bool(double)>("cleanupMess");

        // Send 'cleanupMess' to a_task with argument '1.0'
        bool result_1 = mycleanupMess_1( 1.0 );

        // Since commands are queued in a FIFO, only waiting for the last
        // one to finish would be enough.
        // Wait for command to finish
        while ( result_1 && !mycleanupMess_1.done() )
            sleep(1);
    }

    /**
     * Emit Events.
     */
    {
        Event<void(double,double,double)> move_event 
            = a_task.events()->getEvent<void(double,double,double)>( "move" );
        // emit the event 'move' with given args:
        move_event(1.0,2.0,3.0);

        // or with variable arguments:
        double a = 0.3, b = 0.2, c = 0.1;
        move_event(a, b, c); 


        /**
         * React to Events.
         */

        // This class is defined above.
        Reactor r;
        Handle h2 
            = a_task.events()->setupConnection("move").callback(&r, &Reactor::callback).handle();
    
        h2.connect(); // connect to event "move"

        move_event(0.3,0.2,0.1); // see previous example.

        // now r.a1 == 0.3, r.a2 == 0.2, r.a3 == 0.1 
        // and callback() was called.

        h2.disconnect(); // disconnect again.
    }

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
