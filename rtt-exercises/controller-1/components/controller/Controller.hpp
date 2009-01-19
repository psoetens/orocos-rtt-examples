/*
 * Controller.hpp
 *
 *  Created on: 29-okt-2008
 *      Author: sspr
 */

#ifndef CONTROLLER_HPP_
#define CONTROLLER_HPP_

#include <rtt/TaskContext.hpp>

#include <rtt/Ports.hpp>
#include <rtt/Properties.hpp>

namespace UseCase
{
	using namespace RTT;
	class Controller
		: public RTT::TaskContext {
	protected:

		Property<double> Kp;
		DataPort<double> steer;
		DataPort<double> target;
		DataPort<double> sense;

	public:
		Controller(const std::string& name) :
			TaskContext(name, PreOperational),
			Kp("Kp", "Proportional Gain", 50.0),
			steer("Steer",0.0),
			target("Target"),
			sense("Sense")
		{
			this->properties()->addProperty(&Kp);
			this->ports()->addPort(&steer);
			this->ports()->addPort(&target);
			this->ports()->addPort(&sense);
		}

		bool configureHook() {
			/**
			 * Exercise: The Kp property should have been read from an XML
			 * file 'Controller.cpf', using the Deployment Component
			 * (see deployment/application.cpf).
			 * Check here if it contains a valid value.
			 * If Kp is smaller or equal to zero, return false, otherwise true.
			 */
			if (Kp.value() <= 0.0)
				return false;
			return true;
		}

		bool startHook() {
			return true;
		}

		void updateHook() {
			steer.Set( Kp.value() * (target.Get() - sense.Get()));
		}

		void stopHook() {
		}

		void cleanupHook() {
		}

	};

}

#endif /* CONTROLLER_HPP_ */
