#include "preview_text.h"
#include "preview_utils.h"

PreviewText::PreviewText(QWidget *parent) : PreviewFrame(parent)
{
    m_layout->addWidget(m_textEdit = new QTextEdit);
    m_textEdit->setReadOnly(true);
}

void PreviewText::previewFile(const QString &filePath)
{
    m_textEdit->setText(Utils::readStringFromFile(filePath));
}
