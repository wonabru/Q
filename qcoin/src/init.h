// Copyright (c) 2009-2010 Satoshi Nakamoto
// Copyright (c) 2009-2013 The Bitcoin developers
// Copyright (c) 2014      wonabru
// Distributed under the MIT/X11 software license, see the accompanying
// file COPYING or http://www.opensource.org/licenses/mit-license.php.
#ifndef BITCOIN_INIT_H
#define BITCOIN_INIT_H

#include "wallet.h"
#include "addresstablemodel.h"

extern CWallet* pwalletMain;
extern std::string printNamesInQNetwork();
extern QList<AddressTableEntry> NamesInQNetwork;
extern QList<CKeyID> reserved;

void StartShutdown();
bool ShutdownRequested();
void Shutdown();
void rescan();
bool AppInit2(boost::thread_group& threadGroup);
std::string HelpMessage();

#endif
