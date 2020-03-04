#!/usr/bin/env python3
import xarray as xr
nc = xr.open_dataset("ape_o3_T42.nc")

# relative filename without prefix doesn't work with xarray as it adds CWD.
nc.to_netcdf("/test.nc", format = "esdm")
