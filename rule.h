#ifndef RULE_H
#define RULE_H

#include <QString>
#include <QMap>
#include <QVariant>
#include <QRandomGenerator64>
#include <QColor>

enum RULE_TYPE {
    TYPE_NONE    = 0,
    TYPE_SKIP    = 1,
    TYPE_INT_8   = 2,
    TYPE_UINT_8  = 3,
    TYPE_INT_16  = 4,
    TYPE_UINT_16 = 5,
    TYPE_INT_32  = 6,
    TYPE_UINT_32 = 7,
    TYPE_INT_64  = 8,
    TYPE_UINT_64 = 9
};

enum BYTE_ORDER {
    BYTE_ORDER_NONE = 0,
    BYTE_ORDER_LE = 1,
    BYTE_ORDER_BE = 2
};

class Rule : public QObject
{
    Q_OBJECT

public:
    Rule();
    Rule(const Rule &rule);

    Rule &operator=(const Rule &rule);
    bool operator!() const;
    bool operator==(const Rule& rule) const;
    bool operator!=(const Rule& rule) const;

    friend QDataStream &operator<<(QDataStream &arch, const Rule &rule) {
        arch << rule.Name();
        arch << rule.Type();
        arch << rule.Properties();
        arch << rule.Color();
        arch << rule.ByteOrder();
        arch << rule.Length();
        arch << rule.Value();
        arch << rule.PreSkipCount();
        arch << rule.RepeatCount();
        arch << rule.PostSkipCount();
        arch << rule.SkipOffset();
        arch << rule.RepeatOffset();
        arch << rule.PreSkipOffset();
        arch << rule.PostSkipOffset();
        return arch;
    }
    friend QDataStream &operator>>(QDataStream &arch, Rule &rule) {
        QString ruleName = "";
        RULE_TYPE ruleType = TYPE_NONE;
        QVariantMap ruleProps = QVariantMap();
        QColor ruleColor = QColor();
        BYTE_ORDER ruleByteOrder = BYTE_ORDER_NONE;
        int ruleLength = 0;
        QString ruleValue = "";
        int preSkipCount = 0;
        int repeatCount = 0;
        int postSkipCount = 0;
        int repeatOffset;
        int skipOffset;
        int preSkipOffset;
        int postSkipOffset;

        arch >> ruleName;
        rule.SetName(ruleName);
        arch >> ruleType;
        rule.SetType(ruleType);
        arch >> ruleProps;
        rule.SetProperties(ruleProps);
        arch >> ruleColor;
        rule.SetColor(ruleColor);
        arch >> ruleByteOrder;
        rule.SetByteOrder(ruleByteOrder);
        arch >> ruleLength;
        rule.SetLength(ruleLength);
        arch >> ruleValue;
        rule.SetValue(ruleValue);
        arch >> preSkipCount;
        rule.SetPreSkipCount(preSkipCount);
        arch >> repeatCount;
        rule.SetRepeatCount(repeatCount);
        arch >> postSkipCount;
        rule.SetPostSkipCount(postSkipCount);
        arch >> repeatOffset;
        rule.SetRepeatOffset(repeatOffset);
        arch >> skipOffset;
        rule.SetSkipOffset(skipOffset);
        arch >> preSkipOffset;
        rule.SetPreSkipOffset(preSkipOffset);
        arch >> postSkipOffset;
        rule.SetPostSkipOffset(postSkipOffset);

        return arch;
    }

    operator QString() const {
        return QString("Rule: %1\n"
                       "-Type: %2\n"
                       "-Color: %4\n"
                       "-ByteOrder: %5\n"
                       "-Length: %6\n"
                       "-Value: %7\n"
                       "-PreSkipCount: %8\n"
                       "-RepeatCount: %9\n"
                       "-PostSkipCount: %10\n"
                       "-SkipOffset: %11\n"
                       "-RepeatOffset: %12\n"
                       "-PreSkipOffset: %13\n"
                       "-PostSkipOffset: %14\n")
                .arg(pName)
                .arg(pType)
                .arg(pColor.name())
                .arg(pByteOrder)
                .arg(pLength)
                .arg(pValue)
                .arg(pPreSkipCount)
                .arg(pRepeatCount)
                .arg(pPostSkipCount)
                .arg(pSkipOffset)
                .arg(pRepeatOffset)
                .arg(pPreSkipOffset)
                .arg(pPostSkipOffset);
    }

    void SetName(const QString name);
    QString Name() const;

    void SetType(RULE_TYPE type);
    RULE_TYPE Type() const;

    void SetColor(const QColor color);
    QColor Color() const;

    void SetByteOrder(BYTE_ORDER byteOrder);
    BYTE_ORDER ByteOrder() const;

    void SetLength(int length);
    int Length() const;

    void SetValue(QString value);
    QString Value() const;

    void SetProperty(QString propName, QVariant propData);
    QVariant GetProperty(QString propName) const;

    void SetProperties(QVariantMap props);
    QVariantMap Properties() const;

    void SetRepeatCount(int repeatCount);
    int RepeatCount() const;

    void SetPreSkipCount(int preSkipCount);
    int PreSkipCount() const;

    void SetPostSkipCount(int postSkipCount);
    int PostSkipCount() const;

    void SetRepeatOffset(int repeatOffset);
    int RepeatOffset() const;

    void SetSkipOffset(int skipOffset);
    int SkipOffset() const;

    void SetPreSkipOffset(int preSkipOffset);
    int PreSkipOffset() const;

    void SetPostSkipOffset(int postSkipOffset);
    int PostSkipOffset() const;

private:
    QString pName;
    RULE_TYPE pType;
    QVariantMap pProps;
    QColor pColor;
    BYTE_ORDER pByteOrder;
    int pLength;
    QString pValue;
    int pRepeatCount;
    int pPreSkipCount;
    int pPostSkipCount;
    int pRepeatOffset;
    int pSkipOffset;
    int pPreSkipOffset;
    int pPostSkipOffset;

    QColor pGenerateColor();
    int pGetTypeLength(RULE_TYPE ruleType);
};
Q_DECLARE_METATYPE(Rule)

#endif // RULE_H
