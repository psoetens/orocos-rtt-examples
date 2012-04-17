-- In order to run this script, use:
-- rttlua-gnulinux -i deployment/youbot.lua

-- See also http://www.orocos.org/wiki/orocos/toolchain/LuaCookbook
--  in order to set your LUA_PATH correctly, or errors will occur.

require("rttlib")
rttlib.color = true -- Since we're running this script with -i

-- Get our Lua TC
tc = rtt.getTC()

-- Get the deployer
depl = tc:getPeer("deployer")

-- Start deploying and connecting
-- We need rtt_tf to get the current 2D position for us
depl:import("rtt_tf")
depl:loadComponent("tf","rtt_tf::RTT_TF")
tf = depl:getPeer("tf")
tf:configure()
tf:start()

-- Import our own components
depl:import("controller-youbot")

depl:loadComponent("localisation","Localisation")
depl:addPeer("localisation", "tf") -- for lookupTransform
localisation= depl:getPeer("localisation")
localisation:setPeriod(0.01)
localisation:configure()
localisation:start()

depl:loadComponent("controller","Controller")
depl:loadComponent("areadetection","Areadetection")
depl:loadComponent("teleop","Teleop")
-- Deployment Exercise: Add a supervisor Lua component

-- Data Flow connections
cp=rtt.Variable("ConnPolicy")
cp.transport=3 -- 3 is ROS -- NOTE: set transport back to zero for local connections ! 

cp.name_id="/cmd_vel" -- topic name
depl:stream("controller.cmdvel", cp )
depl:stream("teleop.cmdvel", cp )

cp.name_id="/joy"
depl:stream("teleop.joystick",cp)

-- Data Flow Exercise: 
--  Connect the events of areadetection to the supervisor
--  Connect the localisation's ports to the controller and areadetection
--  Check if all components and ports match periodicity or have an event-port 

-- Only start in case no youbot is present:
depl:loadComponent("youbot","Nobot")
cp.transport = 0
depl:connect("youbot.cmdvel","controller.cmdvel", cp);
depl:connect("youbot.cmdvel","teleop.cmdvel", cp);
depl:connect("youbot.curpos","controller.curlocation", cp);


-- Deployment Exercise:
--  Configure or start the necessary components: 
--  areadetection, nobot (if applicable), supervisor 
--  Remember that the supervisor has the authority to
--  control (start/stop) most components.

-- Deployment Exercise:
--  Visualise the current setup with the rtt_dot_service
