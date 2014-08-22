/*
 * Qt4 qcoin GUI.
 *
 * W.J. van der Laan 2011-2012
 * The Mark Developers 2011-2013
 */
#ifndef WALLETFRAME_H
#define WALLETFRAME_H

#include <QFrame>

class QcoinGUI;
class ClientModel;
class WalletModel;
class WalletStack;

class WalletFrame : public QFrame
{
    Q_OBJECT
public:
    explicit WalletFrame(QcoinGUI *_gui);
    ~WalletFrame();

    void setClientModel(ClientModel *clientModel);

    bool addWallet(const QString& name, WalletModel *walletModel);
    bool setCurrentWallet(const QString& name);

    void removeAllWallets();

    bool handleURI(const QString &uri);

    void showOutOfSyncWarning(bool fShow);

private:
    QcoinGUI *gui;
    ClientModel *clientModel;
    WalletStack *walletStack;

public slots:
    /** Switch to overview (home) page */
    void gotoOverviewPage();
    /** Switch to history (transactions) page */
    void gotoHistoryPage();
    /** Switch to address book page */
    void gotoAddressBookPage();
    void gotoEditName();
    /** Switch to receive coins page */
    void gotoReceiveCoinsPage();
    void gotoNotRegisteredCoinsPage();
    /** Switch to send coins page */
    void gotoSendCoinsPage(QString addr = "");

    /** Show Sign/Verify Message dialog and switch to sign message tab */
    void gotoSignMessageTab(QString addr = "");
    /** Show Sign/Verify Message dialog and switch to verify message tab */
    void gotoVerifyMessageTab(QString addr = "");

    /** Encrypt the wallet */
    void encryptWallet(bool status);
    /** Backup the wallet */
    void backupWallet();
    /** Change encrypted wallet passphrase */
    void changePassphrase();
    /** Ask for passphrase to unlock wallet temporarily */
    void unlockWallet();

    /** Set the encryption status as shown in the UI.
     @param[in] status            current encryption status
     @see WalletModel::EncryptionStatus
     */
    void setEncryptionStatus();
};

#endif // WALLETFRAME_H
