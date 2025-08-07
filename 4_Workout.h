#ifndef WORKOUT_VIEW_H
#define WORKOUT_VIEW_H

#include <gtkmm.h>
#include <string>
#include <vector>

struct WorkoutItem {
    std::string name;
    std::string picture;
    std::string sound;
    std::string description;
    std::string muscles;
};

class WorkoutView : public Gtk::Box {
public:
    WorkoutView();

    void start_plan(const std::vector<WorkoutItem>& items,
                    int exercise_seconds,
                    int total_seconds,
                    int pause_seconds);

    sigc::signal<void()> signal_workout_end() { return m_signal_workout_end; }

    static std::string format_time(int seconds);

protected:
    void end_workout();
    // container holding the workout table and buttons
    Gtk::Box m_table_box{Gtk::Orientation::VERTICAL};
    Gtk::Label m_name_label;
    Gtk::Label m_description_label;
    Gtk::Video m_video;
    Gtk::Label m_muscle_label;
    Gtk::Label m_next_label;
    Gtk::Label m_counter_this_label;
    Gtk::Label m_counter_all_label;
    Gtk::Button m_end_button;
    Gtk::Button m_pause_button;
    Gtk::Button m_next_button;

    Gtk::Grid m_grid;
    Gtk::Box* row_buttons{nullptr};

    // summary shown after the workout ends
    Gtk::Box m_summary_box{Gtk::Orientation::VERTICAL};
    Gtk::ScrolledWindow m_summary_scroll;
    Gtk::Label m_end_title;
    Gtk::Label m_summary_label;

    Gtk::Label* desc_name{nullptr};
    Gtk::Label* desc_desc{nullptr};
    Gtk::Label* desc_picture{nullptr};
    Gtk::Label* desc_muscle{nullptr};

    int remaining_all{0};
    int remaining_current{0};
    int exercise_duration{0};
    int pause_duration{0};
    bool paused{false};
    bool in_pause{false};
    std::vector<WorkoutItem> plan;
    size_t current_index{0};
    sigc::connection conn_tick;

    sigc::signal<void()> m_signal_workout_end;

    void show_summary();
    void set_media_file(const std::string& path);


    Gtk::Window m_pause_window;
    Gtk::Label m_pause_label;

    bool on_timeout_tick();
    void on_pause_clicked();
    void show_workout(size_t index);
    void on_next_step_clicked();
};

#endif // WORKOUT_VIEW_H
