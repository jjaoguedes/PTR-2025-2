import pandas as pd
import matplotlib.pyplot as plt

# Lê o arquivo
df = pd.read_csv("out/sim_out.tsv", sep="\t")

# Gráfico yx(t) e yy(t)
plt.figure()
plt.plot(df["t(s)"], df["yx(m)"], label="y_x(t)")
plt.plot(df["t(s)"], df["yy(m)"], label="y_y(t)")
plt.xlabel("Tempo (s)")
plt.ylabel("Coordenadas (m)")
plt.title("Evolução de y(t)")
plt.legend()
plt.grid(True)
plt.show()

# Gráfico da trajetória
plt.figure()
plt.plot(df["yx(m)"], df["yy(m)"], '-o', markersize=2)
plt.xlabel("x (m)")
plt.ylabel("y (m)")
plt.title("Trajetória do robô")
plt.axis("equal")
plt.grid(True)
plt.show()
