# API Conventions

These are the rules every MedAid endpoint follows, so the frontend and backend can be built at the same time. To change a rule, open a PR against this file so the whole team sees it.

## Basics

- **Base path:** `/api/v1`. Breaking changes need agreement from the team and, once the frontend depends on the old behavior, a new version.
- **Format:** JSON for requests and responses.
- **Backend:** FastAPI with Pydantic models for every request and response. FastAPI publishes the live spec at `/docs`, which is the source of truth for each endpoint's exact fields.
- **Frontend types:** generate TypeScript types from that spec rather than writing them by hand, so the two sides can't drift apart.

## Naming

| Thing | Rule | Example |
|---|---|---|
| URL paths | Lowercase plural nouns, with hyphens between words | `/providers`, `/me/access-log` |
| JSON fields and query parameters | `snake_case` | `time_weighted_average` |
| Enum values | Lowercase `snake_case` strings | `insufficient_data` |
| Lab and vital sign types | LOINC codes | `4548-4` is hemoglobin A1c |

## Data formats

- **IDs:** UUIDs as strings. Provider NPIs are also strings, never numbers, so they stay exact.
- **Dates:** `YYYY-MM-DD`.
- **Timestamps:** ISO 8601 in UTC, ending in `Z`, such as `2026-11-06T21:30:00Z`.
- **Measurements:** always send the unit with the value. Never make a client assume one.
- **Missing values:** include the field as `null` instead of leaving it out.

## Authentication

- Log in through `POST /api/v1/auth/login`. The server sets a session cookie marked `HttpOnly`, `Secure` and `SameSite=Lax`.
- Never store session tokens in `localStorage`.
- In local development only, the cookie can omit `Secure`, since local dev runs over plain HTTP.

## Patient data access

These rules are how we keep one patient from reading another's data, so every PR must follow them.

1. **Patient data lives under `/me`.** The server works out the patient from the session. Patient IDs never appear in URLs or request bodies.
2. **Every patient query filters by the logged-in patient's ID.** This filter goes in one shared dependency, not in each route.
3. **Every new `/me` route needs a test** showing that one patient can't read another's data.
4. **Reads of patient data are written to `access_log`,** which powers the My Data page.
5. **Never log patient data.** Logs may contain request IDs and error codes, but no names, lab values or request bodies.

## Status codes

| Code | When |
|---|---|
| `200` | Successful read or update |
| `201` | Something was created |
| `204` | Success with nothing to return, such as logout |
| `400` | The request is malformed or breaks a rule beyond field validation |
| `401` | Not logged in, or the login failed |
| `403` | Logged in but not allowed |
| `404` | Not found, including records that belong to someone else |
| `409` | Conflict, such as an email that's already registered |
| `422` | A field failed validation |
| `429` | Too many attempts, such as repeated failed logins |
| `500` | A bug on our side |

## Errors

Every error uses the same shape, so the frontend needs only one error handler:

```json
{
  "error": {
    "code": "validation_failed",
    "message": "Some fields are invalid.",
    "details": [
      { "field": "zip", "message": "Must be 5 digits." }
    ]
  }
}
```

- `code` is a stable `snake_case` string for code to check.
- `message` is safe to show to users. Never include stack traces or SQL.
- `details` is a list, and is empty when it doesn't apply.
- FastAPI's default errors look different, so the backend needs custom exception handlers that convert them to this shape.
- Login failures use the same message whether the email or the password was wrong.

## Pagination

List endpoints take `limit` (default 20, max 100) and `offset` (default 0), and return:

```json
{
  "items": [],
  "total": 0,
  "limit": 20,
  "offset": 0
}
```

## Analytics responses

Too little history is a normal situation, not an error, so it returns `200` with a `status` field.

`GET /api/v1/me/analytics/exposure?code=4548-4`

```json
{
  "status": "ok",
  "code": "4548-4",
  "display_name": "Hemoglobin A1c",
  "unit": "%",
  "method": "trapezoid",
  "from_date": "2021-03-14",
  "to_date": "2026-09-02",
  "n_points": 9,
  "duration_years": 5.47,
  "cumulative": 33.9,
  "cumulative_unit": "%·years",
  "time_weighted_average": 6.2
}
```

With too few readings, the value fields are `null`:

```json
{
  "status": "insufficient_data",
  "code": "4548-4",
  "display_name": "Hemoglobin A1c",
  "unit": "%",
  "method": "trapezoid",
  "from_date": null,
  "to_date": null,
  "n_points": 2,
  "duration_years": null,
  "cumulative": null,
  "cumulative_unit": "%·years",
  "time_weighted_average": null
}
```

How engine exceptions map to responses:

| Engine exception | Response |
|---|---|
| `InsufficientDataError` | `200` with `status: "insufficient_data"` |
| `InvalidSeriesError` | `500` with code `internal_error`, and log it, because the database shouldn't contain data this bad |

Analytics are educational. The UI must not present them as a diagnosis or medical advice.

## Data source labels

Any data that could be mistaken for real coverage information must say where it came from, using a `source` field:

| Value | Meaning |
|---|---|
| `cms` | Real data from a CMS public file |
| `demo` | Simulated for this project, and the UI must label it as such |

Example of a provider's insurance list:

```json
{
  "insurance": [
    { "name": "Medicare", "status": "accepts", "source": "cms" },
    { "name": "Aetna", "status": "in_network", "source": "demo" }
  ]
}
```

Medicare `status` comes from the CMS `ind_assgn` column: `Y` becomes `accepts`, and `M` becomes `may_accept`.

## Planned endpoints

This is a starting list. The owner of each area can change the details by PR.

| Method and path | Purpose |
|---|---|
| `GET /health` | Check that the API is running |
| `POST /auth/register` | Create an account and link it to an unclaimed synthetic patient |
| `POST /auth/login` | Start a session |
| `POST /auth/logout` | End a session |
| `GET /me` | The current patient's profile |
| `DELETE /me` | Delete the account |
| `GET /me/observations?code=&from_date=&to_date=` | Lab and vital sign history |
| `GET /me/encounters` | Visit history |
| `GET /me/analytics/exposure?code=` | Cumulative exposure and time-weighted average |
| `GET /me/access-log` | Who accessed this patient's data |
| `GET /me/export` | Download this patient's data |
| `GET /providers?specialty=&zip=&radius_miles=&accepts_medicare=` | Search for providers |
| `GET /providers/{npi}` | Provider profile |
| `GET /specialties` | Specialty list for the search filter |
