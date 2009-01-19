/*
 * Plant.hpp
 *
 *  Created on: 29-okt-2008
 *      Author: sspr
 */

#ifndef PLANT_HPP_
#define PLANT_HPP_

#include <rtt/TaskContext.hpp>

#include <rtt/Ports.hpp>
#include <rtt/Properties.hpp>
#include <rtt/Method.hpp>
#include <rtt/Command.hpp>
#include <rtt/Event.hpp>

namespace UseCase
{
	using namespace RTT;
	class Plant
		: public RTT::TaskContext {
	protected:
		Property<double> inertia;
		DataPort<double> input;
		DataPort<double> output;

	public:
		Plant(const std::string& name)
			: TaskContext(name, PreOperational),
			inertia("inertia","Inertia of the plant.", 10.0),
			input("Input"),
			output("Output")
		{
			this->properties()->addProperty(&inertia);
			this->ports()->addPort(&input);
			this->ports()->addPort(&output);
		}

		bool configureHook() {
			return true;
		}

		bool startHook() {
			return true;
		}

		void updateHook() {
			output.Set( output.Get() + this->engine()->getActivity()->getPeriod()*input.Get()/inertia.value() );
		}

		void stopHook() {
		}

		void cleanupHook() {
		}

};

}

#endif /* PLANT_HPP_ */
