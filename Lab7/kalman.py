import numpy as np

# n is the dimension of your state space 
Ad = np.eye(n) + Delta_T * A 
Bd = Delta_t * B

# We assume uncorrelated noise, and therefore a diagonal matrix works.
sig_u=np.array([[sigma_1**2,0],[0,sigma_2**2]]) 
sig_z=np.array([[sigma_3**2]])