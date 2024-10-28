#ifndef RULEPREVIEW_H
#define RULEPREVIEW_H

#include "rule.h"

#include <QWidget>
#include <QFile>
#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QPaintEvent>
#include <QScrollBar>
#include <QQueue>
#include <QPainter>

namespace Ui {
class RulePreview;
}

class RuleDialog;

class RulePreview : public QWidget
{
    Q_OBJECT

public:
    explicit RulePreview(QWidget *parent = nullptr);
    ~RulePreview();

public slots:
    void AddRule(const Rule rule);
    void AddRules(const QVector<Rule> rules);
    void DeleteRule(const Rule rule);
    void DeleteRuleByName(const QString ruleName);
    void SetRules(const QVector<Rule> rules);

    void SetScrollValue(int scrollValue);

signals:
    void ScrollMaxChanged(int scrollMax);

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    Ui::RulePreview *ui;
    QQueue<Rule> pRules;
    int pScrollValue;
    int pScrollMax;
};

#endif // RULEPREVIEW_H
