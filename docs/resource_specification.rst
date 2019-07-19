Resource Specification
======================

The specification of a PLC's CPU is provided as a prototxt file of the ResourceSpecification proto message format specified in `configuration.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/configuration.proto/>`_ file. In this guide we will use an example of the proto definition given `here <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/inverted_pendulum/CPU_001.proto/txt>`_.

**resource_name**: Name of the CPU::

	resource_name: "CPU_001"

**input_mem_size_bytes**: Input memory size of the CPU. Sensors may be connected to this::

	input_mem_size_bytes: 10000
	
**output_mem_size_bytes**: Output memory size of the CPU. Actuators may be connected to this::

	output_mem_size_bytes: 10000

**resource_global_var**: Global variables which are visible to only Program Organization Units (POU) defined in this resource: Function blocks (FB), Functions (FN) and Programs (PROGRAM). In this example two CPU level global variables are defined. "current_theta" is stored in byte number 1 of the input memory whereas "force" is stored at byte number 1 of the output memory::

	resource_global_var {
	    name: "__RESOURCE_GLOBAL__"
	    datatype_field {
		field_name: "current_theta"
		field_datatype_name: "REAL"
		intf_type : VAR_EXPLICIT_STORAGE
		field_storage_spec {
		    mem_type: INPUT_MEM
		    byte_offset: 1
		    bit_offset: 0
		}
	    }
	    datatype_field {
		field_name: "force"
		field_datatype_name: "REAL"
		intf_type : VAR_EXPLICIT_STORAGE
		field_storage_spec {
		    mem_type: OUTPUT_MEM
		    byte_offset: 1
		    bit_offset: 0
		}
	    }
	}

**pou_var**: A CPU can define multiple pou_var messages. These describe a POU. A POU could be of type FB, FN or PROGRAM and they contain and interface declaration and a code body to hold all the IL instructions associated with POU. POUs are equivalent to functions in the traditional programming sense but FBs and PROGRAMs can hold and update some internal state which can be used each time they are invoked. For further details on the expected behaviour of FBs, FNs and PROGRAMs, please refer to the included book. More details on how to define POUs and their interfaces and code bodies would be included in the IL programming section.

In this example, 3 POUs are defined. Two of them are Function Blocks, while one of them is a PROGRAM. Each POU has its interface specified as a set of datatype_field and code specified in a code_body section::

	pou_var {
	    name: "DivideFB"
	    pou_type: FB
	    datatype_field {
		...
	    }
	    datatype_field {
		...
	    }
	    ...

	    code_body {
		insn: "LD 0.0" 
		...
	    }
	}

	pou_var {
	    name: "GetControlInputFB"
	    pou_type: FB
	    datatype_field {
		...
	    }
	    ...

	    code_body {
		...
	    }
	}

	pou_var {
	    name: "PID_CONTROL"
	    pou_type: PROGRAM
	    datatype_field {
		...
	    }
	    ...
	    code_body {
		...
	    }
	}

.. note:: POUs defined in each resource must have unique names. This is not an IEC 61131-3 requirement but rather a current limitation in OpenSCADA design for simplicity.

.. note:: POUs defined in one resource specification cannot be referenced or used in another resource specification.

**interval_task**: This field is used to specify a task which gets invoked periodically according to the specified period. Programs and Function Blocks could be attached to this task and periodically invoked. In OpenSCADA, for each CPU, only one interval task can be specified. In this example, the interval task is called "CYCLIC_TASK" and it executes one every 10ms.::

	interval_task {
	    task_name: "CYCLIC_TASK"
	    priority: 1
	    interval_task_params {
		interval_ms: 10
	    }
	}

**programs**: Used to attach POUs to tasks. POUs can be attached to tasks with the programs field. It is a message of type ProgramSpecification described in `configuration.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/configuration.proto/>`_. Each programs field defines a mapping between a POU of interest and a task of interest. Multiple POUs can be attached to the same task. A separate programs field is used for each attachement. In this example the POU "PID_CONTROL" is attached with the task "CYCLIC_TASK". Thus the program "PID_CONTROL" gets invoked every 10ms.::

	programs {
	    program_name: "PID_CONTROL"
	    pou_variable_type: "PID_CONTROL"
	    task_name: "CYCLIC_TASK"
	    initialization_maps {
		pou_variable_field_name: "dummy_in"
		mapped_variable_field_name: "start_int"
	    }
	    initialization_maps {
		pou_variable_field_name: "dummy_out"
		mapped_variable_field_name: "global_int_var"
	    }
	}


Arguments to each invocation of the POU can be passed through initialization_maps (a subfield of the ProgramSpecification message). Initialization maps can be used to initialize the POUs input/inout variables as well as specify where output variables could be stored after the invocation. In this example, the program "PID_CONTROL"'s input variable "dummy_in" at the start of every invocation is assigned the value of the global variable "start_int" (which was specified in the System specification i.e, in the previous section). At the end of the invocation, the value of the output variable of the PROGRAM called "dummy_out" is copied to "global_int_var" (which was created in the previous section as a PLC level global variable). It must be noted that the mapped_variable_field_name for an VAR_INPUT variable could also be an immediate value. For instance in the above example, if "dummy_in" needs to be initialized with 10 for every invocation, the initialization map could be modified to::

	    initialization_maps {
		pou_variable_field_name: "dummy_in"
		mapped_variable_field_name: "10"
	    }


