#include <Color.hpp>
#include <Font.hpp>
#include <Functions.hpp>
#include <Mouse.hpp>
#include <Rectangle.hpp>
#include <Vector2.hpp>
#include <algorithm>
#include <format>
#include <functional>
#include <memory>
#include <optional>
#include <ranges>
#include <raylib.h>
#include <raylib-cpp.hpp>
#include <vector>

using std::vector;
using std::shared_ptr;
using std::string;

namespace {
  enum GameState {
    Game, Finished
  };

  enum Symbol {
    None, Cross, Circle
  };

  class RectangleCorners {
    private:
      const raylib::Vector2 m_upperLeft;
      const raylib::Vector2 m_upperRight;
      const raylib::Vector2 m_lowerLeft;
      const raylib::Vector2 m_lowerRight;
    public:
      RectangleCorners(const float x, const float y, const float x2, const float y2)
        : m_upperLeft(x, y),
          m_upperRight(x2, y),
          m_lowerLeft(x, y2),
          m_lowerRight(x2, y2)
      {

      }
      auto static withWidthAndHeight(
        const float x,
        const float y,
        const float width,
        const float height
      ) -> RectangleCorners {
        return RectangleCorners(x, y, x + width, y + height);
      }
      auto static fromRectangle(const raylib::Rectangle &rectangle) -> RectangleCorners {
        return RectangleCorners::withWidthAndHeight(
            rectangle.GetX(), 
            rectangle.GetY(), 
            rectangle.GetWidth(), 
            rectangle.GetHeight()
        );
      }
      auto upperLeft() const -> raylib::Vector2 { return this->m_upperLeft; }
      auto lowerLeft() const -> raylib::Vector2 { return this->m_lowerLeft; }
      auto upperRight() const -> raylib::Vector2 { return this->m_upperRight; }
      auto lowerRight() const -> raylib::Vector2 { return this->m_lowerRight; }
  };

  class Field {
    private:
      const raylib::Rectangle m_collider;
      const shared_ptr<Symbol> m_symbol;
      Field(
          const float x, 
          const float y, 
          const float width, 
          const float height, 
          shared_ptr<Symbol> pointer
      ) : m_collider(x, y, width, height), m_symbol(pointer) {
      } 
      void drawCross(const float lineThickness, const raylib::Color &color = { 0, 0, 0, 255 }) const {
        // TODO change so it doesn't go all the way to the corners
        auto corners = RectangleCorners::fromRectangle(this->m_collider);

        corners.upperLeft().DrawLine(corners.lowerRight(), lineThickness, color);
        corners.upperRight().DrawLine(corners.lowerLeft(), lineThickness, color);
      }
      void drawCross(const raylib::Color &color = { 0, 0, 0, 255 }) const {
        drawCross(1.0, color);
      }
      void drawCircle(const float lineThickness, const raylib::Color &color = { 0, 0, 0, 255 }) const {
        // TODO find use for lineThickness or remove
        // TODO Consider using Rectangle and DrawRoundedLines instead to get an empty circle
        // or get Background color from global state and draw a second circle over the first
        raylib::Vector2 middle = this->m_collider.GetPosition() + (this->m_collider.GetSize() / 2);

        middle.DrawCircle(30.0, color);
      }
      void drawCircle(const raylib::Color &color = { 0, 0, 0, 255}) const {
        this->drawCircle(1.0, color);
      }
    public:
      auto static fromCoordinates(const float x, const float y, const float width, const float height) -> Field {
        shared_ptr<Symbol> pointer = std::make_shared<Symbol>(Symbol::None);
        return Field(x, y, width, height, std::move(pointer));
      }
      auto symbolPointer() const -> shared_ptr<Symbol> {
        return this->m_symbol;
      }
      auto collider() const -> raylib::Rectangle {
        return this->m_collider;
      }
      void setSymbol(const Symbol symbol) {
        *(this->m_symbol) = symbol;
      }
      auto checkCollision(const raylib::Vector2 &point) const -> bool {
        return this->m_collider.CheckCollision(point);
      }
      void drawSymbol(const float lineThickness, const raylib::Color &color = { 0, 0, 0, 255 }) const {
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
      void drawSymbol(const raylib::Color &color = { 0, 0, 0, 255}) const {
        this->drawSymbol(1.0, color);
      }
  };

  class Line {
    private:
      const raylib::Vector2 m_lineStart;
      const raylib::Vector2 m_lineEnd;
    public:
      Line(const raylib::Vector2 lineStart, const raylib::Vector2 lineEnd)
        : m_lineStart(lineStart), m_lineEnd(lineEnd) 
      {

      }
      Line(const float startX, const float startY, const float endX, const float endY) 
        : m_lineStart(startX, startY), m_lineEnd(endX, endY) 
      {

      }
      void draw(const raylib::Color &color) const {
        this->m_lineStart.DrawLine(this->m_lineEnd, color);
      }
      void draw(const float lineThickness, const raylib::Color &color) const {
        this->m_lineStart.DrawLine(this->m_lineEnd, lineThickness, color);
      }
      auto lineStart() -> raylib::Vector2 const { return this->m_lineStart; }
      auto lineEnd() -> raylib::Vector2 const { return this->m_lineEnd; }
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
  
  class Game {
    private:
      const vector<vector<Field>> m_field;
      const vector<Line> m_lines;
      const vector<Evaluator> m_evaluators;
      Game(const vector<vector<Field>> field, vector<Line> lines, vector<Evaluator> evaluators)
        : m_field(field), m_lines(lines), m_evaluators(evaluators) {}
      auto static createLines(
          const float rectWidth, 
          const float rectHeight, 
          const float screenWidth, 
          const float screenHeight
      ) -> vector<Line> {
        auto m_lines = vector<Line>();

        // Lines top to bottom
        m_lines.push_back(Line(rectWidth, 0, rectWidth, screenHeight));
        m_lines.push_back(Line(rectWidth * 2, 0, rectWidth * 2, screenHeight));

        // Lines left to right
        m_lines.push_back(Line(0, rectHeight, screenWidth, rectHeight));
        m_lines.push_back(Line(0, rectHeight * 2, screenWidth, rectHeight * 2));

        return m_lines;
      }
      auto static createField(
        const float rectWidth,
        const float rectHeight
      ) -> vector<vector<Field>> {
        auto field = vector<vector<Field>>();

        for(float i = 0; i < 3; i++) {
          auto current_line = vector<Field>();
          for(float j = 0; j < 3; j++) {
            current_line.push_back(Field::fromCoordinates(
              rectWidth * i,
              rectHeight * j,
              rectWidth,
              rectHeight
            ));
          }

          field.push_back(current_line);
        }

        return field;
      }
      auto static createEvaluators(const vector<vector<Field>> &field) -> vector<Evaluator> {
        auto evaluators = vector<Evaluator>();
        evaluators.push_back(Evaluator::create(
              field[0][0].symbolPointer(), 
              field[0][1].symbolPointer(), 
              field[0][2].symbolPointer()
        ));
        return evaluators;
      }
      auto filterField(const std::function<auto (const Field&) -> bool> &f) const -> vector<Field> {
        auto filteredField = vector<Field>();

        for(const vector<Field> &line : this->m_field) {
          auto filteredLine = line | std::ranges::views::filter(f);
          filteredField.append_range(filteredLine);
        }

        return filteredField;
      }
      auto forEachInField(const std::function<void (const Field&)> &f) const {
        std::ranges::for_each(this->m_field, [&f](const vector<Field> &line) {
          std::ranges::for_each(line, f);
        });
      }
    public:
      auto static create(const float screenWidth, const float screenHeight) -> Game {
        const float rectWidth = screenWidth / 3;
        const float rectHeight = screenHeight / 3;

        auto lines = Game::createLines(rectWidth, rectHeight, screenWidth, screenHeight);
        auto field = Game::createField(rectWidth, rectHeight);
        auto evaluators = Game::createEvaluators(field);
        return Game(std::move(field), std::move(lines), std::move(evaluators));
      }
      void draw(const raylib::Color &color) const {
        std::ranges::for_each(this->m_lines, [&color](auto &line){
          line.draw(color);
        });
      }
      void draw(const float lineThickness, const raylib::Color &color) const {
        std::ranges::for_each(this->m_lines, [lineThickness, &color](auto &line){
          line.draw(lineThickness, color);
        });
      }
      auto findColliders(const raylib::Vector2 &vec) -> vector<Field> {
        return this->filterField([&vec](const Field &field){
          return field.checkCollision(vec);
        });
      }
      auto findCollider(const raylib::Vector2 &vec) -> const Field {
        return this->findColliders(vec).at(0);
      }
      void drawSymbols() const {
        this->forEachInField([](const Field &field){
          field.drawSymbol();
        });
      }
      auto getWinner() const -> Symbol {
        for(const auto &evaluator : this->m_evaluators) {
          Symbol symbol = evaluator.evaluate();
          if(symbol != Symbol::None) {
            return symbol;
          }
        }
        return Symbol::None;
      }
  };
}

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

void shuffleSymbol(shared_ptr<Symbol> symbol) {
  switch (*symbol) {
    case None:
      *symbol = Symbol::Cross;
      break;
    case Cross:
      *symbol = Symbol::Circle;
      break;
    case Circle:
      *symbol = Symbol::None;
      break;
  }
}

void drawCenteredText(
    const string &text, 
    const raylib::Vector2 &position,
    const float fontSize, 
    const float spacing,
    const raylib::Color &color
) {
  const auto font = raylib::Font();
  const raylib::Vector2 size = font.MeasureText(text, fontSize, spacing);
  auto actualPosition = position.Subtract(size.Divide(2));

  font.DrawText(text, actualPosition, fontSize, spacing, color);
}

auto main() -> int {
  constexpr int screenHeight = 600;
  constexpr int screenWidth = 800;

  raylib::Window window(
    screenWidth, 
    screenHeight, 
    "Tic Tac Toe",
    FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI
  );

  auto game = Game::create(screenWidth, screenHeight);
  auto gameState = GameState::Game;
  std::optional<Symbol> winner = std::optional<Symbol>();

  while(!window.ShouldClose()) {
    if(gameState == GameState::Game) {
      std::optional<raylib::Rectangle> rect = std::optional<raylib::Rectangle>();
      if(raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON)) {
        Field field = game.findCollider(raylib::Mouse::GetPosition());
        rect = field.collider();
        shuffleSymbol(field.symbolPointer());

        Symbol symbol = game.getWinner();
        if(symbol != Symbol::None) {
          winner = symbol;
          gameState = GameState::Finished;
        }
      }

      window.BeginDrawing();
      window.ClearBackground(raylib::Color::White());
      
      game.draw(4.0, raylib::Color::Black());
      game.drawSymbols();

      window.EndDrawing();
    } else if (gameState == GameState::Finished) {
      if(!winner.has_value()) {
        return 1;
      }

      window.BeginDrawing();
      window.ClearBackground(raylib::Color::White());

      string text = std::format("{} wins!", winner.value());
      raylib::Vector2 textPosition = raylib::Vector2(
          static_cast<float>(screenWidth) / 2, 
          static_cast<float>(screenHeight) / 2
      );
      drawCenteredText(text, textPosition, 30, 1.5, raylib::Color::Black());

      window.EndDrawing();
    }
  }

  return 0;
}
