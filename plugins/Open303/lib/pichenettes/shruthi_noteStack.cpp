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

#include "shruthi_noteStack.hpp"

#include <string.h>

namespace shruthi
{

  static const int kFreeSlot = 0xff;

  void NoteStack::NoteOn(int note, int velocity)
  {
    // Remove the note from the list first (in case it is already here).
    NoteOff(note);
    // In case of saturation, remove the least recently played note from the
    // stack.
    if (size_ == kNoteStackSize)
    {
      int least_recent_note;
      for (int i = 1; i <= kNoteStackSize; ++i)
      {
        if (pool_[i].next_ptr == 0)
        {
          least_recent_note = pool_[i].note;
        }
      }
      NoteOff(least_recent_note);
    }
    // Now we are ready to insert the new note. Find a free slot to insert it.
    int free_slot;
    for (int i = 1; i <= kNoteStackSize; ++i)
    {
      if (pool_[i].note == kFreeSlot)
      {
        free_slot = i;
      }
    }
    pool_[free_slot].next_ptr = root_ptr_;
    pool_[free_slot].note = note;
    pool_[free_slot].velocity = velocity;
    root_ptr_ = free_slot;
    // The last step consists in inserting the note in the sorted list.
    for (int i = 0; i < size_; ++i)
    {
      if (pool_[sorted_ptr_[i]].note > note)
      {
        for (int j = size_; j > i; --j)
        {
          sorted_ptr_[j] = sorted_ptr_[j - 1];
        }
        sorted_ptr_[i] = free_slot;
        free_slot = 0;
        break;
      }
    }
    if (free_slot)
    {
      sorted_ptr_[size_] = free_slot;
    }
    ++size_;
  }

  void NoteStack::NoteOff(int note)
  {
    int current = root_ptr_;
    int previous = 0;
    while (current)
    {
      if (pool_[current].note == note)
      {
        break;
      }
      previous = current;
      current = pool_[current].next_ptr;
    }
    if (current)
    {
      if (previous)
      {
        pool_[previous].next_ptr = pool_[current].next_ptr;
      }
      else
      {
        root_ptr_ = pool_[current].next_ptr;
      }
      for (int i = 0; i < size_; ++i)
      {
        if (sorted_ptr_[i] == current)
        {
          for (int j = i; j < size_ - 1; ++j)
          {
            sorted_ptr_[j] = sorted_ptr_[j + 1];
          }
          break;
        }
      }
      pool_[current].next_ptr = 0;
      pool_[current].note = kFreeSlot;
      pool_[current].velocity = 0;
      --size_;
    }
  }

  void NoteStack::Clear()
  {
    size_ = 0;
    memset(pool_ + 1, 0, sizeof(NoteEntry) * kNoteStackSize);
    memset(sorted_ptr_ + 1, 0, kNoteStackSize);
    root_ptr_ = 0;
    for (int i = 0; i <= kNoteStackSize; ++i)
    {
      pool_[i].note = kFreeSlot;
    }
  }

} // End namespace shruthi