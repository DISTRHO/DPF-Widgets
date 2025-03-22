/*
 * Syntax highlighting text editor (for ImGui in DPF)
 * Copyright (C) 2021-2023 Filipe Coelho <falktx@falktx.com>
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

#pragma once

#include "DearImGui.hpp"

#include <string>
#include <vector>

START_NAMESPACE_DGL

// --------------------------------------------------------------------------------------------------------------------

// TODO integrate fixes from https://github.com/BalazsJako/ImGuiColorTextEdit/pulls

/**
   Text Editor Widget class, based on ImGuiColorTextEdit.
 */
template <class BaseWidget>
class ImGuiTextEditor : public BaseWidget
{
public:
   /**
      Constructor for a ImGuiTextEditorSubWidget.
    */
    explicit ImGuiTextEditor(Widget* parentGroupWidget);

   /**
      Constructor for a ImGuiTextEditorTopLevelWidget.
    */
    explicit ImGuiTextEditor(Window& windowToMapTo);

   /**
      Destructor.
    */
    virtual ~ImGuiTextEditor();

   /**
      Methods from internal TextEdit.
    */
    void setText(const std::string& text);
    std::string getText() const;

    void setTextLines(const std::vector<std::string>& lines);
    std::vector<std::string> getTextLines() const;

    std::string getSelectedText() const;
    std::string getCurrentLineText() const;

	bool hasTextChangedSinceLastTime();

protected:
   /**
      Whether to show top-bar menu.
    */
    void showMenu(bool show = true);

    /** @internal */
    void onImGuiDisplay() override;

private:
    struct TextEditorPrivateData;
    TextEditorPrivateData* const teData;
    friend class ImGuiTextEditorStandaloneWindow;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiTextEditor)
};

typedef ImGuiTextEditor<ImGuiSubWidget> ImGuiTextEditorSubWidget;
typedef ImGuiTextEditor<ImGuiTopLevelWidget> ImGuiTextEditorTopLevelWidget;

// --------------------------------------------------------------------------------------------------------------------

/**
   Special Text Editor Standalone class.
   Will show a menu bar and handle file open, save and quit operations internally.

   The reason for the separate class is so that we can override onFileSelected from Window.
   This method is not available for widgets, only the Window.
 */
class ImGuiTextEditorStandaloneWindow : public StandaloneWindow
{
public:
   /**
      Direct access to the text editor.
    */
    ImGuiTextEditor<ImGuiTopLevelWidget> editor;

   /**
      Constructor for a ImGuiTextEditorStandaloneWindow without transient parent window.
    */
    explicit ImGuiTextEditorStandaloneWindow(Application& app);

   /**
      Constructor for a ImGuiTextEditorStandaloneWindow with transient parent window.
    */
    explicit ImGuiTextEditorStandaloneWindow(Application& app, Window& transientParentWindow);

protected:
    /** @internal */
    void onDisplay() override;
    /** @internal */
    void onFileSelected(const char* filename) override;

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ImGuiTextEditorStandaloneWindow)
};

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DGL
