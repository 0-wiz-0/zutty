/* This file is part of Zutty.
 * Copyright (C) 2020 Tom Szilagyi
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * See the file LICENSE for the full license.
 */

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdint>
#include <string>
#include <vector>
#include <map>

namespace zutty {

   class Font {
   public:
      /* Load a primary font, determining the atlas geometry and setting up
       * a mapping from unicode code points to atlas grid positions.
       */
      Font (const std::string& filename);

      /* Load an alternate font based on an already loaded primary font,
       * conforming to the same atlas geometry (incl. position mapping)
       * and starting with a copy of the atlas texture data.
       * It is an error if the alternate font has different geometry.
       * Any code point not having a glyph in the alternate font will
       * have the glyph of the primary font (if any) in its atlas.
       */
      Font (const std::string& filename, const Font& priFont);

      ~Font () = default;

      uint16_t getPx () const { return px; };
      uint16_t getPy () const { return py; };
      uint16_t getNx () const { return nx; };
      uint16_t getNy () const { return ny; };

      const std::vector <uint8_t>& getAtlas () const { return atlasBuf; };
      const uint8_t* getAtlasData () const { return atlasBuf.data (); };

      struct AtlasPos {
         uint8_t x;
         uint8_t y;
      };
      using atlasMap_t = std::map <uint16_t, AtlasPos>;
      const atlasMap_t& getAtlasMap () const { return atlasMap; };

   private:
      std::string filename;
      uint16_t px; // glyph width in pixels
      uint16_t py; // glyph height in pixels
      uint16_t nx; // number of glyphs in atlas texture per row
      uint16_t ny; // number of rows in atlas texture
      std::vector <uint8_t> atlasBuf; // loaded atlas data
      atlasMap_t atlasMap; // unicode -> atlas position

      /* Start with 1 so as to leave a blank glyph at (0,0).
       * That blank will get referenced for any out-of-bounds text position
       * lookup in the shader, and guarantees that no fractional glyphs will
       * be shown at the right and bottom edges.
       * Also, any glyph mapping lookup that results in (0,0) means that the
       * character code does not exist in the atlas.
       */
      uint16_t atlas_seq = 1;

      void load (bool overlay = false);
      void loadFace (const FT_Face& face, FT_ULong c);
      void loadFace (const FT_Face& face, FT_ULong c, const AtlasPos& apos);
   };

} // namespace zutty
