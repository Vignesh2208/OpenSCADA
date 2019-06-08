# Description:  This programs runs a simulation of a cart-pole system
#		The conrol algorithm used to balance the pendulum is PID
 


from contrib.emulation_driver import EmulationDriver
from pendulum_sim import PendulumSimulator
import argparse
import os
import signal


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
        os.execvp("pc_grpc_server", path_to_plc_specifications_dir)
    else:
        print "Started PC GRPC Server with pid ", newpid
        return newpid

def start_plc(path_to_plc_specification_file, is_virtual, log_file_fd):

    newpid = os.fork()
    if newpid == 0:
        os.dup2(log_file_fd, sys.stdout.fileno())
        os.dup2(log_file_fd, sys.stderr.fileno())
        if is_virtual:
            
            os.execvp("plc_runner", "-f", path_to_plc_specification_file, "-e")
        else:
            os.execvp("plc_runner", "-f", path_to_plc_specification_file)
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
            cmd_str = "modbus_comm_module -f %s \
                -i %s -p %s -r  %s" % (path_to_plc_specification_file,
                    ip_address_to_listen, listen_port, resource_to_attach)
            
            os.execvp("tracer", "-c", cmd_str, "-r", rel_cpu_speed, "-n", \
                n_insns_per_round)
        else:
            os.execvp("modbus_comm_module", "-f", path_to_plc_specification_file,
                "-i", ip_address_to_listen, "-p", listen_port, "-r", resource_to_attach)
    else:
        print "Started Modbus comm module with pid ", newpid
        return newpid


def main(is_virtual=False,
        num_dilated_nodes=2,
        run_time=10, 
        rel_cpu_speed=1.0,
        num_insns_per_round=1000000):

    global stop

    signal.signal(signal.SIGINT, handler)

    parser = argparse.ArgumentParser()

    parser.add_argument('--plc_spec_file', dest='plc_spec_file',
                        help='path to plc spec prototxt file')

    parser.add_argument('--plc_spec_dir', dest='plc_spec_dir',
            help='path to directory containing spec protxt files of all plcs')

    parser.add_argument('--comm_module_bind_ip', dest='comm_module_bind_ip',
                        help='ip_address to which comm module would bind', \
                        default="0.0.0.0")

    parser.add_argument('--comm_module_listen_port', dest='comm_module_listen_port',
                        help='listen port of comm module', default="1502")

    parser.add_argument('--comm_module_attached_resource', \
        dest='comm_module_attached_resource',
        help='comm module attaches to this resource of the plc')


    fd1 = os.open( "/tmp/pc_grpc_server_log.txt", os.O_RDWR | os.O_CREAT )
    fd2 = os.open( "/tmp/plc_log.txt", os.O_RDWR | os.O_CREAT )
    fd3 = os.open( "/tmp/comm_module_log.txt", os.O_RDWR | os.O_CREAT )

    args = parser.parse_args()
     
    pendulum_sim = PendulumSimulator()

    emulation = EmulationDriver(number_nodes=num_dilated_nodes, is_virtual=is_virtual,
        n_insns_per_round=num_insns_per_round, rel_cpu_speed=rel_cpu_speed,
        physical_system_sim_driver=pendulum_sim)

    # Start pc_grpc_server, all PLCs and all communication modules here 
    print "Starting PC GRPC Server ..."
    grpc_server_pid = start_grpc_server(args.plc_spec_dir, fd1)

    print "Starting PLC ..."
    plc_pid = start_plc(args.plc_spec_file, is_virtual, fd2)

    print "Starting Modbus Comm module ..."
    comm_module_pid = start_comm_module(args.plc_spec_file, \
        args.comm_module_bind_ip, args.comm_module_listen_port, \
        args.comm_module_attached_resource, is_virtual, rel_cpu_speed, \
        n_insns_per_round, fd3)


    # Wait until all processes have started and registered themselves
    emulation.wait_for_initialization()

    total_time_elapsed = 0.0
    while total_time_elapsed <= run_time:
        emulation.run_for(0.1)
        total_time_elapsed += 0.1

        if stop == True:
            break

    print "Stopping Emulation ..."
    emulation.stop_exp() 

    os.close(fd1)
    os.close(fd2)
    os.close(fd3) 


if __name__ == "__main__":
	main()