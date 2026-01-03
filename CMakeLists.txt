cmake_minimum_required(VERSION 3.16)
project(KSesh VERSION 2.0.1 LANGUAGES CXX)

if (APPLE)
  enable_language(Swift)
endif()

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if (APPLE)
  if (IOS)
    set(ksesh_deployment_target "26.0")
  else()
    set(ksesh_deployment_target "13.3")
  endif()
  set(ksesh_apple_development_team_id "XM695N4A7T")
endif()

if (MSVC)
  add_definitions(
    /DUNICODE
    /D_UNICODE
    /source-charset:utf-8
    /we4828
    /wd4100 # Suppress "unreferenced formal parameter" warnings
  )
  add_compile_options(
    $<$<CONFIG:Debug>:/MTd>
    $<$<CONFIG:Release>:/MT>
    $<$<CONFIG:RelWithDebInfo>:/MT>
    $<$<CONFIG:MinSizeRel>:/MT>
    /MP
    /d1trimfile:${CMAKE_SOURCE_DIR}/
  )
  set(ksesh_icon_small "res/app-icon/windows/icon_512x512.png")
  set(ksesh_icon_big "res/app-icon/windows/icon_512x512@2x.png")
elseif (APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${ksesh_deployment_target}")
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${ksesh_apple_development_team_id}")
  set(CMAKE_XCODE_ATTRIBUTE_SWIFT_VERSION "6.0")
  set(CMAKE_XCODE_ATTRIBUTE_DEBUG_INFORMATION_FORMAT "dwarf-with-dsym")
  set(CMAKE_XCODE_ATTRIBUTE_GCC_GENERATE_DEBUGGING_SYMBOLS[variant=Release] "YES")
  set(CMAKE_XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,YES,NO>)
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
  if ("${CMAKE_SYSTEM_NAME}" STREQUAL "iOS")
    set(ksesh_icon_small "res/app-icon/windows/icon_512x512.png")
    set(ksesh_icon_big "res/app-icon/windows/icon_512x512@2x.png")
    set(CMAKE_XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY "1,2")
  else()
    set(ksesh_icon_small "res/app-icon/macos/icon_512x512.png")
    set(ksesh_icon_big "res/app-icon/macos/icon_512x512@2x.png")
  endif()
endif()

add_subdirectory(deps/JUCE EXCLUDE_FROM_ALL)
add_subdirectory(deps/eglyf EXCLUDE_FROM_ALL)

# Developer accidentally uploaded an app binary to the appstoreconnect with wrong version number.
# The appstoreconnect doesn't allow version numbers to go down, so we apply a higher value than CFBundleShortVersionString.
math(EXPR ksesh_bundle_version_major "${PROJECT_VERSION_MAJOR} + 1")
juce_add_gui_app(ksesh
  PRODUCT_NAME "KSesh"
  APP_SANDBOX_ENABLED TRUE
  APP_SANDBOX_INHERIT FALSE
  APP_SANDBOX_OPTIONS "com.apple.security.files.user-selected.read-write;com.apple.security.files.bookmarks.app-scope"
  VERSION "${CMAKE_PROJECT_VERSION}"
  BUILD_VERSION "${ksesh_bundle_version_major}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}"
  BUNDLE_ID "com.github.kbinani.ksesh"
  HARDENED_RUNTIME_ENABLED TRUE
  PLIST_TO_MERGE "
    <plist>
      <dict>
        <key>LSApplicationCategoryType</key>
        <string>public.app-category.productivity</string>
        <key>ITSAppUsesNonExemptEncryption</key>
        <false/>
        <key>LSMinimumSystemVersion</key>
        <string>${ksesh_macos_deployment_target}</string>
        <key>CFBundleLocalizations</key>
        <array>
          <string>en</string>
          <string>ja</string>
        </array>
      </dict>
    </plist>"
  COMPANY_COPYRIGHT "Copyright © 2024 kbinani"
  ICON_BIG "${ksesh_icon_big}"
  ICON_SMALL "${ksesh_icon_small}"
)

list(APPEND ksesh_files
  deps/PDFGen/pdfgen.h

  src/desktop/MainWindow.hpp
  src/desktop/MainComponent.hpp
  src/desktop/SignListComponent.hpp
  src/desktop/TextEditorComponent.hpp
  src/desktop/HieroglyphComponent.hpp
  src/desktop/SplitterComponent.hpp
  src/desktop/Document.hpp
  src/desktop/SignList.hpp
  src/desktop/Harfbuzz.hpp
  src/desktop/String.hpp
  src/desktop/Insertions.hpp
  src/desktop/LookAndFeel.hpp
  src/desktop/Sign.hpp
  src/desktop/SignListButtonContainer.hpp
  src/desktop/AppSetting.hpp
  src/desktop/PresentationSetting.hpp
  src/desktop/PDFGen.cpp
  src/desktop/PDFGen.hpp
  src/desktop/MenuBarModel.hpp
  src/desktop/CommandID.hpp
  src/desktop/Clipboard.hpp
  src/desktop/BottomToolBar.hpp
  src/desktop/AboutComponent.hpp
  src/desktop/ExampleComponent.hpp
  src/desktop/defer.hpp
  src/desktop/FontFamily.hpp
  src/desktop/FontLoaderComponent.hpp
  src/desktop/FontSet.hpp
  src/desktop/FontAdapter.hpp
  src/desktop/CharBase.hpp
  src/desktop/Char.hpp
  src/desktop/Cluster.hpp
  src/desktop/Line.hpp
  src/desktop/Direction.hpp
  src/desktop/LineAndCluster.hpp
  src/desktop/CursorLocation.hpp
  src/desktop/Cursor.hpp
  src/desktop/CaretLocation.hpp
  src/desktop/juce_TextEditor.cpp
  src/desktop/juce_TextEditor.h
  src/desktop/FilePath.hpp
  src/desktop/EMFExporter.hpp
  src/desktop/PDFExporter.hpp
  src/desktop/ScopedHANDLE.hpp
  src/desktop/Status.hpp
)
if (APPLE)
  list(APPEND ksesh_files src/desktop/Application.mm)
else()
  list(APPEND ksesh_files src/desktop/Application.cpp)
endif()

target_sources(ksesh PRIVATE ${ksesh_files})
list(APPEND all_files ${ksesh_files})

juce_add_binary_data(ksesh-binary-data
  NAMESPACE ksesh::BinaryData
  HEADER_NAME BinaryData.hpp
  SOURCES
    "deps/font-tools/EgyptianOpenType/font/eot.ttf"
    "res/license/Egyptian Text Font.txt"
    "res/license/JUCE Framework.txt"
    "res/license/KSesh.txt"
    "res/license/PDFGen.txt"
    "res/translation/japanese.txt"
    "res/font/NewGardiner.ttf"
    "res/license/NewGardiner-OFL.txt"
    "res/font/NotoSansEgyptianHieroglyphs-Regular.ttf"
    "res/license/NotoSansEgyptianHieroglyphs-OFL.txt"
    "res/app-icon/windows/icon_512x512.png"
)

target_compile_definitions(ksesh
  PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:ksesh,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="$<TARGET_PROPERTY:ksesh,JUCE_VERSION>"
)

if (MSVC)
  target_compile_definitions(ksesh
    PRIVATE
      NOMINMAX
      WIN32_LEAN_AND_MEAN
  )
  target_compile_options(ksesh PRIVATE $<$<CONFIG:Release>:/Zi>)
  target_link_options(ksesh PRIVATE $<$<CONFIG:Release>:/DEBUG>)
elseif (APPLE)
  set_target_properties(ksesh
    PROPERTIES
      XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "$(inherited)"
  )
endif()

target_include_directories(ksesh PRIVATE deps/PDFGen deps/eglyf/include)
target_link_libraries(ksesh
  PRIVATE
    ksesh-binary-data
    juce::juce_gui_extra
    juce::juce_cryptography
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
)

juce_add_console_app(ksesh-test)
list(APPEND test_files
  test/main.cpp
)
target_sources(ksesh-test PRIVATE ${test_files})
list(APPEND all_files ${test_files})
target_include_directories(ksesh-test PRIVATE deps/doctest/doctest src/desktop)
target_link_libraries(ksesh-test
  PRIVATE
    ksesh-binary-data
    juce::juce_gui_extra
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
)
if (MSVC)
  target_compile_definitions(ksesh-test
    PRIVATE
      NOMINMAX
      WIN32_LEAN_AND_MEAN
  )
endif()

if (IOS)
  list(APPEND keyboard_files
    src/ime/ios/KeyboardViewController.swift
    src/ime/ios/Button.swift
    src/ime/ios/ContainerView.swift
    src/ime/ios/Layout.swift
    src/ime/ios/HorizontalViewFlow.swift
    src/ime/ios/TextView.swift
    src/ime/ios/Font.swift
    src/ime/ios/CTLine.swift
    src/ime/ios/UnicodeScalar.swift
    src/ime/ios/CTRun.swift
    src/ime/ios/NSString.swift
    src/ime/ios/UIFont.swift
    src/ime/ios/BoundingBox.swift
    src/ime/ios/String.swift
    src/ime/ios/TextViewContent.swift
    src/ime/ios/TextViewSource.swift
    src/ime/ios/Hieroglyph.swift
    src/ime/ios/GlyphsPanelViewController.swift
    src/ime/ios/GlyphsPanelViewController.xib
    src/ime/ios/NSLayoutConstraint.swift
    src/ime/ios/KeyCap.swift
    src/ime/ios/UIKeyboardAppearance.swift
    src/ime/ios/CenterIconButton.swift
    src/ime/ios/Button.swift
    src/ime/ios/AlephButton.swift
    src/ime/ios/ShiftButton.swift
    src/ime/ios/LeftIconButton.swift
    src/ime/ios/RightIconButton.swift
    src/ime/ios/TextButton.swift
    src/ime/ios/BackspaceButton.swift
    src/ime/ios/HieroglyphButton.swift
    src/ime/ios/Info.plist
    res/font/EglyfDebugNG-Regular-3.02.ttf
  )
  add_library(ksesh-keyboard MODULE)
  target_sources(ksesh-keyboard PRIVATE ${keyboard_files})
  list(APPEND all_files ${keyboard_files})
  set_target_properties(ksesh-keyboard
    PROPERTIES
      XCODE_PRODUCT_TYPE "com.apple.product-type.app-extension"
      XCODE_ATTRIBUTE_INFOPLIST_FILE "${CMAKE_CURRENT_SOURCE_DIR}/src/ime/ios/Info.plist"
      XCODE_ATTRIBUTE_PRODUCT_NAME "\$(TARGET_NAME)"
      XCODE_ATTRIBUTE_LD_RUNPATH_SEARCH_PATHS "\$(inherited)\n@executable_path/Frameworks\n@executable_path/../../Frameworks"
      XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER "com.github.kbinani.ksesh.keyboard"
      XCODE_ATTRIBUTE_MACH_O_TYPE "mh_execute"
      RESOURCE "res/font/EglyfDebugNG-Regular-3.02.ttf;src/ime/ios/GlyphsPanelViewController.xib"
  )
  set_target_properties(ksesh
    PROPERTIES
      XCODE_EMBED_APP_EXTENSIONS ksesh-keyboard
  )
endif()

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${all_files})

if (MSVC)
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pkg/windows-store/Package.wapproj.in
    ${CMAKE_CURRENT_SOURCE_DIR}/pkg/windows-store/Package.wapproj
  )
  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/pkg/windows-store/Package.appxmanifest.in
    ${CMAKE_CURRENT_SOURCE_DIR}/pkg/windows-store/Package.appxmanifest
  )
  include_external_msproject(Package "${CMAKE_CURRENT_SOURCE_DIR}/pkg/windows-store/Package.wapproj"
    TYPE C7167F0D-BC9F-4E6E-AFE1-012C56B48DB5
    GUID 7F81454A-13E7-4AC5-9B69-CCD8BCF2C2B3
  )
endif()
