require("rttlib")
rttlib.color = true -- Since we're running this script with -i

-- Get our Lua TC
tc = rtt.getTC()

-- Get the deployer
depl = tc:getPeer("deployer")
depl:import("rtt_dot_service")
depl:loadService("deployer","dot")
tc:setPeriod(0.1)

function updateHook()
   depl:provides("dot"):generate()
end

-- BUG DOES NOT WORK
--tc:configure()
--tc:start()
