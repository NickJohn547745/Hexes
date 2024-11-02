#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hexviewer.h"
#include "ruledialog.h"
#include "deleteruledialog.h"
#include "hexeslogger.h"

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QHBoxLayout>
#include <QDockWidget>
#include <QMetaType>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void AddRecentFile(const QString fileName);

private slots:
    void pInitializeRecentFiles();
    void pAddRecentFile(QString fileName);
    void pClearRecentFiles();
    void pClearActions();
    void pSetBaseDir(QString baseDir);

protected:
    void showEvent(QShowEvent* event) override;

private:
    Ui::MainWindow *ui;
    HexViewer *pViewer;
    RuleDialog *pRuleDialog;
    DeleteRuleDialog *pDelRuleDialog;
    QString pSettingsName;
    QStringList pRecentFiles;
    QVector<QAction*> pActions;
    QString pBaseDir;
};
#endif // MAINWINDOW_H
