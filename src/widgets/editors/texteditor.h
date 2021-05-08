#ifndef TEXTEDITOR_H
#define TEXTEDITOR_H

#include <vtextedit/vtexteditor.h>

namespace vnotex
{
    class TextEditor : public vte::VTextEditor
    {
        Q_OBJECT
    public:
        TextEditor(const QSharedPointer<vte::TextEditorConfig> &p_config,
                   const QSharedPointer<vte::TextEditorParameters> &p_paras,
                   QWidget *p_parent = nullptr);
    };
}

#endif // TEXTEDITOR_H
