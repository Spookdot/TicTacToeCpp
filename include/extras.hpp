#pragma once

#include <Vector2.hpp>
#include <cstdint>
#include <format>
#include <memory>
#include <raylib-cpp.hpp>

#define DEFAULT_COLOR {0, 0, 0, 255}

enum class Symbol : std::uint8_t { None, Cross, Circle };

template <> struct std::formatter<Symbol> : std::formatter<string> {
  auto format(Symbol symbol, format_context &ctx) const {
    switch (symbol) {
      case Symbol::None:
        return formatter<string>::format("None", ctx);
      case Symbol::Cross:
        return formatter<string>::format("Cross", ctx);
      case Symbol::Circle:
        return formatter<string>::format("Circle", ctx);
      default:
        return formatter<string>::format("", ctx);
    }
  }
};

class Field {
  raylib::Rectangle m_collider;
  std::shared_ptr<Symbol> m_symbol;
  Field(const raylib::Rectangle&, std::shared_ptr<Symbol>);
  void drawCross(float lineThickness,
                 const raylib::Color &color = DEFAULT_COLOR) const;
  void drawCross(const raylib::Color &color = DEFAULT_COLOR) const;
  void drawCircle(float lineThickness,
                  const raylib::Color &color = DEFAULT_COLOR) const;
  void drawCircle(const raylib::Color &color = DEFAULT_COLOR) const;

public:
  auto static fromRectangle(const raylib::Rectangle &rectangle)
      -> Field;
  auto symbolPointer() const -> std::shared_ptr<Symbol>;
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
  auto lineStart() const -> const raylib::Vector2&;
  auto lineEnd() const -> const raylib::Vector2&;
};

class Evaluator {
  std::vector<std::shared_ptr<Symbol>> m_line;
  explicit Evaluator(std::vector<std::shared_ptr<Symbol>> &&line);

public:
  auto static create(std::shared_ptr<Symbol> first,
                     std::shared_ptr<Symbol> second,
                     std::shared_ptr<Symbol> third) -> Evaluator;
  auto evaluate() const -> Symbol;
};
