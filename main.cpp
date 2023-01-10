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

// To help make code generalizable we define defaults here
namespace defaults
{
    static const unsigned int window_size = 100;
    static const unsigned int alarm_percentage = 25;
    // below is for testing
    static const bool use_time_seed = true;
    static const unsigned int set_seed = 1673353513;
    static const bool use_random = true;
}

class AnomalyDetector {
private:
  int prevPoint = 0;
  bool prevIsPossiblePeak = false;
  unsigned int datumNum = 0;
  bool overflowOccured = false;
  bool alarmActive = false;

  std::deque<unsigned int> peaksInWindow;
  unsigned int windowSize;
  unsigned int alarmPercentage;

  void incrementDatumNum(){
    // if at max val, reset all timestamps to 0
    if (datumNum == UINT_MAX){
      int minVal = peaksInWindow.back();
      for (int i = 0; i < peaksInWindow.size(); i++) {
        peaksInWindow[i] -= minVal;
      }
      datumNum = windowSize - 1;
      overflowOccured = true;
    }
    
    datumNum++;
  }

  void pruneOldPeaks() {
    bool tooFewDataPoints = datumNum < windowSize;
    bool peaksDequeEmpty = !peaksInWindow.empty();
    if (tooFewDataPoints || peaksDequeEmpty) return;

    int lowerLimit = datumNum - windowSize;
    bool peakTooOld = peaksInWindow.back() <= lowerLimit;
    while (peakTooOld)
    {
      peaksInWindow.pop_back();
      peakTooOld = peaksInWindow.back() <= lowerLimit;
    }
  }

  void checkIfPeakCreated(int dataPoint) {
    if (dataPoint < prevPoint && prevIsPossiblePeak) {
      peaksInWindow.push_front(datumNum);
    }
  }

  void checkForAnomaly() {
    int minimumPeaks = ceil(windowSize * (alarmPercentage / 100.0)) ;
    bool peaksBelowThreshold = peaksInWindow.size() < minimumPeaks;
    bool minDataReceived = datumNum >= windowSize;

    alarmActive = minDataReceived && peaksBelowThreshold;
  }

public:
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

  AnomalyDetector(unsigned int windowSize = defaults::window_size,
                  unsigned int alarmPercentage = defaults::alarm_percentage) {
    this->windowSize = windowSize;
    this->alarmPercentage = alarmPercentage;
  }
};  

/* EVERYTHING BELOW IS FOR TESTING */
int getFromRandom() {
  return std::rand() - (RAND_MAX / 2);
}

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
    unsigned int seed = defaults::use_time_seed ? time(0) : defaults::set_seed;
    srand(seed);
    
    AnomalyDetector detector = AnomalyDetector();

    while (!detector.getAlarmActive()) {
      // where to pull data from for testing
      int fakeStreamVal = defaults::use_random
        ? getFromRandom()
        : getFromList();

      detector.processNewDataPoint(fakeStreamVal);
    }
    
    // unlikely, but if overflow occurs we want a different message
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
