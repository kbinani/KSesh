#include <hb.h>

#include <string>
#include <iostream>

int main() {
  using namespace std;

  hb_buffer_t *buf = hb_buffer_create();
  u8string text = u8"𓇋𓅱𓄔𓅓𓀀𓂋𓐰𓏤𓆎𓅓𓏏𓐰𓊖";
  hb_buffer_add_utf8(buf, (char const*)text.c_str(), -1, 0, -1);

        hb_buffer_set_direction(buf, HB_DIRECTION_LTR);
        hb_buffer_set_script(buf, HB_SCRIPT_EGYPTIAN_HIEROGLYPHS);
        hb_buffer_set_language(buf, hb_language_from_string("en", -1));

  // If you don't know the direction, script, and language
  // hb_buffer_guess_segment_properties(buf);
  //hb_blob_create_from_file
  hb_blob_t *blob = hb_blob_create_from_file_or_fail("./public/font/eot.ttf"); /* or hb_blob_create_from_file_or_fail() */
  if (!blob) {
    return -1;
  }

  hb_face_t *face = hb_face_create(blob, 0);
  hb_font_t *font = hb_font_create(face);

  hb_feature_t userfeatures[10];
  
  userfeatures[0].tag = HB_TAG('h', 'a', 'l', 'n');
  userfeatures[1].tag = HB_TAG('p', 'r', 'e', 's');
  userfeatures[2].tag = HB_TAG('r', 'l', 'i', 'g');
  userfeatures[3].tag = HB_TAG('b', 'l', 'w', 's');
  userfeatures[4].tag = HB_TAG('a', 'b', 'v', 's');
  userfeatures[5].tag = HB_TAG('p', 's', 't', 's');
  userfeatures[6].tag = HB_TAG('s', 's', '0', '1');
//  userfeatures[7].tag = HB_TAG('r', 't', 'l', 'm');
  userfeatures[7].tag = HB_TAG('v', 'r', 't', '2');
  userfeatures[8].tag = HB_TAG('m', 'a', 'r', 'k');
  userfeatures[9].tag = HB_TAG('m', 'k', 'm', 'k');
  for (int i = 0; i < sizeof(userfeatures) / sizeof(hb_feature_t); i++) {
    userfeatures[i].value = 1;
    userfeatures[i].start = HB_FEATURE_GLOBAL_START;
    userfeatures[i].end = HB_FEATURE_GLOBAL_END;
  }
//  hb_shape(font, buf, userfeatures, 9);
  hb_shape(font, buf, nullptr, 0);

  unsigned int glyph_count;
  hb_glyph_info_t* glyph_info = hb_buffer_get_glyph_infos(buf, &glyph_count);
  hb_glyph_position_t* glyph_pos = hb_buffer_get_glyph_positions(buf, &glyph_count);
  hb_position_t cursor_x = 0;
  hb_position_t cursor_y = 0;
  for (unsigned int i = 0; i < glyph_count; i++) {
    hb_codepoint_t glyphid  = glyph_info[i].codepoint;
    hb_position_t x_offset  = glyph_pos[i].x_offset;
    hb_position_t y_offset  = glyph_pos[i].y_offset;
    hb_position_t x_advance = glyph_pos[i].x_advance;
    hb_position_t y_advance = glyph_pos[i].y_advance;
    /* draw_glyph(glyphid, cursor_x + x_offset, cursor_y + y_offset); */
    cout << "[" << glyphid << ", "  << (cursor_x + x_offset) << ", "  << (cursor_y + y_offset) << "]," << endl;
    cursor_x += x_advance;
    cursor_y += y_advance;
  }

  hb_buffer_destroy(buf);
  hb_font_destroy(font);
  hb_face_destroy(face);
  hb_blob_destroy(blob);

  return 0;
}
