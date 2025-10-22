# Changelog

All notable changes to the juce_native_macos_dialogs module will be documented in this file.

## [2.1.0] - 2025-10-21

### Added
- **Small Menu Font Size**: Added optional `useSmallSize` parameter to all popup menu functions
  - Enables more compact menu display using system small font size
  - Available in `showPopupMenu()`, `showPopupMenuAt()`, and `showPopupMenuAtFixed()`
  - Backward compatible with default value of `false`

### Fixed
- **Null Pointer Warning**: Fixed compiler warning in `showPopupMenu()` when passing null to `forView` parameter
  - Now checks for valid view before using `popUpContextMenu:withEvent:forView:`
  - Falls back to `popUpMenuPositioningItem:atLocation:inView:` when no view is available
  - Resolves static analyzer warning about non-null argument requirement

### Changed
- **Objective-C Scope**: Moved `MenuItemTarget` interface and implementation outside `juce` namespace
  - Prevents potential Objective-C++ scoping issues
  - Follows best practices for Objective-C declarations in mixed code

## [2.0.0] - 2025-10-15

### Added
- **Native Popup Menu Support**: Complete `NativeMacPopupMenu` class with three display modes
  - `showPopupMenu()` - Standard context menu at mouse position
  - `showPopupMenuAt()` - Positioned menu with auto-scroll to checked items
  - `showPopupMenuAtFixed()` - Fixed position without auto-scroll (ComboBox style)
- **Advanced Menu Features**:
  - Automatic coordinate system conversion (JUCE ↔ macOS)
  - Checkmark display for selected items
  - Auto-scroll to current selection in preset browsers
  - Full hierarchical menu support (submenus)
  - Proper handling of enabled/disabled states
  - Separator support
- **Documentation**:
  - 16 comprehensive usage examples in examples.cpp
  - Extensive API documentation with use cases
  - Migration guide from JUCE PopupMenu
  - Technical implementation details
  - Performance benchmarks
- **Foundation Framework**: Added to dependencies for NSMenu support

### Fixed
- Critical crash when positioning checked items in submenus (from ob-xd-3 commit 3f47ce8)
- Carbon type conflicts with JUCE (Point, Component) using proper #define guards
- Coordinate system conversion edge cases
- Menu positioning alignment (10px offset for checked items)

### Changed
- Module version bumped to 2.0.0
- Updated module description to include popup menus
- Enhanced examples with menu integration patterns

### Technical
- Implemented recursive `buildNSMenuFromJuceMenu()` helper function
- Added `MenuItemTarget` Objective-C class for menu item selection handling
- Proper memory management with autorelease pools
- Thread-safe menu item ID tracking with static variable

## [1.0.0] - Initial Release

### Added
- **Native Dialogs**:
  - Text input dialog with character limits
  - Information/error dialog
  - Confirmation dialog (two-button)
- **Native Clipboard**:
  - Custom binary data with UTI support
  - Type checking before paste
  - Cross-application clipboard sharing
- **Plugin Support**:
  - Focus restoration for AU/VST plugins
  - DAW-friendly window handling
- **Core Features**:
  - NSAlert integration
  - Cocoa/AppKit framework usage
  - JUCE 7/8 module structure
  - Automatic platform detection (macOS only)

### Platform
- macOS 10.13+ (High Sierra and later)
- JUCE 7.x and JUCE 8.x compatible
- C++17 minimum requirement
