"""
This is a standalone script to run a small switch based scenario and will not
interact with the GUI.

To run the example:
   cd $OSCADA_INSTALLATION/examples/core_examples/inverted_pendulum
   sudo core-python simulation.py --is_virtual=<True/False>

The follwoing simple example topology can be emulated with and without virtual time integration.

	Pendulum Simulator-1   <--->  [ PLC-1 + comm-module (core-node) ] 		   [ MODBUS HMI-1 (core node) ]
							\				  /
							 ------- [ (core switch) ] -------
							/				  \
	Pendulum Simulator-2   <--->  [ PLC-2 + comm-module (core node) ]		   [ MODBUS HMI-2 (core node) ]


Traffic flows in network:

	MODBUS
HMI-1 <---------> PLC-1

	MODBUS
HMI-2 <---------> PLC-2

"""

import logging

import argparse
import os
import signal
import sys
import time

from core.emulator.coreemu import CoreEmu
from core.emulator.data import IpPrefixes
from core.emulator.data import LinkOptions
from core.emulator.enumerations import EventTypes
from core.nodes.base import CoreNode
from core.nodes.network import SwitchNode



from contrib.emulation_driver import EmulationDriver
from examples.common.two_pendulums_2_plcs_sim import PendulumSystemSimulator

stop = False



def handler(signum, frame):
    global stop
    print ('Pressed Ctrl-C! Scheduled clean exit ...')
    stop = True
    


def fork_run_command(cmd_as_list, log_file_fd=None):
    """Forks and runs a command."""
    if not cmd_as_list:
        return None

    newpid = os.fork()
    if newpid == 0:
        if log_file_fd:
            os.dup2(log_file_fd, sys.stdout.fileno())
            os.dup2(log_file_fd, sys.stderr.fileno())
        
        os.setpgrp()
        os.execvp(cmd_as_list[0], cmd_as_list)
        sys.exit(0)
    else:
        print ("Started command: ", " ".join(x for x in cmd_as_list), " with pid: ",  newpid)
        return newpid

def get_comm_module_start_command(path_to_plc_specification_file, ip_address_to_listen, log_file_path,
    listen_port=1502, plc_resource_to_attach="CPU_001"):
    """Returns a command string to start a modbus_comm_module binary."""

    return f"modbus_comm_module -f {path_to_plc_specification_file} -i {ip_address_to_listen} -p {listen_port} -r {plc_resource_to_attach} -l {log_file_path}"


def get_example_hmi_start_command(ip_address_to_connect, log_file_path, port_to_connect=1502):
    """Returns a command string to start a example hmi binary which talks modbus with a modbus comm module."""

    return f"example_hmi -i {ip_address_to_connect} -p {port_to_connect} -l {log_file_path}"


def main():

    parser = argparse.ArgumentParser()
    parser.add_argument('--is_virtual', dest='is_virtual',
                        help='with Kronos', default="False")

    args = parser.parse_args()


    if args.is_virtual == "True":
        is_virtual = True
    else:
        is_virtual = False


    # Kronos specific parameters
    total_num_dilated_executables = 6 # (2 plcs + 2 communication modules + 2 hmis )
    run_time_secs = 5
    rel_cpu_speed=1.0
    num_insns_per_round=1000000


    plc_spec_directory = os.path.dirname(os.path.realpath(__file__))
    plc1_spec_file = f"{plc_spec_directory}/plc1_system_specification.prototxt"
    plc2_spec_file = f"{plc_spec_directory}/plc2_system_specification.prototxt"

    NUM_PLCS = 2
    NUM_HMIS = 2
   
    # ip generator for example
    prefixes = IpPrefixes(ip4_prefix="10.83.0.0/16")

    # create emulator instance for creating sessions and utility methods
    coreemu = CoreEmu()
    session = coreemu.create_session()

    # must be in configuration state for nodes to start, when using "node_add" below
    session.set_state(EventTypes.CONFIGURATION_STATE)

    # create switch network node
    switch = session.add_node(SwitchNode, _id=100)

    # create nodes
    for _ in range(NUM_PLCS + NUM_HMIS):
        node = session.add_node(CoreNode)
        interface = prefixes.create_iface(node)
        session.add_link(node.id, switch.id, iface1_data=interface, options=LinkOptions(delay=1000)) # delay in us

    # instantiate session
    session.instantiate()

    node_ifaces = []

    # get nodes to run example
    plc1_node = session.get_node(1, CoreNode)
    node_ifaces.extend([x.localname for x in plc1_node.get_ifaces(control=False)])

    plc2_node = session.get_node(2, CoreNode)
    node_ifaces.extend([x.localname for x in plc2_node.get_ifaces(control=False)])

    hmi1_node = session.get_node(3, CoreNode)
    node_ifaces.extend([x.localname for x in hmi1_node.get_ifaces(control=False)])

    hmi2_node = session.get_node(4, CoreNode)
    node_ifaces.extend([x.localname for x in hmi2_node.get_ifaces(control=False)])


    print ("node-ifaces ", node_ifaces)
    plc1_ip_address = prefixes.ip4_address(plc1_node.id)
    plc2_ip_address = prefixes.ip4_address(plc2_node.id)

    print (f"PLC-1 IP: {plc1_ip_address}, PLC-2 IP: {plc2_ip_address}")

    # Clear any existing log files
    if os.path.exists("/tmp/pc_grpc_server_log.txt"):
        os.remove("/tmp/pc_grpc_server_log.txt")
    if os.path.exists("/tmp/plc1_log.txt"):
        os.remove("/tmp/plc1_log.txt")
    if os.path.exists("/tmp/plc2_log.txt"):
        os.remove("/tmp/plc2_log.txt")
    if os.path.exists("/tmp/comm_module1_log.txt"):
        os.remove("/tmp/comm_module1_log.txt")
    if os.path.exists("/tmp/comm_module2_log.txt"):
        os.remove("/tmp/comm_module2_log.txt")
    if os.path.exists("/tmp/hmi1.txt"):
        os.remove("/tmp/hmi1.txt")
    if os.path.exists("/tmp/hmi2.txt"):
        os.remove("/tmp/hmi2.txt")

    fd1 = os.open( "/tmp/pc_grpc_server_log.txt", os.O_RDWR | os.O_CREAT )
     
    pendulum_sim = PendulumSystemSimulator()
    if args.is_virtual == "True":
        is_virtual = True
    else:
        is_virtual = False

    # Create an emulation driver. Register pendulum system simulator with it.
    emulation = EmulationDriver(number_dilated_nodes=total_num_dilated_executables, 
        is_virtual=is_virtual, n_insns_per_round=num_insns_per_round, 
        rel_cpu_speed=rel_cpu_speed, physical_system_sim_driver=pendulum_sim)

    # Start pc_grpc_server, all PLCs and all communication modules here 
    emulation.start_grpc_server(plc_spec_directory, fd1)
    
    if is_virtual :
        emulation.add_interfaces_to_vt_control(node_ifaces)

    # Retrieve command strings to run PLCs/HMIs/Communication Modules
    plc1_cmd = emulation.get_plc_exec_command(path_to_plc_specification_file=plc1_spec_file, log_file_path="/tmp/plc1_log.txt")
    plc2_cmd = emulation.get_plc_exec_command(path_to_plc_specification_file=plc2_spec_file, log_file_path="/tmp/plc2_log.txt")
    
    comm_module1_cmd = emulation.wrap_command(get_comm_module_start_command(plc1_spec_file, plc1_ip_address, "/tmp/comm_module1_log.txt"))
    comm_module2_cmd = emulation.wrap_command(get_comm_module_start_command(plc2_spec_file, plc2_ip_address, "/tmp/comm_module2_log.txt"))

    hmi1_cmd = emulation.wrap_command(get_example_hmi_start_command(plc1_ip_address, "/tmp/hmi1.txt"))
    hmi2_cmd = emulation.wrap_command(get_example_hmi_start_command(plc2_ip_address, "/tmp/hmi2.txt"))

    print ("Starting PLCs ...")
    plc1_node.cmd(plc1_cmd, wait=False)
    plc2_node.cmd(plc2_cmd, wait=False)
    print ("Starting PLC Modbus Comm modules ...")
    plc1_node.cmd(comm_module1_cmd, wait=False)
    plc2_node.cmd(comm_module2_cmd, wait=False)
    print ("Starting HMI ...")
    hmi1_node.cmd(hmi1_cmd, wait=False)
    hmi2_node.cmd(hmi2_cmd, wait=False)
    

    # Wait until all processes have started and registered themselves
    emulation.wait_for_initialization()

    # Register an interrupt signal handler.
    signal.signal(signal.SIGINT, handler)

    total_time_elapsed = 0.0
    while total_time_elapsed <= run_time_secs:
        
        emulation.run_for(0.01)
        total_time_elapsed += 0.01
        if is_virtual:
            print ("Time Elapsed: ", total_time_elapsed)
        if stop == True:
            break

    print ("Stopping Emulation ...")
    sys.stdout.flush()
    emulation.stop_exp() 

    os.close(fd1)

    # shutdown session
    coreemu.shutdown()

    print ("Emulation finished ! ")


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    main()
