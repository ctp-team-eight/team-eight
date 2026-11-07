# Engine

The C++ health calculations. The backend calls this code from Python and sends the results to the website.

| Folder | What goes here |
|---|---|
| `include/medaid/` | Header files that list the engine's functions |
| `src/` | The C++ code for the calculations |
| `bindings/` | The bridge between C++ and Python |
| `python/medaid_engine/` | The Python package the backend imports |
| `tests/cpp/` | C++ tests |
| `tests/python/` | Python tests |

**Connects to:** the backend, which passes in readings from the database and gets the results back.
