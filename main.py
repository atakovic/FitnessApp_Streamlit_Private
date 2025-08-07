import streamlit as st
import pandas as pd
from data import load_db


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

    if st.button("Training starten"):
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
        st.session_state["plan"] = plan[["Übung", "Körperpartie", "Beschreibung"]]
        st.session_state["current_idx"] = 0
        st.session_state["page"] = "Training"
        st.rerun()
