
#include <rtt/corba/ControlTaskServer.hpp>
#include <rtt/corba/ControlTaskProxy.hpp>

#include <rtt/PeriodicActivity.hpp>
#include <rtt/Logger.hpp>
#include <cmath>
#include <rtt/os/main.h>
#include <ocl/TaskBrowser.hpp>

#include "ExecutionServer.hpp"

using namespace std;
using namespace RTT;
using namespace RTT::Corba;
using namespace Orocos;

int ORO_main(int argc, char** argv)
{
    Logger::In in("ExecutionServer");
    if (Logger::log().getLogLevel() < Logger::Info )
        Logger::log().setLogLevel(Logger::Info);

    // we 'distribute' peer and keep local local.
    ExecutionServer local("ExecutionDemo");
    ExecutionServer peer("ExecutionPeer");

    // ... make task periodic
    local.setActivity( new PeriodicActivity( OS::HighestPriority, 0.01 ) );
    peer.setActivity( new PeriodicActivity( OS::HighestPriority + 1, 0.01 ) );

    ControlTaskServer::InitOrb(argc, argv);

    ControlTaskServer* corba_server = ControlTaskServer::Create( &peer );

    assert(corba_server);

    local.start();

    // Connect to server.
    ControlTaskProxy* mtask = ControlTaskProxy::Create("ExecutionPeer");
    if (mtask == 0)
        Logger::log() << Logger::Error << "Could not connect to server"<<Logger::endl;
    else {

        // peer to server over CORBA:
        if ( connectPeers( &local, mtask ) == false )
            log(Error) << "Could not connect peers !"<<endlog();

        connectPorts( &local, mtask );

        TaskBrowser browser( &peer );

        // fire-up interactive prompt.
        browser.loop();
    };

    local.stop();
    peer.stop();

    ControlTaskServer::ShutdownOrb();
    ControlTaskServer::DestroyOrb();

    delete mtask;

    return 0;
}

