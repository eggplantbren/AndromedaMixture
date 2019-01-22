import corner
import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

dn4.postprocess()

# Do corner plot
posterior_sample = dn4.my_loadtxt("posterior_sample.txt")
colnames = dn4.load_column_names("posterior_sample.txt")["colnames"]
corner.corner(posterior_sample,
                labels=["A_nosubs", "A_subs",
                        "theta0_nosubs", "theta0_subs",
                        "sigma0", "gamma", "subs_thresh"])
plt.savefig("corner.png")
plt.show()

