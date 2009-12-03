
#include <ocl/ComponentLoader.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Property.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/OutputPort.hpp>
#include <rtt/Method.hpp>
#include <rtt/RTT.hpp>

using namespace Orocos;

/**
 * The Controller component from the example in the Deployment
 * Component manual.
 */
class ControllerType
    : public TaskContext
{
    InputPort<double> sensorValues;
    OutputPort<double> steeringSignals;
    Property<double> gain;

    // Generates a sine, used in the controller-program.ops script
    double tmp;
    double nextPosition() {
        double w = 1.0 * 2 * 3.14;
        tmp += 0.001;
        return 5*sin( w * tmp);
    }
public:
    ControllerType(std::string name)
        : TaskContext(name),
          sensorValues("SensorValues"),
          steeringSignals("SteeringSignals", 30),
          gain("Gain","The proportional gain."),
          tmp(0.0)
    {
        this->properties()->addProperty(&gain);

        this->ports()->addPort( &sensorValues, "1D Measurement of this controller.");
        this->ports()->addPort( &steeringSignals, "1D Action of this controller");

        this->methods()->addMethod( method("nextPosition", &ControllerType::nextPosition,this), "Returns a reference position.");
    }

    // The implementation of this component is completely in the controller-program.ops script
    bool startHook()
    {
        base::ProgramInterfacePtr pi = this->engine()->programs()->getProgram("ControllerAction");
        if (pi)
            return pi->start();
        return false;
    }

    void stopHook()
    {
        base::ProgramInterfacePtr pi = this->engine()->programs()->getProgram("ControllerAction");
        if (pi)
            pi->stop();
    }
        
};

ORO_CREATE_COMPONENT(ControllerType)
