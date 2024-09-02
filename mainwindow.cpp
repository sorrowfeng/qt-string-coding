#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QUrl>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    this->setWindowIcon(QIcon(QApplication::style()->standardIcon(QStyle::SP_DriveFDIcon)));

    connect(ui->teRawText, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    connect(ui->teUrlCode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
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
        qDebug() << "teRawText";
        rawToUrlText();
        urlToEncodeText();
    } else if (senderTextEdit == ui->teUrlCode) {
        // 处理teUrlCode的变化
        qDebug() << "teUrlCode";
        urlToRawText();
        urlToEncodeText();
    } else if (senderTextEdit == ui->teEncode) {
        // 处理teEncode的变化
        qDebug() << "teEncode";
        encodeToUrlText();
        urlToRawText();
    }

    m_processing = false; // 重置标志
}

void MainWindow::rawToUrlText()
{
    QString text = ui->teRawText->toPlainText();

    QString url_text = toUrlEncoded(text);

    disconnect(ui->teUrlCode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    ui->teUrlCode->setText(url_text);
    connect(ui->teUrlCode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
}

void MainWindow::urlToEncodeText()
{
    QString text = ui->teUrlCode->toPlainText();

    QString encode_text;
    QStringList list = text.split("%");
    if (text.contains("%") && list.size() > 0) {
        foreach(QString str, list) {
            if (str.isEmpty()) continue;
            encode_text += "\\" + hexToOctal(str);
        }
    } else {
        encode_text = text;
    }

    disconnect(ui->teEncode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    ui->teEncode->setText(encode_text);
    connect(ui->teEncode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
}

void MainWindow::urlToRawText()
{
    QString text = ui->teUrlCode->toPlainText();

    QString raw_text = fromUrlEncoded(text);

    disconnect(ui->teRawText, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    ui->teRawText->setText(raw_text);
    connect(ui->teRawText, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
}

void MainWindow::encodeToUrlText()
{
    QString text = ui->teEncode->toPlainText();

    QString url_text;
    QStringList list = text.split("\\");
    if (text.contains("\\") && list.size() > 0) {
        foreach(QString str, list) {
            if (str.isEmpty()) continue;
            url_text += "%" + octalToHex(str);
        }
    } else {
        url_text = text;
    }

    disconnect(ui->teUrlCode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
    ui->teUrlCode->setText(url_text);
    connect(ui->teUrlCode, &QTextEdit::textChanged, this, &MainWindow::onTextChanged);
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
    if (!ok) {\
        return QString();
    }

    // 将十进制数转换为十六进制字符串
    return QString::number(decimalValue, 16).toUpper();
}
