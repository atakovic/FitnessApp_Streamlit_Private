import pandas as pd
from pathlib import Path
import streamlit as st

DB_PATH = Path(__file__).parent / "DB.csv"

@st.cache_data
def load_db():
    return pd.read_csv(DB_PATH)

def save_db(df: pd.DataFrame) -> None:
    df.to_csv(DB_PATH, index=False)
