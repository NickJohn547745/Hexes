#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include "rule.h"
#include "blockvalue.h"

#include <QAbstractScrollArea>
#include <QPainter>
#include <QScrollBar>
#include <QPaintEvent>
#include <QResizeEvent>
#include <QTimer>
#include <QQueue>
#include <QFile>
#include <QFileDialog>
#include <QSettings>
#include <QMetaType>

class HexViewer : public QAbstractScrollArea
{
    Q_OBJECT

public:
    explicit HexViewer(QWidget *parent = nullptr);
    ~HexViewer();

    void SetFileName(const QString fileName);

    QStringList RuleNames();
    void RunRules();

    void DeclareVar();
    void SetVar(QString varName, QVariant value);
    QVariant GetVar(QString varName);
    QMap<QString, BlockValue> GetVars();

    QString Text();
    QString Hex();

public slots:
    void AddRule(const Rule rule);
    void AddRules(const QVector<Rule> rules);
    void DeleteRule(const Rule rule);
    void DeleteRuleByName(const QString ruleName);
    void PopRule();
    void ClearRules();

    void SaveRules();
    void OpenRules();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

    QSize sizeHint() const override;

signals:
    void RuleNamesChanged(QStringList ruleNames);
    void VarsChanged(QMap<QString, BlockValue> vars);

private:
    QString pFileName;
    QString pHex;
    QString pText;
    QRectF pViewRect;
    QFile pFile;
    QMap<QString, BlockValue> pVars;

    int pScrollValue;
    int pCursorPosition;   // Cursor position in the text
    bool pCursorVisible;   // Flag for cursor blinking
    int pSelectionStart;   // Selection start index
    int pSelectionEnd;     // Selection end index
    QTimer *pBlinkTimer;   // Timer for blinking cursor
    QQueue<Rule> pRules;

    QString pCleanHex(const QString &text);
    QString pStringToHex(const QString &text);
    void pUpdateScrollBar();

    void pPaintCursor(QPainter &painter);
    void pPaintSelection(QPainter &painter);
    void pPaintHeaders(QPainter &painter);
    void pPaintHex(QPainter &painter);
    void pPaintText(QPainter &painter);
    void pPaintOffsetColumn(QPainter &painter);
    void pPaintRules(QPainter &painter);
    void pPaintHexRule(QPainter &painter, int &currentXPos,
                       int &currentYPos, int ruleLen, bool skip = false);
    void pPaintTextRule(QPainter &painter, int &currentXPos,
                        int &currentYPos, int ruleLen, bool skip = false);

    void pSeekFile();

    int pCalcContentHeight() const;
    int pCalcCharCount() const;
    int pCalcLineCount() const;
    QRectF pCalcPaintRect() const;
    QRectF pCalcTextRect() const;
    QRectF pCalcHexRect() const;
    QRectF pCalcHexHeaderRect() const;
    int pCalcCursorLine();
    QPoint pCalcCursorPosition();
    QRectF pCalcOffsetColumnRect();
    QRectF pCalcOffsetHeaderRect();
    QRectF pCalcRuleHeaderRect();
    QRectF pCalcTextHeaderRect();
    QRectF pCalcRuleRect();
    QString pInsertNewlinesAtIntervals(const QString &input, int interval);
};

#endif // HEXVIEWER_H
