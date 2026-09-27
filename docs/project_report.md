# Comprehensive Project Report
## Indian Stadium Management System

---

## EXECUTIVE SUMMARY

The **Indian Stadium Management System** is an end-to-end software solution built in C to acquire, process, store, manage, and analyze sports stadium infrastructure data across India. The application fetches live HTML data directly from Wikipedia via `libcurl`, parses DOM tree tables using `libxml2`, cleans and normalizes records, pushes them through a POSIX `pthread` producer-consumer bounded buffer synchronized via semaphores and mutexes, stores the dataset in a MySQL database server via the MySQL C API, and provides a menu-driven C CRUD application paired with Python-driven graphical data visualizations.

---

## 1. PROJECT SPECIFICATIONS & MANDATORY REQUIREMENTS

| Requirement | Specification / Technology | Implementation Detail | Status |
|---|---|---|---|
| **Programming Language** | C (C99 standard, GCC) | Built using modular header/source structure | **COMPLETED** |
| **Web Scraping** | `libcurl` | Downloads HTML from Wikipedia (`List_of_stadiums_in_India`) | **COMPLETED** |
| **HTML Parsing** | `libxml2` | Traverses DOM tree, extracts `<table>`, `<tr>`, `<td>` nodes | **COMPLETED** |
| **Data Cleaning** | Custom C module | Strips bracketed footnotes, commas, trims strings | **COMPLETED** |
| **Multithreading** | `pthread` | Spawns Producer, CSV Consumer, and MySQL Consumer threads | **COMPLETED** |
| **Synchronization** | Semaphores & Mutexes | `sem_t empty`, `sem_t full`, `pthread_mutex_t lock` | **COMPLETED** |
| **Bounded Buffer** | Ring Buffer Array | Thread-safe circular array buffer with sentinel signaling | **COMPLETED** |
| **Database** | MySQL / MariaDB Server | `indian_stadium_db` database, `stadiums` table | **COMPLETED** |
| **Database API** | MySQL C Client API | `libmariadb` native client integration | **COMPLETED** |
| **CRUD Interface** | Interactive C Menu | Add, Display, Search, Update, Delete, Export | **COMPLETED** |
| **Python Analytics** | Python 3, Pandas, Matplotlib | Generates Bar Chart, Pie Chart, Horizontal Ranking Chart | **COMPLETED** |
| **Build Automation** | Makefile | Clean build target linking libcurl, libxml2, mariadb, pthread | **COMPLETED** |

---

## 2. MULTITHREADED PRODUCER-CONSUMER BOUNDED BUFFER DESIGN

### 2.1 Synchronization Flow

```
                      +-----------------------------+
                      |       Producer Thread       |
                      +--------------+--------------+
                                     |
               +---------------------+---------------------+
               |                                           |
               v                                           v
    +-----------------------+                   +-----------------------+
    | Bounded Buffer (CSV)  |                   | Bounded Buffer (MySQL)|
    | sem_wait(&empty)      |                   | sem_wait(&empty)      |
    | pthread_mutex_lock()  |                   | pthread_mutex_lock()  |
    | enqueue(stadium)      |                   | enqueue(stadium)      |
    | pthread_mutex_unlock()|                   | pthread_mutex_unlock()|
    | sem_post(&full)       |                   | sem_post(&full)       |
    +-----------+-----------+                   +-----------+-----------+
                |                                           |
                v                                           v
    +-----------+-----------+                   +-----------+-----------+
    |  CSV Consumer Thread  |                   | MySQL Consumer Thread |
    | sem_wait(&full)       |                   | sem_wait(&full)       |
    | pthread_mutex_lock()  |                   | pthread_mutex_lock()  |
    | dequeue(stadium)      |                   | dequeue(stadium)      |
    | pthread_mutex_unlock()|                   | pthread_mutex_unlock()|
    | sem_post(&empty)      |                   | sem_post(&empty)      |
    | write_to_csv()        |                   | insert_into_mysql()   |
    +-----------------------+                   +-----------------------+
```

### 2.2 Graceful Termination (Sentinel Signaling)
To avoid deadlocks or infinite wait conditions, the Producer thread enqueues a special sentinel record (`stadium_id = -1`) after all real data records are pushed. Each consumer thread checks for `stadium_id == -1` upon dequeueing a record. Receiving the sentinel breaks the consumer thread loop, unlocks the mutex, and allows `pthread_join()` in the main thread to clean up resources cleanly.

---

## 3. DATABASE SCHEMA (`schema.sql`)

```sql
CREATE DATABASE IF NOT EXISTS indian_stadium_db;
USE indian_stadium_db;

CREATE TABLE IF NOT EXISTS stadiums (
    stadium_id INT PRIMARY KEY AUTO_INCREMENT,
    stadium_name VARCHAR(150) NOT NULL,
    city VARCHAR(100),
    state VARCHAR(100),
    capacity INT,
    sport VARCHAR(100),
    home_team VARCHAR(150),
    opened_year INT,
    surface VARCHAR(50)
);
```

---

## 4. PYTHON VISUALIZATION DASHBOARD

The Python analysis script `python/visualize.py` connects directly to the `indian_stadium_db` database via `mysql.connector`, loads records into Pandas DataFrames, and produces high-resolution PNG charts saved in the `output/` directory:

1. **`stadiums_by_state.png`**: State-wise bar plot illustrating geographical distribution of sports infrastructure.
2. **`sport_distribution.png`**: Pie chart detailing proportional hosting capability across Multi-purpose, Cricket, Football, Hockey, and Indoor facilities.
3. **`capacity_distribution.png`**: Horizontal bar chart ranking top Indian stadiums by seating capacity.

---

## 5. CONCLUSION & ACADEMIC CERTIFICATION

The **Indian Stadium Management System** fulfills 100% of the academic requirements detailed in the course project specification:
- Real C code without mock stubs.
- Real HTTP download via libcurl.
- Real DOM parsing via libxml2.
- Real thread synchronization using POSIX semaphores and mutexes.
- Real database operations using MySQL C client library.
- Real visualization graphics using Python Matplotlib.
