#ifndef COMBOBOXOTHERINPUT_H
#define COMBOBOXOTHERINPUT_H

#include <QObject>
#include "ShellBase.h"
#include "../XXqtDefine.h"
#include <QLineEdit>
#include <QComboBox>

class ComboBoxOtherInput : public ShellBase
{
    Q_OBJECT
public:
    explicit ComboBoxOtherInput(QObject *parent = nullptr);

    // name
    XX_KEY(kComboBox);
    XX_KEY(kLineEdit);
    void shell(QComboBox *comboBox,QLineEdit *lineEdit);

    // param
    XX_KEY(kOptional);
    XX_KEY(kOtherIndex);

    /**
     * @brief config
     * @param param
     *
     * 可选项
     * optional:[
     *  {name1,title1},
     *  {name2,title2}
     * ]
     * * optional:[
     *  name1,name2,name3
     * ]
     * 其他的index，可缺省，默认是最后一个
     * otherIndex:int
     */
    void config(const QVariant &param);
    void config(const QStringList &optionals, int otherIndex=-1);

    // action
    XX_KEY(kGetText);
    XX_KEY(kSetText);
    QVariant action(const QString &action,const QVariant &param=QVariant());
    QString getText();
    void setText(const QString &text);

protected:
    void resetTarget(const QString &name, QObject *target);

private:
    QLineEdit* lineEdit();
    QComboBox* comboBox();
    Q_SLOT void comboBox_onCurrentIndexChanged(int index);

private:
    QList<QPair<QString,QString>> _optionals;   // name - text
    int _otherIndex;
};

#endif // COMBOBOXOTHERINPUT_H
