require "rttlib"
require "rfsm"
require "rfsm_rtt"
require "rfsmpp"
 
local tc=rtt.getTC();
local fsm
local fqn_out, events_in

-- These need to be defined in order to use them in supervisor_fsm.lua !
teleop=tc:getPeer("teleop")
controller=tc:getPeer("controller")
areadetection=tc:getPeer("areadetection")

 
function configureHook()
   -- load state machine
   fsm = rfsm.init(rfsm.load("components/supervisor/supervisor_fsm.lua"))
 
   if ( not fsm ) then
        return false
   end

   -- enable state entry and exit dbg output
   fsm.dbg=rfsmpp.gen_dbgcolor("sup", 
			       { STATE_ENTER=true, STATE_EXIT=true}, 
			       false)
 
   -- redirect rFSM output to rtt log
   fsm.info=function(...) rtt.logl('Info', table.concat({...}, ' ')) end
   fsm.warn=function(...) rtt.logl('Warning', table.concat({...}, ' ')) end
   fsm.err=function(...) rtt.logl('Error', table.concat({...}, ' ')) end
 
   -- the following creates a string input port, adds it as a event
   -- driven port to the Taskcontext. The third line generates a
   -- getevents function which returns all data on the current port as
   -- events. This function is called by the rFSM core to check for
   -- new events.
   events_in = rtt.InputPort("string")
   tc:addEventPort(events_in, "events", "rFSM event input port")
   fsm.getevents = rfsm_rtt.gen_read_str_events(events_in)
 
   -- optional: create a string port to which the currently active
   -- state of the FSM will be written. gen_write_fqn generates a
   -- function suitable to be added to the rFSM step hook to do this.
   fqn_out = rtt.OutputPort("string")
   tc:addPort(fqn_out, "rFSM_cur_fqn", "current active rFSM state")
   rfsm.post_step_hook_add(fsm, rfsm_rtt.gen_write_fqn(fqn_out))
   return true
end
 
function updateHook() rfsm.run(fsm) end
 
function cleanupHook()
   -- cleanup the created ports.
   rttlib.tc_cleanup()
end

