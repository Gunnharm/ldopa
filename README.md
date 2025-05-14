### cmake configuration with non global python installation
```bash
mkdir build2 && cd build2 && cmake -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DPython3_EXECUTABLE=/home/ganvas/.pyenv/versions/3.13.2/bin/python3 -DPython3_ROOT_DIR=/home/ganvas/.pyenv/versions/3.13.2 ..
```