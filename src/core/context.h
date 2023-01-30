#pragma once

#include <memory>
#include <string>
#include <list>
#include <functional>
#include "helpers/fs.h"

struct Action;

class Texture;

using MenuBar = std::list<std::pair<std::string, std::function<void()>>>;
using ActionList = std::list<std::shared_ptr<Action>>;

class Context
{
public:
    const static MenuBar &GetMenuBar() { return m_MenuBar; }
    static void ProcessPopups();
    static void AppendPopup(const char *path) { m_PopupsToOpen.push_back(path); }

    static void Undo();
    static void Redo();

    static bool CanUndo() { return !m_UndoStack.empty(); } 
    static bool CanRedo() { return !m_RedoStack.empty(); }

    static void LogAction(Action *action);
private:
    static std::list<const char *> m_PopupsToOpen;
    static MenuBar m_MenuBar;
    static ActionList m_UndoStack;
    static ActionList m_RedoStack;
};

