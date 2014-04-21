#include "addresstablemodel.h"

#include "guiutil.h"
#include "walletmodel.h"

#include "wallet.h"
#include "base58.h"

#include <QFont>
#include "editaddressdialog.h"

const QString AddressTableModel::Send = "S";
const QString AddressTableModel::Receive = "R";
const QString AddressTableModel::NotRegistered = "N";
extern QList<CKeyID> reserved;
extern bool DoNotRegister;

struct AddressTableEntryLessThan
{
    bool operator()(const AddressTableEntry &a, const AddressTableEntry &b) const
    {
        return a.address < b.address;
    }
    bool operator()(const AddressTableEntry &a, const QString &b) const
    {
        return a.address < b;
    }
    bool operator()(const QString &a, const AddressTableEntry &b) const
    {
        return a < b.address;
    }
};

    void AddressTablePriv::refreshAddressTable()
    {
        bool lfirst;
        {
        LOCK(wallet->cs_wallet);
        wallet->LoadWallet(lfirst);
        }
        cachedAddressTable.clear();
        {
            LOCK(wallet->cs_wallet);
            BOOST_FOREACH(const PAIRTYPE(CTxDestination, std::string)& item, wallet->mapAddressBook)
            {
                const CQcoinAddress& address = item.first;
                const std::string& strName = item.second;
                bool fMine = IsMine(*wallet, address.Get());
                if(wallet->isNameRegistered(strName)==true)
                {
                    if(fMine == true)
                    {
                        cachedAddressTable.append(AddressTableEntry(AddressTableEntry::Receiving,
                                  QString::fromStdString(strName),
                                  QString::fromStdString(address.ToString())));
                    }else
                        cachedAddressTable.append(AddressTableEntry(AddressTableEntry::Sending,
                                  QString::fromStdString(strName),
                                  QString::fromStdString(address.ToString())));
                }else{
                    cachedAddressTable.append(AddressTableEntry(AddressTableEntry::NotRegistered,
                              QString::fromStdString(strName),
                              QString::fromStdString(address.ToString())));
                }
            }
        }
        // qLowerBound() and qUpperBound() require our cachedAddressTable list to be sorted in asc order
        qSort(cachedAddressTable.begin(), cachedAddressTable.end(), AddressTableEntryLessThan());
    }

    void AddressTablePriv::updateEntry(const QString &address, const QString &label, bool isMine, int status)
    {
        // Find address / label in model
        QList<AddressTableEntry>::iterator lower = qLowerBound(
            cachedAddressTable.begin(), cachedAddressTable.end(), address, AddressTableEntryLessThan());
        QList<AddressTableEntry>::iterator upper = qUpperBound(
            cachedAddressTable.begin(), cachedAddressTable.end(), address, AddressTableEntryLessThan());
        int lowerIndex = (lower - cachedAddressTable.begin());
        int upperIndex = (upper - cachedAddressTable.begin());
        bool inModel = (lower != upper);
        AddressTableEntry::Type newEntryType;
        if(wallet->isNameRegistered(label.toStdString()) == true)
            newEntryType = isMine ? AddressTableEntry::Receiving : AddressTableEntry::Sending;
        else
            newEntryType = AddressTableEntry::NotRegistered;
        switch(status)
        {
        case CT_NEW:
            if(inModel)
            {
                logPrint("Warning: AddressTablePriv::updateEntry: Got CT_NOW, but entry is already in model\n");
                break;
            }
            parent->beginInsertRows(QModelIndex(), lowerIndex, lowerIndex);
            cachedAddressTable.insert(lowerIndex, AddressTableEntry(newEntryType, label, address));
            parent->endInsertRows();
            break;
        case CT_UPDATED:
            if(!inModel)
            {
                logPrint("Warning: AddressTablePriv::updateEntry: Got CT_UPDATED, but entry is not in model\n");
                break;
            }
            lower->type = newEntryType;
            lower->label = label;
            lower->address = address;
            parent->emitDataChanged(lowerIndex);
            break;
        case CT_DELETED:
            if(!inModel)
            {
                logPrint("Warning: AddressTablePriv::updateEntry: Got CT_DELETED, but entry is not in model\n");
                break;
            }
            parent->beginRemoveRows(QModelIndex(), lowerIndex, upperIndex-1);
            cachedAddressTable.erase(lower, upper);
            parent->endRemoveRows();
            break;
        }
        refreshAddressTable();
    }

    int AddressTablePriv::size()
    {
        return cachedAddressTable.size();
    }

    AddressTableEntry *AddressTablePriv::index(int idx)
    {
        if(idx >= 0 && idx < cachedAddressTable.size())
        {
            return &cachedAddressTable[idx];
        }
        else
        {
            return 0;
        }
    }


AddressTableModel::AddressTableModel(CWallet *wallet, WalletModel *parent) :
    QAbstractTableModel(parent),walletModel(parent),wallet(wallet),priv(0)
{
    columns << tr("Label") << tr("Address");
    priv = new AddressTablePriv(wallet, this);
    priv->refreshAddressTable();
}

AddressTableModel::~AddressTableModel()
{
    delete priv;
}

void AddressTableModel::refreshTable()
{
    priv->refreshAddressTable();
}

int AddressTableModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return priv->size();
}

int AddressTableModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    return columns.length();
}

QVariant AddressTableModel::data(const QModelIndex &index, int role) const
{
    if(!index.isValid())
        return QVariant();

    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
        switch(index.column())
        {
        case Label:
            if(rec->label.isEmpty() && role == Qt::DisplayRole)
            {
                return tr("(no label)");
            }
            else
            {
                return rec->label;
            }
        case Address:
            return rec->address;
        }
    }
    else if (role == Qt::FontRole)
    {
        QFont font;
        if(index.column() == Address)
        {
            font = GUIUtil::qcoinAddressFont();
        }
        return font;
    }
    else if (role == TypeRole)
    {
        switch(rec->type)
        {
        case AddressTableEntry::Sending:
            return Send;
        case AddressTableEntry::Receiving:
            return Receive;
        case AddressTableEntry::NotRegistered:
            return NotRegistered;
        default: break;
        }
    }
    return QVariant();
}

bool AddressTableModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if(!index.isValid())
        return false;
    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    editStatus = OK;

    if(role == Qt::EditRole)
    {
        switch(index.column())
        {
        case Label:
            // Do nothing, if old label == new label
            if(rec->label == value.toString())
            {
                editStatus = NO_CHANGES;
                return false;
            }

            //wallet->SetAddressBookName(CQcoinAddress(rec->address.toStdString()).Get(), value.toString().toStdString());
            break;
        case Address:
            // Do nothing, if old address == new address
            if(CQcoinAddress(rec->address.toStdString()) == CQcoinAddress(value.toString().toStdString()))
            {
                editStatus = NO_CHANGES;
                return false;
            }
            // Refuse to set invalid address, set error status and return false
            else if(!walletModel->validateAddress(value.toString()))
            {
                editStatus = INVALID_ADDRESS;
                return false;
            }
            // Check for duplicate addresses to prevent accidental deletion of addresses, if you try
            // to paste an existing address over another address (with a different label)
            else if(wallet->mapAddressBook.count(CQcoinAddress(value.toString().toStdString()).Get()))
            {
                editStatus = DUPLICATE_ADDRESS;
                return false;
            }
            // Double-check that we're not overwriting a receiving address
            else if(rec->type == AddressTableEntry::Sending)
            {
                {
                  //  LOCK(wallet->cs_wallet);
                    // Remove old entry
                  //  wallet->DelAddressBookName(CQcoinAddress(rec->address.toStdString()).Get());
                    // Add new entry with new address
                   // wallet->SetAddressBookName(CQcoinAddress(value.toString().toStdString()).Get(), rec->label.toStdString());
                }
            }
            break;
        }
        return true;
    }
    return false;
}

QVariant AddressTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if(orientation == Qt::Horizontal)
    {
        if(role == Qt::DisplayRole)
        {
            return columns[section];
        }
    }
    return QVariant();
}

Qt::ItemFlags AddressTableModel::flags(const QModelIndex &index) const
{
    if(!index.isValid())
        return 0;
    AddressTableEntry *rec = static_cast<AddressTableEntry*>(index.internalPointer());

    Qt::ItemFlags retval = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    // Can edit address and label for sending addresses,
    // and only label for receiving addresses.
    if(rec->type == AddressTableEntry::NotRegistered || rec->type == AddressTableEntry::Receiving ||
      rec->type == AddressTableEntry::Sending)
    {
        retval |= Qt::ItemIsEditable;
    }
    return retval;
}

QModelIndex AddressTableModel::index(int row, int column, const QModelIndex &parent) const
{
    Q_UNUSED(parent);
    AddressTableEntry *data = priv->index(row);
    if(data)
    {
        return createIndex(row, column, priv->index(row));
    }
    else
    {
        return QModelIndex();
    }
}

void AddressTableModel::updateEntry(const QString &address, const QString &label, bool isMine, int status)
{
    // Update address book model from Mark core
    priv->updateEntry(address, label, isMine, status);
}


bool AddressTableModel::changeName(const QString &label, const QString &addr, std::string nameOld)
{
    std::string name = label.toStdString();
    std::string address = addr.toStdString();
    CQcoinAddress a(address);
    CKeyID key;
    a.GetKeyID(key);
    if(name == "")
        return false;
    if(CQcoinAddress(address).IsValid() == true && name != nameOld)
    {
        LOCK(wallet->cs_wallet);
        if(wallet->isNameRegistered(name) == true)
            return false;
        if(wallet->SetAddressBookName(CQcoinAddress(address).Get(), name, 2) == false)
        {
            return false;
        }
    }else{
        return false;
    }

    editStatus = OK;
    return true;
}

bool AddressTableModel::changeAddress(const QString &label, const QString &addr, std::string addressOld)
{
    std::string name = label.toStdString();
    std::string address = addr.toStdString();
    if(CQcoinAddress(address).IsValid() == true && address != addressOld)
    {
    LOCK(wallet->cs_wallet);
    {
        if(wallet->mapAddressBook.find(CQcoinAddress(address).Get()) == wallet->mapAddressBook.end())
        {
            if(wallet->isNameRegistered(name) == false)
            {
                if(wallet->SetAddressBookName(CQcoinAddress(address).Get(), name, 5) == false)
                {
                     return false;
                }
                wallet->DelAddressBookName(CQcoinAddress(addressOld).Get());
            }
        }else{
            return false;
        }

    }
    }else{
        return false;
    }
    editStatus = OK;
    return true;
}

void AddressTableModel::noChanges()
{
    editStatus = NO_CHANGES;
}

QString AddressTableModel::addRow(const QString &type, const QString &label, const QString &address)
{
    std::string strLabel = label.toStdString();
    std::string strAddress = address.toStdString();

    editStatus = OK;

    if(type == NotRegistered)
    {
        if(wallet->isNameRegistered(wallet->GetDefaultName()) == false)
        {
            QWidget qw;
            QMessageBox::critical(&qw,"Your default name is not registered yet!",QString("You should first register your default name %1").arg(wallet->GetNameAddressBook(wallet->vchDefaultKey.GetID()).c_str()),QMessageBox::Ok);
            return QString();
        }
        CPubKey newKey = wallet->GenerateNewKey();
        {
            LOCK(wallet->cs_wallet);
            if(wallet->mapAddressBook.count(CQcoinAddress(newKey.GetID()).Get()))
            {
                editStatus = DUPLICATE_ADDRESS;
                return QString();
            }
        }
        strAddress = CQcoinAddress(newKey.GetID()).ToString();
        if(DoNotRegister == false)
        {
            if(wallet->isNameRegistered(strLabel) == true)
            {
                QWidget qw;
                QMessageBox::warning(&qw,"Choose another name!",QString("Your name %1 is just registered in network").arg(strLabel.c_str()),QMessageBox::Ok);
                return QString();
            }
        }else{
            {
                LOCK(wallet->cs_wallet);
                wallet->SetNameBookNotToRegistered(CQcoinAddress(strAddress).Get(), strLabel);
            }
            return QString::fromStdString(strAddress);
        }
    }
    else
    {
        return QString();
    }

    // Add entry
    {
        LOCK(wallet->cs_wallet);
        wallet->SetAddressBookName(CQcoinAddress(strAddress).Get(), strLabel);
    }
    return QString::fromStdString(strAddress);
}

void AddressTableModel::setNewName()
{
    EditAddressDialog edg(EditAddressDialog::EditNotRegisteredAddress);
    QWidget qw;
    QMessageBox::warning(&qw,"Choose another name!",QString("Your name %1 is taken").arg(QString(yourName.c_str())),QMessageBox::Ok);
    yourName = "";
    edg.setModal(true);
    edg.setModel(this);
    edg.show();
}

void AddressTableModel::addDefaultReceive()
{
    std::string strLabel = wallet->GetNameAddressBook((CKeyID)(wallet->vchDefaultKey.GetID()));
    CQcoinAddress address((CKeyID)(wallet->GetWalletDefaultPubKey()));
    // Add entry
    {
        LOCK(wallet->cs_wallet);
        wallet->SetAddressBookName(address.Get(), strLabel);
    }
    return;
}

bool AddressTableModel::removeRows(int row, int count, const QModelIndex &parent)
{
    Q_UNUSED(parent);
    AddressTableEntry *rec = priv->index(row);
    if(count != 1 || !rec || rec->type == AddressTableEntry::Sending || rec->type == AddressTableEntry::Receiving)
    {
        // Can only remove one row at a time, and cannot remove rows not in model.
        // Also refuse to remove receiving addresses.
        return false;
    }
    CQcoinAddress addr(rec->address.toStdString());
    {
        LOCK(wallet->cs_wallet);
        wallet->DelAddressBookName(addr.Get());
    }
    return true;
}

/* Look up label for address in address book, if not found return empty string.
 */
QString AddressTableModel::labelForAddress(const QString &address) const
{
    {
        LOCK(wallet->cs_wallet);
        CQcoinAddress address_parsed(address.toStdString());
        std::map<CTxDestination, std::string>::iterator mi = wallet->mapAddressBook.find(address_parsed.Get());
        if (mi != wallet->mapAddressBook.end())
        {
            return QString::fromStdString(mi->second);
        }
    }
    return QString();
}

int AddressTableModel::lookupAddress(const QString &address) const
{
    QModelIndexList lst = match(index(0, Address, QModelIndex()),
                                Qt::EditRole, address, 1, Qt::MatchExactly);
    if(lst.isEmpty())
    {
        return -1;
    }
    else
    {
        return lst.at(0).row();
    }
}

void AddressTableModel::emitDataChanged(int idx)
{
    emit dataChanged(index(idx, 0, QModelIndex()), index(idx, columns.length()-1, QModelIndex()));
}
