import matplotlib.pyplot as plt
import numpy as np


#plot
x = np.array([0, 2, 4, 10, 12])
y = np.array([0, 10, 5, 6, 7])

plt.plot(x, y)
plt.show()

#plot with dots and labels
x = np.array([0, 2, 4, 10, 12])
y = np.array([0, 10, 5, 6, 7])


plt.xlabel("This is y")
plt.ylabel("This is x")

plt.plot(x, y, "o")
plt.show()

#scatter 

x = np.array([5,7,8,7,2,17,2,9,4,11,12,9,6])
y = np.array([99,86,87,88,111,86,103,87,94,78,77,85,86])

plt.scatter(x, y)
plt.show()

# scatter but with 2 lines
x = np.array([5,7,8,7,2,17,2,9,4,11,12,9,6])
y = np.array([99,86,87,88,111,86,103,87,94,78,77,85,86])
plt.scatter(x, y)

x = np.array([2,2,8,1,15,8,12,9,7,3,11,4,7,14,12])
y = np.array([100,105,84,105,90,99,90,95,94,100,79,112,91,80,85])
plt.scatter(x, y)

plt.show()

#Bar 
x = np.array(["A", "B", "C", "D"])
y = np.array([3, 8, 1, 10])

plt.bar(x,y)
plt.show()

#Histogram - is showing avarage of put data
x = np.random.normal(170, 10, 250)
print(x[1:10])

plt.hist(x)
plt.show() 

#pie
y = np.array([35, 25, 25, 15])

plt.pie(y)
plt.show() 