# System Architecture & Technical Specification
## Indian Stadium Management System

---

## 1. Architectural Overview

The **Indian Stadium Management System** is built as an asynchronous multi-tier software system combining low-level C system programming, POSIX multithreading and synchronization, libcurl network ingestion, libxml2 DOM-tree parsing, relational database storage via MySQL/MariaDB C API, and analytical visualizations using Python.

```
                      +------------------------------------------+
                      | Wikipedia: List of stadiums in India    |
                      +--------------------+---------------------+
                                           |
                                           v
                      +--------------------+---------------------+
                      | Ingestion Module (libcurl HTTP)          |
                      +--------------------+---------------------+
                                           |
                                           v
                      +--------------------+---------------------+
                      | DOM Parser & Cleaner (libxml2)           |
                      +--------------------+---------------------+
                                           |
                                           v
                      +--------------------+---------------------+
                      | Producer Thread                          |
                      +----------+--------------------+----------+
                                 |                    |
        Bounded Buffer (CSV)     |                    | Bounded Buffer (MySQL)
        sem_empty / sem_full     |                    | sem_empty / sem_full
        pthread_mutex_t          |                    | pthread_mutex_t
                                 v                    v
                      +----------+---------+  +-------+----------+
                      | CSV Consumer Thread|  | MySQL Consumer   |
                      +----------+---------+  +-------+----------+
                                 |                    |
                                 v                    v
                      +----------+---------+  +-------+----------+
                      | CSV File Dataset   |  | MySQL Database   |
                      | (data/stadiums.csv)|  | (stadiums table) |
                      +--------------------+  +-------+----------+
                                                      |
                                                      v
                                              +-------+----------+
                                              | Menu CRUD C App  |
                                              +-------+----------+
                                                      |
                                                      v
                                              +-------+----------+
                                              | Python Analytics |
                                              | Pandas+Matplotlib|
                                              +------------------+
```

---

## 2. Component Breakdown

### 2.1 HTTP Acquisition Module (`curl_download.c`)
- **Technology**: `libcurl` (CURL_GLOBAL_ALL).
- **Callback**: `MemoryBuffer` structure with dynamic reallocation via `realloc`.
- **User-Agent**: Configured with modern browser User-Agent header (`Mozilla/5.0...`) to bypass Wikipedia automated request filters.
- **Error Handling**: Verifies `CURLcode` and HTTP status codes (`HTTP 200 OK`).

### 2.2 HTML DOM Parser & Data Normalization (`parser.c` & `cleaner.c`)
- **Technology**: `libxml2` (`htmlReadMemory`, DOM tree iteration).
- **Dynamic Header Detection**: Inspects table headers (`<th>` cells) to dynamically align column positions for Stadium Name, Capacity, City, State, Sport, Home Team, and Opened Year.
- **Data Normalization**: Strips footnote citations (`[1]`, `[12]`), footnote symbols (`†`, `‡`), trims whitespace, removes thousand-separator commas, converts raw string metrics into validated integers, and normalizes empty fields with defaults.

### 2.3 Multithreaded Bounded Buffer Architecture (`buffer.c`, `producer.c`, `csv_consumer.c`, `mysql_consumer.c`)
- **Producer-Consumer Design**: A Producer thread reads cleaned `Stadium` records and populates two concurrent bounded buffers:
  1. `buffer_csv`: Consumed asynchronously by `CSV Consumer Thread`.
  2. `buffer_mysql`: Consumed asynchronously by `MySQL Consumer Thread`.
- **Synchronization Primitives**:
  - `sem_t empty`: Tracks available empty slots in the buffer (initialized to `BUFFER_CAPACITY = 10`).
  - `sem_t full`: Tracks filled slots available for reading (initialized to `0`).
  - `pthread_mutex_t lock`: Ensures strict mutual exclusion during buffer insertion (`enqueue`) and removal (`dequeue`).
- **Sentinel Termination Protocol**: Upon pushing all parsed stadium records, the Producer pushes a sentinel record (`stadium_id = -1`) to both buffers. Consumers detect the sentinel, break their consumer loop, and terminate gracefully.

### 2.4 Relational Database Management (`database.c`, `schema.sql`)
- **Technology**: MariaDB / MySQL C API (`libmariadb`).
- **Data Integrity**: Parameter escaping (`mysql_real_escape_string`) prevents SQL injection vulnerabilities.
- **Auto-Initialization**: Program detects database presence, connects to `127.0.0.1:3306`, and auto-creates schema if absent.

### 2.5 Terminal CRUD User Interface (`crud.c`, `csv.c`)
- Interactive C menu enabling real-time database management:
  - **Create**: Add new stadium records with input validation.
  - **Read**: Display formatted tabular views, search by Name, City, State, or Sport.
  - **Update**: Modify record fields selectively.
  - **Delete**: Remove records by ID with user confirmation prompt.
  - **Export**: Export database records directly to standard CSV format.

### 2.6 Python Data Analytics (`visualize.py`)
- Reads dataset directly from MySQL database server.
- Generates 3 visualization figures:
  1. `stadiums_by_state.png`: Vertical bar chart showing stadium density across Indian states.
  2. `sport_distribution.png`: Donut / Pie chart visualizing distribution of sports hosted across facilities.
  3. `capacity_distribution.png`: Horizontal bar chart ranking top Indian stadiums by seating capacity.
