#include <gtk/gtk.h>
#include <libappindicator/app-indicator.h>

#include "xclicker-app.h"
#include "mainwin.h"
#include "utils.h"

struct _XClickerApp
{
	GtkApplication parent;
	AppIndicator *app_indicator;
	MainAppWindow *main_window;
	gboolean window_visible;
};

G_DEFINE_TYPE(XClickerApp, xclicker_app, GTK_TYPE_APPLICATION);

static void xclicker_app_init(XClickerApp *app)
{
	app->app_indicator = NULL;
	app->main_window = NULL;
	app->window_visible = FALSE;
}

/**
 * Creates and shows the system tray icon
 */
static void create_app_indicator(XClickerApp *app)
{
	if (app->app_indicator)
	{
		return;
	}

	// Create the app indicator
	app->app_indicator = app_indicator_new("xclicker", "xclicker", APP_INDICATOR_CATEGORY_APPLICATION_STATUS);
	app_indicator_set_icon_full(app->app_indicator, "xclicker", "XClicker");
	GtkWidget *menu = gtk_menu_new();

	// Show menu item
	GtkWidget *toggle_item = gtk_menu_item_new_with_label("Show Window");
	g_signal_connect_swapped(toggle_item, "activate",
							 G_CALLBACK(show_window), app);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), toggle_item);

	// Separator
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), gtk_separator_menu_item_new());

	// Quit menu item
	GtkWidget *quit_item = gtk_menu_item_new_with_label("Quit");
	g_signal_connect_swapped(quit_item, "activate",
							 G_CALLBACK(xclicker_app_quit), app);
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), quit_item);

	gtk_widget_show_all(menu);

	// Set the menu for the indicator
	app_indicator_set_menu(app->app_indicator, GTK_MENU(menu));

	// Set the indicator active
	app_indicator_set_status(app->app_indicator, APP_INDICATOR_STATUS_ACTIVE);
}

/**
 * Opens up main window.
 */
static void xclicker_app_activate(GApplication *gapp)
{
	XClickerApp *app = XCLICKER_APP(gapp);

	if (!app->main_window)
	{
		app->main_window = main_app_window_new(app);
		app->window_visible = TRUE;

		// Connect to window hide event
		g_signal_connect(app->main_window, "hide", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
		g_signal_connect(app->main_window, "delete-event", G_CALLBACK(gtk_widget_hide_on_delete), NULL);
	}

	if (!app->window_visible)
	{
		gtk_window_present(GTK_WINDOW(app->main_window));
		app->window_visible = TRUE;
	}

	// Create app indicator after window is created
	create_app_indicator(app);

	// Show window immediately on startup
	gtk_window_present(GTK_WINDOW(app->main_window));
	app->window_visible = TRUE;
}

static void xclicker_app_class_init(XClickerAppClass *class)
{
	G_APPLICATION_CLASS(class)->activate = xclicker_app_activate;
}

XClickerApp *xclicker_app_new()
{
	return g_object_new(XCLICKER_APP_TYPE, NULL);
}

/**
 * Shows the main window.
 */
void show_window(XClickerApp *app)
{
	if (!app->main_window)
		return;

	gtk_window_present(GTK_WINDOW(app->main_window));
	app->window_visible = TRUE;
}

/**
 * Quits the application completely.
 */
void xclicker_app_quit(XClickerApp *app)
{
	g_application_quit(G_APPLICATION(app));
}
