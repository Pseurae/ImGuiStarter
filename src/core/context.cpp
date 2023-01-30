#include "core/context.h"
#include "helpers/events.h"
#include "helpers/fs.h"
#include "helpers/texture.h"
#include "helpers/logger.h"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <ios>
#include <ostream>
#include <string>
#include <sys/types.h>
#include <vector>
#include "core/actions.h"

std::list<const char *> Context::m_PopupsToOpen;
ActionList Context::m_UndoStack;
ActionList Context::m_RedoStack;

void Context::ProcessPopups()
{
    m_PopupsToOpen.remove_if([](const char *popup) {
        if (!ImGui::IsPopupOpen(popup))
            ImGui::OpenPopup(popup);
        return true;
    });
}

void Context::Undo()
{
    std::shared_ptr<Action> action = m_UndoStack.front();
    action->undo();
    m_UndoStack.pop_front();
    m_RedoStack.push_front(action);
}

void Context::Redo()
{
    std::shared_ptr<Action> action = m_RedoStack.front();
    action->redo();
    m_RedoStack.pop_front();
    m_UndoStack.push_front(action);
}

void Context::LogAction(Action *action)
{
    std::shared_ptr<Action> ptr(action);
    m_UndoStack.push_front(ptr);
    m_RedoStack.clear();

    while (m_RedoStack.size() > 30)
        m_RedoStack.pop_back();
}

MenuBar Context::m_MenuBar = {
};

