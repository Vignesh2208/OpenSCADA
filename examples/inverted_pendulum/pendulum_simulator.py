# Description:  This programs runs a simulation of a cart-pole system
#		The conrol algorithm used to balance the pendulum is PID
 
import numpy as np,cv2,math,time,matplotlib.pyplot as plt,sys
import grpc

import src.pc_emulator.proto.mem_access_pb2 as mem_access_proto
import src.pc_emulator.proto.mem_access_pb2_grpc as mem_access_grpc

class Cart:
    def __init__(self,x,mass,world_size):
        self.x = x  
        self.y = int(0.6*world_size) 		# 0.6 was chosen for aesthetic reasons.
        self.mass = mass
        self.color = (0,255,0)

class Pendulum:
    def __init__(self,length,theta,ball_mass):
        self.length = length
        self.theta = theta
        self.ball_mass = ball_mass		
        self.color = (0,0,255)

def display_stuff(world_size,cart,pendulum):
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


def apply_control_input(cart,pendulum,F,time_delta,x_tminus2,theta_dot,theta_tminus2,previous_time_delta,g):
    # Finding x and theta on considering the control inputs and the dynamics of the system
    theta_double_dot = (((cart.mass + pendulum.ball_mass) * g * math.sin(pendulum.theta)) + (F * math.cos(pendulum.theta)) - (pendulum.ball_mass * ((theta_dot)**2.0) * pendulum.length * math.sin(pendulum.theta) * math.cos(pendulum.theta))) / (pendulum.length * (cart.mass + (pendulum.ball_mass * (math.sin(pendulum.theta)**2.0)))) 
    x_double_dot = ((pendulum.ball_mass * g * math.sin(pendulum.theta) * math.cos(pendulum.theta)) - (pendulum.ball_mass * pendulum.length * math.sin(pendulum.theta) * (theta_dot**2)) + (F)) / (cart.mass + (pendulum.ball_mass * (math.sin(pendulum.theta)**2)))
    cart.x += ((time_delta**2) * x_double_dot) + (((cart.x - x_tminus2) * time_delta) / previous_time_delta)
    pendulum.theta += ((time_delta**2)*theta_double_dot) + (((pendulum.theta - theta_tminus2)*time_delta)/previous_time_delta)


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

def main():
	# Initializing mass values, g, world size, simulation time and variables
    # required to terminate the simulation
    mass_of_ball = 1.0
    mass_of_cart = 5.0
    g = 9.81
    errors, force, theta, times, x = [],[],[],[],[]
    world_size = 1000
    simulation_time = 35
    previous_timestamp = time.time()
    end_time = previous_timestamp + simulation_time

    # Initializing cart and pendulum objects
    cart = Cart(int(0.5 * world_size),mass_of_cart,world_size)
    pendulum = Pendulum(1,1,mass_of_ball)

    # Initializing other variables needed for the simulation
    theta_dot = 0
    theta_tminus1 = theta_tminus2 = pendulum.theta
    x_tminus1 = x_tminus2 = cart.x
    previous_time_delta = 0
    # The simulation must run for the desired amount of time
    while time.time() <= end_time:		
        current_timestamp = time.time()
        time_delta = (current_timestamp - previous_timestamp)
        time_delta = 0.01
        if previous_time_delta != 0:
            theta_dot = (theta_tminus1 - theta_tminus2 ) / previous_time_delta				
            x_dot = (x_tminus1 - x_tminus2) / previous_time_delta

            F = get_actuator_output()
            apply_control_input(cart,pendulum,F,time_delta,x_tminus2,theta_dot,theta_tminus2,
                previous_time_delta,g)


        # Update the variables and display stuff
        set_sensor_input(pendulum.theta)
        display_stuff(world_size,cart,pendulum)
        previous_time_delta = time_delta
        previous_timestamp = current_timestamp
        theta_tminus2 = theta_tminus1 
        theta_tminus1 = pendulum.theta
        x_tminus2 = x_tminus1
        x_tminus1 = cart.x
        #time.sleep(0.01)

if __name__ == "__main__":
	main()