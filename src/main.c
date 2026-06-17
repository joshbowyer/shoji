/* main.c — Shoji hello-world
 *
 * Tier 2.0 of the Shoji e-ink DE: the smallest libadwaita app that proves the
 * dev loop. Opens a single Adw.ApplicationWindow with the design tokens applied,
 * and renders the "device" — a fake e-ink surface and a centered OLED island.
 *
 * This is the skeleton that becomes the shell. The fake e-ink widget will be
 * replaced with a real e-ink renderer when we move to real hardware (Tier 3).
 * The fake OLED island will be replaced with a real OLED renderer too.
 *
 * Build:  meson setup build && meson compile -C build
 * Run:    ./build/shoji
 */

#include <gtk/gtk.h>
#include <adwaita.h>

/* ----- CSS ----- */

/* Loads data/style.css into the default display. In dev mode, the CSS is read
 * from the local file path. When installed, this should be switched to load
 * from a GResource or the installed share dir.
 */
static void
load_css (void)
{
  GtkCssProvider *provider = gtk_css_provider_new ();
  GFile *file = g_file_new_for_path ("data/style.css");

  if (g_file_query_exists (file, NULL))
    {
      gtk_css_provider_load_from_file (provider, file);
      gtk_style_context_add_provider_for_display (
        gdk_display_get_default (),
        GTK_STYLE_PROVIDER (provider),
        GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    }
  else
    {
      g_warning ("data/style.css not found — design tokens will not apply");
    }

  g_object_unref (file);
  g_object_unref (provider);
}

/* ----- Fake e-ink surface ----- */

/* A Gtk.Box styled to look like the e-ink paper surface. Shows the home
 * screen of the Pocket: clock, weather, AI prompt bar.
 *
 * Eventually this is replaced by a real e-ink renderer that talks to the
 * SPI panel via the e-ink kernel driver. The widget API (build a "screen"
 * with these contents) stays the same.
 */
static GtkWidget *
make_eink_surface (void)
{
  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_size_request (box, 360, 540);
  gtk_widget_add_css_class (box, "eink-surface");

  /* Header: date + time */
  GtkWidget *header = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_widget_set_margin_top (header, 24);
  gtk_widget_set_margin_start (header, 24);
  gtk_widget_set_margin_end (header, 24);
  gtk_widget_add_css_class (header, "eink-header");
  gtk_box_append (GTK_BOX (box), header);

  GtkWidget *date = gtk_label_new ("Tue  ·  Jun 17");
  gtk_widget_set_halign (date, GTK_ALIGN_START);
  gtk_widget_add_css_class (date, "eink-caption");
  gtk_box_append (GTK_BOX (header), date);

  GtkWidget *time = gtk_label_new ("10:56");
  gtk_widget_set_halign (time, GTK_ALIGN_START);
  gtk_widget_add_css_class (time, "eink-time");
  gtk_box_append (GTK_BOX (header), time);

  /* Divider */
  GtkWidget *divider = gtk_separator_new (GTK_ORIENTATION_HORIZONTAL);
  gtk_widget_set_margin_top (divider, 16);
  gtk_widget_set_margin_start (divider, 24);
  gtk_widget_set_margin_end (divider, 24);
  gtk_box_append (GTK_BOX (box), divider);

  /* Weather */
  GtkWidget *weather = gtk_box_new (GTK_ORIENTATION_VERTICAL, 2);
  gtk_widget_set_margin_top (weather, 12);
  gtk_widget_set_margin_start (weather, 24);
  gtk_widget_set_margin_end (weather, 24);
  gtk_box_append (GTK_BOX (box), weather);

  GtkWidget *weather_line = gtk_label_new ("San Antonio  88°F  ☀");
  gtk_widget_set_halign (weather_line, GTK_ALIGN_START);
  gtk_widget_add_css_class (weather_line, "eink-body");
  gtk_box_append (GTK_BOX (weather), weather_line);

  GtkWidget *weather_desc = gtk_label_new ("partly cloudy, light breeze");
  gtk_widget_set_halign (weather_desc, GTK_ALIGN_START);
  gtk_widget_add_css_class (weather_desc, "eink-caption");
  gtk_box_append (GTK_BOX (weather), weather_desc);

  /* AI prompt bar (pinned to bottom) */
  GtkWidget *spacer = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_vexpand (spacer, TRUE);
  gtk_box_append (GTK_BOX (box), spacer);

  GtkWidget *ai_button = gtk_button_new_with_label ("▸  Ask the butler");
  gtk_widget_add_css_class (ai_button, "eink-button-primary");
  gtk_widget_set_margin_bottom (ai_button, 24);
  gtk_widget_set_margin_start (ai_button, 24);
  gtk_widget_set_margin_end (ai_button, 24);
  gtk_box_append (GTK_BOX (box), ai_button);

  return box;
}

/* ----- Fake OLED island ----- */

/* A small centered dark box below the e-ink. Shows the dim idle state.
 *
 * Eventually this is replaced by a real OLED renderer that talks to the
 * I²C display (SH1106/SSD1309). Same widget API, different backend.
 */
static GtkWidget *
make_oled_island (void)
{
  GtkWidget *box = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_size_request (box, 220, 70);
  gtk_widget_set_halign (box, GTK_ALIGN_CENTER);
  gtk_widget_add_css_class (box, "oled-island");

  GtkWidget *left = gtk_label_new ("78%");
  gtk_widget_add_css_class (left, "oled-text");
  gtk_widget_set_margin_start (left, 12);
  gtk_widget_set_halign (left, GTK_ALIGN_START);
  gtk_box_append (GTK_BOX (box), left);

  GtkWidget *spacer = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
  gtk_widget_set_hexpand (spacer, TRUE);
  gtk_box_append (GTK_BOX (box), spacer);

  GtkWidget *right = gtk_label_new ("12:56");
  gtk_widget_add_css_class (right, "oled-text");
  gtk_widget_set_margin_end (right, 12);
  gtk_widget_set_halign (right, GTK_ALIGN_END);
  gtk_box_append (GTK_BOX (box), right);

  return box;
}

/* ----- Window ----- */

static void
activate_cb (GtkApplication *app, gpointer user_data)
{
  GtkWidget *window = adw_application_window_new (GTK_APPLICATION (app));
  gtk_window_set_default_size (GTK_WINDOW (window), 520, 760);
  gtk_window_set_title (GTK_WINDOW (window), "Shoji");
  adw_application_window_set_content (ADW_APPLICATION_WINDOW (window),
    gtk_box_new (GTK_ORIENTATION_VERTICAL, 0));

  /* The "device" is a vertical box containing e-ink + OLED island, centered
   * inside the window so it appears as a physical object. */
  GtkWidget *device = gtk_box_new (GTK_ORIENTATION_VERTICAL, 4);
  gtk_widget_set_halign (device, GTK_ALIGN_CENTER);
  gtk_widget_set_valign (device, GTK_ALIGN_CENTER);
  gtk_widget_set_margin_top (device, 24);
  gtk_widget_set_margin_bottom (device, 24);
  gtk_box_append (GTK_BOX (device), make_eink_surface ());
  gtk_box_append (GTK_BOX (device), make_oled_island ());

  adw_application_window_set_content (ADW_APPLICATION_WINDOW (window), device);
  gtk_window_present (GTK_WINDOW (window));
}

/* ----- Entry point ----- */

int
main (int argc, char *argv[])
{
  AdwApplication *app = adw_application_new ("org.shoji.Shell",
                                              G_APPLICATION_DEFAULT_FLAGS);
  g_signal_connect (app, "activate", G_CALLBACK (activate_cb), NULL);
  load_css ();
  return g_application_run (G_APPLICATION (app), argc, argv);
}
