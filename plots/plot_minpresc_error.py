"""
┌────────────────────────────────────────────────────────────────────────┐
│ Plot the error to a target frequency when using the minimum prescaler. │
└────────────────────────────────────────────────────────────────────────┘

 Florian Dupeyron
 April 2022
"""


import numpy                    as np
from   matplotlib import pyplot as plt

clk         = 64e6
#freqs       = np.arange(500,100000,0.1)
freqs       = np.arange(500,100000,1)
presc_mins  = np.floor(clk/((1<<16)*freqs))+1
periods     = np.floor(clk/(presc_mins*freqs))

freqs_final = clk/(presc_mins*periods)

err_abs     = (freqs_final-freqs)
err         = (err_abs/freqs)*100

fig = plt.figure()
ax  = fig.add_subplot(1,1,1)
ax.plot(freqs, err)
ax.grid()
ax.set_xlabel("Frequency [Hz]")
ax.set_ylabel("Error [%]")

fig2 = plt.figure()
ax   = fig2.add_subplot(1,1,1)
ax.loglog(freqs, err_abs)
ax.set_xlabel("Frequency [Hz]")
ax.set_ylabel("Error [Hz]")
ax.grid(which="both")
plt.show()
