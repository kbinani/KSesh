#pragma once

#include <juce_graphics/fonts/harfbuzz/hb.hh>
#include <juce_graphics/fonts/juce_FunctionPointerDestructor.h>

namespace ksesh {

using HbBlobUniquePtr = std::unique_ptr<hb_blob_t, juce::FunctionPointerDestructor<hb_blob_destroy>>;
using HbFaceUniquePtr = std::unique_ptr<hb_face_t, juce::FunctionPointerDestructor<hb_face_destroy>>;
using HbFontUniquePtr = std::unique_ptr<hb_font_t, juce::FunctionPointerDestructor<hb_font_destroy>>;
using HbBufferUniquePtr = std::unique_ptr<hb_buffer_t, juce::FunctionPointerDestructor<hb_buffer_destroy>>;

} // namespace ksesh
