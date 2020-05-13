#ifndef PREVIEWIMAGE_H
#define PREVIEWIMAGE_H

#include <QLabel>
#include <QPixmap>

#include "preview_frame.h"

class PreviewImage : public PreviewFrame
{
    Q_OBJECT
public:
    PreviewImage(QWidget *parent = nullptr);
    void previewFile(const QString &filePath) override;
protected:
    QLabel *m_label;
};


#endif // PREVIEWIMAGE_H
