# System Programming Project

**Name:** Atalay Ergenekon Karakaya
**ID:** B201202027    

This project contains two C applications:

- App1 (`q1` / `app1`): Process memory analyzer  
- App2 (`q2` / `app2`): Advanced Code Safety Analyzer  

Prepared and tested on Ubuntu (WSL2).

---

## 1. Build

Requirements: gcc, make

From project root:

    make

This creates:

- q1 / app1
- q2 / app2

Clean:

    make clean

---

## 2. App1 (q1 / app1) – Process Memory Analyzer

Features:

- Reads /proc/self/maps and /proc/self/status
- Shows text, data, BSS (if separate), heap, stack
- Lists shared libraries and VDSO
- Prints virtual vs physical memory usage
- Includes a simple heap leak demo

Usage (from project root):

    ./app1
    ./app1 --all
    ./app1 --monitor <seconds>
    ./app1 --leak-check

Test script:

    ./tests/test_q1.sh

---

## 3. App2 (q2 / app2) – Advanced Code Safety Analyzer

Checks C source files for:

- Unsafe C functions:
  - strcpy, strcat, sprintf, gets, scanf
- Unsafe time functions:
  - gmtime, localtime, ctime, asctime
- Extended issues (only in -x mode):
  - Format string vulnerabilities
  - Command injection
  - Possible integer overflow before malloc

General usage:

    ./app2 -s <file1.c>
    ./app2 -r <file1.c>
    ./app2 -x <file1.c>

Examples with sample files:

    ./app2 -s tests/samples/vulnerable1.c tests/samples/vulnerable2.c
    ./app2 -r tests/samples/vulnerable2.c
    ./app2 -x tests/samples/vulnerable2.c

Test script:

    ./tests/test_q2.sh

---

## 4. Short Layout

B201202027/
├── Makefile
├── README.md
├── docs/
│   └── REPORT.pdf
├── src/
│   ├── common/
│   │   ├── utils.c
│   │   └── utils.h
│   ├── q1/
│   │   ├── main.c
│   │   ├── memory_analyzer.c
│   │   ├── memory_analyzer.h
│   │   ├── proc_parser.c
│   │   ├── proc_parser.h
│   │   ├── leak_detector.c
│   │   └── leak_detector.h
│   └── q2/
│       ├── main.c
│       ├── security_checker.c
│       ├── security_checker.h
│       ├── code_parser.c
│       ├── code_parser.h
│       ├── recommender.c
│       └── recommender.h
└── tests/
    ├── test_q1.sh
    ├── test_q2.sh
    └── samples/
        ├── vulnerable1.c
        ├── vulnerable2.c
        └── safe_example.c