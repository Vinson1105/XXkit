#ifndef LINEEDITPSFIX_H
#define LINEEDITPSFIX_H

#include "ShellBase.h"
#include "../XXqtDefine.h"
#include <QLabel>
#include <QLineEdit>

class LineEditPSfix : public ShellBase
{
    Q_OBJECT
public:
    explicit LineEditPSfix(QObject *parent = nullptr);

    XX_KEY(kPrefix);
    XX_KEY(kLineEdit);
    XX_KEY(kSuffix);
    void shell(QLabel *prefix, QLineEdit *lineEdit, QLabel *suffix);

    // action
    XX_KEY(kGetText);
    XX_KEY(kSetText);
    XX_KEY(kSetContent);
    QVariant action(const QString &action,const QVariant &param=QVariant());
    QString getText();
    void setText(QString text);
    void setContent(const QString &content);

private:
    QLabel* prefix();
    QLineEdit* content();
    QLabel* suffix();
};

#endif // LINEEDITPSFIX_H
