
#include <ocl/ComponentLoader.hpp>
#include <rtt/os/TimeService.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Property.hpp>
#include <rtt/Port.hpp>
#include <rtt/RTT.hpp>

using namespace Orocos;

/**
 * The Controller component from the example in the Deployment
 * Component manual.
 */
class PlantType
    : public TaskContext
{
    // Data Ports
    OutputPort<double> position;
    OutputPort<double> velocity;
    InputPort<double> setpoints;
    os::TimeService::ticks stamp;
    // Internal state variables
    double pos,vel;
public:
    PlantType(std::string name)
        : TaskContext(name, PreOperational), // require configuration.
          position("Position", 0.0),
          velocity("Velocity", 0.0),
          setpoints("Setpoints"),
          stamp(0), pos(0.0),vel(0.0)
    {
        this->ports()->addPort( &position, "1D Position of this plant.");
        this->ports()->addPort( &velocity, "1D Velocity of this plant.");
        this->ports()->addPort( &setpoints, "1D drive setpoint of this plant.");
    }

    bool configureHook()
    {
        if ( !setpoints.connected() || !position.connected() ) {
            log(Error) << "Refusing to configure without connected ports."<<endlog();
            return false;
        }

        return true;
    }

    bool startHook()
    {
        // reset timestamp
        stamp = os::TimeService::Instance()->getTicks();
        return true;
    }

    void updateHook()
    {
        // Read setpoint.
        bool ret = setpoints.read( vel );
        if (ret == false) {
            // The current blocking API always returns true.
            // assert(false);
        } else
            pos += vel * os::TimeService::Instance()->secondsSince(stamp);
        stamp = os::TimeService::Instance()->getTicks();

        position.write( pos );
        velocity.write( vel );

        // Inform thread to call updateHook again after this one.
        this->engine()->getActivity()->trigger();
    }

};

ORO_CREATE_COMPONENT(PlantType)
