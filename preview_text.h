#ifndef PREVIEWTEXT_H
#define PREVIEWTEXT_H

#include <QTextEdit>

#include "preview_frame.h"

class PreviewText : public PreviewFrame
{
    Q_OBJECT
public:
    PreviewText(QWidget *parent = nullptr);
protected:
    void previewFile(const QString &filePath) override;
    QTextEdit *m_textEdit;
};

#endif // PREVIEWTEXT_H
