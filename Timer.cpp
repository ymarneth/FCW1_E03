/* Timer.c(pp)                                          HDO, 1998-2020
   ----------
   Simple utility to measure run-times for C and C++.
======================================================================*/

#ifndef __cplusplus // C
  #include <time.h>
#else // C++
  #include <chrono>
  #include <ctime>
  using namespace std;
#endif

#include "Timer.h"

#define USE_HIGH_RES_TIMING_FOR_CPP /*undef to use low res. timing for C++*/

#if  (defined(__cplusplus) && defined(USE_HIGH_RES_TIMING_FOR_CPP))
  #define HIGH_RES_TIMING  /*defined to enable HRT, for C++ only*/
#endif

/*definition of globals used in all functions*/
#ifdef HIGH_RES_TIMING
  static chrono::high_resolution_clock::time_point start_tp;
  static chrono::high_resolution_clock::time_point stop_tp;
#else
  static clock_t start_ticks;
  static clock_t stop_ticks;
#endif

void startTimer() {
  #ifdef HIGH_RES_TIMING
    start_tp = chrono::high_resolution_clock::now();
  #else
    start_ticks = clock();
  #endif
} /*startTimer*/

void stopTimer() {
  #ifdef HIGH_RES_TIMING
    stop_tp = chrono::high_resolution_clock::now();
  #else
    stop_ticks = clock();
  #endif
} /*stopTimer*/

double elapsedTime() {  /*returns elapsed time in seconds*/
  #ifdef HIGH_RES_TIMING
    return
      chrono::duration_cast<chrono::milliseconds>
        (stop_tp - start_tp).count() / 1000.0;
  #else
    return (double)(stop_ticks - start_ticks) / CLOCKS_PER_SEC;
  #endif
} /*elapsedTime*/


/* === test ==========================================================*/

#if 0

#ifndef __cplusplus
  #include <stdio.h>
#else
  #include <iostream>
  using namespace std;
#endif

int main(int argc, char *argv[]) {

  int runs, r, n, i, j;
  runs = 5; /*number of runs with ...*/
  n = 5000; /*... n * n simple computations in each run*/

#ifndef __cplusplus
    printf("C version, ");
#else
    cout << "C++ version, ";
#endif
#ifdef HIGH_RES_TIMING
  cout << "elapsed time with HIGH resolution timing:" << endl;
#else
  #ifndef __cplusplus
    printf( "elapsed time with LOW resolution timing:\n");
  #else
    cout << "elapsed time with LOW resolution timing:" << endl;
  #endif
#endif
  for (r = 0; r < runs; r++) {
    startTimer();
    for (i = 0; i < n; i++)
      for (j = 0; j < n; j++) {
        i = i + 17;
        j = j * 21;
        i = i - 17;
        j = j / 21;
      } /*for*/
    stopTimer();
#ifndef __cplusplus
    printf("%f\n", elapsed());
#else
    cout << elapsedTime() << std::endl;
#endif
  } /*for*/
  return 0;

} /*main*/

#endif

/*end of Timer.c(pp)
======================================================*/
