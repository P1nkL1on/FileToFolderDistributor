#include "preview_image.h"

#include <QImageReader>

PreviewImage::PreviewImage(QWidget *parent) : PreviewFrame(parent)
{
    m_layout->addWidget(m_label = new QLabel("Picture File"), 1);
}

void PreviewImage::previewFile(const QString &filePath)
{
    QImageReader r(filePath);
    QImage image = r.read();
    QPixmap pixmap = QPixmap::fromImage(image);
    const int w = m_layout->contentsRect().width();
    const int h = m_layout->contentsRect().height();
    m_label->setPixmap(pixmap.scaled(w, h, Qt::KeepAspectRatio));
}
