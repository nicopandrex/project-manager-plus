# Project Manager Plus ➕➕

A command line project and task manager + analyzer. Made with C++ and Python and the two languages communicate via JSON files.

---

## Table of Contents

- [Summary](#summary)
- [Features](#features)
- [Installation and Build](#installation-and-build)
- [Running the Program](#running-the-program)
- [Dependencies](#dependencies)
- [Authorship and Citations](#authorship-and-citations)
- [Future Work](#future-work)

---

## Summary 🟰🟰

Project Manager Plus you can create projects, add tasks, track task state, and save them across sessions. It also feature's **Smart Analysis Report**: the program converts the projects state to JSON, it sends this JSON to a Python pandas script, it then reads back a structured report with project info, and the **top three tasks the user should work on next**  Each recommendation also gives a reason to work on it.

The recommendation engine uses a weighted scoring system that takes into account project priority, project status, task state, task type, and (for timed tasks) how much time is left on the timer.

---

## Features

- Create Projects
- Create Tasks and Timed Tasks
- Manage Project and Task States
- Persistence, save and load projects
- Smart Project Analysis with Python
---





## Installation and Build

### Prerequisites

- A C++20-capable compiler 
- CMake 4.0 or higher
- Python 3.8+
- pandas (`pip install pandas`)

The **nlohmann/json** C++ library does **not** need to be installed  `CMakeLists.txt` fetches it automatically via `FetchContent`.

---

## Running the Program

```bash
./build/project_manager_plus
```

When the program is running you'll see a numbered menu. From there you can create projects, open them to add or update tasks, manage timed tasks, and run the Smart Analysis Report. Project state is saved automatically when you exit through the menu (option 5).

For the Smart Analysis Report to work, `python` must be in your system `PATH` and `pandas` must be installed in your enviroment.

---

## Dependencies

| Dependency | Purpose | Installation                              |
|---|---|-------------------------------------------|
| [nlohmann/json](https://github.com/nlohmann/json) | C++ JSON  | Auto-fetched by CMake  no action required |
| [pandas](https://pandas.pydata.org/) | Python data analysis | `pip install pandas`                      |

---

## Authorship and Citations

**All code in this repository was written by me. ** Resources and external documentation and reference articles for specific C++ items and pandas usage. Listed Below:

### Library documentation referenced
- **nlohmann/json**  https://github.com/nlohmann/json and https://json.nlohmann.me/
- **pandas**  https://www.w3schools.com/python/pandas/default.asp

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

- Use Python (e.g. matplotlib) to make visual charts from the data, in addition to the text report.
- Tagging and filtering of tasks.
- Multi-user support with separate save files per user.



Originally created for my Adv Programming class, adapted for personal use, feel free to fork and edit for your own usage