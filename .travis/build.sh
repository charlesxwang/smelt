#!/bin/bash

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then
  wget https://repo.anaconda.com/miniconda/Miniconda3-latest-MacOSX-x86_64.sh -O ~/miniconda.sh
  bash ~/miniconda.sh -b -p $HOME/miniconda
  export PATH="$HOME/miniconda/bin:$PATH"
  conda update --yes conda
  conda install -c intel mkl --yes
  conda install -c intel mkl-include --yes
  conda install -c intel ipp --yes
  conda install -c intel ipp-include --yes
  export MKLROOT=$HOME/miniconda
  export IPPROOT=$HOME/miniconda
  export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$HOME/miniconda/lib  
  mkdir build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Debug
  make
  make test
  rm -rf *
  cmake .. -DCMAKE_BUILD_TYPE=Release
  make
  make test  
else
  wget https://apt.repos.intel.com/intel-gpg-keys/GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB
  sudo apt-key add GPG-PUB-KEY-INTEL-SW-PRODUCTS-2019.PUB
  sudo sh -c 'echo deb https://apt.repos.intel.com/mkl all main > /etc/apt/sources.list.d/intel-mkl.list'
  sudo sh -c 'echo deb https://apt.repos.intel.com/ipp all main > /etc/apt/sources.list.d/intel-ipp.list'
  sudo apt-get update
  sudo apt-get install intel-mkl-2019.3-062
  sudo apt-get install intel-ipp-2019.3-062
  source /opt/intel/mkl/bin/mklvars.sh intel64
  source /opt/intel/ipp/bin/ippvars.sh intel64 linux
  mkdir build
  cd build
  cmake .. -DCMAKE_BUILD_TYPE=Debug
  make
  make test
  rm -rf *
  cmake .. -DCMAKE_BUILD_TYPE=Release  
  make
  make test  
  cd ..
  doxygen Doxyfile
fi
