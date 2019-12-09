#pragma once

#include "defines.h"
#include "gfx.h"

#include <SDL.h>
#include <array>

namespace retro8
{
  class State
  {
  public:
    point_t lastLineEnd;
  };

  class Memory
  {
  private:
    uint8_t memory[1024 * 32];

    static constexpr size_t ADDRESS_SPRITE_SHEET = 0x0000;
    
    
    static constexpr size_t ADDRESS_PALETTES = 0x5f10;
    static constexpr size_t ADDRESS_PEN_COLOR = 0x5f25;

    static constexpr size_t ADDRESS_SCREEN_DATA = 0x6000;

    static constexpr size_t BYTES_PER_SCREEN_ROW = 128;
    static constexpr size_t BYTES_PER_PALETTE = sizeof(retro8::gfx::palette_t);
    static constexpr size_t BYTES_PER_SPRITE = sizeof(retro8::gfx::sprite_t);


  public:
    Memory()
    {
      paletteAt(gfx::DRAW_PALETTE_INDEX)->reset();
    }

    gfx::color_byte_t* penColor() { return reinterpret_cast<gfx::color_byte_t*>(&memory[ADDRESS_PEN_COLOR]); }

    gfx::color_byte_t* screenData() { return reinterpret_cast<gfx::color_byte_t*>(&memory[ADDRESS_SCREEN_DATA]); }
    gfx::sprite_t* spriteAt(size_t index) { return reinterpret_cast<gfx::sprite_t*>(&memory[ADDRESS_SPRITE_SHEET + index * BYTES_PER_SPRITE]); }
    gfx::palette_t* paletteAt(size_t index) { return reinterpret_cast<gfx::palette_t*>(&memory[ADDRESS_PALETTES + index * BYTES_PER_PALETTE]); }

    void setScreenData(coord_t x, coord_t y, color_t c)
    {
      uint8_t* address = &memory[ADDRESS_SCREEN_DATA + (y * BYTES_PER_SCREEN_ROW + x) / 2];

      if (x % 2 == 0)
        *address = (*address & 0xf0) | c;
      else
        *address = (*address & 0x0f) | (c << 4);
    }

    color_t screenData(coord_t x, coord_t y) const
    {
      const uint8_t* address = &memory[ADDRESS_SCREEN_DATA + (y * BYTES_PER_SCREEN_ROW + x) / 2];

      if (x % 2 == 0)
        return static_cast<color_t>(*address & 0x0f);
      else
        return static_cast<color_t>((*address >> 4) & 0x0f);
    }

  };

  class Machine
  {
  private:
    State _state;
    Memory _memory;
    SDL_Surface* _surface;
    gfx::Font _font;

  public:
    Machine()
    {
    }

    void init()
    {
      _surface = SDL_CreateRGBSurface(0, 128, 128, 32, 0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000);
    }

    void color(color_t color);

    void cls(color_t color);

    void pset(coord_t x, coord_t y, color_t color);
    color_t pget(coord_t x, coord_t y);

    void line(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);
    void rect(coord_t x0, coord_t y0, coord_t x1, coord_t y1, color_t color);

    void pal(color_t c0, color_t c1);

    void spr(index_t idx, coord_t x, coord_t y);
    void print(const std::string& string, coord_t x, coord_t y, color_t color);

    const State& state() const { return _state; }
    SDL_Surface* screen() const { return _surface; }
    Memory& memory() { return _memory; }
    gfx::Font& font() { return _font; }
  };
}