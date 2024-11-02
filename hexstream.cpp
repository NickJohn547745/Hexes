#include "hexstream.h"

HexStream::HexStream()
    : QDataStream() {

}

HexStream::HexStream(QIODevice *aIODevice)
    : QDataStream(aIODevice) {

}

HexStream::HexStream(QByteArray *aData, OpenMode aFlags)
    : QDataStream(aData, aFlags) {

}

HexStream::HexStream(const QByteArray &aData)
    : QDataStream(aData) {

}

HexStream::~HexStream() {

}

void HexStream::SetHexByteOrder(BYTE_ORDER aByteOrder) {
    pByteOrder = aByteOrder;
    if (aByteOrder == BYTE_ORDER_BE) {
        setByteOrder(BigEndian);
    } else {
        setByteOrder(BigEndian);
    }
}

BYTE_ORDER HexStream::HexByteOrder() {
    return pByteOrder;
}

void HexStream::ParseSkip(const Rule &aRule) {
    // Skip hex bytes if valid
    skipRawData(aRule.Length());
}

void HexStream::ParseSkip(int aSkipLen) {
    // Skip hex 'aSkipLen' bytes
    skipRawData(aSkipLen);
}

BlockValue HexStream::ParseInt8(const Rule &aRule) {
    qint8 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseUInt8(const Rule &aRule) {
    quint8 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseInt16(const Rule &aRule) {
    qint16 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseUInt16(const Rule &aRule) {
    quint16 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseInt32(const Rule &aRule) {
    qint32 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseUInt32(const Rule &aRule) {
    quint32 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseInt64(const Rule &aRule) {
    qint64 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseUInt64(const Rule &aRule) {
    quint64 val;
    operator >>(val);
    return BlockValue(aRule.Name(), val);
}

BlockValue HexStream::ParseRule(const Rule &aRule) {
    if (!aRule) {
        HexesLogger::HexesError(50, "Attempted to parse null rule!");
        return BlockValue("INVALID");
    }
    if (aRule.Name().isEmpty()) {
        HexesLogger::HexesError(55, "Attempted to parse rule w/ null name!");
        return BlockValue("INVALID");
    }
    if (aRule.Length() < 0) {
        HexesLogger::HexesError(60, "Attempted to rule w/ invalid length!");
        return BlockValue(aRule.Name());
    }

    switch ((int)aRule.Type()) {
    case 1: // Skip
        ParseSkip(aRule);
        break;
    case 2: // int8
        return ParseInt8(aRule);
        break;
    case 3: // uint8
        return ParseUInt8(aRule);
        break;
    case 4: // int16
        return ParseInt16(aRule);
        break;
    case 5: // uint16
        return ParseUInt16(aRule);
        break;
    case 6: // int32
        return ParseInt32(aRule);
        break;
    case 7: // uint32
        return ParseUInt32(aRule);
        break;
    case 8: // int64
        return ParseInt64(aRule);
        break;
    case 9: // uint64
        return ParseUInt64(aRule);
        break;
    }
    return BlockValue(aRule.Name());
}
