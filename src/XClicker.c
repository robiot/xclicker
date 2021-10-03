/* XClicker, a x11 autoclicker
License: GNU General Public License v3.0
*/

#include <gtk/gtk.h>

bool activated = false;
static void print_hello(GtkWidget *widget, gpointer data)
{
    if (activated == false)
    {
        activated = true;
        g_print("Activated\n");
    }
    else
    {
        activated = false;
        g_print("Disabled\n");
    }
}

static void activate(GtkApplication *app, gpointer user_data)
{
    GtkWidget *window;
    GtkWidget *grid;
    GtkWidget *button;
    
    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "XClicker");
    gtk_window_set_default_size(GTK_WINDOW(window), 500, 250);
    gtk_window_set_resizable(GTK_WINDOW(window), false);

    grid = gtk_grid_new();
    gtk_window_set_child(GTK_WINDOW(window), grid);
    // Margin
    gtk_widget_set_margin_top(grid, 20);
    gtk_widget_set_margin_bottom(grid, 20);
    gtk_widget_set_margin_start(grid, 20);
    gtk_widget_set_margin_end(grid, 20);

    button = gtk_button_new_with_label("Start (F6)");
    g_signal_connect(button, "clicked", G_CALLBACK(print_hello), NULL);
    
    gtk_grid_attach(GTK_GRID(grid), button, 2, 1, 2, 1);
    
    //gtk_widget_set_sensitive(button, false);   // Disable

    // Show after load
    gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new("org.gtk.XClicker", G_APPLICATION_FLAGS_NONE);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    return status;
}
