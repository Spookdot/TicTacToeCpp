#include "game.hpp"

#include <algorithm>
#include <functional>
#include <ranges>
#include <utility>

Game::Game(vector<vector<Field>> field, vector<Line> lines,
           vector<Evaluator> evaluators)
    : m_field(std::move(field)), m_lines(std::move(lines)),
      m_evaluators(std::move(evaluators)) {}

auto Game::createLines(const float rectWidth, const float rectHeight,
                       const float screenWidth, const float screenHeight)
    -> vector<Line> {
  auto m_lines = vector<Line>();

  // Lines top to bottom
  m_lines.emplace_back(rectWidth, 0, rectWidth, screenHeight);
  m_lines.emplace_back(rectWidth * 2, 0, rectWidth * 2, screenHeight);

  // Lines left to right
  m_lines.emplace_back(0, rectHeight, screenWidth, rectHeight);
  m_lines.emplace_back(0, rectHeight * 2, screenWidth, rectHeight * 2);

  return m_lines;
}

auto Game::createField(const float rectWidth, const float rectHeight)
    -> vector<vector<Field>> {
  auto field = vector<vector<Field>>();

  for (size_t i = 0; i < 3; i++) {
    auto current_line = vector<Field>();
    for (size_t j = 0; j < 3; j++) {
      current_line.push_back(Field::fromCoordinates(
          rectWidth * static_cast<float>(i), rectHeight * static_cast<float>(j),
          rectWidth, rectHeight));
    }

    field.push_back(current_line);
  }

  return field;
}

auto Game::createEvaluators(const vector<vector<Field>> &field)
    -> vector<Evaluator> {
  auto evaluators = vector<Evaluator>();
  evaluators.push_back(Evaluator::create(field[0][0].symbolPointer(),
                                         field[0][1].symbolPointer(),
                                         field[0][2].symbolPointer()));
  evaluators.push_back(Evaluator::create(field[1][0].symbolPointer(),
                                         field[1][1].symbolPointer(),
                                         field[1][2].symbolPointer()));
  evaluators.push_back(Evaluator::create(field[2][0].symbolPointer(),
                                         field[2][1].symbolPointer(),
                                         field[2][2].symbolPointer()));
  evaluators.push_back(Evaluator::create(field[0][0].symbolPointer(),
                                         field[1][0].symbolPointer(),
                                         field[2][0].symbolPointer()));
  evaluators.push_back(Evaluator::create(field[0][1].symbolPointer(),
                                         field[1][1].symbolPointer(),
                                         field[2][1].symbolPointer()));
  evaluators.push_back(Evaluator::create(field[0][2].symbolPointer(),
                                         field[1][2].symbolPointer(),
                                         field[2][2].symbolPointer()));
  evaluators.push_back(Evaluator::create(field[0][0].symbolPointer(),
                                         field[1][1].symbolPointer(),
                                         field[2][2].symbolPointer()));
  evaluators.push_back(Evaluator::create(field[0][2].symbolPointer(),
                                         field[1][1].symbolPointer(),
                                         field[2][0].symbolPointer()));
  return evaluators;
}

auto Game::filterField(
    const std::function<auto(const Field &)->bool> &filterFunction) const
    -> vector<Field> {
  auto filteredField = vector<Field>();

  for (const vector<Field> &line : this->m_field) {
    auto filteredLine = line | std::ranges::views::filter(filterFunction);
    filteredField.append_range(filteredLine);
  }

  return filteredField;
}

auto Game::forEachInField(
    const std::function<void(const Field &)> &forEachFunction) const -> void {
  std::ranges::for_each(this->m_field,
                        [&forEachFunction](const vector<Field> &line) -> void {
                          std::ranges::for_each(line, forEachFunction);
                        });
}

auto Game::create(const float screenWidth, const float screenHeight) -> Game {
  const float rectWidth = screenWidth / 3;
  const float rectHeight = screenHeight / 3;

  auto lines =
      Game::createLines(rectWidth, rectHeight, screenWidth, screenHeight);
  auto field = Game::createField(rectWidth, rectHeight);
  auto evaluators = Game::createEvaluators(field);
  return {std::move(field), std::move(lines), std::move(evaluators)};
}

void Game::draw(const raylib::Color &color) const {
  std::ranges::for_each(this->m_lines,
                        [&color](auto &line) -> void { line.draw(color); });
}

void Game::draw(const float lineThickness, const raylib::Color &color) const {
  std::ranges::for_each(this->m_lines,
                        [lineThickness, &color](auto &line) -> void {
                          line.draw(lineThickness, color);
                        });
}

auto Game::findColliders(const raylib::Vector2 &vec) -> vector<Field> {
  return this->filterField(
      [&vec](const Field &field) -> bool { return field.checkCollision(vec); });
}

auto Game::findCollider(const raylib::Vector2 &vec) -> const Field {
  return this->findColliders(vec).at(0);
}

void Game::drawSymbols() const {
  this->forEachInField([](const Field &field) -> void { field.drawSymbol(); });
}

auto Game::getWinner() const -> Symbol {
  for (const auto &evaluator : this->m_evaluators) {
    Symbol symbol = evaluator.evaluate();
    if (symbol != Symbol::None) {
      return symbol;
    }
  }
  return Symbol::None;
}
