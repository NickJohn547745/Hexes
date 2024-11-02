#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow) {
    HexesLogger::MakeInstance(this);

    ui->setupUi(this);

    pSettingsName = "mainwindow.ini";
    pRecentFiles = QStringList();
    pActions = QVector<QAction*>();
    pBaseDir = "C:/";

    pViewer = new HexViewer(this);
    setCentralWidget(pViewer);
    //pViewer->SetFileName("C:/Ext/Projects/Qt/Hexes/nazi_zombie_factory_patch.zone");

    //ui->statusbar->showMessage(QString("File Size: %1 bytes").arg(data.size()));

    pRuleDialog = new RuleDialog(this);
    pRuleDialog->SetRuleNames(pViewer->RuleNames());
    connect(ui->actionAdd_Rule, &QAction::triggered, pRuleDialog, &RuleDialog::exec);
    connect(pRuleDialog, &RuleDialog::RuleChanged, pViewer, &HexViewer::AddRule);
    connect(pRuleDialog, &RuleDialog::RulesChanged, pViewer, &HexViewer::AddRules);
    connect(pViewer, &HexViewer::RuleNamesChanged, pRuleDialog, &RuleDialog::SetRuleNames);
    connect(pViewer, &HexViewer::VarsChanged, pRuleDialog, &RuleDialog::SetVars);

    connect(pViewer, &HexViewer::BaseDirChanged, pRuleDialog, &RuleDialog::SetBaseDir);
    connect(pRuleDialog, &RuleDialog::BaseDirChanged, pViewer, &HexViewer::SetBaseDir);
    connect(pViewer, &HexViewer::BaseDirChanged, this, &MainWindow::pSetBaseDir);
    connect(pRuleDialog, &RuleDialog::BaseDirChanged, this, &MainWindow::pSetBaseDir);

    pDelRuleDialog = new DeleteRuleDialog(this);
    pDelRuleDialog->SetRuleNames(pViewer->RuleNames());
    connect(pViewer, &HexViewer::RuleNamesChanged, pDelRuleDialog, &DeleteRuleDialog::SetRuleNames);
    connect(ui->actionDelete_Rule, &QAction::triggered, pDelRuleDialog, &RuleDialog::exec);
    connect(pDelRuleDialog, &DeleteRuleDialog::RuleDeleted, pViewer, &HexViewer::DeleteRuleByName);

    connect(ui->actionClear_Rules, &QAction::triggered, pViewer, &HexViewer::ClearRules);
    connect(ui->actionPop_Rule, &QAction::triggered, pViewer, &HexViewer::PopRule);
    connect(ui->actionRun_Rules, &QAction::triggered, pViewer, &HexViewer::ParseRules);
    connect(ui->actionOpen, &QAction::triggered, pViewer, &HexViewer::OpenFile);
    connect(ui->actionSave_Rule_File, &QAction::triggered, pViewer, &HexViewer::SaveRules);
    connect(ui->actionOpen_Rule_File, &QAction::triggered, pViewer, &HexViewer::OpenRules);
    connect(pViewer, &HexViewer::FileOpened, this, &MainWindow::pAddRecentFile);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::close);

    QSettings HexesSettings(pSettingsName, QSettings::IniFormat);
    if (HexesSettings.contains("file/basedir")) {
        pBaseDir = HexesSettings.value("file/basedir").toString();
        pViewer->SetBaseDir(pBaseDir);
        pRuleDialog->SetBaseDir(pBaseDir);
    }
}

void MainWindow::pClearActions() {
    for (int i = 0; i < pActions.size(); i++) {
        delete pActions[i];
    }
    pActions.clear();
}

void MainWindow::pSetBaseDir(QString baseDir) {
    pBaseDir = baseDir;
}

void MainWindow::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);

    pInitializeRecentFiles();
}

void MainWindow::pClearRecentFiles() {
    QMenu *recentFileMenu = ui->menuRecent_Files;
    recentFileMenu->clear();
    pRecentFiles.clear();

    pClearActions();

    pActions << new QAction("Clear Recents...", recentFileMenu);
    recentFileMenu->addAction(pActions.last());
    connect(pActions.last(), &QAction::triggered, this, &MainWindow::pClearRecentFiles);
}

void MainWindow::pAddRecentFile(QString fileName) {
    setWindowTitle(QString("Hexes - %1").arg(fileName));

    QMenu *recentFileMenu = ui->menuRecent_Files;
    recentFileMenu->clear();

    pRecentFiles.push_back(fileName);
    if (pRecentFiles.size() > 5) {
        pRecentFiles.pop_front();
    }

    QVector<QAction*> pActions;
    for (int i = 0; i < pRecentFiles.size(); i++) {
        QString recentFileName = pRecentFiles[i];
        pActions << new QAction(recentFileName, recentFileMenu);
    }
    recentFileMenu->addActions(pActions);
}

void MainWindow::pInitializeRecentFiles() {
    pClearActions();

    QMenu *recentFileMenu = ui->menuRecent_Files;
    QSettings HexesSettings(pSettingsName, QSettings::IniFormat);
    if (HexesSettings.contains("menu/recentfiles")) {
        pRecentFiles = HexesSettings.value("menu/recentfiles").toStringList();
        for (int i = 0; i < pRecentFiles.size(); i++) {
            QString recentFileName = pRecentFiles[i];
            pActions << new QAction(recentFileName, recentFileMenu);
            recentFileMenu->addAction(pActions.last());
            connect(pActions.last(), &QAction::triggered, this, [&]() {
                pViewer->SetFileName(recentFileName);
            });
        }
        recentFileMenu->addSeparator();
        pActions << new QAction("Clear Recents...", recentFileMenu);
        recentFileMenu->addAction(pActions.last());
        connect(pActions.last(), &QAction::triggered, this, &MainWindow::pClearRecentFiles);
    }
}

MainWindow::~MainWindow() {
    QSettings HexesSettings(pSettingsName, QSettings::IniFormat);
    HexesSettings.setValue("menu/recentfiles", QVariant::fromValue(pRecentFiles));
    HexesSettings.setValue("file/basedir", QVariant::fromValue(pBaseDir));

    pClearActions();
    HexesLogger::DeleteInstance();

    delete ui;
    delete pViewer;
    delete pRuleDialog;
}

