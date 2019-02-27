import dnest4.classic as dn4
import numpy as np

# Load samples
sample = dn4.my_loadtxt("sample.txt")
sample_info = dn4.my_loadtxt("sample_info.txt")

# Log likelihood
logL = sample_info[:,1]

# Best particle
which = np.nonzero(logL == logL.max())[0]
best = sample[which, :]

# Get header
f = open("sample.txt")
header = f.readline()
f.close()
np.savetxt("best_fit.txt", best, header=header[1:])
print("Saved to best_fit.txt")

