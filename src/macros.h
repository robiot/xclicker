#ifndef __MACROS_H
#define __MACROS_H

#include <gtk/gtk.h>

#define UNUSED(x) UNUSED_##x __attribute__((__unused__))

/**
 * Not really a macro, but fits best in this file.
 * @brief Print error with custom formating
 * 
 * @param when 
 * @param code 
 */
static void xapp_error(const char *when, int code)
{
    g_printerr("Something went wrong when: '%s'. Please report errors like this.\n", when);
    
    if (code == -1) {
        return;
    }

    exit(code);
}

#endif
