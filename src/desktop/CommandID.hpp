#pragma once

namespace ksesh {

enum CommandID : juce::CommandID {
  kCommandIDFirst = 1,

  commandFileExportAsPdf,
  commandFileExportAsPng1x,
  commandFileExportAsPng2x,
  commandFileExportAsPng4x,
  commandFileExportAsPng8x,
  commandFileExit,

  kCommandIDMax,
};

}