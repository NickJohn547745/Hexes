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

        return arch;
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

    QColor pGenerateColor();
    int pGetTypeLength(RULE_TYPE ruleType);
};
Q_DECLARE_METATYPE(Rule)

#endif // RULE_H
