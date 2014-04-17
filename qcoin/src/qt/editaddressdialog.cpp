#include "editaddressdialog.h"
#include "ui_editaddressdialog.h"
#include "addressbookpage.h"
#include "sendcoinsdialog.h"
#include "addresstablemodel.h"
#include "guiutil.h"
#include "main.h"
#include "base58.h"

#include <QDataWidgetMapper>
#include <QMessageBox>

QString addressOld = "";
QString nameOld = "";
class SendCoinsDialog;
extern bool DoNotRegister;
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
        DoNotRegister = ui->DoNotRegisterChb->isChecked();
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
           this->model->walletModel->changePubKey(recipients);
        }else if(address == addressOld && name == nameOld)
        {
            return false;
        }
        break;

    }
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
    if(model->wallet->isNameRegistered(model->wallet->GetDefaultName()) == false)
        reserved.push_back(model->wallet->vchDefaultKey.GetID());
    this->hide();
}


void EditAddressDialog::accept()
{
    if(!model)
        return;

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
