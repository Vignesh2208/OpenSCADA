# Description:  This programs runs a simulation of a cart-pole system
#		The conrol algorithm used to balance the pendulum is PID
# To start this simulation run:
#  	cd ~/OpenSCADA
#	bazel run :simulation -- --plc_spec_file=<system spec file> \
#				 --plc_spec_dir=<directory containing plc system spec file> \
#				 --is_virtual=<True/False>
 


from contrib.emulation_driver import EmulationDriver
from pendulum_sim import PendulumSystemSimulator
import argparse
import os
import signal
import sys


stop = False



def handler(signum, frame):
    global stop
    print 'Pressed Ctrl-C! Scheduled clean exit ...'
    stop = True
    

def start_grpc_server(path_to_plc_specifications_dir, log_file_fd):

    newpid = os.fork()
    if newpid == 0:
        os.dup2(log_file_fd, sys.stdout.fileno())
        os.dup2(log_file_fd, sys.stderr.fileno())
        # We change process group here so that any signal sent to the 
        # main process doesn't automatically affect all forked children
        # This is necessary for a clean exit if interrupted
        os.setpgrp()
        args = ["pc_grpc_server", path_to_plc_specifications_dir]
        os.execvp(args[0], args)
    else:
        print "Started PC GRPC Server with pid ", newpid
        return newpid

def start_plc(path_to_plc_specification_file, is_virtual, rel_cpu_speed, 
    n_insns_per_round, log_file_fd):

    newpid = os.fork()
    if newpid == 0:
        os.dup2(log_file_fd, sys.stdout.fileno())
        os.dup2(log_file_fd, sys.stderr.fileno())

        if is_virtual == True:
            # We change process group here so that any signal sent to the 
            # main process doesn't automatically affect all forked children
            # This is necessary for a clean exit if interrupted
            os.setpgrp()
            args = ["plc_runner", "-f", path_to_plc_specification_file, "-e", "1",
            "-n", str(n_insns_per_round), "-s", str(rel_cpu_speed)]
            os.execvp(args[0], args)
        else:
            args = ["plc_runner", "-f", path_to_plc_specification_file]
            os.execvp(args[0], args)
    else:
        print "Started PLC Runner with pid ", newpid
        return newpid

def start_comm_module(path_to_plc_specification_file, ip_address_to_listen,
    listen_port, resource_to_attach, is_virtual, rel_cpu_speed, 
    n_insns_per_round, log_file_fd):

    newpid = os.fork()

    if newpid == 0:
        os.dup2(log_file_fd, sys.stdout.fileno())
        os.dup2(log_file_fd, sys.stderr.fileno())
        
        if is_virtual:
            # We change process group here so that any signal sent to the 
            # main process doesn't automatically affect all forked children
            # This is necessary for a clean exit if interrupted
            os.setpgrp()
            cmd_str = "modbus_comm_module -f %s -i %s -p %s -r  %s" \
                % (path_to_plc_specification_file,
                    ip_address_to_listen, listen_port, resource_to_attach)
            args = ["tracer", "-c", cmd_str, "-r", str(rel_cpu_speed), "-n", \
                str(n_insns_per_round)]
            os.execvp(args[0], args)
        else:
            args = ["modbus_comm_module", "-f", path_to_plc_specification_file, \
                "-i", ip_address_to_listen, "-p", listen_port, "-r", resource_to_attach]
            os.execvp(args[0], args)
    else:
        print "Started Modbus comm module with pid ", newpid
        return newpid

def start_example_hmi(is_virtual, rel_cpu_speed, 
    n_insns_per_round, log_file_fd):
    newpid = os.fork()
    if newpid == 0:
        os.dup2(log_file_fd, sys.stdout.fileno())
        os.dup2(log_file_fd, sys.stderr.fileno())
        if is_virtual == True:
            # We change process group here so that any signal sent to the 
            # main process doesn't automatically affect all forked children
            # This is necessary for a clean exit if interrupted
            os.setpgrp()
            cmd_str = os.environ['OSCADA_INSTALLATION'] + "/bazel-bin/example_hmi_2conns"
            args = ["tracer", "-c", cmd_str, "-r", str(rel_cpu_speed), "-n", \
                str(n_insns_per_round)]
            os.execvp(args[0], args)
        else:
            args = [os.environ['OSCADA_INSTALLATION'] + "/bazel-bin/example_hmi_2conns"]
            os.execvp(args[0], args)
    else:
        print "Started example hmi with pid ", newpid
        return newpid


def main(num_dilated_nodes=5,
        run_time=5, 
        rel_cpu_speed=1.0,
        num_insns_per_round=1000000):

    global stop

    assert 'OSCADA_INSTALLATION' in os.environ

    

    parser = argparse.ArgumentParser()

    parser.add_argument('--plc_spec_file', dest='plc_spec_file',
                        help='path to plc spec prototxt file')

    parser.add_argument('--is_virtual', dest='is_virtual',
                        help='with Kronos', default="False")

    parser.add_argument('--plc_spec_dir', dest='plc_spec_dir',
            help='path to directory containing spec protxt files of all plcs')

    parser.add_argument('--comm_module_1_bind_ip', dest='comm_module_1_bind_ip',
                        help='ip_address to which comm module would bind', \
                        default="0.0.0.0")

    parser.add_argument('--comm_module_2_bind_ip', dest='comm_module_2_bind_ip',
                        help='ip_address to which comm module would bind', \
                        default="0.0.0.0")

    parser.add_argument('--comm_module_1_listen_port', dest='comm_module_1_listen_port',
                        help='listen port of comm module', default="1502")
    parser.add_argument('--comm_module_2_listen_port', dest='comm_module_2_listen_port',
                        help='listen port of comm module', default="1503")

    parser.add_argument('--comm_module_1_attached_resource', \
        dest='comm_module_1_attached_resource',
        help='comm module 1 attaches to this resource of the plc',
        default='CPU_001')

    parser.add_argument('--comm_module_2_attached_resource', \
        dest='comm_module_2_attached_resource',
        help='comm module 2 attaches to this resource of the plc',
        default='CPU_002')

    if os.path.exists("/tmp/pc_grpc_server_log.txt"):
        os.remove("/tmp/pc_grpc_server_log.txt")

    if os.path.exists("/tmp/pc_log.txt"):
        os.remove("/tmp/plc_log.txt")

    if os.path.exists("/tmp/comm_module_1_log.txt"):
        os.remove("/tmp/comm_module_1_log.txt")

    if os.path.exists("/tmp/comm_module_2_log.txt"):
        os.remove("/tmp/comm_module_2_log.txt")

    if os.path.exists("/tmp/example_hmi.txt"):
        os.remove("/tmp/example_hmi.txt")

    os.system ("rm /tmp/Input_*")
    os.system ("rm /tmp/Output_*")
    os.system ("rm /tmp/Pendulum_PLC*")
    fd1 = os.open( "/tmp/pc_grpc_server_log.txt", os.O_RDWR | os.O_CREAT )
    fd2 = os.open( "/tmp/plc_log.txt", os.O_RDWR | os.O_CREAT )    
    fd3 = os.open( "/tmp/comm_module_1_log.txt", os.O_RDWR | os.O_CREAT )
    fd4 = os.open( "/tmp/comm_module_2_log.txt", os.O_RDWR | os.O_CREAT )
    fd5 = os.open( "/tmp/example_hmi.txt", os.O_RDWR | os.O_CREAT )

    args = parser.parse_args()
     
    pendulum_sim = PendulumSystemSimulator()
    if args.is_virtual == "True":
        is_virtual = True
    else:
        is_virtual = False

    emulation = EmulationDriver(number_dilated_nodes=num_dilated_nodes, 
        is_virtual=is_virtual, n_insns_per_round=num_insns_per_round, 
        rel_cpu_speed=rel_cpu_speed, physical_system_sim_driver=pendulum_sim)

    # Start pc_grpc_server, all PLCs and all communication modules here 
    print "Starting PC GRPC Server ..."
    grpc_server_pid = start_grpc_server(args.plc_spec_dir, fd1)

    print "Starting PLC ..."
    plc_pid = start_plc(args.plc_spec_file, is_virtual, rel_cpu_speed, \
        num_insns_per_round, fd2)
    
    print "Starting 2 Modbus Comm modules ..."
    comm_module_1_pid = start_comm_module(args.plc_spec_file, \
        args.comm_module_1_bind_ip, args.comm_module_1_listen_port, \
        args.comm_module_1_attached_resource, is_virtual, rel_cpu_speed, \
        num_insns_per_round, fd3)
    comm_module_2_pid = start_comm_module(args.plc_spec_file, \
        args.comm_module_2_bind_ip, args.comm_module_2_listen_port, \
        args.comm_module_2_attached_resource, is_virtual, rel_cpu_speed, \
        num_insns_per_round, fd4)

    print "Starting HMI ..."
    example_hmi_pid = start_example_hmi(is_virtual, rel_cpu_speed, \
        num_insns_per_round, fd5)
    
    

    

    # Wait until all processes have started and registered themselves
    emulation.wait_for_initialization()

    signal.signal(signal.SIGINT, handler)

    total_time_elapsed = 0.0
    while total_time_elapsed <= run_time:
        emulation.run_for(0.001)
        total_time_elapsed += 0.001
        pendulum_sim.display()
        if is_virtual:
            print "Time Elapsed: ", total_time_elapsed
        if stop == True:
            break

    pendulum_sim.finish_video()

    print "Stopping Emulation ..."
    emulation.stop_exp() 

    os.close(fd1)
    os.close(fd2)
    os.close(fd3)
    os.close(fd4)
    os.close(fd5)



    print "Interrupting all spawned processes !"
    os.kill(grpc_server_pid, signal.SIGINT)
    
    if is_virtual == False:
        os.kill(plc_pid, signal.SIGINT)
        os.kill(comm_module_1_pid, signal.SIGINT)
        os.kill(comm_module_2_pid, signal.SIGINT)
        os.kill(example_hmi_pid, signal.SIGINT)
        os.kill(grpc_server_pid, signal.SIGINT)

    print "Emulation finished ! "




if __name__ == "__main__":
	main()
