#include <algorithm>
#include <bitset>
#include <iostream>
#include <set>
#include <vector>

using namespace std;

const int kNumSeg = 7;
typedef bitset<kNumSeg> Seg;
const int kNumDigits = 10;
Seg kDigitSegs[kNumDigits] = {
  Seg("1111110"),  // 0
  Seg("0110000"),  // 1
  Seg("1101101"),  // 2
  Seg("1111001"),  // 3
  Seg("0110011"),  // 4
  Seg("1011011"),  // 5
  Seg("1011111"),  // 6
  Seg("1110000"),  // 7
  Seg("1111111"),  // 8
  Seg("1111011"),  // 9
};

int NextDigit(int digit, int step = 1) {
  return (digit + kNumDigits - (step % kNumDigits)) % kNumDigits;
}

int main(int argc, char* argv[]) {
  int T = 0;
  cin >> T;
  for (int t = 0; t < T; ++t) {
    int N = 0;
    cin >> N;
    vector<Seg> states;
    Seg mask;  // records good segments
    for (int n = 0; n < N; ++n) {
      Seg seg;
      cin >> seg;
      states.push_back(seg);
      mask |= seg;
    }
    set<int> first_digits;  // possible first digit guesses
    for (int i = 0; i < kNumDigits; ++i) {
      first_digits.insert(i);
    }
    for (int n = 0; n < N; ++n) {  // tests first digit guesses
      for (set<int>::iterator iter = first_digits.begin();
           iter != first_digits.end(); ) {
        int guess_digit = NextDigit(*iter, n);
        bool guess_matches_state =
            (kDigitSegs[guess_digit] & mask) == states[n];
        if (!guess_matches_state) {
          first_digits.erase(iter++);
        } else {
          ++iter;
        }
      }
    }
    Seg bad_masks[kNumDigits];  // stores bad segments
    for (set<int>::iterator iter = first_digits.begin();
         iter != first_digits.end(); ) {  // checks bad segments consistency
      Seg& bad_mask = bad_masks[*iter];
      for (int n = 0; n < N; ++n) {
        bad_mask |= (kDigitSegs[NextDigit(*iter, n)] ^ states[n]);
      }
      bool bad_segment_lights = std::any_of(
          states.begin(), states.end(), [&bad_mask](const Seg& state) {
            return (state & bad_mask).any();
          });
      if (bad_segment_lights) {
        first_digits.erase(iter++);
      } else {
        ++iter;
      }
    }
    set<unsigned long> next_displays;  // checks next-display results uniqueness
    for (auto digit : first_digits) {
      next_displays.insert((kDigitSegs[NextDigit(digit, N)] & mask).to_ulong());
      next_displays.insert(
          (kDigitSegs[NextDigit(digit, N)] & ~bad_masks[digit]).to_ulong());
    }
    cout << "Case #" << (t+1) << ": ";
    if (next_displays.size() != 1) {  // print result
      cout << "ERROR!";
    } else {
      cout << Seg(*next_displays.begin());
    }
    cout << endl;
  }
  return 0;
}
