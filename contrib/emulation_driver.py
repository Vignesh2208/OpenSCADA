import sys
import os
import time
import kronos_functions as kf
import sys

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
        self.timestep_per_round_secs =\
             (float(n_insns_per_round)/rel_cpu_speed)/1000000000.0
        self.total_time_elapsed = 0.0
        assert number_dilated_nodes > 0 
        if self.is_virtual == True:
            print "Initializing Kronos ..."
            if kf.initializeExp(1) < 0 :
                print "Kronos initialization failed ! Make sure you are running\
                    the dilated kernel and kronos module is loaded !"
                sys.exit(0)

        self.physical_system_sim_driver = physical_system_sim_driver

    def wait_for_initialization(self):
        """Wait for all dilated nodes to register themselves with Kronos."""

        print "Waiting for all nodes to register ..."
        if self.is_virtual == True:
            while kf.synchronizeAndFreeze(self.num_tracers) <= 0:
                print "Kronos >> Synchronize and Freeze failed. Retrying in 1 sec"
                time.sleep(1)
        print "Resuming ..."

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
            print "Starting Synchronized Experiment ..."
            kf.startExp()

        if self.is_virtual:
            n_rounds = float(time_step_secs) / self.timestep_per_round_secs

            if n_rounds <= 0 :
                n_rounds = 1
            kf.progress_n_rounds(int(n_rounds))
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
            print "Stopping Synchronized Experiment ..."
            kf.stopExp()
