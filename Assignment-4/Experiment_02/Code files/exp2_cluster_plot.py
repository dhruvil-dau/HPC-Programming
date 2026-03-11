import pandas as pd
import matplotlib.pyplot as plt

data = pd.read_csv("exp2_cluster.csv")

plt.bar(data["Problem_Index"],
        data["Interpolation_Time(sec)"])

plt.xlabel("Problem Index")
plt.ylabel("Interpolation Time (sec)")
plt.title("Experiment-02 Cluster")

plt.savefig("exp2_cluster_plot.png")
plt.show()
