#ifndef HEXESLOGGER_H
#define HEXESLOGGER_H

#include <QWidget>
#include <QMessageBox>

const QString HEXES_INFO_TITLE = "Hexes Info Message";
const QString HEXES_ERROR_TITLE = "Hexes Error Message";
const QString HEXES_FATAL_TITLE = "Hexes Fatal Message";
const QString HEXES_INFO_TEMPLATE = "Hexes Info: %1";
const QString HEXES_ERROR_TEMPLATE = "Hexes Error [%1]: %2";
const QString HEXES_FATAL_TEMPLATE = "Hexes Fatal [%1]: %2";

class HexesLogger
{
public:
    static HexesLogger* Instance();
    static void DeleteInstance();
    static void MakeInstance(QWidget *aParent = nullptr);

    static void HexesInfo(const QString pInfoMsg);
    static void HexesError(int pErrorId, const QString pErrorMsg);
    static void HexesFatal(int pFatalId, const QString pFatalMsg);

private:
    HexesLogger(QWidget *aParent = nullptr);

    static QWidget *pParent;
    static HexesLogger *pInstance;
};

#endif // HEXESLOGGER_H
