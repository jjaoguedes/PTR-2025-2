
import csv, statistics as st
from collections import defaultdict

def load(path):
    by_task = defaultdict(lambda: {"comp": [], "jit": []})
    with open(path, "r") as f:
        rd = csv.DictReader(f)
        for r in rd:
            by_task[r["task"]]["comp"].append(float(r["comp_ms"]))
            by_task[r["task"]]["jit"].append(float(r["jitter_ms"]))
    return by_task

def stats(vec):
    if not vec: return (0,0,0,0,0)
    m = st.mean(vec)
    var = st.pvariance(vec)
    sd = var**0.5
    return (m, var, sd, min(vec), max(vec))

def merge(nl, wl):
    tasks = sorted(set(list(nl.keys()) + list(wl.keys())))
    rows = []
    for t in tasks:
        a = stats(nl[t]["comp"]); b = stats(nl[t]["jit"])
        c = stats(wl[t]["comp"]); d = stats(wl[t]["jit"])
        rows.append([t,
            *[f"{x:.3f}" for x in a], *[f"{x:.3f}" for x in b],
            *[f"{x:.3f}" for x in c], *[f"{x:.3f}" for x in d]
        ])
    return rows

no_load = load("out/perf_noload.csv")
with_load = load("out/perf_load.csv")

rows = merge(no_load, with_load)

header = ["task",
          "comp_mean_nl","comp_var_nl","comp_sd_nl","comp_min_nl","comp_max_nl",
          "jit_mean_nl","jit_var_nl","jit_sd_nl","jit_min_nl","jit_max_nl",
          "comp_mean_wl","comp_var_wl","comp_sd_wl","comp_min_wl","comp_max_wl",
          "jit_mean_wl","jit_var_wl","jit_sd_wl","jit_min_wl","jit_max_wl"]

with open("out/stats.csv", "w", newline="") as f:
    wr = csv.writer(f)
    wr.writerow(header)
    wr.writerows(rows)

print("Tabela salva em out/stats.csv")
