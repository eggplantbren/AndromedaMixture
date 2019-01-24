import dnest4.classic as dn4
import matplotlib.pyplot as plt
import numpy as np

x = np.linspace(-120.0, 120.0, 1001)
y = np.linspace(-120.0, 120.0, 1001)
x, y = np.meshgrid(x, y[::-1])
R = np.sqrt(x**2 + y**2)
theta = np.arctan2(y, x)

posterior_sample = dn4.my_loadtxt("posterior_sample.txt")
indices = dn4.load_column_names("posterior_sample.txt")["indices"]
plot = False

mu_v_tot0 = np.zeros(x.shape)
mu_v_tot1 = np.zeros(x.shape)


for k in range(500):# posterior_sample.shape[0]):

    if plot:
        plt.subplot(1, 2, 1)
    A = posterior_sample[k, indices["A[0]"]]
    theta0 = posterior_sample[k, indices["theta0[0]"]]*np.pi/180.0
    sth = np.sin(theta0)
    cth = np.cos(theta0)
    dist = x*sth - y*cth;
    mu_v = A*dist/np.abs(dist)
    mu_v_tot0 += mu_v

    if plot:
        plt.imshow(mu_v, cmap="coolwarm", extent=[-120, 120, -120, 120])


    if plot:
        plt.subplot(1, 2, 2)

    A = posterior_sample[k, indices["A[1]"]]
    theta0 = posterior_sample[k, indices["theta0[1]"]]
    sth = np.sin(theta0)
    cth = np.cos(theta0)
    dist = x*sth - y*cth;
    mu_v = A*dist/np.abs(dist)
    mu_v_tot1 += mu_v

    if plot:
        plt.imshow(mu_v, cmap="coolwarm", extent=[-120, 120, -120, 120])
        plt.show()

    print(k+1)

plt.subplot(1, 2, 1)
plt.imshow(mu_v_tot0/posterior_sample.shape[0],
                cmap="coolwarm", extent=[-120, 120, -120, 120])
plt.subplot(1, 2, 2)
plt.imshow(mu_v_tot1/posterior_sample.shape[0],
                cmap="coolwarm", extent=[-120, 120, -120, 120])
plt.show()

