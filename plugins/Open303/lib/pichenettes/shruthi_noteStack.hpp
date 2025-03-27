// Copyright 2009 Emilie Gillet.
//
// Author: Emilie Gillet (emilie.o.gillet@gmail.com)
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
// -----------------------------------------------------------------------------
//
// Stack of currently pressed keys.
//
// Currently pressed keys are stored as a linked list. The linked list is used
// as a LIFO stack to allow monosynth-like behaviour. An example of such
// behaviour is:
// player presses and holds C4-> C4 is played.
// player presses and holds C5 (while holding C4) -> C5 is played.
// player presses and holds G4 (while holding C4&C5)-> G4 is played.
// player releases C5 -> G4 is played.
// player releases G4 -> C4 is played.
//
// The nodes used in the linked list are pre-allocated from a pool of 16
// nodes, so the "pointers" (to the root element for example) are not actual
// pointers, but indices of an element in the pool.
//
// Additionally, an array of pointers is stored to allow random access to the
// n-th note, sorted by ascending order of pitch (for arpeggiation).
//
// (Very) minor modifications by toneburst 2025
// Converted all uint8_t vars to int

#ifndef shruthi_noteStack_h
#define shruthi_noteStack_h

static const int kNoteStackSize = 8;

namespace shruthi
{
  struct NoteEntry
  {
    int note;
    int velocity;
    int next_ptr; // Base 1.
  };

  class NoteStack
  {
  public:
    NoteStack() {}

    void Init() { Clear(); }

    void NoteOn(int note, int velocity);
    void NoteOff(int note);
    void Clear();

    int size() { return size_; }
    int max_size() { return kNoteStackSize; }
    const NoteEntry &most_recent_note() { return pool_[root_ptr_]; }
    const NoteEntry &least_recent_note()
    {
      int current = root_ptr_;
      while (current && pool_[current].next_ptr)
      {
        current = pool_[current].next_ptr;
      }
      return pool_[current];
    }
    const NoteEntry &sorted_note(int index)
    {
      return pool_[sorted_ptr_[index]];
    }
    const NoteEntry &played_note(int index) const
    {
      int current = root_ptr_;
      index = size_ - index - 1;
      for (int i = 0; i < index; ++i)
      {
        current = pool_[current].next_ptr;
      }
      return pool_[current];
    }
    const NoteEntry &note(int index) { return pool_[index]; }
    NoteEntry *mutable_note(int index) { return &pool_[index]; }
    const NoteEntry &dummy() { return pool_[0]; }

  private:
    int size_;
    NoteEntry pool_[kNoteStackSize + 1];     // First element is a dummy node!
    int root_ptr_;                       // Base 1.
    int sorted_ptr_[kNoteStackSize + 1]; // Base 1.
  };

} // End namespace shruthi

#endif // shruthi_noteStack_h
