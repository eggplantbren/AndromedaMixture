"""
Extract the relevant parts from the data file.
"""

f = open("table_final.dat")

# Empty lists
R = []
theta = []
v = []
sig_v = []
classification = []

# Read all lines from the file
k = 0 # Line number
while True:
    line = f.readline()
    if len(line) == 0:
        break

    # If the line contains no hash
    if "#" not in line:

        # Split values
        entries = line.split()
        R.append(float(entries[3]))
        theta.append(float(entries[4]))
        v.append(float(entries[15]))
        sig_v.append(float(entries[16]))

        if entries[18] == "Substructure":
            classification.append(1)
        elif entries[18] == "Non-substructure":
            classification.append(0)
        else:
            classification.append(2)

        print(entries[3], entries[4], entries[15], entries[16], entries[18])

    k += 1

f.close()

# Convert to arrays
import numpy as np
R = np.array(R)
theta = np.array(theta)
v = np.array(v)
sig_v = np.array(sig_v)
classification = np.array(classification)

# Angle to radians and rotate 90 degrees
theta = (theta+ 90.0)*np.pi/180.0

# Save to text file
data = np.empty((len(R), 5))
data[:,0] = R*np.cos(theta)
data[:,1] = R*np.sin(theta)
data[:,2] = v
data[:,3] = sig_v
data[:,4] = classification
np.savetxt("data.txt", data)

# Plot some stuff
import matplotlib.pyplot as plt

away = data[:,2] > 0.0
towards = data[:,2] <= 0.0
plt.scatter(data[away,0], data[away,1], marker="o", color="r", alpha=0.5,
                s=np.abs(data[away, 2]))
plt.scatter(data[towards,0], data[towards,1], marker="o", color="b", alpha=0.5,
                s=np.abs(data[towards, 2]))
plt.show()

