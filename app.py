import streamlit as st
from main import workout_page
from add import add_page
from database import database_page
from training import training_page

st.set_page_config(page_title="Fitness App", layout="wide")

if "page" not in st.session_state:
    st.session_state["page"] = "Workout"

pages = {
    "Workout": workout_page,
    "Add Workout": add_page,
    "Database": database_page,
}

if "plan" in st.session_state:
    pages["Training"] = training_page

st.sidebar.title("Navigation")
page_names = list(pages.keys())
current = st.session_state["page"]
index = page_names.index(current) if current in page_names else 0
page = st.sidebar.radio("Seite", page_names, index=index)
st.session_state["page"] = page
pages[page]()
