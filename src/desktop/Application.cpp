#include <juce_gui_extra/juce_gui_extra.h>
#include <juce_opengl/juce_opengl.h>

#include "PDFGen.hpp"

#include <iostream>

#pragma GCC diagnostic ignored "-Wunused-variable"

// clang-format off
#include "String.hpp"
#include "Harfbuzz.hpp"
#include "BinaryData.hpp"
#include "Insertions.hpp"
#include "SignList.hpp"
#include "PresentationSetting.hpp"
#include "Content.hpp"
#include "AppSetting.hpp"
#include "HieroglyphComponent.hpp"
#include "TextEditorComponent.hpp"
#include "Sign.hpp"
#include "SignListButtonContainer.hpp"
#include "SignListComponent.hpp"
#include "SplitterComponent.hpp"
#include "CommandID.hpp"
#include "MenuBarModel.hpp"
#include "MainComponent.hpp"
#include "MainWindow.hpp"
#include "LookAndFeel.hpp"
// clang-format on

using namespace std::literals::string_literals;

namespace ksesh {

class Application : public juce::JUCEApplication, public juce::DarkModeSettingListener {
public:
  Application() {}

  juce::String const getApplicationName() override {
    return JUCE_APPLICATION_NAME_STRING;
  }

  juce::String const getApplicationVersion() override {
    return JUCE_APPLICATION_VERSION_STRING;
  }

  bool moreThanOneInstanceAllowed() override {
    return true;
  }

  static juce::String EmrTypeString(DWORD t) {
    switch (t) {
    case EMR_HEADER:
      return "EMR_HEADER";
    case EMR_POLYBEZIER:
      return "EMR_POLYBEZIER";
    case EMR_POLYGON:
      return "EMR_POLYGON";
    case EMR_POLYLINE:
      return "EMR_POLYLINE";
    case EMR_POLYBEZIERTO:
      return "EMR_POLYBEZIERTO";
    case EMR_POLYLINETO:
      return "EMR_POLYLINETO";
    case EMR_POLYPOLYLINE:
      return "EMR_POLYPOLYLINE";
    case EMR_POLYPOLYGON:
      return "EMR_POLYPOLYGON";
    case EMR_SETWINDOWEXTEX:
      return "EMR_SETWINDOWEXTEX";
    case EMR_SETWINDOWORGEX:
      return "EMR_SETWINDOWORGEX";
    case EMR_SETVIEWPORTEXTEX:
      return "EMR_SETVIEWPORTEXTEX";
    case EMR_SETVIEWPORTORGEX:
      return "EMR_SETVIEWPORTORGEX";
    case EMR_SETBRUSHORGEX:
      return "EMR_SETBRUSHORGEX";
    case EMR_EOF:
      return "EMR_EOF";
    case EMR_SETPIXELV:
      return "EMR_SETPIXELV";
    case EMR_SETMAPPERFLAGS:
      return "EMR_SETMAPPERFLAGS";
    case EMR_SETMAPMODE:
      return "EMR_SETMAPMODE";
    case EMR_SETBKMODE:
      return "EMR_SETBKMODE";
    case EMR_SETPOLYFILLMODE:
      return "EMR_SETPOLYFILLMODE";
    case EMR_SETROP2:
      return "EMR_SETROP2";
    case EMR_SETSTRETCHBLTMODE:
      return "EMR_SETSTRETCHBLTMODE";
    case EMR_SETTEXTALIGN:
      return "EMR_SETTEXTALIGN";
    case EMR_SETCOLORADJUSTMENT:
      return "EMR_SETCOLORADJUSTMENT";
    case EMR_SETTEXTCOLOR:
      return "EMR_SETTEXTCOLOR";
    case EMR_SETBKCOLOR:
      return "EMR_SETBKCOLOR";
    case EMR_OFFSETCLIPRGN:
      return "EMR_OFFSETCLIPRGN";
    case EMR_MOVETOEX:
      return "EMR_MOVETOEX";
    case EMR_SETMETARGN:
      return "EMR_SETMETARGN";
    case EMR_EXCLUDECLIPRECT:
      return "EMR_EXCLUDECLIPRECT";
    case EMR_INTERSECTCLIPRECT:
      return "EMR_INTERSECTCLIPRECT";
    case EMR_SCALEVIEWPORTEXTEX:
      return "EMR_SCALEVIEWPORTEXTEX";
    case EMR_SCALEWINDOWEXTEX:
      return "EMR_SCALEWINDOWEXTEX";
    case EMR_SAVEDC:
      return "EMR_SAVEDC";
    case EMR_RESTOREDC:
      return "EMR_RESTOREDC";
    case EMR_SETWORLDTRANSFORM:
      return "EMR_SETWORLDTRANSFORM";
    case EMR_MODIFYWORLDTRANSFORM:
      return "EMR_MODIFYWORLDTRANSFORM";
    case EMR_SELECTOBJECT:
      return "EMR_SELECTOBJECT";
    case EMR_CREATEPEN:
      return "EMR_CREATEPEN";
    case EMR_CREATEBRUSHINDIRECT:
      return "EMR_CREATEBRUSHINDIRECT";
    case EMR_DELETEOBJECT:
      return "EMR_DELETEOBJECT";
    case EMR_ANGLEARC:
      return "EMR_ANGLEARC";
    case EMR_ELLIPSE:
      return "EMR_ELLIPSE";
    case EMR_RECTANGLE:
      return "EMR_RECTANGLE";
    case EMR_ROUNDRECT:
      return "EMR_ROUNDRECT";
    case EMR_ARC:
      return "EMR_ARC";
    case EMR_CHORD:
      return "EMR_CHORD";
    case EMR_PIE:
      return "EMR_PIE";
    case EMR_SELECTPALETTE:
      return "EMR_SELECTPALETTE";
    case EMR_CREATEPALETTE:
      return "EMR_CREATEPALETTE";
    case EMR_SETPALETTEENTRIES:
      return "EMR_SETPALETTEENTRIES";
    case EMR_RESIZEPALETTE:
      return "EMR_RESIZEPALETTE";
    case EMR_REALIZEPALETTE:
      return "EMR_REALIZEPALETTE";
    case EMR_EXTFLOODFILL:
      return "EMR_EXTFLOODFILL";
    case EMR_LINETO:
      return "EMR_LINETO";
    case EMR_ARCTO:
      return "EMR_ARCTO";
    case EMR_POLYDRAW:
      return "EMR_POLYDRAW";
    case EMR_SETARCDIRECTION:
      return "EMR_SETARCDIRECTION";
    case EMR_SETMITERLIMIT:
      return "EMR_SETMITERLIMIT";
    case EMR_BEGINPATH:
      return "EMR_BEGINPATH";
    case EMR_ENDPATH:
      return "EMR_ENDPATH";
    case EMR_CLOSEFIGURE:
      return "EMR_CLOSEFIGURE";
    case EMR_FILLPATH:
      return "EMR_FILLPATH";
    case EMR_STROKEANDFILLPATH:
      return "EMR_STROKEANDFILLPATH";
    case EMR_STROKEPATH:
      return "EMR_STROKEPATH";
    case EMR_FLATTENPATH:
      return "EMR_FLATTENPATH";
    case EMR_WIDENPATH:
      return "EMR_WIDENPATH";
    case EMR_SELECTCLIPPATH:
      return "EMR_SELECTCLIPPATH";
    case EMR_ABORTPATH:
      return "EMR_ABORTPATH";
    case EMR_GDICOMMENT:
      return "EMR_GDICOMMENT";
    case EMR_FILLRGN:
      return "EMR_FILLRGN";
    case EMR_FRAMERGN:
      return "EMR_FRAMERGN";
    case EMR_INVERTRGN:
      return "EMR_INVERTRGN";
    case EMR_PAINTRGN:
      return "EMR_PAINTRGN";
    case EMR_EXTSELECTCLIPRGN:
      return "EMR_EXTSELECTCLIPRGN";
    case EMR_BITBLT:
      return "EMR_BITBLT";
    case EMR_STRETCHBLT:
      return "EMR_STRETCHBLT";
    case EMR_MASKBLT:
      return "EMR_MASKBLT";
    case EMR_PLGBLT:
      return "EMR_PLGBLT";
    case EMR_SETDIBITSTODEVICE:
      return "EMR_SETDIBITSTODEVICE";
    case EMR_STRETCHDIBITS:
      return "EMR_STRETCHDIBITS";
    case EMR_EXTCREATEFONTINDIRECTW:
      return "EMR_EXTCREATEFONTINDIRECTW";
    case EMR_EXTTEXTOUTA:
      return "EMR_EXTTEXTOUTA";
    case EMR_EXTTEXTOUTW:
      return "EMR_EXTTEXTOUTW";
    case EMR_POLYBEZIER16:
      return "EMR_POLYBEZIER16";
    case EMR_POLYGON16:
      return "EMR_POLYGON16";
    case EMR_POLYLINE16:
      return "EMR_POLYLINE16";
    case EMR_POLYBEZIERTO16:
      return "EMR_POLYBEZIERTO16";
    case EMR_POLYLINETO16:
      return "EMR_POLYLINETO16";
    case EMR_POLYPOLYLINE16:
      return "EMR_POLYPOLYLINE16";
    case EMR_POLYPOLYGON16:
      return "EMR_POLYPOLYGON16";
    case EMR_POLYDRAW16:
      return "EMR_POLYDRAW16";
    case EMR_CREATEMONOBRUSH:
      return "EMR_CREATEMONOBRUSH";
    case EMR_CREATEDIBPATTERNBRUSHPT:
      return "EMR_CREATEDIBPATTERNBRUSHPT";
    case EMR_EXTCREATEPEN:
      return "EMR_EXTCREATEPEN";
    case EMR_POLYTEXTOUTA:
      return "EMR_POLYTEXTOUTA";
    case EMR_POLYTEXTOUTW:
      return "EMR_POLYTEXTOUTW";
    case EMR_SETICMMODE:
      return "EMR_SETICMMODE";
    case EMR_CREATECOLORSPACE:
      return "EMR_CREATECOLORSPACE";
    case EMR_SETCOLORSPACE:
      return "EMR_SETCOLORSPACE";
    case EMR_DELETECOLORSPACE:
      return "EMR_DELETECOLORSPACE";
    case EMR_GLSRECORD:
      return "EMR_GLSRECORD";
    case EMR_GLSBOUNDEDRECORD:
      return "EMR_GLSBOUNDEDRECORD";
    case EMR_PIXELFORMAT:
      return "EMR_PIXELFORMAT";
    case EMR_RESERVED_105:
      return "EMR_RESERVED_105";
    case EMR_RESERVED_106:
      return "EMR_RESERVED_106";
    case EMR_RESERVED_107:
      return "EMR_RESERVED_107";
    case EMR_RESERVED_108:
      return "EMR_RESERVED_108";
    case EMR_RESERVED_109:
      return "EMR_RESERVED_109";
    case EMR_RESERVED_110:
      return "EMR_RESERVED_110";
    case EMR_COLORCORRECTPALETTE:
      return "EMR_COLORCORRECTPALETTE";
    case EMR_SETICMPROFILEA:
      return "EMR_SETICMPROFILEA";
    case EMR_SETICMPROFILEW:
      return "EMR_SETICMPROFILEW";
    case EMR_ALPHABLEND:
      return "EMR_ALPHABLEND";
    case EMR_SETLAYOUT:
      return "EMR_SETLAYOUT";
    case EMR_TRANSPARENTBLT:
      return "EMR_TRANSPARENTBLT";
    case EMR_RESERVED_117:
      return "EMR_RESERVED_117";
    case EMR_GRADIENTFILL:
      return "EMR_GRADIENTFILL";
    case EMR_RESERVED_119:
      return "EMR_RESERVED_119";
    case EMR_RESERVED_120:
      return "EMR_RESERVED_120";
    case EMR_COLORMATCHTOTARGETW:
      return "EMR_COLORMATCHTOTARGETW";
    case EMR_CREATECOLORSPACEW:
      return "EMR_CREATECOLORSPACEW";
    }
    return "unknown";
  }

  static int Proc(HDC hdc, HANDLETABLE *lpht, const ENHMETARECORD *lpmr, int nHandles, LPARAM data) {
    auto name = EmrTypeString(lpmr->iType);
    assert(name.length() > 0);
    DBG("Type=" << name);
    return 1;
  }

  void initialise(juce::String const &) override {
    HbBlobUniquePtr blob(hb_blob_create(BinaryData::eot_ttf,
                                        BinaryData::eot_ttfSize,
                                        HB_MEMORY_MODE_READONLY,
                                        nullptr,
                                        nullptr));
    HbFaceUniquePtr face(hb_face_create(blob.get(), 0));
    fFont.reset(hb_font_create(face.get()));

    ENHMETAHEADER header1;
    {
      HENHMETAFILE f = GetEnhMetaFileA("unnamed.emf");
      auto size = GetEnhMetaFileHeader(f, 0, nullptr);
      GetEnhMetaFileHeader(f, size, &header1);
      EnumEnhMetaFile(nullptr, f, Proc, 0, 0);
      DeleteEnhMetaFile(f);
    }

    auto c = std::make_shared<Content>(U"t\nW10", fFont);
    auto o = c->toEMF(fFont, PresentationSetting());
    FILE *fp = fopen("result.emf", "wb");
    fwrite(o.data(), o.size(), 1, fp);
    fclose(fp);

    DBG("============================");
    ENHMETAHEADER header2;
    {
      HENHMETAFILE f = GetEnhMetaFileA("result.emf");
      auto size = GetEnhMetaFileHeader(f, 0, nullptr);
      GetEnhMetaFileHeader(f, size, &header2);
      EnumEnhMetaFile(nullptr, f, Proc, 0, 0);
      DeleteEnhMetaFile(f);
    }

    quit();
    return;

    fLaf = std::make_unique<LookAndFeel>();
    fLaf->setColourScheme(fSetting.getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(fLaf.get());
    juce::Desktop::getInstance().addDarkModeSettingListener(this);
    fMainWindow = std::make_unique<MainWindow>(getApplicationName(), fFont);
  }

  void shutdown() override {
    fMainWindow = nullptr;
  }

  void systemRequestedQuit() override {
    if (auto target = juce::ApplicationCommandManager::findDefaultComponentTarget(); target) {
      juce::ApplicationCommandTarget::InvocationInfo info(commandFileExit);
      target->invoke(info, false);
    }
  }

  void anotherInstanceStarted(juce::String const &) override {
  }

  void darkModeSettingChanged() override {
    if (fSetting.colorScheme != AppSetting::ColorScheme::Auto) {
      return;
    }
    auto laf = std::make_unique<LookAndFeel>();
    laf->setColourScheme(fSetting.getColorScheme(juce::Desktop::getInstance().isDarkModeActive()));
    juce::LookAndFeel::setDefaultLookAndFeel(laf.get());
    fLaf.swap(laf);
  }

private:
  std::unique_ptr<MainWindow> fMainWindow;
  std::unique_ptr<LookAndFeel> fLaf;
  HbFontUniquePtr fFont;
  AppSetting fSetting;
};

std::vector<std::u32string> const SignList::enclosureBeginning = {
    U"𓉘"s,
    U"𓉙"s,
    U"𓉚"s,
    U"𓊆"s,
    U"𓊈"s,
    U"𓍹"s,
};
std::vector<std::u32string> const SignList::enclosureTerminal = {
    U"𓉛"s,
    U"𓉜"s,
    U"𓉝"s,
    U"𓊇"s,
    U"𓊉"s,
    U"𓍺"s,
    U"𓍻"s,
};

std::u32string const SignList::topStartInsertion = U"\U00013432";
std::u32string const SignList::bottomStartInsertion = U"\U00013433";
std::u32string const SignList::topEndInsertion = U"\U00013434";
std::u32string const SignList::bottomEndInsertion = U"\U00013435";

} // namespace ksesh

START_JUCE_APPLICATION(ksesh::Application)
