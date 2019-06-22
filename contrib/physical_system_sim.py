from __future__ import division
from abc import ABCMeta, abstractmethod

class PhysicalSystemSim:
    """Generic Physical system simulation abstract class.

    Any physical system simulator which needs to interact with OpenSCADA PLCs
    needs to implement this Class.
    """
    __metaclass__ = ABCMeta
 
    def __init__(self, **kwargs):
        pass
    
    @abstractmethod
    def progress(self, timestep_secs):
        """This method is called internally to advance the simulation by a step size.

        The OpenSCADA emulation_driver.py invokes this function. A physical system
        simulator should implement the logic to advance its simulation by
        timestep_secs. During this progress, it may set sensor input's of PLCs
        and get Actuator outputs from PLC's and act accordingly.

        Args:
            timestep_secs (float): Timestep to advance in secs.
        Returns:
            None
        Raises:
            None
        """
        pass