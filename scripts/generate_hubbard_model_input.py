#!/usr/bin/env python

import h5py
import numpy as np

t = np.array([[ 0.0, -1.0,  0.0, -1.0],
              [-1.0,  0.0, -1.0,  0.0],
              [ 0.0, -1.0,  0.0, -1.0],
              [-1.0,  0.0, -1.0,  0.0]])

U = np.array([2.0,2.0,2.0,2.0])
xmu = np.array([1.0,1.0,1.0,1.0])

Ns = len(U)
sectors = np.array([[Ns/2,Ns/2],])

data = h5py.File("input.h5", "w");

data["BETA"] = 10.0
data["magnetic_field"] = 0.0

hop_g = data.create_group("sectors")
hop_g.create_dataset("values", data=sectors)


hop_g = data.create_group("hopping")
hop_g.create_dataset("values", data=t)

int_g = data.create_group("interaction")
int_ds = int_g.create_dataset("values", shape=(Ns,), data=U)

int_g = data.create_group("chemical_potential")
int_ds = int_g.create_dataset("values", shape=(Ns,), data=xmu)

