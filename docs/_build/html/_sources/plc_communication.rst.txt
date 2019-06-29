PLC Communication
=================

OpenSCADA ships with a modbus communication module which can be attached to a specific CPU in a PLC. The built-in modbus communication module connects to the input memory of the CPU and RAM memory of the PLC. It listens for modbus requests to read from the input registers and read/write to the RAM memory. A modified `libmodbus  <https://github.com/Vignesh2208/OpenSCADA/tree/master/contrib/libmodbus>`_ library included with the repository can be used to build **hmi**  clients which send modbus requests to the communication module.

Starting the Modbus Communication Module
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The built-in modbus communication module used the CommModule Interface defined `here <https://github.com/Vignesh2208/OpenSCADA/tree/master/src/pc_emulator/ext_modules/include/comm_module.h>`_. In the Advanced Topics section we will describe how this interface can be used to build more complex communication modules.

The simple modbus communication module included with the installation can be started with the command::

	modbus_comm_module [Options [-ipr]] -f <path_plc_spec_prototxt>

where the following options are optional:

 * -i: IP address to bind to. Default is localhost
 * -p: Port to listen on. Default is 1502
 * -r: Resource Name/CPU Name to attach to. This CPU must be present in the PLC.

This command starts a modus server listening on at the specified ip address, port and attaches creates a memory mapped file of the PLC's RAM and the CPU's input memory if not already present. If the PLC has already been started, it just attaches to the already created memory mapped files.

Illustration
^^^^^^^^^^^^

An example **hmi** client is included `here <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/inverted_pendulum/hmi.cc>`_. It uses a slightly modified libmodbus library which is documented `here <http://libmodbus.org/documentation/>`_. The example **hmi** client sends a modbus read request every 100 milliseconds to read from input registers 0 to 3. In Modbus terminology, each register is 2 bytes and thus register 0 refers to input memory bytes 0 and 1::

	// reads registers starting at 0 and ending at 3 i.e registers 0,1 and 2 or total of 6 bytes
	rc = modbus_read_input_registers(ctx, 0, 3, input_mem_registers);

From the read input registers, it extracts floating point value (of size 4 bytes) starting from byte number 1::

	input_mem_bytes = (uint8_t *) input_mem_registers;
        memcpy(&current_theta, &input_mem_bytes[1], sizeof(float));

.. note:: input_mem_registers which is a (unit16_t *) array needs to casted into a (unit8_t *) array to interpret read values as bytes instead of registers.

Further examples on how to design clients to write values can be found `here <https://github.com/Vignesh2208/OpenSCADA/tree/master/contrib/libmodbus/tests>`_.


