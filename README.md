# CodingChallenge (ARG)
## Summary of Changes
A few of the main problems with the original code were
* rand() not seeded.
* The set of 100 points was not tested continously. Needed a sliding window.
* Unnecessarily assumes no negative values will come in stream.
* Repeated hardcoded values that needed abstracting. This can easily lead to generality.
* Using static within the namespace prevents reusability.
* A few minor-ish problems with readability that could be easily resolved.

These have all been resolved in my modified code.

## Testing
### Working Correctly
I tested by using CMAKE to generate the makefile and then ran the makefile in shell with 
`
make
`
in the directory with my makefile. Then I ran the generated binary, which for me was named `AnomalyDetector` with `./AnomalyDetector` within the right working directory. I then repeatedly ran `./AnomalyDetector` while observing the different values printed for every run. I can also manually input the stream of numbers by modifying the `fakeStreamList` variable and switching `defaults::use_random` to `false`.

### Timing
If one wishes to do a timing test this can easily be done by wrapping the testing while loop with a for loop that should run a large number of times (say 10,000,000). The time is recorded before the for loop and then after the for loop. The delta is found and recorded. This can be done for different `windowSize` and `alarmPercentage` values as well.

## Implementing
The code is decently well commented. If implmented in a proper application, then the class and function declarations of `AnomalyDetector` should be extracted into its own `AnomalyDetector.hpp` file and given definition in an `AnomalyDetector.cpp` file. Then the `.hpp` file can be included in other files.
