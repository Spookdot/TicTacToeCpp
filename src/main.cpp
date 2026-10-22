#include <Mouse.hpp>
#include <Window.hpp>
#include <Font.hpp>
#include <Vector2.hpp>
#include <Rectangle.hpp>
#include <Color.hpp>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <raylib.h>

#include "extras.hpp"
#include "game.hpp"

using std::shared_ptr;
using std::string;

constexpr float LINE_THICKNESS = 4.0;
constexpr unsigned int TITLE_FONT_SIZE = 50;
constexpr unsigned int FONT_SIZE = 30;
constexpr float TEXT_SPACING = 1.5;

namespace {
enum class GameState : std::uint8_t { Start, Game, Finished };

void  shuffleSymbol(const shared_ptr<Symbol> &symbol) {
  switch (*symbol) {
    case Symbol::None:
      *symbol = Symbol::Cross;
      break;
    case Symbol::Cross:
      *symbol = Symbol::Circle;
      break;
    case Symbol::Circle:
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

auto createCenteredRectangle(const raylib::Vector2 &position, const float width,
                             const float height) -> raylib::Rectangle {
  const float actualX = position.x - (width / 2);
  const float actualY = position.y - (height / 2);
  return {actualX, actualY, width, height};
}
} // namespace

auto main() -> int {
  constexpr int screenHeight = 600;
  constexpr int screenWidth = 800;
  const auto screenHeightFloat = static_cast<float>(screenHeight);
  const auto screenWidthFloat = static_cast<float>(screenWidth);
  const raylib::Rectangle screen(0, 0, screenWidth, screenHeight);

  try {
    raylib::Window window(screenWidth, screenHeight, "Tic Tac Toe",
                          FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

    // General data
    auto gameState = GameState::Start;
    std::optional<Symbol> winner = std::optional<Symbol>();

    // GameState::Start data
    const raylib::Vector2 buttonPosition(screenWidthFloat / 2, screenHeightFloat / 3 * 2);
    const float buttonWidth = screenWidthFloat / 8;
    const float buttonHeight = screenHeightFloat / 12;
    const auto startButton = createCenteredRectangle(buttonPosition, buttonWidth, buttonHeight);

    // GameState::Game data
    auto game = Game::create(screen);

    while (!raylib::Window::ShouldClose()) {
      if (gameState == GameState::Start) {
        if (raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON)) {
          const bool startButtonPressed =
              startButton.CheckCollision(raylib::Mouse::GetPosition());
          if (startButtonPressed) {
            gameState = GameState::Game;
          }
        }

        window.BeginDrawing();
        window.ClearBackground(raylib::Color::White());

        const raylib::Vector2 textPosition(
          screenWidthFloat / 2, 
          screenHeightFloat / 3
        );
        drawCenteredText("Tic Tac Toe", textPosition, TITLE_FONT_SIZE, TEXT_SPACING,
                         raylib::Color::Black());

        startButton.DrawLines(raylib::Color::Black(), LINE_THICKNESS);

        window.EndDrawing();
      } else if (gameState == GameState::Game) {
        std::optional<raylib::Rectangle> rect =
            std::optional<raylib::Rectangle>();
        if (raylib::Mouse::IsButtonPressed(MOUSE_LEFT_BUTTON)) {
          const Field field = game.findCollider(raylib::Mouse::GetPosition());
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

        game.draw(LINE_THICKNESS, raylib::Color::Black());
        game.drawSymbols();

        window.EndDrawing();
      } else if (gameState == GameState::Finished) {
        if (!winner.has_value()) {
          return EXIT_FAILURE;
        }

        window.BeginDrawing();
        window.ClearBackground(raylib::Color::White());

        string text = std::format("{} wins!", winner.value());
        const raylib::Vector2 textPosition(
          screenWidthFloat / 2,
          screenHeightFloat / 2
        );
        drawCenteredText(text, textPosition, FONT_SIZE, TEXT_SPACING, raylib::Color::Black());

        window.EndDrawing();
      }
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << '\n';
    return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}
