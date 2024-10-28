#include "rule.h"

Rule::Rule() {
    pName = "";
    pType = TYPE_NONE;
    pProps = QVariantMap();
    pColor = pGenerateColor();
    pByteOrder = BYTE_ORDER_NONE;
    pLength = -1;
    pValue = "";
    pRepeatCount = 0;
    pPreSkipCount = 0;
    pPostSkipCount = 0;
}

Rule::Rule(const Rule &rule) {
    pName = rule.Name();
    pType = rule.Type();
    pProps = rule.Properties();
    pColor = rule.Color();
    pByteOrder = rule.ByteOrder();
    pLength = rule.Length();
    pValue = rule.Value();
    pRepeatCount = rule.RepeatCount();
    pPreSkipCount = rule.PreSkipCount();
    pPostSkipCount = rule.PostSkipCount();
}

Rule &Rule::operator=(const Rule &rule) {
    if (this == &rule) {
        return *this;
    }

    pName = rule.Name();
    pType = rule.Type();
    pProps = rule.Properties();
    pColor = rule.Color();
    pByteOrder = rule.ByteOrder();
    pLength = rule.Length();
    pValue = rule.Value();
    pRepeatCount = rule.RepeatCount();
    pPreSkipCount = rule.PreSkipCount();
    pPostSkipCount = rule.PostSkipCount();

    return *this;
}

bool Rule::operator==(const Rule &rule) const {
    bool nameMatch = pName == rule.Name();
    bool typeMatch = pType == rule.Type();
    bool propsMatch = pProps == rule.Properties();
    bool byteOrderMatch = pByteOrder == rule.ByteOrder();
    bool lengthMatch = pLength == rule.Length();
    bool valueMatch = pValue == rule.Value();
    bool repeatMatch = pRepeatCount == rule.RepeatCount();
    bool preSkipMatch = pPreSkipCount == rule.PreSkipCount();
    bool postSkipMatch = pPostSkipCount == rule.PostSkipCount();
    return nameMatch && typeMatch && propsMatch &&
            byteOrderMatch && lengthMatch && valueMatch &&
            repeatMatch && preSkipMatch && postSkipMatch;
}

bool Rule::operator!=(const Rule &rule) const  {
    return !operator==(rule);
}

void Rule::SetName(const QString name) {
    pName = name;
}

QString Rule::Name() const {
    return pName;
}

void Rule::SetType(RULE_TYPE type) {
    pType = type;

    if (pLength == -1) {
        pLength = pGetTypeLength(type);
    }
}

RULE_TYPE Rule::Type() const {
    return pType;
}

void Rule::SetColor(const QColor color) {
    pColor = color;
}

QColor Rule::Color() const {
    return pColor;
}

void Rule::SetByteOrder(BYTE_ORDER byteOrder) {
    pByteOrder = byteOrder;
}

BYTE_ORDER Rule::ByteOrder() const {
    return pByteOrder;
}

void Rule::SetLength(int length) {
    pLength = length;
}

int Rule::Length() const {
    return pLength;
}

void Rule::SetValue(QString value) {
    pValue = value;
}

QString Rule::Value() const {
    return pValue;
}

QVariant Rule::GetProperty(QString propName) const {
    return pProps[propName];
}

void Rule::SetProperties(QVariantMap props) {
    pProps = props;
}

QVariantMap Rule::Properties() const {
    return pProps;
}

void Rule::SetRepeatCount(int repeatCount) {
    pRepeatCount = repeatCount;
}

int Rule::RepeatCount() const {
    return pRepeatCount;
}

void Rule::SetPreSkipCount(int preSkipCount) {
    pPreSkipCount = preSkipCount;
}

int Rule::PreSkipCount() const {
    return pPreSkipCount;
}

void Rule::SetPostSkipCount(int postSkipCount) {
    pPostSkipCount = postSkipCount;
}

int Rule::PostSkipCount() const {
    return pPostSkipCount;
}

QColor Rule::pGenerateColor() {
    QRandomGenerator64 *globalRand = QRandomGenerator64::global();
    double rRand = globalRand->generateDouble();
    double gRand = globalRand->generateDouble();
    double bRand = globalRand->generateDouble();
    return QColor(rRand * 255, gRand * 255, bRand * 255);
}

int Rule::pGetTypeLength(RULE_TYPE ruleType) {
    int typeLength = 0;
    switch ((int)ruleType) {
    case 1: // TYPE_SKIP
        typeLength = 0;
        break;
    case 2: // TYPE_INT_8
        typeLength = 1;
        break;
    case 3: // TYPE_UINT_8
        typeLength = 1;
        break;
    case 4: // TYPE_INT_16
        typeLength = 2;
        break;
    case 5: // TYPE_UINT_16
        typeLength = 2;
        break;
    case 6: // TYPE_INT_32
        typeLength = 4;
        break;
    case 7: // TYPE_UINT_32
        typeLength = 4;
        break;
    case 8: // TYPE_INT_64
        typeLength = 8;
        break;
    case 9: // TYPE_UINT_64
        typeLength = 8;
        break;
    default: // Unknown
        typeLength = 0;
        break;
    }
    return typeLength;
}

void Rule::SetProperty(QString propName, QVariant propData) {
    pProps[propName] = propData;
}
