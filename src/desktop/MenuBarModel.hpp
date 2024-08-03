#pragma once

#pragma GCC diagnostic ignored "-Wunused-private-field"

namespace ksesh {

class MenuBarModel : public juce::MenuBarModel {
public:
  explicit MenuBarModel(juce::ApplicationCommandManager *manager) : fManager(manager) {}

  juce::StringArray getMenuBarNames() override {
    juce::StringArray names;
    names.add(TRANS(u8"File"));
    return names;
  }

  juce::PopupMenu getMenuForIndex(int topLevelMenuIndex, juce::String const &menuName) override {
    juce::PopupMenu menu;
    return menu;
  }

  void menuItemSelected(int menuItemID, int topLevelMenuIndex) override {
  }

private:
  juce::ApplicationCommandManager *const fManager;
};

} // namespace ksesh
