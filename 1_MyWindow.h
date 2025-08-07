#ifndef GTKMM_MYWINDOW_H
#define GTKMM_MYWINDOW_H

#include <gtkmm.h>
#include "2_Add_Workout.h"
#include "3_database.h"
#include "4_Workout.h"
#include <vector>
#include <string>

class MyWindow : public Gtk::Window
{
public:
    MyWindow();
    virtual ~MyWindow();

protected:
    void on_start_clicked();
    void show_message(const Glib::ustring& text);
    void on_all_oberkoerper_toggled();
    void on_all_unterkoerper_toggled();
    std::vector<WorkoutItem> generate_workout_plan(const std::vector<std::string>& muscles,
                                                  int total_sec, int exercise_sec, int pause_sec);

protected:

    //Child widgets:
    Gtk::Frame m_Frame;
    Gtk::Box m_Box;                          //erschafft eine Box für die Inhalte
    Gtk::CenterBox mc_Box_OK;                //erschafft einen Grid für Buttons
    Gtk::CenterBox mc_Box_UK;                //erschafft einen Grid für Buttons
    Gtk::Box m_Box_OK;                       //erschafft einen Grid für Buttons
    Gtk::Box m_Box_UK;                       //erschafft einen Grid für Buttons
    Gtk::Stack m_Stack;                      //erschafft einen Sidebar - Zusatz - Pflicht
    Gtk::Box m_SidebarBox{Gtk::Orientation::HORIZONTAL}; //erschafft eine zusätzliche Box für den Container
    Gtk::StackSidebar m_Sidebar;             //erschafft einen Sidebar

    int row;                            //Reihe für Grid
    int col;                            //Spalte für Grid
    std::vector<Gtk::CheckButton*> checkbuttons_oberkoerper;
    std::vector<Gtk::CheckButton*> checkbuttons_unterkoerper;

    Gtk::Button m_Button_start;         //erschaffe Button: starte das Training

    Gtk::Scale m_Scale_Gesamtzeit;      //setze die Gesamtzeit des Trainings
    Gtk::Scale m_Scale_Pausenzeit;      //setze die Pausenzeit zwischen den Übungen
    Gtk::Scale m_Scale_Uebungszeit;     //setze die Übungszeit einzelner Übungen

    Add_Workout* m_AddWorkoutView{nullptr};
    database* m_databaseView{nullptr};
    WorkoutView* m_workoutView{nullptr};
    Glib::RefPtr<Gtk::StackPage> m_workoutPage;

};

#endif //GTKMM_MYWINDOW_H