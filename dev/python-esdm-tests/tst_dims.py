import math
import subprocess
import sys
import unittest
import os
import tempfile
import warnings
import re

import numpy as NP
from collections import OrderedDict
from numpy.random.mtrand import uniform

import netCDF4

# test attribute creation.
FILE_NAME = "esdm://testfile_dims"
VAR_NAME="dummy_var"
DIM1_NAME="x"
DIM1_LEN=2
DIM2_NAME="y"
DIM2_LEN=3
DIM3_NAME="z"
DIM3_LEN=25

class CreateDimsTestCase(unittest.TestCase):

    def setUp(self):
        self.file = FILE_NAME
        f = netCDF4.Dataset(self.file,'w')
        f.createDimension(DIM1_NAME, DIM1_LEN)
        f.createDimension(DIM2_NAME, DIM2_LEN)
        f.createDimension(DIM3_NAME, DIM3_LEN)

        v = f.createVariable(VAR_NAME, 'f8',(DIM1_NAME,DIM2_NAME,DIM3_NAME))
        f.close()

    def tearDown(self):
        pass

    def runTest(self):
        """testing dimensions and creation of datataset"""
        f  = netCDF4.Dataset(self.file, 'r')
        d1 = f.dimensions[DIM1_NAME]
        assert d1.name == "x"
        assert d1.size == 2
        v = f.variables[VAR_NAME]
        assert v.shape == (2, 3, 25)
        f.close()

if __name__ == '__main__':
    unittest.main()
