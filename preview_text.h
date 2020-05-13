#ifndef PREVIEWTEXT_H
#define PREVIEWTEXT_H

#include <QTextEdit>

#include "preview_frame.h"

class PreviewText : public PreviewFrame
{
    Q_OBJECT
public:
    PreviewText(QWidget *parent = nullptr);
    void previewFile(const QString &filePath) override;
protected:
    QTextEdit *m_textEdit;
};

#endif // PREVIEWTEXT_H
