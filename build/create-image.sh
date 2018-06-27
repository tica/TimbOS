# /bin/bash

cp build/template.img ./boot.img

export MTOOLSRC=build/mtools.cfg

mcd x:..
mcd x:boot

mcopy $1 x:
mcopy $2 x:menu.cfg

mv ./boot.img $3


