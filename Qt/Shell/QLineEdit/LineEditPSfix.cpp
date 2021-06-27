#include "LineEditPSfix.h"

XX_KEY_C(LineEditPSfix,kPrefix,"prefix");
XX_KEY_C(LineEditPSfix,kLineEdit,"lineEdit");
XX_KEY_C(LineEditPSfix,kSuffix,"suffix");
XX_KEY_C(LineEditPSfix,kGetText,"getText");

LineEditPSfix::LineEditPSfix(QObject *parent) : ShellBase(parent)
{

}

void LineEditPSfix::shell(QLabel *prefix, QLineEdit *lineEdit, QLabel *suffix){
    if(nullptr == lineEdit){
        return;
    }

    TargetMap map;
    if(nullptr!=prefix) map[kPrefix] = prefix;
    if(nullptr!=lineEdit) map[kLineEdit] = lineEdit;
    if(nullptr!=suffix) map[kSuffix] = suffix;
    ShellBase::shell(map);
}

QVariant LineEditPSfix::action(const QString &action,const QVariant &param){
    if(kGetText == action){
        return getText();
    }
    return QVariant();
}
QString LineEditPSfix::getText(){
    QString text;
    if(nullptr!=prefix()) text+=prefix()->text();
    if(nullptr!=content()) text+=content()->text();
    if(nullptr!=suffix()) text+=suffix()->text();
    return text;
}
void LineEditPSfix::setText(QString text){
    if(nullptr!=prefix()){
        QString pre = prefix()->text();
        if(text.startsWith(pre)){
            text = text.right(text.length()-pre.length());
        }
    }

    if(nullptr!=suffix()){
        QString suf = suffix()->text();
        if(text.endsWith(suf)){
            text = text.left(text.length()-suf.length());
        }
    }

    if(nullptr!=content()){
        content()->setText(text);
    }
}
void LineEditPSfix::setContent(const QString &content){
    if(nullptr!=this->content()){
        this->content()->setText(content);
    }
}

QLabel* LineEditPSfix::prefix(){
    return qobject_cast<QLabel*>(target(kPrefix));
}
QLineEdit* LineEditPSfix::content(){
    return qobject_cast<QLineEdit*>(target(kLineEdit));
}
QLabel* LineEditPSfix::suffix(){
    return qobject_cast<QLabel*>(target(kSuffix));
}
