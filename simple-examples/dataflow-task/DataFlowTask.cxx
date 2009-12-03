#include <rtt/TaskContext.hpp>
#include <rtt/PeriodicActivity.hpp>
#include <rtt/MethodC.hpp>
#include <rtt/CommandC.hpp>
#include <rtt/EventC.hpp>
#include <rtt/ConnectionC.hpp>
#include <rtt/ConnectionFactory.hpp>
#include <rtt/BufferPort.hpp>
#include <rtt/DataPort.hpp>
#include <iostream>
#include <rtt/os/main.h>

#include <ocl/TaskBrowser.hpp>

using namespace Orocos;
using namespace std;

/**
 * Whatever kind of data can be communicated between tasks.
 */
struct MyDataStruct
{
    double X;
    double Y;
    double Z;
};

/**
 * This class implements the 'DataFlow' examples given in
 * The Orocos Task Context Manual.
 */
class MyTask_1 
    : public RTT::TaskContext
{
    BufferPort<double> bufPort;
    ReadDataPort<double> inpPort;
    WriteDataPort<MyDataStruct> outPort;
public:
    /**
     * Constructor. Sets up the interface of this TaskContext.
     */
    MyTask_1(std::string name) 
        : RTT::TaskContext(name),
          bufPort("BufferData", 10, 0.0),
          inpPort("X_Data"),
          outPort("Y_Data")
    {
        // Setup a read-write buffer port.
        this->ports()->addPort( &bufPort, "Buffered read/write port." ); 
        // Setup a read-only data port.
        this->ports()->addPort( &inpPort, "This component reads X data." );
        // Setup a read-write data port.
        this->ports()->addPort( &outPort, "This component writes User defined data."  );

    }
  
    /**
     * This function contains the application's startup code.
     * Return false to abort startup.
     */
    bool startHook() {
        // check that we are properly connected.
        if ( ! bufPort.connected() || ! inpPort.connected() || ! outPort.connected() ) {
            Logger::log() << Logger::Error << "Not all ports were properly connected. Aborting."<<Logger::endl;
            if ( !bufPort.connected() )
                Logger::log() << bufPort.getName() << " not connected."<<Logger::endl;
            if ( !inpPort.connected() )
                Logger::log() << inpPort.getName() << " not connected."<<Logger::endl;
            if ( !outPort.connected() )
                Logger::log() << outPort.getName() << " not connected."<<Logger::endl;
            return false;
        }

        // Write initial values.
        MyDataStruct mds;
        mds.X = 1.0;
        mds.Y = 2.0;
        mds.Z = mds.X * mds.Y;

        outPort.data()->Set( mds );

        return true;
    }

    /**
     * This function is periodically called.
     */
    void updateHook() {
        // Read the inbound ports:
        double bufval;
        // only write result if a value could be found
        // in the buffer.
        if ( bufPort.buffer()->Pop(bufval) ) {
            double inval = inpPort.data()->Get();

            // Do a 'calculation' :
            MyDataStruct mds;
            mds.X = bufval;
            mds.Y = inval;
            mds.Z = bufval * inval;

            // write the result:
            outPort.data()->Set( mds );
        }
    }

    /**
     * This function is called when the task is stopped.
     */
    void stopHook() {
        // Your cleanup code
    }
};

/**
 * This class implements the 'DataFlow' examples given in
 * The Orocos Task Context Manual.
 */
class MyTask_2
    : public RTT::TaskContext
{
    BufferPort<double> bufPort;
    WriteDataPort<double> outPort;
    ReadDataPort<MyDataStruct> inpPort;
public:
    /**
     * Constructor. Sets up the interface of this TaskContext.
     */
    MyTask_2(std::string name) 
        : RTT::TaskContext(name),
          bufPort("BufferData",10),
          outPort("X_Data"),
          inpPort("Y_Data")
    {
        // Setup a read-write buffer port.
        this->ports()->addPort( &bufPort, "Buffered read/write port." ); 
        // Setup a read-only data port.
        this->ports()->addPort( &inpPort, "This component writes X data." );
        // Setup a read-write data port.
        this->ports()->addPort( &outPort, "This component reads User defined data."  );
    }
  
    /**
     * This function contains the application's startup code.
     * Return false to abort startup.
     */
    bool startHook() {
        // check that we are properly connected.
        if ( ! bufPort.connected() || ! inpPort.connected() || ! outPort.connected() ) {
            Logger::log() << Logger::Error << "Not all ports were properly connected. Aborting."<<Logger::endl;
            if ( !bufPort.connected() )
                Logger::log() << bufPort.getName() << " not connected."<<Logger::endl;
            if ( !inpPort.connected() )
                Logger::log() << inpPort.getName() << " not connected."<<Logger::endl;
            if ( !outPort.connected() )
                Logger::log() << outPort.getName() << " not connected."<<Logger::endl;
            return false;
        }
        // Write initial values.
        outPort.data()->Set( 3.0 );
        return true;
    }

    /**
     * This function is periodically called.
     */
    void updateHook() {
        // Read the inbound ports:
        MyDataStruct mds;
        mds = inpPort.data()->Get();

        // Do a 'calculation' :
        double bufval = mds.X;
        double outval = mds.Y;

        if ( mds.Z != bufval * outval ) {
            Logger::log() << Logger::Error << "Corrupt data detected !" << Logger::endl;
        }

        // write the result:
        // if buffer not full, also write the outPort.
        while ( bufPort.buffer()->Push( bufval+1 ) )
            outPort.data()->Set( outval-1 );
    }

    /**
     * This function is called when the task is stopped.
     */
    void stopHook() {
        // Your cleanup code
    }
};

/**
 * main() function
 */
int ORO_main(int arc, char* argv[])
{
    // Set log level more verbose than default,
    // such that we can see output :
    if ( Logger::log().getLogLevel() < Logger::Info ) {
        Logger::log().setLogLevel( Logger::Info );
        Logger::log() << Logger::Info << argv[0] << " manually raises LogLevel to 'Info' (5). See also file 'orocos.log'."<<Logger::endl;
    }

    MyTask_1 a_task("ATask");
    MyTask_2 b_task("BTask");

    a_task.connectPeers( &b_task );
    a_task.connectPorts( &b_task );

    // ... make both tasks periodic
    a_task.setActivity( new PeriodicActivity(OS::HighestPriority, 0.01 ) );
    b_task.setActivity( new PeriodicActivity(OS::HighestPriority, 0.1 ) );

    a_task.start(); 
    b_task.start(); 


    /**
     * Browse the task.
     */
    TaskBrowser browser( &a_task );

    /**
     * Start the console reader.
     */
    browser.loop();

    a_task.stop();
    b_task.stop();

    return 0;
}
