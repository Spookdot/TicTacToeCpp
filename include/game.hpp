#pragma once

#include <functional>

#include "extras.hpp"

class Game {
  private:
    const vector<vector<Field>> m_field;
    const vector<Line> m_lines;
    const vector<Evaluator> m_evaluators;
    Game(const vector<vector<Field>> field, vector<Line> lines, vector<Evaluator> evaluators);
    auto static createLines(
        const float rectWidth, 
        const float rectHeight, 
        const float screenWidth, 
        const float screenHeight
    ) -> vector<Line>;
    auto static createField(
      const float rectWidth,
      const float rectHeight
    ) -> vector<vector<Field>>;
    auto static createEvaluators(const vector<vector<Field>> &field) -> vector<Evaluator>;
    auto filterField(const std::function<auto(const Field &)->bool> &f) const
        -> vector<Field>;
    auto forEachInField(const std::function<void(const Field &)> &f) const;

  public:
    auto static create(const float screenWidth, const float screenHeight)
        -> Game;
    void draw(const raylib::Color &color) const;
    void draw(const float lineThickness, const raylib::Color &color) const;
    auto findColliders(const raylib::Vector2 &vec) -> vector<Field>;
    auto findCollider(const raylib::Vector2 &vec) -> const Field;
    void drawSymbols() const;
    auto getWinner() const -> Symbol;
};
