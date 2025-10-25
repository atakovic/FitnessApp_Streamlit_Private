# Fitness App Streamlit

Diese Version der Fitness-App wurde auf Python und [Streamlit](https://streamlit.io/) umgestellt und startet automatisch im Wide-Layout.

## Installation

```bash
pip install -r requirements.txt
```

## Start

```bash
streamlit run app.py
```

`app.py` stellt die Navigation bereit und bindet die Einzelseiten ein:

- `main.py` – Auswahl der Übungen und Zeiten
- `add.py` – neue Workouts in die CSV-Datenbank einfügen
- `database.py` – Anzeige der Datenbank
- `training.py` – Ablauf des Trainings (erscheint erst nach dem Start)

Die App bietet drei Bereiche und eine zusätzliche Trainingsansicht:

- **Workout**: Übungen auswählen und Zeiten festlegen. Nach dem Start erscheint automatisch die Seite **Training**.
- **Add Workout**: Neue Übungen zur CSV-Datenbank hinzufügen.
- **Database**: Gesamte Datenbank anzeigen.
- **Training**: Wird nur angezeigt, wenn ein Trainingsplan aktiv ist, zeigt passende GIFs/Videos, zählt Gesamt- und Übungszeit herunter, bietet eine Pause-Funktion und gibt akustische Hinweise für die nächste Übung sowie das Trainingsende.
