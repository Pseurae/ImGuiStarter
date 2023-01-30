#pragma once

#include <stdint.h>

struct Action
{
    virtual ~Action() = default;
    virtual void undo() = 0;
    virtual void redo() = 0;
};

