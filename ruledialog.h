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

    void SetBaseDir(QString dir);

    void SetRuleNames(QStringList ruleNames);
    void SetVars(QMap<QString, BlockValue> vars);

private slots:
    void pQueueRule();
    void pClearRules();
    void pSaveRules();
    void pPopFirstRule();
    void pPopLastRule();
    void pCancelDialog();
    void pImportSnippet();
    void pExportSnippet();
    void pTypeChanged(int index);
    void pSkip_PrevVarToggled(bool toggled);
    void pSkip_StaticValToggled(bool toggled);
    void pPre_PrevVarToggled(bool toggled);
    void pPre_StaticValToggled(bool toggled);
    void pPost_PrevVarToggled(bool toggled);
    void pPost_StaticValToggled(bool toggled);
    void pRepeat_PrevVarToggled(bool toggled);
    void pRepeat_StaticValToggled(bool toggled);

signals:
    void RuleChanged(Rule rule);
    void RulesChanged(QVector<Rule> rules);
    void BaseDirChanged(QString dir);

protected:
    void closeEvent(QCloseEvent *event) override;

private:
    Ui::RuleDialog *ui;
    Rule pRule;
    QStringList pRuleNames;
    QMap<QString, BlockValue> pVars;
    int pDefaultRuleCount;
    QQueue<Rule> pRules;
    QString pSnippetDir;

    void pHideLayout(QLayout *layout);
    void pShowLayout(QLayout *layout);
};

#endif // RULEDIALOG_H
