#include "preview_image.h"

#include <QImageReader>

PreviewImage::PreviewImage(QWidget *parent) : PreviewFrame(parent)
{
    m_layout->addWidget(m_label = new QLabel("Picture File"), 1);
    m_layout->setAlignment(Qt::AlignCenter);
    m_label->setAlignment(Qt::AlignCenter);
}

void PreviewImage::previewFile(const QString &filePath)
{
    QImageReader r(filePath);
    const bool isGif = r.supportsAnimation() and r.imageCount() > 1;

    const int w = m_layout->contentsRect().width();
    const int h = m_layout->contentsRect().height();
    int iw = r.size().width();
    int ih = r.size().height();

    // iw = 2000, ih = 1000, as = 2
    // w = 400 h = 500

    if (iw > w or ih > h){
        const qreal aspectRatio = qreal(iw) / ih;
        if (iw > w){
            iw = w;
            ih = w / aspectRatio;
        }
        if (ih > h){
            ih = h;
            iw = h * aspectRatio;
        }
    }
    if (isGif){
        if (m_movie){
            m_movie->stop();
            delete m_movie;
        }
        m_movie= new QMovie(filePath);
        m_label->setMovie(m_movie);
        m_movie->setScaledSize(QSize(iw , ih));
        m_movie->start();
    } else {
        QImage image = r.read();
        QPixmap pixmap = QPixmap::fromImage(image);
        m_label->setPixmap(pixmap.scaled(iw, ih));
    }
}
