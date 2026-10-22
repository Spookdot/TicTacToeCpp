#include "extras.hpp"

#include <Vector2.hpp>
#include <Rectangle.hpp>
#include <Color.hpp>
#include <vector>
#include <memory>
#include <utility>

using std::shared_ptr;
using std::vector;

constexpr float CIRCLE_ROUNDNESS = 1.0F;
constexpr unsigned int CIRCLE_SEGMENTS = 5;

namespace {
void centerRectangleInPlace(raylib::Rectangle &rect, const raylib::Rectangle &area) {
  const raylib::Vector2 position = area.GetPosition() + ((area.GetSize() - rect.GetSize()) / 2);
  rect.SetPosition(position);
}
} // namespace

Field::Field(const raylib::Rectangle &rectangle, std::shared_ptr<Symbol> pointer)
    : m_collider(rectangle), m_symbol(std::move(pointer)) {}

void Field::drawCross(const float lineThickness,
                      const raylib::Color &color) const {
  // TODO change so it doesn't go all the way to the corners
  const raylib::Vector2 upperLeft = this->m_collider.GetPosition();
  const raylib::Vector2 upperRight(upperLeft.x + this->m_collider.width, upperLeft.y);
  const raylib::Vector2 lowerLeft(upperLeft.x, upperLeft.y + this->m_collider.height);
  const raylib::Vector2 lowerRight = upperLeft + this->m_collider.GetSize();

  upperLeft.DrawLine(lowerRight, lineThickness, color);
  upperRight.DrawLine(lowerLeft, lineThickness, color);
}

void Field::drawCross(const raylib::Color &color) const {
  drawCross(1.0, color);
}

void Field::drawCircle(const float lineThickness,
                       const raylib::Color &color) const {
  // TODO find use for lineThickness or remove
  // TODO Consider using Rectangle and DrawRoundedLines instead to get an empty
  // circle or get Background color from global state and draw a second circle
  // over the first

  const float resizeFactor = 0.8F;
  // Set both to height to create a square
  raylib::Rectangle circle = {};
  circle.SetSize(
      this->m_collider.height * resizeFactor, 
      this->m_collider.height * resizeFactor
  );
  centerRectangleInPlace(circle, this->m_collider);

  circle.DrawRoundedLines(CIRCLE_ROUNDNESS, CIRCLE_SEGMENTS, lineThickness, color);
}

void Field::drawCircle(const raylib::Color &color) const {
  this->drawCircle(1.0, color);
}

auto Field::fromRectangle(const raylib::Rectangle &rectangle) -> Field {
  shared_ptr<Symbol> pointer = std::make_shared<Symbol>(Symbol::None);
  return {rectangle, std::move(pointer)};
}

auto Field::symbolPointer() const -> shared_ptr<Symbol> {
  return this->m_symbol;
}

auto Field::collider() const -> raylib::Rectangle { return this->m_collider; }

void Field::setSymbol(const Symbol symbol) { *this->m_symbol = symbol; }

auto Field::checkCollision(const raylib::Vector2 &point) const -> bool {
  return this->m_collider.CheckCollision(point);
}

void Field::drawSymbol(const float lineThickness,
                       const raylib::Color &color) const {
  switch (*this->m_symbol) {
    case Symbol::Cross:
      this->drawCross(lineThickness, color);
      break;
    case Symbol::Circle:
      this->drawCircle(lineThickness, color);
      break;
    case Symbol::None:
      break;
  }
}

void Field::drawSymbol(const raylib::Color &color) const {
  this->drawSymbol(1.0, color);
}

void Line::draw(const raylib::Color &color) const {
  this->lineStart.DrawLine(this->lineEnd, color);
}

void Line::draw(const float lineThickness, const raylib::Color &color) const {
  this->lineStart.DrawLine(this->lineEnd, lineThickness, color);
}

Evaluator::Evaluator(vector<shared_ptr<Symbol>> &&line) : m_line(std::move(line)) {}
auto Evaluator::create(shared_ptr<Symbol> first,
                       shared_ptr<Symbol> second,
                       shared_ptr<Symbol> third) -> Evaluator {
  return Evaluator({std::move(first), std::move(second), std::move(third)});
}
auto Evaluator::evaluate() const -> Symbol {
  const bool symbolsAreSame =
      (*m_line.at(0) == *m_line.at(1)) && (*m_line.at(1) == *m_line.at(2));
  if (symbolsAreSame) {
    return *m_line.at(0);
  }
  return Symbol::None;
}
