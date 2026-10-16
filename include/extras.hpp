#pragma once

#include <cstdint>
#include <format>
#include <memory>
#include <raylib-cpp.hpp>

#define DEFAULT_COLOR {0, 0, 0, 255}

using std::shared_ptr;
using std::vector;

enum Symbol : std::uint8_t { None, Cross, Circle };

template <> struct std::formatter<Symbol> : std::formatter<string> {
  auto format(Symbol symbol, format_context &ctx) const {
    switch (symbol) {
    case None:
      return formatter<string>::format("None", ctx);
    case Cross:
      return formatter<string>::format("Cross", ctx);
    case Circle:
      return formatter<string>::format("Circle", ctx);
    default:
      return formatter<string>::format("", ctx);
    }
  }
};

class RectangleCorners {
  raylib::Vector2 m_upperLeft;
  raylib::Vector2 m_upperRight;
  raylib::Vector2 m_lowerLeft;
  raylib::Vector2 m_lowerRight;

public:
  RectangleCorners(float x, float y, float x2, float y2);
  auto static withWidthAndHeight(float x, float y, float width, float height)
      -> RectangleCorners;
  auto static fromRectangle(const raylib::Rectangle &rectangle)
      -> RectangleCorners;
  auto upperLeft() const -> raylib::Vector2;
  auto lowerLeft() const -> raylib::Vector2;
  auto upperRight() const -> raylib::Vector2;
  auto lowerRight() const -> raylib::Vector2;
};

class Field {
  raylib::Rectangle m_collider;
  shared_ptr<Symbol> m_symbol;
  Field(float x, float y, float width, float height,
        shared_ptr<Symbol> pointer);
  void drawCross(float lineThickness,
                 const raylib::Color &color = DEFAULT_COLOR) const;
  void drawCross(const raylib::Color &color = DEFAULT_COLOR) const;
  void drawCircle(float lineThickness,
                  const raylib::Color &color = DEFAULT_COLOR) const;
  void drawCircle(const raylib::Color &color = DEFAULT_COLOR) const;

public:
  auto static fromCoordinates(float x, float y, float width, float height)
      -> Field;
  auto symbolPointer() const -> shared_ptr<Symbol>;
  auto collider() const -> raylib::Rectangle;
  void setSymbol(Symbol symbol);
  auto checkCollision(const raylib::Vector2 &point) const -> bool;
  void drawSymbol(float lineThickness,
                  const raylib::Color &color = DEFAULT_COLOR) const;
  void drawSymbol(const raylib::Color &color = DEFAULT_COLOR) const;
};

class Line {
  raylib::Vector2 m_lineStart;
  raylib::Vector2 m_lineEnd;

public:
  Line(raylib::Vector2 lineStart, raylib::Vector2 lineEnd);
  Line(float startX, float startY, float endX, float endY);
  void draw(const raylib::Color &color) const;
  void draw(float lineThickness, const raylib::Color &color) const;
  auto lineStart() -> raylib::Vector2 const;
  auto lineEnd() -> raylib::Vector2 const;
};

class Evaluator {
  vector<shared_ptr<Symbol>> m_line;
  explicit Evaluator(const vector<shared_ptr<Symbol>> &line);

public:
  auto static create(const shared_ptr<Symbol> &first,
                     const shared_ptr<Symbol> &second,
                     const shared_ptr<Symbol> &third) -> Evaluator;
  auto evaluate() const -> Symbol;
};
