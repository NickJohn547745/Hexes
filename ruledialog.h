#ifndef RULEDIALOG_H
#define RULEDIALOG_H

#include "rule.h"
#include "blockvalue.h"
#include "rulepreview.h"

#include <QDialog>
#include <QQueue>

namespace Ui {
class RuleDialog;
}

class RuleDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RuleDialog(QWidget *parent = nullptr);
    ~RuleDialog();

    Rule GetRule();
    void SetRule(const Rule rule);

    void SetRuleNames(QStringList ruleNames);
    void SetVars(QMap<QString, BlockValue> vars);

private slots:
    void pQueueRule();
    void pClearRules();
    void pSaveRules();
    void pPopFirstRule();
    void pPopLastRule();
    void pCancelDialog();
    void pTypeChanged(int index);
    void pRepeatToggled(bool toggled);
    void pPrevVarToggled(bool toggled);
    void pStaticValToggled(bool toggled);
    void pPre_PrevVarToggled(bool toggled);
    void pPre_StaticValToggled(bool toggled);
    void pPost_PrevVarToggled(bool toggled);
    void pPost_StaticValToggled(bool toggled);

signals:
    void RuleChanged(Rule rule);
    void RulesChanged(QVector<Rule> rules);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::RuleDialog *ui;
    Rule pRule;
    QStringList pRuleNames;
    QMap<QString, BlockValue> pVars;
    int pDefaultRuleCount;
    QVector<Rule> pRules;

    void pHideLayout(QLayout *layout);
    void pShowLayout(QLayout *layout);
};

#endif // RULEDIALOG_H
