#pragma once

namespace ksesh {

enum CommandID : juce::CommandID {
  kCommandIDFirst = 1,

  commandFileExportAsPdf,
  commandFileExportAsPng,
  commandFileExit,

  kCommandIDMax,
};

}
