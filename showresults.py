import corner
import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

dn4.postprocess()

# Do corner plot
posterior_sample = dn4.my_loadtxt("posterior_sample.txt")

# Convert angle to degrees and change conventions
phi0 = posterior_sample[:,1]*180.0/np.pi
phi0[phi0 > 180.0] += -360.0
posterior_sample[:,1] = phi0
corner.corner(posterior_sample,
                labels=["$A$", "$\\phi_0$", "$\\sigma_0$", "$\\gamma$"])
plt.show()

