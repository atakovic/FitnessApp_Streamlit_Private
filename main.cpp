//https://docs.gtk.org/gtk4/
//https://gnome.pages.gitlab.gnome.org/gtkmm-documentation/
//https://gnome.pages.gitlab.gnome.org/gtkmm/

#include <gtkmm.h> //header for gtkmm
#include <gtkmm/application.h>
#include "1_MyWindow.h"
#include "3_database.h"


int main(int argc, char* argv[]) {
    auto app = Gtk::Application::create("org.alentest.fitnessapp");
        // Gtk::Application object, stored in a Glib::RefPtr smartpointer
        // is needed in all gtkmm applications




    return app->make_window_and_run<MyWindow>(argc, argv);       //creates and shows the window-object
    //starting with MyWindow


}
