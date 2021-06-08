#include "ComboBoxOtherInput.h"
#include "../Utils/XXjson.h"
#include "../Utils/XXlogger.h"

XX_KEY_C(ComboBoxOtherInput,kComboBox,"comboBox");
XX_KEY_C(ComboBoxOtherInput,kLineEdit,"lineEdit");

XX_KEY_C(ComboBoxOtherInput,kOptional,"optional");
XX_KEY_C(ComboBoxOtherInput,kOtherIndex,"otherIndex");

XX_KEY_C(ComboBoxOtherInput,kGetText,"getText");
XX_KEY_C(ComboBoxOtherInput,kSetText,"setText");

ComboBoxOtherInput::ComboBoxOtherInput(QObject *parent)
    : ShellBase(parent){

}

void ComboBoxOtherInput::shell(QComboBox *comboBox,QLineEdit *lineEdit){
    TargetMap map;
    map[kComboBox] = comboBox;
    map[kLineEdit] = lineEdit;
    ShellBase::shell(map);
}

void ComboBoxOtherInput::config(const QVariant &param){
    QStringList optionals;
    int otherIndex = -1;

    if(param.canConvert<QVariantMap>()){
        QVariantMap map = param.toMap();
        if(map.contains(kOptional)){
           optionals = map[kOptional].toStringList();
        }
        if(map.contains(kOtherIndex)){
            otherIndex = map[kOtherIndex].toInt();
        }
    }
    else if(param.canConvert<QStringList>()){
        optionals = param.toStringList();
    }
    else{
        xxLog("param could not convert to QVariantMap/QList<QPair<QString,QString>>. type:%d", param.type());
        return;
    }
    config(optionals,otherIndex);
}
void ComboBoxOtherInput::config(const QStringList &optionals, int otherIndex){
    QComboBox *box = comboBox();
    QLineEdit *edit = lineEdit();

    box->clear();
    edit->clear();

    _optionals.clear();
    _otherIndex = otherIndex;
    foreach(auto optional,optionals){
        QPair<QString,QString> item;
        if(optional.contains(':')){
            item.first = optional.section(':',0,0);
            item.second = optional.section(':',1);
        }
        else{
            item.first = optional;
            item.second = optional;
        }
        _optionals << item;
        comboBox()->addItem(item.second,item.first);
    }

    edit->hide();
}

QVariant ComboBoxOtherInput::action(const QString &action,const QVariant &param){
    if(kGetText == action){
        return getText();
    }
    else if(kSetText == action){
        setText(param.toString());
    }
    else {

    }
    return QVariant();
}
QString ComboBoxOtherInput::getText(){
    int currentIndex = comboBox()->currentIndex();
    if(currentIndex<0){
        return "";
    }
    if(lineEdit()->isHidden()){
        return comboBox()->currentText();
    }
    else{
        return lineEdit()->text();
    }
}
void ComboBoxOtherInput::setText(const QString &text){
    if(_optionals.isEmpty()){
        return;
    }

    int target = -1;
    for (int index=0; index<_optionals.count(); index++) {
        if(_optionals[index].second == text){
            target = index;
            break;
        }
    }

    if(target == _otherIndex){
        lineEdit()->setText(text);
        target = _otherIndex<0?_optionals.count()-1:target;
    }
    else if(-1 == target){
        return;
    }
    else{
        lineEdit()->setText("");
    }
    comboBox()->setCurrentIndex(target);
}

void ComboBoxOtherInput::resetTarget(const QString &name, QObject *target){
    ShellBase::resetTarget(name,target);
    if(kLineEdit == name){
        qobject_cast<QWidget*>(target)->hide();
    }
    else if(kComboBox == name){
        connect(target,SIGNAL(currentIndexChanged(int)),this,SLOT(comboBox_onCurrentIndexChanged(int)));
    }
    else{

    }
}

QLineEdit* ComboBoxOtherInput::lineEdit(){
    return qobject_cast<QLineEdit*>(target(kLineEdit));
}
QComboBox* ComboBoxOtherInput::comboBox(){
    return qobject_cast<QComboBox*>(target(kComboBox));
}
void ComboBoxOtherInput::comboBox_onCurrentIndexChanged(int index){
    QLineEdit *text = lineEdit();
    QComboBox *box = comboBox();

    if(index<0 || 0==box->count()){
        text->hide();
        return;
    }

    text->setHidden(index!=_otherIndex && (-1==_otherIndex&&index+1!=box->count()));
}
