#include "editaddressdialog.h"
#include "ui_editaddressdialog.h"
#include "addressbookpage.h"

#include "addresstablemodel.h"
#include "guiutil.h"
#include "main.h"
#include "base58.h"

#include <QDataWidgetMapper>
#include <QMessageBox>

QString addressOld = "";
QString nameOld = "";

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
        ui->addressEdit->setEnabled(true);
        break;
    case NewSendingAddress:
        setWindowTitle(tr("One can register name only by solving block"));
        ui->addressEdit->setEnabled(false);
        ui->labelEdit->setEnabled(false);
        break;
    case EditReceivingAddress:
        setWindowTitle(tr("Put your name"));
        ui->addressEdit->setEnabled(true);
        ui->labelEdit->setEnabled(true);
        break;
    case EditSendingAddress:
        setWindowTitle(tr("Names registered in PLM Network"));
        ui->addressEdit->setEnabled(true);
        ui->labelEdit->setEnabled(false);
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
}

bool EditAddressDialog::saveCurrentRow()
{
    if(!model)
        return false;

    if(yourName == "0")
        acceptAndDestroy();

    switch(mode)
    {
        case NewSendingAddress:
            model->noChanges();
        break;
        case NewReceivingAddress:
        address = model->addRow(
                mode == NewSendingAddress ? AddressTableModel::Send : AddressTableModel::Receive,
                ui->labelEdit->text(),
                ui->addressEdit->text());
        break;
        case EditSendingAddress:
     //       model->noChanges();
     //   break;
        case EditReceivingAddress:
        addressOld = ui->addressEdit->text();
        nameOld = ui->labelEdit->text();
        if(::IsMine(*(model->wallet),model->wallet->GetAddress(nameOld.toStdString()).Get()) == false && ::IsMine(*(model->wallet),CQcoinAddress(addressOld.toStdString()).Get()) == false)
        {
            return false;
        }

        if(mapper->submit())
        {
            address = ui->addressEdit->text();
            name = ui->labelEdit->text();
            if(model->changeName(name,address) == false)
            {
                if(model->changeAddress(name,address) == false)
                {
                    if(mapper->submit())
                    {
                        address = addressOld;
                        name = nameOld;
                    }
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

    return !address.isEmpty();
}

void EditAddressDialog::acceptAndDestroy()
{
    QString name((const char *)yourName.c_str());
    CQcoinAddress qaddress((CKeyID)(model->wallet->GetWalletDefaultPubKey()));
    QString address((const char *)(qaddress.ToString().c_str()));
    while(name == "0")
    {
        sleep(2);
        name = ui->labelEdit->text();
        if(name != "0")
            if(model->changeName(name,address) == false)
                break;
    }
    yourName =name.toStdString();
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
