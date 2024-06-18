# spp
preprocess shell files

`spp` will follow `cpp` syntax and substitute definitions passed from command
line `-DVAR_IS_DEFINED` or variables defined in a header file. 


## Usage

First you need to annotate your shell files and use the recognised syntax for
spp to find what you intend to substitute.

`spp -Dyacc -DTEST_ARG_0 -Dlorem_ipsum test1.sh test2.h`


```
#!/bin/bash

print_startup_message(){
#-- @ifdef TEST_ARG_0
  echo "Why, hello there!"
#-- @elif yacc
  echo "Welcome back!"
#-- @else
  :
#-- @endif
#-- @ifdef lorem_ipsum
  wget $CPE_IPADDR/userconfig.yaml
#-- @endif
}
print_startup_message
```

## Building

Install required packages
```
sudo apt install cmake libgtest-dev shellcheck g++
```

Run `./build -h` from the root of this project for help on how to build. It is
a helper script. Alternatively, you may just call `cmake` from the root of this
project, but that will scatter cmake files all around which is rather annoying.

