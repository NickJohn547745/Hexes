#ifndef BLOCKVALUE_H
#define BLOCKVALUE_H

#include <QObject>
#include <QVariant>

enum BLOCK_TYPE {
    BLOCK_TYPE_NONE = 0,
    BLOCK_TYPE_INT8 = 1,
    BLOCK_TYPE_UINT8 = 2,
    BLOCK_TYPE_INT16 = 3,
    BLOCK_TYPE_UINT16 = 4,
    BLOCK_TYPE_INT32 = 5,
    BLOCK_TYPE_UINT32 = 6,
    BLOCK_TYPE_INT64 = 7,
    BLOCK_TYPE_UINT64 = 8,
};

class BlockValue : public QObject
{
    Q_OBJECT
public:
    explicit BlockValue(QObject *parent = nullptr);
    BlockValue(const BlockValue &blockVal);

    BlockValue &operator=(const BlockValue &blockVal);

    void SetName(QString name);
    QString Name() const;

    void SetBlockType(BLOCK_TYPE blockType);
    BLOCK_TYPE BlockType() const;

    void SetValue(qint8 val);
    void SetValue(quint8 val);
    void SetValue(qint16 val);
    void SetValue(quint16 val);
    void SetValue(qint32 val);
    void SetValue(quint32 val);
    void SetValue(qint64 val);
    void SetValue(quint64 val);

    QVariant Value() const;
    int ValueToInt();

private:
    QString pName;
    BLOCK_TYPE pBlockType;
    QVariant pData;
};

#endif // BLOCKVALUE_H
