#ifndef HEXVIEWER_H
#define HEXVIEWER_H

#include "rule.h"
#include "blockvalue.h"
#include "hexstream.h"
#include "hexeslogger.h"

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

const int DEFAULT_SINGLE_STEP = 45;
const int DEFAULT_PAGE_STEP = 45;

const int OFFSET_HEADER_WIDTH = 80;
const int MIN_PADDING = 5;
const int MAX_PADDING = 15;
const int HEADER_HEIGHT = 15;
const int STATUS_BAR_HEIGHT = 35;
const int FONT_HEIGHT = 15;

class HexViewer : public QAbstractScrollArea
{
    Q_OBJECT

public:
    explicit HexViewer(QWidget *parent = nullptr);
    ~HexViewer();

    void SetFileName(const QString fileName);

    QStringList RuleNames();
    void ParseRules();

    void DeclareVar();
    void SetVar(QString varName, QVariant value);
    QVariant GetVar(QString varName);
    QMap<QString, BlockValue> GetVars();

    void SetBaseDir(QString dir);

    QString Text();
    QString Hex();

    QString pFormatValuesGrid(const QVector<int>& values, int width, const QFont& font, int bitSize, int &textHeight);

public slots:
    void AddRule(const Rule rule);
    void AddRules(const QVector<Rule> rules);
    void DeleteRule(const Rule rule);
    void DeleteRuleByName(const QString ruleName);
    void PopRule();
    void ClearRules();

    void SaveRules();
    void OpenRules();
    void OpenFile();

protected:
    void resizeEvent(QResizeEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void showEvent(QShowEvent *event) override;

    QSize sizeHint() const override;

private slots:
    void pScrollValueChanged(int value);
    void pBlinkCursor();

signals:
    void RuleNamesChanged(QStringList ruleNames);
    void VarsChanged(QMap<QString, BlockValue> vars);
    void FileOpened(const QString aFileName);
    void BaseDirChanged(QString dir);

private:
    QString pFileName;
    QString pHex;
    QString pText;
    QRectF pViewRect;
    QFile pFile;
    QMap<QString, BlockValue> pVars;
    QFontMetrics pFontMetrics;
    QString pFileDir;

    int pScrollValue;
    int pCursorPosition;   // Cursor position in the text
    bool pCursorVisible;   // Flag for cursor blinking
    int pSelectionStart;   // Selection start index
    int pSelectionEnd;     // Selection end index
    QTimer *pBlinkTimer;   // Timer for blinking cursor
    QQueue<Rule> pRules;

    // Init, update, & utility functions
    void pInitCursorTimer();
    void pInitScrollBar();
    void pUpdateScrollBar();
    QString pCleanHex(const QString &text);
    QString pStringToHex(const QString &text);
    void pSeekFile();
    void pRefreshRules();

    // Cursor functions
    void pPaintCursor(QPainter &painter);
    int pCalcCursorLine() const;
    QPoint pCalcCursorPosition() const;

    // Selection functions
    void pPaintSelection(QPainter &painter);

    // Header functions
    void pPaintHeaders(QPainter &painter);
    QRectF pCalcHexHeaderRect() const;
    QRectF pCalcOffsetHeaderRect() const;
    QRectF pCalcRuleHeaderRect() const;
    QRectF pCalcTextHeaderRect() const;

    // Hex content functions
    void pPaintHex(QPainter &painter);
    QRectF pCalcHexRect() const;

    // Text content functions
    void pPaintText(QPainter &painter);
    QRectF pCalcTextRect() const;

    // Offset functions
    void pPaintOffsetColumn(QPainter &painter);
    QRectF pCalcOffsetColumnRect() const;

    // Rule functions
    void pPaintRules(QPainter &painter);
    QRectF pCalcRuleRect() const;
    void pPaintHexRule(QPainter &painter, int &currentXPos,
                       int &currentYPos, int ruleLen, bool skip = false);
    void pPaintTextRule(QPainter &painter, int &currentXPos,
                        int &currentYPos, int ruleLen, bool skip = false);

    // Other calculations
    QRectF pCalcPaintRect() const;
    int pCalcContentHeight() const;
    int pCalcCharCount() const;
    int pCalcLineCount() const;
};

#endif // HEXVIEWER_H
