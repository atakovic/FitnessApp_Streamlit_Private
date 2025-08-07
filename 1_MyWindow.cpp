//
// Created by alen on 20.05.25.
//
#include "1_MyWindow.h"
#include "2_Add_Workout.h"
#include <string>
#include <fstream>
#include <vector>
#include <algorithm>
#include <random>
#include <iostream>
#include <sstream>

#include "3_database.h"
#include "4_Workout.h"
#include "db_utils.h"

namespace {

std::vector<std::string> parse_csv_line(const std::string& line) {
    std::vector<std::string> result;
    std::string cell;
    bool in_quotes = false;
    for(char c : line) {
        if(c == '"') {
            in_quotes = !in_quotes;
        } else if(c == ',' && !in_quotes) {
            result.push_back(cell);
            cell.clear();
        } else {
            cell += c;
        }
    }
    result.push_back(cell);
    return result;
}

bool read_csv_record(std::istream& is, std::string& record) {
    record.clear();
    if(!std::getline(is, record))
        return false;
    size_t quote_cnt = std::count(record.begin(), record.end(), '"');
    while(quote_cnt % 2 != 0) {
        std::string next_line;
        if(!std::getline(is, next_line))
            break;
        record += '\n';
        record += next_line;
        quote_cnt += std::count(next_line.begin(), next_line.end(), '"');
    }
    return true;
}

} // namespace

/*Schau bei Glade nach, Klicky-Verfahren zur Erstellung der Oberfläche
 *oder Mono-Develop für GTKmm
 *Hinweis: Gtk-Notebook für Sidebar
 */

//First Page

MyWindow::MyWindow() {
    //Window Properties
    set_title("Fitness App");                                           //set window title
    maximize();                                                           //maximize window size
    m_Frame.set_margin(10);
    set_child(m_Frame);

    //Seiteninstanzen erstellen
    m_AddWorkoutView = Gtk::make_managed<Add_Workout>();
    m_databaseView = Gtk::make_managed<database>();
    m_AddWorkoutView->signal_workout_added().connect(sigc::mem_fun(*m_databaseView, &database::load_data));


    //Sidebar
    // Stack-Seite hinzufügen
    m_Stack.set_transition_type(Gtk::StackTransitionType::SLIDE_LEFT_RIGHT);
    // Eine Seite hinzufügen (m_Box enthält alle Einstellungen)
    m_Sidebar.set_stack(m_Stack);

    // Hauptbox zusammenbauen: Sidebar links, Stack rechts
    m_Frame.set_child(m_SidebarBox);
    m_SidebarBox.append(m_Sidebar);
    m_SidebarBox.append(m_Stack);

    //Seite 1 MyWindow - Hauptseite - Training starten
    auto page1 = m_Stack.add(m_Box);                      // Widget hinzufügen
    //page1->(Gtk::make_managed<Gtk::Label>("Hier kannst du Workouts hinzufügen"));
    page1->set_name("Main Page");                                    // interner Name
    page1->set_title("Main Page");                                   // für Sidebar sichtbar

    //Seite 2 Add Workouts
    auto page2_content = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    page2_content->append(*(Gtk::make_managed<Gtk::Label>("Hier kannst du Workouts hinzufügen")));
    auto page2 = m_Stack.add(*m_AddWorkoutView);
    page2->set_name("Add Workouts");
    page2->set_title("Add Workouts");


    //Seite 3 Database
    auto page3_content = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL);
    page3_content->append(*(Gtk::make_managed<Gtk::Label>("Datenbank anzeigen")));
    auto page3 = m_Stack.add(*m_databaseView);
    page3->set_name("database");
    page3->set_title("Database");


    //Frame
    //m_Frame.set_label("First Page");
    //m_Frame.set_label_align(Gtk::Align::CENTER);

    //Box
    m_Box.set_orientation(Gtk::Orientation::VERTICAL);
    m_Box_OK.set_orientation(Gtk::Orientation::HORIZONTAL);
    m_Box_UK.set_orientation(Gtk::Orientation::HORIZONTAL);

    //Grid

    //Slider
    Gtk::Label *label = new Gtk::Label("Gesamtzeit in Minuten");
    m_Scale_Gesamtzeit.set_orientation(Gtk::Orientation::HORIZONTAL);
    m_Scale_Gesamtzeit.add_mark(0, Gtk::PositionType::BOTTOM, "0");
    m_Scale_Gesamtzeit.add_mark(120, Gtk::PositionType::BOTTOM, "120");
    m_Scale_Gesamtzeit.set_digits(0);                                   //Dezimalstellen
    m_Scale_Gesamtzeit.set_range(0, 120);                       //in Minutes
    m_Scale_Gesamtzeit.set_draw_value();                                //zeigt die Values an
    m_Scale_Gesamtzeit.set_value(30);                                   //Startwert
    m_Box.append(*label);
    m_Box.append(m_Scale_Gesamtzeit);

    label = new Gtk::Label("Pausenzeit in Sekunden");
    m_Scale_Pausenzeit.set_orientation(Gtk::Orientation::HORIZONTAL);
    m_Scale_Pausenzeit.add_mark(15, Gtk::PositionType::BOTTOM, "15");
    m_Scale_Pausenzeit.add_mark(120, Gtk::PositionType::BOTTOM, "120");
    m_Scale_Pausenzeit.set_digits(0);                                   //Dezimalstellen
    m_Scale_Pausenzeit.set_draw_value();                                //zeigt die Values an
    m_Scale_Pausenzeit.set_range(15, 120);                      //in Seconds
    m_Scale_Pausenzeit.set_value(15);                                   //Startwert
    m_Box.append(*label);
    m_Box.append(m_Scale_Pausenzeit);

    label = new Gtk::Label("Übungszeit in Sekunden");
    m_Scale_Uebungszeit.set_orientation(Gtk::Orientation::HORIZONTAL);
    m_Scale_Uebungszeit.add_mark(0, Gtk::PositionType::BOTTOM, "0");
    m_Scale_Uebungszeit.add_mark(360, Gtk::PositionType::BOTTOM, "360");
    m_Scale_Uebungszeit.set_digits(0);                                   //Dezimalstellen
    m_Scale_Uebungszeit.set_draw_value();                                //zeigt die Values an
    m_Scale_Uebungszeit.set_range(0, 360);                       //in Seconds
    m_Scale_Uebungszeit.set_value(30);                                   //Startwert
    m_Box.append(*label);
    m_Box.append(m_Scale_Uebungszeit);


    //Buttonanzeige Muskelgruppen
    label = new Gtk::Label("Oberkörper");
    m_Box.append(*label);
    std::vector<std::string> oberkoerper = {"Alle", "Brust", "Schulter", "Arme", "Rücken", "Bauch"};
    for (int i = 0; i < oberkoerper.size(); i++) {
        Gtk::CheckButton *button = new Gtk::CheckButton(oberkoerper[i]);
        checkbuttons_oberkoerper.push_back(button);
        m_Box_OK.append(*button);
    }
    if(!checkbuttons_oberkoerper.empty())
        checkbuttons_oberkoerper.front()->signal_toggled().connect(
                sigc::mem_fun(*this, &MyWindow::on_all_oberkoerper_toggled));
    mc_Box_OK.set_center_widget(m_Box_OK);
    m_Box.append(mc_Box_OK);


    label = new Gtk::Label("Unterkörper");
    m_Box.append(*label);
    std::vector<std::string> unterkoerper = {"Alle", "Beine", "Gesäß", "Waden", "Rücken"};
    for (int i = 0; i < unterkoerper.size(); i++) {
        Gtk::CheckButton *button = new Gtk::CheckButton(unterkoerper[i]);
        checkbuttons_unterkoerper.push_back(button);
        m_Box_UK.append(*button);
        }
    if(!checkbuttons_unterkoerper.empty())
        checkbuttons_unterkoerper.front()->signal_toggled().connect(
                sigc::mem_fun(*this, &MyWindow::on_all_unterkoerper_toggled));
    mc_Box_UK.set_center_widget(m_Box_UK);
    m_Box.append(mc_Box_UK);




    //Button
    m_Button_start.set_label("Start Training");
    m_Button_start.set_margin(10);
    m_Box.append(m_Button_start); //put Button in Box
    m_Button_start.signal_clicked().connect(sigc::mem_fun(*this, &MyWindow::on_start_clicked));

}

MyWindow::~MyWindow()
{
}

void MyWindow::on_start_clicked() {
    bool ok_selected = false;
    bool uk_selected = false;
    std::vector<std::string> muscles_ok;
    std::vector<std::string> muscles_uk;

    if(!checkbuttons_oberkoerper.empty()) {
        if(checkbuttons_oberkoerper.front()->get_active()) {
            ok_selected = true;
            for(size_t i=1;i<checkbuttons_oberkoerper.size();++i)
                muscles_ok.push_back(checkbuttons_oberkoerper[i]->get_label());
        } else {
            for(size_t i=1;i<checkbuttons_oberkoerper.size();++i) {
                if(checkbuttons_oberkoerper[i]->get_active()) {
                    ok_selected = true;
                    muscles_ok.push_back(checkbuttons_oberkoerper[i]->get_label());
                }
            }
        }
    }

    if(!checkbuttons_unterkoerper.empty()) {
        if(checkbuttons_unterkoerper.front()->get_active()) {
            uk_selected = true;
            for(size_t i=1;i<checkbuttons_unterkoerper.size();++i)
                muscles_uk.push_back(checkbuttons_unterkoerper[i]->get_label());
        } else {
            for(size_t i=1;i<checkbuttons_unterkoerper.size();++i) {
                if(checkbuttons_unterkoerper[i]->get_active()) {
                    uk_selected = true;
                    muscles_uk.push_back(checkbuttons_unterkoerper[i]->get_label());
                }
            }
        }
    }

    if(!ok_selected && !uk_selected) {
        show_message("Triff eine Auswahl!");
        return;
    }

    std::vector<std::string> all_muscles = muscles_ok;
    all_muscles.insert(all_muscles.end(), muscles_uk.begin(), muscles_uk.end());

    int this_sec = static_cast<int>(m_Scale_Uebungszeit.get_value());
    int all_sec = static_cast<int>(m_Scale_Gesamtzeit.get_value() * 60);
    int pause_sec = static_cast<int>(m_Scale_Pausenzeit.get_value());

    auto plan = generate_workout_plan(all_muscles, all_sec, this_sec, pause_sec);

    if(!m_workoutView) {
        m_workoutView = Gtk::make_managed<WorkoutView>();
        m_workoutPage = m_Stack.add(*m_workoutView);
        m_workoutPage->set_name("Workout");
        m_workoutPage->set_title("Workout");
        m_workoutView->signal_workout_end().connect([this](){
            m_Stack.set_visible_child(m_Box);
            if(m_workoutView) {
                m_Stack.remove(*m_workoutView);
                m_workoutView = nullptr;
                m_workoutPage.reset();
            }
        });
    }

    m_workoutView->start_plan(plan, this_sec, all_sec, pause_sec);
    m_Stack.set_visible_child(*m_workoutView);
}

void MyWindow::show_message(const Glib::ustring& text) {
    auto* win = this;
    auto dialog = Gtk::make_managed<Gtk::MessageDialog>(*win, text, false,
                                  Gtk::MessageType::INFO,
                                  Gtk::ButtonsType::OK);
    dialog->set_modal(true);
    dialog->signal_response().connect([dialog](int){ dialog->hide(); });
    dialog->show();
}

void MyWindow::on_all_oberkoerper_toggled() {
    bool active = checkbuttons_oberkoerper.front()->get_active();
    for(size_t i = 1; i < checkbuttons_oberkoerper.size(); ++i)
        checkbuttons_oberkoerper[i]->set_active(active);
}

void MyWindow::on_all_unterkoerper_toggled() {
    bool active = checkbuttons_unterkoerper.front()->get_active();
    for(size_t i = 1; i < checkbuttons_unterkoerper.size(); ++i)
        checkbuttons_unterkoerper[i]->set_active(active);
}

std::vector<WorkoutItem> MyWindow::generate_workout_plan(const std::vector<std::string>& muscles,
                                                         int total_sec, int exercise_sec, int pause_sec) {
    std::vector<WorkoutItem> available;

    std::string path = find_db_path();
    std::ifstream file(path);
    std::string record;
    if(read_csv_record(file, record)) {
        // skip header
    }

    while(read_csv_record(file, record)) {
        auto cells = parse_csv_line(record);
        if(cells.size() > 8) {
            const std::string& parts = cells[3];
            std::vector<std::string> tokens;
            std::string token;
            std::istringstream ss(parts);
            while(std::getline(ss, token, ',')) {
                size_t start = token.find_first_not_of(" \t");
                size_t end = token.find_last_not_of(" \t");
                if(start == std::string::npos)
                    continue;
                tokens.push_back(token.substr(start, end - start + 1));
            }

            bool match = false;
            for(const auto& m : muscles) {
                for(const auto& t : tokens) {
                    if(t == m) {
                        match = true;
                        break;
                    }
                }
                if(match) break;
            }

            if(match) {
                WorkoutItem item;
                item.name = cells[0];
                item.picture = cells[4];
                item.sound = cells[5];
                item.description = cells[8];
                item.muscles = cells[3];
                available.push_back(item);
            }
        }
    }
    file.close();

    int cycle = exercise_sec + pause_sec;
    int count = cycle > 0 ? total_sec / cycle : 0;
    if(count <= 0 || available.empty())
        return {};

    std::vector<WorkoutItem> plan;
    std::mt19937 rng(std::random_device{}());
    std::shuffle(available.begin(), available.end(), rng);

    if(count <= (int)available.size()) {
        plan.insert(plan.end(), available.begin(), available.begin() + count);
    } else {
        plan = available;
        std::uniform_int_distribution<> dist(0, available.size() - 1);
        while((int)plan.size() < count) {
            plan.push_back(available[dist(rng)]);
        }
    }

    std::cout << "Workout Plan:" << std::endl;
    for(const auto& w : plan)
        std::cout << "- " << w.name << std::endl;

    return plan;
}