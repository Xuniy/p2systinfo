import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

data=pd.read_csv('measures.csv', names=["Run 1", "Run 2", "Run 3", "Run 4", "Run 5"])
data=data.dropna(axis=1)
data=data.T

data.columns=['Thread 1', 'Thread 2', 'Thread 4', 'Thread 8', 'Thread 16']
mean=data.mean()

std=data.std()

thread=[2**i for i in range(5)]

plt.plot(thread, mean, label='Mean', color='blue')
plt.errorbar(thread, mean, yerr=std, fmt='o', color='blue', capsize=5, label='Standard Deviation')
plt.xlabel('Number of Threads')
plt.ylabel('Compilation time in sec')
plt.title('Compilation time vs Number of Threads')
plt.xscale('log', base=2)
plt.xticks(thread, thread)

plt.legend()
plt.savefig("perf.pdf")
plt.show()