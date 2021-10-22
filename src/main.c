/* XClicker, a x11 autoclicker
License: GNU General Public License v3.0
*/

#include <gtk/gtk.h>
#include "xclicker-app.h"

int main(int argc, char *argv[])
{
    return g_application_run(G_APPLICATION(xclicker_app_new()), argc, argv);
}
