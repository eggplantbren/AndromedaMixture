import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(-120.0, 120.0, 250)
y = np.linspace(-120.0, 120.0, 250)
x, y = np.meshgrid(x, y[::-1])
R = np.sqrt(x**2 + y**2)
theta = np.arctan2(y, x)

posterior_sample = dn4.my_loadtxt("posterior_sample.txt")
indices = dn4.load_column_names("posterior_sample.txt")["indices"]
plot = True

for k in range(posterior_sample.shape[0]):

    A = posterior_sample[k, indices["A[0]"]]
    theta0 = posterior_sample[k, indices["theta0[0]"]]*np.pi/180.0
    sth = np.sin(theta0)
    cth = np.cos(theta0)
    dist = x*sth - y*cth;
    mu_v0 = A*dist

    A = posterior_sample[k, indices["A[1]"]]
    theta0 = posterior_sample[k, indices["theta0[1]"]]
    sth = np.sin(theta0)
    cth = np.cos(theta0)
    dist = x*sth - y*cth;
    mu_v1 = A*dist

    if plot:
        top = np.max([mu_v0, mu_v1])
        bottom = np.min([mu_v0, mu_v1])

        plt.subplot(1, 2, 1)
        plt.imshow(mu_v0, cmap="coolwarm", extent=[-120, 120, -120, 120],
                        vmin=bottom, vmax=top)

        plt.subplot(1, 2, 2)
        plt.imshow(mu_v1, cmap="coolwarm", extent=[-120, 120, -120, 120],
                        vmin=bottom, vmax=top)
        plt.show()

    print(k+1)

