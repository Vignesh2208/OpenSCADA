System Specification
====================

The system level specification of the PLC is specified as a prototxt file of the SystemConfiguration proto message format specified in `system_spectification.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/system_specification.proto/>`_ file. In this guide we will use an example of the proto definition given `here <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/inverted_pendulum/system_specification.proto/txt>`_.

**configuration_name**: A name given to identify this PLC. In this example it is Pendulum_PLC::
	
	configuration_name: "Pendulum_PLC"

**log_level**: Logging level. 5 types are supported: LOG_NONE, LOG_NOTICE, LOG_INFO, LOG_ERROR, LOG_VERBOSE. ::

	log_level: LOG_NOTICE

**log_file_path**: (optional) If specified the logs are stored here.

**run_time_secs**: (optional) Total time for which the PLC should run. If unspecified, will run forever untill interrupted.
 
**hardware_spec**: Description about the PLC's hardware according to HardwareSpecification message defined in `system_spectification.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/system_specification.proto/>`_. (Full example is not included here for brevity). In this hardware_spec, num_resources specifies the number of CPUs attached to the PLC. Here it is 1. The hardware_spec also includes the mean and standard deviation of execution times of all instructions, system functions and system function blocks. The mean and standard deviation execution times are all specified in nanoseconds::
	
	hardware_spec {
	    num_resources: 1
	    ram_mem_size_bytes: 10000

	    # Instruction mean, std execution times, used only in virtual time mode
	    ins_spec {
		ins_name:   "ADD"
		mu_exec_time_ns:    1000
		sigma_exec_time_ns: 100
	    }

	    ins_spec {
		ins_name:   "AND"
		mu_exec_time_ns:    1000
		sigma_exec_time_ns: 100
	    }

	    ...

	    # SFCs (System functions) - mean, std of execution times
	    sfc_spec {
		sfc_name: "ABS"
		mu_exec_time_ns: 2000
		sigma_exec_time_ns: 0
	    }

	    ...
	
	    # SFBs (System function blocks) - mean, std of execution times
	    sfb_spec {
		sfb_name: "TON"
		mu_exec_time_ns: 1500
		sigma_exec_time_ns: 0
	    }

            ...
	}

**datatype_declaration**: Multiple datatypes can be declared in the system specification and later used inside PLC programs or function blocks. A datatype can belong to one of the categories belonging to DataTypeCategory enum defined in `configuration.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/configuration.proto/>`_. Full explanation of the datatype_declaration message will be included in subsequent sections. In this example 4 datatypes are declared.

* An integer typedef datatype called INT_TYPE_DEF. Any variable of this datatype will be an integer with an initial value of 10::

	datatype_declaration {
	    name: "INT_TYPE_DEF"
	    datatype_category: INT
	    datatype_spec {
		initial_value: "10"
	    }
	}

* A 1-D integer array datatype called INT_1DARR_TYPE_DEF. Any variable of this data type will be a 10 dimensional array initialized to [-1,0,1,2,3,4,5,6,7,8]::

	datatype_declaration {
	    name: "INT_1DARR_TYPE_DEF"
	    datatype_category: INT
	    datatype_spec {
		initial_value: "{-1,0,1,2,3,4,5,6,7,8}"
		dimension_1:  10
	    }   
	}

* A 2-D integer array datatype called INT_2DARR_TYPE_DEF. Any variable of this data type will be a 2 x 2 matrix initialized to [[0,1],[2,3]]::

	datatype_declaration {
	    name: "INT_2DARR_TYPE_DEF"
	    datatype_category: INT
	    datatype_spec {
		initial_value: "{{0,1},{2,3}}"
		dimension_1:  2
		dimension_2: 2
	    }   
	}

* A complex structure datatype called COMPLEX_STRUCT. It has multiple fields and each field is specified with the datatype name of the field. This could inturn be an elementary/inbuilt datatype or a complex datatype. The list of inbuilt elementary datatypes are described in the IL programming section. In this example the structure has 5 fields with three of them using the 3 previously declared datatypes we just looked at::

	datatype_declaration {
	    name: "COMPLEX_STRUCT"
	    datatype_category: DERIVED
	    datatype_field {
		field_name: "string_field"
		# String is an elementary datatype. It is a char array of size 1000
		field_datatype_name: "STRING"
	    }
	    datatype_field {
		field_name: "int_field"
		field_datatype_name: "INT_TYPE_DEF"
	    }
	    datatype_field {
		field_name: "real_field"
		# Real is an elementary datatype. It is equivalent to float.
		field_datatype_name: "REAL"
		initial_value: "0.1"
	    }
	    datatype_field {
		field_name: "oned_arr_field"
		field_datatype_name: "INT_1DARR_TYPE_DEF"
	    }
	    datatype_field {
		field_name: "twod_arr_field"
		field_datatype_name: "INT_2DARR_TYPE_DEF"
	    }
	}

**var_global**: Variables which can be used by all programs running on this PLC can be declared in the system specification. All PLC level global variables are declared in the var_global section. Variable declaration is similar to datatype declaration with an optional interface_type and storage_specification. Interface types assign meaning to the way the variable is interpreted by the PLC program. For instance a variable of interface_type VAR_INPUT is equivalent to an INPUT variable defined in IEC 61131-3 specification. A variable declared inside the var_global section can only optionally have VAR_EXPLICIT_STORAGE interface type.

Storage specifications denote where the variable is stored, i.e whether the address of the variable is backed by RAM/IO memory. They can only be present if the interface type is VAR_EXPLICIT_STORAGE, otherwise the variable is allocated statically and does not point to any byte in the PLC's RAM or IO memory. Further explation of variable interface_types and storage_specifications are described in the IL programming section. 

In this example, 4 PLC level global variables are declared:

* "global_bool_var" is a boolean whose value is stored in RAM byte number 3 and bit number 1 within the byte. 
* "global_int_var" is an integer variable which is stored in RAM starting at byte number 4. 
* "start_int" is of type INT_TYPE_DEF declared before but it is statically and its memory location is not addressible. 
* "complex_global" is a global variable of type COMPLEX_STRUCT and is stored in RAM memory starting at byte 30::

	var_global {
	    name: "__CONFIG_GLOBAL__"
	    datatype_field {
		field_name: "global_bool_var"
		field_datatype_name: "BOOL"
		intf_type : VAR_EXPLICIT_STORAGE
		field_storage_spec {
		    mem_type: RAM_MEM
		    byte_offset: 3
		    bit_offset: 1
		}
	    }
	    datatype_field {
		field_name: "global_int_var"
		field_datatype_name: "INT"
		intf_type : VAR_EXPLICIT_STORAGE
		field_storage_spec {
		    full_storage_spec: "%MW4"
		}
	    }
	    datatype_field {
		field_name: "start_int"
		field_datatype_name: "INT_TYPE_DEF"
	    }
	    datatype_field {
		field_name: "complex_global"
		field_datatype_name: "COMPLEX_STRUCT"
		intf_type: VAR_EXPLICIT_STORAGE
		field_storage_spec {
		    full_storage_spec: "%MW30"
		}
	    }
	}

**resource_file_path**: Indicates where to find specification this PLC first and only CPU. Since the PLC can have multiple CPUs, this field can be repeated::

	resource_file_path: "~/OpenSCADA/examples/inverted_pendulum/CPU_001.prototxt"

