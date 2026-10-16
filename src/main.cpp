#include <cstdint>
#include <format>
#include <memory>
#include <optional>
#include <raylib-cpp.hpp>
#include <raylib.h>

#include "extras.hpp"
#include "game.hpp"

using std::shared_ptr;
using std::string;

namespace {
enum GameState : std::uint8_t { Start, Game, Finished };

void shuffleSymbol(const shared_ptr<Symbol> &symbol) {
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

void drawCenteredText(const string &text, const raylib::Vector2 &position,
                      const float fontSize, const float spacing,
                      const raylib::Color &color) {
  const auto font = raylib::Font();
  const raylib::Vector2 size = font.MeasureText(text, fontSize, spacing);
  auto actualPosition = position.Subtract(size.Divide(2));

  font.DrawText(text, actualPosition, fontSize, spacing, color);
}

auto createCenteredRectangle(const float x, const float y, const float width,
                             const float height) -> raylib::Rectangle {
  const float actualX = x - (width / 2);
  const float actualY = y - (height / 2);
  return {actualX, actualY, width, height};
}
} // namespace

auto main() -> int {
  constexpr int screenHeight = 600;
  constexpr int screenWidth = 800;
  const auto screenHeightFloat = static_cast<float>(screenHeight);
  const auto screenWidthFloat = static_cast<float>(screenWidth);

  raylib::Window window(screenWidth, screenHeight, "Tic Tac Toe",
                        FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

  // General data
  auto gameState = GameState::Start;
  std::optional<Symbol> winner = std::optional<Symbol>();

  // GameState::Start data
  raylib::Rectangle startButton = createCenteredRectangle(
      screenWidthFloat / 2, screenHeightFloat / 3 * 2, 100, 50);

  // GameState::Game data
  auto game = Game::create(screenWidth, screenHeight);

  while (!window.ShouldClose()) {
    if (gameState == GameState::Start) {
      if (raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON)) {
        bool startButtonPressed =
            startButton.CheckCollision(raylib::Mouse::GetPosition());
        if (startButtonPressed) {
          gameState = GameState::Game;
        }
      }

      window.BeginDrawing();
      window.ClearBackground(raylib::Color::White());

      raylib::Vector2 textPosition =
          raylib::Vector2(screenWidthFloat / 2, screenHeightFloat / 3);
      drawCenteredText("Tic Tac Toe", textPosition, 50, 1.5,
                       raylib::Color::Black());

      startButton.DrawLines(raylib::Color::Black(), 2.0);

      window.EndDrawing();
    } else if (gameState == GameState::Game) {
      std::optional<raylib::Rectangle> rect =
          std::optional<raylib::Rectangle>();
      if (raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON)) {
        Field field = game.findCollider(raylib::Mouse::GetPosition());
        rect = field.collider();
        shuffleSymbol(field.symbolPointer());

        Symbol symbol = game.getWinner();
        if (symbol != Symbol::None) {
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
      if (!winner.has_value()) {
        return 1;
      }

      window.BeginDrawing();
      window.ClearBackground(raylib::Color::White());

      string text = std::format("{} wins!", winner.value());
      raylib::Vector2 textPosition =
          raylib::Vector2(screenWidthFloat / 2, screenHeightFloat / 2);
      drawCenteredText(text, textPosition, 30, 1.5, raylib::Color::Black());

      window.EndDrawing();
    }
  }

  return 0;
}
