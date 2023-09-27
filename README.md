# Fast Fourier Transform with C++

## Overview

This repository contains a C++ program that performs the Fast Fourier Transform (FFT) using the Cooley-Tukey algorithm. The program is designed to run on a CPU and is compiled using GCC. The time taken for performing FFT is also measured.

## Dependencies

- C++11 or later
- GCC (GNU Compiler Collection)
- Standard Libraries: `<iostream>`, `<cmath>`, `<vector>`, `<complex>`, `<chrono>`, `<fstream>`

## Compilation

To compile the code, navigate to the project directory and run the following command:

```bash
g++ -o fft_program main.cpp -std=c++11 -lm
```

This will compile the code and produce an executable named `fft_program`.

## Running the Program

Run the compiled code with the following command:

```bash
./fft_program
```

## Features

- Zero-pads the input array to the next power of 2.
- Utilizes the Cooley-Tukey FFT algorithm to transform the input in place.
- Measures the time taken for the FFT.
- Outputs the FFT results into a CSV file.

## Output

After running the program, the time taken for the FFT operation will be printed in the terminal. A CSV file named `fft_output.csv` will be generated, containing the real and imaginary parts of the transformed data.

## License

This project is open source and available under [MIT License](LICENSE).
