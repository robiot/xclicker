/** XClicker, a x11 autoclicker
 * All source files in this repository are licensed under the
 * GNU General Public License v3.0.
 * Dependencies are licensed by their own.
 *
 * https://github.com/robiot/xclicker
 */

#include <gtk/gtk.h>
#include <X11/Xlib.h>
#include "xclicker-app.h"
#include "config.h"

int main(int argc, char *argv[])
{
    XInitThreads();
    srand(time(NULL));

    return g_application_run(G_APPLICATION(xclicker_app_new()), argc, argv);
}
