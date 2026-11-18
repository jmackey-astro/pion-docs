#!/usr/bin/env python3
"""Plot the temperature and ion fractions of the 1D NEMO HII-region example.

Usage:
  python3 plot_hii40.py params_HII40_NEMO_n128.txt HII40_n128_0000.<step>.silo

The parameter file is used to find the names of the tracers.
"""
import sys
import numpy as np
import matplotlib.pyplot as plt
from pypion.ReadData import ReadData

pfile, snapshot = sys.argv[1], sys.argv[2]
outfile = sys.argv[3] if len(sys.argv) > 3 else "hii40_nemo.png"

# Tracer names from the parameter file.  In the Silo file, tracer n with
# name e.g. "He1+" is the variable "Tr011_He1p" ('+' is written as 'p').
silo_name = {}
for line in open(pfile):
    w = line.split()
    if len(w) >= 2 and w[0].startswith("Tracer") and w[0][6:].isdigit():
        n = int(w[0][6:])
        silo_name[w[1]] = "Tr{:03d}_{}".format(n, w[1].replace("+", "p"))

data = ReadData([snapshot])


def get(var):
    """A variable of the snapshot as a 1D numpy array."""
    return np.array(data.get_1Darray(silo_name.get(var, var))["data"][0])


d = data.get_1Darray("Density")
ncell = len(d["data"][0])
xmin, xmax = d["min_extents"][0][0], d["max_extents"][0][0]
r = (xmin + (np.arange(ncell) + 0.5) * (xmax - xmin) / ncell) / 3.086e18  # pc
time_myr = d["sim_time"].value / 3.156e13 if hasattr(d["sim_time"], "value") \
    else d["sim_time"] / 3.156e13


def ion_fractions(element, nion):
    """Fractions of the ionisation stages 0 (neutral) to nion-1 of an element.

    The tracers are the neutral atom and the ions up to the second-highest
    stage; the fully ionised stage is the element minus all of these.
    """
    x_el = get("X_" + element)
    names = [element] + ["{}{}+".format(element, i) for i in range(1, 30)]
    names = [n for n in names if n in silo_name]
    stages = [get(n) / x_el for n in names]
    stages.append(1.0 - np.sum(stages, axis=0))
    return stages[:nion]


ROMAN = ["I", "II", "III", "IV", "V", "VI", "VII", "VIII", "IX"]

fig, ax = plt.subplots(3, 1, figsize=(6, 9), sharex=True)

ax[0].plot(r, get("Temperature"), "k-")
ax[0].set_ylabel("Temperature (K)")
ax[0].set_title("NEMO HII region, t = {:.2f} Myr".format(time_myr))

for el, n, style in (("H", 2, "-"), ("He", 3, "--")):
    for i, f in enumerate(ion_fractions(el, n)):
        ax[1].plot(r, f, style, label="{} {}".format(el, ROMAN[i]))
ax[1].set_ylabel("Ion fraction")
ax[1].legend(fontsize=8, ncol=2, loc="center left")

for el, n, style in (("O", 4, "-"), ("N", 4, "--")):
    for i, f in enumerate(ion_fractions(el, n)):
        ax[2].plot(r, f, style, label="{} {}".format(el, ROMAN[i]))
ax[2].set_ylabel("Ion fraction")
ax[2].set_xlabel("Radius (pc)")
ax[2].legend(fontsize=8, ncol=2, loc="center left")

for a in ax[1:]:
    a.set_ylim(-0.02, 1.02)
fig.tight_layout()
fig.savefig(outfile, dpi=120)
print("wrote", outfile)
