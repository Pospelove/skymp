// Made for skymp2-server (June 2018)
// Thanks to Ivan Savelo for his help

#pragma once
#include "DSLine.h"
#include <cassert>
#include <cstdint>
#include <set>
#include <stdexcept>
#include <unordered_map>
#include <utility>

template <class T = uint64_t>
class Grid
{
public:
  void Grid::Move(const T& id, int16_t x, int16_t y)
  {
    auto& obj = objects[id];

    if (obj.coords != std::make_pair(x, y)) {
      auto to = std::make_pair(x, y);
      this->MoveImpl(id, obj.active ? &obj.coords : nullptr, &to);

      obj.active = true;
      obj.coords = { x, y };
    }
  }

  std::pair<int16_t, int16_t> Grid::GetPos(const T& id) const
  {
    if (objects[id].active)
      return objects[id].coords;
    throw std::logic_error("grid: id not found");
  }

  void Grid::Forget(const T& id)
  {
    auto& obj = objects[id];

    if (obj.active) {
      obj.active = false;
      this->MoveImpl(id, &obj.coords, nullptr);
      objects.erase(id);
    }
  }

  const std::set<T>& Grid::GetNeighbours(int16_t x, int16_t y) const
  {
    return nei[x][y];
  }

  const std::set<T>& Grid::GetNeighboursAndMe(const T& id) const
  {
    auto& pos = objects[id].coords;
    return GetNeighbours(pos.first, pos.second);
  }

  std::set<T> GetNeighbours(const T& id)
  {
    auto res = GetNeighboursAndMe(id);
    auto n = res.erase(id);
    assert(n == 1);
    return res;
  }

private:
  struct Obj
  {
    bool active = 0;
    std::pair<int16_t, int16_t> coords = { -32000, -32000 };
  };

  void Grid::MoveImpl(const T& id, std::pair<int16_t, int16_t>* from,
                      std::pair<int16_t, int16_t>* to)
  {
    auto& obj = objects[id];

    if (from) {
      for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
          nei[from->first + i][from->second + j].erase(id);
        }
      }
    }

    if (to) {
      for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
          nei[to->first + i][to->second + j].insert(id);
        }
      }
    }
  }

  mutable std::unordered_map<T, Obj> objects;
  mutable DSLine<DSLine<std::set<T>>> nei;

  static bool IsNeighbours(int16_t x1, int16_t y1, int16_t x2, int16_t y2)
  {
    if (x1 <= x2 + 1 && x1 >= x2 - 1 && y1 <= y2 + 1 && y1 >= y2 - 1)
      return true;
    return false;
  }
};