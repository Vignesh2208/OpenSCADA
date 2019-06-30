Kronos driven co-simulation
============================

In this section, we discuss how Kronos can be used to run time synchronized OpenSCADA PLCs and physical system simulations. This discussion is based on the `inverted_pendulum <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/inverted_pendulum>`_ example included with the installation. We will refer the script `simulation.py <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/inverted_pendulum/simulation.py>`_ in the rest of this discussion.

Initializing the co-simulation
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

* The first step in running a time synchronized co-simulation involves creating a physical system simulator object. As discussed in Section, Simulating Physical Systems, this object must inherit from PhysicalSystemSim abstract class defined in `physical_system_sim.py <https://github.com/Vignesh2208/OpenSCADA/tree/master/contrib/physical_system_sim.py>`_. In the running example, a class called PendulumSimulator is implemented and an object of this class is instantiated and used for this purpose::

	pendulum_sim = PendulumSimulator()


* The next step involves initializing an EmulationDriver object (defined in `emulation_driver.py <https://github.com/Vignesh2208/OpenSCADA/tree/master/contrib/emulation_driver.py>`_) and register the previously created physical system simulator object. This step also assumes that Kronos module is installed and loaded::

	    emulation = EmulationDriver(number_dilated_nodes=num_dilated_nodes, \
					is_virtual=is_virtual, \
					n_insns_per_round=num_insns_per_round, 
					rel_cpu_speed=rel_cpu_speed, \
 					physical_system_sim_driver=pendulum_sim)

The emulation driver class takes in some Kronos specific arguments which are briefly described here. For a more thorough discussion, please refer `Kronos <https://github.com/Vignesh2208/Kronos>`_ documentation.

	* **is_virtual**: If True Kronos is initialized
        * **physical_system_driver**: An object which implements PhysicalSystemSim abstract class defined in contib/physical_system_sim.py. If it is None, then it denotes that this co-simulation has no attached physical simulator. 
        * **number_dilated_nodes**: Ignored unless is_virtual is True. Denotes number of nodes under Kronos control. Note that each PLC's CPU counts as a separate node. So if there are two PLCs each with 2 CPUs, then there are 4 dilated_nodes in total.
        * **rel_cpu_speed**: Ignored unless is_virtual is True. Denotes the relative cpu speed / (equivalent to TDF). In Kronos it represents the number of instructions that can be executed in 1ns of virtual time.
        * **n_insns_per_round**: Number of instructions to execute per round. When divided by the rel_cpu_speed, it denotes amount of time the co-simulation would advance in one-round. In the running example, n_insns_per_round is 1000000 and rel_cpu_speed is 1 which implies that in each round, the co-simulation would run for 1000000 ns or 1ms.

* The next step involves starting the GRPC server to initialize all memory mapped files and serve as interface to query PLCs which would be subsequently started::
	
	print "Starting PC GRPC Server ..."
	grpc_server_pid = start_grpc_server(args.plc_spec_dir, fd1) 

.. note:: The GRPC server does not count as a dilated node and should not be added to Kronos's control

Starting dilated processes
^^^^^^^^^^^^^^^^^^^^^^^^^^

In this subsection, we discuss the next stage of building a time synchronized co-simulation which involves launching PLCs, communication modules and HMI clients and adding them to Kronos's control. The rest of this discussion assumes that Kronos is installed and loaded and the previous stage is complete.

To launch any process under Kronos's control, it has to launched by a **tracer** binary which ships with Kronos installation. For example, let us consider a simple command with arguments **ls -al**. It can be added to Kronos's control as follows::
	
	tracer -c "ls -al" -r <rel_cpu_speed> -n <n_insns_per_round>

Using the tracer binary, OpenSCADA PLCs, communication modules and HMIs are added to Kronos' control. The relevant portions of this stage from the running example are included here::


	print "Starting PLC ..."
	plc_pid = start_plc(args.plc_spec_file, is_virtual, rel_cpu_speed, \
	num_insns_per_round, fd2)

	print "Starting Modbus Comm module ..."
	comm_module_pid = start_comm_module(args.plc_spec_file, \
	args.comm_module_bind_ip, args.comm_module_listen_port, \
	args.comm_module_attached_resource, is_virtual, rel_cpu_speed, \
	num_insns_per_round, fd3)

	print "Starting HMI ..."
	example_hmi_pid = start_example_hmi(is_virtual, rel_cpu_speed, \
	num_insns_per_round, fd4)

.. note:: The start_plc function does not invoke a tracer explicitly. Instead it passes rome arguments to plc_runner which implicitly tell it to mimic multiple tracers (one for each CPU) and register each one with Kronos.

Subsequently, the next step involves waiting for all started process to register themselves with Kronos::

	# Wait until all processes have started and registered themselves
    	emulation.wait_for_initialization()

After the completion of this stage, the experiment is frozen and ready to run in a time synchronized fashion.

Running the co-simulation
^^^^^^^^^^^^^^^^^^^^^^^^^

The co-simulation can be run using the emulation_driver object. It provides a method **run_for(time_secs)** which takes in as input the number of virtual time seconds to run. In running_example, since the co-simulation advances by 1ms each round (rel_cpu_speed is 1.0 and n_insns_per_round is 1000000), the argument provided to the **run_for** method is implicitly converted into the number of rounds to run. In each round, all the dilated processes are triggered and **progress** method of the physical system simulator object is invoked once::

	total_time_elapsed = 0.0
	while total_time_elapsed <= run_time:
		# Run for 10ms or 10 rounds.
		emulation.run_for(0.01)
		total_time_elapsed += 0.01
		if is_virtual:
		    print "Time Elapsed: ", total_time_elapsed
		if stop == True:
		    break


Stopping the co-simulation
^^^^^^^^^^^^^^^^^^^^^^^^^^

The co-simulation can be stopped by invoking the **stop_exp()** method provided by the emulation_driver::

	emulation.stop_exp()

	



	
	 
