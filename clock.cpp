
#include <iostream>
#include <chrono>
#include <thread>

using namespace std;
using namespace std::chrono;
typedef std::chrono::microseconds mus;
typedef std::chrono::seconds s;



auto bpm_to_midi_clock_ticks(float BPM){
//   convert Time to microsecond midi tick
    long long microseconds_per_tick = 60000000 / (96 * BPM);

    return microseconds(microseconds_per_tick);
}


// Timer function
void timer(chrono::microseconds tick){
  static int tick_count = 0;

  while(true){
// get time now
  auto start_time = chrono::steady_clock::now();
//sleep for 1 tick
  this_thread::sleep_for(tick);
// get time after sleeping
  auto end_time = chrono::steady_clock::now();
// calculate elapsed time
  auto elapsed_time = duration_cast<microseconds>(end_time-start_time).count();
// check if we really slept for 1 tick if not sleep for the remaining time should smooth out clock jitters
  auto remaining_time = tick.count() - elapsed_time;
  if ( remaining_time >0){
    this_thread::sleep_for(microseconds(remaining_time));
  }
}
}

 int main(){
      float BPM = 60;
      auto tick = bpm_to_midi_clock_ticks(BPM);
 return 0;

}

