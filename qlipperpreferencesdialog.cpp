#include <QtGui/QInputDialog>

#include "qlipperitem.h"
#include "qlipperpreferences.h"
#include "qlipperpreferencesdialog.h"


QlipperPreferencesDialog::QlipperPreferencesDialog(QWidget *parent) :
    QDialog(parent)
{
    setupUi(this);

    QlipperPreferences *s = QlipperPreferences::Instance();
    historyComboBox->setValue(s->value("historyCount", 5).toInt());
    displaySizeComboBox->setValue(s->value("displaySize", 30).toInt());
    trimCheckBox->setChecked(s->value("trim", true).toBool());
    shortcutWidget->setKeySequence(QKeySequence(s->value("shortcut", "CTRL+ALT+V").toString()));
    x11SelectionCheckBox->setChecked(s->value("X11Selection", false).toBool());
    macFindBufferCheckBox->setChecked(s->value("macFindBuffer", false).toBool());

    foreach(QlipperItem item, QlipperPreferences::Instance()->getStickyItems())
    {
        listWidget->addItem(item.content().toString());
    }

    connect(stickyAddButton, SIGNAL(clicked()),
            this, SLOT(stickyAddButton_clicked()));
    connect(stickyRemoveButton, SIGNAL(clicked()),
            this, SLOT(stickyRemoveButton_clicked()));
    connect(stickyUpButton, SIGNAL(clicked()),
            this, SLOT(stickyUpButton_clicked()));
    connect(stickyDownButton, SIGNAL(clicked()),
            this, SLOT(stickyDownButton_clicked()));
    connect(listWidget, SIGNAL(currentRowChanged(int)),
            this, SLOT(listWidget_currentRowChanged(int)));

    if (listWidget->count())
        listWidget->setCurrentRow(0);
    else
    {
        stickyUpButton->setEnabled(false);
        stickyDownButton->setEnabled(false);
        stickyRemoveButton->setEnabled(false);
    }
}

void QlipperPreferencesDialog::accept()
{
    QlipperPreferences *s = QlipperPreferences::Instance();
    s->setValue("historyCount", historyComboBox->value());
    s->setValue("displaySize", displaySizeComboBox->value());
    s->setValue("trim", trimCheckBox->isChecked());
    s->setValue("X11Selection", x11SelectionCheckBox->isChecked());
    s->setValue("macFindBuffer", macFindBufferCheckBox->isChecked());
    s->setValue("shortcut", shortcutWidget->keySequence().toString());

    QList<QlipperItem> list;
    for (int i = 0; i < listWidget->count(); ++i)
    {
        QlipperItem item(listWidget->item(i)->text());
        list.append(item);
    }
    QlipperPreferences::Instance()->saveStickyItems(list);

    QDialog::accept();
}

void QlipperPreferencesDialog::stickyAddButton_clicked()
{
    bool ok;
    QString s = QInputDialog::getText(this, tr("Add New Sticky Item"), tr("Add New Sticky Item"), QLineEdit::Normal, "", &ok);
    if (ok && !s.isEmpty())
    {
        listWidget->addItem(s);
        listWidget->setCurrentRow(listWidget->count()-1);
    }
}

void QlipperPreferencesDialog::stickyRemoveButton_clicked()
{
    QListWidgetItem * item = listWidget->takeItem(listWidget->currentRow());
    if (item)
        delete item;
}

void QlipperPreferencesDialog::stickyUpButton_clicked()
{
    int row = listWidget->currentRow();
    if (row == 0)
        return;

    QListWidgetItem * item = listWidget->takeItem(row);
    if (!item)
        return;

    listWidget->insertItem(row-1, item);
    listWidget->setCurrentRow(row-1);
}

void QlipperPreferencesDialog::stickyDownButton_clicked()
{
    int row = listWidget->currentRow();

    if (row == listWidget->count()-1)
        return;

    QListWidgetItem * item = listWidget->takeItem(row);
    if (!item)
        return;

    listWidget->insertItem(row+1, item);
    listWidget->setCurrentRow(row+1);
}

void QlipperPreferencesDialog::listWidget_currentRowChanged(int row)
{
    int count = listWidget->count();
    stickyUpButton->setEnabled(count && row != 0);
    stickyDownButton->setEnabled(count && row != count-1);
    stickyRemoveButton->setEnabled(count);
}