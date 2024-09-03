#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUrl>
#include <QDebug>
#include <QRegularExpression>
#include <QRegularExpressionMatch>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DriveFDIcon)));

    connect(ui->teRawText, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->teEncode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onTextChanged()
{
    if (m_processing) {
        return; // 防止循环调用
    }

    m_processing = true;

    QTextEdit *senderTextEdit = qobject_cast<QTextEdit *>(sender());
    if (senderTextEdit == ui->teRawText) {
        // 处理teRawText的变化
        rawToEncodeText();
    } else if (senderTextEdit == ui->teEncode) {
        // 处理teEncode的变化
        encodeToRawText();
    }

    m_processing = false; // 重置标志
}

void MainWindow::rawToEncodeText()
{
    QString text = ui->teRawText->toPlainText();

    QString encode_text;
    for (int i = 0; i < text.size(); ++i) {
        QChar ch = text.at(i);
        // 获取QChar的unicode码点
        ushort codePoint = ch.unicode();

        // 判断QChar是否是非ASCII字符
        if (codePoint > 127) {
            QString url_text = toUrlEncoded(ch);
            QStringList list = url_text.split("%");

            if (url_text.contains("%") && list.size() > 0) {
                foreach(QString str, list) {
                    if (str.isEmpty()) continue;
                    encode_text += "\\" + hexToOctal(str);
                }
            } else {
                encode_text.append(url_text);
            }
        } else {
            encode_text.append(ch);
        }
    }


    disconnect(ui->teEncode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    ui->teEncode->setText(encode_text);
    connect(ui->teEncode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
}

void MainWindow::encodeToRawText()
{
    QString text = ui->teEncode->toPlainText();

    QString raw_text = processGroups(text);

    disconnect(ui->teRawText, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    ui->teRawText->setText(raw_text);
    connect(ui->teRawText, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
}

// 处理字母及符号的QString
QString MainWindow::processAlphaNumeric(const QString &segment)
{
    return segment;
}

// 处理带\的QString
QString MainWindow::processBackslashGroups(const QString &segment)
{
    QString text;
    QStringList list = segment.split('\\');
    foreach (QString str, list) {
        if (str.isEmpty()) continue;
        text += "%" + octalToHex(str);
    }
    return fromUrlEncoded(text);
}

// 分组提取并分类处理函数
QString MainWindow::processGroups(const QString &input)
{
    QString result;

    QRegularExpression re("\\\\(\\d{3})|\\\\(\\d{1,2})(?=[^\\d])|\\\\(\\d{4,})");
    // 匹配 \ 后面的三位数字、两位及以下数字后面接非数字、以及大于三位数字
    QRegularExpressionMatchIterator i = re.globalMatch(input);

    int lastPos = 0;
    QString backslashSegment;
    bool inBackslashSegment = false;

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        int startPos = match.capturedStart();
        int endPos = match.capturedEnd();

        // 处理上一个匹配结束到当前匹配开始之间的部分
        if (startPos > lastPos) {
            QString segment = input.mid(lastPos, startPos - lastPos);
            // 结束上一次统计的匹配部分
            if (inBackslashSegment) {
                result += processBackslashGroups(backslashSegment);
                backslashSegment.clear();
                inBackslashSegment = false;
            }
            // 处理字母及符号的QString
            result += processAlphaNumeric(segment);
        }

        // 处理匹配到的数字部分
        QString matchedGroup = match.captured(0);
        if (matchedGroup.length() == 4) {
            backslashSegment.append(matchedGroup);
            inBackslashSegment = true;
        } else if (matchedGroup.length() < 4) {
            backslashSegment.append(matchedGroup);
            inBackslashSegment = true;
        } else {
            // 如果数字部分大于三位，分开处理
            QString firstPart = matchedGroup.left(3);
            QString secondPart = matchedGroup.mid(3);
            backslashSegment.append(firstPart);
            result += processBackslashGroups(backslashSegment);
            backslashSegment.clear();
            // 处理字母及符号的QString
            result += processAlphaNumeric(secondPart);
            inBackslashSegment = false;
        }

        lastPos = endPos;
    }

    if (inBackslashSegment) {
        result += processBackslashGroups(backslashSegment);
    }

    // 处理最后一个匹配结束到字符串结束之间的部分
    if (lastPos < input.length()) {
        QString segment = input.mid(lastPos);
        // 处理字母及符号的QString
        result += processAlphaNumeric(segment);
    }
    return result;
}


QString MainWindow::toUrlEncoded(const QString &input)
{
    return QUrl::toPercentEncoding(input);
}

QString MainWindow::fromUrlEncoded(const QString &encodedInput)
{
    return QUrl::fromPercentEncoding(encodedInput.toUtf8());
}

QString MainWindow::hexToOctal(const QString &hexString)
{
    // 将十六进制字符串转换为十进制数
    bool ok;
    int decimalValue = hexString.toInt(&ok, 16);
    if (!ok) {
        return QString();
    }

    // 将十进制数转换为八进制字符串
    return QString::number(decimalValue, 8);
}

QString MainWindow::octalToHex(const QString &octalString)
{
    // 将八进制字符串转换为十进制数
    bool ok;
    int decimalValue = octalString.toInt(&ok, 8);
    if (!ok) {
        return QString();
    }

    // 将十进制数转换为十六进制字符串
    return QString::number(decimalValue, 16).toUpper();
}
