/*******************************************************************************
 BEGIN_JUCE_MODULE_DECLARATION

  ID:               juce_native_macos_dialogs
  vendor:           discoDSP
  version:          2.1.0
  name:             Native macOS Dialog Components
  description:      Native macOS dialogs, popup menus, and clipboard integration for JUCE
  website:          https://discodsp.com/
  license:          GPL
  minimumCppStandard: 17

  dependencies:     juce_core, juce_gui_basics
  OSXFrameworks:    Cocoa AppKit Foundation
  iOSFrameworks:

 END_JUCE_MODULE_DECLARATION
*******************************************************************************/

#pragma once
#define JUCE_NATIVE_MACOS_DIALOGS_H_INCLUDED

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

//==============================================================================
/** Config: JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD
    Enables the native macOS pasteboard (clipboard) support.
*/
#ifndef JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD
 #define JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD 1
#endif

//==============================================================================
namespace juce
{

//==============================================================================
/**
    Native macOS dialog boxes using NSAlert and Cocoa frameworks.

    This class provides native macOS dialogs that integrate better with the system
    than JUCE's cross-platform dialogs. Particularly useful for AU/VST plugins
    running in DAWs.

    @tags{GUI}
*/
class JUCE_API  NativeMacDialogs
{
public:
    //==============================================================================
    /** Shows a native macOS text input dialog.

        @param title          The dialog title
        @param message        The informative message to display
        @param currentText    The initial text to show in the input field
        @param maxLength      Maximum number of characters allowed (0 = no limit)
        @param okButtonText   Text for the OK/confirm button
        @param cancelButtonText Text for the cancel button
        @param outText        Reference to string that will receive the entered text

        @returns true if the user clicked OK, false if cancelled
    */
    static bool showTextInputDialog (const juce::String& title,
                                     const juce::String& message,
                                     const juce::String& currentText,
                                     int maxLength,
                                     const juce::String& okButtonText,
                                     const juce::String& cancelButtonText,
                                     juce::String& outText);

    //==============================================================================
    /** Shows a native macOS information/error dialog.

        @param title          The dialog title
        @param message        The message to display
        @param buttonText     Text for the OK button
    */
    static void showInfoDialog (const juce::String& title,
                               const juce::String& message,
                               const juce::String& buttonText = "OK");

    //==============================================================================
    /** Shows a native macOS confirmation dialog with two buttons.

        @param title          The dialog title
        @param message        The message to display
        @param button1Text    Text for the first button (returns true)
        @param button2Text    Text for the second button (returns false)

        @returns true if button1 was clicked, false if button2 was clicked
    */
    static bool showConfirmDialog (const juce::String& title,
                                   const juce::String& message,
                                   const juce::String& button1Text = "OK",
                                   const juce::String& button2Text = "Cancel");

private:
    NativeMacDialogs() = delete;
    JUCE_DECLARE_NON_COPYABLE (NativeMacDialogs)
};

//==============================================================================
#if JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD

/**
    Native macOS pasteboard (clipboard) with custom data type support.

    Allows copying and pasting custom binary data to/from the system clipboard
    using a custom UTI (Uniform Type Identifier).

    @tags{Core}
*/
class JUCE_API  NativeMacPasteboard
{
public:
    //==============================================================================
    /** Copies binary data to the macOS clipboard with a custom type identifier.

        @param data        Pointer to the data to copy
        @param size        Size of the data in bytes
        @param typeUTI     Custom UTI (e.g., "com.yourcompany.yourapp.datatype")
    */
    static void copyDataToClipboard (const void* data, size_t size,
                                     const juce::String& typeUTI);

    //==============================================================================
    /** Checks if clipboard contains data of the specified custom type.

        @param typeUTI     The custom UTI to check for
        @returns true if clipboard contains data of this type
    */
    static bool clipboardContainsDataType (const juce::String& typeUTI);

    //==============================================================================
    /** Retrieves binary data from the clipboard.

        @param memoryBlock  MemoryBlock that will receive the clipboard data
        @param typeUTI      The custom UTI to retrieve
        @returns true if data was successfully retrieved
    */
    static bool fetchDataFromClipboard (juce::MemoryBlock& memoryBlock,
                                       const juce::String& typeUTI);

private:
    NativeMacPasteboard() = delete;
    JUCE_DECLARE_NON_COPYABLE (NativeMacPasteboard)
};

#endif // JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD

//==============================================================================
/**
    Native macOS popup menus using NSMenu for better system integration.

    Provides native macOS popup menus that look and behave like system menus,
    with proper support for checkmarks, submenus, and auto-scrolling to
    selected items.

    @tags{GUI}
*/
class JUCE_API  NativeMacPopupMenu
{
public:
    //==============================================================================
    /** Shows a native macOS popup menu at the current mouse location.

        @param menu              The JUCE PopupMenu to display
        @param parentComponent   Optional parent component for coordinate conversion
        @param useSmallSize      If true, uses small menu font size (more compact)

        @returns the selected menu item ID, or 0 if cancelled
    */
    static int showPopupMenu (const juce::PopupMenu& menu,
                             juce::Component* parentComponent = nullptr,
                             bool useSmallSize = false);

    //==============================================================================
    /** Shows a native macOS popup menu at a specific screen position.

        This version auto-scrolls to show checked items, making it ideal for
        preset browsers and lists where the current selection should be visible.

        When a menu item has a checkmark (isTicked), the menu automatically
        positions itself so that the checked item appears at the cursor location.

        @param menu              The JUCE PopupMenu to display
        @param screenPosition    Position in screen coordinates (JUCE style, top-left origin)
        @param useSmallSize      If true, uses small menu font size (more compact)

        @returns the selected menu item ID, or 0 if cancelled
    */
    static int showPopupMenuAt (const juce::PopupMenu& menu,
                               juce::Point<int> screenPosition,
                               bool useSmallSize = false);

    //==============================================================================
    /** Shows a native macOS popup menu at a fixed position without auto-scroll.

        This version positions the menu at the exact location without centering
        on checked items. Ideal for ComboBox-style dropdowns where you want the
        menu to appear directly below or above a component.

        Unlike showPopupMenuAt, this does NOT reposition the menu based on
        which item is checked, giving you precise control over menu placement.

        @param menu              The JUCE PopupMenu to display
        @param screenPosition    Position in screen coordinates (JUCE style, top-left origin)
        @param useSmallSize      If true, uses small menu font size (more compact)

        @returns the selected menu item ID, or 0 if cancelled
    */
    static int showPopupMenuAtFixed (const juce::PopupMenu& menu,
                                    juce::Point<int> screenPosition,
                                    bool useSmallSize = false);

private:
    NativeMacPopupMenu() = delete;
    JUCE_DECLARE_NON_COPYABLE (NativeMacPopupMenu)
};

} // namespace juce
