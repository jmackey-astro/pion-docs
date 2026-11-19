#!/usr/bin/env python3
"""Plot the 2D NEMO example with two stars: the hydrogen number density
in the upper half-plane (R > 0), and the fraction of oxygen in O^{2+}
reflected into the lower half-plane (R < 0).  Each snapshot is plotted in
its own panel, one above the other.

Usage:
  python3 plot_twosrc.py params_TwoSrc_NEMO_n128.txt snap1.silo [snap2.silo ...] [out.png]

The parameter file is used to find the names of the tracers and the
positions of the stars.
"""
import sys
import numpy as np
import matplotlib.pyplot as plt
from pypion.ReadData import ReadData

pfile = sys.argv[1]
snapshots = [a for a in sys.argv[2:] if a.endswith(".silo")]
outfiles = [a for a in sys.argv[2:] if a.endswith(".png")]
outfile = outfiles[0] if outfiles else "twosrc_nemo.png"
pc = 3.086e18  # cm
m_p = 1.6726e-24  # g
Rplot = 3.0  # pc, outer radius of the plot
lognH_range = (2.1, 3.7)  # colour scale of log10(n_H)

# Tracer names and star positions from the parameter file.  In the Silo
# file, tracer n with name e.g. "O2+" is the variable "Tr008_O2p".
silo_name = {}
stars = []
for line in open(pfile):
    w = line.split()
    if len(w) < 2:
        continue
    if w[0].startswith("Tracer") and w[0][6:].isdigit():
        n = int(w[0][6:])
        silo_name[w[1]] = "Tr{:03d}_{}".format(n, w[1].replace("+", "p"))
    if w[0].startswith("RT_position_") and w[0].endswith("_0"):
        stars.append(float(w[1]) / pc)

fig, axes = plt.subplots(len(snapshots), 1, squeeze=False,
                         figsize=(7, 0.5 + 3.4 * len(snapshots)))
axes = axes[:, 0]

for ax, snapshot in zip(axes, snapshots):
    data = ReadData([snapshot])

    def get(var):
        """A variable of the snapshot as a 2D numpy array [R, z]."""
        return np.array(data.get_2Darray(silo_name.get(var, var))["data"][0])

    d = data.get_2Darray("Density")
    rho = np.array(d["data"][0])
    zmin, rmin = np.array(d["min_extents"][0][:2]) / pc
    zmax, rmax = np.array(d["max_extents"][0][:2]) / pc
    t = d["sim_time"]
    t = getattr(t, "value", t) / 3.156e13  # Myr

    n_H = get("X_H") * rho / m_p
    o2 = get("O2+") / get("X_O")  # fraction of oxygen in O^{2+}

    im1 = ax.imshow(np.log10(n_H), origin="lower", cmap="viridis",
                    vmin=lognH_range[0], vmax=lognH_range[1],
                    extent=[zmin, zmax, rmin, rmax])
    # Reflect about the symmetry axis: row 0 (R=0) goes to the top.
    im2 = ax.imshow(o2[::-1, :], origin="lower", cmap="magma", vmin=0,
                    vmax=1, extent=[zmin, zmax, -rmax, -rmin])
    ax.plot(stars, np.zeros(len(stars)), "w*", ms=12, mec="k")
    ax.axhline(0.0, color="w", lw=0.5)
    ax.set_xlim(zmin, zmax)
    ax.set_ylim(-Rplot, Rplot)
    ax.set_ylabel("R (pc)")
    ax.text(0.02, 0.96, "t = {:.2f} Myr".format(t), transform=ax.transAxes,
            color="w", va="top")
    # One colour bar beside each half of the plot.
    c1 = fig.colorbar(im1, cax=ax.inset_axes([1.03, 0.52, 0.04, 0.48]))
    c1.set_label(r"$\log_{10}\,n_\mathrm{H}$ (cm$^{-3}$)")
    c2 = fig.colorbar(im2, cax=ax.inset_axes([1.03, 0.0, 0.04, 0.48]))
    c2.set_label(r"O$^{2+}$ / O")

axes[-1].set_xlabel("z (pc)")
fig.subplots_adjust(left=0.1, right=0.82, bottom=0.08, top=0.98,
                    hspace=0.12)
fig.savefig(outfile, dpi=120)
print("wrote", outfile)
