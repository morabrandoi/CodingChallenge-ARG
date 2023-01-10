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
 * The intern decided to use a random number generator to test the code. You can alter and modify the GetDataPoint() function to suit your implementation.
 * You should provide some evaluation of the performance of your solution or at least an explanation how it should be done.
 * Please, make notes where you can think of vulnerabilities or potential issues with your implementation.
 * Your documentations must be sufficient for another engineer to implement this code into a larger application.
 */

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <climits>
#include <deque>
#include <cmath>

// To intern: we define defaults here to make the code reusable/generalizable
namespace defaults
{
    static const unsigned int window_size = 100;
    static const unsigned int alarm_percentage = 25;
    // below is for testing
    static const bool use_time_seed = true;
    static const unsigned int set_seed = 1673353513;
    static const bool use_random = true;
}

// To intern: Class instead of namespace for reusabiltiy
class AnomalyDetector {
private:
  int prevPoint = 0;
  bool prevIsPossiblePeak = false;
  unsigned int datumNum = 0;
  bool overflowOccured = false;
  bool alarmActive = false;

  // To intern: only storing relevant peaks saves space and time (constant time
  // insertion/deletion at front and back).

  // Notes: if the application requires more implementation details like
  // copy constructors, destructors, iterators, etc. Then it might make sense to
  // have the deque be public. This may cause unwanted access to other methods.
  std::deque<unsigned int> peaksInWindow;
  unsigned int windowSize;
  unsigned int alarmPercentage;

  void incrementDatumNum(){
    // To intern: resetting all time step vals but keeping relative order
    if (datumNum == UINT_MAX){
      overflowOccured = true;

      int offset = UINT_MAX - windowSize;

      // modifying all recent peaks
      for (int i = 0; i < peaksInWindow.size(); i++) {
        peaksInWindow[i] -= offset;
      }
      // modifying datanum itself. Subtracting 1 more because next line adds 1
      datumNum -= (offset + 1);
    }
    
    datumNum++;
  }

  // To intern: if peak falls outside of window it is no longer relevant so we
  // delete it from our queue.
  // Note: the implementation here is nice because we could easily change the
  // code to instead work on the last 100 seconds of datapoints instead of just
  // the last 100 datapoints. The pruning would be time based as would the
  // inserting of peaks.
  void pruneOldPeaks() {
    bool tooFewDataPoints = datumNum < windowSize;
    bool peaksDequeEmpty = !peaksInWindow.empty();
    if (tooFewDataPoints || peaksDequeEmpty) return;

    int lowerLimit = datumNum - windowSize;
    while (peaksInWindow.back() <= lowerLimit) peaksInWindow.pop_back();
  }

  // To intern: this is how to check for peaks as we consume the stream
  void checkIfPeakCreated(int dataPoint) {
    if (dataPoint < prevPoint && prevIsPossiblePeak) {
      peaksInWindow.push_front(datumNum);
    }
  }

  // To intern: deriving minimumPeaks good for reusability/generalization. Also
  // we check minDataReceived because we need to have enough datapoints before
  // checking if there is an anomaly.
  void checkForAnomaly() {
    int minimumPeaks = ceil(windowSize * (alarmPercentage / 100.0)) ;
    bool peaksBelowThreshold = peaksInWindow.size() < minimumPeaks;
    bool minDataReceived = datumNum >= windowSize;

    alarmActive = minDataReceived && peaksBelowThreshold;
  }

public:
  // To intern: large integrating functions should be highly readable.
  void processNewDataPoint(int dataPoint) {
    incrementDatumNum();
    pruneOldPeaks();
    checkIfPeakCreated(dataPoint);

    checkForAnomaly();

    prevIsPossiblePeak = dataPoint > prevPoint && datumNum > 1;
    prevPoint = dataPoint;
  }

  // getters
  bool getAlarmActive() {return alarmActive;}
  bool getOverflowOccured() {return overflowOccured;}
  int getDatumNum() {return datumNum;}

  // To intern: by allowing for params we add reuasbility.

  // Note: We should be checking the inputs to prevent underflow since we are
  // dealing with unsigned ints
  AnomalyDetector(unsigned int windowSize = defaults::window_size,
                  unsigned int alarmPercentage = defaults::alarm_percentage) {
    this->windowSize = windowSize;
    this->alarmPercentage = alarmPercentage;
  }
};  

/* EVERYTHING BELOW IS FOR TESTING */

// To intern: it was never specified that all stream values would be nonnegative
// so it is important to include negative values as we test.
// Note: It would be cool in the future to implement sampling from a custom
// distribution that more accurately reflects the real-world data.
int getFromRandom() {
  return std::rand() - (RAND_MAX / 2);
}

// To intern: to better test corner cases or investigate errors we should have
// an option for manual input which is below.
// Note: a vector or array would make more sense here but this would require
// either an extra import or an extra global var so it is avoided for now. Also
// the hardcoded list can result in segfault if the data is such that an alarm
// is never thrown.
std::deque<int> fakeStreamList = {
  1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,3,3,3,3,3,3,3,3,3,3,3,3,3,
  3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3
  };
int getFromList() {
  int ret = fakeStreamList.front();
  fakeStreamList.pop_front();
  return ret;
}

int main() {
    // getting random seed from time or from given
    // Note: If this were a proper production testing environment we would want
    // to print the seed to some sort of log file. It also may make sense to
    // pull the set seed from arguments passed in via shell.
    unsigned int seed = defaults::use_time_seed ? time(0) : defaults::set_seed;
    // To intern: We would benefit from a different random number everytime and
    // to ensure reproducability we also should be setting + recording the seed
    srand(seed);
    
    AnomalyDetector detector = AnomalyDetector();

    // To intern: note how since we pull one data point at a time we are hitting
    // the goal of checking *continous* sets of 100. This requires a sliding
    // window technique which is implemented in the detector class.
    while (!detector.getAlarmActive()) {
      // deciding where to pull data from for testing
      int fakeStreamVal = defaults::use_random
        ? getFromRandom()
        : getFromList();

      detector.processNewDataPoint(fakeStreamVal);
    }
    
    // To intnrn: it is unlikely, but if overflow occurs we want
    // a different message to be printed
    std::cout << std::endl;
    std::cout << "Random seed used: " << seed << std::endl;
    std::cout << "Anomaly detected after "
              << (detector.getOverflowOccured()
                ? std::to_string(UINT_MAX) + "+"
                : std::to_string(detector.getDatumNum()))
              << " data points." << std::endl;
    std::cout << std::endl;
    return 0;
}
