import sys
import os
import time
import kronos_functions as kf

class EmulationDriver(object):

    def __init__(self, number_nodes, n_insns_per_round=1000000,
            rel_cpu_speed=1.0, is_virtual=False,
            physical_system_sim_driver=None):
        self.is_virtual = is_virtual
        self.num_tracers = number_nodes
        self.n_progressed_rounds = 0
        self.timestep_per_round_secs 
        = (float(n_insns_per_round)/rel_cpu_speed)/1000000000.0
        self.total_time_elapsed = 0.0
        assert number_nodes > 0 
        if self.is_virtual == True:
            print "Initializing Kronos ..."
            kf.initializeExp(1)

        self.physical_system_sim_driver = physical_system_sim_driver

    def wait_for_initialization(self):
        print "Waiting for all nodes to register ..."
        if self.is_virtual == True:
            kf.synchronizeAndFreeze(self.num_tracers)
        print "Resuming ..."

    def progress_for(self, time_step_secs):

        if self.is_virtual and self.n_progressed_rounds == 0 :
            print "Starting Synchronized Experiment ..."
            kf.startExp()

        if self.is_virtual:
            n_rounds = float(time_step_secs) / self.timestep_per_round_secs

            if n_rounds <= 0 :
                n_rounds = 1
            kf.progress_n_rounds(n_rounds)
        else:
            time.sleep(time_step_secs)

        self.total_time_elapsed += float(time_step_secs)
        if self.physical_system_sim_driver is not None:
            self.physical_system_sim_driver.progress(float(time_step_secs))

    def run_for(self, run_time):
        if self.is_virtual:
            current_timestamp = 0.0
        else:
            current_timestamp = time.time()
        end_time = current_timestamp + run_time

        synchronization_timestep = self.timestep_per_round_secs
        while current_timestamp <= end_time:

            if self.is_virtual == False:		
                current_timestamp = time.time()
            else:
                current_timestamp += synchronization_timestep

            if self.is_virtual == True and (current_timestamp * 10) % 1 == 0 :
                print "Current Time: ", current_timestamp
                
            self.progress_for(synchronization_timestep)



    def stop_exp(self):
        if self.is_virtual:
            print "Stopping Synchronized Experiment ..."
            kf.stopExp()