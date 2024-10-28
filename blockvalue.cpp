#include "blockvalue.h"

BlockValue::BlockValue(QObject *parent)
    : QObject{parent},
    pBlockType{BLOCK_TYPE_INT8},
    pName{""},
    pData{} {

}

BlockValue::BlockValue(const BlockValue &blockVal) {
    pBlockType = blockVal.BlockType();
    pName = blockVal.Name();
    pData = blockVal.Value();
}

BlockValue &BlockValue::operator=(const BlockValue &blockVal) {
    if (this == &blockVal) {
        return *this;
    }

    pBlockType = blockVal.BlockType();
    pName = blockVal.Name();
    pData = blockVal.Value();

    return *this;
}

int BlockValue::ValueToInt() {
    switch ((int)pBlockType) {
    case 0: // BLOCK_TYPE_INT8
        return static_cast<int>(pData.value<qint8>());
        break;
    case 1: // BLOCK_TYPE_UINT8
        return static_cast<int>(pData.value<quint8>());
        break;
    case 2: // BLOCK_TYPE_INT16
        return static_cast<int>(pData.value<qint16>());
        break;
    case 3: // BLOCK_TYPE_UINT16
        return static_cast<int>(pData.value<quint16>());
        break;
    case 4: // BLOCK_TYPE_INT32
        return static_cast<int>(pData.value<qint32>());
        break;
    case 5: // BLOCK_TYPE_UINT32
        return static_cast<int>(pData.value<quint32>());
        break;
    case 6: // BLOCK_TYPE_INT64
        return static_cast<int>(pData.value<qint64>());
        break;
    case 7: // BLOCK_TYPE_UINT64
        return static_cast<int>(pData.value<quint64>());
        break;
    }
    return -1;
}

void BlockValue::SetName(QString name) {
    pName = name;
}

QString BlockValue::Name() const {
    return pName;
}

void BlockValue::SetBlockType(BLOCK_TYPE blockType) {
    pBlockType = blockType;
}

BLOCK_TYPE BlockValue::BlockType() const {
    return pBlockType;
}

void BlockValue::SetValue(qint8 val) {
    pBlockType = BLOCK_TYPE_INT8;
    pData = QVariant::fromValue(val);
}

void BlockValue::SetValue(quint8 val) {
    pBlockType = BLOCK_TYPE_UINT8;
    pData = QVariant::fromValue(val);
}

void BlockValue::SetValue(qint16 val) {
    pBlockType = BLOCK_TYPE_INT16;
    pData = QVariant::fromValue(val);
}

void BlockValue::SetValue(quint16 val) {
    pBlockType = BLOCK_TYPE_UINT16;
    pData = QVariant::fromValue(val);
}

void BlockValue::SetValue(qint32 val) {
    pBlockType = BLOCK_TYPE_INT32;
    pData = QVariant::fromValue(val);
}

void BlockValue::SetValue(quint32 val) {
    pBlockType = BLOCK_TYPE_UINT32;
    pData = QVariant::fromValue(val);
}

void BlockValue::SetValue(qint64 val) {
    pBlockType = BLOCK_TYPE_INT64;
    pData = QVariant::fromValue(val);
}

void BlockValue::SetValue(quint64 val) {
    pBlockType = BLOCK_TYPE_UINT64;
    pData = QVariant::fromValue(val);
}

QVariant BlockValue::Value() const {
    return pData;
}
