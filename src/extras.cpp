#include "extras.hpp"

#include <memory>
#include <raylib-cpp.hpp>

RectangleCorners::RectangleCorners(const float x, const float y, const float x2,
                                   const float y2)
    : m_upperLeft(x, y), m_upperRight(x2, y), m_lowerLeft(x, y2),
      m_lowerRight(x2, y2) {}

auto RectangleCorners::withWidthAndHeight(const float x, const float y,
                                          const float width, const float height)
    -> RectangleCorners {
  return RectangleCorners(x, y, x + width, y + height);
}

auto RectangleCorners::fromRectangle(const raylib::Rectangle &rectangle)
    -> RectangleCorners {
  return RectangleCorners::withWidthAndHeight(
      rectangle.GetX(), rectangle.GetY(), rectangle.GetWidth(),
      rectangle.GetHeight());
}

auto RectangleCorners::upperLeft() const -> raylib::Vector2 {
  return this->m_upperLeft;
}

auto RectangleCorners::lowerLeft() const -> raylib::Vector2 {
  return this->m_lowerLeft;
}

auto RectangleCorners::upperRight() const -> raylib::Vector2 {
  return this->m_upperRight;
}

auto RectangleCorners::lowerRight() const -> raylib::Vector2 {
  return this->m_lowerRight;
}


Field::Field(const float x, const float y, const float width,
             const float height, shared_ptr<Symbol> pointer)
    : m_collider(x, y, width, height), m_symbol(pointer) {}

void Field::drawCross(const float lineThickness,
                      const raylib::Color &color) const {
  // TODO change so it doesn't go all the way to the corners
  auto corners = RectangleCorners::fromRectangle(this->m_collider);

  corners.upperLeft().DrawLine(corners.lowerRight(), lineThickness, color);
  corners.upperRight().DrawLine(corners.lowerLeft(), lineThickness, color);
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
  raylib::Vector2 middle =
      this->m_collider.GetPosition() + (this->m_collider.GetSize() / 2);

  middle.DrawCircle(30.0, color);
}

void Field::drawCircle(const raylib::Color &color) const {
  this->drawCircle(1.0, color);
}

auto Field::fromCoordinates(const float x, const float y, const float width,
                            const float height) -> Field {
  shared_ptr<Symbol> pointer = std::make_shared<Symbol>(Symbol::None);
  return Field(x, y, width, height, std::move(pointer));
}

auto Field::symbolPointer() const -> shared_ptr<Symbol> {
  return this->m_symbol;
}

auto Field::collider() const -> raylib::Rectangle { return this->m_collider; }

void Field::setSymbol(const Symbol symbol) { *(this->m_symbol) = symbol; }

auto Field::checkCollision(const raylib::Vector2 &point) const -> bool {
  return this->m_collider.CheckCollision(point);
}

void Field::drawSymbol(const float lineThickness,
                       const raylib::Color &color) const {
  switch (*(this->m_symbol)) {
  case Cross:
    this->drawCross(lineThickness, color);
    break;
  case Circle:
    this->drawCircle(lineThickness, color);
    break;
  case None:
    break;
  }
}

void Field::drawSymbol(const raylib::Color &color) const {
  this->drawSymbol(1.0, color);
}


Line::Line(const raylib::Vector2 lineStart, const raylib::Vector2 lineEnd)
    : m_lineStart(lineStart), m_lineEnd(lineEnd) {}

Line::Line(const float startX, const float startY, const float endX,
           const float endY)
    : m_lineStart(startX, startY), m_lineEnd(endX, endY) {}

void Line::draw(const raylib::Color &color) const {
  this->m_lineStart.DrawLine(this->m_lineEnd, color);
}

void Line::draw(const float lineThickness, const raylib::Color &color) const {
  this->m_lineStart.DrawLine(this->m_lineEnd, lineThickness, color);
}

auto Line::lineStart() -> raylib::Vector2 const { return this->m_lineStart; }

auto Line::lineEnd() -> raylib::Vector2 const { return this->m_lineEnd; }
