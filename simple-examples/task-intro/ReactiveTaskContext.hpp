
#include <rtt/TaskContext.hpp>
#include <rtt/scripting/StateMachine.hpp>
#include <rtt/Activity.hpp>
#include <rtt/Event.hpp>
#include <rtt/Logger.hpp>

using namespace RTT;

/**
 * A periodic TaskContext which reacts to events.
 * It shows TWO possible ways: in C++ and using a script StateMachine.
 * The C++ code to setup event reaction is here found in the Constructor,
 * the script is called "ReactiveSM.osd" and is loaded by the ORO_main function.
 */
class ReactiveTaskContext
    : public TaskContext
{
    Activity mytask;
    Attribute<int> last_prime;
    /**
     * Stores the Event Connection.
     */
    Handle shandle;
    Handle ahandle;
public:
    ReactiveTaskContext(const std::string& name)
        : TaskContext(name),
          mytask(RTT::os::HighestPriority + RTT::os::IncreasePriority, 0.1, this->engine() ),
          last_prime("last_prime")
    {
        this->attributes()->addAttribute( &last_prime );

        // register our event handler functions to an event, using the nameserver.
        Event<void(int, int)>* source;
        source = Event<void(int, int)>::nameserver.getObject("PrimeEvent");
        if ( source == 0 ) {
            Logger::log()<<Logger::Error<< "ReactiveTaskContext could not find PrimeEvent !"<<Logger::endl;
        } else {
            shandle = source->setup( boost::bind(&ReactiveTaskContext::handleAEvent, this, _1, _2) );
            ahandle = source->setup(  boost::bind(&ReactiveTaskContext::deferedAEvent, this, _1, _2),
                                      this->engine()->events() );
            Logger::log()<<Logger::Info<< "ReactiveTaskContext: SYN/ASYN reaction to PrimeEvent ready."<<Logger::endl;
            Logger::log()<<Logger::Info<< "ReactiveTaskContext: Waiting for start() to react to Asyn Events."<<Logger::endl;
        }
    }

    ~ReactiveTaskContext()
    {
    }

    /**
     * This method is directly called when the event is emitted.
     */
    void handleAEvent(int a, int b)
    {
        Logger::log()<<Logger::Info<< "ReactiveTaskContext reacts directly (Syn) to PrimeEvent("<<a<<", "<< b <<")" << Logger::endl;
    }

    /**
     * This method is called within our ExecutionEngine when the event is emitted.
     */
    void deferedAEvent(int a, int b)
    {
        Logger::log()<<Logger::Info<< "ReactiveTaskContext reacts defered (Asyn) to PrimeEvent("<<a<<", "<< b <<")" << Logger::endl;
    }

    /**
     * start this task, accept events and commands.
     */
    virtual bool startHook()
    {
      scripting::StateMachinePtr sm = this->engine()->states()->getStateMachine("reactiveMachine");
        if (sm) {
            sm->activate();
            sm->start();
        } else
            Logger::log()<<Logger::Warning<< "ReactiveTaskContext did not find its state machine !"<<Logger::endl;

        // connect our 'handle' functions to the event.
        shandle.connect();
        ahandle.connect();

        Logger::log()<<Logger::Info<< "ReactiveTaskContext starts reacting to Asyn PrimeEvent !"<<Logger::endl;
        return true;
    }

    /**
     * updateHook() is called by the engine after all commands,events
     * etc. are handled.
     */
    virtual void updateHook() {
    }

    virtual void stopHook()
    {
        scripting::StateMachinePtr sm = this->engine()->states()->getStateMachine("reactiveMachine");
        if (sm)
            sm->deactivate();

        // disconnect our 'handle' functions from the event.
        shandle.disconnect();
        ahandle.disconnect();

        Logger::log()<<Logger::Info<< "ReactiveTaskContext stops reacting to Asyn PrimeEvent."<<Logger::endl;
    }
};
