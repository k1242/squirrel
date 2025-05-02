import numpy as np

dtype = np.float32

np.save("data/B0-768-256-w.npy", (np.random.rand(256, 768) - 0.5).astype(dtype).round(2).T) # should be transposed
np.save("data/B0-256-a.npy", (np.random.rand(256) - 0.5).astype(dtype).round(2))

np.save("data/B1p-32-256-w.npy", (np.random.rand(32, 256) - 0.5).astype(dtype).round(2))
np.save("data/B1p-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))

np.save("data/B1o-32-256-w.npy", (np.random.rand(32, 256) - 0.5).astype(dtype).round(2))
np.save("data/B1o-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))

np.save("data/B2-32-64-w.npy", (np.random.rand(32, 64) - 0.5).astype(dtype).round(2))
np.save("data/B2-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))


np.save("data/C1-1-32-w.npy", (np.random.rand(1, 32) - 0.5).astype(dtype).round(2))
np.save("data/C1-1-a.npy", (np.random.rand(1) - 0.5).astype(dtype).round(2))


np.save("data/A0-768-32-w.npy", (np.random.rand(32, 768) - 0.5).astype(dtype).round(2).T)

np.save("data/A1-32-32-w.npy", (np.random.rand(32, 32) - 0.5).astype(dtype).round(2))
np.save("data/A1-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))

np.save("data/A2-1-32-w.npy", (np.random.rand(1, 32) - 0.5).astype(dtype).round(2))
np.save("data/A2-1-a.npy", (np.random.rand(1) - 0.5).astype(dtype).round(2))