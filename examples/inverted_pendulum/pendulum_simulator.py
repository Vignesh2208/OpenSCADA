# Description:  This programs runs a simulation of a cart-pole system
#		The conrol algorithm used to balance the pendulum is PID
 
import numpy as np,cv2,math,time,matplotlib.pyplot as plt,sys
#from control.matlab import *

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

def find_error(pendulum):
	# There's a seperate function for this because of the wrap-around problem
	# This function returns the error
	previous_error = (pendulum.theta % (2 * math.pi)) - 0
	if previous_error > math.pi:
		previous_error = previous_error - (2 * math.pi)
	return previous_error


def main():
	# Initializing mass values, g, world size, simulation time and variables required to terminate the simulation
	mass_of_ball = 1.0
	mass_of_cart = 5.0
	g = 9.81
	errors, force, theta, times, x = [],[],[],[],[]
	world_size = 1000
	simulation_time = 35
	previous_timestamp = time.time()
	end_time = previous_timestamp + simulation_time

	# Initializing cart and pendulum objects
	cart = Cart(int(0.2 * world_size),mass_of_cart,world_size)
	pendulum = Pendulum(1,1,mass_of_ball)

	# Initializing other variables needed for the simulation
	theta_dot = 0
	theta_tminus1 = theta_tminus2 = pendulum.theta
	x_tminus1 = x_tminus2 = cart.x
	previous_error = find_error(pendulum)
	integral = 0
	previous_time_delta = 0
	
	# The simulation must run for the desired amount of time
	while time.time() <= end_time:		
		current_timestamp = time.time()
		time_delta = (current_timestamp - previous_timestamp)
		error = find_error(pendulum)
		if previous_time_delta != 0:	# This condition is to make sure that theta_dot is not infinity in the first step
			theta_dot = (theta_tminus1 - theta_tminus2 ) / previous_time_delta				
			x_dot = (x_tminus1 - x_tminus2) / previous_time_delta
			

			# Inputs needed for PLC: (1) pendulum's current theta, (2) current decision time (3) prev decision time
			# (4) Previously computed integral (5) pendulum's prev theta
			# Outputs from PLC: (1) Force F
			# Side effects: It stores current theta, current decision time, current computed integral
			# and uses it for computing next control input
			F,intergral = find_pid_control_input(time_delta,error,previous_error,integral,g)
			apply_control_input(cart,pendulum,F,time_delta,x_tminus2,theta_dot,theta_tminus2,previous_time_delta,g)
			
	
		# Update the variables and display stuff
		display_stuff(world_size,cart,pendulum)
		previous_time_delta = time_delta # This can be set here as well as in the PLC
		previous_timestamp = current_timestamp # This can be set here as well as in the PLC
		previous_error = error	# This doesn't need to be here. It needs to be tracked only in the PLC
		theta_tminus2 = theta_tminus1 # This can be set here only
		theta_tminus1 = pendulum.theta	# This can be set here only
		x_tminus2 = x_tminus1	# This can be set here only
		x_tminus1 = cart.x	# This can be set here only

if __name__ == "__main__":
	main()