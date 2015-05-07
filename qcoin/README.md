http://www.q-coin.org

Copyright (c) 2009-2013 Bitcoin Developers
Copyright (c) 2014-2015	Mark	Developers

License
-------

Mark is released under the terms of the MIT license. See `COPYING` for more
information or see http://opensource.org/licenses/MIT.

Polish Mark is open cryptocurrency. It is based on hostnames in internet as the valid accounts.
Hostnames should accept port 8444 in order to be prooved as valid.
Hostnames are the names of the account. Names have to be uniq in order to be registered in network.
Names can have max 110 letters from a to z (only lower case), cyphers from 0 to 9, or '-', and only one dot '.' in name.
Subdomains are not allowed as the name (only one dot is allowed).
If you will try to register unproper hostname, or which is not accepting port 8444, the block will be registered,
money will be send to registered names, but your name will not be registered and you will not be supplied with Marks on this account in future 
(no one will see your account).
If you will not supply with no more valid hostnames, the wallet will created fake blocks with names with slash '/'.
Such a blocks are treated as the valid blocks but names are not registered (like it is with subdomain names).
If your hostname will disapear from insternet on some time, then new users of wallets, which synchronize the blocks will not see your name as valid.
They will not have you as registered account and will not send you money, when they create and mined a block. No longer you will be seen by this user, unless
he will synchronize blocks from beginning and can connect to your hostname. This is just punishment for fake users. There is no
fees for transactions, but network should work, so one have to force users to be all the time online.

The accounts in Mark network cannot be annonymouse. Everybody can see the holder of hostname.
Transactions can be send only to registered accounts.

One can calculate the value of the account based on costs of having hostname. The hostname costs around 40$ per year (with hosting on server).
No of blocks per year 210240. No of coins getting by one account is 210240 PLM = 40$ => 1 PLM = 0.02 Cent. This is when there will be one computer in network.
In order to have 1PLM = 1USD, there should be in network only 5000 hostnames (available in internet servers).
