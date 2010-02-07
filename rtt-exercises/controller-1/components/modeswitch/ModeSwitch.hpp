#ifndef APPLICATION_SETUP_HPP
#define APPLICATION_SETUP_HPP

#include <rtt/TaskContext.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Port.hpp>
#include <rtt/scripting/ScriptingAccess.hpp>
#include <ocl/TimerComponent.hpp>

namespace UseCase
{
	using namespace RTT;

    /**
     * Control switching between automatic and joystick mode.
     */
    class ModeSwitch
        : public TaskContext
    {
        /* Exercise: Add an output port 'switchMode' which takes a
         * string as argument. The string can be "Automatic"
         * or "Manual". You will write this port from the
         * TaskBrowser console.
         */
         OutputPort<std::string> switchMode;
         InputPort<std::string> switchModeCatcher;

         InputPort<RTT::os::Timer::TimerId> time_event;

        /* Exercise: Add an Attribute of type bool which
         * contains the status of a safety switch.
         */
         bool safetySwitch;
    public:
        ModeSwitch(const std::string& name)
            : TaskContext(name),
              switchMode("switchMode"),
              switchModeCatcher("switchModeCatcher"),
              time_event("timeout"),
              safetySwitch(true)
        {
        	this->ports()->addPort(&switchMode, "Signals a mode switch.");
        	this->ports()->addEventPort(&time_event);
        	this->ports()->addEventPort(&switchModeCatcher);
        	this->addAlias("safetySwitch",safetySwitch);

        	switchMode.connectTo(switchModeCatcher,ConnPolicy::buffer(10,ConnPolicy::BUFFER,true));
        }

        ~ModeSwitch()
        {
        }

        /**
         * Exercise: Implement startHook() and stopHook()
         * which activate, start and stop the state machine loaded in
         * this component, using the Scripting service.
         */
        bool startHook() {
            scripting::ScriptingAccess* sa = dynamic_cast<scripting::ScriptingAccess*>(getService("scripting"));
            scripting::StateMachinePtr sm = sa->getStateMachine("the_statemachine");
        	if (!sm) {
        		log(Error) << "State Machine the_statemachine not loaded in ModeSwitch."<< endlog();
        		return false;
        	}
        	return sm->activate() && sm->start();
        }

        void stopHook() {
            scripting::ScriptingAccess* sa = dynamic_cast<scripting::ScriptingAccess*>(getService("scripting"));
            scripting::StateMachinePtr sm = sa->getStateMachine("the_statemachine");
        	if (!sm) {
        		log(Error) << "State Machine the_statemachine not loaded in ModeSwitch."<< endlog();
        		return;
        	}
        	sm->stop();
        	sm->deactivate();
        }

        /**
         * Exercise: Write an updateHook() function that
         * checks the status of the safety switch. If it isn't
         * true, write the mode switch port with the value "manual".
         */
        void updateHook() {
        	if (safetySwitch != true) {
        		switchMode.write("manual");
        	}
        }
};

}

#endif
