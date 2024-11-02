#include "ruledialog.h"
#include "ui_ruledialog.h"

RuleDialog::RuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RuleDialog) {
    ui->setupUi(this);

    pRuleNames = QStringList();
    pVars = QMap<QString, BlockValue>();
    pDefaultRuleCount = 1;
    pRules = QQueue<Rule>();
    pSnippetDir = "C:/";

    ui->toolBox->setCurrentIndex(0);

    ui->groupBox_SkipOptions->hide();
    pHideLayout(ui->layout_ByteOrder);

    ui->label_Pre_SkipOffset->hide();
    ui->spinBox_Pre_Offset->hide();

    ui->label_SkipOffset->hide();
    ui->spinBox_Skip_Offset->hide();

    ui->label_Post_SkipOffset->hide();
    ui->spinBox_Post_Offset->hide();

    ui->label_RepeatOffset->hide();
    ui->spinBox_Repeat_Offset->hide();

    connect(ui->checkBox_PreSkip_Offset, &QCheckBox::toggled, this, [this](bool toggled) {
        ui->label_Pre_SkipOffset->setVisible(toggled);
        ui->spinBox_Pre_Offset->setVisible(toggled);
    });
    connect(ui->checkBox_Skip_Offset, &QCheckBox::toggled, this, [this](bool toggled) {
        ui->label_SkipOffset->setVisible(toggled);
        ui->spinBox_Skip_Offset->setVisible(toggled);
    });
    connect(ui->checkBox_PostSkip_Offset, &QCheckBox::toggled, this, [this](bool toggled) {
        ui->label_Post_SkipOffset->setVisible(toggled);
        ui->spinBox_Post_Offset->setVisible(toggled);
    });
    connect(ui->checkBox_Repeat_Offset, &QCheckBox::toggled, this, [this](bool toggled) {
        ui->label_RepeatOffset->setVisible(toggled);
        ui->spinBox_Repeat_Offset->setVisible(toggled);
    });

    connect(ui->verticalScrollBar, &QScrollBar::valueChanged, ui->widget_RulePreview, &RulePreview::SetScrollValue);
    connect(ui->widget_RulePreview, &RulePreview::ScrollMaxChanged, ui->verticalScrollBar, &QScrollBar::setMaximum);

    connect(ui->pushButton_QueueRule, &QPushButton::clicked, this, &RuleDialog::pQueueRule);
    connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &RuleDialog::pCancelDialog);
    connect(ui->pushButton_Clear, &QPushButton::clicked, this, &RuleDialog::pClearRules);
    connect(ui->pushButton_PopFirstRule, &QPushButton::clicked, this, &RuleDialog::pPopFirstRule);
    connect(ui->pushButton_PopLastRule, &QPushButton::clicked, this, &RuleDialog::pPopLastRule);
    connect(ui->pushButton_Save, &QPushButton::clicked, this, &RuleDialog::pSaveRules);
    connect(ui->comboBox_Type, &QComboBox::currentIndexChanged, this, &RuleDialog::pTypeChanged);

    connect(ui->pushButton_Import, &QPushButton::clicked, this, &RuleDialog::pImportSnippet);
    connect(ui->pushButton_Export, &QPushButton::clicked, this, &RuleDialog::pExportSnippet);

    connect(this, &RuleDialog::RulesChanged, ui->widget_RulePreview, &RulePreview::SetRules);

    ui->comboBox_Repeat_Vars->hide();
    connect(ui->radioButton_Repeat_PrevVar, &QRadioButton::toggled, this, &RuleDialog::pRepeat_PrevVarToggled);
    connect(ui->radioButton_Repeat_StaticVal, &QRadioButton::toggled, this, &RuleDialog::pRepeat_StaticValToggled);

    ui->comboBox_Skip_Vars->hide();
    connect(ui->radioButton_Skip_PrevVar, &QRadioButton::toggled, this, &RuleDialog::pSkip_PrevVarToggled);
    connect(ui->radioButton_Skip_StaticVal, &QRadioButton::toggled, this, &RuleDialog::pSkip_StaticValToggled);

    ui->comboBox_Pre_Vars->hide();
    connect(ui->radioButton_Pre_StaticVal, &QRadioButton::toggled, this, &RuleDialog::pPre_StaticValToggled);
    connect(ui->radioButton_Pre_PrevVar, &QRadioButton::toggled, this, &RuleDialog::pPre_PrevVarToggled);

    ui->comboBox_Post_Vars->hide();
    connect(ui->radioButton_Post_StaticVal, &QRadioButton::toggled, this, &RuleDialog::pPost_StaticValToggled);
    connect(ui->radioButton_Post_PrevVar, &QRadioButton::toggled, this, &RuleDialog::pPost_PrevVarToggled);
}

RuleDialog::~RuleDialog() {
    delete ui;
}

void RuleDialog::pQueueRule() {
    QString name = ui->lineEdit_Name->text();
    if (name == "") { return; }

    RULE_TYPE type = (RULE_TYPE)ui->comboBox_Type->currentIndex();

    Rule rule;
    rule.SetName(name);
    rule.SetType(type);

    if (type == TYPE_NONE) {
        return;
    } else if (type == TYPE_SKIP) {
        if (ui->radioButton_Skip_PrevVar->isChecked()) {
            QString itemText = ui->comboBox_Skip_Vars->currentText();
            const QString leadingNumber = itemText.split('.').first() + ".";
            const QString varText = itemText.replace(leadingNumber, "")
                    .split('(').first().trimmed();
            BlockValue blockVal = pVars[varText];
            rule.SetLength(blockVal.ToInt());
        } else if (ui->radioButton_Skip_StaticVal->isChecked()) {
            rule.SetLength(ui->spinBox_Skip_StaticVal->value());
        }
    }

    if (ui->radioButton_Repeat_StaticVal->isChecked()) {
        rule.SetRepeatCount(ui->spinBox_Repeat_StaticVal->value());
    } else {
        QString itemText = ui->comboBox_Repeat_Vars->currentText();
        const QString leadingNumber = itemText.split('.').first() + ".";
        const QString varText = itemText.replace(leadingNumber, "")
                .split('(').first().trimmed();
        BlockValue blockVal = pVars[varText];
        rule.SetRepeatCount(blockVal.ToInt());
    }

    if (ui->groupBox_PreSkip->isChecked()) {
        if (ui->radioButton_Pre_StaticVal->isChecked()) {
            rule.SetPreSkipCount(ui->spinBox_Pre_StaticVal->value());
        } else {
            QString itemText = ui->comboBox_Pre_Vars->currentText();
            const QString leadingNumber = itemText.split('.').first() + ".";
            const QString varText = itemText.replace(leadingNumber, "")
                    .split('(').first().trimmed();
            BlockValue blockVal = pVars[varText];
            rule.SetPreSkipCount(blockVal.ToInt());
        }
    }

    if (ui->groupBox_PostSkip->isChecked()) {
        if (ui->radioButton_Post_StaticVal->isChecked()) {
            rule.SetPostSkipCount(ui->spinBox_Post_StaticVal->value());
        } else {
            QString itemText = ui->comboBox_Post_Vars->currentText();
            const QString leadingNumber = itemText.split('.').first() + ".";
            const QString varText = itemText.replace(leadingNumber, "")
                    .split('(').first().trimmed();
            BlockValue blockVal = pVars[varText];
            rule.SetPostSkipCount(blockVal.ToInt());
        }
    }

    if (ui->radioButton_BE->isChecked()) {
        rule.SetByteOrder(BYTE_ORDER_BE);
    } else {
        rule.SetByteOrder(BYTE_ORDER_LE);
    }

    rule.SetSkipOffset(ui->spinBox_Skip_Offset->value());
    rule.SetPreSkipOffset(ui->spinBox_Pre_Offset->value());
    rule.SetPostSkipOffset(ui->spinBox_Post_Offset->value());
    rule.SetRepeatOffset(ui->spinBox_Repeat_Offset->value());

    pRules.enqueue(rule);
    ui->widget_RulePreview->SetRules(pRules.toList());
}

void RuleDialog::pSaveRules() {
    emit RulesChanged(pRules.toList());
    close();
}

void RuleDialog::pClearRules() {
    pRules.clear();
    ui->widget_RulePreview->SetRules(pRules.toList());
}

void RuleDialog::pPopFirstRule() {
    pRules.pop_front();
    ui->widget_RulePreview->SetRules(pRules.toList());
}

void RuleDialog::pPopLastRule() {
    pRules.pop_back();
    ui->widget_RulePreview->SetRules(pRules.toList());
}

void RuleDialog::pCancelDialog() {
    pDefaultRuleCount--;
    close();
}

void RuleDialog::pImportSnippet() {
    QStringList filters;
    filters.append("Rule File (*.hrs)");
    filters.append("All Files(*.*)");

    QFileDialog importDialog(this);
    importDialog.setWindowTitle("Open Hexes Rule Snippet");
    importDialog.setFileMode(QFileDialog::ExistingFile);
    importDialog.setDirectory(pSnippetDir);
    importDialog.setAcceptMode(QFileDialog::AcceptOpen);
    importDialog.setNameFilters(filters);
    if (importDialog.exec() != QDialog::Accepted) {
        qDebug() << "Open snippet dialog was not accepted!";
        return;
    }

    QString snippetPath = importDialog.selectedFiles().first();
    QString tempSnippetPath = snippetPath;
    pSnippetDir = tempSnippetPath.replace(tempSnippetPath.split('/').last(), "");
    emit BaseDirChanged(pSnippetDir);

    QFile snippetFile(snippetPath);
    if (!snippetFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open snippet file!";
        return;
    }
    snippetFile.close();

    QSettings snippetImport(snippetPath, QSettings::IniFormat);
    pRules = snippetImport.value("rules").value<QQueue<Rule>>();
    ui->widget_RulePreview->SetRules(pRules.toList());
}

void RuleDialog::pExportSnippet() {
    QStringList filters;
    filters.append("Rule File (*.hrs)");
    filters.append("All Files(*.*)");

    QFileDialog exportDialog(this);
    exportDialog.setWindowTitle("Save Hexes Rule Snippet");
    exportDialog.setDirectory(pSnippetDir);
    exportDialog.setAcceptMode(QFileDialog::AcceptSave);
    exportDialog.setNameFilters(filters);
    if (exportDialog.exec() != QDialog::Accepted) {
        qDebug() << "Save snippet dialog was not accepted!";
        return;
    }

    QString snippetPath = exportDialog.selectedFiles().first();
    QString tempSnippetPath = snippetPath;
    pSnippetDir = tempSnippetPath.replace(tempSnippetPath.split('/').last(), "");
    emit BaseDirChanged(pSnippetDir);

    QFile snippetFile(snippetPath);
    if (!snippetFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to save snippet file!";
        return;
    }
    snippetFile.close();

    QSettings snippetExport(snippetPath, QSettings::IniFormat);
    snippetExport.setValue("rules", QVariant::fromValue(pRules));
}

void RuleDialog::pHideLayout(QLayout* layout) {
    if (!layout)
        return;

    for (int i = 0; i < layout->count(); ++i) {
        QWidget *widget = layout->itemAt(i)->widget();
        if (widget) {
            widget->hide();  // Hide each widget in the layout
        }
    }
}

void RuleDialog::pShowLayout(QLayout* layout) {
    if (!layout)
        return;

    for (int i = 0; i < layout->count(); ++i) {
        QWidget *widget = layout->itemAt(i)->widget();
        if (widget) {
            widget->show();  // Hide each widget in the layout
        }
    }
}

void RuleDialog::pTypeChanged(int index) {
    ui->groupBox_SkipOptions->hide();
    pShowLayout(ui->layout_ByteOrder);

    switch (index) {
    case 0: // none
        ui->groupBox_SkipOptions->hide();
        pHideLayout(ui->layout_ByteOrder);
        break;
    case 1: // skip
        ui->groupBox_SkipOptions->show();
        pHideLayout(ui->layout_ByteOrder);
        break;
    case 2: // int8 [1]
        break;
    case 3: // uint8 [1]
        break;
    case 4: // int16 [2]
        break;
    case 5: // uint16 [2]
        break;
    case 6: // int32 [3]
        break;
    case 7: // uint32 [3]
        break;
    case 8: // int64 [4]
        break;
    case 9: // uint64 [4]
        break;
    }
}

void RuleDialog::pSkip_PrevVarToggled(bool toggled) {
    ui->comboBox_Skip_Vars->setVisible(toggled);
    ui->spinBox_Skip_StaticVal->setVisible(!toggled);
}

void RuleDialog::pSkip_StaticValToggled(bool toggled) {
    ui->comboBox_Skip_Vars->setVisible(!toggled);
    ui->spinBox_Skip_StaticVal->setVisible(toggled);
}

void RuleDialog::pRepeat_PrevVarToggled(bool toggled) {
    ui->comboBox_Repeat_Vars->setVisible(toggled);
    ui->spinBox_Repeat_StaticVal->setVisible(!toggled);
}

void RuleDialog::pRepeat_StaticValToggled(bool toggled) {
    ui->comboBox_Repeat_Vars->setVisible(!toggled);
    ui->spinBox_Repeat_StaticVal->setVisible(toggled);
}

void RuleDialog::pPre_PrevVarToggled(bool toggled) {
    ui->comboBox_Pre_Vars->setVisible(toggled);
    ui->spinBox_Pre_StaticVal->setVisible(!toggled);
}

void RuleDialog::pPre_StaticValToggled(bool toggled) {
    ui->comboBox_Pre_Vars->setVisible(!toggled);
    ui->spinBox_Pre_StaticVal->setVisible(toggled);
}

void RuleDialog::pPost_PrevVarToggled(bool toggled) {
    ui->comboBox_Post_Vars->setVisible(toggled);
    ui->spinBox_Post_StaticVal->setVisible(!toggled);
}

void RuleDialog::pPost_StaticValToggled(bool toggled) {
    ui->comboBox_Post_Vars->setVisible(!toggled);
    ui->spinBox_Post_StaticVal->setVisible(toggled);
}

void RuleDialog::closeEvent(QCloseEvent *event) {
    ui->toolBox->setCurrentIndex(0);

    if (pRuleNames.contains(QString("RULE %1").arg(pDefaultRuleCount))) {
        pDefaultRuleCount++;
    }
    const QString defaultName = QString("RULE %1").arg(pDefaultRuleCount);
    ui->lineEdit_Name->setText(defaultName);

    ui->comboBox_Type->setCurrentIndex(0);

    ui->radioButton_LE->setChecked(true);

    ui->spinBox_Skip_StaticVal->setValue(1);
    ui->checkBox_Skip_Offset->setChecked(false);
    ui->label_SkipOffset->hide();
    ui->spinBox_Skip_Offset->hide();
    ui->radioButton_Skip_StaticVal->setChecked(true);
    ui->spinBox_Skip_StaticVal->setValue(0);
    ui->comboBox_Skip_Vars->setCurrentIndex(0);

    ui->groupBox_PreSkip->setChecked(false);
    ui->checkBox_PreSkip_Offset->setChecked(false);
    ui->radioButton_Pre_StaticVal->setChecked(true);
    ui->spinBox_Pre_StaticVal->show();
    ui->comboBox_Pre_Vars->hide();
    ui->spinBox_Pre_Offset->setValue(0);
    ui->spinBox_Pre_StaticVal->setValue(0);

    ui->groupBox_PostSkip->setChecked(false);
    ui->checkBox_PostSkip_Offset->setChecked(false);
    ui->radioButton_Post_StaticVal->setChecked(true);
    ui->spinBox_Post_StaticVal->show();
    ui->comboBox_Post_Vars->hide();
    ui->spinBox_Post_Offset->setValue(0);
    ui->spinBox_Post_StaticVal->setValue(0);

    ui->radioButton_Repeat_StaticVal->setChecked(true);
    ui->checkBox_Repeat_Offset->setChecked(false);
    ui->label_RepeatOffset->hide();
    ui->spinBox_Repeat_Offset->hide();
    ui->spinBox_Repeat_StaticVal->setValue(0);

    pRules.clear();
    ui->widget_RulePreview->SetRules(pRules.toList());

    QDialog::closeEvent(event);
}

Rule RuleDialog::GetRule() {
    return pRule;
}

void RuleDialog::SetRule(const Rule rule) {
    ui->lineEdit_Name->setText(rule.Name());

    if (rule.Type() == TYPE_SKIP) {
        ui->spinBox_Skip_StaticVal->setValue(rule.Length());
    }
}

void RuleDialog::SetBaseDir(QString dir) {
    pSnippetDir = dir;
}

void RuleDialog::SetRuleNames(QStringList ruleNames) {
    pRuleNames = ruleNames;
}

void RuleDialog::SetVars(QMap<QString, BlockValue> vars) {
    ui->comboBox_Repeat_Vars->clear();
    ui->comboBox_Repeat_Vars->addItem("Select Variable");

    ui->comboBox_Skip_Vars->clear();
    ui->comboBox_Skip_Vars->addItem("Select Variable");

    ui->comboBox_Pre_Vars->clear();
    ui->comboBox_Pre_Vars->addItem("Select Variable");

    ui->comboBox_Post_Vars->clear();
    ui->comboBox_Post_Vars->addItem("Select Variable");

    int i = 0;
    for (auto [key, value] : vars.asKeyValueRange()) {
        i++;
        QString varName = (QString)key;
        BlockValue varVal = (BlockValue)value;
        QString itemName = QString("%1. %2 (%3)")
                .arg(QString::number(i), varName, varVal.ToVariant().toString());
        ui->comboBox_Repeat_Vars->addItem(itemName);
        ui->comboBox_Skip_Vars->addItem(itemName);
        ui->comboBox_Pre_Vars->addItem(itemName);
        ui->comboBox_Post_Vars->addItem(itemName);
    }
    pVars = vars;
}
