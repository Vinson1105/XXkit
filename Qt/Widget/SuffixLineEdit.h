#ifndef SUFFIXLINEEDIT_H
#define SUFFIXLINEEDIT_H

#include <QFrame>
#include <QLabel>
#include <QLineEdit>

namespace Ui {
class SuffixLineEdit;
}

class SuffixLineEdit : public QFrame{
    Q_OBJECT
public:
    explicit SuffixLineEdit(QWidget *parent = nullptr);
    ~SuffixLineEdit();

    /**
     * @brief config 配置一些基本参数
     * @param param 支持的key有以下
     *
     * "returnPrefixWhenEmptyContent":bool
     * "hidePrefixWithoutStartWiths":bool
     */
    void config(const QVariantMap &param);

    /**
     * @brief suffix
     * @return 返回前缀的label控件
     */
    QLabel* prefix();
    /**
     * @brief content
     * @return 返回内容的content控件
     */
    QLineEdit* content();

    QString text();
    void setText(const QString &text);

public:
    static const char * const kReturnPrefixWhenEmptyContent;
    static const char * const kHidePrefixWithoutStartWiths;

private:
    void hidePrefix(bool hide);

private:
    Ui::SuffixLineEdit *ui;
    bool _returnPrefixWhenEmptyContent;    // 是否content为空是返回前缀，默认为false
    bool _hidePrefixWithoutStartWiths;      // 是否当text不是以前缀开头，如果不是就隐藏prefix，默认为true
};

#endif // SUFFIXLINEEDIT_H
