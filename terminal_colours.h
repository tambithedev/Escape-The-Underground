//Created by chatgpt. Honestly, I'm not sure if I'll end up including colours.

#ifndef TERMINAL_COLORS_H
#define TERMINAL_COLORS_H

#include <iostream>
#include <string>

#ifdef _WIN32
#include <windows.h>
#endif

namespace termcolour {

    inline void enable() {
    #ifdef _WIN32
        static bool enabled = false;
        if (enabled) return;

        HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
        if (hOut == INVALID_HANDLE_VALUE) return;

        DWORD mode = 0;
        if (!GetConsoleMode(hOut, &mode)) return;

        mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
        SetConsoleMode(hOut, mode);

        enabled = true;
    #endif
    }

    // Toggle this if you want a fallback (e.g., disable colours)
    inline bool use_colour = true;

    inline std::string wrap(const std::string& code, const std::string& text) {
        if (!use_colour) return text;
        return code + text + "\033[0m";
    }

    // Basic colours
    const std::string RED     = "\033[31m";
    const std::string GREEN   = "\033[32m";
    const std::string YELLOW  = "\033[33m";
    const std::string BLUE    = "\033[34m";
    const std::string MAGENTA = "\033[35m";
    const std::string CYAN    = "\033[36m";

    // Styles
    const std::string BOLD      = "\033[1m";
    const std::string UNDERLINE = "\033[4m";

    // Helper functions
    inline std::string red(const std::string& text)     { return wrap(RED, text); }
    inline std::string green(const std::string& text)   { return wrap(GREEN, text); }
    inline std::string yellow(const std::string& text)  { return wrap(YELLOW, text); }
    inline std::string blue(const std::string& text)    { return wrap(BLUE, text); }
    inline std::string magenta(const std::string& text) { return wrap(MAGENTA, text); }
    inline std::string cyan(const std::string& text)    { return wrap(CYAN, text); }

    inline std::string bold(const std::string& text) {
        return wrap(BOLD, text);
    }

    inline std::string underline(const std::string& text) {
        return wrap(UNDERLINE, text);
    }

} // namespace termcolour

#endif
