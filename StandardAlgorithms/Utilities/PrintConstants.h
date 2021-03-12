#pragma once

struct PrintConstants final
{
    PrintConstants() = delete;

    // '{' - beginning marker.
    static const char Begin;

    // '}' - ending marker.
    static const char End;
};