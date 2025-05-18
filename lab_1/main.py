import matplotlib.pyplot as plt

x = [100, 1000, 10000, 100000, 1000000]
y = [0, 3, 35, 381, 3771]

fig, ax = plt.subplots()
ax.plot(x, y)
plt.show()