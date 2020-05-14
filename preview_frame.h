#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <QBoxLayout>
#include <QLabel>

class PreviewFrame : public QWidget
{
    Q_OBJECT
public:
    PreviewFrame(QWidget *parent = nullptr);
    void setFile(const QString &filePath);
protected:
    virtual void previewFile(const QString &filePath);
    QVBoxLayout *m_layout;
    QLabel *m_titleLabel;
};

#endif // DISTRIBUTOR_H
