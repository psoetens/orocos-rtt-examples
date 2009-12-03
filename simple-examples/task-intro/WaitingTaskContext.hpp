
#include <rtt/TaskContext.hpp>
#include <rtt/Command.hpp>
#include <rtt/Activity.hpp>
#include <rtt/Event.hpp>
#include <rtt/Logger.hpp>
#include <math.h>

using namespace RTT;

/**
 * This is a non periodic TaskContext
 * which waits to process commands.
 * This task IS NOT REALTIME. It does logging, 
 * memory allocation etc which is not allowed in
 * hard realtime. It serves purely as an example to
 * show the user what is going on.
 */
class WaitingTaskContext
    : public TaskContext
{
    // Our fixed priority :
    Constant<int> priority_;

    // A busy flag for our command.
    bool busy;

    // store primes
    std::vector<int> primes;

    // A non periodic task from RTT
    Activity mytask;

    // Fire this event when ready factoring primes.
    Event<void(int,int)> primeEvent;
public:
    /**
     * This TaskContext has its own nonperiodic task
     * which will execute its Processor's command queue.
     */
    WaitingTaskContext(const std::string& name )
        : TaskContext(name),
          priority_("Priority", 25 ),
          mytask(priority_.get(), this->engine() ),
          primeEvent("PrimeEvent")
    {
        // Add our commands to the TaskContext :
        this->commands()->addCommand( command( "factor", &WaitingTaskContext::factorInPrimesCommand,
                            &WaitingTaskContext::isDoneFactoring, this),
                            "Factor a value into its primes.",
                            "number", "The number to factor in primes." );

        // Add a const TaskAttribute :
        this->attributes()->addConstant( &priority_ );

        // Add event:
        this->events()->addEvent( &primeEvent,
                                  "Event fired when a number has been factored in primes.",
                                  "Value", "The value that was factored.",
                                  "Primes", "The number of primes found.");
    }

    /**
     * A command to do a 'lengthy' calculation (or IO,...).
     * Since the task is nonperiodic, this command may take
     * as long as necessary, and the implementation of a command
     * for a non periodic task is thus different than if it were
     * a periodic task ( where the command may not block ).
     */
    bool factorInPrimesCommand( int value )
    {
        // check validity of arguments :
        if ( value < 1 ) {
            Logger::log() <<Logger::Error<< " Rejected negative number "<< value <<Logger::endl;
            return false;
        }

        busy = true;
        Logger::log() << Logger::Info<< " Factoring "<< value <<Logger::endl;
        
        // non efficient implementation to stretch time : 
        // search primes first
        primes.clear();
        primes.push_back( 2 );
        int nextvalue = 3;
        while ( nextvalue < double(value) ) {
            bool isprime = true;
            for( std::vector<int>::iterator it = primes.begin();
                 it != primes.end(); ++it ) {
                if ( nextvalue % *it == 0 ) {
                    isprime = false;
                    break;
                }
            }
            if ( isprime ) {
                Logger::log() <<Logger::Debug<< " Found Prime "<< nextvalue<< Logger::endl;
                primes.push_back( nextvalue );
            }
            ++nextvalue;
        }
        Logger::log() <<Logger::Info<< " Found "<<primes.size()<< " primes smaller than "<< value << Logger::endl;

        // now try to divide value by primes :
        int copy = value;
        std::vector<int> factors;
        for( std::vector<int>::iterator it = primes.begin();
             it != primes.end(); ) {
            if ( copy % *it == 0 ) {
                Logger::log() <<Logger::Info<< " Found Factor "<< *it << Logger::endl;
                copy = copy / *it;
                factors.push_back( *it );
            }
            else 
                ++it; // only increase if factor not usable.
        }
        if ( factors.size() == 0 ) {
            Logger::log() <<Logger::Info<< value << " is prime !" << Logger::endl;
            primeEvent( value, 1 );
        } else {
            Logger::log() <<Logger::Info<< " Done Factoring "<< value<<" in "<<factors.size()<<" factors." << Logger::endl;
            // Notify other tasks of result :
            primeEvent( value, factors.size() );
        }

        busy = false;
        return true;
    }

    bool isDoneFactoring() const
    {
        return !busy;
    }
};
