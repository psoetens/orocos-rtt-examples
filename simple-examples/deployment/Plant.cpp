
#include <ocl/ComponentLoader.hpp>
#include <rtt/TimeService.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Property.hpp>
#include <rtt/Ports.hpp>
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
    WriteDataPort<double> position;
    WriteDataPort<double> velocity;
    BufferPort<double> setpoints;
    TimeService::ticks stamp;
    // Internal state variables
    double pos,vel;
public:
    PlantType(std::string name)
        : TaskContext(name, PreOperational), // require configuration.
          position("Position", 0.0),
          velocity("Velocity", 0.0),
          setpoints("Setpoints", 2),
          stamp(0), pos(0.0),vel(0.0)
    {
        this->ports()->addPort( &position, "1D Position of this plant.");
        this->ports()->addPort( &velocity, "1D Velocity of this plant.");
        this->ports()->addPort( &setpoints, "1D drive setpoint of this plant.");
    }

    /**
     * Reimplement the TaskCore stop() method.
     */
    bool stop()
    {
        if (this->isRunning() == false)
            return false;
        setpoints.Push(0.0);
        return TaskCore::stop();
    }

    bool configureHook()
    {
        if ( !setpoints.ready() || !position.ready() ) {
            log(Error) << "Refusing to configure without connected ports."<<endlog();
            return false;
        }

        // We reconfigure the buffered data connection to allow us to block on empty.
        setpoints = new BufferLockFree<double, BlockingPolicy, NonBlockingPolicy>(20);
        return true;
    }

    bool startHook()
    {
        // reset timestamp
        stamp = TimeService::Instance()->getTicks();
        return true;
    }

    void updateHook()
    {
        // Read setpoint, this is blocking on empty, see configureHook()
        bool ret = setpoints.Pop( vel );
        if (ret == false) {
            // The current blocking API always returns true.
            // assert(false);
        } else
            pos += vel * TimeService::Instance()->secondsSince(stamp);
        stamp = TimeService::Instance()->getTicks();

        position.Set( pos );
        velocity.Set( vel );

        // Inform thread to call updateHook again after this one.
        this->engine()->getActivity()->trigger();
    }

};

ORO_CREATE_COMPONENT(PlantType)
