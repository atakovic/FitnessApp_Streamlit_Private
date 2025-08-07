import streamlit as st
import pandas as pd
import random
from pathlib import Path

DB_PATH = Path(__file__).parent / "DB.csv"

@st.cache_data
def load_db():
    return pd.read_csv(DB_PATH)

def save_db(df: pd.DataFrame) -> None:
    df.to_csv(DB_PATH, index=False)

def workout_page():
    st.header("Workout Planer")
    df = load_db()

    ok_muscles = ["Brust", "Schulter", "Arme", "Rücken", "Bauch"]
    uk_muscles = ["Beine", "Gesäß", "Waden", "Rücken"]

    selected_ok = st.multiselect("Oberkörper", ok_muscles)
    selected_uk = st.multiselect("Unterkörper", uk_muscles)

    total_minutes = st.number_input("Gesamtzeit (Minuten)", min_value=0, value=30, step=5)
    exercise_sec = st.number_input("Übungszeit (Sekunden)", min_value=0, value=30, step=5)
    pause_sec = st.number_input("Pausenzeit (Sekunden)", min_value=0, value=15, step=5)

    if st.button("Plan erstellen"):
        muscles = selected_ok + selected_uk
        if not muscles:
            st.warning("Bitte wähle mindestens eine Muskelgruppe aus.")
            return
        pattern = "|".join(muscles)
        available = df[df["Körperpartie"].str.contains(pattern, case=False, na=False)]
        total_sec = total_minutes * 60
        cycle = exercise_sec + pause_sec
        count = total_sec // cycle if cycle > 0 else 0
        if available.empty or count <= 0:
            st.warning("Keine passenden Workouts gefunden.")
            return
        plan = available.sample(min(count, len(available)))
        if count > len(plan):
            plan = pd.concat([plan, available.sample(count - len(plan), replace=True)], ignore_index=True)
        st.session_state['plan'] = plan[['Übung', 'Körperpartie', 'Beschreibung']]
        st.session_state['current_idx'] = 0
        st.success("Plan erstellt")

    if 'plan' in st.session_state:
        plan = st.session_state['plan']
        idx = st.session_state.get('current_idx', 0)
        if idx < len(plan):
            row = plan.iloc[idx]
            st.subheader(f"{row['Übung']}")
            st.write(row['Beschreibung'])
            if st.button("Nächste Übung"):
                st.session_state['current_idx'] = idx + 1
        else:
            st.success("Workout abgeschlossen")
            if st.button("Plan zurücksetzen"):
                st.session_state.pop('plan')
                st.session_state['current_idx'] = 0

def add_workout_page():
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

def database_page():
    st.header("Datenbank")
    df = load_db()
    st.dataframe(df)

pages = {
    "Workout": workout_page,
    "Add Workout": add_workout_page,
    "Database": database_page,
}

st.sidebar.title("Navigation")
page = st.sidebar.radio("Seite", list(pages.keys()))
pages[page]()
