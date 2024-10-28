#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaType>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    ui->setupUi(this);

    pViewer = new HexViewer(this);
    setCentralWidget(pViewer);
    pViewer->SetFileName("C:/Ext/Projects/Qt/Hexes/nazi_zombie_factory_patch.zone");

    //ui->statusbar->showMessage(QString("File Size: %1 bytes").arg(data.size()));

    pRuleDialog = new RuleDialog(this);
    pRuleDialog->SetRuleNames(pViewer->RuleNames());
    connect(ui->actionAdd_Rule, &QAction::triggered, pRuleDialog, &RuleDialog::exec);
    connect(pRuleDialog, &RuleDialog::RuleChanged, pViewer, &HexViewer::AddRule);
    connect(pRuleDialog, &RuleDialog::RulesChanged, pViewer, &HexViewer::AddRules);
    connect(pViewer, &HexViewer::RuleNamesChanged, pRuleDialog, &RuleDialog::SetRuleNames);
    connect(pViewer, &HexViewer::VarsChanged, pRuleDialog, &RuleDialog::SetVars);

    pDelRuleDialog = new DeleteRuleDialog(this);
    pDelRuleDialog->SetRuleNames(pViewer->RuleNames());
    connect(pViewer, &HexViewer::RuleNamesChanged, pDelRuleDialog, &DeleteRuleDialog::SetRuleNames);
    connect(ui->actionDelete_Rule, &QAction::triggered, pDelRuleDialog, &RuleDialog::exec);
    connect(pDelRuleDialog, &DeleteRuleDialog::RuleDeleted, pViewer, &HexViewer::DeleteRuleByName);

    connect(ui->actionClear_Rules, &QAction::triggered, pViewer, &HexViewer::ClearRules);
    connect(ui->actionPop_Rule, &QAction::triggered, pViewer, &HexViewer::PopRule);
    connect(ui->actionRun_Rules, &QAction::triggered, pViewer, &HexViewer::RunRules);
    connect(ui->actionSave, &QAction::triggered, pViewer, &HexViewer::SaveRules);
    connect(ui->actionOpen, &QAction::triggered, pViewer, &HexViewer::OpenRules);
}

MainWindow::~MainWindow() {
    delete ui;
    delete pViewer;
    delete pRuleDialog;
}

