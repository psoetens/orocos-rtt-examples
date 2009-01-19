/*
 * Joystick.hpp
 *
 *  Created on: 28-okt-2008
 *      Author: sspr
 */

#ifndef JOYSTICK_HPP_
#define JOYSTICK_HPP_

#include <rtt/TaskContext.hpp>

#include <rtt/Ports.hpp>
#include <rtt/Properties.hpp>
#include <rtt/Method.hpp>

namespace UseCase
{
	using namespace RTT;

	class Joystick
		: public TaskContext {
	protected:
		double jog_value;
		Property<double> scale;
		Method<void(double)> setPosition;
		DataPort<double> output;

		void methodimpl(double d) {
			jog_value = d;
		}
	public:
		Joystick(const std::string& name) :
			TaskContext(name, PreOperational),
			jog_value(0.0),
			scale("scale", "Description", 1.0),
			setPosition("setPosition", &Joystick::methodimpl, this),
			output("output", 0.0)
		{
			this->properties()->addProperty(&scale);
			this->methods()->addMethod(&setPosition, "Method Description", "d", "Argument");
			this->ports()->addPort(&output);
		}

		bool configureHook() {
			if ( output.connected() == false ) {
				log(Error) << "Joystick Output port not connected!"<<endlog();
				return false;
			}
			return true;
		}

		bool startHook() {
			return true;
		}

		void updateHook() {
			output.Set( scale.value() * jog_value );
		}

		void stopHook() {
		}

		void cleanupHook() {
		}

	};

}

#endif /* JOYSTICK_HPP_ */
