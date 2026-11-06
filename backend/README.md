# Backend

The server the website talks to, built with Python and FastAPI. It handles logins, reads from the database and calls the engine.

| Folder | What goes here |
|---|---|
| `app/routers/` | The URLs, grouped by area, such as auth, patient data and providers |
| `app/schemas/` | The shape of the data going in and out of each URL |
| `app/models/` | Python classes that match the database tables |
| `app/services/` | The logic behind each URL, including calls to the engine |
| `app/core/` | Shared setup, such as settings, database connection and security |
| `tests/` | Backend tests |

**Connects to:**

- The frontend, by providing the URLs it calls
- The database, by reading and saving data
- The engine, by calling it for health calculations
