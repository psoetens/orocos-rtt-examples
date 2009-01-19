/*
 * Automatic.hpp
 *
 *  Created on: 28-okt-2008
 *      Author: sspr
 */

#ifndef AUTOMATIC_HPP_
#define AUTOMATIC_HPP_

#include <rtt/TaskContext.hpp>

#include <rtt/Ports.hpp>
#include <rtt/Command.hpp>
#include <rtt/Event.hpp>

namespace UseCase
{
	using namespace RTT;

	class Automatic
		: public RTT::TaskContext
	{
	protected:
		double target, step;
		bool target_reached;
		Command<bool(double)> move;
		Event<void(double)> atposition;
		DataPort<double> output;
		DataPort<double> input;

		bool move_impl(double d) {
			target = input.Get() + d;

			// step and d must have same sign.
			if ( d * step < 0)
				step = -step;

			if ( d < step ) {
				output.Set(target);
				log(Info) << "Did instant move to target "<< target <<endlo();
				return true;
			}

			target_reached = false;
			log(Info) << "Moving to target " << target << endlog();
			return true;
		}

		bool atpos_impl(double d) {
			if ( output.Get() - target < step && output.Get() - target > -step )
				return true;
			return false;
		}

	public:
		Automatic(const std::string& name) :
			TaskContext(name, PreOperational),
			target(0.0), step(0.0), target_reached(true),
			move("move",&Automatic::move_impl, &Automatic::atpos_impl, this),
			atposition("atposition"),
			output("output"),
			input("input")
		{
			this->commands()->addCommand(&move, "Move over a distance", "d",
					"Distance to move");
			this->events()->addEvent(&atposition, "Emited when the position is reached", "p", "The position reached.");
			this->ports()->addPort(&output,"Sends the interpolated target position.");
			this->ports()->addPort(&input, "Reads in the current position.");
		}

		bool configureHook() {
			Logger::In in("Automatic::configureHook()");
			if ( output.connected() == false ) {
				log(Error) << "Automatic Output port not connected!"<<endlog();
				return false;
			}
			if ( input.connected() == false ) {
				log(Error) << "Automatic Input port not connected!"<<endlog();
				return false;
			}

			if ( this->getPeriod() >= 1 ) {
				log(Error) << "Please choose an execution period < 1."<<endlog();
				return false;
			}

			return true;
		}

		bool startHook() {
			// set our interpolation step to 1 unit/s.
			step = this->getPeriod();

			return true;
		}

		void updateHook() {
			output.Set( output.Get() + step );
			if ( atpos_impl(target) && !target_reached) {
				output.Set(target);
				log(Info) << "Position reached at " << target <<endlog();
				atposition(target);
				step = 0.0;
				target_reached = true;
			}
		}

		void stopHook() {
			step = 0.0;
		}

		void cleanupHook() {
		}

	};
}

#endif /* AUTOMATIC_HPP_ */
