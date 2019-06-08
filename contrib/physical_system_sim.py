from __future__ import division
from abc import ABCMeta, abstractmethod

class PhysicalSystemSim:

    __metaclass__ = ABCMeta
 
    def __init__(self, **kwargs):
        pass
    
    @abstractmethod
    def progress(self, timestep_secs):
        pass