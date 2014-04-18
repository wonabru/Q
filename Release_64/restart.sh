#!/bin/sh
mkdir ~/.PLMBackup
cp ~/.PLM/myq.dat ~/.PLMBackup
rm -rf ~/.PLM/[a-k]*
rm -rf ~/.PLM/[n-z]*
rm -rf ~/.PLM
echo "After synchronization is complete close PLM"
./PLM
cp -rf ~/.PLMBackup/myq.dat ~/.PLM
