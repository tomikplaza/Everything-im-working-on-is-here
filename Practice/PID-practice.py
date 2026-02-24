import time
import matplotlib.pyplot as plt
import numpy as np

x = 0   #Used for graph later

target = 100    #End destination  
position = 0    #current position   

kp = 0.5    #P - applies power based on error

ki = 0.01   #I - the longer p doesn't do anything the more power it gives to prevent wind disturbion and being stuck          
integral = 0    #integral - add up and sums until it's enough to push the drone faster (if the error is higher than target error is - so it pushes the drone back)

kd = 2.5    #D - if you're going too fast it pushes you back to not overshoot it


print(f"Target: {target}")
print("----------------------------")

last_error = 0
# The Control Loop
while x != 200: #run until drone moved 200x times
    error = target - position      #error - distance between current position and target position and if it's higher than target it changes to -error (negative)
    x += 1 


    p_term = kp * error
    
    
    integral = integral + error
    i_term = ki * integral
    
    derivative = error - last_error     #derivative - calculate the speed from position last time (how much it changed)
    d_term = kd * derivative

   
    output = p_term + i_term + d_term   #output - end result to give to how much move
    
    
    position = position + (output * 0.1)    #update the position

    last_error = error
    
    print(f"position: {position:.2f} | Error: {error:.2f} | P: {p_term:.2f} | I: {i_term:.2f} | D: {d_term:.2f}")
    
    y = np.array([position])

    time.sleep(0.1) #break to simulate time [time.sleep(1) = 1s]
    plt.plot(x, position, "o")
    plt.pause(0.01) #update the graph and show it
        
plt.show() #show the end result