import base64
import time
from pathlib import Path

import streamlit as st


def _format_time(seconds: float) -> str:
    mins, secs = divmod(int(seconds), 60)
    return f"{mins:02d}:{secs:02d}"


def _play_sound(file_name: str) -> None:
    path = Path(__file__).parent / file_name
    if not path.exists():
        return
    data = path.read_bytes()
    b64 = base64.b64encode(data).decode()
    st.markdown(
        f"<audio autoplay><source src='data:audio/mp3;base64,{b64}' type='audio/mp3'></audio>",
        unsafe_allow_html=True,
    )


def training_page() -> None:
    st.header("Training")
    if "plan" not in st.session_state:
        st.warning("Kein Trainingsplan vorhanden.")
        return

    plan = st.session_state["plan"]
    idx = st.session_state.get("current_idx", 0)
    phase = st.session_state.get("phase", "exercise")
    total_rem = st.session_state.get("total_remaining", 0.0)
    phase_rem = st.session_state.get("phase_remaining", 0.0)
    paused = st.session_state.get("paused", False)
    now = time.time()
    last = st.session_state.get("last_tick", now)

    if not paused:
        elapsed = now - last
        total_rem = max(total_rem - elapsed, 0)
        phase_rem = max(phase_rem - elapsed, 0)
        st.session_state["total_remaining"] = total_rem
        st.session_state["phase_remaining"] = phase_rem
    st.session_state["last_tick"] = now

    sound = st.session_state.pop("play_sound", None)
    if sound == "next":
        _play_sound("bell_pause.mp3")
    elif sound == "end":
        _play_sound("bell_end.mp3")

    if total_rem <= 0 or idx >= len(plan):
        if not st.session_state.get("finished"):
            st.session_state["play_sound"] = "end"
            st.session_state["finished"] = True
            st.rerun()
        st.success("Workout abgeschlossen")
        if st.button("Plan zurücksetzen"):
            for key in [
                "plan",
                "current_idx",
                "total_remaining",
                "phase_remaining",
                "phase",
                "paused",
                "finished",
            ]:
                st.session_state.pop(key, None)
            st.session_state["page"] = "Workout"
            st.rerun()
        return

    if phase_rem <= 0:
        if phase == "exercise":
            st.session_state["phase"] = "pause"
            st.session_state["phase_remaining"] = st.session_state.get("pause_sec", 0)
        else:
            st.session_state["phase"] = "exercise"
            st.session_state["phase_remaining"] = st.session_state.get("exercise_sec", 0)
            st.session_state["current_idx"] = idx + 1
            st.session_state["play_sound"] = "next"
        st.rerun()

    phase = st.session_state["phase"]
    idx = st.session_state.get("current_idx", 0)
    total_rem = st.session_state.get("total_remaining", 0.0)
    phase_rem = st.session_state.get("phase_remaining", 0.0)

    row = plan.iloc[idx]
    next_name = plan.iloc[idx + 1]["Übung"] if idx + 1 < len(plan) else "Ende"

    st.markdown(f"**Gesamt:** {_format_time(total_rem)}")
    st.markdown(f"**Aktuell:** {_format_time(phase_rem)}")
    st.caption(f"Nächste Übung: {next_name}")

    if phase == "exercise":
        st.subheader(row["Übung"])
        st.write(row["Beschreibung"])
        media_dir = Path(__file__).parent / "Bilder"
        gif = media_dir / f"{row['Übung']}.gif"
        mp4 = media_dir / f"{row['Übung']}.mp4"
        if gif.exists():
            st.image(str(gif))
        elif mp4.exists():
            st.video(str(mp4))
    else:
        st.subheader("Pause")

    if paused:
        if st.button("Weiter"):
            st.session_state["paused"] = False
            st.session_state["last_tick"] = time.time()
            st.rerun()
    else:
        if st.button("Pause"):
            st.session_state["paused"] = True
            st.rerun()

    if not st.session_state.get("paused", False):
        time.sleep(1)
        st.rerun()

