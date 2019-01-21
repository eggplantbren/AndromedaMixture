"""
Extract the relevant parts from the data file.
"""

f = open("table_final2.dat")

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
        R.append(float(entries[4]))
        theta.append(float(entries[5]))
        v.append(float(entries[15]))
        sig_v.append(float(entries[16]))
        classification.append(str(entries[18]))

        print(entries[4], entries[5], entries[15], entries[16], entries[18])

    k += 1

f.close()

# Plot some stuff
import matplotlib.pyplot as plt

plt.plot(R, theta, "o")
plt.show()

