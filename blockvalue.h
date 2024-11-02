#ifndef BLOCKVALUE_H
#define BLOCKVALUE_H

#include <QObject>
#include <QVariant>

/*
 * BLOCK_TYPE enum
 *
 * Value types that the BlockVlaue class can
 * ingest and convert to/from.
 */
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

/*
 * BlockValue class
 *
 * Represents a variable value resulting from
 * parsing a block of data from the input.
 *
 * Constructed preferrably with a name and value.
 */

class BlockValue
{

public: /*** PUBLIC MEMBERS ***/
    /* Block Value Constructors */
    BlockValue();                                   // Default
    BlockValue(const BlockValue &aBlockVal);        // Copy
    BlockValue(const QString aName);                // Name
    BlockValue(const QString aName, qint8 aVal);    // Name + int8
    BlockValue(const QString aName, quint8 aVal);   // Name + uint8
    BlockValue(const QString aName, qint16 aVal);   // Name + int16
    BlockValue(const QString aName, quint16 aVal);  // Name + uint16
    BlockValue(const QString aName, qint32 aVal);   // Name + int32
    BlockValue(const QString aName, quint32 aVal);  // Name + uint32
    BlockValue(const QString aName, qint64 aVal);   // Name + int64
    BlockValue(const QString aName, quint64 aVal);  // Name + uint64

    /* Block Value Operators */
    BlockValue &operator=(const BlockValue &aBlockVal); // = operator

    /* Setters and Getters */
    void SetName(const QString aName);          // Set Name
    QString Name() const;                       // Get Name
    void SetBlockType(BLOCK_TYPE aBlockType);   // Set Block Value Type
    BLOCK_TYPE BlockType() const;               // Get Block Value Type

    /* Data Access Methods */
    int ToInt() const;          // Convert to integer
    QString ToString() const;   // Convert to string
    QVariant ToVariant() const; // Return as raw data

private: /*** PRIVATE MEMBERS ***/
    QString pName;
    BLOCK_TYPE pBlockType;
    QVariant pData;
};

#endif // BLOCKVALUE_H
