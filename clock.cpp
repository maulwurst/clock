#include <iostream>
#include <chrono>
#include <thread>
#include <vector>
#include <RtMidi.h>

using namespace std;
using namespace std::chrono;

typedef chrono::microseconds mus;
typedef chrono::seconds s;

struct MidiClockTimer {
    mus tick_duration;
    float bpm;
    int ppqn = 960;
    RtMidiOut* midi_out;  // RtMidi output object
    bool isRunning = false;
    bool stop_clock = false;

    // Constructor initializes RtMidi and sets up the clock
    MidiClockTimer(float bpm, int ppqn)
        : bpm(bpm), ppqn(ppqn), midi_out(nullptr) {

        // Initialize the MIDI output
        try {
            midi_out = new RtMidiOut();
            // Open the first available MIDI output port
            midi_out->openPort(0);
        }
        catch (RtMidiError& error) {
            error.printMessage();
            midi_out = nullptr;
        }

        tick_duration = midi_clock_ticks(bpm, ppqn);
    }

    // Destructor: close the MIDI output port
    ~MidiClockTimer() {
        if (midi_out) {
            midi_out->closePort();
            delete midi_out;
        }
    }
        // Calculate the tick duration based on BPM and PPQN
    static mus midi_clock_ticks(float bpm, int ppqn) {
        long long microseconds_per_tick = 60000000 / (ppqn * bpm);
        return microseconds(microseconds_per_tick);
    }

    // Method to start the MIDI clock
    void start_clock() {
        SendMidiStart();
        while (!stop_clock) {
            // Get the time now
            chrono::steady_clock::time_point start_time = chrono::steady_clock::now();

            // Sleep for 1 tick
            this_thread::sleep_for(tick_duration);
            sendMidiClockPulse();
            if (stop_clock) {
                break; // Break the loop if stop clock is set to true
            }

            // Get the time after sleeping
            chrono::steady_clock::time_point end_time = chrono::steady_clock::now();

            // Calculate elapsed time
            long long elapsed_time = duration_cast<microseconds>(end_time - start_time).count();

            // Check if we really slept for 1 tick; if not, sleep for the remaining time
            long long remaining_time = tick_duration.count() - elapsed_time;
            cout << "tick" << endl;

            if (remaining_time > 0) {
                this_thread::sleep_for(microseconds(remaining_time));
                cout << "clock fixed" << endl;
            }
        }
        //Send stop after loop ends
            SendMidiStop();

    }

    // Set a new BPM and recalculate the tick duration
    void setBPM(float newBPM) {
        bpm = newBPM;
        tick_duration = midi_clock_ticks(bpm, ppqn);
    }

    // Set a new PPQN and recalculate the tick duration
    void setPPQN(int newPPQN) {
        ppqn = newPPQN;
        tick_duration = midi_clock_ticks(bpm, ppqn);
    }

    // Method to send a MIDI clock pulse (0xF8)
    void sendMidiClockPulse() {
        if (midi_out) {
            vector<unsigned char> message;
            message.push_back(0xF8);  // MIDI Clock Pulse message (0xF8)
            midi_out->sendMessage(&message);  // Send the message
        }
    }

    // Send MIDI Start message if the clock isn't already running
    void SendMidiStart() {
        if (!isRunning) {
            vector<unsigned char> message;
            message.push_back(0xFA);  // MIDI Start Message (0xFA)
            midi_out->sendMessage(&message);
            cout << "MIDI Start sent" << endl;
            isRunning = true;
        }
    }

    // Send MIDI Stop message and stop the clock
    void SendMidiStop() {
        if (isRunning == true) {
            vector<unsigned char> message;
            message.push_back(0xFC);  // MIDI Stop Message (0xFC)
            midi_out->sendMessage(&message);
            isRunning = false;

        }
    }
    void sendMidiContinue() {
    if (midi_out) {
        vector<unsigned char> message = { 0xFB };  // MIDI Continue message (0xFB)
        midi_out->sendMessage(&message);
        cout << "MIDI Continue sent" << endl;
    }
}

void sendMidiReset() {
    if (midi_out) {
        vector<unsigned char> message = { 0xFF };  // MIDI Reset message (0xFF)
        midi_out->sendMessage(&message);
        cout << "MIDI Reset sent" << endl;
    }
}
    void changeBPM(float newBPM) {
    setBPM(newBPM);
}

void changePPQN(int newPPQN) {
    setPPQN(newPPQN);
}




};

int main() {
    float bpm = 60;
    int ppqn = 960;
    MidiClockTimer timer(bpm, ppqn);

    timer.start_clock();

    return 0;
}
}
