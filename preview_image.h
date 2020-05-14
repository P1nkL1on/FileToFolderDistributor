#ifndef PREVIEWIMAGE_H
#define PREVIEWIMAGE_H

#include <QLabel>
#include <QPixmap>
#include <QMovie>

#include "preview_frame.h"

class PreviewImage : public PreviewFrame
{
    Q_OBJECT
public:
    PreviewImage(QWidget *parent = nullptr);
protected:
    void previewFile(const QString &filePath) override;
    QLabel *m_label;
    QMovie *m_movie = nullptr;
};


#endif // PREVIEWIMAGE_H
