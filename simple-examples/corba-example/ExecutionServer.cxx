
#include <rtt/transports/corba/ControlTaskServer.hpp>

#include <rtt/Activity.hpp>
#include <rtt/Logger.hpp>
#include <rtt/os/main.h>

#include "ExecutionServer.hpp"

using namespace std;
using namespace RTT::corba;
using namespace Orocos;

int ORO_main(int argc, char** argv)
{
    Logger::In in("ExecutionServer");
    if (Logger::log().getLogLevel() < Logger::Info )
        Logger::log().setLogLevel(Logger::Info);

    // Create a component. See ExecutionServer.hpp
    ExecutionServer server("ExecutionDemo");

    // ... make task periodic
    server.setActivity( new Activity( os::HighestPriority, 0.01 ) );

    ControlTaskServer::InitOrb(argc, argv);

    ControlTaskServer::Create( &server );

    server.start();

    ControlTaskServer::RunOrb();

    server.stop();

    ControlTaskServer::ShutdownOrb();
    ControlTaskServer::DestroyOrb();

    return 0;
}

