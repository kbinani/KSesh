#pragma once

namespace ksesh {

enum CommandID : juce::CommandID {
  kCommandIDFirst = 1,

  commandFileNew,
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

  commandEditCopyAsEmf,
  commandEditCopyAsImage1x,
  commandEditCopyAsImage2x,
  commandEditCopyAsImage4x,
  commandEditCopyAsImage8x,

  kCommandIDMax,
};

} // namespace ksesh
