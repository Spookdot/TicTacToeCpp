#pragma once

#include <functional>

#include "extras.hpp"

class Game {
  std::vector<std::vector<Field>> m_field;
  std::vector<Line> m_lines;
  std::vector<Evaluator> m_evaluators;
  Game(std::vector<std::vector<Field>> field, std::vector<Line> lines,
       std::vector<Evaluator> evaluators);
  auto static createLines(const raylib::Rectangle&, const raylib::Rectangle&) -> std::vector<Line>;
  auto static createField(float rectWidth, float rectHeight)
      -> std::vector<std::vector<Field>>;
  auto static createEvaluators(const std::vector<std::vector<Field>> &field)
      -> std::vector<Evaluator>;
  auto filterField(
      const std::function<auto(const Field &)->bool> &filterFunction) const
      -> std::vector<Field>;
  auto forEachInField(
      const std::function<void(const Field &)> &forEachFunction) const -> void;

public:
  auto static create(const raylib::Rectangle&) -> Game;
  void draw(const raylib::Color &color) const;
  void draw(float lineThickness, const raylib::Color &color) const;
  auto findColliders(const raylib::Vector2 &vec) -> std::vector<Field>;
  auto findCollider(const raylib::Vector2 &vec) -> Field;
  void drawSymbols() const;
  auto getWinner() const -> Symbol;
};
