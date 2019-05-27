# Description:  This programs runs a simulation of a cart-pole system
#		The conrol algorithm used to balance the pendulum is PID
 
import numpy as np,cv2,math,time,matplotlib.pyplot as plt,sys
import grpc

from contrib.emulation_driver import EmulationDriver
import src.pc_emulator.proto.mem_access_pb2 as mem_access_proto
import src.pc_emulator.proto.mem_access_pb2_grpc as mem_access_grpc

class Cart:
    def __init__(self,x,mass,world_size):
        self.x = x  
        self.y = int(0.6*world_size) 
        self.mass = mass
        self.color = (0,255,0)

class Pendulum:
    def __init__(self,length,theta,ball_mass):
        self.length = length
        self.theta = theta
        self.ball_mass = ball_mass		
        self.color = (0,0,255)


def set_sensor_input(curr_theta):
    try:
        with grpc.insecure_channel('localhost:50051') as channel:
            stub = mem_access_grpc.AccessServiceStub(channel)
            response = stub.SetSensorInput(
                mem_access_proto.SensorInput(
                    PLC_ID='Pendulum_PLC',
                    ResourceName='CPU_001',
                    MemType=0,
                    ByteOffset=1,
                    BitOffset=0,
                    VariableDataTypeName="REAL",
                    ValueToSet = str(curr_theta)))
    except Exception as e:
        pass

def get_actuator_output():
    try:
        with grpc.insecure_channel('localhost:50051') as channel:
            stub = mem_access_grpc.AccessServiceStub(channel)
            response = stub.GetActuatorOutput(
                mem_access_proto.ActuatorOutput(
                    PLC_ID='Pendulum_PLC',
                    ResourceName='CPU_001',
                    MemType=1,
                    ByteOffset=1,
                    BitOffset=0,
                    VariableDataTypeName="REAL"))

            if response.status == "SUCCESS":
                return float(response.value)
            #print "Returning 0.0"
            return 0.0
    except Exception as e:
        return 0.0


class PendulumSimulator(object):

    def __init__(self):
        self.mass_of_ball = 1.0
        self.mass_of_cart = 5.0
        self.g = 9.81
        self.errors, self.force, self.theta, self.times, self.x = [],[],[],[],[]
        self.world_size = 1000
        
        # Initializing cart and pendulum objects
        self.cart = Cart(int(0.5 * self.world_size),
            self.mass_of_cart, self.world_size)
        self.pendulum = Pendulum(1,1, self.mass_of_ball)

        # Initializing other variables needed for the simulation
        self.theta_dot = 0
        self.theta_tminus1 = self.theta_tminus2 = self.pendulum.theta
        self.x_tminus1 = self.x_tminus2 = self.cart.x
        self.previous_time_delta = 0

    def display_stuff(self, world_size, cart, pendulum):
        # This function displays the pendulum and cart.
        length_for_display = pendulum.length * 100
        A = np.zeros((world_size,world_size,3),np.uint8)
        cv2.line(A,(0,int(0.6 * world_size)),(world_size,int(0.6 * world_size)),(255,255,255),2)
        cv2.rectangle(A,(int(cart.x) + 25,cart.y + 15),(int(cart.x) - 25,cart.y - 15),cart.color,-1)	
        pendulum_x_endpoint = int(cart.x - (length_for_display) * math.sin(pendulum.theta))
        pendulum_y_endpoint = int(cart.y - (length_for_display) * math.cos(pendulum.theta))
        cv2.line(A,(int(cart.x),cart.y),(pendulum_x_endpoint,pendulum_y_endpoint),pendulum.color,4)
        cv2.circle(A,(pendulum_x_endpoint,pendulum_y_endpoint),6,(255,255,255),-1)
        cv2.imshow('WindowName',A)
        cv2.waitKey(5)


    def apply_control_input(self, cart,pendulum,F,time_delta,x_tminus2,theta_dot,theta_tminus2,previous_time_delta,g):
        # Finding x and theta on considering the control inputs and the dynamics of the system
        theta_double_dot = (((cart.mass + pendulum.ball_mass) * g * math.sin(pendulum.theta)) + (F * math.cos(pendulum.theta)) - (pendulum.ball_mass * ((theta_dot)**2.0) * pendulum.length * math.sin(pendulum.theta) * math.cos(pendulum.theta))) / (pendulum.length * (cart.mass + (pendulum.ball_mass * (math.sin(pendulum.theta)**2.0)))) 
        x_double_dot = ((pendulum.ball_mass * g * math.sin(pendulum.theta) * math.cos(pendulum.theta)) - (pendulum.ball_mass * pendulum.length * math.sin(pendulum.theta) * (theta_dot**2)) + (F)) / (cart.mass + (pendulum.ball_mass * (math.sin(pendulum.theta)**2)))
        cart.x += ((time_delta**2) * x_double_dot) + (((cart.x - x_tminus2) * time_delta) / previous_time_delta)
        pendulum.theta += ((time_delta**2)*theta_double_dot) + (((pendulum.theta - theta_tminus2)*time_delta)/previous_time_delta)
        
    
    def progress_for(time_step_size):
        if self.previous_time_delta != 0:
            self.theta_dot = (self.theta_tminus1 
                - self.theta_tminus2 ) / self.previous_time_delta				
            self.x_dot = (self.x_tminus1 - self.x_tminus2) / self.previous_time_delta

            F = get_actuator_output()
            apply_control_input(self.cart, self.pendulum, F, time_step_size,
                self.x_tminus2, self.theta_dot, self.theta_tminus2,
                self.previous_time_delta, self.g)


        # Update the variables and display stuff
        set_sensor_input(self.pendulum.theta)
        display_stuff(self.world_size, self.cart, self.pendulum)
        self.previous_time_delta = time_step_size
        self.theta_tminus2 = self.theta_tminus1 
        self.theta_tminus1 = self.pendulum.theta
        self.x_tminus2 = self.x_tminus1
        self.x_tminus1 = self.cart.x


def main(is_virtual=False,
        num_nodes=2,
        run_time=10, 
        rel_cpu_speed=1.0,
        num_insns_per_round=1000000):

    if is_virtual:
        current_timestamp = 0.0
    else:
        current_timestamp = time.time()

    simulation_time = run_time
    end_time = current_timestamp + simulation_time

    emulation = EmulationDriver(number_nodes=num_nodes, is_virtual=is_virtual,
        n_insns_per_round=num_insns_per_round, rel_cpu_speed=rel_cpu_speed)    
    pendulum_sim = PendulumSimulator()


	synchronization_timestep = emulation.timestep_per_round_secs
    while current_timestamp <= end_time:

        if is_virtual == False:		
            current_timestamp = time.time()
        else:
            current_timestamp += synchronization_timestep

        if is_virtual == True and (current_timestamp * 10) % 1 == 0 :
            print "Current Time: ", current_timestamp

            
        emulation.progress_for(synchronization_timestep)
        pendulum_sim.progress_for(synchronization_timestep)


if __name__ == "__main__":
	main(is_virtual=False, 
        num_nodes=2,
        run_time=10, 
        rel_cpu_speed=1.0,
        num_insns_per_round=1000000)