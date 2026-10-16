#pragma once

#include <functional>

#include "extras.hpp"

class Game {
  vector<vector<Field>> m_field;
  vector<Line> m_lines;
  vector<Evaluator> m_evaluators;
  Game(vector<vector<Field>> field, vector<Line> lines,
       vector<Evaluator> evaluators);
  auto static createLines(float rectWidth, float rectHeight, float screenWidth,
                          float screenHeight) -> vector<Line>;
  auto static createField(float rectWidth, float rectHeight)
      -> vector<vector<Field>>;
  auto static createEvaluators(const vector<vector<Field>> &field)
      -> vector<Evaluator>;
  auto filterField(
      const std::function<auto(const Field &)->bool> &filterFunction) const
      -> vector<Field>;
  auto forEachInField(
      const std::function<void(const Field &)> &forEachFunction) const -> void;

public:
  auto static create(float screenWidth, float screenHeight) -> Game;
  void draw(const raylib::Color &color) const;
  void draw(float lineThickness, const raylib::Color &color) const;
  auto findColliders(const raylib::Vector2 &vec) -> vector<Field>;
  auto findCollider(const raylib::Vector2 &vec) -> const Field;
  void drawSymbols() const;
  auto getWinner() const -> Symbol;
};
