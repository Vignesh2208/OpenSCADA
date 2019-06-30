Creating a PLC 
==============

A PLC in OpenSCADA comprises of a RAM module and one or more CPUs. Each CPU module encompasses an Input and Output Memory module and a description of the program or set of programs to execute on that CPU. Thus a single PLC can define and run multiple Instruction List (IL) programs. To completely describe a PLC, a user would need to provide two types of configuration files:

* **A system specification**: The system specification file includes hardware specific details about the PLC which includes the number of CPUs, the size of RAM memory and the mean and standard deviation of instruction execution times. The system specification file also declares data-types which are used by all IL programs and global variables which may be used by them. Access variables (described in Advanced Topics) can also be declared and made accesible outside the PLC to communication modules.



* **A resource specification**: A separate resource specification should be defined for each CPU on the PLC. The resource specification includes the size of Input and Output memory attached to the CPU as well as all global variables specific to the CPU. Periodic and interrupt tasks can be defined in the resource specification. Functions, Function blocks and Programs to be run on that CPU are also defined and attached to tasks.

In subsequent sections, we describe each configuration in detail with examples.




