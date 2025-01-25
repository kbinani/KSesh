#pragma once

namespace ksesh {

enum CommandID : juce::CommandID {
  mainComponentCommandIDBegin = 1,
  commandFileNew = mainComponentCommandIDBegin,
  commandFileOpen,
  commandFileSave,
  commandFileSaveAs,
  commandFileExportAsPdf,
  commandFileExportAsPng1x,
  commandFileExportAsPng2x,
  commandFileExportAsPng4x,
  commandFileExportAsPng8x,
#if defined(JUCE_WINDOWS)
  commandFileExportAsEmf,
#endif
  commandFileExit,

  commandEditSwitchFocus,
  commandEditSignListMoveLeft,
  commandEditSignListMoveUp,
  commandEditSignListMoveRight,
  commandEditSignListMoveDown,
  commandEditCopyAsUnicodeWithoutFormatControl,
  commandEditCopyAsUnicodeWithFormatControl,
#if defined(JUCE_WINDOWS)
  commandEditCopyAsEmf,
#endif
  commandEditCopyAsImage1x,
  commandEditCopyAsImage2x,
  commandEditCopyAsImage4x,
  commandEditCopyAsImage8x,
#if defined(JUCE_MAC)
  commandEditCopyAsPdf,
#endif
  commandViewTogglePreviewVisibility,
  commandHelpAbout,
  commandHelpExample,

  commandUpdateMenuModel,
  mainComponentCommandIDEnd,

  applicationCommandIDBegin,
  commandViewThemeAuto = applicationCommandIDBegin,
  commandViewThemeDark,
  commandViewThemeMidnight,
  commandViewThemeGray,
  commandViewThemeLight,
  applicationCommandIDEnd,
};

} // namespace ksesh
