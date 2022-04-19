"""
┌──────────────────────────────────────────────────────────────────────┐
│ Plots the error to a target frequency for different given prescalers │
└──────────────────────────────────────────────────────────────────────┘

 Florian Dupeyron
 April 2022
"""


import numpy as np
from matplotlib import pyplot as plt

freq      = 10023.3948
clk       = 64e6

presc_min = int(clk/((1<<16)*freq))+1
presc     = np.arange(presc_min,(1<<16))

per       = clk/(presc*freq)
freq_got  = (clk/presc)/np.floor(per)

#err       = ((freq-freq_got)/freq)*100
err = freq_got-freq

plt.plot(presc, err)
plt.show()
