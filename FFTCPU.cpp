#include <iostream>
#include <cmath>
#include <vector>
#include <complex>
#include <chrono> // Include this for timing
#include <fstream>  // Include this for file I/O

typedef std::complex<double> Complex;

void fft(std::vector<Complex> &a)
{
    int n = a.size();
    if (n <= 1)
        return;

    std::vector<Complex> even(n / 2), odd(n / 2);

    for (int i = 0; i < n / 2; ++i)
    {
        even[i] = a[i * 2];
        odd[i] = a[i * 2 + 1];
    }

    fft(even);
    fft(odd);

    for (int i = 0; i < n / 2; ++i)
    {
        Complex t = std::polar(1.0, -2 * M_PI * i / n) * odd[i];
        a[i] = even[i] + t;
        a[i + n / 2] = even[i] - t;
    }
}

int main()
{
    // Your original array
    std::vector<Complex> a = {0, 1, 2, 3, 4, 5, 6, 7, 1, 2, 2, 7, 2, 6, 1, 5, 7, 3, 2, 6, 1, 7, 8, 2, 2, 7, 9, 10, 15, 12, 12, 124, 1, 2, 5, 2, 4, 6, 1, 7, 9, 1, 6, 4, 3, 2, 5, 6, 7, 4, 3, 2, 6, 4, 6, 4, 6, 3, 7, 2, 8, 5, 7, 5, 2, 5, 1};

    // Find the next power of 2 and zero-pad the array
    int original_size = a.size();
    int padded_size = 1;
    while (padded_size < original_size)
    {
        padded_size *= 2;
    }
    a.resize(padded_size, 0);

    // Start the timer
    auto start = std::chrono::high_resolution_clock::now();

    fft(a);

    // Stop the timer
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    // Print the duration
    std::cout << "Time taken for FFT on CPU: " << duration.count() << " microseconds" << std::endl;

    // Open a file for writing
    std::ofstream file("fft_output.csv");

    // Write the header
    file << "Real,Imaginary\n";

    // Write the FFT output to the file
    for (const auto &x : a)
    {
        file << x.real() << ',' << x.imag() << '\n';
    }

    // Close the file
    file.close();

    return 0;
}
