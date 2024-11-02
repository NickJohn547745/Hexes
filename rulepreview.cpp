#include "rulepreview.h"
#include "ui_rulepreview.h"

RulePreview::RulePreview(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::RulePreview)
{
    ui->setupUi(this);

    pRules = QQueue<Rule>();
    pScrollValue = 0;
    pScrollMax = 0;
}

RulePreview::~RulePreview() {
    delete ui;
}

void RulePreview::AddRule(const Rule rule) {
    pRules.enqueue(rule);
    update();
}

void RulePreview::AddRules(const QVector<Rule> rules) {
    for (int i = 0; i < rules.size(); i++) {
        pRules.enqueue(rules[i]);
    }
    update();
}

void RulePreview::DeleteRule(const Rule rule) {
    if (!pRules.contains(rule)) { return; }
    if (pRules.size() == 0) { return; }

    for (int i = 0; i < pRules.size(); i++) {
        Rule currentRule = pRules.dequeue();

        if (currentRule != rule) {
            pRules.enqueue(currentRule);
        }
    }
    update();
}

void RulePreview::DeleteRuleByName(const QString ruleName) {
    if (pRules.size() == 0) { return; }

    for (int i = 0; i < pRules.size(); i++) {
        Rule currentRule = pRules.dequeue();

        if (currentRule.Name() != ruleName) {
            pRules.enqueue(currentRule);
        }
    }
    update();
}

void RulePreview::SetRules(const QVector<Rule> rules) {
    pRules.clear();
    AddRules(rules);
}

void RulePreview::SetScrollValue(int scrollValue) {
    pScrollValue = scrollValue;
    update();
}

void RulePreview::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);

    QPainter painter(this);

    int ruleIndex = 1;
    QRectF rulesRect = event->rect();
    painter.setBrush(QColor(45, 45, 45));
    painter.setPen(QPen(QColor(60, 60, 60), 2));
    painter.drawRoundedRect(rulesRect, 8, 8);

    painter.setFont(QFont("CommitMono", 7));

    QRectF clipRect(0, rulesRect.top(), width(), rulesRect.height());
    painter.setClipRect(clipRect);

    foreach (Rule rule, pRules) {
        painter.setPen(QPen(rule.Color(), 1));

        QString typeText = "";

        QColor ruleBackground(rule.Color());
        ruleBackground.setAlpha(75);
        painter.setBrush(ruleBackground);
        int offset = ruleIndex * 5;

        // Draw the rule information in the rule stack on the right side
        QRectF ruleBackgroundRect(rulesRect.left() + 10, offset - pScrollValue + rulesRect.top() + 11 + (QFontMetrics(painter.font()).height() * 3) * (ruleIndex - 1),
                        rulesRect.width() - 20, QFontMetrics(painter.font()).height() * 3);
        QRectF ruleRect(rulesRect.left() + 11, offset - pScrollValue + rulesRect.top() + 12 + (QFontMetrics(painter.font()).height() * 3) * (ruleIndex - 1),
                        rulesRect.width() - 22, QFontMetrics(painter.font()).height() * 3);
        painter.drawRect(ruleBackgroundRect);

        pScrollMax = qMax(pScrollMax, (int)ruleRect.bottom());

        bool isSkip = false;
        switch ((int)rule.Type()) {
        case 1: // TYPE_SKIP
            typeText = QString("Skip %1 byte(s)").arg(rule.Length());
            isSkip = true;
            break;
        case 2: // TYPE_INT_8
        case 3: // TYPE_UINT_8
        case 4: // TYPE_INT_16
        case 5: // TYPE_UINT_16
        case 6: // TYPE_INT_32
        case 7: // TYPE_UINT_32
        case 8: // TYPE_INT_64
        case 9: // TYPE_UINT_64
            typeText = QString("%1-BIT %2 INTEGER").arg(rule.Length() * 8).arg(rule.Type() % 2 == 0 ? "" : "US");
            break;
        default:
            typeText = "UH-OH";
            break;
        }

        painter.setPen(Qt::white);
        QString paddedIndex = QString::number(ruleIndex).rightJustified(2, '0');
        QString ruleName = rule.Name().toUpper();

        QString ruleText = QString("%1. %2\n%3\nN/A\n").arg(paddedIndex, ruleName, typeText);
        painter.drawText(ruleRect, Qt::TextWrapAnywhere, ruleText);
        QString ruleByteOrder = QString(rule.ByteOrder() == BYTE_ORDER_BE ? "BE" : "LE") + "\n\n";
        if (!isSkip) {
            painter.drawText(ruleRect, Qt::AlignRight, ruleByteOrder);
        }
        ruleIndex++;
    }
    emit ScrollMaxChanged(qMax(0, static_cast<int>(pScrollMax - rulesRect.height())));
}

QSize RulePreview::sizeHint() const {
    return parentWidget()->size();
}
