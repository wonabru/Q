Marka Polska (former Q-coin) will start at 20/4/2014 21:00:00

To download source and compiled version:

	sudo apt-get install git -y
	git clone https://github.com/wonabru/qcoin

http://www.q-coin.org

Copyright (c) 2009-2013 Bitcoin Developers
Copyright (c) 2014	Q-coin	Developers

License
-------

Qcoin is released under the terms of the MIT license. See `COPYING` for more
information or see http://opensource.org/licenses/MIT.

Dependencies
---------------------

 Library     Purpose           Description
 -------     -------           -----------
 libssl      SSL Support       Secure communications
 libdb4.8    Berkeley DB       Blockchain & wallet storage
 libboost    Boost             C++ Library
 miniupnpc   UPnP Support      Optional firewall-jumping support

[miniupnpc](http://miniupnp.free.fr/) may be used for UPnP port mapping.  It can be downloaded from [here](
http://miniupnp.tuxfamily.org/files/).  UPnP support is compiled in and
turned off by default.  Set USE_UPNP to a different value to control this:

	USE_UPNP=     No UPnP support miniupnp not required
	USE_UPNP=0    (the default) UPnP support turned off by default at runtime
	USE_UPNP=1    UPnP support turned on by default at runtime

IPv6 support may be disabled by setting:

	USE_IPV6=0    Disable IPv6 support

Licenses of statically linked libraries:
 Berkeley DB   New BSD license with additional requirement that linked
               software must be free open source
 Boost         MIT-like license
 miniupnpc     New (3-clause) BSD license

- Versions used in this release:
-  GCC           4.3.3
-  OpenSSL       1.0.1c
-  Berkeley DB   4.8.30.NC
-  Boost         1.50
-  miniupnpc     1.6

Dependency Build Instructions: Ubuntu & Debian
----------------------------------------------
Build requirements:

	sudo apt-get update
	sudo apt-get upgrade -y
	sudo apt-get install build-essential -y
	sudo apt-get install libssl-dev -y

for Ubuntu 12.04:

	sudo apt-get install libboost-all-dev -y

 db4.8 packages are available [here](https://launchpad.net/~Q/+archive/Q).

 Ubuntu precise has packages for libdb5.1-dev and libdb5.1++-dev,
 but using these will break binary wallet compatibility, and is not recommended.

for other Ubuntu & Debian:

	sudo apt-get install libdb4.8-dev -y
	sudo apt-get install libdb4.8++-dev -y
	sudo apt-get install libboost1.37-dev -y
 (If using Boost 1.37, append -mt to the boost libraries in the makefile)

Optional:

	sudo apt-get install libminiupnpc-dev -y (see USE_UPNP compile flag)

Boost
-----
If you need to build Boost yourself:

	tar xfz boost_1_50_0.tar.gz
	cd boost_1_50_0
	./bootstrap.sh
	./b2
	sudo ./b2 install
	cd ..

miniupnpc
---------
	
	tar -xzvf miniupnpc-1.6.tar.gz
	cd miniupnpc-1.6
	make
	sudo make install
	cd ..


Berkeley DB
-----------
You need Berkeley DB 4.8.  If you have to build Berkeley DB yourself:

	tar xfz db-4.8.30.NC.tar.gz
	cd db-4.8.30.NC/build_unix
	../dist/configure --enable-cxx
	make
	sudo make install
	sudo su
	echo "/usr/local/BerkeleyDB.4.8/lib" > /etc/ld.so.conf.d/db.conf
	ldconfig
	exit
	cd ../..




OpenSSL
-----

	tar xfz openssl-1.0.1c.tar.gz
	cd openssl-1.0.1c
	./config
	make
	sudo make install
	cd ..

Qt
-----

	sudo apt-get install qtcreator libqt4-dev -y

Building
-----

	cd Q/qcoin
	qtcreator Q-qt.pro
	[Build -> Rebuild All]

Running program
-----

Goto catalog qcoin/Release_64/ or qcoin/Release_32/
or qcoin/Debug_64/ or qcoin/Debug_32/

	./PLM

Recovering wallet
-----

If for longer time nothing happens kill process ./PLM
Goto catalog qcoin/Release_64/ or qcoin/Release_32/
or qcoin/Debug_64/ or qcoin/Debug_32/

	./restart.sh

In ~/.PLMBackup is backup your waller myq.dat

If it appears error: 'Salvage fails' do not panic just do:
Find your walllet myq.dat it should be in ~/.PLMBackup or in ~/.PLM/wallet.*.bak then to recover wallet:

	/usr/local/BerkeleyDB.4.8/bin/db_dump myq.dat > dump.txt
	/usr/local/BerkeleyDB.4.8/bin/db_load myqload.dat < dump.txt
	mkdir backup
	cp -rf myq.dat backup/
	cp -rf myqload.dat myq.dat
	./PLM -afterremoveblocks=1	

Gittian Build (Windows, Linux and OSX installers building)
-----

Please look at: https://github.com/wonabru/qcoin/blob/master/doc/gitian-building.md



Windows Build
-----

Download:
miniUPnPc binary http://miniupnp.tuxfamily.org/files/download.php?file=upnpc-exe-win32-20110215.zip.  Unzip to C:\upnpc-exe-win32-20110215
miniUPnPc source http://miniupnp.tuxfamily.org/files/download.php?file=miniupnpc-1.5.20110215.tar.gz.  Untar to C:\upnpc-exe-win32-20110215\miniupnpc (you only need *.h, but the others won't hurt) untar with cd \c\upnpc-exe-win32-20110215 && tar xvvf \c\Users\Matt\Downloads\miniupnpc-1.5.20110215.tar.gz && mv miniupnpc-1.5.20110215 miniupnpc
QT-4.8 http://download.qt-project.org/development_releases/qt/4.8/4.8.6-rc1/qt-opensource-windows-x86-mingw482-4.8.6-rc1.exe

Note that the reason for using the binary miniupnpc release is that I was unable to get it to build on my system.

Add C:\MinGW\bin to your PATH environment variable (Google is your friend as it depends on your Windows Version).

In the DOS Shell:

GCC-4.4
-----

Install tdm-mingw-1.905.0-4.4.0-2.exe

In the msys Shell (MinGW shell in your start folder or C:\MinGW\msys\1.0\msys.bat):

w32api
-----

	cp w32api-3.13-mingw32-dev.tar.gz /c/
	tar xfz w32api-3.13-mingw32-dev.tar.gz
	
In the DOS Shell:

wxWidgets:
-----

	cd wxWidgets-2.8.12\build\msw
	mingw32-make -f makefile.gcc

Boost:
-----

	cd boost-1.50.0
	cp bjam.exe boost-1.50.0
	bjam.exe toolset=gcc --build-type=complete stage

In the msys Shell:

OpenSSL:
-----

	cd openssl-1.0.1c
	./config
	make
	perl util/mkdef.pl 32 libeay enable-static-engine > libeay32.def
	dllwrap --dllname libeay32.dll --output-lib libeay32.a --def libeay32.def libcrypto.a -lws2_32 -lgdi32

Copy the libeay32.dll file to the folder where you are building/running PLM.

Berkeley DB:
-----

	cd db-4.8.30.NC/build_unix
	sh ../dist/configure --enable-mingw --enable-cxx
	make

Q-coin
-----

	make -f makefile.mingw

Run with PLM.exe (make sure you have the libeay32.dll in PLM folder)

