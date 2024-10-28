#include "ruledialog.h"
#include "ui_ruledialog.h"

RuleDialog::RuleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RuleDialog) {
    ui->setupUi(this);

    pRuleNames = QStringList();
    pVars = QMap<QString, BlockValue>();
    pDefaultRuleCount = 1;
    pRules = QVector<Rule>();

    pHideLayout(ui->layout_Skip);
    ui->groupBox_2->hide();
    ui->groupBox_PreSkip->hide();
    ui->groupBox_PostSkip->hide();

    connect(ui->verticalScrollBar, &QScrollBar::valueChanged, ui->widget_RulePreview, &RulePreview::SetScrollValue);
    connect(ui->widget_RulePreview, &RulePreview::ScrollMaxChanged, ui->verticalScrollBar, &QScrollBar::setMaximum);

    connect(ui->pushButton_QueueRule, &QPushButton::clicked, this, &RuleDialog::pQueueRule);
    connect(ui->pushButton_Cancel, &QPushButton::clicked, this, &RuleDialog::pCancelDialog);
    connect(ui->pushButton_Clear, &QPushButton::clicked, this, &RuleDialog::pClearRules);
    connect(ui->pushButton_PopFirstRule, &QPushButton::clicked, this, &RuleDialog::pPopFirstRule);
    connect(ui->pushButton_PopLastRule, &QPushButton::clicked, this, &RuleDialog::pPopLastRule);
    connect(ui->pushButton_Save, &QPushButton::clicked, this, &RuleDialog::pSaveRules);
    connect(ui->comboBox_Type, &QComboBox::currentIndexChanged, this, &RuleDialog::pTypeChanged);

    connect(this, &RuleDialog::RulesChanged, ui->widget_RulePreview, &RulePreview::SetRules);

    connect(ui->groupBox_RepeatRule, &QGroupBox::toggled, this, &RuleDialog::pRepeatToggled);

    ui->comboBox_Vars->hide();
    connect(ui->radioButton_PrevVar, &QRadioButton::toggled, this, &RuleDialog::pPrevVarToggled);
    connect(ui->radioButton_StaticVal, &QRadioButton::toggled, this, &RuleDialog::pStaticValToggled);

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
    int index = ui->comboBox_Type->currentIndex();
    RULE_TYPE type = (RULE_TYPE)index;
    if (type == TYPE_NONE) { return; }

    int repeatCount = 0;
    int preSkip = 0;
    int postSkip = 0;
    if (ui->groupBox_RepeatRule->isChecked()) {
        if (ui->radioButton_PrevVar->isChecked()) {
            QString itemText = ui->comboBox_Vars->currentText();
            const QString leadingNumber = itemText.split('.').first() + ".";
            const QString varText = itemText.replace(leadingNumber, "")
                    .split('(').first().trimmed();
            BlockValue blockVal = pVars[varText];
            repeatCount = blockVal.ValueToInt();
        } else if (ui->radioButton_StaticVal->isChecked()) {
            repeatCount = ui->spinBox_2->value();
        }
    }
    QString name = ui->lineEdit_Name->text();
    if (name == "") { return; }

    if (ui->groupBox_PreSkip->isChecked()) {
        if (ui->radioButton_Pre_StaticVal->isChecked()) {
            preSkip = ui->spinBox_Pre_StaticVal->value();
        } else {
            QString itemText = ui->comboBox_Pre_Vars->currentText();
            const QString leadingNumber = itemText.split('.').first() + ".";
            const QString varText = itemText.replace(leadingNumber, "")
                    .split('(').first().trimmed();
            BlockValue blockVal = pVars[varText];
            preSkip = blockVal.ValueToInt();
        }
    }

    if (ui->groupBox_PostSkip->isChecked()) {
        if (ui->radioButton_Post_StaticVal->isChecked()) {
            postSkip = ui->spinBox_Post_StaticVal->value();
        } else {
            QString itemText = ui->comboBox_Post_Vars->currentText();
            const QString leadingNumber = itemText.split('.').first() + ".";
            const QString varText = itemText.replace(leadingNumber, "")
                    .split('(').first().trimmed();
            BlockValue blockVal = pVars[varText];
            postSkip = blockVal.ValueToInt();
        }
    }

    QString ruleName = name;

    BYTE_ORDER byteOrder = BYTE_ORDER_NONE;
    if (ui->radioButton_BE->isChecked()) {
        byteOrder = BYTE_ORDER_BE;
    } else {
        byteOrder = BYTE_ORDER_LE;
    }

    Rule rule;
    rule.SetName(ruleName);
    rule.SetType(type);
    rule.SetByteOrder(byteOrder);
    rule.SetRepeatCount(repeatCount);
    rule.SetPreSkipCount(preSkip);
    rule.SetPostSkipCount(postSkip);

    if (index == 1) { // skip
        rule.SetLength(ui->spinBox->value());
    }

    pRules.push_back(rule);
    ui->widget_RulePreview->SetRules(pRules);
}

void RuleDialog::pSaveRules() {
    emit RulesChanged(pRules);
    close();
}

void RuleDialog::pClearRules() {
    pRules.clear();
    ui->widget_RulePreview->SetRules(pRules);
}

void RuleDialog::pPopFirstRule() {
    pRules.pop_front();
    ui->widget_RulePreview->SetRules(pRules);
}

void RuleDialog::pPopLastRule() {
    pRules.pop_back();
    ui->widget_RulePreview->SetRules(pRules);
}

void RuleDialog::pCancelDialog() {
    pDefaultRuleCount--;
    close();
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
    pHideLayout(ui->layout_Skip);
    pShowLayout(ui->layout_ByteOrder);

    switch (index) {
    case 0: // none
        break;
    case 1: // skip
        pShowLayout(ui->layout_Skip);
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

void RuleDialog::pRepeatToggled(bool toggled) {
    ui->groupBox_PreSkip->setVisible(toggled);
    ui->groupBox_PostSkip->setVisible(toggled);
}

void RuleDialog::pPrevVarToggled(bool toggled) {
    ui->comboBox_Vars->setVisible(toggled);
    ui->spinBox_2->setVisible(!toggled);
}

void RuleDialog::pStaticValToggled(bool toggled) {
    ui->comboBox_Vars->setVisible(!toggled);
    ui->spinBox_2->setVisible(toggled);
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
    pDefaultRuleCount++;

    const QString defaultName = QString("RULE %1").arg(pDefaultRuleCount);
    ui->lineEdit_Name->setText(defaultName);
    ui->comboBox_Type->setCurrentIndex(0);

    ui->radioButton_LE->setChecked(true);
    ui->spinBox->setValue(1);

    ui->radioButton_StaticVal->setChecked(true);
    ui->spinBox_2->setValue(1);
    ui->comboBox_Vars->setCurrentIndex(0);

    pRules.clear();
    ui->widget_RulePreview->SetRules(pRules);

    QDialog::closeEvent(event);
}

Rule RuleDialog::GetRule() {
    return pRule;
}

void RuleDialog::SetRule(const Rule rule) {
    ui->lineEdit_Name->setText(rule.Name());

    if (rule.Type() == TYPE_SKIP) {
        ui->spinBox->setValue(rule.Length());
    }
}

void RuleDialog::SetRuleNames(QStringList ruleNames) {
    pRuleNames = ruleNames;
}

void RuleDialog::SetVars(QMap<QString, BlockValue> vars) {
    ui->comboBox_Vars->clear();
    ui->comboBox_Vars->addItem("Select Variable");

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
                .arg(QString::number(i), varName, varVal.Value().toString());
        ui->comboBox_Vars->addItem(itemName);
        ui->comboBox_Pre_Vars->addItem(itemName);
        ui->comboBox_Post_Vars->addItem(itemName);
    }
    pVars = vars;
}
