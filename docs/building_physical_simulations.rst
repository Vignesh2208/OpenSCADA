Simulating Physical Systems
===========================

OpenSCADA PLCs can communicate with simulations of physical systems or processes. To interact with PLCs, a proxy Sensor/Actuator driver can be built using the GRPC API already discussed in the Section PLC I/O. 

An abstract python class called PhysicalSystemSim is defined in `physical_system_sim.py <https://github.com/Vignesh2208/OpenSCADA/tree/master/contrib/physical_system_sim.py>`_ and every physical system simulation must implement an object derived from this class::

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

A specific implementation of a physical system simulation must inherit from this abstract class and register itself with an object of type EmulationDriver which is a built-in class defined inside `emulation_driver.py <https://github.com/Vignesh2208/OpenSCADA/tree/master/contrib/emulation_driver.py>`_. The **progress** method needs to be implemented by the inherited object. It is called implicitly by the emulation_driver and when upon each call, the logic embedded in the physical system simulation should be advance the simulation for the specified duration in seconds and pause it. During this progress, the logic may set sensor input's of PLCs and get Actuator outputs from PLCs.

An example pendulum simulator is included with the installation for reference. It is implemented in `pendulum_sim.py <https://github.com/Vignesh2208/OpenSCADA/tree/master/examples/iverted_pendulum/pendulum_sim.py>`_ file located inside examples/inverted_pendulum. This example implements a cart pole which can move left or right inresponse to an applied force. A PLC reads the current angle of the pendulum and applies force on the cart to move it left or right so that the pendulum stays upright. 

The pendulum simulator implements the **progress** method. Inside this method, it gets the actuator output (which is force applied on the cart) and applies it in the simulation. It then sets the current pendulum angle as a sensor input to the PLC. It uses the GRPC API described in Section PLC I/O to interact with the attached PLC.

In the next section, we build on this discussion and show how to use the built-in EmulationDriver class to configure and run time synchronized OpenSCADA PLCs and physical system simulations.
