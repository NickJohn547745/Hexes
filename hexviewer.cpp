#include "hexviewer.h"

HexViewer::HexViewer(QWidget *parent)
    : QAbstractScrollArea(parent) {
    pText = "";
    pHex = "";
    pFileName = "";
    pViewRect = QRectF(0, 0, 800, 557);
    pBlinkTimer = new QTimer(this);
    pRules = QQueue<Rule>();
    pScrollValue = 0;
    pVars = QMap<QString, BlockValue>();

    setFont(QFont("CommitMono", 10));

    qRegisterMetaType<Rule>("Rule");
    qRegisterMetaType<QQueue<Rule>>("QQueue<Rule>");
    //qRegisterMetaTypeStreamOperators<QQueue<Rule>>("QQueue<Rule>");

    verticalScrollBar()->setSingleStep(45);
    verticalScrollBar()->setPageStep(45);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, [this](int value) {
        pScrollValue = value;
        pSeekFile();
    });

    connect(pBlinkTimer, &QTimer::timeout, this, [this]() {
        pCursorVisible = !pCursorVisible;
        update();  // Trigger update to show/hide cursor
    });
    pBlinkTimer->start(500);  // Blinking interval
}

HexViewer::~HexViewer() {
    delete pBlinkTimer;
}

void HexViewer::SetFileName(const QString fileName) {
    pFileName = fileName;

    pFile.setFileName(pFileName);
    if (!pFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open " << pFileName;
    } else {
        qDebug() << "Opened " << pFileName;
    }
    pSeekFile();
}

void HexViewer::AddRule(const Rule rule) {
    pRules.enqueue(rule);

    emit RuleNamesChanged(RuleNames());
    RunRules();
}

void HexViewer::AddRules(const QVector<Rule> rules) {
    for (int i = 0; i < rules.size(); i++) {
        pRules.enqueue(rules[i]);
    }

    emit RuleNamesChanged(RuleNames());
    RunRules();
}

void HexViewer::DeleteRule(const Rule rule) {
    if (!pRules.contains(rule)) { return; }
    if (pRules.size() == 0) { return; }

    for (int i = 0; i < pRules.size(); i++) {
        Rule currentRule = pRules.dequeue();

        if (currentRule != rule) {
            pRules.enqueue(currentRule);
        }
    }

    emit RuleNamesChanged(RuleNames());
    update();
}

void HexViewer::DeleteRuleByName(const QString ruleName) {
    if (pRules.size() == 0) { return; }

    for (int i = 0; i < pRules.size(); i++) {
        Rule currentRule = pRules.dequeue();

        if (currentRule.Name() != ruleName) {
            pRules.enqueue(currentRule);
        }
    }

    emit RuleNamesChanged(RuleNames());
    update();
}

void HexViewer::PopRule() {
    if (pRules.isEmpty()) { return; }

    pRules.dequeue();
    emit RuleNamesChanged(RuleNames());
    update();
}

void HexViewer::ClearRules() {
    if (pRules.isEmpty()) { return; }

    pRules.clear();
    emit RuleNamesChanged(RuleNames());
    update();
}

void HexViewer::SaveRules() {
    QString filePath = QFileDialog::getSaveFileName(this, "Save Hexes Rule File",
                                                    "C:/", "Rule File (*.hrf);;All Files(*.*)");
    QFile ruleFile(filePath);
    if (!ruleFile.open(QIODevice::WriteOnly)) {
        qDebug() << "Failed to save rule file!";
        return;
    }
    ruleFile.close();

    QSettings ruleExport(filePath, QSettings::IniFormat);
    ruleExport.setValue("rules", QVariant::fromValue(pRules));
}

void HexViewer::OpenRules() {
    QString filePath = QFileDialog::getOpenFileName(this, "Open Hexes Rule File",
                                                    "C:/", "Rule File (*.hrf);;All Files(*.*)");
    QFile ruleFile(filePath);
    if (!ruleFile.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to save rule file!";
        return;
    }
    ruleFile.close();

    QSettings ruleExport(filePath, QSettings::IniFormat);
    pRules = ruleExport.value("rules").value<QQueue<Rule>>();
    emit RuleNamesChanged(RuleNames());

    RunRules();
}

QStringList HexViewer::RuleNames() {
    QStringList ruleNames;
    foreach (Rule rule, pRules) {
        ruleNames.append(rule.Name());
    }
    return ruleNames;
}

void HexViewer::RunRules() {
    QDataStream inStream(QByteArray::fromHex(pHex.toUtf8()));

    for (int i = 0; i < pRules.size(); i++) {
        Rule rule = pRules[i];

        for (int j = 0; j <= rule.RepeatCount(); j++) {
            if (rule.ByteOrder() == BYTE_ORDER_BE) {
                inStream.setByteOrder(QDataStream::BigEndian);
            } else {
                inStream.setByteOrder(QDataStream::LittleEndian);
            }

            QString ruleName = rule.Name().toUpper();
            if (rule.RepeatCount() > 0) {
                ruleName += QString(" %1").arg(j);
            }

            BlockValue val;
            val.SetName(ruleName);

            if (rule.PreSkipCount() > 0) {
                inStream.skipRawData(rule.PreSkipCount());
            }

            switch ((int)rule.Type()) {
            case 1: // Skip
                inStream.skipRawData(rule.Length());
                break;
            case 2: // int8
                qint8 val_8;
                inStream >> val_8;
                rule.SetValue(QString::number(val_8));
                val.SetValue(val_8);
                break;
            case 3: // uint8
                quint8 val_u8;
                inStream >> val_u8;
                rule.SetValue(QString::number(val_u8));
                val.SetValue(val_u8);
                break;
            case 4: // int16
                qint16 val_16;
                inStream >> val_16;
                rule.SetValue(QString::number(val_16));
                val.SetValue(val_16);
                break;
            case 5: // uint16
                quint16 val_u16;
                inStream >> val_u16;
                rule.SetValue(QString::number(val_u16));
                val.SetValue(val_u16);
                break;
            case 6: // int32
                qint32 val_32;
                inStream >> val_32;
                rule.SetValue(QString::number(val_32));
                val.SetValue(val_32);
                break;
            case 7: // uint32
                quint32 val_u32;
                inStream >> val_u32;
                rule.SetValue(QString::number(val_u32));
                val.SetValue(val_u32);
                break;
            case 8: // int64
                qint64 val_64;
                inStream >> val_64;
                rule.SetValue(QString::number(val_64));
                val.SetValue(val_64);
                break;
            case 9: // uint64
                quint64 val_u64;
                inStream >> val_u64;
                rule.SetValue(QString::number(val_u64));
                val.SetValue(val_u64);
                break;
            }
            if (rule.PostSkipCount() > 0) {
                inStream.skipRawData(rule.PostSkipCount());
            }

            if ((rule.Type() != TYPE_NONE)
                    && (rule.Type() != TYPE_SKIP)) {
                pVars[ruleName] = val;
                emit VarsChanged(pVars);
            }
            pRules[i] = rule;
        }
    }
    update();
}

QMap<QString, BlockValue> HexViewer::GetVars() {
    return pVars;
}

QString HexViewer::Text() {
    return pText;
}

QString HexViewer::Hex() {
    return pHex;
}

QRectF HexViewer::pCalcOffsetColumnRect() {
    return QRectF(5, 30, 80, height() - 35);
}

QRectF HexViewer::pCalcHexHeaderRect() const {
    QRectF paintRect = pCalcPaintRect();

    qreal columnWidth = paintRect.right() - 125;
    qreal hexColumnWidth = (columnWidth / 5 * 3) - 15;

    return QRectF(95, 5, hexColumnWidth - 10, 15);
}

QRectF HexViewer::pCalcTextHeaderRect() {
    QRectF paintRect = pCalcPaintRect();

    qreal columnWidth = paintRect.right() - 125;
    qreal hexColumnWidth = (columnWidth / 5 * 3) - 15;
    qreal textColumnWidth = (columnWidth / 5) + 15;

    return QRectF(95 + hexColumnWidth, 5, textColumnWidth - 10, 15);
}

QRectF HexViewer::pCalcRuleHeaderRect() {
    QRectF paintRect = pCalcPaintRect();

    qreal columnWidth = paintRect.right() - 125;

    qreal hexColumnWidth = (columnWidth / 5 * 3) - 15;
    qreal textColumnWidth = (columnWidth / 5) + 15;
    qreal ruleColumnWidth = (columnWidth / 5) + 15;

    return QRectF(95 + hexColumnWidth + textColumnWidth, 5, ruleColumnWidth - 10, 15);
}

QRectF HexViewer::pCalcOffsetHeaderRect() {
    return QRectF(5, 5, 80, 15);
}

void HexViewer::pPaintOffsetColumn(QPainter &painter) {
    QRectF offSetColumnRect = pCalcOffsetColumnRect();
    QRectF hexRect = pCalcHexRect();

    int charWidth = fontMetrics().horizontalAdvance('F');
    int hexCharsPerLine = qCeil(hexRect.width() / charWidth) / 3;

    QString offset = "";
    int nextOffset = verticalScrollBar()->value();
    for (int i = 0; i < qCeil(hexRect.height() / fontMetrics().height()); i++) {
        offset += QString::number(nextOffset).rightJustified(8, '0') + " ";
        nextOffset += hexCharsPerLine;
    }
    offset = offset.trimmed();

    painter.setPen(Qt::blue);
    painter.drawText(offSetColumnRect, Qt::TextWordWrap | Qt::AlignHCenter, offset);
}

QRectF HexViewer::pCalcHexRect() const {
    QRectF paintRect = pCalcPaintRect();

    qreal columnWidth = paintRect.right() - 125;
    qreal hexColumnWidth = (columnWidth / 5 * 3) - 15;
    qreal scrollHeight = paintRect.height();

    return QRectF(95, 30, hexColumnWidth - 10, scrollHeight - 35);
}

void HexViewer::pPaintHex(QPainter &painter) {
    QRectF hexRect = pCalcHexRect();

    painter.setPen(Qt::black);
    painter.drawText(hexRect, Qt::TextWordWrap, pHex);
}

QRectF HexViewer::pCalcTextRect() const {
    QRectF paintRect = pCalcPaintRect();
    qreal columnWidth = paintRect.right() - 125;
    qreal hexColumnWidth = 80 + (columnWidth / 5 * 3);
    qreal textColumnWidth = (columnWidth / 5);
    qreal scrollHeight = paintRect.height();
    int charWidth = fontMetrics().horizontalAdvance("F");

    return QRectF(hexColumnWidth, 30, textColumnWidth - charWidth, scrollHeight - 35);
}

QRectF HexViewer::pCalcRuleRect() {
    QRectF paintRect = pCalcPaintRect();

    qreal columnWidth = paintRect.right() - 125;

    qreal hexColumnWidth = (columnWidth / 5 * 3) - 15;
    qreal textColumnWidth = (columnWidth / 5) + 15;
    qreal rulesColumnWidth = (columnWidth / 5) + 15;
    qreal scrollHeight = pCalcContentHeight();

    return QRectF(95 + hexColumnWidth + textColumnWidth, 30, rulesColumnWidth - 10, scrollHeight - 35);
}

QString HexViewer::pInsertNewlinesAtIntervals(const QString &input, int interval) {
    QString result = input;

    for (int i = interval; i < result.length(); i += interval) {
        result.insert(i, '\n');
        i++;
    }

    return result;
}


void HexViewer::pPaintText(QPainter &painter) {
    QRectF textRect = pCalcTextRect();

    painter.setPen(Qt::black);
    painter.drawText(textRect, Qt::TextWrapAnywhere, pText);
}

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
        if (adjustedYPos + fontMetrics().height() >= hexRect.top() &&
            adjustedYPos <= hexRect.bottom()) {

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
        if (adjustedYPos + fontMetrics().height() >= textRect.top() &&
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



void HexViewer::pSeekFile() {
    QRectF hexRect(pCalcHexRect());
    qDebug() << "hexRect width" << hexRect.width();
    int dataLength = (hexRect.height() / fontMetrics().height()) * pCalcCharCount();

    if (dataLength < 0) {
        return;
    } else if (pFile.isOpen()) {
        pFile.seek(verticalScrollBar()->value());
        QByteArray data = pFile.read(dataLength + 2);
        pHex = pCleanHex(data.toHex());
        pText = QString::fromLocal8Bit(data).replace('\0', '.');//,  pCalcCharCount());
    } else {
        qDebug() << QString("File '%1' not open...").arg(pFile.fileName());
        return;
    }
    pUpdateScrollBar();
    update();
}

void HexViewer::pPaintRules(QPainter &painter) {
    int ruleIndex = 1;
    QRectF rulesRect = pCalcRuleRect();

    painter.save();
    painter.setFont(QFont("CommitMono", 7));

    QRectF hexRect(pCalcHexRect());
    int hexXPos = hexRect.left();
    int hexYPos = hexRect.top();

    QRectF textRect(pCalcTextRect());
    int textXPos = textRect.left();
    int textYPos = textRect.top();

    QRectF clipRect(0, rulesRect.top(), width(), rulesRect.height());
    painter.setClipRect(clipRect);

    QQueue<Rule> rules = QQueue<Rule>(pRules);

    foreach (Rule rule, rules) {
        QString ruleName = rule.Name().toUpper();
        if (rule.RepeatCount() > 0) {
            ruleName += " %1";
        }

        for (int i = 0; i <= rule.RepeatCount(); i++) {
            QString formattedName = ruleName;
            int preSkipCount = rule.PreSkipCount();
            int postSkipCount = rule.PostSkipCount();

            if (rule.RepeatCount() > 0) {
                formattedName = formattedName.arg(i + 1);
            }

            painter.setPen(QPen(rule.Color(), 1));

            QString typeText = "";
            QString valueText = "";

            QColor ruleBackground(rule.Color());
            ruleBackground.setAlpha(75);
            painter.setBrush(ruleBackground);

            // Draw the rule information in the rule stack on the right side
            QRectF ruleBackgroundRect(rulesRect.left(), -verticalScrollBar()->value() + rulesRect.top() + 1 + (QFontMetrics(painter.font()).height() * 3) * (ruleIndex - 1),
                            rulesRect.width(), QFontMetrics(painter.font()).height() * 3);
            QRectF ruleRect(rulesRect.left() + 1, -verticalScrollBar()->value() + rulesRect.top() + 2 + (QFontMetrics(painter.font()).height() * 3) * (ruleIndex - 1),
                            rulesRect.width() - 2, QFontMetrics(painter.font()).height() * 3);
            painter.drawRect(ruleBackgroundRect);

            if (preSkipCount > 0) {
                //typeText = QString("Skip %1 byte(s)").arg(rule.Length());
                pPaintHexRule(painter, hexXPos, hexYPos, preSkipCount, true);
                pPaintTextRule(painter, textXPos, textYPos, preSkipCount, true);
                //isSkip = true;
            }
            if (postSkipCount > 0) {
                //typeText = QString("Skip %1 byte(s)").arg(rule.Length());
                pPaintHexRule(painter, hexXPos, hexYPos, postSkipCount, true);
                pPaintTextRule(painter, textXPos, textYPos, postSkipCount, true);
                //isSkip = true;
            }

            bool isSkip = false;
            switch ((int)rule.Type()) {
            case 1: // TYPE_SKIP
                typeText = QString("Skip %1 byte(s)").arg(rule.Length());
                pPaintHexRule(painter, hexXPos, hexYPos, rule.Length(), true);
                pPaintTextRule(painter, textXPos, textYPos, rule.Length(), true);
                isSkip = true;
                break;

            case 2: // TYPE_INT_8
            case 3: // TYPE_UINT_8
            case 4: // TYPE_INT_16
            case 5: // TYPE_UINT_16
            case 6: // TYPE_INT_32
            case 7: // TYPE_UINT_32
            case 8: // TYPE_INT_64
            case 9: // TYPE_UINT_64
                typeText = QString("%1-BIT %2 INTEGER").arg(rule.Length() * 8).arg(rule.Type() % 2 == 0 ? "" : "US");
                valueText = pVars[formattedName].Value().toString();
                pPaintHexRule(painter, hexXPos, hexYPos, rule.Length());
                pPaintTextRule(painter, textXPos, textYPos, rule.Length());
                break;

            default:
                typeText = "UH-OH";
                break;
            }

            painter.setPen(Qt::black);
            QString paddedIndex = QString::number(ruleIndex).rightJustified(2, '0');
            QString ruleText = QString("%1. %2\n%3\n%4\n").arg(paddedIndex, formattedName,
                                                               typeText, valueText);
            painter.drawText(ruleRect, Qt::TextWrapAnywhere, ruleText);

            QString ruleByteOrder = QString(rule.ByteOrder() == BYTE_ORDER_BE ? "BE" : "LE") + "\n\n";
            if (!isSkip) {
                painter.drawText(ruleRect, Qt::AlignRight, ruleByteOrder);
            }

            ruleIndex++;
        }
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
        qWarning() << "QPainter is not active";
        return;
    }
    if (event->rect().isEmpty()) { return; }
    painter.fillRect(pViewRect, Qt::white);

    pPaintHeaders(painter);
    pPaintOffsetColumn(painter);
    pPaintHex(painter);
    pPaintText(painter);
    pPaintRules(painter);

    if (pCursorVisible) {
        QPoint cursorPos = pCalcCursorPosition();

        QFontMetrics fm(font());
        int cursorHeight = fm.height();

        painter.drawLine(cursorPos.x(), cursorPos.y(), cursorPos.x(), cursorPos.y() + cursorHeight);
    }
    if (pSelectionStart >= 0 && pSelectionEnd >= pSelectionStart) {
        if (pSelectionStart >= 0 && pSelectionEnd >= pSelectionStart) {
            QFontMetrics fm(font());
            int startX = fm.horizontalAdvance(pText.left(pSelectionStart));
            int endX = fm.horizontalAdvance(pText.left(pSelectionEnd));
            int height = fm.height();

            painter.fillRect(QRect(startX, 0, endX - startX, height), QColor(0, 120, 215, 100)); // Semi-transparent blue
        }
    }
}

void HexViewer::showEvent(QShowEvent *event) {
    pUpdateScrollBar();
    pSeekFile();
    QAbstractScrollArea::showEvent(event);
}

QSize HexViewer::sizeHint() const {
    return pCalcPaintRect().size().toSize();
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

QPoint HexViewer::pCalcCursorPosition() {
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

int HexViewer::pCalcCursorLine() {
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
