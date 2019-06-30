Instruction List Programming
============================

This section of the guide describes how Instruction List (IL) programs can be written in OpenSCADA. It assumes that the reader has some familiarity with IL programs and is not designed to be a tutorial for IL programming. It must be noted that the IL backend in OpenSCADA is designed to be simple and it does not exhaustively include all features specified in IEC-61131-3. A Few of them have been omitted in the implementation for convenience and they are documented here.

We first describe the fundamental building blocks of IL programs in OpenSCADA: DataTypes, Program Organization Units (POUs) and Tasks. Then, the capabilities of the current implementation are described including the set of supported IL instructions, System Functions and System Function Blocks. Finally, some of the missing IL specific features are documented for future work.

DataTypes
^^^^^^^^^

DataTypes are a fundamental part of any IL program. Each datatype in OpenSCADA is associated with a datatype category, datatype name and optional dimension attributes. If a dimension attribute is specified with the datatype definition, then it is interpreted as an array. 

DataTypes are primarily two categories: Elementary and Non-Elementary. Each category has many subcategories. Elementary data types have been implicitly defined and they do not need to be declared during system specification. The table given below lists all the elementary data type names, their categories and size in bits. It also includes an example string representation of a value of the data type. (Note that strings in OpenSCADA are character arrays with fixed length of 1000)

	=============	==============	=============	========================
	DataType Name	Category	Size in Bits	Example
	=============	==============	=============	========================
	BOOL	   	BOOL		1		"TRUE"
    	BYTE	   	BYTE		8		"16#A1"
    	WORD	   	WORD		16		"16#A102"
    	DWORD	  	DWORD		32		"16#A1010101"
    	LWORD	  	LWORD		64		"16#B101010101010101"
    	CHAR	   	CHAR		8		'a'
    	USINT	  	USINT		8		"123"
    	SINT	   	SINT		8		"-123"
    	UINT	   	UINT		16		"123"
    	INT	    	INT		16		"123"
    	UDINT	 	UDINT		32		"123"
    	DINT	  	DINT		32		"123"
    	ULINT	 	ULINT		64		"123"
    	LINT	  	LINT		64		"123"
    	REAL	  	REAL		32		"0.2"
    	LREAL	 	LREAL		64		"0.234"
    	TIME	  	TIME		64		"t#1.2s"
    	DATE	  	DATE		96		"d#02-01-2010"
    	TOD		TIME_OF_DAY	96		"tod#23:59:59"
    	DT		DATE_AND_TIME	192		"dt#02-01-2010 23:59:59"
	STRING		CHAR		8000		"Hello how are you!"
	=============	==============	=============	========================

**Declaring a data type**: Custom data types can be declared in the system specification protoxt file using a datatype_declaration field of type DataType message defined in `configuration.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/configuration.proto/>`_. Its fields are explained below:

* **name**: Name of the datatype
* **datatype_category**: Category of the datatype. If a custom datatype is to be defined, the category needs to be specified as DERIVED. A DERIVED datatype cannot have datatype field of POU category. By default, it is set to POU.
* **pou_type**: If the datatype_category is POU, this field denotes the type of POU: i.e Function (FN), Function Block (FB) or Program (PROGRAM)
* **datatype_field**: Must be specified only if datatype_category is POU or DERIVED. This describes an individual field of the complex datatype. 
	* *field_name*: Name of the field
        * *field_datatype_name*: Name of the datatype of the field. It cannot be a POU category datatype if the parent datatype_category is not POU
	* *initial_value*: (string) May only be specified if the category of this field's datatype belongs to one of the categories in the table listed above
        * *dimension_1*: must only be specified if this field is supposed to be an array
	* *dimension_2*: must only be specified if this field is supposed to be a 2d-array
	* *intf_type*: Refers to interface type of the field. It may only be specified if the parent datatype_category is POU. The interface type of a field changes the way it is interpreted upon POU's invocation. Since POUs contain the execution logic and need to be invoked, its fields are treated as arguments and return values. Field interface types decide which fields are input arguments, return values, both or neither. There are several supported field interface types which are described here:
		* VAR_INPUT: The field is treated as an input variable. Its value can only be read within the called POU and can only be set from the calling POU
    		* VAR_OUTPUT: The field is treated as an output variable. Its value can only be set within the called POU and it can only be read from the calling POU
    		* VAR_IN_OUT: The field is treated as both input/output variable. During the time of POU invocation, another variable from calling POU's scope is assigned to this field. It could be modified within the called POU and these modifications reflect changes in the calling POU as well.
    		* VAR: The field is internal to the POU and equivalent to a local variable. But unlike local variable in the traditional sense, its value is retained across invocations if the PoUType is a PROGRAM or FB. It cannot be used in a FN.
    		* VAR_TEMP: The field is internal to the POU and equivalent to a local variable.
    		* VAR_EXTERNAL: The field is equivalent to an extern variable. It cannot be accessed/assigned during POU invocation. It points to global variables declared at the resource/PLC level. The field name and field datatype names must match these previously declared global variables. The field can be accessed for read/write operations within the called POU.
		* VAR_EXPLICIT_STORAGE: The field is backed by an address in memory (RAM or IO). It is equivalent to a directly represented variable in IL terminology. These variables cannot be accessed/assigned during POU invocation but any changes to the associated memory location is reflected in the affected logic. 
		* VAR_ACCESS: Discussed in Advanced Topics
	* *field_storage_spec*: If the intf_type is VAR_EXPLICIT_STORAGE, then details on the backing memory address is specified here. It includes memory type, byte offset and bit offset. The bit offset is ignored unless the field's datatype category is BOOL. The field_storage_spec may also be specified as a string in short form: e.g "%M4.1" to denote byte 4, bit 1 in RAM or "%I2.0" to denote byte 2, bit 0 in Input memory or "%Q3.0" to denote byte 3, bit 0 in Output memory.
	* *field_qualifier*: Field qualifiers are additional optional attributes which could assigned to (1) VAR_INPUT or VAR_EXPLICIT_STORAGE boolean fields (2) VAR_ACCESS fields. For VAR_INPUT/VAR_EXPLICIT_STORAGE boolean fields, two qualifiers are allowed R_EDGE and F_EDGE which denote rising edge and falling edge respectively. When a boolean field with a R_EDGE qualifier is read inside the called POU, the read operation returns TRUE iff the field experienced a rising edge transition. Similarly, when a boolean field with a F_EDGE qualifier is read inside the called POU, the read operation returns TRUE iff the field experienced a falling edge transition. Field qualifiers for VAR_ACCESS fields are discussed later in Advanced Topics.

* **datatype_spec**: A datatype declaration may also optionally have a datatype_spec field. datatype_spec may only be specified for DataTypes where datatype_category is not in {POU, DERIVED}. It could be used to assign initial_values and convert this datatype into an array:
	* initial_value: Specified as a string only if the datatype_category is present in the table listed above
	* dimension_1: Must only be specified if this datatype is supposed to be a typedef of an ARRAY of [datatype_category] i.e if datatype_category is INT and if datatype_spec with dimension_1 = 10 is specified, then this datatype is a typedef of an integer array of size 10 (ARRAY[10] of INT)
	* dimension_2: Must only be specified if this datatype is typedeffing a 2d-array

* **code_body**: It contains the set of instructions to execute upon a POU's invocation. Thus it may only be specified if the dataype_category is POU

Illustrations
-------------

Declaring a new datatype called "TIME_TYPE_DEF" which is a typedef of the elementary TIME datatype. The initial value of any field of this datatype would be "t#1s"::

	datatype_declaration {
	    name: "TIME_ALIAS"
	    datatype_category: TIME
	    datatype_spec {
		initial_value: "t#1s"
	    }
	}

Declaring a new datatype called "INT_1DARR" which is a 1-D INT array of size 10 with initial values: {-1,0,1,2,3,4,5,6,7,8}:: 

	datatype_declaration {
	    name: "INT_1DARR"
	    datatype_category: INT
	    datatype_spec {
		initial_value: "{-1,0,1,2,3,4,5,6,7,8}"
		dimension_1:  10
	    }   
	}

Declaring a new datatype called "INT_2DARR" which is a 2-D INT array of size 2 x 2 with initial values: {{0,1},{2,3}}::

	datatype_declaration {
	    name: "INT_2DARR"
	    datatype_category: INT
	    datatype_spec {
		initial_value: "{{0,1},{2,3}}"
		dimension_1:  2
		dimension_2: 2
	    }   
	}

Declaring a structure called "COMPLEX_STRUCT_1" with multiple fields::

	datatype_declaration {
	    name: "COMPLEX_STRUCT_1"
	    datatype_category: DERIVED
	    datatype_field {
		# This field is a character array of length 1000
		field_name: "string_field"
		field_datatype_name: "STRING"
	    }
	    datatype_field {
		# This field is an integer
		field_name: "int_field"
		field_datatype_name: "INT"
	    }
	    datatype_field {
		# This field is a REAL number and its initial value is 0.1
		field_name: "real_field"
		field_datatype_name: "REAL"
		initial_value: "0.1"
	    }
	    datatype_field {
		# This field is a 1D integer array of size 10 with initial values {-1,0,1,2,3,4,5,6,7,8} 
		field_name: "oned_arr_field"
		field_datatype_name: "INT_1DARR"
	    }
	    datatype_field {
		# This field is a 2D integer array of size 2 x 2 with initial values {{0,1},{2,3}} 
		field_name: "twod_arr_field"
		field_datatype_name: "INT_2DARR"
	    }
	}

Declaring a nested structure with mixed type of fields i.e both elementary and non-elementary::

	datatype_declaration {
	    name: "COMPLEX_STRUCT_2"
	    datatype_category: DERIVED
	    datatype_field {
		# Note: this is a field of the previously declared COMPLEX_STRUCT_1 datatype
		field_name: "complex_field"
		field_datatype_name: "COMPLEX_STRUCT_1"
	    }
	    datatype_field {
		field_name: "int_field"
		field_datatype_name: "INT_TYPE_DEF"
	    }
	    datatype_field {
		field_name: "word_field"
		field_datatype_name: "WORD"
		initial_value: "16#1"
	    }
	    datatype_field {
		field_name: "time_field"
		field_datatype_name: "TIME"
	    }
	    datatype_field {
		field_name: "date_field"
		field_datatype_name: "DATE"
	    }
	    datatype_field {
		field_name: "date_tod_field"
		field_datatype_name: "DT"
	    }
	    datatype_field {
		field_name: "tod_field"
		field_datatype_name: "TOD"
	    }
	}

Declaring a nested structure with fields which are arrays of derived datatypes::

	datatype_declaration {
	    name: "COMPLEX_STRUCT_3"
	    datatype_category: DERIVED
	    datatype_field {
		# Note that this field is a 1-D ARRAY of [COMPLEX_STRUCT_2] with two elements
		field_name: "complex_vector"
		field_datatype_name: "COMPLEX_STRUCT_2"
		dimension_1: 2
	    }
	}

Program Organization Units (POUs)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Program Organization Units are special categories of DataTypes which include program logic as well. POUs are blocks of code with a calling interface. The fields of a POU form its calling interface and each field is associated with an interface type (as discussed in the previous section) which decides whether the field is INPUT/OUTPUT or INOUT. There are three types of POUs: Functions (FN), Function Blocks (FBs) and PROGRAMS. Some of the IL specific rules about these POUs are re-stated here briefly for completeness. For further details, please refer the attached book.

Functions are stateless i.e every invocation of an Function with the same input produces the same output. Functions cannot have fields with interface type VAR. Function Blocks and Programs are stateful operations. Each FB, PROGRAM is like a class and variables of their type are like objects of the class. Invocations of a variable of type FB or PROGRAM can store state which may be used by the logic during its next invocation.

Here, we illustrate how POUs can be written and how then can invoke other POUs to perform various tasks. We used the PID control example given `here <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/inverted_pendulum/CPU_001.prototxt>`_. The first POU we look at is a Function Block called DivideFB which takes a Dividend, Divisor as inputs and returns a Quotient and Reminder. It also sets an error flag if the divisor is 0::

	pou_var {
	    name: "DivideFB"
	    pou_type: FB
	    datatype_field {
		field_name: "Dividend"
		field_datatype_name: "REAL"
		intf_type: VAR_INPUT
	    }
	    datatype_field {
		field_name: "Divisor"
		field_datatype_name: "REAL"
		intf_type: VAR_INPUT
	    }
	    datatype_field {
		field_name: "Quotient"
		field_datatype_name: "INT"
		intf_type: VAR_OUTPUT
	    }
	    datatype_field {
		field_name: "DivRem"
		field_datatype_name: "REAL"
		intf_type: VAR_OUTPUT
	    }
	    datatype_field {
		field_name: "DivError"
		field_datatype_name: "BOOL"
		intf_type: VAR_OUTPUT
	    }

	    code_body {
		insn: "LD 0.0" 
		insn: "EQ Divisor" 
		insn: "JMPC Error" 
		insn: "LD Dividend" 
		insn: "DIV Divisor"
		insn: "REAL_TO_INT"
		insn: "ST Quotient"
		insn: "MUL Divisor"
		insn: "ST DivRem" 
		insn: "LD Dividend"
		insn: "SUB DivRem"
		insn: "ST DivRem"
		insn: "LD FALSE"
		insn: "ST DivError"
		insn: "JMP End"
		insn: "Error: LD 0"
		insn: "ST Quotient"
		insn: "ST DivRem"
		insn: "LD TRUE"
		insn: "ST DivError"
		insn: "End: RET"
	    }
	}

The fields Dividend, Divisor, Quotient, DivRem and DivError together form the **Calling Interface** of the FB. Divided and Divisor have intf_type set to VAR_INPUT which means that a POU which calls a variable of type DivideFB, needs to set/assign values to these two fields. The FB inturn outputs three fields Quotient, DivRem and DivError which can be read by the calling POU as we show next. We do not explain the logic of the FB here but we highly encourage the reader to do so.

.. note:: One caveat needs to pointed out with respect to Labels embedded inside instructions: The labels should be inline with the instruction i.e. there cannot be a Label with no associated instruction in the same line. In the above example "Error: LD 0" cannot be split into two separate lines with one line containing the label "Error" and the other line containing the instruction "LD 0".

In the next step, we look at one other FB and a PROGRAM which invokes both. The GetControlInputFB given below has 4 INPUT, 1 INOUT, 1 OUTPUT and 5 TEMP fields. The TEMP fields are local variables with constant values. We omit its code here for brevity.::

	pou_var {
	    name: "GetControlInputFB"
	    pou_type: FB
	    datatype_field {
		field_name: "time_delta"
		field_datatype_name: "REAL"
		intf_type: VAR_INPUT
	    }
	    datatype_field {
		field_name: "curr_error"
		field_datatype_name: "REAL"
		intf_type: VAR_INPUT
	    }
	    datatype_field {
		field_name: "prev_error"
		field_datatype_name: "REAL"
		intf_type: VAR_INPUT
	    }
	    datatype_field {
		field_name: "integral"
		field_datatype_name: "REAL"
		intf_type: VAR_INPUT
	    }
	    
	    datatype_field {
		field_name: "g"
		field_datatype_name: "REAL"
		initial_value: "9.81"
		intf_type: VAR_TEMP
	    }
	    datatype_field {
		field_name: "Kp"
		field_datatype_name: "REAL"
		initial_value: "-150.0"
		intf_type: VAR_TEMP
	    }
	    datatype_field {
		field_name: "Kd"
		field_datatype_name: "REAL"
		initial_value: "-20.0"
		intf_type: VAR_TEMP
	    }
	    datatype_field {
		field_name: "Ki"
		field_datatype_name: "REAL"
		initial_value: "-20.0"
		intf_type: VAR_TEMP
	    }

	    datatype_field {
		field_name: "derivative"
		field_datatype_name: "REAL"
		intf_type: VAR_TEMP
	    }

	    datatype_field {
		field_name: "integral"
		field_datatype_name: "REAL"
		intf_type: VAR_IN_OUT
	    }

	    datatype_field {
		field_name: "force"
		field_datatype_name: "REAL"
		intf_type: VAR_OUTPUT
	    }

	    code_body {
		...
	    }
	}

In the calling interface of the PID_CONTROL program, notice two fields "div" and "get_control" whose datatypes are the two previously defined Function Blocks. This is an example of an instantiation of a POU in IL. The fields "div" and "get_control" are objects of type "DivideFB" and "GetControlInputFB" respectively. These objects can be invoked/called within the POU to execute their embedded logic. Also notice, two other fields "current_theta" and "force" which are declared as VAR_EXTERNAL. These are global variables which are defined at the resource level in the resource specification `file <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/inverted_pendulum/CPU_001.prototxt>`_::

	pou_var {
	    name: "PID_CONTROL"
	    pou_type: PROGRAM
	    datatype_field {
		field_name: "dummy_in"
		field_datatype_name: "INT"
		intf_type: VAR_INPUT
	    }
	    datatype_field {
		field_name: "dummy_out"
		field_datatype_name: "INT"
		intf_type: VAR_OUTPUT
	    }
	    datatype_field {
		field_name: "prev_time"
		field_datatype_name: "TIME"
		intf_type: VAR
	    }
	    datatype_field {
		field_name: "prev_theta"
		field_datatype_name: "REAL"
		intf_type: VAR
	    }
	    datatype_field {
		field_name: "prev_error"
		field_datatype_name: "REAL"
		intf_type: VAR
	    }
	    datatype_field {
		field_name: "prev_integral"
		field_datatype_name: "REAL"
		initial_value: "0.0"
		intf_type: VAR
	    }

	    datatype_field {
		field_name: "div"
		field_datatype_name: "DivideFB"
		intf_type: VAR
	    }

	    datatype_field {
		field_name: "get_control"
		field_datatype_name: "GetControlInputFB"
		intf_type: VAR
	    }

	    datatype_field {
		field_name: "started"
		field_datatype_name: "BOOL"
		initial_value: "False"
		intf_type: VAR
	    }

	    datatype_field {
		field_name: "curr_time"
		field_datatype_name: "TIME"
		intf_type: VAR_TEMP
	    }

	    datatype_field {
		field_name: "time_delta"
		field_datatype_name: "REAL"
		intf_type: VAR_TEMP
	    }
	    datatype_field {
		field_name: "two_pi"
		field_datatype_name: "REAL"
		initial_value: "6.28"
		intf_type: VAR_TEMP
	    }

	    datatype_field {
		field_name: "tmp"
		field_datatype_name: "REAL"
		initial_value: "0.0"
		intf_type: VAR_TEMP
	    } 
	    datatype_field {
		field_name: "curr_error"
		field_datatype_name: "REAL"
		intf_type: VAR_TEMP
	    }
	    datatype_field {
		field_name: "current_theta"
		field_datatype_name: "REAL"
		intf_type: VAR_EXTERNAL
	    }
	    datatype_field {
		field_name: "force"
		field_datatype_name: "REAL"
		intf_type: VAR_EXTERNAL
	    }
	    code_body {
		# If not started, store prev_time, prev_error
		insn: "LD started"
		insn: "JMPC already_started"
		insn: "LD TRUE"
		insn: "ST started"
		insn: "GTOD"
		insn: "ST prev_time"
		insn: "CAL div(Dividend:= current_theta, Divisor:= two_pi)"
		insn: "LD div.DivRem"
		insn: "ST tmp"
		insn: "LD 3.14"
		insn: "LT tmp"
		insn: "JMPCN else1"
		insn: "LD tmp"
		insn: "SUB two_pi"
		insn: "ST tmp"
		insn: "else1: LD tmp"
		insn: "ST prev_error"
		insn: "RET"


		insn: "already_started: GTOD"
		insn: "ST curr_time"
		insn: "SUB prev_time"
		insn: "TIME_TO_REAL"
		insn: "ST time_delta"
		insn: "LD curr_time"
		insn: "ST prev_time"
		insn: "CAL div(Dividend:= current_theta, Divisor:= two_pi)"
		insn: "LD div.DivRem"
		insn: "ST tmp"
		insn: "LD 3.14"
		insn: "LT tmp"
		insn: "JMPCN else"
		insn: "LD tmp"
		insn: "SUB two_pi"
		insn: "ST tmp"
		insn: "else: LD tmp"
		insn: "ST curr_error"
		insn: "CAL get_control(time_delta:= time_delta, curr_error:= curr_error, prev_error:= prev_error, integral:= prev_integral)"
		insn: "LD get_control.force"
		insn: "ST force"
		insn: "LD curr_error"
		insn: "ST prev_error"
		insn: "RET"
	    }
	}

The DivideFB object is called in the line::

	CAL div(Dividend:= current_theta, Divisor:= two_pi)

Its INPUT variables are set to current_theta and two_pi which are both fields in the calling POU i.e PID_CONTROL. After the invocation, one of its output is read and subsequently stored in another variable called tmp::
	
	LD div.DivRem
	ST tmp

Instead, this can also be accomplished in one line as follows::

	CAL div(Dividend:= current_theta, Divisor:= two_pi, DivRem=>tmp)

Similarly, the GetControlInputFB object is called in the line:: 
	
	CAL get_control(time_delta:= time_delta, curr_error:= curr_error, prev_error:= prev_error, integral:= prev_integral)

Its INPUT and INOUT variables are set during the invocation and its output field is read in the next line::

	LD get_control.force

On the contrary to Function Blocks and Programs, Functions cannot be instantiated with variables. Functions like TIME_TO_REAL and REAL_TO_INT (which are SFCs both used in these FBs and PROGRAM) are simply invoked as::

	FUNCTION_NAME operand1, operand2, ..., operandN

and the result of the Function can be loaded from the Accumulator after its invocation. Functions in OpenSCADA can return only one value and the specified operands are mapped to corresponding fields of the function in the same order.

Supported Instructions
----------------------

OpenSCADA ships with the following IL instructions. Some of these instructions can take multiple operands or a variable number of operands. For further details on each instruction, please consult the included references:


	==============================================================
				Instructions
	=============	==============	=============	==============

	ADD		GE		LT		NOT
	AND		GT		MOD		OR
	DIV		LD		MUL		SHL
	EQ		LE		NE		SHR
	ST		SUB		XOR		CAL
	JMP						
	=============	==============	=============	==============

Supported System Functions
--------------------------

OpenSCADA ships with the following System Functions (SFCs). Some of these SFCs can take multiple operands or a variable number of operands. For further details on each instruction, please consult the included references:


	==============================================================
			System Functions (SFCs)
	=============	==============	=============	==============

	ABS		ACOS		ASIN		ANY_TO_ANY
	ASIN		ATAN		COS		EXP
	GTOD		LIMIT		LN		LOG
	MAX		MIN		MUX		SEL
	SIN		SQRT		TAN		
	=============	==============	=============	==============
	
.. note:: The ANY_TO_ANY SFC is not actually a single SFC but a set of SFCs which are used for type casting. For example the TIME_TO_REAL and REAL_TO_INT SFCs used in the previous example are a part of this class of SFCs. For a full list of allowed type casting SFCs, consult the included references.

Supported System Function Blocks
--------------------------------

OpenSCADA ships with following System Function Blocks (SFBs). For further details on each instruction, please consult the included references:

	==============================================================
			System Function Blocks (SFBs)
	=============	==============	=============	==============

	SR		RS		R_TRIG		F_TRIG
	CTU		CTD		CTUD		TP
	TON		TOFF				
	=============	==============	=============	==============

Tasks
^^^^^

POUs merely contain the calling interface and the logic to be executed upon invocation. Tasks specify conditions underwhich a POU is invoked. There are type types of tasks:

* **Interval Tasks**: Interval Tasks are executed periodically at a specified period. In OpenSCADA, each CPU can have atmost one associated interval task. Interval tasks are specified using the **interval_task** field in Resource Specification. The following example defines an interval_task called CYCLIC_TASK which has a period of 10ms::

          interval_task {
                task_name: "CYCLIC_TASK"
                priority: 1
                interval_task_params {
                        interval_ms: 10
                }
          }


* **Interrupt Tasks**: Interrupt Tasks are configured with a boolean trigger variable which could be a PLC level global variable or a resource level global variable. Interrupt tasks are invoked when the trigger variable goes from FALSE to TRUE i.e during a positive transition. In OpenSCADA, each CPU can have multiple associated interrupt tasks and interrupt tasks always have a higher priority over interval tasks and can pre-empt any currently running interval task. If multiple interrupt tasks become simultaneously active, they are executed in the order of their priority. The trigger condition is checked after each instruction execution. The following example defines an interrupt_task called INTERRUPT_TASK which is triggered by "global_bool_var" (which is global variable) defined in System Specification::

          interrupt_task {
                task_name: "INTERRUPT_TASK"
                priority: 1
                interrupt_task_params {
                        trigger_variable_field: "global_bool_var"
                }
          }


Attaching POUs to Tasks
-----------------------

Task definition by itself does not specify which POUs are attached to a task. In OpenSCADA, POUs of type FBs and PROGRAMS can be attached to Tasks using the **programs** field of the Resource Specification. It is a message of type ProgramSpecification described in `configuration.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/configuration.proto/>`_. Each programs field defines a mapping between a POU of interest and a task of interest. Multiple POUs can be attached to the same task. A separate programs field is used for each attachement. In this example the POU "PID_CONTROL" is attached with the task "CYCLIC_TASK". Thus the program "PID_CONTROL" gets invoked every 10ms.::

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






