import sys
import os
import time
import kronos_functions as kf
import sys
import signal

class EmulationDriver(object):
    """A Wrapper used to drive co/simulation-emulation of PLCs and physical system simulations."""

    def __init__(self, 
            number_dilated_nodes, 
            n_insns_per_round=1000000,
            rel_cpu_speed=1.0, 
            is_virtual=False,
            physical_system_sim_driver=None):

        """Performs some initialization and also optionally initializes Kronos.

        Args:
            is_virtual: If True Kronos is initialized
            physical_system_driver: An object which implements PhysicalSystemSim
                abstract class defined in contib/physical_system_sim.py. If it
                is None, then it denotes that this co-simulation has no attached
                physical simulator.
            number_dilated_nodes: Ignored unless is_virtual is True. Denotes
                number of nodes under Kronos control. Note that each PLC's CPU
                counts as a separate node. So if there are two PLCs each with
                2 CPUs, then there are 4 dilated_nodes in total.
            rel_cpu_speed: Ignored unless is_virtual is True. Denotes the relative
                cpu speed / (equivalent to TDF). In Kronos it represents the number
                of instructions that can be executed in 1ns of virtual time.
            n_insns_per_round: Number of instructions to execute per round.
                When divided by the rel_cpu_speed, it denotes amount of time
                the co-simulation would advance in one-round. 
                
            Note:
            -----
            For the PLCs, which use Application-driven virtual time advancement mechanism (APP-VT),
            the rel_cpu_speed and n_insns_per_round indivdually have no-effect.
            They are only used to calculate the step size of each round of the PLC.
            But if there are other nodes/process in the emulation (besides PLCs
            for e.g HMIs, modbus_comm_module) which use Instruction-driven 
            virtual time advancement (INS-VT), then these two quantities become relevant.
        """
        
        self.is_virtual = is_virtual
        self.num_tracers = number_dilated_nodes
        self.n_progressed_rounds = 0
        self.timestep_per_round_secs = float(n_insns_per_round)/(rel_cpu_speed*1000000000.0)
        self.rel_cpu_speed = rel_cpu_speed
        self.n_insns_per_round = n_insns_per_round
        self.total_time_elapsed = 0.0
        self.grpc_server_pid = None
        self.spinner_pid = None
        
        if self.is_virtual == True:
            assert number_dilated_nodes > 0
            print ("Initializing Kronos ...")
            if kf.initializeExp(number_dilated_nodes + 1) < 0 :
                print ("Kronos initialization failed ! Make sure you are running "
                       "the dilated kernel and kronos module is loaded !")
                sys.exit(0)
            self.start_dummy_spinner()

        self.physical_system_sim_driver = physical_system_sim_driver        
        if os.path.isdir("/tmp/OpenSCADA"):
            print ("Removing any pre-existing files ...")
            import shutil
            shutil.rmtree("/tmp/OpenSCADA")

    def start_dummy_spinner(self):
        """Starts a dummy process and adds it to kronos control. 

        This dummy processes serves as an anchor for all virtual time controlled network interfaces.
        """
        if not self.is_virtual:
            return
        newpid = os.fork()
        spinner_cmd = ["tracer", "-c", "/bin/x64_synchronizer", "-r", str(self.rel_cpu_speed), "-s"]
        if newpid == 0:
            os.setpgrp()
            os.execvp(spinner_cmd[0], spinner_cmd)
            sys.exit(0)
        else:
            print ("Started  initialization task with pid: ",  newpid)
            self.spinner_pid = newpid
            
    def add_interfaces_to_vt_control(self, interface_names_list):
        """Accepts a list of network interfaces and adds them to kronos control.

        This is useful in a CORE experiment containing link delays. If link delays are enforced
        at a interface using the netem module, then these interfaces need to be added to kronos
        control to ensure that packet delays are enforced in virtual time.
        """
        if not self.is_virtual:
            return
        print ("Adding specified network interfaces to virtual time control")
        for intf in interface_names_list:
            if kf.setNetDeviceOwner(0, intf) < 0:
                print (f"Warning. Failed to add interface {intf} to virtual time control")

    def wait_for_initialization(self):
        """Wait for all dilated nodes to register themselves with Kronos."""
        
        if self.is_virtual == True:
            print ("Waiting for all nodes to register with kronos ...")
            while kf.synchronizeAndFreeze() <= 0:
                print ("Kronos >> Synchronize and Freeze failed. Retrying in 1 sec")
                time.sleep(1)
            print ("Resuming ...")


    def start_grpc_server(self, path_to_plc_specifications_dir, log_file_fd=None):
        """Starts a GRPC server which facilitates I/O operations on all started PLCs with any external program."""

        print ("Starting GRPC server to facilitate I/O operations on all started PLC with simulated sensors/actuators")
        newpid = os.fork()
        if newpid == 0:
            if log_file_fd:
                os.dup2(log_file_fd, sys.stdout.fileno())
                os.dup2(log_file_fd, sys.stderr.fileno())

            # We change process group here so that any signal sent to the 
            # main process doesn't automatically affect all forked children
            # This is necessary for a clean exit if interrupted
            os.setpgrp()
            args = ["pc_grpc_server", path_to_plc_specifications_dir]
            os.execvp(args[0], args)
        else:
            print ("Started PC GRPC Server with pid ", newpid)
            print ("Waiting for 10 secs for initialization to complete ...")
            time.sleep(10)
            self.grpc_server_pid = newpid
            return newpid

    def get_plc_exec_command(
        self, path_to_plc_specification_file, log_file_path=None, as_list=False):
        """Returns a command string which can be exectuted to start a running PLC emulator."""

        if self.is_virtual:
            if not as_list:
                if log_file_path:
                    return f"plc_runner -f {path_to_plc_specification_file} -e 1 -n {self.n_insns_per_round} -s {self.rel_cpu_speed} -l {log_file_path}"
                else:
                    return f"plc_runner -f {path_to_plc_specification_file} -e 1 -n {self.n_insns_per_round} -s {self.rel_cpu_speed}"
            if log_file_path:
                return ["plc_runner", "-f", path_to_plc_specification_file, "-e", "1",
                        "-n", str(self.n_insns_per_round), "-s", str(self.rel_cpu_speed), "-l", log_file_path]
            else:
                return ["plc_runner", "-f", path_to_plc_specification_file, "-e", "1",
                        "-n", str(self.n_insns_per_round), "-s", str(self.rel_cpu_speed)]
        else:
            if not as_list:
                if log_file_path:
                    return f"plc_runner -f {path_to_plc_specification_file} -l {log_file_path}"
                else:
                    return f"plc_runner -f {path_to_plc_specification_file}"
            if log_file_path:
                return ["plc_runner", "-f", path_to_plc_specification_file, "-l", log_file_path]
            else:
                return ["plc_runner", "-f", path_to_plc_specification_file]

    def wrap_command(self, orig_cmd_string, as_list=False):
        """Wraps a command around a tracer to bring it under virtual time control if needed. """
        if self.is_virtual:
            if not as_list:
                return f"tracer -c \"{orig_cmd_string}\" -r {self.rel_cpu_speed}"
            return ["tracer", "-c", orig_cmd_string, "-r", str(self.rel_cpu_speed)]
        else:
            if not as_list:
                return orig_cmd_string
            return orig_cmd_string.split(' ')

    def progress_for(self, time_step_secs):
        """Run the entire co/simulation-emulation for specified time.

        Args:
            time_step_secs (float): Time to advance by.
        Returns:
            None
        Raises:
            None
        """

        if self.is_virtual and self.n_progressed_rounds == 0 :
            print ("Starting Synchronized Experiment ...")

        if self.is_virtual:
            n_rounds = float(time_step_secs) / self.timestep_per_round_secs

            if n_rounds <= 0 :
                n_rounds = 1
            kf.progressBy(self.n_insns_per_round, int(n_rounds))
            self.n_progressed_rounds += int(n_rounds)
        else:
            pass
            #time.sleep(time_step_secs)

        self.total_time_elapsed += float(time_step_secs)
        if self.physical_system_sim_driver is not None:
            start_time = float(time.time())
            self.physical_system_sim_driver.progress(float(time_step_secs))
            end_time = float(time.time())
            if not self.is_virtual:
                left_over = time_step_secs - (end_time - start_time)
                if left_over > 0:
                    time.sleep(left_over)

    def run_for(self, run_time):
        """Run the co/simulation-emulation for specified duration.
        
        A wrapper around the previous function.
        Args:
            run_time: Duration to run for.
        Returns:
            None
        Raises:
            None
        """

        if self.is_virtual:
            current_timestamp = 0.0
        else:
            current_timestamp = time.time()
        end_time = current_timestamp + run_time

        if self.is_virtual == False:
            return self.progress_for(run_time)

        synchronization_timestep = self.timestep_per_round_secs
        while current_timestamp <= end_time:

            if self.is_virtual == False:		
                current_timestamp = time.time()
            else:
                current_timestamp += synchronization_timestep
                
            self.progress_for(synchronization_timestep)



    def stop_exp(self):
        """Stops the Kronos experiment."""

        if self.is_virtual:
            print ("Stopping Synchronized Experiment ...")
            kf.stopExp()
        if self.grpc_server_pid:
            os.kill(self.grpc_server_pid, signal.SIGINT)
