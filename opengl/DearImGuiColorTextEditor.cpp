/*
 * Syntax highlighting text editor (for ImGui in DPF)
 * Copyright (C) 2021 Filipe Coelho <falktx@falktx.com>
 * Copyright (c) 2017 BalazsJako
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "DearImGuiColorTextEditor.hpp"

#ifndef IMGUI_SKIP_IMPLEMENTATION
# include "DearImGuiColorTextEditor/TextEditor.cpp"
#else
# include "DearImGuiColorTextEditor/TextEditor.h"
#endif

#include <fstream>

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
struct ImGuiTextEditor<BaseWidget>::PrivateData {
    ImGuiTextEditor<BaseWidget>* const self;
    TextEditor editor;
    std::string file;
    bool showMenu;

    explicit PrivateData(ImGuiTextEditor<BaseWidget>* const s)
        : self(s),
          showMenu(false)
    {
        // https://github.com/BalazsJako/ColorTextEditorDemo/blob/master/main.cpp

        editor.SetLanguageDefinition(TextEditor::LanguageDefinition::CPlusPlus());

        // TESTING
        std::ifstream t("/Shared/Personal/FOSS/GIT/DISTRHO/DISTRHO_OneKnob-Series/dpf/dgl/Image.hpp");

        if (t.good())
        {
            const std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
            editor.SetText(str);
            file = "/Shared/Personal/FOSS/GIT/DISTRHO/DISTRHO_OneKnob-Series/dpf/dgl/Image.hpp";
        }
    }

    void renderMenuContent()
    {
        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("File"))
            {
                if (ImGui::MenuItem("Open..."))
                {
                    self->getWindow().openFileBrowser();
                }
                if (ImGui::MenuItem("Save", "Ctrl+S", nullptr, file.size() != 0))
                {
                    // auto textToSave = editor.GetText();
                    /// save text....
                }
                if (ImGui::MenuItem("Quit", "Ctrl+Q"))
                {
                    self->getWindow().getApp().quit();
                }
                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Edit"))
            {
                bool ro = editor.IsReadOnly();

                if (ImGui::MenuItem("Read-only mode", nullptr, &ro))
                    editor.SetReadOnly(ro);

                ImGui::Separator();

                if (ImGui::MenuItem("Undo", "Ctrl-Z", nullptr, !ro && editor.CanUndo()))
                    editor.Undo();
                if (ImGui::MenuItem("Redo", "Ctrl-Y", nullptr, !ro && editor.CanRedo()))
                    editor.Redo();

                ImGui::Separator();

                if (ImGui::MenuItem("Copy", "Ctrl-C", nullptr, editor.HasSelection()))
                    editor.Copy();
                if (ImGui::MenuItem("Cut", "Ctrl-X", nullptr, !ro && editor.HasSelection()))
                    editor.Cut();
                if (ImGui::MenuItem("Delete", "Del", nullptr, !ro && editor.HasSelection()))
                    editor.Delete();
                if (ImGui::MenuItem("Paste", "Ctrl-V", nullptr, !ro && ImGui::GetClipboardText() != nullptr))
                    editor.Paste();

                ImGui::Separator();

                if (ImGui::MenuItem("Select all", "Ctrl-A", nullptr))
                    editor.SetSelection(TextEditor::Coordinates(), TextEditor::Coordinates(editor.GetTotalLines(), 0));

                ImGui::EndMenu();
            }

            if (ImGui::BeginMenu("View"))
            {
                if (ImGui::MenuItem("Dark palette"))
                    editor.SetPalette(TextEditor::GetDarkPalette());
                if (ImGui::MenuItem("Light palette"))
                    editor.SetPalette(TextEditor::GetLightPalette());
                if (ImGui::MenuItem("Retro blue palette"))
                    editor.SetPalette(TextEditor::GetRetroBluePalette());
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }
    }

    DISTRHO_DECLARE_NON_COPYABLE(PrivateData)
};

// --------------------------------------------------------------------------------------------------------------------

template <>
ImGuiTextEditor<ImGuiSubWidget>::ImGuiTextEditor(Widget* const parent)
    : ImGuiSubWidget(parent),
      pData(new PrivateData(this)) {}

template <>
ImGuiTextEditor<ImGuiTopLevelWidget>::ImGuiTextEditor(Window& windowToMapTo)
    : ImGuiTopLevelWidget(windowToMapTo),
      pData(new PrivateData(this)) {}

template <class BaseWidget>
ImGuiTextEditor<BaseWidget>::~ImGuiTextEditor()
{
    delete pData;
}

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
void ImGuiTextEditor<BaseWidget>::setText(const std::string& text)
{
    pData->editor.SetText(text);
}

template <class BaseWidget>
std::string ImGuiTextEditor<BaseWidget>::getText() const
{
    return pData->editor.GetText();
}

template <class BaseWidget>
void ImGuiTextEditor<BaseWidget>::setTextLines(const std::vector<std::string>& lines)
{
    pData->editor.SetTextLines(lines);
}

template <class BaseWidget>
std::vector<std::string> ImGuiTextEditor<BaseWidget>::getTextLines() const
{
    return pData->editor.GetTextLines();
}

template <class BaseWidget>
std::string ImGuiTextEditor<BaseWidget>::getSelectedText() const
{
    return pData->editor.GetSelectedText();
}

template <class BaseWidget>
std::string ImGuiTextEditor<BaseWidget>::getCurrentLineText()const
{
    return pData->editor.GetCurrentLineText();
}

// --------------------------------------------------------------------------------------------------------------------

template <class BaseWidget>
void ImGuiTextEditor<BaseWidget>::onImGuiDisplay()
{
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize;
    if (pData->showMenu)
        flags |= ImGuiWindowFlags_MenuBar;

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(BaseWidget::getWidth(), BaseWidget::getHeight()));

    if (ImGui::Begin("TextEdit", nullptr, flags))
    {
        if (pData->showMenu)
            pData->renderMenuContent();

        TextEditor& editor(pData->editor);

        const TextEditor::Coordinates cpos = editor.GetCursorPosition();

        ImGui::Text("%6d/%-6d %6d lines  | %s | %s | %s | %s", cpos.mLine + 1, cpos.mColumn + 1, editor.GetTotalLines(),
                    editor.IsOverwrite() ? "Ovr" : "Ins",
                    editor.CanUndo() ? "*" : " ",
                    editor.GetLanguageDefinition().mName.c_str(), pData->file.c_str());

        editor.Render("TextEditor");
    }

    ImGui::End();
}

// --------------------------------------------------------------------------------------------------------------------

template class ImGuiTextEditor<ImGuiSubWidget>;
template class ImGuiTextEditor<ImGuiTopLevelWidget>;

// --------------------------------------------------------------------------------------------------------------------

ImGuiTextEditorStandaloneWindow::ImGuiTextEditorStandaloneWindow(Application& app)
    : StandaloneWindow(app),
      editor(*this)
{
    editor.pData->showMenu = true;
}

ImGuiTextEditorStandaloneWindow::ImGuiTextEditorStandaloneWindow(Application& app, Window& transientParentWindow)
    : StandaloneWindow(app, transientParentWindow),
      editor(*this)
{
    editor.pData->showMenu = true;
}

void ImGuiTextEditorStandaloneWindow::onDisplay()
{
}

void ImGuiTextEditorStandaloneWindow::onFileSelected(const char* const filename)
{
    if (filename == nullptr)
        return;

    std::ifstream t(filename);

    if (t.good())
    {
        const std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        editor.pData->editor.SetText(str);
        editor.pData->file = filename;
    }
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
