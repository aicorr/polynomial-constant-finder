# Polynomial Constant Term Finder

This C++ program implements Shamir's Secret Sharing algorithm to find the constant term of a polynomial given its roots in JSON format.

## Problem Description

Given a polynomial f(x) = ax^n + bx^(n-1) + ... + c, the program finds the constant term 'c' using:
- Lagrange interpolation
- Base conversion from various number systems
- JSON parsing for input data

## Features

- **JSON Parsing**: Custom JSON parser to handle the specific input format
- **Base Conversion**: Converts numbers from any base (2-36) to decimal
- **Lagrange Interpolation**: Calculates the polynomial constant term
- **Error Handling**: Robust error handling for invalid inputs

## Compilation

```bash
make
```

Or manually:
```bash
g++ -std=c++11 -Wall -Wextra -O2 -o polynomial_solver main.cpp
```

## Usage

```bash
make run
```

Or:
```bash
./polynomial_solver
```

## Algorithm

1. **Parse JSON**: Extract points with x-coordinates, bases, and values
2. **Convert Bases**: Convert y-values from their respective bases to decimal
3. **Lagrange Interpolation**: Use the first k points to find f(0) where:
   - f(0) represents the constant term 'c'
   - k is the minimum number of points needed (degree + 1)

## Test Cases

### Test Case 1
- n = 4 points, k = 3 needed
- Points in bases 10, 2, 10, 4
- Expected to find the constant term of a quadratic polynomial

### Test Case 2  
- n = 10 points, k = 7 needed
- Points in various bases (3, 6, 7, 8, 12, 15, 16)
- Expected to find the constant term of a 6th degree polynomial


## 🛠️ Compilation & Usage

### Prerequisites
- C++11 compatible compiler (GCC, Clang, MSVC)
- Standard C++ library

### Build Instructions

**Linux/macOS:**
```bash
make
make run
```

### Manual Compilation
g++ -std=c++11 -Wall -Wextra -O2 -o polynomial_solver main.cpp
./polynomial_solver