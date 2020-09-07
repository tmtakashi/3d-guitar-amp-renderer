from scipy.special import sph_harm
import numpy as np

order = 3
sph_mat = np.zeros((3, (order+1)**2), dtype=np.complex128)
positions = np.array([
    [np.pi/4, np.pi/3, 1.0],
    [3 * np.pi/2, np.pi/6, 1.0],
    [np.pi/3, np.pi/4, 1.0],
])

for n in range(order + 1):
    for m in range(n + 1):
        sph_mat[:, n**2+n-m] = sph_harm(-m, n, positions[:, 0], positions[:, 1])
        sph_mat[:, n**2+n+m] = sph_harm(m, n, positions[:, 0], positions[:, 1])

for val in sph_mat.ravel():
    print(round(val.real, 5), round(val.imag, 5), end="")

