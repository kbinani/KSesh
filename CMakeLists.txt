cmake_minimum_required(VERSION 3.16)
project(KSesh VERSION 1.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(ksesh_macos_deployment_target "13.3")
set(ksesh_apple_development_team_id "XM695N4A7T")

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
  )
  set(ksesh_icon_small "res/app-icon/windows/icon_512x512.png")
  set(ksesh_icon_big "res/app-icon/windows/icon_512x512@2x.png")
elseif (APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "${ksesh_macos_deployment_target}")
  set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "${ksesh_apple_development_team_id}")
  set(CMAKE_OSX_ARCHITECTURES "arm64;x86_64")
  add_compile_options(-Wno-shorten-64-to-32)
  set(ksesh_icon_small "res/app-icon/macos/icon_512x512.png")
  set(ksesh_icon_big "res/app-icon/macos/icon_512x512@2x.png")
endif()

add_subdirectory(deps/JUCE)

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
  src/desktop/Content.hpp
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
  src/desktop/otf/Type.hpp
  src/desktop/otf/Tag.hpp
  src/desktop/otf/TableRecord.hpp
  src/desktop/otf/TableDirectory.hpp
  src/desktop/otf/FontFile.hpp
  src/desktop/otf/io/InputStream.hpp
  src/desktop/otf/io/OutputStream.hpp
  src/desktop/otf/io/FileInputStream.hpp
  src/desktop/otf/io/FileOutputStream.hpp
  src/desktop/otf/io/ByteInputStream.hpp
  src/desktop/otf/io/ByteOutputStream.hpp
  src/desktop/otf/table/Table.hpp
  src/desktop/otf/table/ReadonlyTable.hpp
  src/desktop/otf/table/FontHeaderTable.hpp
  src/desktop/otf/table/MaximumProfileTable.hpp
)
if (APPLE)
  list(APPEND ksesh_files src/desktop/Application.mm)
else()
  list(APPEND ksesh_files src/desktop/Application.cpp)
endif()

target_sources(ksesh PRIVATE ${ksesh_files})

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
  foreach (target IN ITEMS ksesh ksesh-binary-data)
    set_target_properties(${target} PROPERTIES XCODE_ATTRIBUTE_ONLY_ACTIVE_ARCH $<IF:$<OR:$<CONFIG:Debug>,$<CONFIG:RelWithDebInfo>>,YES,NO>)
  endforeach()
endif()

target_include_directories(ksesh PRIVATE deps/PDFGen)
target_link_libraries(ksesh
  PRIVATE
    ksesh-binary-data
    juce::juce_gui_extra
  PUBLIC
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
)

list(APPEND test_files
  test/main.cpp
  test/ContentTests.hpp
)
juce_add_console_app(ksesh-test)
target_sources(ksesh-test PRIVATE test/main.cpp ${test_files})
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

source_group(TREE ${CMAKE_CURRENT_LIST_DIR} FILES ${ksesh_files} ${test_files})

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
