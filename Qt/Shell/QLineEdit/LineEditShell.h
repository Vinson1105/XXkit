#ifndef LineEditShell_h
#define LineEditShell_h

#include "../ShellBase.h"
#include <QLineEdit>
#include <QRegExp>
#include <QSharedPointer>

class LineEditShell : public ShellBase
{
	Q_OBJECT
public:
	LineEditShell(QLineEdit *target = nullptr, QObject *parent = nullptr);

	/**
	 expression:正则表达式
	 maxLength:最大长度，默认是0，不作闲置
	 strongMatchMode:是否为强制匹配模式，默认是false关闭
	 */
	virtual void config(const QVariant &param);

	const static char * const kExpression;
	const static char * const kMaxLength;
	const static char * const kStrongMatchMode;

protected:
	virtual void resetTarget(QObject *target);

private:
	void config(const QString &expression);
	void config(const QVariantMap &map);

	Q_SLOT void onTextEdited(QString text);
	Q_SLOT void onTextChanged(QString text);

private:
	QSharedPointer<QRegExp> _expression;
	int _maxLength;
	bool _strongMatchMode;
	bool _isMatching;
	QString _lastText;
};


#endif // !LineEditShell_h
