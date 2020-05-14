#include "preview_frame.h"

PreviewFrame::PreviewFrame(QWidget *parent) : QWidget(parent)
{
    QVBoxLayout *l = new QVBoxLayout;
    setLayout(l);
    l->addLayout(m_layout = new QVBoxLayout, 1);
    l->addWidget(m_titleLabel = new QLabel());
}

void PreviewFrame::setFile(const QString &filePath)
{
    m_titleLabel->setText(filePath);
    previewFile(filePath);
}

void PreviewFrame::previewFile(const QString &filePath)
{
    Q_UNUSED(filePath)
    Q_ASSERT_X(false, "PreviewFrame::previewFile", "Not implemtented!");
}
