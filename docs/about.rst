About
============

OpenSCADA is platform which supports high fidelity emulation of IEC 61131-3 compliant PLC emulators and SCADA protocols. It can be used to build and simulate complex environments involving cyber-physical systems. OpenSCADA PLCs can be connected to an network emulator like CORE/Mininet and interact with other emulated processes. OpenSCADA also provides python interfaces to link the cyber-network emulation with a physical system simulator. Using these interfaces OpenSCADA PLCs can interact with physical system simulators like Matpower, Simulink, ManPy etc. OpenSCADA also ships with support for virtual time based control which is handled by `Kronos <http://github.com/Vignesh2208/Kronos/>`_. This allows tight time-synchronization between the physical system simulation and OpenSCADA emulation. 

Motivation
------------------

As manufacturing systems and smart grid networks become increasingly networked with intelligent electronic devices (IEDs) and PLCs, it becomes necessary to understand how these devices interact with the physical system. Several scenarios need to be simulated to identify the sensitivity of the control system and study how changes to cyber components affect the physical side. OpenSADA enables protyping high fidelity SCADA emulation scenarios in a safe virtualized environment.

OpenSCADA implements a generic Instruction List processing backend which can be used to emulate different classes of PLCs. It allows users to specify a hardware spec of the PLC they want to emulate which includes the number of CPUs in the PLC, its RAM, Input and Output memory size and execution time of each supported instruction. Users can then leverage OpenSCADA to write Instruction List PLC programs which are interpreted and executed by a backend engine implemented in C++ according to the specified hardware spec. With this feature, users can emulate acurately, the functionality and timing behaviour of vendor specific PLCs like Siemens, ModiCon, SEL etc. OpenSCADA also proves a generic interface to interact with these soft-PLCs. Emulations of sensors and actuators can talk to the PLC using the generic interface. Specific SCADA protocols can also be implemented to used the generic interface and exchange data with the PLC. 

Users can leverage this high fidelity emulation platform for various tasks like (1) building performance models of PLC programs, e.g to study how changes to the program would impact its timing and subsequently the underlying physical system, (2) understand how changes to the cyber network affects the control system, e.g how does adding a layer of encryption impact the system, (3) load-test the cyber-physical system under various scenarios to improve coverage, (4) perform what-if cost analysis, e.g what would be the impact of replacing one PLC with another for a particular task, or, what is the best/cheapest PLC to buy to execute a specifc program etc. The possibilities are huge and we welcome further contributions to improve the project!




