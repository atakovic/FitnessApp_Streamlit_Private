import streamlit as st
from data import load_db


def database_page():
    st.header("Datenbank")
    df = load_db()
    st.dataframe(df)
