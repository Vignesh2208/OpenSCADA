import numpy as np,cv2,math,time,matplotlib.pyplot as plt,sys
import grpc
from contrib.physical_system_sim import PhysicalSystemSim
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





class SinglePendulumSimulator(object):

    def __init__(self, controlling_plc_cpu):
        self.mass_of_ball = 1.0
        self.mass_of_cart = 5.0
        self.g = 9.81
        self.errors, self.force, self.theta, self.times, self.x = [],[],[],[],[]
        self.world_size = 1000
        self.total_time_elapsed = 0
        
        # Initializing cart and pendulum objects
        self.cart = Cart(int(0.5 * self.world_size),
            self.mass_of_cart, self.world_size)
        self.pendulum = Pendulum(1,1, self.mass_of_ball)

        # Initializing other variables needed for the simulation
        self.theta_dot = 0
        self.theta_tminus1 = self.theta_tminus2 = self.pendulum.theta
        self.x_tminus1 = self.x_tminus2 = self.cart.x
        self.previous_time_delta = 0
	self.cpu_id = controlling_plc_cpu
        height = self.world_size
        width = self.world_size
        layers = 3
        size = (width,height)
        #self.out = cv2.VideoWriter('/tmp/simulation-%s.mp4' %(str(self.cpu_id)),cv2.VideoWriter_fourcc(*'MP4V'), 15, size)
        self.im_array = []
        self.im_count = 1
        self.pendulum_angles = []
    def display_stuff(self):
        # This function displays the pendulum and cart.
        length_for_display = self.pendulum.length * 100
        A = np.zeros((self.world_size, self.world_size,3), np.uint8)
        cv2.line(A,(0,int(0.6 * self.world_size)),(self.world_size, \
            int(0.6 * self.world_size)), (255,255,255), 2)
        cv2.rectangle(A,(int(self.cart.x) + 25, self.cart.y + 15), \
            (int(self.cart.x) - 25, self.cart.y - 15), self.cart.color, -1)	
        pendulum_x_endpoint = int(self.cart.x - (length_for_display) * \
            math.sin(self.pendulum.theta))
        pendulum_y_endpoint = int(self.cart.y - (length_for_display) * \
            math.cos(self.pendulum.theta))
        cv2.line(A,(int(self.cart.x), self.cart.y), \
            (pendulum_x_endpoint,pendulum_y_endpoint), self.pendulum.color, 4)
        cv2.circle(A, (pendulum_x_endpoint, pendulum_y_endpoint), 6,\
            (255,255,255),-1)
        cv2.imshow(self.cpu_id,A)
        cv2.waitKey(1)
        #if self.im_count < 5000:
        #    self.im_array.append(A)
        self.im_count += 1


    def apply_control_input(self, F, time_delta):
        # Finding x and theta on considering the control inputs and the dynamics of the system
        theta_double_dot = (((self.cart.mass + self.pendulum.ball_mass) * self.g\
             * math.sin(self.pendulum.theta)) + (F * math.cos(self.pendulum.theta)) \
            - (self.pendulum.ball_mass * ((self.theta_dot)**2.0) * self.pendulum.length * \
            math.sin(self.pendulum.theta) * math.cos(self.pendulum.theta))) / \
            (self.pendulum.length * (self.cart.mass + (self.pendulum.ball_mass * \
            (math.sin(self.pendulum.theta)**2.0)))) 

        x_double_dot = ((self.pendulum.ball_mass * self.g * math.sin(self.pendulum.theta) * \
            math.cos(self.pendulum.theta)) - (self.pendulum.ball_mass * self.pendulum.length * \
            math.sin(self.pendulum.theta) * (self.theta_dot**2)) + (F)) / (self.cart.mass + \
            (self.pendulum.ball_mass * (math.sin(self.pendulum.theta)**2)))

            
        self.cart.x += ((time_delta**2) * x_double_dot) + (((self.cart.x - self.x_tminus2) * time_delta) / \
            self.previous_time_delta)

        self.pendulum.theta += ((time_delta**2)*theta_double_dot) + (((self.pendulum.theta - \
            self.theta_tminus2)*time_delta) / self.previous_time_delta)
        
    
    # Implementation of abstract class method
    def progress(self,time_step_size):
        if self.previous_time_delta != 0:
            self.theta_dot = (self.theta_tminus1 
                - self.theta_tminus2 ) / self.previous_time_delta				
            self.x_dot = (self.x_tminus1 - self.x_tminus2) / self.previous_time_delta

            F = self.get_actuator_output(self.cpu_id)
            self.apply_control_input(F, time_step_size)


        # Update the variables and display stuff
        self.set_sensor_input(self.pendulum.theta, self.cpu_id)
        if self.total_time_elapsed <= 5.0:
            self.pendulum_angles.append(self.pendulum.theta)
        self.total_time_elapsed += time_step_size
        
        self.previous_time_delta = time_step_size
        self.theta_tminus2 = self.theta_tminus1 
        self.theta_tminus1 = self.pendulum.theta
        self.x_tminus2 = self.x_tminus1
        self.x_tminus1 = self.cart.x


    def display(self):
        try:
            self.display_stuff()
        except:
            pass

    def set_sensor_input(self, curr_theta, cpu_id):
        try:
            with grpc.insecure_channel('localhost:50051') as channel:
                stub = mem_access_grpc.AccessServiceStub(channel)
                response = stub.SetSensorInput(
                    mem_access_proto.SensorInput(
                        PLC_ID='Pendulum_PLC',
                        ResourceName=cpu_id,
                        MemType=0,
                        ByteOffset=1,
                        BitOffset=0,
                        VariableDataTypeName="REAL",
                        ValueToSet = str(curr_theta)))
        except Exception as e:
            pass

    def get_actuator_output(self, cpu_id):
        try:
            with grpc.insecure_channel('localhost:50051') as channel:
                stub = mem_access_grpc.AccessServiceStub(channel)
                response = stub.GetActuatorOutput(
                    mem_access_proto.ActuatorOutput(
                        PLC_ID='Pendulum_PLC',
                        ResourceName=cpu_id,
                        MemType=1,
                        ByteOffset=1,
                        BitOffset=0,
                        VariableDataTypeName="REAL"))

                if response.status == "SUCCESS":
                    return float(response.value)
                return 0.0
        except Exception as e:
            return 0.0

class PendulumSystemSimulator(PhysicalSystemSim):
     def __init__(self):
         self.pendulums = []
         self.pendulums.append(SinglePendulumSimulator("CPU_001"))
         self.pendulums.append(SinglePendulumSimulator("CPU_002"))
     # Implementation of abstract class method
     def progress(self,time_step_size):
          self.pendulums[0].progress(time_step_size)
          self.pendulums[1].progress(time_step_size)

     def display(self):
          self.pendulums[0].display()
          self.pendulums[1].display()

     def finish_video(self):
          pass
          #for im in self.pendulums[0].im_array:
          #   self.pendulums[0].out.write(im)
          #self.pendulums[0].out.release()

          #for im in self.pendulums[1].im_array:
          #   self.pendulums[1].out.write(im)
          #self.pendulums[1].out.release()
          with open('/tmp/pendulum-1.txt', 'w') as f:
              for angle in self.pendulums[0].pendulum_angles:
                  f.write(str(angle) + '\n')
          with open('/tmp/pendulum-2.txt', 'w') as f:
              for angle in self.pendulums[1].pendulum_angles:
                  f.write(str(angle) + '\n')

