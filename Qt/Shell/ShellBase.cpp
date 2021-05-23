#include "ShellBase.h"
ShellBase::ShellBase(QObject *target, QObject *parent)
	: QObject(parent)
	, _target(nullptr){
	resetTarget(target);
}
ShellBase::~ShellBase() {
	resetTarget(nullptr);
}

void ShellBase::shell(QObject *target) {
	if (nullptr != _target) {
		return;
	}
	resetTarget(target);
}

void ShellBase::resetTarget(QObject *target) {
	if (target == _target) {
		return;
	}

	if (nullptr != _target) {
		disconnect(_target, &QObject::destroyed, this, &ShellBase::onTargetDeleted);
	}

	_target = target;
	if (nullptr != _target) {
		connect(_target, &QObject::destroyed, this, &ShellBase::onTargetDeleted);
	}
}
QObject* ShellBase::target() {
	return _target;
}

void ShellBase::onTargetDeleted(QObject *object) {
	if (object == _target) {
		_target = nullptr;
	}
}