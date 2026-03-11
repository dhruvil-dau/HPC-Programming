import pandas as pd
import matplotlib.pyplot as plt

data=pd.read_csv("exp2_labpc.csv")

plt.bar(data["Problem_Index"],
        data["Interpolation_Time(sec)"])

plt.xlabel("Problem Index")
plt.ylabel("Interpolation Time")
plt.title("Experiment-02 Consistency")

plt.savefig("exp2_labpc_plot.png")
plt.show()
