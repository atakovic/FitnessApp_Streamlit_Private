//
// Created by alen on 07.06.25.
//
#include "3_database.h"
#include <gtkmm.h>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
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


database::database() : Gtk::Box(Gtk::Orientation::VERTICAL) {
    set_hexpand(true);
    set_vexpand(true);
    treeStore = Gtk::TreeStore::create(csv_columns);

    treeView.set_model(treeStore);
    treeView.append_column("Übung", csv_columns.col_uebung);
    treeView.append_column("Rubrik", csv_columns.col_rubrik);
    treeView.append_column("Seite", csv_columns.col_seite);
    treeView.append_column("Körperpartie", csv_columns.col_koerperpartie);
    treeView.append_column("Bildlink", csv_columns.col_bildlink);
    treeView.append_column("Sound", csv_columns.col_sound);
    treeView.append_column("Max Zeit", csv_columns.col_maxzeit);
    treeView.append_column("Mind Zeit", csv_columns.col_mindzeit);
    treeView.append_column("Beschreibung", csv_columns.col_beschreibung);
    treeView.set_grid_lines(Gtk::TreeView::GridLines::BOTH);

    treeView.set_hexpand(true);
    treeView.set_vexpand(true);

    scrolled_window.set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    scrolled_window.set_hexpand(true);
    scrolled_window.set_vexpand(true);
    scrolled_window.set_child(treeView);
    append(scrolled_window);

    load_data();
}

CSVModel::~CSVModel() {}

database::~database() {}

void database::load_data() {
    treeStore->clear();

    std::string path = find_db_path();
    std::ifstream file(path);
    if(!file.is_open())
        return;

    std::string line;
    if(read_csv_record(file, line)) {
        // skip header
    }

    while(read_csv_record(file, line)) {
        auto cells = parse_csv_line(line);
        Gtk::TreeModel::iterator iter = treeStore->append();
        Gtk::TreeModel::Row row = *iter;

        if(cells.size() > 0) row[csv_columns.col_uebung] = Glib::ustring(cells[0]);
        if(cells.size() > 1) row[csv_columns.col_rubrik] = Glib::ustring(cells[1]);
        if(cells.size() > 2) row[csv_columns.col_seite] = Glib::ustring(cells[2]);
        if(cells.size() > 3) row[csv_columns.col_koerperpartie] = Glib::ustring(cells[3]);
        if(cells.size() > 4) row[csv_columns.col_bildlink] = Glib::ustring(cells[4]);
        if(cells.size() > 5) row[csv_columns.col_sound] = Glib::ustring(cells[5]);
        if(cells.size() > 6) row[csv_columns.col_maxzeit] = Glib::ustring(cells[6]);
        if(cells.size() > 7) row[csv_columns.col_mindzeit] = Glib::ustring(cells[7]);
        if(cells.size() > 8) row[csv_columns.col_beschreibung] = Glib::ustring(cells[8]);
    }
    file.close();
    treeView.columns_autosize();
}

