
import csv
import matplotlib.pyplot as plt

t, y1, y2, xr, yr = [], [], [], [], []
with open("out/traj.csv", "r") as f:
    rd = csv.DictReader(f)
    for r in rd:
        t.append(float(r["t"]))
        y1.append(float(r["y1"]))
        y2.append(float(r["y2"]))
        xr.append(float(r["xref"]))
        yr.append(float(r["yref"]))

plt.figure()
plt.plot(t, y1, label="y1 (x)")
plt.plot(t, y2, label="y2 (y)")
plt.plot(t, xr, label="x_ref", linestyle="--")
plt.plot(t, yr, label="y_ref", linestyle="--")
plt.xlabel("Tempo (s)"); plt.ylabel("Posição (m)")
plt.title("Saídas y(k) e referências")
plt.legend(); plt.grid(True)
plt.show()
