#include "preview_frame.h"

PreviewFrame::PreviewFrame(QWidget *parent) : QWidget(parent)
{
    setLayout(m_layout = new QVBoxLayout);
}

void PreviewFrame::previewFile(const QString &filePath)
{
    Q_UNUSED(filePath)
    Q_ASSERT_X(false, "PreviewFrame::previewFile", "Not implemtented!");
}
