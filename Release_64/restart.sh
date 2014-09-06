#!/bin/sh
mkdir ~/.PLMBackup > /dev/null 2>&1
cp ~/.PLM/myq.dat ~/.PLMBackup
rm -rf ~/.PLM/[a-d]*
rm -rf ~/.PLM/peers.dat
./PLM -afterremoveblocks=1
