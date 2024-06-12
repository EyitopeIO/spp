# spp
preprocess shell files

`spp` will follow `cpp` syntax and substitute definitions passed from command
line `-DVAR_IS_DEFINED` or variables defined in a header file. 


## Usage

First you need to annotate your shell files and use the recognised syntax for
spp to find what you intend to substitute.

`spp -DUSER_RETURN=y -DCPE_IPADDR="192.168.3.1" -I/home/dev test1.sh test2.h` 


```
#!/bin/sh

#-- @include <config.h>

IP_ADDRESS=CPE_IPADDR

print_startup_message(){
#-- @ifdef USER_FIRSTBOOT
  echo "Why, hello there!"
#-- @elif USER_RETURN
  echo "Welcome back!"
#-- @else
  : 
#-- @endif
  wget CPE_IPADDR/userconfig.yaml
}
```
