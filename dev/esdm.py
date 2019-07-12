import numpy
from netCDF4 import Dataset
rootgrp = Dataset("esdm://tester", "w", format="ESDM")
print (rootgrp.data_model)
lat = rootgrp.createDimension("lat", 73)
lon = rootgrp.createDimension("lon", 144)

rootgrp.description = "Example Description"

latitudes = rootgrp.createVariable("lat","f4", ("lat",))
longitudes = rootgrp.createVariable("lon","f4", ("lon",))

latitudes.unit = "degree"
latitudes.something = 5

lats =  numpy.arange(-90,91,2.5)
lons =  numpy.arange(-180,180,2.5)
latitudes[:] = lats
longitudes[:] = lons

rootgrp.close()
