import corner
import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

dn4.postprocess(rng_seed=0)
posterior_sample = dn4.my_loadtxt("posterior_sample.txt")

# Do corner plot
#colnames = dn4.load_column_names("posterior_sample.txt")["colnames"]
#corner.corner(posterior_sample, labels=colnames)
#plt.savefig("corner.png")
#plt.show()



# Corner plot for V2

# Fonts
plt.rcParams["font.family"] = "serif"
plt.rcParams["font.size"] = 16
plt.rc("text", usetex=True)
#plt.rcParams["axes.labelpad"] = 24



posterior_sample = posterior_sample[:, [0, 1, 2, 3, 6, 7, 8, 9, 10]]
fig = corner.corner(posterior_sample,
              labels=["$A_0$ (km/s)", "$A_1$ (km/s)", "$\\phi_0$ (deg)", "$\\phi_1$ (deg)",
                      "$\\sigma_0$ (km/s)", "$\\sigma_1$ (km/s)", "$\\gamma_0$",
                      "$\\gamma_1$", "$p_{\\rm subs}$"],
              plot_contours=False, plot_density=False,
              data_kwargs={"alpha": 0.15, "markersize": 0.75},
              label_kwargs=dict(fontsize=24))

# Extract the axes
axes = np.array(fig.axes).reshape((9, 9))

# Set limits manually for theta0 and theta1
for i in range(0, 2):
    axes[2, i].set_ylim([-180.0, 180.0])
for i in range(3, 9):
    axes[i, 2].set_xlim([-180.0, 180.0])
for i in range(0, 3):
    axes[3, i].set_ylim([-180.0, 180.0])
for i in range(4, 9):
    axes[i, 3].set_xlim([-180.0, 180.0])


#plt.tight_layout()

plt.savefig("corner.png", dpi=240)
plt.show()

