#include <iostream>
#include <vector>

#include "medaid/integrate.hpp"

int main() {
    // Months since the first test, and the A1c result at each test.
    const std::vector<double> months{0.0, 1.0, 2.0, 14.0};
    const std::vector<double> a1c{8.0, 8.0, 8.0, 6.0};

    const double area = medaid::trapezoid(months, a1c);
    const double total_time = months.back() - months.front();

    std::cout << "Area under the line: " << area << "\n";
    std::cout << "Total time: " << total_time << " months\n";
    std::cout << "Time-weighted average: " << area / total_time << "\n";
}
