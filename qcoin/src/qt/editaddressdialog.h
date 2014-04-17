#ifndef EDITADDRESSDIALOG_H
#define EDITADDRESSDIALOG_H

#include <QDialog>

namespace Ui {
    class EditAddressDialog;
}
class AddressTableModel;

QT_BEGIN_NAMESPACE
class QDataWidgetMapper;
QT_END_NAMESPACE

/** Dialog for editing an address and associated information.
 */
class EditAddressDialog : public QDialog
{
    Q_OBJECT

public:
    enum Mode {
        NewReceivingAddress,
        NewSendingAddress,
        NewNotRegisteredAddress,
        EditReceivingAddress,
        EditSendingAddress,
        EditNotRegisteredAddress
    };

    explicit EditAddressDialog(Mode mode, QWidget *parent = 0);

    ~EditAddressDialog();

    void setModel(AddressTableModel *model);
    void loadRow(int row);

    QString getAddress() const;
    void setAddress(const QString &address);


signals:
public slots:
    void accept();
    void acceptAndDestroy();
private:
    bool saveCurrentRow();
    Ui::EditAddressDialog *ui;
    QDataWidgetMapper *mapper;
    Mode mode;
    AddressTableModel *model;

    QString address;
    QString name;
};

#endif // EDITADDRESSDIALOG_H
