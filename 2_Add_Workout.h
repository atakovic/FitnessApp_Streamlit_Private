//
// Created by alen on 07.06.25.
//

#ifndef INC_2_ADD_WORKOUT_H
#define INC_2_ADD_WORKOUT_H
#include <gtkmm.h>
#include <map>
#include <vector>

class Add_Workout : public Gtk::Box
{
public:
    Add_Workout();
    virtual ~Add_Workout();

    sigc::signal<void()> signal_workout_added() { return m_signal_workout_added; }

protected:

    //Child widgets
    Gtk::Frame m_Frame;
    Gtk::Box m_Box;                                     //erschafft eine Box für die Inhalte

    //Listbox
    Gtk::ListBox m_ListBox;                             //erschafft eine Liste für Namen(Bezeichner) & InputFeld
    std::map<std::string, Gtk::Entry*> input_entries;   // Zugriff auf Eingaben per Name
    void add_input_row(const std::string& label_text, const std::string& key); //Methode für ListBox
    void add_muscle_row(const std::string& label_text);

    //Muscle selection
    Gtk::Box* m_muscle_box{nullptr};
    std::vector<Gtk::CheckButton*> m_muscle_buttons;

    Gtk::ScrolledWindow scrolled_window;

    //Buttons
    Gtk::Button m_Button_add;                           //erschaffe Button: Füge Inhalte in DB hinzu
    Gtk::Button m_Button_reset;                         //erschaffe Button: Resete Inhalte aus Page

    void on_add_clicked();
    void on_reset_clicked();
    bool validate_inputs();
    void show_message(const Glib::ustring& text);

    sigc::signal<void()> m_signal_workout_added;


};

#endif //INC_2_ADD_WORKOUT_H