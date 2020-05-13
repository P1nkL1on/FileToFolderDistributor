#ifndef DISTRIBUTOR_H
#define DISTRIBUTOR_H

#include <QBoxLayout>
#include <QWidget>

class PreviewFrame : public QWidget
{
    Q_OBJECT
public:
    PreviewFrame(QWidget *parent = nullptr);
    virtual void previewFile(const QString &filePath);
protected:
    QVBoxLayout *m_layout;
};

#endif // DISTRIBUTOR_H
