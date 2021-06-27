#include "SuffixLineEdit.h"
#include "ui_SuffixLineEdit.h"

const char * const SuffixLineEdit::kReturnPrefixWhenEmptyContent = "returnPrefixWhenEmptyContent";
const char * const SuffixLineEdit::kHidePrefixWithoutStartWiths = "hidePrefixWithoutStartWiths";

SuffixLineEdit::SuffixLineEdit(QWidget *parent)
    : QFrame(parent)
    , ui(new Ui::SuffixLineEdit)
    , _returnPrefixWhenEmptyContent(false)
    , _hidePrefixWithoutStartWiths(true){
    ui->setupUi(this);
}

SuffixLineEdit::~SuffixLineEdit(){
    delete ui;
}

void SuffixLineEdit::config(const QVariantMap &param){
    if(param.contains(kReturnPrefixWhenEmptyContent) && param[kReturnPrefixWhenEmptyContent].type() == QVariant::Bool){
        _returnPrefixWhenEmptyContent = param[kReturnPrefixWhenEmptyContent].toBool();
    }
    if(param.contains(kHidePrefixWithoutStartWiths) && param[kHidePrefixWithoutStartWiths].type() == QVariant::Bool){
        _hidePrefixWithoutStartWiths = param[kHidePrefixWithoutStartWiths].toBool();
    }
}
QLabel* SuffixLineEdit::prefix(){
    return ui->label_prefix;
}
QLineEdit* SuffixLineEdit::content() {
    return ui->lineEdit_content;
}

QString SuffixLineEdit::text(){
    QString prefix = ui->label_prefix->text();
    QString content = ui->lineEdit_content->text();
    if(!_returnPrefixWhenEmptyContent && content.isEmpty()){
        return "";
    }
    else{
        return prefix+content;
    }
}
void SuffixLineEdit::setText(const QString &text){
    QString prefix = ui->label_prefix->text();
    if(!prefix.isEmpty() && text.startsWith(prefix)){
        if(_hidePrefixWithoutStartWiths){
            hidePrefix(false);
        }
        ui->lineEdit_content->setText(text.right(text.length()-prefix.length()));
    }
    else{
        if(_hidePrefixWithoutStartWiths){
            // 尽管text是没有前缀，但是由于的空字符串，这里是不需要隐藏前缀控件
            hidePrefix(!text.isEmpty());
        }
        ui->lineEdit_content->setText(text);
    }
}

void SuffixLineEdit::hidePrefix(bool hide){
    if(hide){
        ui->label_prefix->hide();
    }
    else{
        ui->label_prefix->show();
    }
}
