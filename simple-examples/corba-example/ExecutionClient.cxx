
#include <rtt/Logger.hpp>
#include <rtt/corba/ControlTaskProxy.hpp>
#include <rtt/os/main.h>
#include <iostream>

#include <ocl/TaskBrowser.hpp>

using namespace std;
using namespace RTT::Corba;
using namespace Orocos;

int ORO_main(int argc, char** argv)
{
    Logger::In in("ExecutionClient");
    if (Logger::log().getLogLevel() < Logger::Info )
        Logger::log().setLogLevel(Logger::Info);

    ControlTaskProxy::InitOrb(argc, argv);

    // Get name from commandline.
    std::string servername("ExecutionDemo");
    bool is_ior = false;

    if ( argc == 2 ) {
        servername = argv[1];
    }

    if (servername.substr(0,3) == "IOR" ) {
        log(Info) << "Using user given IOR." <<endlog();
        is_ior = true;
    }

    // Connect to server.
    ControlTaskProxy* mtask = ControlTaskProxy::Create( servername, is_ior );

    TaskBrowser browser( mtask );

    // fire-up interactive prompt.
    browser.loop();

    delete mtask;

    ControlTaskProxy::DestroyOrb();

    return 0;
}
