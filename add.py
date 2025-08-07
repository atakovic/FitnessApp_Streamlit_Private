import streamlit as st
from data import load_db, save_db


def add_page():
    st.header("Workout hinzufügen")
    with st.form("add_workout"):
        name = st.text_input("Übung")
        rubrik = st.text_input("Rubrik")
        seite = st.text_input("Seite")
        part = st.text_input("Körperpartie")
        bildlink = st.text_input("Bildlink")
        sound = st.text_input("Sound")
        maxzeit = st.number_input("Max Zeit", min_value=0, step=1)
        mindzeit = st.number_input("Mind Zeit", min_value=0, step=1)
        beschreibung = st.text_area("Beschreibung")
        submitted = st.form_submit_button("Speichern")
    if submitted:
        df = load_db()
        df.loc[len(df)] = [name, rubrik, seite, part, bildlink, sound, maxzeit, mindzeit, beschreibung]
        save_db(df)
        st.success("Eintrag gespeichert")
