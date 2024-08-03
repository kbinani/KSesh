#pragma once

namespace ksesh {

enum CommandID : juce::CommandID {
  kCommandIDFirst = 1,

  commandFileExportAsPdf,
  commandFileExit,

  kCommandIDMax,
};

}
