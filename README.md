# MedAid

A healthcare portal where patients can view their health history and long-term trends, and find providers and insurance information. Built by Team Eight. All patient data in this project is synthetic.

## Where everything goes

| Folder | What it is | Proposed lead |
|---|---|---|
| [`frontend/`](frontend/) | The website patients see | Jin |
| [`backend/`](backend/) | The server: logins, URLs and connecting the parts | Sergei (to confirm), with Mark |
| [`database/`](database/) | Tables and data imports | Mubasshir |
| [`engine/`](engine/) | C++ health calculations | Mark |
| [`analysis/`](analysis/) | Data exploration and method testing | Jin and Mubasshir |
| [`docs/`](docs/) | Shared team documents | Everyone |

The leads are proposals until the team agrees on them. A lead looks after an area, but anyone can contribute to any folder.

## How the parts connect

```text
Website (frontend) -> Server (backend) -> Database
                                       -> Engine
```

The website asks the backend for data. The backend gets it from the database, calls the engine for any calculations, and sends back the result.

## How we work

1. Get the latest code: `git switch main`, then `git pull`.
2. Make a branch for your task: `git switch -c your-branch-name`.
3. Commit your work and push the branch: `git push -u origin your-branch-name`.
4. Open a pull request into `main` on GitHub.
5. Have a teammate review it, then merge it.
