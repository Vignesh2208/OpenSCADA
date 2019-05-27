import sys
import os
import time
import kronos_functions as kf

class EmulationDriver(object):

    def __init__(self, number_nodes, n_insns_per_round=1000000,
            rel_cpu_speed=1.0, is_virtual=False):
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

        self.wait_for_initialization()

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

    def stop_exp(self):
        if self.is_virtual:
            print "Stopping Synchronized Experiment ..."
            kf.stopExp()