#ifndef DELETERULEDIALOG_H
#define DELETERULEDIALOG_H

#include <QDialog>

namespace Ui {
class DeleteRuleDialog;
}

class DeleteRuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DeleteRuleDialog(QWidget *parent = nullptr);
    ~DeleteRuleDialog();

    void SetRuleNames(QStringList ruleNames);

public slots:
    void DeleteRule();
    void Cancel();

signals:
    void RuleDeleted(QString ruleName);

private:
    Ui::DeleteRuleDialog *ui;
    QStringList pRuleNames;
};

#endif // DELETERULEDIALOG_H
