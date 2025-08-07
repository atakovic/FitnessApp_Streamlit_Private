import streamlit as st


def training_page():
    st.header("Training")
    if "plan" not in st.session_state:
        st.warning("Kein Trainingsplan vorhanden.")
        return
    plan = st.session_state["plan"]
    idx = st.session_state.get("current_idx", 0)
    if idx < len(plan):
        row = plan.iloc[idx]
        st.subheader(f"{row['Übung']}")
        st.write(row['Beschreibung'])
        if st.button("Nächste Übung"):
            st.session_state["current_idx"] = idx + 1
            st.experimental_rerun()
    else:
        st.success("Workout abgeschlossen")
        if st.button("Plan zurücksetzen"):
            st.session_state.pop("plan")
            st.session_state["current_idx"] = 0
            st.session_state["page"] = "Workout"
            st.experimental_rerun()
