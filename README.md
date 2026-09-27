# Indian Stadium Management System

A production-grade, asynchronous multithreaded system built in **C** for web scraping, data cleaning, concurrent dataset ingestion, relational database management, and Python-driven analytical visualization of Indian sports stadiums.

---

## 1. Features & Architecture Highlights

* **Web Scraping Pipeline**: Real HTTP acquisition from Wikipedia (`List_of_stadiums_in_India`) using `libcurl`.
* **HTML DOM Tree Parsing**: Table extraction using `libxml2` DOM tree parser with dynamic table header alignment.
* **Data Normalization & Cleaning**: Automated removal of footnote citations (`[1]`), footnote symbols (`†`), thousand-separators, and string formatting errors.
* **POSIX Multithreading & Producer-Consumer Bounded Buffer**: Concurrent processing using `pthread`, POSIX semaphores (`sem_t empty`, `sem_t full`), and mutual exclusion locks (`pthread_mutex_t`). Dual consumers write asynchronously to CSV and MySQL.
* **MySQL Database Storage**: Native C integration using MySQL/MariaDB C client library (`libmariadb`).
* **Menu-Driven C CRUD Application**: Complete Create, Read (display table & search), Update, Delete, and CSV Export operations.
* **Python Data Visualizations**: Automated generation of 3 analytical charts using `pandas` and `matplotlib` saved as high-resolution PNGs in `output/`.

---

## 2. Directory Structure

```text
Indian-Stadium-Management-System/
├── README.md                 # Project Overview & Execution Guide
├── Makefile                  # Build Automation Rules
├── include/                  # C Header Files
│   ├── stadium.h             # Stadium Data Structure Definition
│   ├── curl_download.h       # libcurl Web Acquisition Interface
│   ├── parser.h              # libxml2 HTML Parser Interface
│   ├── cleaner.h             # Data Normalization Engine Interface
│   ├── buffer.h              # Thread-Safe Bounded Buffer Interface
│   ├── producer.h            # Producer Thread Interface
│   ├── csv_consumer.h        # CSV Consumer Thread Interface
│   ├── mysql_consumer.h      # MySQL Consumer Thread Interface
│   ├── database.h            # MySQL Database Access Layer
│   ├── crud.h                # Terminal Menu & Full Pipeline Interface
│   └── csv.h                 # CSV Exporter Interface
├── src/                      # C Implementation Files
│   ├── main.c                # Application Entry Point
│   ├── curl_download.c       # HTTP Memory Buffer Downloader
│   ├── parser.c              # DOM Tree Table Row & Cell Parser
│   ├── cleaner.c             # Footnote Stripping & Value Cleaner
│   ├── buffer.c              # POSIX Semaphore & Mutex Bounded Buffer
│   ├── producer.c            # Dual-Buffer Producer Implementation
│   ├── csv_consumer.c        # Asynchronous CSV Writing Consumer
│   ├── mysql_consumer.c      # Asynchronous MySQL Ingestion Consumer
│   ├── database.c            # MySQL Connection & CRUD SQL Queries
│   ├── crud.c                # Interactive Menu Logic
│   └── csv.c                 # Database to CSV Export Engine
├── data/
│   └── stadiums.csv          # Generated CSV Dataset
├── database/
│   ├── schema.sql            # MySQL Database Schema DDL
│   └── seed.sql              # Initial Seed Dataset
├── python/
│   ├── visualize.py          # Python Visualization Script
│   └── requirements.txt      # Python Package Dependencies
├── output/                   # Output Visualizations
│   ├── stadiums_by_state.png     # Chart 1: Bar Chart (State Distribution)
│   ├── sport_distribution.png    # Chart 2: Pie Chart (Sports Distribution)
│   └── capacity_distribution.png # Chart 3: Horizontal Bar Chart (Top Capacity)
├── docs/                     # Technical Documentation
│   ├── architecture.md       # Technical Architecture Specification
│   ├── testing.md            # Empirical Test Matrix & Verification Log
│   └── project_report.md     # Full Academic Project Report
└── bin/                      # Compiled Executables
```

---

## 3. Mandatory Prerequisites & Setup

### Environment Requirements
* **Compiler**: MinGW-w64 / GCC (C99 support)
* **Build Tool**: `make`
* **C Libraries**:
  - `libcurl`
  - `libxml2`
  - `libmariadb` / `mysql-client`
  - `pthread` (POSIX Threads)
* **Database**: MariaDB / MySQL Database Server (running on `127.0.0.1:3306`)
* **Python**: Python 3.8+ with packages: `pandas`, `matplotlib`, `mysql-connector-python`

### Quick Setup Commands (MSYS2 / Windows)
```bash
# Install MSYS2 UCRT64 toolchain & packages
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-make \
          mingw-w64-ucrt-x86_64-curl mingw-w64-ucrt-x86_64-libxml2 \
          mingw-w64-ucrt-x86_64-libmariadbclient

# Install Python packages
pip install pandas matplotlib mysql-connector-python
```

---

## 4. Compilation & Building

To build the project executable:

```bash
# Clean previous build artifacts and compile
make clean
make
```

The compiled binary will be placed at `bin/stadium_system.exe`.

---

## 5. Execution Guide

### Step 1: Ensure MySQL/MariaDB Server is Running
Ensure MariaDB/MySQL is listening on port `3306`. The database `indian_stadium_db` will be created automatically by the C program if it does not exist.

### Step 2: Run the C Application
```bash
./bin/stadium_system.exe
```

Upon launching:
1. The program automatically checks if the database contains records.
2. If empty, it executes the end-to-end multithreaded web scraper pipeline.
3. The interactive CRUD menu will open.

### Interactive Menu Options
```text
=======================================================
        INDIAN STADIUM MANAGEMENT SYSTEM (C & MySQL)   
=======================================================
  1. Add New Stadium (CREATE)
  2. Display All Stadiums (READ)
  3. Search Stadiums
  4. Update Stadium Information (UPDATE)
  5. Delete Stadium Record (DELETE)
  6. Export Database Records to CSV
  7. Re-run Web Scraper Pipeline (libcurl + libxml2 + Threads)
  8. Exit System
=======================================================
```

### Step 3: Run Python Visualizations
Generate analytical charts from the populated database:

```bash
python python/visualize.py
```

The generated PNG images will be saved in `output/`:
- `output/stadiums_by_state.png`
- `output/sport_distribution.png`
- `output/capacity_distribution.png`

---

## 6. Academic Requirement Verification Checklist

- [x] **C Language & GCC** used throughout system
- [x] **libcurl** used for real HTTP Wikipedia download
- [x] **libxml2** used for real HTML table DOM parsing
- [x] **Data Cleaner** normalizes capacity, dates, and removes footnotes
- [x] **pthread** creates Producer, CSV Consumer, and MySQL Consumer threads
- [x] **POSIX Semaphores** (`sem_t empty`, `sem_t full`) synchronize bounded buffer
- [x] **POSIX Mutexes** (`pthread_mutex_t`) protect shared ring buffer
- [x] **MySQL C API** (`libmariadb`) used for database operations
- [x] **CRUD Operations** fully implemented (Create, Read, Search, Update, Delete)
- [x] **CSV Export** implemented and verified
- [x] **Python Analytics** (Pandas + Matplotlib) generates 3 PNG charts
- [x] **Makefile** provides clean build and link rules
- [x] **Documentation & Architecture Diagrams** fully provided in `docs/`
