#!/bin/sh
mkdir ~/.PLMBackup
cp ~/.PLM/myq.dat ~/.PLMBackup
rm -rf ~/.PLM/[a-k]*
rm -rf ~/.PLM/[n-z]*
