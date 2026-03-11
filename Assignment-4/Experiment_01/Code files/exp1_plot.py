import pandas as pd
import matplotlib.pyplot as plt
import numpy as np

files = [
    "exp1_problem1_labpc.csv",
    "exp1_problem2_labpc.csv",
    "exp1_problem3_labpc.csv"
]

labels = ["NX=250,NY=100","NX=500,NY=200","NX=1000,NY=400"]

for i in range(3):
    data = pd.read_csv(files[i])
    plt.plot(np.log10(data["Particles"]),
             np.log10(data["Interpolation_Time(sec)"]),
             marker='o', label=labels[i])

plt.xlabel("log10(Number of Particles)")
plt.ylabel("log10(Interpolation Time)")
plt.title("Experiment-01 Scaling")
plt.legend()
plt.savefig("exp1_labpc_plot.png")
plt.show()
