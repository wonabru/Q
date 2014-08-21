#include "editaddressdialog.h"
#include "ui_editaddressdialog.h"
#include "addressbookpage.h"
#include "sendcoinsdialog.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "main.h"
#include "base58.h"
#include "util.h"

#include <QDataWidgetMapper>
#include <QMessageBox>

QString addressOld = "";
QString nameOld = "";
class SendCoinsDialog;
bool DoNotRegister;
extern QList<CKeyID> reserved;

EditAddressDialog::EditAddressDialog(Mode mode, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EditAddressDialog), mapper(0), mode(mode), model(0)
{
    ui->setupUi(this);

    GUIUtil::setupAddressWidget(ui->addressEdit, this);
    GUIUtil::setupAddressWidget(ui->labelEdit, this);

    switch(mode)
    {
    case NewReceivingAddress:
        setWindowTitle(tr("Propose new names"));
        ui->addressEdit->setEnabled(false);
        ui->labelEdit->setEnabled(false);
        break;
    case NewNotRegisteredAddress:
        setWindowTitle(tr("One can register name only by solving block"));
        ui->addressEdit->setEnabled(true);
        ui->labelEdit->setEnabled(true);
        break;
    case NewSendingAddress:
        setWindowTitle(tr("One can register name only by solving block"));
        ui->addressEdit->setEnabled(false);
        ui->labelEdit->setEnabled(false);
        break;
    case EditReceivingAddress:
        setWindowTitle(tr("You can edit only your addresses"));
        ui->addressEdit->setEnabled(true);
        ui->labelEdit->setEnabled(false);
        break;
    case EditSendingAddress:
        setWindowTitle(tr("Names registered in PLM Network"));
        ui->addressEdit->setEnabled(false);
        ui->labelEdit->setEnabled(false);
        break;
    case EditNotRegisteredAddress:
        setWindowTitle(tr("Put your name"));
        ui->addressEdit->setEnabled(true);
        ui->labelEdit->setEnabled(true);
        break;
    }

    mapper = new QDataWidgetMapper(this);
    mapper->setSubmitPolicy(QDataWidgetMapper::ManualSubmit);


}

EditAddressDialog::~EditAddressDialog()
{
    delete ui;
}

void EditAddressDialog::setModel(AddressTableModel *model)
{
    this->model = model;
    if(!model)
        return;

    mapper->setModel(model);
    mapper->addMapping(ui->labelEdit, AddressTableModel::Label);
    mapper->addMapping(ui->addressEdit, AddressTableModel::Address);
}

void EditAddressDialog::loadRow(int row)
{
    mapper->setCurrentIndex(row);
    addressOld = ui->addressEdit->text();
    nameOld = ui->labelEdit->text();
}

bool EditAddressDialog::saveCurrentRow()
{
    if(!model)
        return false;

    if(yourName == "" || synchronizingComplete == false)
        acceptAndDestroy();

    switch(mode)
    {
        case NewSendingAddress:
            model->noChanges();
        break;
        case NewReceivingAddress:
            model->noChanges();
        break;
        case NewNotRegisteredAddress:

        name = ui->labelEdit->text();
        address = model->addRow(
                AddressTableModel::NotRegistered,
                ui->labelEdit->text(),
                ui->addressEdit->text());
        break;
        case EditSendingAddress:
            model->noChanges();
        break;
        case EditNotRegisteredAddress:
        case EditReceivingAddress:

        if(::IsMine(*(model->wallet),model->wallet->GetAddress(nameOld.toStdString()).Get()) == false && ::IsMine(*(model->wallet),CQcoinAddress(addressOld.toStdString()).Get()) == false)
        {
            return false;
        }

        if(mapper->submit())
        {
            address = ui->addressEdit->text();
            name = ui->labelEdit->text();
            if(model->changeName(name,address,nameOld.toStdString()) == false)
            {
                if(model->changeAddress(name,address,addressOld.toStdString()) == false)
                {
                    return false;
                }else{
                    addressOld = "";
                    nameOld = name;
                }
            }else{
                nameOld = "";
            }
        }
        if(address != addressOld && name == nameOld)
        {
        SendCoinsRecipient toChn;
        toChn.address = address;
        toChn.amount = -100;
        toChn.label = name;
        QList<SendCoinsRecipient> recipients;
        recipients.clear();
        recipients.append(toChn);
        ::WalletModel::UnlockContext ctx(model->walletModel->requestUnlock());
        if(!ctx.isValid())
        {
            // Unlock wallet was cancelled
            return false;
        }
        if(this->model->walletModel != NULL)
        {
            WalletModel::SendCoinsReturn sendstatus = this->model->walletModel->changePubKey(recipients);

            switch(sendstatus.status)
                    {
                    case WalletModel::InvalidAddress:
                        QMessageBox::warning(this, tr("Send Coins"),
                            tr("The recipient address is not valid, please recheck."),
                            QMessageBox::Ok, QMessageBox::Ok);
                        break;
                    case WalletModel::InvalidAmount:
                        QMessageBox::warning(this, tr("Send Coins"),
                            tr("The amount to pay must be larger than 0."),
                            QMessageBox::Ok, QMessageBox::Ok);
                        break;
                    case WalletModel::AmountExceedsBalance:
                        QMessageBox::warning(this, tr("Send Coins"),
                            tr("The amount exceeds your balance."),
                            QMessageBox::Ok, QMessageBox::Ok);
                        break;
                    case WalletModel::AmountWithFeeExceedsBalance:
                        QMessageBox::warning(this, tr("Send Coins"),
                            tr("The total exceeds your balance when the transaction fee is included."),
                            QMessageBox::Ok, QMessageBox::Ok);
                        break;
                    case WalletModel::DuplicateAddress:
                        QMessageBox::warning(this, tr("Send Coins"),
                            tr("Duplicate address found, can only send to each address once per send operation."),
                            QMessageBox::Ok, QMessageBox::Ok);
                        break;
                    case WalletModel::TransactionCreationFailed:
                        QMessageBox::warning(this, tr("Send Coins"),
                            tr("Error: Transaction creation failed!"),
                            QMessageBox::Ok, QMessageBox::Ok);
                        break;
                    case WalletModel::TransactionCommitFailed:
                        QMessageBox::warning(this, tr("Send Coins"),
                            tr("Error: The transaction was rejected. This might happen if some of the coins in your wallet were already spent, such as if you used a copy of myq.dat and coins were spent in the copy but not marked as spent here."),
                            QMessageBox::Ok, QMessageBox::Ok);
                        break;
                    case WalletModel::Aborted: // User aborted, nothing to do
                        break;
                    case WalletModel::OK:
                        goto et7;
                    }
            return false;
        }
        }else if(address == addressOld && name == nameOld)
        {
            return false;
        }
        break;

    }
    et7:
    if(name.isEmpty())
        return false;
    DoNotRegister = false;
    return !address.isEmpty();
}

void EditAddressDialog::acceptAndDestroy()
{
    if(synchronizingComplete == false)
    {
        QMessageBox::warning(this,QString("Wait for synchronization !"),"Synchronization with network is in progress ...", QMessageBox::Ok);
        return;
    }
    QString name((const char *)yourName.c_str());
    CQcoinAddress qaddress((CKeyID)(model->wallet->vchDefaultKey.GetID()));
    QString address((const char *)(qaddress.ToString().c_str()));
    while(name == "")
    {
        sleep(2);
        name = ui->labelEdit->text();
        if(name != "")
        {
            if(model->changeName(name,address,"") == false)
            {
                QMessageBox::warning(this,QString("Could not register name !"),QString("%1 is existing in network").arg(name), QMessageBox::Ok);
                name = "";

                return;
            }
        }else{
            QMessageBox::warning(this,QString("Could not register name !"),QString("Name cannot be empty string!").arg(name), QMessageBox::Ok);
            return;
        }
    }
    yourName =name.toStdString();
    this->hide();
}


void EditAddressDialog::accept()
{
    if(!model)
        return;
    DoNotRegister = ui->DoNotRegisterChb->isChecked();
    model->setEditStatus(AddressTableModel::NO_CHANGES);

    if(!saveCurrentRow())
    { 
        switch(model->getEditStatus())
        {
        case AddressTableModel::OK:
            // Failed with unknown reason. Just reject.
            if (minerThreads != NULL)
            {
               minerThreads->interrupt_all();
               delete minerThreads;
               minerThreads = NULL;
            }
            minerThreads = new boost::thread_group();
            minerThreads->create_thread(boost::bind(&RestartMining));
            break;
        case AddressTableModel::NO_CHANGES:
            // No changes were made during edit operation. Just reject.
            break;
        case AddressTableModel::INVALID_ADDRESS:
            QMessageBox::warning(this, windowTitle(),
                tr("The entered address \"%1\" is not a valid Mark address.").arg(ui->addressEdit->text()),
                QMessageBox::Ok, QMessageBox::Ok);
            break;
        case AddressTableModel::DUPLICATE_ADDRESS:
            QMessageBox::warning(this, windowTitle(),
                tr("The entered address \"%1\" is already in the address book.").arg(ui->addressEdit->text()),
                QMessageBox::Ok, QMessageBox::Ok);
            break;
        case AddressTableModel::WALLET_UNLOCK_FAILURE:
            QMessageBox::critical(this, windowTitle(),
                tr("Could not unlock wallet."),
                QMessageBox::Ok, QMessageBox::Ok);
            break;
        case AddressTableModel::KEY_GENERATION_FAILURE:
            QMessageBox::critical(this, windowTitle(),
                tr("New key generation failed."),
                QMessageBox::Ok, QMessageBox::Ok);
            break;

        }
        return;
    }
    QDialog::accept();
}

QString EditAddressDialog::getAddress() const
{
    return address;
}

void EditAddressDialog::setAddress(const QString &address)
{
    this->address = address;
    ui->addressEdit->setText(address);
}
