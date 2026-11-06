# Database

The PostgreSQL table definitions, plus the scripts that load data into them.

| Folder | What goes here |
|---|---|
| `migrations/` | Changes to the tables, saved in order so everyone's database matches |
| `imports/` | Scripts that load Synthea patients, CMS providers and other datasets |
| `seeds/` | Small sample data for testing and demos |
| `raw/` | Downloaded datasets on your own computer. Git ignores this folder |
| `output/` | Generated data on your own computer. Git ignores this folder |

**Commit the scripts and settings that produce the data, not the data itself.** The CMS file alone is 900 MB. Only use public or synthetic data, never real patient records.

**Connects to:** the backend, which reads and saves data using these tables. Tell the backend owner before renaming a table or column.
