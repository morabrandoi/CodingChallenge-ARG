/* Instructions:
 * You have been tasked to evaluate and improve code produced by an intern. The code has not been assessed by anyone yet.
 * You need to provide feedback and improve the code explaining why you made the changes and what effect they have so that the intern can learn.
 * You are expected to demonstrate how to write high quality, mature, efficient and production ready code in line with high standards and development principles.
 *
 * The business requirements for the project were:
 *      * Create a solution that analyzes data from a continuous stream.
 *      * The ideal operation is when the data generates an integer value, followed by lower one, followed by higher one, followed by lower one, etc.
 *      * In that way it generates 50% peaks. Practically, during healthy operation we get about 33% of peaks.
 *      * We need to trigger an alarm if we get fewer than 25% of peaks over any continuous set of 100 samples.
 *
 * The data comes in very fast and you need to maximize throughput and provide the outcome as soon as possible.
 * The intern decided to use a random number generator to test the code. You can alter any function to suit your implementation.
 * You should provide some evaluation of the performance of your solution or at least an explanation how it should be done.
 * Please, make notes where you can think of vulnerabilities or potential issues with your implementation.
 * Your documentations must be sufficient for another engineer to implement this code into a larger application.
 */

// NOTE: README.md contains summary docs

#include <iostream>
#include <cstdlib>

namespace AnomalyDetector {
    class Buffer {
    public:
        // To intern: the repeated use of hardcoded "100" indicates we should
        // factor this out to its own static const variable.
        int buffer[100];  
        int index;
        Buffer() : buffer(), index(0){}

        // To intern: could use the modular arithmetic idiom to make this more
        // readable
        void Add(int value) {
            if (index == 100) {
                index = 0;
            }
            buffer[index] = value;
            index++;
        }
    };
    // To intern: binding the class to the entire namespace prevents multiple
    // anomaly detectors from running at once.
    static Buffer buffer1;

    // To intern: We need to see the randomness or it will be the same sequence
    // every time. Also why assume all values fall in [0,100]?
    int GetDataPoint() {
        buffer1.Add(rand() % 100);  
        return buffer1.index;
    }

    // To intern: this works but requires analyzing the whole array at once
    int PeakDetector() {
        int peaksDetected = 0;
        for (int i = 1; i < 98; ++i) {
            if (buffer1.buffer[i] > buffer1.buffer[i - 1] && buffer1.buffer[i] > buffer1.buffer[i + 1]) {
                peaksDetected++;
            }
        }
        return peaksDetected;
    }
}

int main() {
    // To intern: this implementation does not act over a sliding window. The
    // concept of "cycles" indicates this.
    int cycleCounter = 0;
    // To intern: "true" could be directly replaced with "peaks < 25" and the
    // else clause unpacked for a cleaner refactor.
    while (true) {
        while (AnomalyDetector::GetDataPoint() < 100) {}
        int peaks = AnomalyDetector::PeakDetector();
        if (peaks < 25) {
            std::cout << "Anomaly detected after " << std::to_string(cycleCounter) << " cycles!" << std::endl;
            break;
        } else {
            cycleCounter++;
        }
    }
    return 0;
}

// NOTE: README.md contains summary docs