#include "deleteruledialog.h"
#include "ui_deleteruledialog.h"

DeleteRuleDialog::DeleteRuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DeleteRuleDialog) {
    ui->setupUi(this);

    pRuleNames = QStringList();

    connect(ui->pushButton_Delete, &QPushButton::clicked, this, &DeleteRuleDialog::DeleteRule);
    connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &DeleteRuleDialog::Cancel);
}

DeleteRuleDialog::~DeleteRuleDialog() {
    delete ui;
}

void DeleteRuleDialog::SetRuleNames(QStringList ruleNames) {
    ui->listWidget_Rules->clear();
    foreach (QString ruleName, ruleNames) {
        ui->listWidget_Rules->addItem(ruleName);
    }
}

void DeleteRuleDialog::DeleteRule() {
    QList<QListWidgetItem *> items = ui->listWidget_Rules->selectedItems();
    if (items.count() == 0) { return; }

    foreach (QListWidgetItem *item, items) {
        emit RuleDeleted(item->text());
    }

    close();
}

void DeleteRuleDialog::Cancel() {
    close();
}
