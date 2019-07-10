import numpy
from netCDF4 import Dataset
rootgrp = Dataset("esdm://tester", "w")
print (rootgrp.data_model)
#latitudes = rootgrp.createVariable("lat","f4",("lat",))
#longitudes = rootgrp.createVariable("lon","f4",("lon",))

rootgrp.description = "Example Description"
#lats =  numpy.arange(-90,91,2.5)
#lons =  numpy.arange(-180,180,2.5)
#latitudes[:] = lats
#longitudes[:] = lons
rootgrp.close()
