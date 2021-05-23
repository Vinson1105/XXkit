#include "LineEditShell.h"

const char * const LineEditShell::kExpression = "expression";
const char * const LineEditShell::kMaxLength = "maxLength";
const char * const LineEditShell::kStrongMatchMode = "strongMatchMode";

LineEditShell::LineEditShell(QLineEdit *target, QObject *parent) 
	:ShellBase(target,parent){
	_expression = nullptr;
	_maxLength = 0;
	_strongMatchMode = false;
	_isMatching = false;
}

void LineEditShell::config(const QVariant &param) {
	if (param.type() == QVariant::String) {
		config(param.toString());
	}
	else if (param.type() == QVariant::Map) {
		config(param.toMap());
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

		disconnect(lineEdit, &QLineEdit::textEdited, this, &LineEditShell::onTextChanged);
		disconnect(lineEdit, &QLineEdit::textChanged, this, &LineEditShell::onTextChanged);
	}
	else {
		QLineEdit *lineEdit = qobject_cast<QLineEdit*>(target);
		if (nullptr == lineEdit) {
			return;
		}

		connect(lineEdit, &QLineEdit::textEdited, this, &LineEditShell::onTextChanged);
		connect(lineEdit, &QLineEdit::textChanged, this, &LineEditShell::onTextChanged);
	}
}

void LineEditShell::config(const QString &expression) {
	_expression.reset(new QRegExp(expression));
}
void LineEditShell::config(const QVariantMap &map) {
	if (map.contains(kExpression) && QVariant::String==map[kExpression].type()) {
		_expression.reset(new QRegExp(map[kExpression].toString()));
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
		target->setText(_lastText);
		_isMatching = true;
	}

	_lastText = target->text();
}
void LineEditShell::onTextChanged(QString text) {

}
