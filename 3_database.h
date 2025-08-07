//
// Created by alen on 07.06.25.
//

#ifndef DATABASE_H
#define DATABASE_H
#include <gtkmm.h>

class CSVModel : public Gtk::TreeModel::ColumnRecord {
public:
    CSVModel() {
        add(col_uebung);
        add(col_rubrik);
        add(col_seite);
        add(col_koerperpartie);
        add(col_bildlink);
        add(col_sound);
        add(col_maxzeit);
        add(col_mindzeit);
        add(col_beschreibung);
    }

    Gtk::TreeModelColumn<Glib::ustring> col_uebung;
    Gtk::TreeModelColumn<Glib::ustring> col_rubrik;
    Gtk::TreeModelColumn<Glib::ustring> col_seite;
    Gtk::TreeModelColumn<Glib::ustring> col_koerperpartie;
    Gtk::TreeModelColumn<Glib::ustring> col_bildlink;
    Gtk::TreeModelColumn<Glib::ustring> col_sound;
    Gtk::TreeModelColumn<Glib::ustring> col_maxzeit;
    Gtk::TreeModelColumn<Glib::ustring> col_mindzeit;
    Gtk::TreeModelColumn<Glib::ustring> col_beschreibung;

    virtual ~CSVModel();
};

class database : public Gtk::Box
{
public:
    database();
    virtual ~database();

    void load_data();

protected:
    CSVModel csv_columns;
    Glib::RefPtr<Gtk::TreeStore> treeStore;
    Gtk::TreeView treeView;
    Gtk::ScrolledWindow scrolled_window;

};





#endif //DATABASE_H