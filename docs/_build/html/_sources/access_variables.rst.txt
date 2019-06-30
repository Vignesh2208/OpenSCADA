Access Variables
================

Access variables are defined in IEC 61131-3 specification as a means to allow access to PLC program variables. Variables which are declared under this scope can be accessed within the program as well as by external modules (e.g communication modules). In OpenSCADA, Access variables can be defined in the system specification. The may be referenced inside any POU by declaring the same variable as VAR_EXTERNAL in the PoU definition. They are not statically allocated but they may point to memory locations or to other global variables or to input and output variables of PROGRAMS defined in any CPU.

For illustration purposes we use the `idle_plc <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/idle_plc>`_ example. The **system_specification.prototxt** file included with the example declares many access variables in the **var_access** block::

	var_access {
		name: "__CONFIG_ACCESS__"
		datatype_field {
			field_name: "GLOBAL_BOOL_VAR"
			field_datatype_name: "BOOL"
			field_storage_spec {
			    full_storage_spec: "global_bool_var"
			}
			field_qualifier: READ_ONLY
		}
		datatype_field {
			field_name: "GLOBAL_INT_VAR"
			field_datatype_name: "INT"
			field_storage_spec {
			    full_storage_spec: "global_int_var"
			}
			field_qualifier: READ_WRITE
		}
		datatype_field {
			field_name: "GLOBAL_BOOL_ARR"
			field_datatype_name: "BOOL"
			field_storage_spec {
			    full_storage_spec: "global_bool_arr"
			}
		}
		datatype_field {
			field_name: "GLOBAL_INT_ARR"
			field_datatype_name: "INT"
			field_storage_spec {
			    full_storage_spec: "global_int_arr"
			}
		}
		datatype_field {
			field_name: "ACCESS_INT_ARR"
			field_datatype_name: "INT"
			dimension_1: 2
			dimension_2: 2
			field_storage_spec {
			    full_storage_spec: "%MW100"
			}	
		}
		datatype_field {
			field_name: "GLOBAL_COMPLEX_VAR"
			field_datatype_name: "COMPLEX_STRUCT_3"
			field_storage_spec {
			    full_storage_spec: "complex_global_2"
			}
			field_qualifier: READ_WRITE
		}  
		datatype_field {
			field_name: "SENSOR_IN_1"
			field_datatype_name: "INT_TYPE_DEF"
			field_storage_spec {
			    full_storage_spec: "CPU_001.PROGRAM_1.sensor_input_1"
			}
			field_qualifier: READ_WRITE
		 }  

		 datatype_field {
			field_name: "SENSOR_IN_2"
			field_datatype_name: "BOOL_TYPE_DEF"
			field_storage_spec {
			    full_storage_spec: "CPU_001.PROGRAM_1.sensor_input_2"
			}
			field_qualifier: READ_WRITE
		}  


		datatype_field {
			field_name: "MOTOR_OUT_1"
			field_datatype_name: "INT_TYPE_DEF"
			field_storage_spec {
			    full_storage_spec: "CPU_001.PROGRAM_1.motor_output_1"
			}
			# must be read when pointing to an output variable
			field_qualifier: READ
		}  
		datatype_field {
			field_name: "MOTOR_OUT_2"
			field_datatype_name: "BOOL_TYPE_DEF"
			field_storage_spec {
			    full_storage_spec: "CPU_001.PROGRAM_1.motor_output_2"
			}
			# must be read when pointing to an output variable
			field_qualifier: READ
		}  
		datatype_field {
			field_name: "SENSOR_IN_3"
			field_datatype_name: "COMPLEX_STRUCT_3"
			field_storage_spec {
			    full_storage_spec: "CPU_001.PROGRAM_1.sensor_input_3"
			}
			field_qualifier: READ_WRITE
		}  
		datatype_field {
			field_name: "MOTOR_OUT_3"
			field_datatype_name: "COMPLEX_STRUCT_3"
			field_storage_spec {
			    full_storage_spec: "CPU_001.%QW2000"
			}
			field_qualifier: READ_WRITE
		}  
	}

* In this example 12 Access variables are declared. All of these access variables point to other global variables or memory locations or input/output variables of PROGRAM POUs. Each access variable is also assigned a field qualifier which indicates permissions given to an external module trying to access it. If an access variable only has READ permissions, it cannot be written to by an external module.

* The **GLOBAL_BOOL_VAR** access variable points to the memory location of the **global_bool_var** global variable (which is also declared in the **var_global** section in the specification file. Similarly **GLOBAL_INT_VAR**, **GLOBAL_BOOL_ARR** and **GLOBAL_INT_ARR** also point to other global variables.

*  **ACCESS_INT_ARR** is a 2 x 2 INT array pointing to RAM memory 100 while **MOTOR_OUT_3** is of type COMPLEX_STRUCT_3 and points to output memory byte no 2000 of CPU_001

* Access variables can also point to input and output memory locations of POUs of type PROGRAMS. These POUs could be defined in any of the managed CPUs. For instance, in this example, **SENSOR_IN_1** and **SENSOR_IN_2** point to sensor_input_1 and sensor_input_2 input variables of PROGRAM_1 defined in CPU_001. Similarly, **MOTOR_OUT_1** and **MOTOR_OUT_2** point to two output variables of the same program.

Access variables can be used to build custom communication modules as we show in next section.

