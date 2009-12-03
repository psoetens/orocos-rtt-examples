#include <rtt/TaskContext.hpp>
#include <rtt/Command.hpp>
#include <rtt/Method.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Ports.hpp>
#include <rtt/RTT.hpp>

#include <cmath>
#include <iostream>

using namespace Orocos;
using namespace RTT::Corba;
using namespace std;


/**
 * An example of an Orocos Component.
 */
struct ExecutionServer
    : public TaskContext
{
    Property<int> sprop;
    Property<string> strprop;
    Property<PropertyBag> bprop;
    Property<vector<double> > vprop;
    Attribute<bool> truth;
    Attribute<string> word;
    Constant<double> pi;
    Attribute<double> d1;
    Attribute<int> i1;
    Attribute<std::vector<double> > vattr;

    DataPort<int> xport;
    BufferPort<double> yport;

    ExecutionServer(std::string n)
        : TaskContext(n),
          sprop("Strength","The strenght", 5),
          strprop("Sentence","The sentence", "To be or not to be."),
          bprop("SubBag","Demo for hierchical properties"),
          vprop("Array","Sequence of numbers."),
          truth("Truth",false),
          word("Word","silence"),
          pi("pi", M_PI), d1("d1", 0.0), i1("i1", 0),
          vattr("List"),
          xport("XP"),
          yport("YP",24)
    {
        // Add commands:
        this->commands()->addCommand( command("command", &ExecutionServer::escommand,
                                       &ExecutionServer::escondition, this),
                                       "Command Description.",
                                       "A1","Argument 1",
                                       "A2","Argument 2",
                                       "A3","Argument 3");

        // Add attributes and properties:
        attributes()->addAttribute( &truth);
        attributes()->addAttribute( &word);
        attributes()->addAttribute( &d1);
        attributes()->addAttribute( &i1);
        attributes()->addConstant( &pi );
        properties()->addProperty( &sprop );

        bprop.set().add( &sprop );
        bprop.set().add( &strprop );
        properties()->addProperty( &bprop );

        properties()->addProperty( &vprop );
        attributes()->addAttribute( &vattr );

        ports()->addPort( &xport, "X Port");
        ports()->addPort( &yport, "Y Port");

        std::vector<double> v(10,3.3);
        vprop.set(v);
        v.resize(12, 1.1);
        vattr.set(v);
    }

    bool escommand( int a, int b, int c)
    {
        Logger::In in("ExecutionServer");
        Logger::log() <<Logger::Info << "escommand( "<<a<<", "<<b<<", "<<c<<" ) received."<<Logger::endl;
        return true;
    }

    bool escondition(int a ) const
    {
        Logger::In in("ExecutionServer");
        Logger::log() <<Logger::Info << "condition( "<<a<<" ) checked."<<Logger::endl;
        return true;
    }

};

