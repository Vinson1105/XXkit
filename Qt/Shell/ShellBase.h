#ifndef ShellBase_h
#define ShellBase_h

#include <QObject>
#include <QVariant>

class ShellBase : public QObject
{
	Q_OBJECT
public:
	ShellBase(QObject *target = nullptr, QObject *parent = nullptr);
	virtual ~ShellBase();

	virtual void shell(QObject *target);
	virtual void config(const QVariant &param) {}

	Q_SIGNAL void sigEvent(QString event, QVariant info);

protected:
	virtual void resetTarget(QObject *target);
	QObject* target();

	Q_SLOT virtual void onTargetDeleted(QObject *object);

private:
	QObject *_target;
};

#endif
