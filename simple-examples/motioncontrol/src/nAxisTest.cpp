
//hardware interfaces
#include <ocl/Kuka361nAxesVelocityController.hpp>
#include <ocl/EmergencyStop.hpp>

//User interface
#include <ocl/TaskBrowser.hpp>

//Reporting
#include <ocl/FileReporting.hpp>

//nAxes components
#include <ocl/nAxesComponents.hpp>
#include <ocl/ReferenceSensor.hpp>
#include <ocl/naxespositionviewer.hpp>

//CartesianComponents
#include <ocl/CartesianComponents.hpp>

//Kinematics component
#include <kdl/kinfam/kuka361.hpp>
#include <kdl/toolkit.hpp>

//#include <rtt/Activities.hpp>
#include <rtt/TaskContext.hpp>
#include <rtt/Attribute.hpp>
#include <rtt/Activities.hpp>

#include <rtt/os/main.h>

using namespace Orocos;
using namespace RTT;
using namespace std;
using namespace KDL;

// main() function

int ORO_main(int argc, char* argv[])
{
    Toolkit::Import( KDLToolkit );

    KinematicFamily* kukakf = new Kuka361();
    TaskContext* my_robot = new Kuka361nAxesVelocityController("Robot");

    EmergencyStop _emergency(my_robot);
    
    /// Creating Event Handlers
    _emergency.addEvent(my_robot,"driveOutOfRange");
    _emergency.addEvent(my_robot,"positionOutOfRange");

    //nAxesComponents
    nAxesSensor sensor("nAxesSensor",6);
    nAxesGeneratorPos generatorPos("nAxesGeneratorPos",6);
    nAxesGeneratorVel generatorVel("nAxesGeneratorVel",6);
    nAxesControllerPos controllerPos("nAxesControllerPos",6);
    nAxesControllerPosVel controllerPosVel("nAxesControllerPosVel",6);
    nAxesControllerVel controllerVel("nAxesControllerVel",6);
    nAxesEffectorVel effector("nAxesEffectorVel",6);
    ReferenceSensor refsensor("nAxesRefSensor",6);
    NAxesPositionViewer viewer("Viewer");

    //CartesianComponents
    CartesianSensor cartsensor("CartesianSensor",kukakf);
    CartesianGeneratorPos cartgenerator("CartesianGenerator");
    CartesianControllerPosVel cartcontroller("CartesianController");
    CartesianEffectorVel carteffector("CartesianEffector",kukakf);

    
    //connecting sensor and effector to hardware
    connectPorts(my_robot,&effector);
    connectPorts(my_robot,&sensor);
    connectPorts(my_robot,&refsensor);
    connectPorts(my_robot,&cartsensor);
    connectPorts(my_robot,&carteffector);
    connectPorts(my_robot,&viewer);
    
    //connection naxes components to each other
    connectPorts(&sensor,&generatorPos);
    connectPorts(&sensor,&generatorVel);
    connectPorts(&sensor,&controllerPos);
    connectPorts(&sensor,&controllerPosVel);
    connectPorts(&sensor,&controllerVel);
    connectPorts(&generatorPos,&controllerPos);
    connectPorts(&generatorPos,&controllerPosVel);
    connectPorts(&generatorVel,&controllerVel);
    connectPorts(&controllerPos,&effector);
    connectPorts(&controllerPosVel,&effector);
    connectPorts(&controllerVel,&effector);
    
    //connecting cartesian components to eachother
    connectPorts(&cartsensor,&cartgenerator);
    connectPorts(&cartsensor,&cartcontroller);
    connectPorts(&cartsensor,&carteffector);
    connectPorts(&cartgenerator,&cartcontroller);
    connectPorts(&cartcontroller,&carteffector);


    //Reporting
    FileReporting reporter("Reporting");
    reporter.addPeer(&sensor);
    reporter.addPeer(&generatorPos);
    reporter.addPeer(&generatorVel);
    reporter.addPeer(&controllerPos);
    reporter.addPeer(&controllerPosVel);
    reporter.addPeer(&controllerVel);
    reporter.addPeer(&effector);  
    reporter.addPeer(&cartsensor);
    reporter.addPeer(&cartgenerator);
    reporter.addPeer(&cartcontroller);
    reporter.addPeer(&carteffector);  

    //Create supervising TaskContext
    TaskContext super("RobotDemo");
  
    // Link components to supervisor
    super.addPeer(my_robot);
    super.addPeer(&reporter);
    super.addPeer(&sensor);    
    super.addPeer(&refsensor);
    super.addPeer(&generatorPos); 
    super.addPeer(&generatorVel); 
    super.addPeer(&controllerPos);
    super.addPeer(&controllerPosVel);
    super.addPeer(&controllerVel);
    super.addPeer(&effector);
    super.addPeer(&cartsensor);    
    super.addPeer(&cartgenerator); 
    super.addPeer(&cartcontroller);
    super.addPeer(&carteffector);

    super.addPeer(&viewer);
    
    // Load programs in supervisor
    super.scripting()->loadPrograms("programs/program_calibrate_offsets.ops");
    super.scripting()->loadPrograms("programs/program_moveto.ops");
    
    // Load StateMachine in supervisor
    vector<double> temp = vector<double>(6,0);
    Attribute<vector<double> > joints = Attribute<vector<double> >("joints",temp);
    Attribute<Frame> f = Attribute<Frame>("f",Frame::Identity());
    super.attributes()->addAttribute(&joints);
    super.attributes()->addAttribute(&f);

    super.scripting()->loadStateMachines("states/states.osd");

    // Creating Tasks
#if defined(OROPKG_OS_LXRT)
#warning building for lxrt
    PeriodicActivity _kukaTask(0,0.002, my_robot->engine() ); 
    PeriodicActivity superTask(0,0.002,super.engine());
#else
    PeriodicActivity _kukaTask(0,0.01, my_robot->engine() ); 
    PeriodicActivity superTask(0,0.01,super.engine());
#endif
    PeriodicActivity _sensorTask(0,0.01, sensor.engine() ); 
    PeriodicActivity _refsensorTask(0,0.01, refsensor.engine() ); 
    PeriodicActivity _generatorPosTask(0,0.01, generatorPos.engine() ); 
    PeriodicActivity _generatorVelTask(0,0.01, generatorVel.engine() ); 
    PeriodicActivity _controllerPosTask(0,0.01, controllerPos.engine() ); 
    PeriodicActivity _controllerPosVelTask(0,0.01, controllerPosVel.engine() ); 
    PeriodicActivity _controllerVelTask(0,0.01, controllerVel.engine() ); 
    PeriodicActivity _effectorTask(0,0.01, effector.engine() ); 
    PeriodicActivity _cartsensorTask(0,0.01, cartsensor.engine() ); 
    PeriodicActivity _cartgeneratorTask(0,0.01, cartgenerator.engine() ); 
    PeriodicActivity _cartcontrollerTask(0,0.01, cartcontroller.engine() ); 
    PeriodicActivity _carteffectorTask(0,0.01, carteffector.engine() ); 

    PeriodicActivity reportingTask(2,0.1,reporter.engine());
    PeriodicActivity _viewerTask(1,0.01,viewer.engine());
  
    TaskBrowser browser(&super);
    browser.setColorTheme( TaskBrowser::whitebg );
    
    superTask.start();
    _kukaTask.start();
  
    //Load Reporterconfiguration and start Reporter
    //reporter.load();
    //reportingTask.start();
    
    // Start the console reader.
    browser.loop();

    superTask.stop();
    _kukaTask.stop();
    
    delete kukakf;
    delete my_robot;
    
    return 0;
}


