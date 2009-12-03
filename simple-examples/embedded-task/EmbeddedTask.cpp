#include <rtt/os/main.h>
#include <rtt/RTT.hpp>
#include <rtt/Activities.hpp>

#ifdef TCORE
#include <rtt/TaskCore.hpp>
#else
#include <rtt/TaskContext.hpp>
#endif

#include <rtt/Method.hpp>
#include <rtt/Command.hpp>
#include <rtt/DispatchInterface.hpp>
#include <rtt/Ports.hpp>

using namespace RTT;
using namespace std;



class MyTask2 
  :
#ifdef TCORE
  public TaskCore
#else
  public TaskContext
#endif
{
public:
    Command<bool(void)> cmdStartCycle;
protected:
    /**
     * The first command starts a cycle.
     */
    bool startCycle() { printf("Start cycle\r"); return true; }
    bool cycleDone() const { printf("Check cycle done\r"); return
                                                               true; }
        
    Property<long> test;

public:
    MyTask2(std::string name)
#ifdef TCORE
        : TaskCore(name),
#else
	  : TaskContext(name),
#endif
	    cmdStartCycle("startCycle", &MyTask2::startCycle,
			  &MyTask2::cycleDone, this), 
	    test("Test", "TestProp", 1)
    {
#ifndef TCORE
      this->commands()->addCommand(&cmdStartCycle);
#endif
    }
        
    ~MyTask2() {}
};

class MyTask
#ifdef TCORE
    : public TaskCore
#else
      : public TaskContext
#endif
{
public:
    ReadDataPort<double>    posport;
    WriteDataPort<int>  turn_outport;
    ReadDataPort<int>  turn_inport;

    Event<void(int)>    turnEvent;
    Event<void(double)> posEvent;
    
    Command<bool(int)>  startSequence;

    Method<bool()>      smallMethod;

    Property<double>    multiplier;

    MyTask(string name)
#ifdef TCORE
        : TaskCore(name),
#else
	  : TaskContext(name),
#endif
          posport("PosPort"),
          turn_inport("TurnInPort"),
          turn_outport("TurnOutPort"),
          turnEvent("TurnEvent"),
          posEvent("PosEvent"),
          startSequence("StartSequence",
                        &MyTask::startSequenceCF,
                        &MyTask::isSequenceDone,
                        this),
          smallMethod("SmallMethod",
                      &MyTask::smallMethodF,
                      this),
          multiplier("Multiplier","ABC", 1.0)
    {
#ifndef TCORE
      this->commands()->addCommand(&startSequence);
      this->ports()->addPort(&posport);
      this->ports()->addPort(&turn_inport);
#endif
    }

    void update() {
        posport.Get();

        turnEvent(8);

        int i = 3;
        bool ok = startSequence( i );

        bool result = smallMethod();

        multiplier.get();

        // SYN + ASYN
        posEvent.connect( boost::bind(&MyTask::reactS, this, _1) );
        posEvent.connect( boost::bind(&MyTask::reactAS, this, _1), this->engine()->events(), EventProcessor::OnlyLast );
    }
    
    void reactS( double d ) {
    }

    void reactAS( double d ) {
    }

    bool smallMethodF() const { return true; }

    bool startSequenceCF(int nbr) { if (nbr < 1) return false; return true; }

    bool isSequenceDone( int nbr) const { if (nbr < 1) return false; return true; }

};

int ORO_main(int argc, char** argv)
{
    MyTask et("PhoneHome");
    MyTask et2("PhoneHome2");

    et.setActivity( new NonPeriodicActivity( 10 ) );
    et2.setActivity( new PeriodicActivity( 11, 0.001 ) );

    et.turn_outport.connectTo( &et2.turn_inport );

    return 0;
}
