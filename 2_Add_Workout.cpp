//
// Created by alen on 07.06.25.
//
#include "2_Add_Workout.h"
#include <gtkmm.h>
#include <fstream>
#include <vector>
#include "db_utils.h"

Add_Workout::Add_Workout() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    set_hexpand(true);
    set_vexpand(true);
    //Window Properties
    //Box
    m_Box.set_orientation(Gtk::Orientation::VERTICAL);
    m_Box.set_hexpand(true);
    m_Box.set_vexpand(true);

    //Listbox
    m_ListBox.set_hexpand(true);
    m_ListBox.set_vexpand(true);
    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_hexpand(true);
    scrolled_window.set_vexpand(true);
    scrolled_window.set_child(m_ListBox);
    m_Box.append(scrolled_window);
    // Eingabefelder hinzufügen
    add_input_row("Name:", "Name");
    add_muscle_row("Muskelpartie:");
    add_input_row("Beschreibung:", "Beschreibung");
    add_input_row("Mind. Zeit:", "minZeit");            //Achtung: andere Bezeichnung
    add_input_row("Max. Zeit:", "maxZeit");             //Achtung: andere Bezeichnung
    add_input_row("Foto/GIF/Video:", "Frame");          //Achtung: andere Bezeichnung
    add_input_row("Sound:", "Sound");


    //Buttons
    m_Button_reset.set_label("Reset Workout");
    m_Box.append(m_Button_reset); //put Button in Box

    m_Button_add.set_label("Add Workout");
    m_Box.append(m_Button_add); //put Button in Box

    m_Button_reset.signal_clicked().connect(sigc::mem_fun(*this, &Add_Workout::on_reset_clicked));
    m_Button_add.signal_clicked().connect(sigc::mem_fun(*this, &Add_Workout::on_add_clicked));


    append(m_Box);



}

Add_Workout::~Add_Workout()
{
}


void Add_Workout::add_input_row(const std::string& label_text, const std::string& key) {
    auto* row = Gtk::make_managed<Gtk::ListBoxRow>();
    auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);

    auto* label = Gtk::make_managed<Gtk::Label>(label_text);
    label->set_valign(Gtk::Align::CENTER);
    label->set_halign(Gtk::Align::START);
    label->set_xalign(0.0);                                    // Text im Label linksbündig
    label->set_size_request(160);                         // 40 % bei ca. 400px Fensterbreite

    auto* entry = Gtk::make_managed<Gtk::Entry>();
    entry->set_hexpand(true);                           // 60 % des restlichen Raums
    entry->set_valign(Gtk::Align::CENTER);

    hbox->append(*label);
    hbox->append(*entry);

    row->set_child(*hbox);
    m_ListBox.append(*row);

    // Speichere Entry in Map, um später den Text zu bekommen
    input_entries[key] = entry;
}

void Add_Workout::add_muscle_row(const std::string& label_text) {
    auto* row = Gtk::make_managed<Gtk::ListBoxRow>();
    auto* hbox = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 10);

    auto* label = Gtk::make_managed<Gtk::Label>(label_text);
    label->set_valign(Gtk::Align::CENTER);
    label->set_halign(Gtk::Align::START);
    label->set_xalign(0.0);
    label->set_size_request(160);

    m_muscle_box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    const std::vector<std::string> muscles = {"Brust","Schulter","Bizeps","Trizeps","Arme","Rücken","Beine","Gesäß","Waden"};
    for(const auto& m : muscles) {
        auto* cb = Gtk::make_managed<Gtk::CheckButton>(m);
        m_muscle_buttons.push_back(cb);
        m_muscle_box->append(*cb);
    }
    m_muscle_box->set_hexpand(true);

    hbox->append(*label);
    hbox->append(*m_muscle_box);
    row->set_child(*hbox);
    m_ListBox.append(*row);
}

bool Add_Workout::validate_inputs() {
    bool valid = true;

    auto check_entry = [&](const std::string& key, bool mandatory) {
        auto it = input_entries.find(key);
        if(it == input_entries.end()) return true;
        Gtk::Entry* e = it->second;
        bool empty = e->get_text().empty();
        if(mandatory && empty) {
            e->add_css_class("error");
            valid = false;
        } else {
            e->remove_css_class("error");
        }
        return !empty;
    };

    check_entry("Name", true);
    check_entry("Beschreibung", true);
    bool has_min = check_entry("minZeit", true);
    bool has_max = check_entry("maxZeit", true);
    check_entry("Frame", true); // Foto/GIF/Video
    check_entry("Sound", false);

    // check integers
    int min_val = 0;
    int max_val = 0;
    if(has_min) {
        try { min_val = std::stoi(input_entries["minZeit"]->get_text()); } catch(...) { valid = false; }
        if(min_val <= 0) { input_entries["minZeit"]->add_css_class("error"); valid = false; }
    }
    if(has_max) {
        try { max_val = std::stoi(input_entries["maxZeit"]->get_text()); } catch(...) { valid = false; }
        if(max_val <= 0) { input_entries["maxZeit"]->add_css_class("error"); valid = false; }
    }
    if(has_min && has_max && max_val < min_val) {
        input_entries["maxZeit"]->add_css_class("error");
        valid = false;
    }

    bool muscle_selected = false;
    for(auto* cb : m_muscle_buttons) if(cb->get_active()) { muscle_selected = true; break; }
    if(!muscle_selected && m_muscle_box) {
        m_muscle_box->add_css_class("error");
        valid = false;
    } else if(m_muscle_box) {
        m_muscle_box->remove_css_class("error");
    }

    return valid;
}

void Add_Workout::on_add_clicked() {
    if(!validate_inputs()) {
        show_message("Pflichtfelder sind nicht alle komplett ausgefüllt");
        return;
    }

    std::string name = input_entries["Name"]->get_text();
    std::string beschreibung = input_entries["Beschreibung"]->get_text();
    std::string minZeit = input_entries["minZeit"]->get_text();
    std::string maxZeit = input_entries["maxZeit"]->get_text();
    std::string frame = input_entries["Frame"]->get_text();
    std::string sound = input_entries["Sound"]->get_text();

    std::string koerperpartie;
    for(size_t i=0;i<m_muscle_buttons.size();++i) {
        if(m_muscle_buttons[i]->get_active()) {
            if(!koerperpartie.empty()) koerperpartie += ", ";
            koerperpartie += m_muscle_buttons[i]->get_label();
        }
    }

    std::string path = find_db_path();
    std::ofstream file(path, std::ios::app);
    file << name << ",Übung,Nein," << koerperpartie << "," << frame << "," << sound << "," << maxZeit << "," << minZeit << "," << beschreibung << "\n";
    file.close();

    show_message("Workout wurde hinzugefügt");

    on_reset_clicked();

    m_signal_workout_added.emit();
}

void Add_Workout::on_reset_clicked() {
    for(auto& kv : input_entries) {
        kv.second->set_text("");
        kv.second->remove_css_class("error");
    }
    if(m_muscle_box)
        m_muscle_box->remove_css_class("error");
    for(auto* cb : m_muscle_buttons)
        cb->set_active(false);
}

void Add_Workout::show_message(const Glib::ustring& text) {
    if(auto* win = dynamic_cast<Gtk::Window*>(get_root())) {
        auto dialog = Gtk::make_managed<Gtk::MessageDialog>(*win, text, false,
                                   Gtk::MessageType::INFO,
                                   Gtk::ButtonsType::OK);
        dialog->set_modal(true);
        dialog->signal_response().connect([dialog](int){ dialog->hide(); });
        dialog->show();
    }
}