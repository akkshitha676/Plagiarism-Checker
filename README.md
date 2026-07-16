# Plagiarism Checker

A C++-based plagiarism detection system developed as part of the **CS293 Data Structures and Algorithms Laboratory** at **IIT Bombay**. The project progressively implements different techniques for analyzing and comparing source code submissions to detect potential plagiarism.

The repository is organized into multiple phases, demonstrating the evolution of the plagiarism detection system from basic preprocessing to a more comprehensive detection pipeline.

---

## Features

- Tokenization of C++ source code
- Source code normalization and preprocessing
- Multiple plagiarism detection strategies
- Modular implementation for different detection algorithms
- Docker support for a reproducible development environment
- Incremental implementation across multiple project phases

---

## Repository Structure

```
plagiarism_checker/
├── README.md
├── Dockerfile
├── problem_statement.pdf
├── phase1/
├── phase2/
└── phase3/
```

### Phase 1

Implements the foundational components required for plagiarism detection, including source code tokenization and preprocessing.

### Phase 2

Extends the initial implementation by introducing plagiarism detection logic and additional comparison mechanisms.

### Phase 3

Contains the final implementation of the plagiarism checker with multiple detection modules and the complete project pipeline.

---

## Technologies Used

- C++
- Standard Template Library (STL)
- Make
- Docker

---

## Building the Project

Navigate to the desired phase and build the project using the provided Makefile.

Example:

```bash
cd phase3
make
```

---

## Running

After building, execute the generated binary according to the requirements of the selected phase.

Example:

```bash
./main
```

> The executable name may vary depending on the phase or Makefile configuration.

---

## Project Evolution

The project is divided into three phases to illustrate the gradual development of the plagiarism detection system:

- **Phase 1:** Tokenization and preprocessing
- **Phase 2:** Similarity detection logic
- **Phase 3:** Complete plagiarism detection framework

---

## Documentation

The original project specification is included in the repository as:

```
problem_statement.pdf
```

---

## Course Information

This project was developed as part of the **CS293 Data Structures and Algorithms Laboratory** at **IIT Bombay**.

The repository has been cleaned and reorganized for portfolio purposes while preserving the original implementation.

---

## Future Improvements

Possible extensions include:

- Support for additional programming languages
- Improved similarity metrics
- AST-based code comparison
- Web-based visualization interface
- Performance benchmarking on larger datasets
