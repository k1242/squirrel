import numpy as np
import os

dtype = np.float32

os.makedirs("data/weights_rnd", exist_ok=True)

np.save("data/weights_rnd/B0-768-256-w.npy", (np.random.rand(256, 768) - 0.5).astype(dtype).round(2).T) # should be transposed
np.save("data/weights_rnd/B0-256-a.npy", (np.random.rand(256) - 0.5).astype(dtype).round(2))

np.save("data/weights_rnd/B1p-32-256-w.npy", (np.random.rand(32, 256) - 0.5).astype(dtype).round(2))
np.save("data/weights_rnd/B1p-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))

np.save("data/weights_rnd/B1o-32-256-w.npy", (np.random.rand(32, 256) - 0.5).astype(dtype).round(2))
np.save("data/weights_rnd/B1o-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))

np.save("data/weights_rnd/B2-32-64-w.npy", (np.random.rand(32, 64) - 0.5).astype(dtype).round(2))
np.save("data/weights_rnd/B2-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))


np.save("data/weights_rnd/C1-1-32-w.npy", (np.random.rand(1, 32) - 0.5).astype(dtype).round(2))
np.save("data/weights_rnd/C1-1-a.npy", (np.random.rand(1) - 0.5).astype(dtype).round(2))


np.save("data/weights_rnd/A0-768-32-w.npy", (np.random.rand(32, 768) - 0.5).astype(dtype).round(2).T)

np.save("data/weights_rnd/A1-32-32-w.npy", (np.random.rand(32, 32) - 0.5).astype(dtype).round(2))
np.save("data/weights_rnd/A1-32-a.npy", (np.random.rand(32) - 0.5).astype(dtype).round(2))

np.save("data/weights_rnd/A2-1-32-w.npy", (np.random.rand(1, 32) - 0.5).astype(dtype).round(2))
np.save("data/weights_rnd/A2-1-a.npy", (np.random.rand(1) - 0.5).astype(dtype).round(2))