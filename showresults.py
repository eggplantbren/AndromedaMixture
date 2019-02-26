import corner
import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

dn4.postprocess(rng_seed=0)

# Do corner plot
posterior_sample = dn4.my_loadtxt("posterior_sample.txt")
colnames = dn4.load_column_names("posterior_sample.txt")["colnames"]
corner.corner(posterior_sample, labels=colnames)
plt.savefig("corner.png")
plt.show()




## Corner plot for V2
#posterior_sample = posterior_sample[:, [0, 1, 2, 3, 6, 7, 8, 9, 10]]
#corner.corner(posterior_sample,
#              labels=["$A_0$ (km/s)", "$A_1$ (km/s)", "$\\theta_0^0$ (deg)", "$\\theta_0^1$ (deg)",
#                      "$\\sigma_0^0$ (km/s)", "$\\sigma_0^1$ (km/s)", "$\\gamma_0$",
#                      "$\\gamma_1$", "$p_{\\rm subs}$"],
#              plot_contours=False, plot_density=False,
#                data_kwargs={"alpha": 0.15, "markersize": 0.75})
#plt.savefig("corner.png", dpi=240)
#plt.show()

