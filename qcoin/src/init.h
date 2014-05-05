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
extern CWallet* wallet1;
extern CWallet* walletQ;
extern CWallet* walletWonabru;
extern std::string printNamesInQNetwork();
extern QList<AddressTableEntry> NamesInQNetwork;
extern QList<AddressTableEntry> NamesInQNetworkToChange;
extern QList<CKeyID> reserved;
extern QMap<CAddress,int> tryingAddresses;
extern std::string whoami;
const int mainNodesNumber=5;
extern bool synchronizingComplete;
extern bool synchronizingAlmostComplete;
extern std::string mainNodes[mainNodesNumber];
extern boost::thread_group* minerThreads;

void StartShutdown();
bool ShutdownRequested();
void Shutdown();
void rescan(CWallet *wallet, CBlockIndex *pindexBest, CBlockIndex *pindexGenesisBlock);
bool AppInit2(boost::thread_group& threadGroup);
std::string HelpMessage();

#endif
