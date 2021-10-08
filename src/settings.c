#include <gtk/gtk.h>

void settings_dialog_new()
{
    GtkBuilder *builder = gtk_builder_new_from_resource("/org/gtk/xclicker/ui/settings-dialog.ui");
    GtkDialog *dialog = GTK_DIALOG(gtk_builder_get_object(builder, "dialog"));
    gtk_dialog_run(dialog);

    // Else you will have to click twice to close the dialog
    gtk_widget_destroy(GTK_WIDGET(dialog));
}