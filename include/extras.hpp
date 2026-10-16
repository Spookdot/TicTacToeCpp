#pragma once

#include <format>
#include <memory>
#include <raylib-cpp.hpp>

using std::shared_ptr;
using std::vector;

enum Symbol {
  None, Cross, Circle
};

template<>
struct std::formatter<Symbol> : std::formatter<string> {
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
  private:
    const raylib::Vector2 m_upperLeft;
    const raylib::Vector2 m_upperRight;
    const raylib::Vector2 m_lowerLeft;
    const raylib::Vector2 m_lowerRight;
  public:
    RectangleCorners(const float x, const float y, const float x2, const float y2);
    auto static withWidthAndHeight(const float x, const float y,
                                   const float width, const float height)
        -> RectangleCorners;
    auto static fromRectangle(const raylib::Rectangle &rectangle)
        -> RectangleCorners;
    auto upperLeft() const -> raylib::Vector2;
    auto lowerLeft() const -> raylib::Vector2;
    auto upperRight() const -> raylib::Vector2;
    auto lowerRight() const -> raylib::Vector2;
};

class Field {
  private:
    const raylib::Rectangle m_collider;
    const shared_ptr<Symbol> m_symbol;
    Field(const float x, const float y, const float width, const float height,
          shared_ptr<Symbol> pointer);
    void drawCross(const float lineThickness,
                   const raylib::Color &color = {0, 0, 0, 255}) const;
    void drawCross(const raylib::Color &color = {0, 0, 0, 255}) const;
    void drawCircle(const float lineThickness,
                    const raylib::Color &color = {0, 0, 0, 255}) const;
    void drawCircle(const raylib::Color &color = {0, 0, 0, 255}) const;

  public:
    auto static fromCoordinates(const float x, const float y, const float width,
                                const float height) -> Field;
    auto symbolPointer() const -> shared_ptr<Symbol>;
    auto collider() const -> raylib::Rectangle;
    void setSymbol(const Symbol symbol);
    auto checkCollision(const raylib::Vector2 &point) const -> bool;
    void drawSymbol(const float lineThickness,
                    const raylib::Color &color = {0, 0, 0, 255}) const;
    void drawSymbol(const raylib::Color &color = {0, 0, 0, 255}) const;
};

class Line {
  private:
    const raylib::Vector2 m_lineStart;
    const raylib::Vector2 m_lineEnd;
  public:
    Line(const raylib::Vector2 lineStart, const raylib::Vector2 lineEnd);
    Line(const float startX, const float startY, const float endX,
         const float endY);
    void draw(const raylib::Color &color) const;
    void draw(const float lineThickness, const raylib::Color &color) const;
    auto lineStart() -> raylib::Vector2 const;
    auto lineEnd() -> raylib::Vector2 const;
};

class Evaluator {
  private:
    const vector<shared_ptr<Symbol>> m_line;
    Evaluator(const vector<shared_ptr<Symbol>> line)
      : m_line(line) {}
  public:
    auto static create(
        const shared_ptr<Symbol> first,
        const shared_ptr<Symbol> second,
        const shared_ptr<Symbol> third
    ) -> Evaluator {
      const vector<shared_ptr<Symbol>> line = vector<shared_ptr<Symbol>>({ first, second, third });
      return Evaluator(std::move(line));
    }
    auto evaluate() const -> Symbol {
      bool symbolsAreSame = (*(m_line[0]) == *(m_line[1])) && (*(m_line[1]) == *(m_line[2]));
      if(symbolsAreSame) {
        return *(m_line[0]);
      }
      return Symbol::None;
    }
};
