#ifndef APPLICATION_SETUP_HPP
#define APPLICATION_SETUP_HPP

#include <rtt/TaskContext.hpp>
#include <rtt/Attribute.hpp>

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
         Event<void(std::string)> switchMode;

        /* Exercise: Add an Attribute of type bool which
         * contains the status of a safety switch.
         */
         Attribute<bool> safetySwitch;
    public:
        ModeSwitch(const std::string& name)
            : TaskContext(name),
              switchMode("switchMode"),
              safetySwitch("safetySwitch",true )
        {
        	this->events()->addEvent(&switchMode, "Signals a mode switch.",
									 "mode", "The mode to which the application should switch.");
        	this->attributes()->addAttribute(&safetySwitch);
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
            scripting::StateMachinePtr sm = this->engine()->states()->getStateMachine("the_statemachine");
        	if (!sm) {
        		log(Error) << "State Machine the_statemachine not loaded in ModeSwitch."<< endlog();
        		return false;
        	}
        	return sm->activate() && sm->start();
        }

        void stopHook() {
            scripting::StateMachinePtr sm = this->engine()->states()->getStateMachine("the_statemachine");
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
        	if (safetySwitch.get() != true) {
        		switchMode("manual");
        	}
        }
};

}

#endif
