#!/bin/sh
mkdir ~/.PLMBackup > /dev/null 2>&1
cp ~/.PLM/myq.dat ~/.PLMBackup
rm -rf ~/.PLM/[a-d]*
./PLM -afterremoveblocks=1
