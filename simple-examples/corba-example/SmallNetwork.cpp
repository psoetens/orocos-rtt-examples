
#include <rtt/transports/corba/ControlTaskServer.hpp>

#include <rtt/Activity.hpp>
#include <rtt/Logger.hpp>
#include <rtt/os/main.h>

#include "ExecutionServer.hpp"

using namespace std;
using namespace RTT::corba;
using namespace Orocos;

/**
 * Set up a small network of peer components, which can
 * all be browsed using the Corba Interface. Only one component
 * is initially exported as a ControlTaskServer. The rest is
 * created automatically.
 */
int ORO_main(int argc, char** argv)
{
    Logger::In in("SmallNetwork");
    if (Logger::log().getLogLevel() < Logger::Info )
        Logger::log().setLogLevel(Logger::Info);

    // Create components. See ExecutionServer.hpp
    ExecutionServer servera("ComponentA");
    ExecutionServer serverb("ComponentB");
    ExecutionServer serverc("ComponentC");
    TaskContext     serverd("ComponentD");

    // Create a network
    connectPeers(&servera, &serverb);
    connectPeers(&servera, &serverc);
    connectPeers(&serverc, &serverd);

    // Connect some ports
    connectPorts(&servera, &serverb);
    connectPeers(&servera, &serverc);

    Activity serveract(1, 0.01, servera.engine() );

    // Setup Corba environment
    ControlTaskServer::InitOrb(argc, argv);

    ControlTaskServer::Create( &servera );

    serveract.start();

    // Listen on netowork socket:
    ControlTaskServer::RunOrb();

    serveract.stop();

    ControlTaskServer::ShutdownOrb();
    ControlTaskServer::DestroyOrb();

    return 0;
}

