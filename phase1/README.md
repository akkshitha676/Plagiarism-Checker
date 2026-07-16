# Phase 1 – Tokenization

## Overview

Phase 1 implements the initial components of the plagiarism detection system. The focus is on tokenizing C++ source code and validating the implementation using a collection of test cases.

## Directory Structure

```text
phase1/
├── Makefile
├── main.cpp
├── tokenizer.cpp
├── match_submissions.hpp
└── testcases/
```

## Components

| File | Description |
|------|-------------|
| `main.cpp` | Entry point for Phase 1 |
| `tokenizer.cpp` | Implements source code tokenization |
| `match_submissions.hpp` | Interface used for submission matching |
| `testcases/` | Sample programs and expected outputs for validating the tokenizer |

## Building

```bash
make
```

## Running

```bash
./main
```

The executable name depends on the generated build output.
