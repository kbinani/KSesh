#pragma once

namespace ksesh {

enum CommandID : juce::CommandID {
  kCommandIDFirst = 1,

  commandFileOpen,
  commandFileSave,
  commandFileSaveAs,
  commandFileExportAsPdf,
  commandFileExportAsPng1x,
  commandFileExportAsPng2x,
  commandFileExportAsPng4x,
  commandFileExportAsPng8x,
#if JUCE_WINDOWS
  commandFileExportAsEmf,
#endif
  commandFileExit,

  kCommandIDMax,
};

} // namespace ksesh
