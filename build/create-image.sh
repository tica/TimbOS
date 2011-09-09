# /bin/bash

cp build/template.img ./boot.img

export MTOOLSRC=build/mtools.cfg

mcopy $1 x:\boot\

mv ./boot.img $2


