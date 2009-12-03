
#include <rtt/TaskContext.hpp>
#include <rtt/Command.hpp>
#include <rtt/Method.hpp>
#include <rtt/PeriodicActivity.hpp>
#include <rtt/Property.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/StateMachine.hpp>
#include <rtt/Logger.hpp>
#include <rtt/MultiVector.hpp>

#include <iostream>

using namespace RTT;
using namespace std;

/**
 * A Task which executes programs periodically.
 */
class PeriodicTaskContext
    : public TaskContext
{
    /**
     * Use the ZeroTimeThread.
     */
    PeriodicActivity mytask;

    Constant<double>       speed_of_light;

    /**
     * Attribute for counter.
     */
    Attribute<int>       counter;
    Attribute<int>       target;

    Property<std::string> parameter; 
    Property<PropertyBag> itemcollection;
    Property<double>      item1;
    Property<double>      item2;
    Property< std::vector<double> >      vect;

    StateMachinePtr countmachine;
public:
    /**
     * Create a TaskContext with periodic executing Processor,
     * at the periodic frequency of 100Hz.
     */
    PeriodicTaskContext(const std::string& name )
        : TaskContext(name),
          mytask( RTT::OS::HighestPriority, 0.01, this->engine() ),
          speed_of_light("SpeedOfLight",300000000.0),
          counter("Counter",0),
          target("Target",0),
          parameter("Parameter", "A configuration value", "TypeA"),
          itemcollection("ItemCollection", "A Collection of items"),
          item1("Item_1", "An item"),
          item2("Item_2", "Other item", 0.4 ),
          vect("v","std::vector with 10 items.", std::vector<double>(10, 1.0))
    {
        this->properties()->addProperty( &vect );
        // Add a Property to the AttributeRepository :
        this->properties()->addProperty( &parameter );
        // Add a PropertyBag to the AttributeRepository :
        this->properties()->addProperty( &itemcollection );
        itemcollection.value().add( &item1 );
        itemcollection.value().add( &item2 );

        // Add some example attributes, accessible from scripting :
        this->attributes()->addConstant( &speed_of_light );
        this->attributes()->addAttribute( &counter );
        this->attributes()->addAttribute( &target );

        // Demonstration how to retrieve a property from the repository :
        Property<std::string>* par2 = this->properties()->getProperty<std::string>("Parameter");
        assert ( parameter.get() == par2->get() );

        this->commands()->addCommand( command( "countTo", &PeriodicTaskContext::countTo,
                            &PeriodicTaskContext::hasReached, this),
                            "Count to a given number using a StateMachine program.",
                            "Target", "Number to count to." );

    }

    virtual bool startHook()
    {
        countmachine = this->engine()->states()->getStateMachine("counterMachine");
        if ( !countmachine ) {
            Logger::log()<<Logger::Error<< "PeriodicTaskContext did not find 'counterMachine' !" << Logger::endl;
            return false;
        }

        countmachine->activate();
        // this start is only allowed because activation is guaranteed
        // atomic ( empty entry in initial state ).
        return countmachine->start();
    }

    virtual void stopHook()
    {
        if (countmachine )
            countmachine->stop();
    }

    virtual bool configureHook() 
    {
        Logger::log()<<Logger::Info<< "PeriodicTaskContext configuring..." << Logger::endl;
        // optionally check for consistency after a readProperties (not done).
        return this->marshalling()->readProperties( this->getName() + ".cpf" );
    }

    virtual void cleanupHook()
    {
        Logger::log()<<Logger::Info<< "PeriodicTaskContext writing XML..." << Logger::endl;
        this->marshalling()->writeProperties( this->getName() + ".cpf" );
    }

    /**
     * A command to count to a value.
     */
    bool countTo( int value )
    {
        if ( !countmachine || countmachine->currentState() == 0 )
            return false;
        if ( value < 1 || countmachine->currentState()->getName() != "Waiting" )
            return false; // busy or reject.

        // safely init the data, this command is serialised
        // with the execution of the State Machine.
        target.set( value );

        Logger::log()<<Logger::Info<< "PeriodicTaskContext counts to "<<value<<"." << Logger::endl;

        // command accepted.
        return true;
    }

    /**
     * The completion condition of countTo.
     */
    bool hasReached( int value ) const
    {
        return counter.get() == target.get();
    }
};
