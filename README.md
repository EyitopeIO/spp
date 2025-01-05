# spp
preprocess shell files. `spp` mostly follows the c-preprocessor syntax.

## Usage

First you need to annotate your shell files and use the recognised syntax for
spp to find what you intend to substitute. See annotated example further below.
Any comment/definition after the endif directive is ignored.

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
#-- @endif TEST_ARG_0
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

Run `build` from the root of this project. The executable would be `cbuild/spp`
Alternatively, you may just call `cmake`. Use `build -h` for help.

