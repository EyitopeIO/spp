# spp
preprocess shell files. `spp` mostly follows the c-preprocessor syntax.

## Usage

First you need to annotate your shell files and use the recognised syntax for
spp to find what you intend to substitute. See annotated example further below

`spp -Dyacc -DTEST_ARG_0 -Dlorem_ipsum test1.sh test2.sh`


```
#!/bin/bash

print_startup_message(){
#-- @ifdef TEST_ARG_0
  echo "Why, hello there!"
#-- @elif yacc || bison
  echo "Welcome back!"
#-- @else
  :
#-- @endif
#-- @ifdef lorem_ipsum && dolor_sit
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

Run `./build` from the root of this project for help on how to build. It is
a helper script. Alternatively, you may just call `cmake`, but take care not
to call it in the project root, otherwise cmake files will be scattered about,
which is annoying to cleanup.if you did so by accident.

Run `./build --help` [yet to be implemented] for how to use this script.

