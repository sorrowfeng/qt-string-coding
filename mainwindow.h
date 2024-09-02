#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTextChanged();

private:
    void rawToUrlText();
    void urlToEncodeText();
    void urlToRawText();
    void encodeToUrlText();


    QString toUrlEncoded(const QString &input);
    QString fromUrlEncoded(const QString &encodedInput);
    QString hexToOctal(const QString &hexString);
    QString octalToHex(const QString &octalString);

private:
    Ui::MainWindow *ui;

    bool m_processing = false;
};

#endif // MAINWINDOW_H
