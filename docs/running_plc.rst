Starting the PLC
================

The PLC can be started after the System Specification file and referenced Resource specification files are provided. A PLC can be started in normal mode or in virtual time mode under Kronos's control.  The PLC can be started as follows::

	plc_runner [Options [-ens]] -f <path_to_system_spec prototxt file> 
 
where the following options are optional:
 * -e: 1 or 0 to enable/disable Kronos. Default is 0.
 * -n: num_insns_per_round - only valid if Kronos is enabled. Please refer Kronos documentation `here <https://github.com/Vignesh2208/Kronos>`_.
 * -s: relative cpu speed - only valid if Kronos is enabled. Please refer Kronos documentation `here <https://github.com/Vignesh2208/Kronos>`_.

The Kronos specific options are used to compute the virtual timestep size for each round. Before starting a PLC in virtual time mode, Kronos needs to be installed and loaded. In a subsequent section titled "Building Co-Simulations", we discuss in detail, the steps involved in starting a PLC in virtual time mode and subsequently advancing the emulation in virtual time. 

Starting a PLC in normal mode would be useful for debugging purposes. The PLC can be stopped at any time in normal mode by pressing Ctrl-C. Otherwise it would run for the specified run_time_secs duration configured in the System specification.
