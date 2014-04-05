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
-  Boost         1.37
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

miniupnpc
---------
	
	tar -xzvf miniupnpc-1.6.tar.gz
	cd miniupnpc-1.6
	make
	sudo make install


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


Boost
-----
If you need to build Boost yourself:

	tar xfz boost_1_50_0.tar.gz
	cd boost_1_50_0
	./bootstrap.sh
	./b2
	sudo ./b2 install

OpenSSL
-----

	tar xfz openssl-1.0.1c.tar.gz
	cd openssl-1.0.1c
	./config
	make
	sudo make install

Qt
-----

	sudo apt-get install qtcreator libqt4-dev -y

Building
-----

	cd Q/qcoin
	qtcreator Q-qt.pro
	[Build -> Rebuild All]

