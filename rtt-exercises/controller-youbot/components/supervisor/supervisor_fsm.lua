return rfsm.state {
   entry = function()
	      areadetection:start()
	   end,
   exit  = function()
	      areadetection:stop()
	   end,
   manual = rfsm.state {
      entry = function () teleop:start() end,
      exit  = function () teleop:stop() end,
      good = rfsm.state {
	 entry = function() 
		    ls = teleop:getProperty("l_scale")
		    ls:set(1.0)
		    as = teleop:getProperty("a_scale")
		    as:set(1.0)
		 end,
      },
      slow = rfsm.state {
	 entry = function() 
		    ls = teleop:getProperty("l_scale")
		    ls:set(0.5)
		    as = teleop:getProperty("a_scale")
		    as:set(0.5)
		 end,
      },
      out = rfsm.state {
	 entry = function() 
		    ls = teleop:getProperty("l_scale")
		    ls:set(0.2)
		    as = teleop:getProperty("a_scale")
		    as:set(0.2)
		 end,
      },
      rfsm.trans { src='initial', tgt='good' },
      rfsm.trans { src='good', tgt='slow', events={'e_slow'} },
      rfsm.trans { src='out', tgt='slow', events={'e_slow'} },
      rfsm.trans { src='good', tgt='out', events={'e_out'} },
      rfsm.trans { src='slow', tgt='out', events={'e_out'} },
      rfsm.trans { src='out', tgt='good', events={'e_safe'} },
      rfsm.trans { src='slow', tgt='good', events={'e_safe'} },
   },
   circling = rfsm.state {
      entry = function () controller:start() end,
      exit  = function () controller:stop() end,
   },

   rfsm.trans { src='initial', tgt='manual' },
   rfsm.trans { src='circling', tgt='manual', events={'e_manual'} },
   rfsm.trans { src='.manual.good', tgt='circling', events={'e_circle'} },
   rfsm.trans { src='circling', tgt='.manual.slow', events={'e_slow'} },
   rfsm.trans { src='circling', tgt='.manual.out', events={'e_out'} },
}
