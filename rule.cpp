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
    pRepeatOffset = 0;
    pSkipOffset = 0;
    pPreSkipOffset = 0;
    pPostSkipOffset = 0;
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
    pRepeatOffset = rule.RepeatOffset();
    pSkipOffset = rule.SkipOffset();
    pPreSkipOffset = rule.PreSkipOffset();
    pPostSkipOffset = rule.PostSkipOffset();
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
    pRepeatOffset = rule.RepeatOffset();
    pSkipOffset = rule.SkipOffset();
    pPreSkipOffset = rule.PreSkipOffset();
    pPostSkipOffset = rule.PostSkipOffset();

    return *this;
}

bool Rule::operator!() const {
    bool nameNull = pName.isEmpty();
    bool typeNull = pType == TYPE_NONE;
    bool propsNull = pProps.isEmpty();
    bool byteOrderNull = pByteOrder == BYTE_ORDER_NONE;
    bool lengthNull = pLength < 0;
    bool valueNull = pValue.isLower() || pValue.isEmpty();
    bool repeatNull = pRepeatCount < 0;
    bool preSkipNull = pPreSkipCount < 0;
    bool postSkipNull = pPostSkipCount < 0;
    return nameNull && typeNull && propsNull &&
            byteOrderNull && lengthNull && valueNull &&
            repeatNull && preSkipNull && postSkipNull;
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
    pName = name.toUpper();
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
    if (pPreSkipCount < 0) { return; }
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

void Rule::SetRepeatOffset(int repeatOffset) {
    pRepeatOffset = repeatOffset;
}

int Rule::RepeatOffset() const {
    return pRepeatOffset;
}

void Rule::SetSkipOffset(int skipOffset) {
    pSkipOffset = skipOffset;
}

int Rule::SkipOffset() const {
    return pSkipOffset;
}

void Rule::SetPreSkipOffset(int preSkipOffset) {
    pPreSkipOffset = preSkipOffset;
}

int Rule::PreSkipOffset() const {
    return pPreSkipOffset;
}

void Rule::SetPostSkipOffset(int postSkipOffset) {
    pPostSkipOffset = postSkipOffset;
}

int Rule::PostSkipOffset() const {
    return pPostSkipOffset;
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
