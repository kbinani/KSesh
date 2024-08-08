#pragma once

namespace ksesh {

class AboutComponent : public juce::Component {
  enum : int {
    aboutComponentMargin = 24,
    aboutComponentPadding = 60,
  };

  class AttributedStringComponent : public juce::Component {
  public:
    void paint(juce::Graphics &g) override {
      auto bounds = getLocalBounds().toFloat();
      auto padded = bounds.reduced(aboutComponentPadding);
      fString.draw(g, padded);
    }

    float getTextHeight(float width) {
      juce::TextLayout layout;
      layout.createLayout(fString, width);
      return layout.getHeight() + 2 * aboutComponentPadding;
    }

    juce::AttributedString fString;
  };

  static void AddParagraph(juce::String const &title, std::optional<juce::String> url, juce::String const &lines, juce::AttributedString &str) {
    juce::Font regular(juce::FontOptions(14));
    juce::Font bold = regular.boldened();
    str.append(title + "\n", bold, juce::Colours::black);
    str.append("\n", regular, juce::Colours::black);
    if (url) {
      str.append(*url + "\n", regular, juce::Colours::black);
      str.append("\n", regular, juce::Colours::black);
    }
    for (auto const &line : juce::StringArray::fromLines(lines.trimStart())) {
      str.append(line + "\n", regular, juce::Colours::black);
    }
    str.append("\n", regular, juce::Colours::black);
  }

public:
  AboutComponent() {
    fContainer = std::make_unique<juce::Viewport>();
    fContainer->setScrollBarsShown(true, false, true, false);
    addAndMakeVisible(*fContainer);
    fContent = std::make_unique<AttributedStringComponent>();
    fContainer->setViewedComponent(fContent.get(), false);
    fCloseButton = std::make_unique<juce::ShapeButton>("", juce::Colours::grey, juce::Colours::grey.brighter(), juce::Colours::grey.darker());
    juce::Path path;
    path.startNewSubPath(5, 5);
    path.lineTo(15, 15);
    path.startNewSubPath(15, 5);
    path.lineTo(5, 15);
    fCloseButton->setShape(path, false, true, false);
    fCloseButton->setOutline(juce::Colours::black, 5);
    fCloseButton->onClick = [this]() {
      if (onClickClose) {
        onClickClose();
      }
    };
    addAndMakeVisible(*fCloseButton);

    AddParagraph("About KSesh", std::nullopt, R"(
KSesh: An Interactive Hieroglyphic Editor

Copyright (C) 2024 kbinani

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU Affero General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Affero General Public License for more details.

You should have received a copy of the GNU Affero General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.)",
                 fContent->fString);

    AddParagraph("Egyptian Text Font", "https://github.com/microsoft/font-tools/tree/main/EgyptianOpenType/font", R"(
Copyright 2015, Microsoft Corporation (www.microsoft.com), with Reserved Font Name Selawik.  All Rights Reserved.  Selawik is a trademark of Microsoft Corporation in the United States and/or other countries.

This Font Software is licensed under the SIL Open Font License, Version 1.1.
This license is copied below, and is also available with a FAQ at:
http://scripts.sil.org/OFL

-----------------------------------------------------------
SIL OPEN FONT LICENSE Version 1.1 - 26 February 2007
-----------------------------------------------------------
PREAMBLE
The goals of the Open Font License (OFL) are to stimulate worldwide
development of collaborative font projects, to support the font creation
efforts of academic and linguistic communities, and to provide a free and
open framework in which fonts may be shared and improved in partnership
with others.

The OFL allows the licensed fonts to be used, studied, modified and
redistributed freely as long as they are not sold by themselves. The
fonts, including any derivative works, can be bundled, embedded,
redistributed and/or sold with any software provided that any reserved
names are not used by derivative works. The fonts and derivatives,
however, cannot be released under any other type of license. The
requirement for fonts to remain under this license does not apply
to any document created using the fonts or their derivatives.

DEFINITIONS
"Font Software" refers to the set of files released by the Copyright
Holder(s) under this license and clearly marked as such. This may
include source files, build scripts and documentation.

"Reserved Font Name" refers to any names specified as such after the
copyright statement(s).

"Original Version" refers to the collection of Font Software components as
distributed by the Copyright Holder(s).

"Modified Version" refers to any derivative made by adding to, deleting,
or substituting -- in part or in whole -- any of the components of the
Original Version, by changing formats or by porting the Font Software to a
new environment.

"Author" refers to any designer, engineer, programmer, technical
writer or other person who contributed to the Font Software.

PERMISSION & CONDITIONS
Permission is hereby granted, free of charge, to any person obtaining
a copy of the Font Software, to use, study, copy, merge, embed, modify,
redistribute, and sell modified and unmodified copies of the Font
Software, subject to the following conditions:

1) Neither the Font Software nor any of its individual components,
in Original or Modified Versions, may be sold by itself.

2) Original or Modified Versions of the Font Software may be bundled,
redistributed and/or sold with any software, provided that each copy
contains the above copyright notice and this license. These can be
included either as stand-alone text files, human-readable headers or
in the appropriate machine-readable metadata fields within text or
binary files as long as those fields can be easily viewed by the user.

3) No Modified Version of the Font Software may use the Reserved Font
Name(s) unless explicit written permission is granted by the corresponding
Copyright Holder. This restriction only applies to the primary font name as
presented to the users.

4) The name(s) of the Copyright Holder(s) or the Author(s) of the Font
Software shall not be used to promote, endorse or advertise any
Modified Version, except to acknowledge the contribution(s) of the
Copyright Holder(s) and the Author(s) or with their explicit written
permission.

5) The Font Software, modified or unmodified, in part or in whole,
must be distributed entirely under this license, and must not be
distributed under any other license. The requirement for fonts to
remain under this license does not apply to any document created
using the Font Software.

TERMINATION
This license becomes null and void if any of the above conditions are
not met.

DISCLAIMER
THE FONT SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO ANY WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT
OF COPYRIGHT, PATENT, TRADEMARK, OR OTHER RIGHT. IN NO EVENT SHALL THE
COPYRIGHT HOLDER BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
INCLUDING ANY GENERAL, SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL
DAMAGES, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF THE USE OR INABILITY TO USE THE FONT SOFTWARE OR FROM
OTHER DEALINGS IN THE FONT SOFTWARE.)",
                 fContent->fString);

    AddParagraph("JUCE Framework", "https://github.com/juce-framework/JUCE", R"(
# The JUCE Framework

The JUCE Framework is an open source framework licensed under a combination of
open source and commercial licences.

The JUCE Framework modules are dual-licensed under the
[AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html) and the commercial [JUCE
licence](https://juce.com/legal/juce-8-licence/).

## The JUCE Licence

If you are not licensing the JUCE Framework modules under the
[AGPLv3](https://www.gnu.org/licenses/agpl-3.0.en.html) then by downloading,
installing, or using the JUCE Framework, or combining the JUCE Framework with
any other source code, object code, content or any other copyrightable work, you
agree to the terms of the the [JUCE 8 End User Licence
Agreement](https://juce.com/legal/juce-8-licence/), and all incorporated terms
including the [JUCE Privacy Policy](https://juce.com/legal/juce-privacy-policy/)
and the [JUCE Website Terms of
Service](https://juce.com/legal/juce-website-terms-of-service/), as applicable,
which will bind you. If you do not agree to the terms of this Agreement, we will
not license the JUCE Framework to you, and you must discontinue the installation
or download process and cease use of the JUCE Framework.

THE JUCE FRAMEWORK IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES,
WHETHER EXPRESSED OR IMPLIED, INCLUDING WARRANTY OF MERCHANTABILITY OR FITNESS
FOR A PARTICULAR PURPOSE, ARE DISCLAIMED.

For more information, visit the [JUCE website](https://juce.com).)",
                 fContent->fString);

    AddParagraph("PDFGen", "https://github.com/AndreRenaud/PDFGen", R"(
This is free and unencumbered software released into the public domain.

Anyone is free to copy, modify, publish, use, compile, sell, or
distribute this software, either in source code form or as a compiled
binary, for any purpose, commercial or non-commercial, and by any
means.

In jurisdictions that recognize copyright laws, the author or authors
of this software dedicate any and all copyright interest in the
software to the public domain. We make this dedication for the benefit
of the public at large and to the detriment of our heirs and
successors. We intend this dedication to be an overt act of
relinquishment in perpetuity of all present and future rights to this
software under copyright law.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
OTHER DEALINGS IN THE SOFTWARE.

For more information, please refer to <http://unlicense.org>)",
                 fContent->fString);
  }

  void paint(juce::Graphics &g) override {
    g.fillAll(juce::Colours::black.withAlpha(0.5f));
    g.setColour(juce::Colours::white);
    auto bounds = getLocalBounds().reduced(aboutComponentMargin).toFloat();
    g.fillRoundedRectangle(bounds, 8);
  }

  void resized() override {
    auto bounds = getLocalBounds();
    bounds.reduce(aboutComponentMargin, aboutComponentMargin);
    fContainer->setBounds(bounds);
    auto b = fContent->getBounds().withWidth(bounds.getWidth());
    auto textHeight = std::max<float>(bounds.getHeight(), fContent->getTextHeight(bounds.getWidth()));
    fContent->setBounds(b.withHeight(textHeight));
    fCloseButton->setBounds(juce::Rectangle<int>(bounds.removeFromTop(20).removeFromRight(20)).translated(-10, 10));
  }

public:
  std::function<void()> onClickClose;

private:
  std::unique_ptr<juce::Viewport> fContainer;
  std::unique_ptr<AttributedStringComponent> fContent;
  std::unique_ptr<juce::ShapeButton> fCloseButton;
};

} // namespace ksesh
