# Example of simple Lobster-eye MPO geometry

##  Environment
- Geant4 ver 11.3
- ROOT v6.32.02

-  X-ray reflection physics codes are based on XRTG4.
 https://xraytracer.com
- Source codes are in source/src.
- These codes are partly modified for G4 11.3 and HiZ WXM MPO.

## How to build
```
 $ mkdir build
 $ cd build
 $ cmake ../source
 $ make
 $ make install
```

## How to run
### Move to TestBench directory
```
 $ cd TestBench
```

### with GUI
 ```
 $ ../bin/ex_mpo1
 ...
 Idle> /run/beamOn 10
 ...
 Idle> exit
```

### in Batch mode
 ```
 $ ../bin/ex_mpo1 run_gpsGamma.mac
```

### Plot PSF
```
 $ python -i plt_psfsct.py
```

