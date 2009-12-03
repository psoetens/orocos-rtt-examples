
/**
 * This file demonstrate three tasks which interact
 * which each other.
 */

#include <rtt/os/main.h>

#include "WaitingTaskContext.hpp"
#include "PeriodicTaskContext.hpp"
#include "ReactiveTaskContext.hpp"

#include <ocl/TaskBrowser.hpp>
#include <rtt/Logger.hpp>

// Detect if user has setup Corba...
#include <rtt/RTT.hpp>
#ifdef OROPKG_CORBA
#define USE_CORBA
#include <rtt/corba/ControlTaskServer.hpp>
using namespace RTT::Corba;
#endif

using namespace Orocos;

int ORO_main(int argc, char** argv)
{
    // Set log level more verbose than default,
    // such that we can see output :
    if ( Logger::log().getLogLevel() < Logger::Info ) {
        Logger::log().setLogLevel( Logger::Info );
        Logger::log() << Logger::Info << argv[0] << " manually raises LogLevel to 'Info' (5). See also file 'orocos.log'."<<Logger::endl;
    }

    WaitingTaskContext wc("FactoringTask");
    PeriodicTaskContext tc("PeriodicTask");
    ReactiveTaskContext rc("ReactiveTask");

    // Setup a triangle-like network
    wc.connectPeers( &tc );
    wc.connectPeers( &rc );
    rc.connectPeers( &tc );


    // Load a state machine after the Peer network
    // is setup.
    tc.scripting()->loadStateMachines("CountingSM.osd");

    rc.scripting()->loadStateMachines("ReactiveSM.osd");

    TaskBrowser browser( &tc );

    // start all tasks...
    wc.start();
    tc.start();
    rc.start();

#ifdef USE_CORBA
    // Setup Corba Server:
    ControlTaskServer::InitOrb( argc, argv );
    ControlTaskServer::Create( &rc );

    // Run the orb in its own thread:
    ControlTaskServer::ThreadOrb();
#endif

    // Accept user commands from console.
    browser.loop();

    // stop all tasks...
    wc.stop();
    tc.stop();
    rc.stop();

#ifdef USE_CORBA
    // Cleanup corba:
    ControlTaskServer::ShutdownOrb();
    ControlTaskServer::DestroyOrb();
#endif

    return 0;
}
