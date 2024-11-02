#include "blockvalue.h"

/*
 * Block Value default constructor
 */
BlockValue::BlockValue() {
    pName = "";
    pBlockType = BLOCK_TYPE_NONE;
    pData = QVariant::fromValue(0);
}

/*
 * Block Value copy constructor
 */
BlockValue::BlockValue(const BlockValue &aBlockVal) {
    pName = aBlockVal.Name();
    pBlockType = aBlockVal.BlockType();
    pData = aBlockVal.ToVariant();
}

/*
 * Block Value constructor w/ name
 */
BlockValue::BlockValue(const QString aName) {
    pName = aName;
    pBlockType = BLOCK_TYPE_NONE;
    pData = QVariant::fromValue(0);
}

/*
 * Block Value constructor w/ name & int8
 */
BlockValue::BlockValue(const QString aName, qint8 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_INT8;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value constructor w/ name & uint8
 */
BlockValue::BlockValue(const QString aName, quint8 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_UINT8;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value constructor w/ name & int16
 */
BlockValue::BlockValue(const QString aName, qint16 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_INT16;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value constructor w/ name & uint16
 */
BlockValue::BlockValue(const QString aName, quint16 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_UINT16;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value constructor w/ name & int32
 */
BlockValue::BlockValue(const QString aName, qint32 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_INT32;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value constructor w/ name & uint32
 */
BlockValue::BlockValue(const QString aName, quint32 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_UINT32;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value constructor w/ name & int64
 */
BlockValue::BlockValue(const QString aName, qint64 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_INT64;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value constructor w/ name & uint64
 */
BlockValue::BlockValue(const QString aName, quint64 aVal) {
    pName = aName;
    pBlockType = BLOCK_TYPE_UINT64;
    pData = QVariant::fromValue(aVal);
}

/*
 * Block Value = operator implementation
 */
BlockValue &BlockValue::operator=(const BlockValue &aBlockVal) {
    if (this == &aBlockVal) {
        return *this;
    }

    pBlockType = aBlockVal.BlockType();
    pName = aBlockVal.Name();
    pData = aBlockVal.ToVariant();

    return *this;
}

/*
 * Return Block Value as an integer
 */
int BlockValue::ToInt() const {
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

/*
 * Return Block Value as a string
 */
QString BlockValue::ToString() const {
    return QString::number(ToInt());
}

/*
 * Set Block Value type (uint8, int16, etc)
 */
QVariant BlockValue::ToVariant() const {
    return pData;
}

/*
 * Set Block Value's name
 */
void BlockValue::SetName(const QString aName) {
    pName = aName;
}

/*
 * Returns Block Value's name
 */
QString BlockValue::Name() const {
    return pName;
}

/*
 * Set Block Balue type (uint8, int16, etc)
 */
void BlockValue::SetBlockType(BLOCK_TYPE aBlockType) {
    pBlockType = aBlockType;
}

/*
 * Get Block Balue type (uint8, int16, etc)
 */
BLOCK_TYPE BlockValue::BlockType() const {
    return pBlockType;
}
