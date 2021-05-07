# CRO - Check out ReadOut of CRE

This programs save output from DAQ encoder of Charge Readout Electronics (CRE) to CERN ROOT Tree.

## Requirements 

<ol>
<li>CERN ROOT: https://root.cern/</li>
<li>BOOST: https://www.boost.org/</li>
<li>CMAKE: https://cmake.org/</li>
</ol> 

##  Create virtual env with CONDA

Conda: https://docs.conda.io/en/latest/

If you have not conda: 

```console
wget -nv http://repo.continuum.io/miniconda/Miniconda3-latest-Linux-x86_64.sh -O miniconda.sh
bash miniconda.sh -b -p $HOME/miniconda
source $HOME/miniconda/etc/profile.d/conda.sh
```

```console
conda env create -f cro_env.yml
conda activate cro_env
```

## Installation

```console
mkdir build
cmake ..
make
```

## Examples

### Read raw data:

```console
./bin/cro -m 1:1 -input /path/to/daq.dat -output ./tmp/ro.root
```

### Read .root data:

```console
cd script
python dump.py ../tmp/ro_r504_s0.root
```
# Links

OMA_SOFT (np02daq): https://gitlab.cern.ch/vgalymov/np02daq
