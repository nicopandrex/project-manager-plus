# Project Manager Plus

A command-line task and project management tool written in **C++**, with a **Python** data-analysis backend that recommends what you should work on next. The C++ program handles all user interaction, persistence, and the object model; Python (via pandas) handles the analytical heavy lifting. The two languages communicate through JSON files.

---

## Table of Contents

- [Summary](#summary)
- [Features](#features)
- [Languages and Why](#languages-and-why)
- [Architecture](#architecture)
- [Project Structure](#project-structure)
- [Installation and Build](#installation-and-build)
- [Running the Program](#running-the-program)
- [Dependencies](#dependencies)
- [Authorship and Citations](#authorship-and-citations)
- [Known Issues](#known-issues)
- [Future Work](#future-work)

---

## Summary

Project Manager Plus lets a user create projects, add standard or **timed** tasks to them, track task state (ToDo / Doing / Done), and persist everything to disk between runs. Its headline feature is a **Smart Analysis Report**: the program serializes the current project state to JSON, hands it off to a Python script powered by pandas, and reads back a structured report containing project summaries, per-project completion percentages, and the **top three tasks the user should work on next** — each with a human-readable reason for the recommendation.

The recommendation engine uses a weighted scoring system that takes into account project priority, project status, task state, task type, and (for timed tasks) how much time is left on the timer.

---

## Features

| Feature | Description |
|---|---|
| Project management | Create, rename, view, and change the status of projects (Planned / Active / Paused / Completed) with priorities 1–5. |
| Standard tasks | Add tasks to a project and move them through ToDo → Doing → Done. |
| Timed tasks | Tasks with built-in countdown timers that can be started, paused, and reset. Timer state survives program restarts by syncing against the system clock. |
| Persistent storage | All projects and tasks are saved to a JSON file on exit and reloaded on startup. |
| Smart Analysis Report | Exports project data, runs a Python pandas pipeline, and prints a summary, per-project completion breakdown, and the top three recommended next tasks with reasons. |
| Input validation | Defensive input handling throughout the menu system — invalid entries are rejected without crashing. |

---

## Languages and Why

- **C++ (C++20)** — used for the program's foundation: the object model (`Project`, `Task`, `TimedTask`, `CountdownTimer`), the menu-driven UI, persistence, and the bridge to Python. C++ was chosen because it's fast, has strong static typing, and supports the kind of polymorphism (`Task` → `TimedTask`) the project relies on.
- **Python 3** — used exclusively for the Smart Analysis Report. Python with pandas is well-suited to lightweight tabular data work: building a DataFrame of tasks, scoring them, sorting, and producing a structured output is far more concise in pandas than it would be in C++.

---



## Installation and Build

### Prerequisites

- A C++20-capable compiler (GCC 11+, Clang 13+, or MSVC 2022+)
- CMake 4.0 or higher
- Python 3.8+
- pandas (`pip install pandas`)

The **nlohmann/json** C++ library does **not** need to be installed manually — `CMakeLists.txt` fetches it automatically via `FetchContent`.

### Build steps

```bash
git clone https://github.com/nicopandrex/project-manager-plus.git
cd project-manager-plus
cmake -B build
cmake --build build
```

This produces an executable named `project_manager_plus` 

---

## Running the Program

```bash
./build/project_manager_plus
```

You'll be greeted by a numbered main menu. From there you can create projects, open them to add or update tasks, manage timed tasks, and run the Smart Analysis Report. Project state is saved automatically when you exit through the menu (option 5).

For the Smart Analysis Report to work, `python` must be on your system `PATH` and `pandas` must be installed.

---

## Dependencies

| Dependency | Purpose | Installation |
|---|---|---|
| [nlohmann/json](https://github.com/nlohmann/json) | C++ JSON serialization | Auto-fetched by CMake — no action required |
| [pandas](https://pandas.pydata.org/) | Python data analysis | `pip install pandas` |

---

## Authorship and Citations

**All code in this repository was written by me. ** I did consult external documentation and reference articles for specific C++ idioms and pandas usage; those references are listed below.

### Library documentation referenced
- **nlohmann/json** — https://github.com/nlohmann/json and https://json.nlohmann.me/
- **pandas** — https://www.w3schools.com/python/pandas/default.asp

### C++ technique references
- Clearing and handling invalid `cin` input (`std::cin.clear()` / `std::cin.ignore()`):
  https://www.geeksforgeeks.org/cpp/cin-clear-function-in-cpp/ and
  https://www.geeksforgeeks.org/cpp/cin-ignore-function-in-cpp/
- Erasing elements from a `std::vector`:
  https://www.geeksforgeeks.org/cpp/vector-erase-in-cpp-stl/
- Move semantics (`projects.push_back(std::move(p))`):
  https://www.geeksforgeeks.org/cpp/move-assignment-operator-in-cpp-11/
- C++17 filesystem library:
  https://www.geeksforgeeks.org/cpp/file-system-library-in-cpp-17/
- Polymorphic copy with `clone()` and `std::unique_ptr`:
  https://www.fluentcpp.com/2017/09/08/make-polymorphic-copy-modern-cpp/

---


## Future Work

- Use Python (e.g. matplotlib) to generate visual charts from the analysis data, in addition to the text report.
- Tagging and filtering of tasks.
- Multi-user support with separate save files per user.