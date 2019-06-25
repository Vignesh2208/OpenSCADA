PLC I/O
=======

This section discusses how to interact with a PLC to read its CPU's output memory and set values to its CPU's input memory. The ability to write values to the input memory of a PLC's CPU is necessary to simulate sensors in the physical environment. Sensors typically measure quantities in the physical system and these measured values appear in the input memory. The ability to read values from the output memory of a PLC's CPU is necessary to simulate actuators. The PLC's logic typically writes values to the output memory and this is sent to one or more connected actuators. To write/read from input/output memories of a PLC's CPU, OpenSCADA provides a unified grpc interface. 

A grpc server is started and pointed to a directory containing the system specification files of all interested PLCs::

	pc_grpc_server <Path to directory containing system specification files of all PLCs>

The GRPC server binds to localhost and listens on port 50051. It can be quit any time by pressing Ctrl-C (i.e by sending an interrupt signal). 

For every PLC, the GRPC server upon startup reads its system spec file and creates a memory mapped files to store the contents of the input/output memories of each CPU. The PLC itself doesn't need to be running at this time and it could even be started after the GRPC server has been setup. When the PLC is launched, it will re-use the already created memory mapped files for the input/output memories of each CPU. Through memory mapped I/O, the GRPC server would be able to write to a CPU's input memory or read from its output memory. GRPC requests can be sent by external processes to write(read) to(from) the input(output) memory of a specific CPU in a particular PLC. In the following sections, we show how to construct grpc requests to write to input memory and read from output memory


Simulating Sensors
^^^^^^^^^^^^^^^^^^

Sensors are attached to a specific CPU of a PLC and write to its input memory. To simulate a sensor, the SetSensorInput defined in `mem_access.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/mem_access.proto/>`_ can be used. It needs a request message of type SensorInput with the following fields which need to be set:

* **PLC_ID**: ID of the PLC or the configuration_name specified in its system specification
* **ResourceName**: CPU Name inside the PLC
* **MemType**: Should typically be 0 - to denote Input Memory
* **ByteOffset**: Starting byte address inside the memory
* **BitOffset**: Starting bit address inside the memory. Ignored unless variable datatype is BOOL.
* **VariableDataTypeName**: Name of the variable data type. For instance if the variable data type is INT, then the memory location referred to by the ByteOffset is interpreted as an INT pointer. The value passed in the ValueToSet field is storedover 2 bytes (since sizeof(INT) is 2 bytes) starting at ByteOffset. Only elementary data type names specified in configuration.proto are allowed.
* **ValueToSet**: String representation of the value to set. Note that each datatype has a unique string representation as discussed in the IL-Programming section.

Illustration
------------

The following python function accepts a floating point argument called "curr_theta" and copies it to the input memory of CPU: "CPU_001" belonging a PLC called "Pendulum_PLC"::

	def set_sensor_input(self, curr_theta):
		try:
		    with grpc.insecure_channel('localhost:50051') as channel:
		        stub = mem_access_grpc.AccessServiceStub(channel)
		        response = stub.SetSensorInput(
		            mem_access_proto.SensorInput(
		                PLC_ID='Pendulum_PLC',
		                ResourceName='CPU_001',
		                MemType=0,
		                ByteOffset=1,
		                BitOffset=0,
		                VariableDataTypeName="REAL",
		                ValueToSet = str(curr_theta)))
		except Exception as e:
		    pass


Simulating Actuators
^^^^^^^^^^^^^^^^^^^^

Actuators are attached to a specific CPU of a PLC and read from its output memory. To simulate an actuator, the GetActuatorOutput defined in `mem_access.proto <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/proto/mem_access.proto/>`_ can be used. It needs a request message of type ActuatorOutput with the following fields which need to be set:

* **PLC_ID**: ID of the PLC or the configuration_name specified in its system specification
* **ResourceName**: CPU Name inside the PLC
* **MemType**: Should typically be 0 - to denote Input Memory
* **ByteOffset**: Starting byte address inside the memory
* **BitOffset**: Starting bit address inside the memory. Ignored unless variable datatype is BOOL.
* **VariableDataTypeName**: Name of the variable data type. For instance if the variable data type is INT, then the memory location referred to by the ByteOffset is interpreted as an INT pointer. The content of memory location over 2 bytes (since sizeof(INT) is 2 bytes) starting at ByteOffset is returned. Only elementary data type names specified in configuration.proto are allowed.


Illustration
------------

The following python function returns a floating point number starting at byte 1 of the output memory of CPU: "CPU_001" belonging to a PLC called "Pendulum_PLC"::


	def get_actuator_output(self):
		try:
		    with grpc.insecure_channel('localhost:50051') as channel:
		        stub = mem_access_grpc.AccessServiceStub(channel)
		        response = stub.GetActuatorOutput(
		            mem_access_proto.ActuatorOutput(
		                PLC_ID='Pendulum_PLC',
		                ResourceName='CPU_001',
		                MemType=1,
		                ByteOffset=1,
		                BitOffset=0,
		                VariableDataTypeName="REAL"))

		        if response.status == "SUCCESS":
		            return float(response.value)
		        return 0.0
		except Exception as e:
		    return 0.0

