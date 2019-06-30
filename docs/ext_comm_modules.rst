Extending Communication Modules
===============================

Custom communication modules can be built with the CommModule class defined in `comm_module.h <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/ext_modules/include/comm_module.h>`_. 

* An object of the CommModule class can get pointers to Input/Output memory of any managed CPU or the PLC's RAM memory using its methods::

	uint8_t * GetPtrToInputMemory(string ResourceName)
	uint8_t * GetPtrToOutputMemory(string ResourceName)
	uint8_t * GetPtrToRAMMemory() 

* Additional methods also return the size of each memory component::

	int GetRAMMemSize();
	int GetInputMemSize(string ResourceName)
	int GetOutputMemSize(string ResourceName);

* The CommModule class also allows controlled access to ACCESS variables which are defined in the system specification::

	std::unique_ptr<PCVariableContainer>
                GetVariableContainer(string NestedAccessPath)

An access variable name can be specified as an input and it would return an object of type `PCVariableContainer <http://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/ext_modules/include/ext_module_intf.h>`_ which allows controlled access to the access variable based on its specified qualifier (READ/READ_WRITE).

An example is illustrated in `examples/idle_plc/comm_module.cc <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/idle_plc/comm_module.cc>`_. A new communication module object is defined over the system_specification present in the example. The file describes in detail how the communication_module object can read/write to access variables and memory locations. Interested reader is encouraged to refer to the file for further clarifications.

The CommModule class thus provides a useful interface which can be used to build custom communication modules which can use a variety of SCADA protocols. The CommModule interface is used by the sample modus implementation (defined `here <https://github.com/Vignesh2208/OpenSCADA/tree/master/contrib/modbus_comm_module.cc>`_) which is shipped with OpenSCADA.
