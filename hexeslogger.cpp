#include "hexeslogger.h"

HexesLogger *HexesLogger::pInstance = nullptr;
QWidget *HexesLogger::pParent = nullptr;

void HexesLogger::MakeInstance(QWidget *aParent) {
    pInstance = new HexesLogger(aParent);
}

void HexesLogger::HexesInfo(const QString pInfoMsg) {
    QMessageBox::information(pParent,
                             HEXES_INFO_TITLE,
                             QString(HEXES_INFO_TEMPLATE).arg(pInfoMsg));
}

void HexesLogger::HexesError(int pErrorId, const QString pErrorMsg) {
    QString errorIdString = QString::number(pErrorId);
    QMessageBox::warning(pParent,
                             HEXES_ERROR_TITLE,
                             QString(HEXES_ERROR_TEMPLATE).arg(errorIdString, pErrorMsg));
}

void HexesLogger::HexesFatal(int pFatalId, const QString pFatalMsg) {
    QString fatalIdString = QString::number(pFatalId);
    QMessageBox::critical(pParent,
                             HEXES_FATAL_TITLE,
                             QString(HEXES_FATAL_TEMPLATE).arg(fatalIdString, pFatalMsg));
}

void HexesLogger::DeleteInstance() {
    delete pInstance;
}

HexesLogger::HexesLogger(QWidget *aParent) {
    pParent = aParent;
}

HexesLogger* HexesLogger::Instance() {
    if (!pInstance) {
        MakeInstance();
    }
    return pInstance;
}
