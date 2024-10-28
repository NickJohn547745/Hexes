#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "hexviewer.h"
#include "ruledialog.h"
#include "deleteruledialog.h"

#include <QMainWindow>
#include <QFile>
#include <QDebug>
#include <QHBoxLayout>
#include <QDockWidget>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    HexViewer *pViewer;
    RuleDialog *pRuleDialog;
    DeleteRuleDialog *pDelRuleDialog;
};
#endif // MAINWINDOW_H
