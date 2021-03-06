#ifndef __PROBLEM_TWOPLAYER_H
#define __PROBLEM_TWOPLAYER_H

#include "pftwo.h"

#include <mutex>
#include <atomic>

#include "n-markov-controller.h"
#include "../fceulib/emulator.h"
#include "weighted-objectives.h"

struct TwoPlayerProblem {
  // Player 1 and Player 2 controllers.
  using Input = uint16;
  using ControllerHistory = NMarkovController::History;
  
  static inline uint8 Player1(Input i) { return (i >> 8) & 255; }
  static inline uint8 Player2(Input i) { return i & 255; }
  static inline Input MakeInput(uint8 p1, uint8 p2) {
    return ((uint16)p1 << 8) | (uint16)p2;
  }
  
  // Save state; these are portable between workers.
  struct State {
    vector<uint8> save;
    // PERF This is actually part of save. But we use it to
    // compute objective functions without having to restore
    // the save.
    // PERF We could have a TPP-level remapping of indices used
    // in the objective functions to a dense sequence of integers,
    // which would usually be much fewer than 2048, and only
    // store the memory values keyed by those denser indices here.
    vector<uint8> mem;
    int depth;
    ControllerHistory prev1, prev2;
  };

  static int64 StateBytes(const State &s) {
    return s.save.size() + s.mem.size() + sizeof (State);
  }

  // Generator itself is not thread-safe, but you can create
  // many of them easily.
  struct InputGenerator {
    enum class Type {
      MARKOV,
      JUMP_LEFT,
    };
    Type type;
    int counter;
    const TwoPlayerProblem *tpp;
    ControllerHistory prev1, prev2;
    Input RandomInput(ArcFour *rc);
  };
  
  void SaveSolution(const string &filename_part,
		    const vector<Input> &inputs,
		    const State &state,
		    const string &info);
  
  // An individual instance of the emulator that can be used to
  // execute steps. We create one of these per thread.
  struct Worker {
    explicit Worker(TwoPlayerProblem *parent) : tpp(parent) {}
    // Same game is loaded as parent Problem.
    unique_ptr<Emulator> emu;
    int depth = 0;
    // Previous input for the two players.
    ControllerHistory previous1, previous2;
    
    // Sample a random input; may depend on the current state (for
    // example, in Markov models). Doesn't execute the input.
    Input RandomInput(ArcFour *rc);

    // Make a new input generator at the current state, which allows
    // generating multiple sequential random inputs without executing
    // them. Intended to be efficient to create.
    InputGenerator Generator(ArcFour *rc) {
      if (true || rc->Byte() < 220) {
	return InputGenerator{InputGenerator::Type::MARKOV, 0, tpp,
	                      previous1, previous2};
      } else {
	return InputGenerator{InputGenerator::Type::JUMP_LEFT, 0, tpp,
	                      previous1, previous2};
      }
    }
    
    // Called in the worker thread before anything else, but
    // the worker's state must be valid before this point.
    void Init();
    
    State Save() {
      MutexLock ml(&mutex);
      return State{ emu->SaveUncompressed(), emu->GetMemory(), depth,
                    previous1, previous2 };
    }

    void Restore(const State &state) {
      MutexLock ml(&mutex);
      emu->LoadUncompressed(state.save);
      depth = state.depth;
      // (Doesn't actually need the memory to restore.)
      previous1 = state.prev1;
      previous2 = state.prev2;
    }
    void Exec(Input input) {
      MutexLock ml(&mutex);
      const uint8 input1 = Player1(input), input2 = Player2(input);
      emu->Step(input1, input2);
      IncrementNESFrames(1);
      depth++;
      // Here it would be better if we could just use a template
      // function (nmarkov model templated over n) without referencing
      // the parent.
      previous1 = tpp->markov1->Push(previous1, input1);
      previous2 = tpp->markov2->Push(previous2, input2);
    }

    void Observe();

    void Visualize(vector<uint8> *argb256x256);
    void VizText(vector<string> *text);
    
    void ClearStatus() {
      SetStatus(nullptr);
      SetNumer(0);
      SetDenom(0);
    }
    inline void SetStatus(const char *s) {
      status.store(s, std::memory_order_relaxed);
    }
    inline void SetNumer(int n) {
      numer.store(n, std::memory_order_relaxed);
    }
    inline void SetDenom(int d) {
      denom.store(d, std::memory_order_relaxed);
    }
    inline void IncrementNESFrames(int d) {
      nes_frames.fetch_add(d, std::memory_order_relaxed);
    }
    
    // Current operation. Should point to a string literal;
    // other code may hang on to references.
    std::atomic<const char *> status{nullptr};
    // Fraction complete.
    std::atomic<int> numer{0}, denom{0};
    // For benchmarking, the total number of NES frames (or
    // equivalent) executed by this worker. This isn't
    // all the program does, but it is the main bottleneck,
    // so we want to make sure we aren't stalling them.
    std::atomic<int> nes_frames{0};
    
    const TwoPlayerProblem *tpp = nullptr;

    // Lock emulator etc.
    std::mutex mutex;
  };

  // Commits observations.
  void Commit() {
    CHECK(observations.get());
    observations->Commit();
  }
  // Score in [0, 1]. Should be stable in-between calls to
  // Commit.
  double Score(const State &state) {
    // Give a tiny penalty to longer paths to the same value, so
    // that we prefer to optimize these away.
    // This is only 4.6 hours of game time, but this is a dirty hack
    // anyway. XXX FIX HACK!
    // I made it one billion, because a millionth was actually causing
    // problems with lack of progress (I think).

    return observations->GetWeightedValue(state.mem);
    /*
    double depth_penalty = 1.0 - (state.depth * (1.0 / 1000000000.0));
    if (depth_penalty > 1.0) depth_penalty = 1.0;
    else if (depth_penalty < 0.0) depth_penalty = 0.0;
    return observations->GetWeightedValue(state.mem) *
      depth_penalty;
    */
  }
  
  // Must be thread safe and leave Worker in a valid state.
  Worker *CreateWorker();

  TwoPlayerProblem(const map<string, string> &config);
 
  string game;
  int warmup_frames = -1;
  vector<pair<uint8, uint8>> original_inputs;
  unique_ptr<NMarkovController> markov1, markov2;
  // After warmup inputs.
  State start_state;
  // For play after warmup.
  unique_ptr<WeightedObjectives> objectives;
  unique_ptr<Observations> observations;
};


#endif
