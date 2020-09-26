# Description:  This programs runs a simulation of a cart-pole system
#		The conrol algorithm used to balance the pendulum is PID
# To start this simulation run:
#  	cd ~/OpenSCADA/examples/inverted_pendulum
#	sudo python simulation.py --is_virtual=<True/False>
 


from contrib.emulation_driver import EmulationDriver
from examples.common.single_pendulum_sim import PendulumSimulator
import argparse
import os
import signal
import sys
import time

stop = False



def handler(signum, frame):
    global stop
    print ('Pressed Ctrl-C! Scheduled clean exit ...')
    stop = True
    


def fork_run_command(cmd_as_list, log_file_fd=None):
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

def get_comm_module_start_command(path_to_plc_specification_file, ip_address_to_listen,
    listen_port, plc_resource_to_attach, log_file_path):
    return f"modbus_comm_module -f {path_to_plc_specification_file} -i {ip_address_to_listen} -p {listen_port} -r {plc_resource_to_attach} -l {log_file_path}"


def get_example_hmi_start_command(ip_address_to_connect, port_to_connect, log_file_path):
    return f"example_hmi -i {ip_address_to_connect} -p {port_to_connect} -l {log_file_path}"
    


def main(num_dilated_nodes=3,
        run_time_secs=5, 
        rel_cpu_speed=1.0,
        num_insns_per_round=1000000):

    global stop

    

    parser = argparse.ArgumentParser()

    parser.add_argument('--plc_spec_file', dest='plc_spec_file',
                        default=f"{os.path.expanduser('~')}/OpenSCADA/examples/inverted_pendulum/plc_system_specification.prototxt",
                        help='path to plc spec prototxt file')

    parser.add_argument('--is_virtual', dest='is_virtual',
                        help='with Kronos', default="False")

    parser.add_argument('--plc_spec_dir', dest='plc_spec_dir',
            default=f"{os.path.expanduser('~')}/OpenSCADA/examples/inverted_pendulum",
            help='path to directory containing spec protoxt files of all plcs')

    parser.add_argument('--comm_module_bind_ip', dest='comm_module_bind_ip',
                        help='ip_address to which comm module would bind', \
                        default="0.0.0.0")

    parser.add_argument('--comm_module_listen_port', dest='comm_module_listen_port',
                        help='listen port of comm module', default="1502")

    parser.add_argument('--comm_module_attached_resource', \
        dest='comm_module_attached_resource',
        help='comm module attaches to this resource of the plc',
        default='CPU_001')

    if os.path.exists("/tmp/pc_grpc_server_log.txt"):
        os.remove("/tmp/pc_grpc_server_log.txt")

    if os.path.exists("/tmp/plc_log.txt"):
        os.remove("/tmp/plc_log.txt")

    if os.path.exists("/tmp/comm_module_log.txt"):
        os.remove("/tmp/comm_module_log.txt")

    if os.path.exists("/tmp/example_hmi.txt"):
        os.remove("/tmp/example_hmi.txt")

    fd1 = os.open( "/tmp/pc_grpc_server_log.txt", os.O_RDWR | os.O_CREAT )
    args = parser.parse_args()
     
    pendulum_sim = PendulumSimulator()
    if args.is_virtual == "True":
        is_virtual = True
    else:
        is_virtual = False

    emulation = EmulationDriver(number_dilated_nodes=num_dilated_nodes, 
        is_virtual=is_virtual, n_insns_per_round=num_insns_per_round, 
        rel_cpu_speed=rel_cpu_speed, physical_system_sim_driver=pendulum_sim)

    # Start pc_grpc_server, all PLCs and all communication modules here 
    emulation.start_grpc_server(args.plc_spec_dir, fd1)

    print ("Starting PLC ...")
    plc_pid = fork_run_command(emulation.get_plc_exec_command(
				path_to_plc_specification_file=args.plc_spec_file,
                                log_file_path="/tmp/plc_log.txt",
				as_list=True))
   
    
    print ("Starting Modbus Comm module ...")
    comm_module_start_command = get_comm_module_start_command(
	args.plc_spec_file, args.comm_module_bind_ip, args.comm_module_listen_port,
        args.comm_module_attached_resource, "/tmp/comm_module_log.txt")

    comm_module_pid = fork_run_command(emulation.wrap_command(
				orig_cmd_string=comm_module_start_command,
				as_list=True))

    #print ("Starting HMI ...")
    hmi_start_command = get_example_hmi_start_command(
	args.comm_module_bind_ip, args.comm_module_listen_port, "/tmp/example_hmi.txt")

    hmi_pid = fork_run_command(emulation.wrap_command(
				orig_cmd_string=hmi_start_command,
				as_list=True))

    # Wait until all processes have started and registered themselves
    emulation.wait_for_initialization()
    signal.signal(signal.SIGINT, handler)

    total_time_elapsed = 0.0
    while total_time_elapsed <= run_time_secs:
        
        emulation.run_for(0.01)
        total_time_elapsed += 0.01
        #if is_virtual and total_time_elapsed >= 0.98:
        #    input('Press key to continue')
        if is_virtual:
            print ("Time Elapsed: ", total_time_elapsed)
        if stop == True:
            break

    print ("Stopping Emulation ...")
    sys.stdout.flush()
    emulation.stop_exp() 

    os.close(fd1)
    pendulum_sim.finish_video()

    print ("Interrupting all spawned processes !")
    if is_virtual == False:
        os.kill(plc_pid, signal.SIGINT)
        os.kill(comm_module_pid, signal.SIGINT)
        os.kill(hmi_pid, signal.SIGINT)

    print ("Emulation finished ! ")




if __name__ == "__main__":
	main()
