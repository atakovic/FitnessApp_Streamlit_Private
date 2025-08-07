#include "4_Workout.h"

#include <iomanip>
#include <sstream>
#include <glibmm/spawn.h>
#include <glibmm/refptr.h>
#include <fstream>
#include <gdkmm/pixbuf.h>
#include <gdkmm/pixbufanimation.h>
#include "db_utils.h"

WorkoutView::WorkoutView() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    set_hexpand(true);
    set_vexpand(true);

    m_table_box.set_hexpand(true);
    m_table_box.set_vexpand(true);
    m_table_box.set_halign(Gtk::Align::START);

    m_grid.set_column_spacing(10);
    m_grid.set_row_spacing(5);
    m_grid.set_hexpand(true);
    m_grid.set_column_homogeneous(false);
    m_grid.set_halign(Gtk::Align::START);

    desc_name = Gtk::make_managed<Gtk::Label>("Name des Workouts:");
    desc_picture = Gtk::make_managed<Gtk::Label>("Foto/GIF:");
    desc_desc = Gtk::make_managed<Gtk::Label>("Beschreibung:");
    desc_muscle = Gtk::make_managed<Gtk::Label>("Muskel(n):");

    for(auto* lbl : {desc_name, desc_picture, desc_desc, desc_muscle}) {
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);
        lbl->set_xalign(0.0f);
    }
    for(auto* lbl : {&m_name_label, &m_description_label, &m_muscle_label,
                     &m_next_label, &m_counter_this_label, &m_counter_all_label}) {
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);
        lbl->set_xalign(0.0f);
        lbl->set_wrap(true);
    }
    m_video.set_halign(Gtk::Align::START);
    m_video.set_hexpand(true);
    m_video.set_autoplay(true);
    m_video.set_loop(true);
    m_video.set_size_request(240,240);
    auto label_next = Gtk::make_managed<Gtk::Label>("Next Workout:");
    auto label_cthis = Gtk::make_managed<Gtk::Label>("Counter this:");
    auto label_call = Gtk::make_managed<Gtk::Label>("Counter all:");
    for(auto* lbl : {label_next, label_cthis, label_call}) {
        lbl->set_halign(Gtk::Align::START);
        lbl->set_hexpand(true);
        lbl->set_xalign(0.0f);
    }

    m_grid.attach(*desc_name,        0,0,1,1);
    m_grid.attach(m_name_label,      1,0,1,1);
    m_grid.attach(*desc_picture,     0,1,1,1);
    m_grid.attach(m_video,         1,1,1,1);
    m_grid.attach(*desc_desc,        0,2,1,1);
    m_grid.attach(m_description_label,1,2,1,1);
    m_grid.attach(*desc_muscle,      0,3,1,1);
    m_grid.attach(m_muscle_label,    1,3,1,1);
    m_grid.attach(*label_next,       0,4,1,1);
    m_grid.attach(m_next_label,      1,4,1,1);
    m_grid.attach(*label_cthis,      0,5,1,1);
    m_grid.attach(m_counter_this_label,1,5,1,1);
    m_grid.attach(*label_call,       0,6,1,1);
    m_grid.attach(m_counter_all_label,1,6,1,1);

    m_table_box.append(m_grid);
    m_table_box.set_spacing(5);

    row_buttons = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL);
    m_end_button.set_label("End Workout");
    m_pause_button.set_label("Pause");
    m_next_button.set_label("Next Step");
    row_buttons->append(m_end_button);
    row_buttons->append(m_pause_button);
    row_buttons->append(m_next_button);
    m_table_box.append(*row_buttons);

    m_pause_button.signal_clicked().connect(sigc::mem_fun(*this, &WorkoutView::on_pause_clicked));
    m_end_button.signal_clicked().connect(sigc::mem_fun(*this, &WorkoutView::end_workout));
    m_next_button.signal_clicked().connect(sigc::mem_fun(*this, &WorkoutView::on_next_step_clicked));

    m_pause_window.set_title("Pause");
    m_pause_window.set_modal(true);
    m_pause_window.set_decorated(false);
    m_pause_window.set_child(m_pause_label);

    m_summary_box.set_hexpand(true);
    m_summary_box.set_vexpand(true);
    m_summary_box.set_halign(Gtk::Align::START);

    m_summary_label.set_wrap(true);
    m_summary_label.set_hexpand(true);
    m_summary_label.set_xalign(0.0f);

    m_summary_scroll.set_policy(Gtk::PolicyType::AUTOMATIC,
                                Gtk::PolicyType::AUTOMATIC);
    m_summary_scroll.set_child(m_summary_label);
    m_summary_scroll.set_hexpand(true);
    m_summary_scroll.set_vexpand(true);

    m_summary_box.append(m_end_title);
    m_summary_box.append(m_summary_scroll);
    append(m_table_box);
    append(m_summary_box);
    m_summary_box.set_visible(false);
}

void WorkoutView::start_plan(const std::vector<WorkoutItem>& items,
                             int exercise_seconds,
                             int total_seconds,
                             int pause_seconds) {
    plan = items;
    exercise_duration = exercise_seconds;
    pause_duration = pause_seconds;
    remaining_all = total_seconds;
    current_index = 0;
    in_pause = false;
    paused = false;
    m_pause_button.set_label("Pause");
    m_end_button.set_visible(true);
    m_pause_button.set_visible(true);
    m_next_button.set_visible(true);
    desc_name->show();
    desc_picture->show();
    desc_desc->show();
    desc_muscle->show();
    m_muscle_label.show();
    m_next_label.show();
    m_counter_this_label.show();
    m_counter_all_label.show();
    row_buttons->show();
    m_name_label.show();
    m_description_label.show();
    m_summary_box.hide();
    m_table_box.show();

    if(auto* win = dynamic_cast<Gtk::Window*>(get_root()))
        m_pause_window.set_transient_for(*win);

    show_workout(current_index);
    remaining_current = exercise_duration;
    m_counter_this_label.set_text(std::to_string(remaining_current));
    m_counter_all_label.set_text(format_time(remaining_all));

    if(conn_tick)
        conn_tick.disconnect();
    conn_tick = Glib::signal_timeout().connect_seconds(sigc::mem_fun(*this, &WorkoutView::on_timeout_tick), 1);
}

bool WorkoutView::on_timeout_tick() {
    if(paused)
        return true;

    if(remaining_all > 0) {
        --remaining_all;
        m_counter_all_label.set_text(format_time(remaining_all));
        if(remaining_all == 0) {
            try {
                auto path = find_data_file("bell_end.mp3");
                Glib::spawn_command_line_async("paplay \"" + path + "\"");
            } catch(...) {}
            end_workout();
            return false;
        }
    }

    if(remaining_current > 0)
        --remaining_current;

    if(in_pause) {
        m_pause_label.set_text("Pause: " + std::to_string(remaining_current));
        if(remaining_current == 0) {
            m_pause_window.hide();
            in_pause = false;
            if(current_index + 1 < plan.size()) {
                ++current_index;
                show_workout(current_index);
                remaining_current = exercise_duration;
                m_counter_this_label.set_text(std::to_string(remaining_current));
            } else {
                try {
                    auto path = find_data_file("bell_end.mp3");
                    Glib::spawn_command_line_async("paplay \"" + path + "\"");
                } catch(...) {}
                end_workout();
                return false;
            }
        }
    } else {
        m_counter_this_label.set_text(std::to_string(remaining_current));
        if(remaining_current == 0) {
            if(current_index + 1 >= plan.size()) {
                try {
                    auto path = find_data_file("bell_end.mp3");
                    Glib::spawn_command_line_async("paplay \"" + path + "\"");
                } catch(...) {}
                end_workout();
                return false;
            } else if(pause_duration > 0) {
                in_pause = true;
                remaining_current = pause_duration;
                m_pause_label.set_text("Pause: " + std::to_string(remaining_current));
                m_pause_window.present();
                try {
                    auto path = find_data_file("bell_pause.mp3");
                    Glib::spawn_command_line_async("paplay \"" + path + "\"");
                } catch(...) {}
            } else {
                ++current_index;
                show_workout(current_index);
                remaining_current = exercise_duration;
                m_counter_this_label.set_text(std::to_string(remaining_current));
            }
        }
    }

    return remaining_all > 0;
}

void WorkoutView::on_pause_clicked() {
    paused = !paused;
    if(paused)
        m_pause_button.set_label("Resume");
    else
        m_pause_button.set_label("Pause");
}

void WorkoutView::set_media_file(const std::string& path) {
    try {
        auto file = Gio::File::create_for_path(path);
        auto media = Gtk::MediaFile::create(file);
        m_video.set_media_stream(media);
        m_video.show();
    } catch (const Glib::Error& ex) {
        m_video.set_media_stream(nullptr); // ggf. leeren
    }
}


void WorkoutView::show_workout(size_t index) {
    if(index >= plan.size())
        return;
    const WorkoutItem& item = plan[index];
    m_name_label.set_text(item.name);

    std::string path;

    if(!item.picture.empty()) {
        try {
            path = find_data_file(item.picture);
            std::ifstream chk(path);
            if(!chk.good())
                path.clear();
        } catch(...) {}
    }

    if(path.empty()) {
        std::string base = "Bilder/" + item.name;
        path = find_data_file(base + ".gif");
        std::ifstream gifchk(path);
        if(!gifchk.good()) {
            path = find_data_file(base + ".webp");
        }
    }

    set_media_file(path);

    m_description_label.set_text(item.description);
    m_muscle_label.set_text(item.muscles);

    if(index + 1 < plan.size())
        m_next_label.set_text(plan[index + 1].name);
    else
        m_next_label.set_text("");

    if(!item.sound.empty()) {
        try {
            auto path = find_data_file(item.sound);
            Glib::spawn_command_line_async("paplay \"" + path + "\"");
        } catch(...) {}
    }
}


std::string WorkoutView::format_time(int seconds) {
    std::ostringstream ss;
    int minutes = seconds / 60;
    int secs = seconds % 60;
    ss << std::setw(2) << std::setfill('0') << minutes << ':'
       << std::setw(2) << std::setfill('0') << secs;
    return ss.str();
}

void WorkoutView::end_workout() {
    if(conn_tick)
        conn_tick.disconnect();
    m_pause_window.hide();
    show_summary();
}

void WorkoutView::show_summary() {
    std::string list;
    for(const auto& item : plan)
        list += item.name + "\n";

    m_end_title.set_markup("<span size='xx-large'><b>ENDE</b></span>");
    m_end_title.set_halign(Gtk::Align::FILL);
    m_summary_label.set_text(list);
    m_summary_label.set_halign(Gtk::Align::FILL);
    m_summary_label.set_justify(Gtk::Justification::LEFT);
    m_summary_box.set_halign(Gtk::Align::FILL);
    m_summary_scroll.set_halign(Gtk::Align::FILL);

    m_table_box.hide();
    m_summary_box.show();
}

void WorkoutView::on_next_step_clicked() {
    if(remaining_all > 0 && remaining_current > 0) {
        remaining_all -= remaining_current;
        if(remaining_all < 0) remaining_all = 0;
        m_counter_all_label.set_text(format_time(remaining_all));
    }
    if(in_pause) {
        m_pause_window.hide();
        in_pause = false;
        if(current_index + 1 < plan.size()) {
            ++current_index;
            show_workout(current_index);
            remaining_current = exercise_duration;
            m_counter_this_label.set_text(std::to_string(remaining_current));
        } else {
            try {
                auto path = find_data_file("bell_end.mp3");
                Glib::spawn_command_line_async("paplay \"" + path + "\"");
            } catch(...) {}
            end_workout();
        }
    } else {
        if(current_index + 1 >= plan.size()) {
            try {
                auto path = find_data_file("bell_end.mp3");
                Glib::spawn_command_line_async("paplay \"" + path + "\"");
            } catch(...) {}
            end_workout();
        } else if(pause_duration > 0) {
            in_pause = true;
            remaining_current = pause_duration;
            m_pause_label.set_text("Pause: " + std::to_string(remaining_current));
            m_pause_window.present();
            try {
                auto path = find_data_file("bell_pause.mp3");
                Glib::spawn_command_line_async("paplay \"" + path + "\"");
            } catch(...) {}
        } else {
            ++current_index;
            show_workout(current_index);
            remaining_current = exercise_duration;
            m_counter_this_label.set_text(std::to_string(remaining_current));
        }
    }
}

