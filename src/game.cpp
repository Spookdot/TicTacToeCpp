#include "game.hpp"

#include <algorithm>
#include <cassert>
#include <functional>
#include <iterator>
#include <ranges>
#include <utility>

using std::vector;
namespace ranges = std::ranges;

Game::Game(vector<vector<Field>> field, vector<Line> lines,
           vector<Evaluator> evaluators)
    : m_field(std::move(field)), m_lines(std::move(lines)),
      m_evaluators(std::move(evaluators)) {}

auto Game::createLines(const raylib::Rectangle &rect, const raylib::Rectangle &screen)
    -> vector<Line> {
  auto lines = vector<Line>();

  // Lines top to bottom
  lines.emplace_back(rect.width, 0, rect.width, screen.height);
  lines.emplace_back(rect.width * 2, 0, rect.width * 2, screen.height);

  // Lines left to right
  lines.emplace_back(0, rect.height, screen.width, rect.height);
  lines.emplace_back(0, rect.height * 2, screen.width, rect.height * 2);

  return lines;
}

auto Game::createField(const float rectWidth, const float rectHeight)
    -> vector<vector<Field>> {
  auto field = vector<vector<Field>>();

  for (size_t i = 0; i < 3; i++) {
    auto current_line = vector<Field>();
    for (size_t j = 0; j < 3; j++) {
      raylib::Rectangle rectangle = {
        rectWidth * static_cast<float>(i), 
        rectHeight * static_cast<float>(j),
        rectWidth, 
        rectHeight
      };
      current_line.push_back(Field::fromRectangle(rectangle));
    }

    field.push_back(current_line);
  }

  return field;
}

auto Game::createEvaluators(const vector<vector<Field>> &field)
    -> vector<Evaluator> {
  const vector<vector<Field>> fieldsToEvaluate = {
    { field.at(0).at(0), field.at(0).at(1), field.at(0).at(2) },
    { field.at(1).at(0), field.at(1).at(1), field.at(1).at(2) },
    { field.at(2).at(0), field.at(2).at(1), field.at(2).at(2) },
    { field.at(0).at(0), field.at(1).at(0), field.at(2).at(0) },
    { field.at(0).at(1), field.at(1).at(1), field.at(2).at(1) },
    { field.at(0).at(2), field.at(1).at(2), field.at(2).at(2) },
    { field.at(0).at(0), field.at(1).at(1), field.at(2).at(2) },
    { field.at(0).at(2), field.at(1).at(1), field.at(2).at(0) },
  };

  vector<Evaluator> evaluators = {};
  ranges::transform(fieldsToEvaluate, std::back_inserter(evaluators), [](const vector<Field> &fields) -> Evaluator {
      return Evaluator::create(fields.at(0).symbolPointer(), fields.at(1).symbolPointer(), fields.at(2).symbolPointer());
  });

  return evaluators;
}

auto Game::filterField(
    const std::function<auto(const Field &)->bool> &filterFunction) const
    -> vector<Field> {
  auto filteredField = vector<Field>();

  for (const vector<Field> &line : this->m_field) {
    auto filteredLine = line | ranges::views::filter(filterFunction);
    filteredField.append_range(filteredLine);
  }

  return filteredField;
}

auto Game::forEachInField(
    const std::function<void(const Field &)> &forEachFunction) const -> void {
  ranges::for_each(this->m_field,
                        [&forEachFunction](const vector<Field> &line) -> void {
                          ranges::for_each(line, forEachFunction);
                        });
}

auto Game::create(const raylib::Rectangle &screen) -> Game {
  const float rectWidth = screen.width / 3;
  const float rectHeight = screen.height / 3;
  const raylib::Rectangle rect(0, 0, rectWidth, rectHeight);

  auto lines = Game::createLines(rect, screen);
  auto field = Game::createField(rectWidth, rectHeight);
  auto evaluators = Game::createEvaluators(field);

  return {std::move(field), std::move(lines), std::move(evaluators)};
}

void Game::draw(const raylib::Color &color) const {
  ranges::for_each(this->m_lines,
                        [&color](auto &line) -> void { line.draw(color); });
}

void Game::draw(const float lineThickness, const raylib::Color &color) const {
  ranges::for_each(this->m_lines,
                        [lineThickness, &color](auto &line) -> void {
                          line.draw(lineThickness, color);
                        });
}

auto Game::findColliders(const raylib::Vector2 &vec) -> vector<Field> {
  return this->filterField(
      [&vec](const Field &field) -> bool { return field.checkCollision(vec); });
}

auto Game::findCollider(const raylib::Vector2 &vec) -> Field {
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
