from netCDF4 import Dataset
rootgrp = Dataset("esdm://tester", "w")
print (rootgrp.data_model)
rootgrp.close()
