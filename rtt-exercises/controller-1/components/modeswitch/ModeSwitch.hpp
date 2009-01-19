#ifndef APPLICATION_SETUP_HPP
#define APPLICATION_SETUP_HPP

#include <rtt/TaskContext.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Event.hpp>

namespace UseCase
{
	using namespace RTT;

    /**
     * Control switching between automatic and joystick mode.
     */
    class ModeSwitch
        : public TaskContext
    {
        /* Exercise: Add an event 'switchMode' which takes a
         * string as argument. The string can be "Automatic"
         * or "Manual". You will emit this event from the
         * TaskBrowser console.
         */

        /* Exercise: Add an Attribute of type bool which
         * contains the status of a safety switch.
         */
    public:
        ModeSwitch(const std::string& name)
            : TaskContext(name)
        {
        }

        ~ModeSwitch()
        {
        }

        /**
         * Exercise: Implement startHook() and stopHook()
         * which activate, start and stop the state machine loaded in
         * this component.
         */

        /**
         * Exercise: Write an updateHook() function that
         * checks the status of the safety switch. If it isn't
         * true, emit the mode switch event with the value "manual".
         */
};

}

#endif
