class PhysicalSystemSim(object):
 
    def __init__(self, **kwargs):
        pass
    
    @abstractmethod
    def progress(self, timestep_secs):
        pass