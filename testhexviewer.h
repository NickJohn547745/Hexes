#ifndef TESTHEXVIEWER_H
#define TESTHEXVIEWER_H

#include "hexviewer.h"

#include <QTest>

class TestQString: public QObject
{
    Q_OBJECT

private slots:
    void pCalcCharCount();

private:
    HexViewer *pHexViewer;
};


#endif // TESTHEXVIEWER_H
