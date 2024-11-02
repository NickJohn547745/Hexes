#include "hexviewer.h"

/*
 * HexViewer Constructor
 */
HexViewer::HexViewer(QWidget *parent)
    : QAbstractScrollArea(parent)
    , pText("")
    , pHex("")
    , pFileName("")
    , pViewRect(0, 0, 800, 557)
    , pBlinkTimer(new QTimer(this))
    , pRules(QQueue<Rule>())
    , pScrollValue(0)
    , pVars(QMap<QString, BlockValue>())
    , pFontMetrics(QFontMetrics(QFont("CommitMono", 10)))
    , pFileDir("C:/"){
    // Set font to monospaced option
    setFont(QFont("CommitMono", 10));
    // Initialize scroll bar and cursor timer
    pInitScrollBar();
    pInitCursorTimer();
}

/*
 * HexViewer Destructor
 */
HexViewer::~HexViewer() {
    delete pBlinkTimer;
}

/*
 * Scroll Bar Initialization
 */
void HexViewer::pInitScrollBar() {
    verticalScrollBar()->setSingleStep(DEFAULT_SINGLE_STEP);
    verticalScrollBar()->setPageStep(DEFAULT_PAGE_STEP);
    connect(verticalScrollBar(), &QScrollBar::valueChanged,
            this, &HexViewer::pScrollValueChanged);
}

/*
 * Cursor Blink Timer Initialization
 */
void HexViewer::pInitCursorTimer() {
    connect(pBlinkTimer, &QTimer::timeout,
            this, &HexViewer::pBlinkCursor);
    pBlinkTimer->start(500);  // Blinking interval
}

/*
 * Set filename of hex file we want to open
 */
void HexViewer::SetFileName(const QString fileName) {
    if (fileName.isEmpty()) { return; }
    // Set file name & open for read-only
    pFile.setFileName(pFileName = fileName);
    if (!pFile.open(QIODevice::ReadOnly)) {
        const QString errorMsg("Failed to open " + pFileName);
        HexesLogger::HexesError(20, errorMsg);
        return;
    }
    // Re-search the file contents to display
    pSeekFile();
}

/*
 * Add a rule to our local queue
 */
void HexViewer::AddRule(const Rule rule) {
    pRules.enqueue(rule);
    // Refresh rules and view
    pRefreshRules();
}

/*
 * Adds multiple rules to our local queue
 */
void HexViewer::AddRules(const QVector<Rule> rules) {
    if (rules.isEmpty()) { return; }
    // Add rules from incoming queue
    for (int i = 0; i < rules.size(); i++) {
        pRules.enqueue(rules[i]);
    }
    // Refresh rules and view
    pRefreshRules();
}

/*
 * Remove a rule from our local queue
 */
void HexViewer::DeleteRule(const Rule rule) {
    if (!pRules.contains(rule)) { return; }
    if (pRules.size() == 0) { return; }
    // Search for and remove rule
    for (int i = 0; i < pRules.size(); i++) {
        Rule currentRule = pRules.dequeue();
        if (currentRule != rule) {
            pRules.enqueue(currentRule);
        }
    }
    // Refresh rules and view
    pRefreshRules();
}

/*
 * Remove a rule, by name, from our local queue
 */
void HexViewer::DeleteRuleByName(const QString ruleName) {
    if (pRules.size() == 0) { return; }
    // Search for and remove rule
    for (int i = 0; i < pRules.size(); i++) {
        Rule currentRule = pRules.dequeue();
        if (currentRule.Name() != ruleName) {
            pRules.enqueue(currentRule);
        }
    }
    // Refresh rules and view
    pRefreshRules();
}

/*
 * Pop/dequeue a rule from our local queue
 */
void HexViewer::PopRule() {
    if (pRules.isEmpty()) { return; }
    pRules.dequeue();
    // Refresh rules and view
    pRefreshRules();
}

/*
 * Clear all rules from our local queue
 */
void HexViewer::ClearRules() {
    if (pRules.isEmpty()) { return; }
    pRules.clear();
    // Refresh rules and view
    pRefreshRules();
}

/*
 * Save rules to a Hexes Rule File (.hrf)
 */
void HexViewer::SaveRules() {
    // Have user choose where to save the .hrf
    QString filePath = QFileDialog::getSaveFileName(this, "Save Hexes Rule File",
                                                    pFileDir, "Rule File (*.hrf);;All Files(*.*)");
    QString tempFilePath = filePath;
    pFileDir = tempFilePath.replace(tempFilePath.split('/').last(), "");
    emit BaseDirChanged(pFileDir);

    // Open rule file, or report error, then close
    QFile ruleFile(filePath);
    if (!ruleFile.open(QIODevice::WriteOnly)) {
        HexesLogger::HexesError(25, "Failed to save rule file!");
        return;
    }
    ruleFile.close();
    // Write to rule file in ini format
    QSettings ruleExport(filePath, QSettings::IniFormat);
    ruleExport.setValue("rules", QVariant::fromValue(pRules));
}

/*
 * Open a Hexes Rule File (.hrf)
 */
void HexViewer::OpenRules() {
    // Have user choose where to save the .hrf
    QString filePath = QFileDialog::getOpenFileName(this, "Open Hexes Rule File",
                                                    pFileDir, "Rule File (*.hrf);;All Files(*.*)");
    QString tempFilePath = filePath;
    pFileDir = tempFilePath.replace(tempFilePath.split('/').last(), "");
    emit BaseDirChanged(pFileDir);

    // Open rule file, or report error, then close
    QFile ruleFile(filePath);
    if (!ruleFile.open(QIODevice::ReadOnly)) {
        HexesLogger::HexesError(30, "Failed to open rule file!");
        return;
    }
    ruleFile.close();
    // Read from rule file in ini format
    QSettings ruleExport(filePath, QSettings::IniFormat);
    pRules = ruleExport.value("rules").value<QQueue<Rule>>();
    // Notify & refresh rules and view
    pRefreshRules();
    emit FileOpened(filePath.split('/').last());
}

/*
 * Open a binary input file
 */
void HexViewer::OpenFile() {
    // Have user choose where to save the .hrf
    QString filePath = QFileDialog::getOpenFileName(this, "Open Input File",
                                                    pFileDir, "All Files(*.*)");

    QString tempFilePath = filePath;
    pFileDir = tempFilePath.replace(tempFilePath.split('/').last(), "");
    emit BaseDirChanged(pFileDir);

    SetFileName(filePath);
    // Notify & refresh rules and view
    pRefreshRules();
    emit FileOpened(filePath);
}

/*
 * Get a list of rule names from local queue
 */
QStringList HexViewer::RuleNames() {
    if (pRules.isEmpty()) { return QStringList(); }
    // Create and return name list
    QStringList ruleNames;
    foreach (Rule rule, pRules) {
        ruleNames.append(rule.Name());
    }
    return ruleNames;
}

/*
 * Re-parse rules and update the variables' values
 */
void HexViewer::ParseRules() {
    // Read in hex data as a byte-stream
    HexStream hexStream(QByteArray::fromHex(pHex.toUtf8()));
    // Iterate through local rules
    for (int i = 0; i < pRules.size(); i++) {
        Rule &rule = pRules[i]; // Current rule
        qDebug() << rule.Name();
        if ((rule.Type() == TYPE_NONE) || (rule.Type() == TYPE_SKIP)) {
            hexStream.ParseSkip(rule.Length() + rule.SkipOffset());
            continue;
        }
        // Repeat rule per defined repeat count
        for (int j = 0; j <= rule.RepeatCount() + rule.RepeatOffset(); j++) {
            // Set byte order and execute pre-skip
            hexStream.SetHexByteOrder(rule.ByteOrder());
            hexStream.ParseSkip(rule.PreSkipCount() + rule.PreSkipOffset());

            // Create parse block to get value for variable
            BlockValue val = hexStream.ParseRule(rule);
            rule.SetValue(val.ToString());

            // Execute post-skip
            hexStream.ParseSkip(rule.PostSkipCount() + rule.PostSkipOffset());

            // Add a var suffix (index) if repeating
            if (rule.RepeatCount() > 0) {
                pVars[rule.Name() + QString(" %1").arg(j)] = val;
            } else {
                pVars[rule.Name()] = val;
            }
        }
    }
    emit VarsChanged(pVars);
    update();
}

/*
 * Return the local variable map
 * - Note: May not be populated
 */
QMap<QString, BlockValue> HexViewer::GetVars() {
    return pVars;
}

void HexViewer::SetBaseDir(QString dir) {
    pFileDir = dir;
}

/*
 * Get the text representation of the data
 */
QString HexViewer::Text() {
    return pText;
}

/*
 * Get the hex representation of the data
 */
QString HexViewer::Hex() {
    return pHex;
}

QString HexViewer::pFormatValuesGrid(const QVector<int> &values, int width, const QFont &font, int bitSize, int& textHeight) {
    if (values.isEmpty()) return "";

    QFontMetrics fm(font);
    QString result;

    // Determine the width for the index based on the number of digits in the largest index
    int maxIndexDigits = QString::number(values.size()).length();

    // Determine padding width for values based on bit size (minimal necessary width)
    int valuePaddingWidth = 0;
    switch (bitSize) {
    case 8:  valuePaddingWidth = 3; break;  // int8 max is 255
    case 16: valuePaddingWidth = 5; break;  // int16 max is 65535
    case 32: valuePaddingWidth = 10; break; // int32 max is 4294967295
    case 64: valuePaddingWidth = 20; break; // int64 max is very large
    default: valuePaddingWidth = 10; break; // Default to int32-like width
    }

    // Calculate a compact maximum entry width (index + value) with minimal padding
    int maxEntryWidth = fm.horizontalAdvance(QString("%1=%2  ")
                                             .arg(values.size(), maxIndexDigits)
                                             .arg(0, valuePaddingWidth, 10, QChar('0')));

    // Determine the optimal number of entries per line based on available width
    int entriesPerLine = std::max(1, width / maxEntryWidth);

    // Build the grid-like output
    int lineCount = 0;
    for (int i = 0; i < values.size(); ++i) {
        if (i > 0 && i % entriesPerLine == 0) {
            result += "\n";
            lineCount++;
        } else if (i > 0) {
            result += ", ";  // Minimal spacing between entries
        }

        // Format each entry with a left-aligned index and right-aligned value
        result += QString("%1=%2")
                .arg(i + 1, maxIndexDigits, 10, QChar(' '))            // Left-pad index minimally
                .arg(values[i], -valuePaddingWidth, 10, QChar(' '));   // Right-pad value minimally
    }

    // Account for the final line
    lineCount++;

    // Calculate the height of the text block
    textHeight = lineCount * fm.height();

    return result;
}

/*
 * Calculate the hex header rectangle
 */
QRectF HexViewer::pCalcHexHeaderRect() const {
    qreal columnWidth = viewport()->width()
            - (OFFSET_HEADER_WIDTH + (3 * MAX_PADDING));
    qreal hexColumnWidth = (columnWidth / 5 * 3) - 2 * MAX_PADDING;

    return QRectF(OFFSET_HEADER_WIDTH + MAX_PADDING, MIN_PADDING,
                  hexColumnWidth, HEADER_HEIGHT);
}

/*
 * Calculate the text header rectangle
 */
QRectF HexViewer::pCalcTextHeaderRect() const {
    qreal columnWidth = viewport()->width()
            - (OFFSET_HEADER_WIDTH + (3 * MAX_PADDING));
    qreal hexColumnWidth = (columnWidth / 5 * 3) - MAX_PADDING;
    qreal textColumnWidth = (columnWidth / 5) + MAX_PADDING;

    return QRectF(OFFSET_HEADER_WIDTH + hexColumnWidth + MAX_PADDING, MIN_PADDING,
                  textColumnWidth - 2 * MAX_PADDING, MAX_PADDING);
}

/*
 * Calculate the rule header rectangle
 */
QRectF HexViewer::pCalcRuleHeaderRect() const {
    qreal columnWidth = viewport()->width()
            - (OFFSET_HEADER_WIDTH + (3 * MAX_PADDING));
    qreal hexColumnWidth = (columnWidth / 5 * 3);
    qreal textColumnWidth = (columnWidth / 5) + MAX_PADDING;
    qreal ruleColumnWidth = (columnWidth / 5) + MAX_PADDING;

    return QRectF(OFFSET_HEADER_WIDTH + hexColumnWidth + textColumnWidth, MIN_PADDING,
                  ruleColumnWidth - MAX_PADDING, MAX_PADDING);
}

/*
 * Calculate the offset header rectangle
 */
QRectF HexViewer::pCalcOffsetHeaderRect() const {
    return QRectF(MIN_PADDING, MIN_PADDING,
                  OFFSET_HEADER_WIDTH, HEADER_HEIGHT);
}

/*
 * Calculate the offset column rectangle
 */
QRectF HexViewer::pCalcOffsetColumnRect() const {
    return QRectF(MIN_PADDING, FONT_HEIGHT + HEADER_HEIGHT,
                  OFFSET_HEADER_WIDTH, height() - STATUS_BAR_HEIGHT);
}

/*
 * Paint the offset column
 */
void HexViewer::pPaintOffsetColumn(QPainter &painter) {
    QRectF offSetColumnRect = pCalcOffsetColumnRect();
    QRectF hexRect = pCalcHexRect();

    QString offset = "";
    int nextOffset = verticalScrollBar()->value();
    for (int i = 0; i < qCeil(hexRect.height() / fontMetrics().height()); i++) {
        offset += QString::number(nextOffset).rightJustified(8, '0') + " ";
        nextOffset += pCalcCharCount();
    }
    offset = offset.trimmed();

    painter.setPen(Qt::blue);
    painter.drawText(offSetColumnRect, Qt::TextWordWrap | Qt::AlignHCenter, offset);
}

/*
 * Calculate the hex content rectangle
 */
QRectF HexViewer::pCalcHexRect() const {
    qreal columnWidth = viewport()->width()
            - (OFFSET_HEADER_WIDTH + (3 * MAX_PADDING));
    qreal hexColumnWidth = (columnWidth / 5 * 3) - 2 * MAX_PADDING;

    return QRectF(OFFSET_HEADER_WIDTH + MAX_PADDING, HEADER_HEIGHT + FONT_HEIGHT,
                  hexColumnWidth, viewport()->height());
}

/*
 * Paint the hex content
 */
void HexViewer::pPaintHex(QPainter &painter) {
    QRectF hexRect = pCalcHexRect();

    painter.setPen(Qt::black);
    painter.drawText(hexRect, Qt::TextWordWrap, pHex);
}

/*
 * Calculate the text content rect
 */
QRectF HexViewer::pCalcTextRect() const {
    qreal columnWidth = viewport()->width()
            - (OFFSET_HEADER_WIDTH + (3 * MAX_PADDING));
    qreal hexColumnWidth = (columnWidth / 5 * 3);
    qreal textColumnWidth = (columnWidth / 5) - MAX_PADDING / 2;

    return QRectF(OFFSET_HEADER_WIDTH + hexColumnWidth, HEADER_HEIGHT + FONT_HEIGHT,
                  textColumnWidth, viewport()->height());
}

/*
 * Calculate rule sidebar rectangle
 */
QRectF HexViewer::pCalcRuleRect() const {
    qreal columnWidth = viewport()->width()
            - (OFFSET_HEADER_WIDTH + (3 * MAX_PADDING));
    qreal hexColumnWidth = (columnWidth / 5 * 3);
    qreal textColumnWidth = (columnWidth / 5) + MAX_PADDING;
    qreal ruleColumnWidth = (columnWidth / 5) + MAX_PADDING;

    return QRectF(OFFSET_HEADER_WIDTH + hexColumnWidth + textColumnWidth, HEADER_HEIGHT + FONT_HEIGHT,
                  ruleColumnWidth, viewport()->height());
}

/*
 * Paint the text content
 */
void HexViewer::pPaintText(QPainter &painter) {
    QRectF textRect = pCalcTextRect();

    painter.setPen(Qt::black);
    painter.drawText(textRect, Qt::TextWrapAnywhere, pText);
}

/*
 * Paint a hex rule over the content
 */
void HexViewer::pPaintHexRule(QPainter &painter, int &currentXPos,
                              int &currentYPos, int ruleLen, bool skip) {
    int charWidth = fontMetrics().horizontalAdvance("F");
    int charPerLine = pCalcCharCount();
    QRectF hexRect = pCalcHexRect();
    int scrollHeight = verticalScrollBar()->value();
    int adjustedYPos = currentYPos - scrollHeight;  // Apply scroll offset

    while (ruleLen > 0) {
        int remainingCharsOnLine = charPerLine - ((currentXPos - hexRect.left()) / (3 * charWidth));
        int charsToDraw = qMin(remainingCharsOnLine, ruleLen);
        int rectWidth = charsToDraw * 3 * charWidth;

        // Only draw if within the visible area vertically
        if (adjustedYPos >= hexRect.top() &&
                adjustedYPos <= (hexRect.bottom() - fontMetrics().height())) {

            QRectF skipRect(QPointF(currentXPos, adjustedYPos + 1),
                            QSizeF(rectWidth - charWidth, fontMetrics().height() - 2));
            painter.drawRect(skipRect);
            if (skip) {
                painter.drawLine(skipRect.topLeft(), skipRect.bottomRight());
                painter.drawLine(skipRect.topRight(), skipRect.bottomLeft());
            }
        }

        currentXPos += rectWidth;
        ruleLen -= charsToDraw;

        // Move to the next line if the current position exceeds the line width
        if (currentXPos >= hexRect.left() + charPerLine * 3 * charWidth) {
            currentXPos = hexRect.left();
            adjustedYPos += fontMetrics().height();
        }
    }

    // Restore the Y position accounting for scroll offset
    currentYPos = adjustedYPos + scrollHeight;
}

/*
 * Paint a text rule over the content
 */
void HexViewer::pPaintTextRule(QPainter &painter, int &currentXPos,
                               int &currentYPos, int ruleLen, bool skip) {
    int charWidth = fontMetrics().horizontalAdvance("F");
    int charPerLine = pCalcCharCount();
    QRectF textRect = pCalcTextRect();
    int scrollHeight = verticalScrollBar()->value();
    int adjustedYPos = currentYPos - scrollHeight;  // Apply scroll offset

    while (ruleLen > 0) {
        int remainingCharsOnLine = charPerLine - ((currentXPos - textRect.left()) / charWidth);

        // Ensure we do not run into a situation where remainingCharsOnLine becomes zero or negative
        if (remainingCharsOnLine <= 0) {
            currentXPos = textRect.left();
            adjustedYPos += fontMetrics().height();
            remainingCharsOnLine = charPerLine;
        }

        int charsToDraw = qMin(remainingCharsOnLine, ruleLen);

        // If charsToDraw is still 0, break out to avoid an infinite loop
        if (charsToDraw <= 0) break;

        int rectWidth = charsToDraw * charWidth;

        // Only draw if within the visible area vertically
        if (adjustedYPos >= textRect.top() &&
                adjustedYPos <= textRect.bottom()) {

            QRectF skipRect(QPointF(currentXPos, adjustedYPos + 1),
                            QSizeF(rectWidth, fontMetrics().height() - 2));
            painter.drawRect(skipRect);
            if (skip) {
                painter.drawLine(skipRect.topLeft(), skipRect.bottomRight());
                painter.drawLine(skipRect.topRight(), skipRect.bottomLeft());
            }
        }

        currentXPos += rectWidth;
        ruleLen -= charsToDraw;

        // Move to the next line if the current position exceeds the line width
        if (currentXPos >= textRect.left() + charPerLine * charWidth) {
            currentXPos = textRect.left();
            adjustedYPos += fontMetrics().height();
        }
    }

    // Restore the Y position accounting for scroll offset
    currentYPos = adjustedYPos + scrollHeight;
}

/*
 * Seek to a new file location, process
 * the contents, and update the view.
 */
void HexViewer::pSeekFile() {
    QRectF hexRect(pCalcHexRect());
    int dataLength = (hexRect.height() / fontMetrics().height()) * pCalcCharCount();

    if (dataLength < 0) {
        return;
    } else if (pFile.isOpen()) {
        pFile.seek(verticalScrollBar()->value());
        QByteArray data = pFile.read(dataLength + 2);
        pHex = pCleanHex(data.toHex());
        pText = QString::fromLocal8Bit(data).replace('\0', '.');//,  pCalcCharCount());
    } else {
        //HexesLogger::HexesError(35, QString("File '%1' not open...").arg(pFile.fileName()));
        //return;
    }
    pUpdateScrollBar();
    update();
}

/*
 * void pRefreshRules()
 *
 * Notify rules have changed and update view
 */
void HexViewer::pRefreshRules() {
    // Notify current rules have changed
    emit RuleNamesChanged(RuleNames());
    // Parse new vars and values
    ParseRules();
}

/*
 * void pPaintRules(QPainter &painter)
 *
 * Paint the hex rules to the sidebar
 */
void HexViewer::pPaintRules(QPainter &painter) {
    int ruleIndex = 1;
    bool isSkip = false;
    qreal fontHeight = QFontMetrics(painter.font()).height();
    QRectF rulesRect = pCalcRuleRect();

    painter.save();
    painter.setFont(QFont("CommitMono", 7));

    QRectF hexRect(pCalcHexRect());
    int hexXPos = hexRect.left();
    int hexYPos = hexRect.top();

    QRectF textRect(pCalcTextRect());
    int textXPos = textRect.left();
    int textYPos = textRect.top();

    int currY = 0;

    foreach (Rule rule, pRules) {
        QString typeText = "";
        QString valueText = "";
        int preSkipCount = rule.PreSkipCount();
        int postSkipCount = rule.PostSkipCount();
        QVector<int> repeatVals = QVector<int>();

        QString ruleName = rule.Name();

        QColor ruleBackground(rule.Color());
        ruleBackground.setAlpha(75);
        painter.setPen(Qt::black);
        painter.setBrush(ruleBackground);

        valueText.clear();
        for (int i = 0; i <= rule.RepeatCount(); i++) {
            QString formattedName = rule.Name();
            if (rule.RepeatCount() > 0) {
                formattedName = QString(rule.Name() + " %1").arg(i + 1);
            }
            // Paint pre-skip
            if (preSkipCount > 0) {
                pPaintHexRule(painter, hexXPos, hexYPos, preSkipCount, true);
                pPaintTextRule(painter, textXPos, textYPos, preSkipCount, true);
            }
            // Paint skip or int
            if (rule.Type() == TYPE_SKIP) {
                pPaintHexRule(painter, hexXPos, hexYPos, rule.Length(), true);
                pPaintTextRule(painter, textXPos, textYPos, rule.Length(), true);
                isSkip = true;
            } else {
                typeText = QString("%1-BIT %2 INTEGER").arg(rule.Length() * 8)
                        .arg(rule.Type() % 2 == 0 ? "" : "US");
                valueText += pVars[formattedName].ToString() + "\n";
                if (rule.RepeatCount() > 0) {
                    repeatVals.push_back(pVars[formattedName].ToInt());
                }
                pPaintHexRule(painter, hexXPos, hexYPos, rule.Length());
                pPaintTextRule(painter, textXPos, textYPos, rule.Length());
            }
            // Paint post-skip
            if (postSkipCount > 0) {
                pPaintHexRule(painter, hexXPos, hexYPos, postSkipCount, true);
                pPaintTextRule(painter, textXPos, textYPos, postSkipCount, true);
            }
        }

        if (!isSkip) {
            int blockHeight;
            if (repeatVals.size() > 0) {
                valueText = pFormatValuesGrid(repeatVals, rulesRect.width(), painter.font(), rule.Length() * 8, blockHeight);
            }

            qreal ruleHeight;
            if (rule.RepeatCount() == 0) {
                ruleHeight = fontHeight * 2;
            } else {
                ruleHeight = fontHeight * 2 + blockHeight;
            }

            QRectF ruleRect(rulesRect.left() + 1, -verticalScrollBar()->value() + rulesRect.top() + 2 + currY,
                            rulesRect.width() - 2, ruleHeight);
            QString paddedIndex = QString::number(ruleIndex).rightJustified(2, '0');
            QString ruleText = QString("%1. %2\n%3\n%4\n").arg(paddedIndex, ruleName,
                                                               typeText, valueText);
            QString ruleByteOrder = QString(rule.ByteOrder() == BYTE_ORDER_BE ? "BE" : "LE") + "\n\n";
            painter.drawText(ruleRect, Qt::TextWrapAnywhere, ruleText);
            painter.drawText(ruleRect, Qt::AlignRight, ruleByteOrder);

            QRectF ruleBackgroundRect(rulesRect.left(), -verticalScrollBar()->value() + rulesRect.top() + 1 + currY,
                                      rulesRect.width(), ruleHeight);

            painter.drawRect(ruleBackgroundRect);
            ruleIndex++;
            currY += ruleHeight;
        }
        isSkip = false;
    }
    painter.restore();
}

void HexViewer::pPaintHeaders(QPainter &painter) {
    QRectF offSetHeaderRect = pCalcOffsetHeaderRect();

    painter.setPen(Qt::blue);
    painter.drawText(offSetHeaderRect, Qt::AlignHCenter | Qt::AlignVCenter, "Offset (d)");

    QRectF hexHeaderRect = pCalcHexHeaderRect();

    int charWidth = fontMetrics().horizontalAdvance('F');
    int hexCharsPerLine = qCeil(hexHeaderRect.width() / charWidth) / 3;
    QString hexHeader = "";
    for (int i = 0; i < hexCharsPerLine; i++) {
        hexHeader += QString::number(i).rightJustified(2, '0') + " ";
    }
    hexHeader = hexHeader.trimmed();

    painter.setPen(Qt::blue);
    painter.drawText(hexHeaderRect, Qt::AlignVCenter, hexHeader);

    QRectF textHeaderRect = pCalcTextHeaderRect();

    painter.setPen(Qt::blue);
    painter.drawText(textHeaderRect, Qt::AlignVCenter, "Decoded Text");

    QRectF stackHeaderRect = pCalcRuleHeaderRect();

    painter.setPen(Qt::blue);
    painter.drawText(stackHeaderRect, Qt::AlignVCenter, "Rule Stack");
}

void HexViewer::paintEvent(QPaintEvent *event) {
    QPainter painter(viewport());
    if (!painter.isActive()) {
        HexesLogger::HexesError(45, "QPainter is not active!");
        return;
    }
    if (event->rect().isEmpty()) { return; }
    painter.fillRect(pViewRect, Qt::white);

    pPaintHeaders(painter);
    pPaintOffsetColumn(painter);
    pPaintHex(painter);
    pPaintText(painter);
    pPaintRules(painter);

    // if (pCursorVisible) {
    //     QPoint cursorPos = pCalcCursorPosition();

    //     int cursorHeight = pFontMetrics.height();

    //     painter.drawLine(cursorPos.x(), cursorPos.y(), cursorPos.x(), cursorPos.y() + cursorHeight);
    // }
    // if (pSelectionStart >= 0 && pSelectionEnd >= pSelectionStart) {
    //     if (pSelectionStart >= 0 && pSelectionEnd >= pSelectionStart) {
    //         int startX = pFontMetrics.horizontalAdvance(pText.left(pSelectionStart));
    //         int endX = pFontMetrics.horizontalAdvance(pText.left(pSelectionEnd));
    //         int height = pFontMetrics.height();

    //         painter.fillRect(QRect(startX, 0, endX - startX, height), QColor(0, 120, 215, 100)); // Semi-transparent blue
    //     }
    // }
}

void HexViewer::showEvent(QShowEvent *event) {
    pUpdateScrollBar();
    pSeekFile();
    QAbstractScrollArea::showEvent(event);
}

QSize HexViewer::sizeHint() const {
    return pCalcPaintRect().size().toSize();
}

void HexViewer::pScrollValueChanged(int value) {
    pScrollValue = value;
    pSeekFile();
}

void HexViewer::pBlinkCursor() {
    pCursorVisible = !pCursorVisible;
    update();
}

void HexViewer::resizeEvent(QResizeEvent *event) {
    QAbstractScrollArea::resizeEvent(event);

    pViewRect = QRectF(QPoint(0, 0), event->size());
    pSeekFile();
}

void HexViewer::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Left) {
        pCursorPosition = qMax(0, pCursorPosition - 1);
    } else if (event->key() == Qt::Key_Right) {
        pCursorPosition = qMin(pText.length(), pCursorPosition + 1);
    }
    update();
}

void HexViewer::mousePressEvent(QMouseEvent *event) {
    QFontMetrics fm(font());
    int pos = event->pos().x() / fm.averageCharWidth();
    pCursorPosition = qMin(pos, pText.length());
    pSelectionStart = pCursorPosition;
    update();
}

void HexViewer::mouseMoveEvent(QMouseEvent *event) {
    QFontMetrics fm(font());
    int pos = event->pos().x() / fm.averageCharWidth();
    pSelectionEnd = qMin(pos, pText.length());
    update();
}

QString HexViewer::pCleanHex(const QString &text) {
    QString result;
    for (int i = 0; i < text.length(); i += 2) {
        result.append(text.mid(i, 2)); // Get two characters at a time
        if (i + 2 < text.length()) {
            result.append(" "); // Add a space after every two characters
        }
    }
    return result.toUpper();
}

int HexViewer::pCalcContentHeight() const {
    int numLines = pHex.length() / pCalcCharCount();
    return numLines * fontMetrics().height();
}

int HexViewer::pCalcCharCount() const {
    int charWidth = fontMetrics().horizontalAdvance("F");
    QRectF hexHeaderRect(pCalcHexHeaderRect());
    return qCeil(hexHeaderRect.width() / charWidth) / 3;
}

int HexViewer::pCalcLineCount() const {
    return pText.length() / pCalcCharCount();
}

QPoint HexViewer::pCalcCursorPosition() const {
    int lineHeight = fontMetrics().height();

    int line = pCalcCursorLine();

    int lineStart = 0;
    for (int i = 0; i < line; ++i) {
        lineStart += rect().width() / fontMetrics().averageCharWidth();
    }

    int cursorX = fontMetrics().horizontalAdvance(pText.mid(lineStart, pCursorPosition - lineStart));
    int cursorY = line * lineHeight;

    return QPoint(cursorX, cursorY);
}

QRectF HexViewer::pCalcPaintRect() const {
    return pViewRect;
}

int HexViewer::pCalcCursorLine() const {
    qreal columnWidth = rect().right() - 90 - verticalScrollBar()->width();
    qreal scrollHeight = verticalScrollBar()->height();
    qreal hexColumnWidth = (columnWidth / 4 * 3) - 15;
    QRectF hexRect(95, 30, hexColumnWidth - 10, scrollHeight - 35);

    int charsPerLine = hexRect.width() / fontMetrics().averageCharWidth();

    return pCursorPosition / charsPerLine;
}

void HexViewer::pPaintCursor(QPainter &painter) {
    Q_UNUSED(painter);
}

void HexViewer::pPaintSelection(QPainter &painter) {
    Q_UNUSED(painter);
}

QString HexViewer::pStringToHex(const QString &text) {
    return text.toUtf8().toHex();
}

void HexViewer::pUpdateScrollBar() {
    verticalScrollBar()->setRange(0, pFile.size());
    verticalScrollBar()->setPageStep(pCalcCharCount());
    verticalScrollBar()->setSingleStep(pCalcCharCount());

    update();
}
