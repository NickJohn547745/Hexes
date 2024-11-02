#ifndef HEXSTREAM_H
#define HEXSTREAM_H

#include "rule.h"
#include "blockvalue.h"
#include "hexeslogger.h"

#include <QDataStream>

class HexStream : public QDataStream
{
public:
    HexStream();
    explicit HexStream(QIODevice *aIODevice);
    HexStream(QByteArray *aData, OpenMode aFlags);
    HexStream(const QByteArray &aData);
    ~HexStream();

    void SetHexByteOrder(BYTE_ORDER aByteOrder);
    BYTE_ORDER HexByteOrder();

    void ParseSkip(const Rule &aRule);
    void ParseSkip(int aSkipLen);
    BlockValue ParseInt8(const Rule &aRule);
    BlockValue ParseUInt8(const Rule &aRule);
    BlockValue ParseInt16(const Rule &aRule);
    BlockValue ParseUInt16(const Rule &aRule);
    BlockValue ParseInt32(const Rule &aRule);
    BlockValue ParseUInt32(const Rule &aRule);
    BlockValue ParseInt64(const Rule &aRule);
    BlockValue ParseUInt64(const Rule &aRule);
    BlockValue ParseRule(const Rule &aRule);

private:
    BYTE_ORDER pByteOrder;
};

#endif // HEXSTREAM_H
