#include "LineEditShell.h"
#include <QDebug>

XX_KEY_C(LineEditShell, kExpression, "expression");
XX_KEY_C(LineEditShell, kMaxLength, "maxLength");
XX_KEY_C(LineEditShell, kStrongMatchMode, "strongMatchMode");

XX_KEY_C(LineEditShell, kSigTextChanged, "sigTextChanged");
XX_KEY_C(LineEditShell, kPreviousText, "previousText");
XX_KEY_C(LineEditShell, kCurrentText, "currentText");


LineEditShell::LineEditShell(QLineEdit *target, QObject *parent) 
	:ShellBase(target,parent){
	_expression = nullptr;
	_maxLength = 0;
	_strongMatchMode = false;
	_isMatching = false;
}

void LineEditShell::config(const QVariant &param) {
	if (param.type() == QVariant::String) {
        configExperssion(param.toString());
	}
	else if (param.type() == QVariant::Map) {
        configAll(param.toMap());
	}
	else {

	}
}

void LineEditShell::resetTarget(QObject *target) {
	ShellBase::resetTarget(target);
	if (nullptr == target) {
		QObject *lastTarget = this->target();
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(lastTarget);
		if (nullptr == lineEdit) {
			return;
		}

        disconnect(lineEdit, &QLineEdit::textEdited, this, &LineEditShell::onTextEdited);
		disconnect(lineEdit, &QLineEdit::textChanged, this, &LineEditShell::onTextChanged);
	}
	else {
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(target);
		if (nullptr == lineEdit) {
			return;
		}

        connect(lineEdit, &QLineEdit::textEdited, this, &LineEditShell::onTextEdited);
		connect(lineEdit, &QLineEdit::textChanged, this, &LineEditShell::onTextChanged);
	}
}

void LineEditShell::configExperssion(const QString &expression) {
	_expression.reset(new QRegExp(expression));
}
void LineEditShell::configAll(const QVariantMap &map) {
	if (map.contains(kExpression) && QVariant::String==map[kExpression].type()) {
        configExperssion(map[kExpression].toString());
	}
	else if (map.contains(kMaxLength) && QVariant::Int==map[kMaxLength].type()) {
		_maxLength = map[kMaxLength].toInt();
	}
	else if (map.contains(kStrongMatchMode) && QVariant::Bool == map[kStrongMatchMode].type()) {
		_strongMatchMode = map[kStrongMatchMode].toBool();
	}
	else {

	}
}

void LineEditShell::onTextEdited(QString text) {
    QLineEdit *target = qobject_cast<QLineEdit*>(sender());

	if (nullptr != _expression && !_expression->exactMatch(text)) {
        _isMatching = false;
	}
	if (_maxLength > 0 && text.length() > _maxLength) {
        _isMatching = false;
	}

	if (_strongMatchMode && !_isMatching) {
        target->setText(_previousText);
        _isMatching = true;
	}


    QString previous = _previousText;
    _previousText = target->text();
    QVariantMap info = {{kPreviousText,previous},{kCurrentText,_previousText}};
    emit sigEvent(kSigTextChanged,info);
}
void LineEditShell::onTextChanged(QString text) {

}
