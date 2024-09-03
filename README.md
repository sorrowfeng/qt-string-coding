
# Qt 字符串的编码方式
# 问题

总所周知，Qt的ui文件在编译时，会自动生成一个ui_xxxxx.h的头文件，打开一看，其实就是将摆放的控件new出来以及布局的代码。
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/3ab35cefc0c14f9ca0afc47ebd86aa0c.png)


只要用Qt提供的uic.exe工具，自己也可以将ui文件输出为代码文件。

在Qt安装目录所对应编译器的bin文件夹下，就能找到uic.exe
我这里路径是

> C:\Qt\Qt5.9.8\5.9.8\msvc2015\bin

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/debd3bb7d3a84de8909d9e57a17bdda8.png)

我们只需要用同目录下的designer.exe，设计好布局及控件，然后使用uic工具就能生成出代码

> uic.exe test.ui -o ui_test.h

-o 的意思是output输出为文件，如果不加后面的，则会直接在控制台打印。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/5c7e0affe71a4d85838081b6af05bfd5.png)

这边我已经准备了一个ui文件，并输出至ui_test.h

打开它我们可以看到

```c
/********************************************************************************
** Form generated from reading UI file 'test.ui'
**
** Created by: Qt User Interface Compiler version 5.9.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_TEST_H
#define UI_TEST_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_Form
{
public:
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton;
    QPushButton *pushButton_2;

    void setupUi(QWidget *Form)
    {
        if (Form->objectName().isEmpty())
            Form->setObjectName(QStringLiteral("Form"));
        Form->resize(400, 300);
        verticalLayout = new QVBoxLayout(Form);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        pushButton = new QPushButton(Form);
        pushButton->setObjectName(QStringLiteral("pushButton"));
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(pushButton->sizePolicy().hasHeightForWidth());
        pushButton->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(pushButton);

        pushButton_2 = new QPushButton(Form);
        pushButton_2->setObjectName(QStringLiteral("pushButton_2"));
        sizePolicy.setHeightForWidth(pushButton_2->sizePolicy().hasHeightForWidth());
        pushButton_2->setSizePolicy(sizePolicy);

        verticalLayout->addWidget(pushButton_2);


        retranslateUi(Form);

        QMetaObject::connectSlotsByName(Form);
    } // setupUi

    void retranslateUi(QWidget *Form)
    {
        Form->setWindowTitle(QApplication::translate("Form", "Form", Q_NULLPTR));
        pushButton->setText(QApplication::translate("Form", "\346\265\213\350\257\225\346\214\211\351\222\256", Q_NULLPTR));
        pushButton_2->setText(QApplication::translate("Form", "\342\234\205\346\265\213\350\257\225\345\220\204\347\247\215\346\240\274\345\274\217\360\237\215\2602024-09-03 14:03:16", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class Form: public Ui_Form {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_TEST_H

```

代码很简单，主要就是一个setupUi函数，用来建立布局及控件。
一个retranslateUi函数用来处理控件的文本显示内容

# 现象

当我们查看文本显示的内容，会发现该内容和ui文件中的完全不同，例如

> 测试按钮
> 对应为
> \346\265\213\350\257\225\346\214\211\351\222\256

> ✅❎测试各种格式✓✕2024-09-03 14:03:16
> 对应为
> \342\234\205\346\265\213\350\257\225\345\220\204\347\247\215\346\240\274\345\274\217\360\237\215\2602024-09-03 14:03:16

而且编码后的文本内容，在Qt的代码中可以直接设置该文本，并且可以显示为对应编码的原文本。

例如，我在setText的时候直接设置

> \344\275\240\345\245\275\357\274\214\346\234\250\351\243\216\345\217\257\345\217\257

这么一串字符串，输出的直接就是中文文本。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/81dcc245eaa144a9a61a10109dc78584.png)
# 编码解析

所以这到底是什么一种什么编码格式呢？下面为你解析。


首先，在用uic文件进行代码生成的时候，我们可以看到英文字母及标点符号，是不会进行上面这样的编码的，就像下面。
![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/5c6b4acd62e042709fe1fdd8e7007762.png)

而其他的字符则会，编码成对应的 \234\456 这样的文件。

其实就是在ASCII表内的字符是以原文本的方式进行保存。

[ASCII码对照表](https://ascii.911cha.com/)


而其他部分则是进行了一个编码。

编码方式，我们现在以“ 你好” 为例。
> 你好

我们如果将该字符串以url方式进行一个编码，则会得到
> %E4%BD%A0%E5%A5%BD

接下来我们将 % 换成 \ ，并将上面从16进制转为8进制，则会得到
> \344\275\240\345\245\275

这串字符，则和Qt所生成的完全相同。


通过对QString进行遍历打印出QChar，我们可以得知QString是以unicode方式进行存储。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/1ba59fa4fab04f509381ce0205cdb340.png)

[八进制数的表达方法！八进制数在转义符中的使用！](https://blog.csdn.net/love_gaohz/article/details/44586911)

通过该文章，我们也可以知道 \ 加 八进制数 是起到一个转义的作用。


# 示例代码

我这边也编写了一个Qt字符串的编码工具开源给大家，可以方便的进行字符串原文本与编码文本直接的相互转换。

![在这里插入图片描述](https://i-blog.csdnimg.cn/direct/1cc86cdd697b4fa9bf3a29ecbfffe377.gif)


> GitHub 仓库地址
> https://github.com/sorrowfeng/qt-string-coding