# Fitness App Streamlit

Diese Version der Fitness-App wurde auf Python und [Streamlit](https://streamlit.io/) umgestellt.

## Installation

```bash
pip install -r requirements.txt
```

## Start

```bash
streamlit run app.py
```

Die App bietet drei Bereiche und eine zusätzliche Trainingsansicht:

- **Workout**: Übungen auswählen und Zeiten festlegen. Nach dem Start erscheint automatisch die Seite **Training**.
- **Add Workout**: Neue Übungen zur CSV-Datenbank hinzufügen.
- **Database**: Gesamte Datenbank anzeigen.
- **Training**: Wird nur angezeigt, wenn ein Trainingsplan aktiv ist und führt durch die Übungen.
