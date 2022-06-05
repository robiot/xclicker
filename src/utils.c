#include <utils.h>

#include <gtk/gtk.h>
#include <stdlib.h>

void xapp_error(const char *when, int code)
{
    g_printerr("Something went wrong when: '%s'. Please report errors like this.\n", when);

    if (code == -1)
        return;

    exit(code);
}
