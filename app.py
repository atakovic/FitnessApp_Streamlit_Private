import streamlit as st
from main import workout_page
from add import add_page
from database import database_page
from training import training_page

st.set_page_config(page_title="Fitness App", layout="wide")

pages = {
    "Workout": workout_page,
    "Add Workout": add_page,
    "Database": database_page,
}

if "plan" in st.session_state:
    pages["Training"] = training_page

st.sidebar.title("Navigation")
page = st.sidebar.radio("Seite", list(pages.keys()), key="page")
pages[page]()
