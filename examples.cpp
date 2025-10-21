/*******************************************************************************
 Example Usage of juce_native_macos_dialogs Module

 This file demonstrates how to use the native macOS dialogs, popup menus,
 and clipboard integration in your JUCE project.
*******************************************************************************/

#include "../juce_native_macos_dialogs/juce_native_macos_dialogs.h"

//==============================================================================
// Example 1: Text Input Dialog (e.g., Rename Preset)
//==============================================================================
void exampleTextInputDialog()
{
    juce::String newName;
    bool wasRenamed = juce::NativeMacDialogs::showTextInputDialog(
        "Rename Preset",                    // Title
        "Enter new preset name:",           // Message
        "My Preset",                        // Current/default text
        20,                                 // Max length (20 chars)
        "Rename",                           // OK button text
        "Cancel",                           // Cancel button text
        newName                             // Output string
    );

    if (wasRenamed)
    {
        DBG("Preset renamed to: " + newName);
        // Apply the rename logic here
    }
    else
    {
        DBG("Rename cancelled");
    }
}

//==============================================================================
// Example 2: Search Dialog
//==============================================================================
void exampleSearchDialog()
{
    juce::String searchText;
    bool searchInitiated = juce::NativeMacDialogs::showTextInputDialog(
        "Search Presets",                           // Title
        "Enter search terms to find presets:",      // Message
        "",                                         // Empty initial text
        0,                                          // No length limit
        "Search",                                   // OK button text
        "Cancel",                                   // Cancel button text
        searchText                                  // Output string
    );

    if (searchInitiated && searchText.isNotEmpty())
    {
        DBG("Searching for: " + searchText);
        // Perform search logic here
    }
}

//==============================================================================
// Example 3: Information Dialog
//==============================================================================
void exampleInfoDialog()
{
    juce::NativeMacDialogs::showInfoDialog(
        "License Status",                   // Title
        "Your license is activated!",       // Message
        "OK"                                // Button text
    );
}

//==============================================================================
// Example 4: Error Dialog
//==============================================================================
void exampleErrorDialog()
{
    juce::NativeMacDialogs::showInfoDialog(
        "Error",                                    // Title
        "Failed to load preset. File may be corrupted.", // Message
        "OK"                                        // Button text
    );
}

//==============================================================================
// Example 5: Confirmation Dialog
//==============================================================================
void exampleConfirmDialog()
{
    bool userConfirmed = juce::NativeMacDialogs::showConfirmDialog(
        "Delete Preset",                    // Title
        "Are you sure you want to delete this preset? This cannot be undone.", // Message
        "Delete",                           // Button 1 (returns true)
        "Cancel"                            // Button 2 (returns false)
    );

    if (userConfirmed)
    {
        DBG("User confirmed deletion");
        // Delete the preset
    }
    else
    {
        DBG("Deletion cancelled");
    }
}

//==============================================================================
// Example 6: Custom Clipboard Copy (Pasteboard)
//==============================================================================
#if JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD

void exampleCopyPresetToClipboard()
{
    // Example: Copy a preset to clipboard
    juce::MemoryBlock presetData;

    // ... fill presetData with your preset information ...

    // Use a custom UTI (Uniform Type Identifier) for your app
    juce::NativeMacPasteboard::copyDataToClipboard(
        presetData.getData(),
        presetData.getSize(),
        "com.yourcompany.yourapp.preset"  // Custom UTI
    );

    DBG("Preset copied to clipboard");
}

//==============================================================================
// Example 7: Check Clipboard and Paste
//==============================================================================
void examplePastePresetFromClipboard()
{
    const juce::String customUTI = "com.yourcompany.yourapp.preset";

    // Check if clipboard contains our custom data type
    if (juce::NativeMacPasteboard::clipboardContainsDataType(customUTI))
    {
        juce::MemoryBlock presetData;

        if (juce::NativeMacPasteboard::fetchDataFromClipboard(presetData, customUTI))
        {
            DBG("Preset data retrieved from clipboard: " +
                juce::String(presetData.getSize()) + " bytes");

            // ... process the preset data ...
        }
    }
    else
    {
        juce::NativeMacDialogs::showInfoDialog(
            "Paste Error",
            "No preset data found on clipboard.",
            "OK"
        );
    }
}

#endif // JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD

//==============================================================================
// Example 8: Integration in a JUCE Component
//==============================================================================
class MyPluginEditor : public juce::AudioProcessorEditor
{
public:
    MyPluginEditor (juce::AudioProcessor& p)
        : AudioProcessorEditor (p)
    {
        renameButton.onClick = [this] { handleRenameButtonClick(); };
        addAndMakeVisible (renameButton);

        setSize (400, 300);
    }

private:
    void handleRenameButtonClick()
    {
        juce::String currentPresetName = "Current Preset";
        juce::String newName;

        bool renamed = juce::NativeMacDialogs::showTextInputDialog(
            "Rename Preset",
            "Enter new preset name:",
            currentPresetName,
            20,
            "Rename",
            "Cancel",
            newName
        );

        if (renamed && newName.isNotEmpty())
        {
            // Update your preset name
            DBG("Preset renamed to: " + newName);
        }
    }

    juce::TextButton renameButton { "Rename Preset" };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (MyPluginEditor)
};

//==============================================================================
// Example 9: Using in Message Thread
//==============================================================================
void exampleMessageThreadSafe()
{
    // If calling from a background thread, use MessageManager
    juce::MessageManager::callAsync([]
    {
        juce::NativeMacDialogs::showInfoDialog(
            "Background Task Complete",
            "Your export has finished successfully.",
            "OK"
        );
    });
}

//==============================================================================
// POPUP MENU EXAMPLES
//==============================================================================

//==============================================================================
// Example 10: Basic Native Popup Menu
//==============================================================================
void exampleBasicPopupMenu()
{
    juce::PopupMenu menu;

    menu.addItem(1, "Load Preset");
    menu.addItem(2, "Save Preset");
    menu.addSeparator();
    menu.addItem(3, "Import Bank");
    menu.addItem(4, "Export Bank", true, false);  // Disabled item

    // Show at current mouse position
    int result = juce::NativeMacPopupMenu::showPopupMenu(menu);

    if (result == 1)
        DBG("Load Preset selected");
    else if (result == 2)
        DBG("Save Preset selected");
    else if (result == 3)
        DBG("Import Bank selected");
}

//==============================================================================
// Example 11: Preset Browser with Auto-Scroll
//==============================================================================
void examplePresetBrowser()
{
    juce::PopupMenu menu;
    int currentPresetIndex = 25;  // Current selection

    // Add 50 presets
    for (int i = 1; i <= 50; ++i)
    {
        bool isCurrentPreset = (i == currentPresetIndex);
        menu.addItem(i, "Preset " + juce::String(i), true, isCurrentPreset);
    }

    // Get position to show menu (e.g., below a button)
    juce::Point<int> screenPos(100, 200);

    // Show menu with auto-scroll to checked item
    // The menu will automatically scroll to show "Preset 25"
    int result = juce::NativeMacPopupMenu::showPopupMenuAt(menu, screenPos);

    if (result > 0)
    {
        DBG("Selected preset: " + juce::String(result));
        currentPresetIndex = result;
    }
}

//==============================================================================
// Example 12: ComboBox-Style Fixed Position Menu
//==============================================================================
void exampleComboBoxMenu()
{
    juce::PopupMenu menu;
    int currentVoiceCount = 4;

    // Voice count options
    menu.addItem(1, "1 Voice", true, currentVoiceCount == 1);
    menu.addItem(2, "2 Voices", true, currentVoiceCount == 2);
    menu.addItem(4, "4 Voices", true, currentVoiceCount == 4);
    menu.addItem(6, "6 Voices", true, currentVoiceCount == 6);
    menu.addItem(8, "8 Voices", true, currentVoiceCount == 8);

    // Position below a component (like a ComboBox)
    juce::Point<int> screenPos(150, 300);

    // Show menu at exact position without auto-scroll
    // Perfect for dropdown menus where you want fixed positioning
    int result = juce::NativeMacPopupMenu::showPopupMenuAtFixed(menu, screenPos);

    if (result > 0)
    {
        currentVoiceCount = result;
        DBG("Voice count changed to: " + juce::String(result));
    }
}

//==============================================================================
// Example 13: Hierarchical Menu with Submenus
//==============================================================================
void exampleHierarchicalMenu()
{
    juce::PopupMenu mainMenu;

    // File submenu
    juce::PopupMenu fileMenu;
    fileMenu.addItem(101, "New");
    fileMenu.addItem(102, "Open");
    fileMenu.addItem(103, "Save");
    fileMenu.addItem(104, "Save As...");
    mainMenu.addSubMenu("File", fileMenu);

    mainMenu.addSeparator();

    // Programs submenu with presets
    juce::PopupMenu programsMenu;
    for (int i = 1; i <= 20; ++i)
    {
        programsMenu.addItem(200 + i, "Preset " + juce::String(i));
    }
    mainMenu.addSubMenu("Programs", programsMenu);

    // Banks submenu
    juce::PopupMenu banksMenu;
    banksMenu.addItem(301, "Factory Bank 1");
    banksMenu.addItem(302, "Factory Bank 2");
    banksMenu.addItem(303, "User Bank");
    mainMenu.addSubMenu("Banks", banksMenu);

    int result = juce::NativeMacPopupMenu::showPopupMenu(mainMenu);

    if (result >= 101 && result <= 104)
        DBG("File menu item selected: " + juce::String(result));
    else if (result >= 200 && result < 300)
        DBG("Program selected: " + juce::String(result - 200));
    else if (result >= 301)
        DBG("Bank selected: " + juce::String(result - 300));
}

//==============================================================================
// Example 14: Integration with Component Button
//==============================================================================
class PresetMenuButton : public juce::TextButton
{
public:
    PresetMenuButton()
        : juce::TextButton("Presets")
    {
        onClick = [this] { showPresetMenu(); };
    }

private:
    void showPresetMenu()
    {
        juce::PopupMenu menu;

        // Build preset list
        for (int i = 1; i <= 128; ++i)
        {
            bool isCurrentPreset = (i == currentPreset);
            menu.addItem(i, "Preset " + juce::String::formatted("%03d", i),
                        true, isCurrentPreset);
        }

        // Get screen position of button's bottom-left corner
        auto bounds = getScreenBounds();
        juce::Point<int> menuPos(bounds.getX(), bounds.getBottom());

        // Show menu with auto-scroll to current preset
        int result = juce::NativeMacPopupMenu::showPopupMenuAt(menu, menuPos);

        if (result > 0)
        {
            currentPreset = result;
            DBG("Loaded preset: " + juce::String(result));
        }
    }

    int currentPreset = 1;
};

//==============================================================================
// Example 15: Context Menu on Right-Click
//==============================================================================
class ContextMenuComponent : public juce::Component
{
public:
    void mouseDown(const juce::MouseEvent& e) override
    {
        if (e.mods.isPopupMenu())  // Right-click or Ctrl+click
        {
            showContextMenu();
        }
    }

private:
    void showContextMenu()
    {
        juce::PopupMenu menu;

        menu.addItem(1, "Copy");
        menu.addItem(2, "Paste", canPaste());
        menu.addSeparator();
        menu.addItem(3, "Reset");
        menu.addItem(4, "Randomize");

        // Show at current mouse position
        int result = juce::NativeMacPopupMenu::showPopupMenu(menu, this);

        switch (result)
        {
            case 1: handleCopy(); break;
            case 2: handlePaste(); break;
            case 3: handleReset(); break;
            case 4: handleRandomize(); break;
        }
    }

    void handleCopy() { DBG("Copy"); }
    void handlePaste() { DBG("Paste"); }
    void handleReset() { DBG("Reset"); }
    void handleRandomize() { DBG("Randomize"); }
    bool canPaste() { return true; }
};

//==============================================================================
// Example 16: Migrating from JUCE PopupMenu to Native
//==============================================================================
void exampleMigrationFromJUCE()
{
    juce::PopupMenu menu;
    menu.addItem(1, "Option 1");
    menu.addItem(2, "Option 2");
    menu.addItem(3, "Option 3");

    // OLD JUCE way:
    // int result = menu.show();

    // NEW Native macOS way:
    #if JUCE_MAC
        int result = juce::NativeMacPopupMenu::showPopupMenu(menu);
    #else
        int result = menu.show();
    #endif

    if (result > 0)
        DBG("Selected: " + juce::String(result));
}
