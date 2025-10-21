/*******************************************************************************
 Native macOS Dialogs Module Implementation

 This file provides native macOS dialog and popup menu implementations
 using Cocoa/AppKit/Foundation frameworks.
*******************************************************************************/

#include "juce_native_macos_dialogs.h"

#if JUCE_MAC

// Handle Carbon type conflicts with JUCE
// These types conflict between Carbon and JUCE, so we need to hide Carbon's versions
#define Point CarbonDummyPointName
#define Component CarbonDummyCompName

#import <Foundation/Foundation.h>
#import <Cocoa/Cocoa.h>
#import <AppKit/AppKit.h>

// Restore JUCE types after importing macOS headers
#undef Point
#undef Component

namespace juce
{

//==============================================================================
// NativeMacDialogs Implementation
//==============================================================================

bool NativeMacDialogs::showTextInputDialog (const juce::String& title,
                                            const juce::String& message,
                                            const juce::String& currentText,
                                            int maxLength,
                                            const juce::String& okButtonText,
                                            const juce::String& cancelButtonText,
                                            juce::String& outText)
{
    @autoreleasepool
    {
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText: [NSString stringWithUTF8String: title.toRawUTF8()]];
        [alert setInformativeText: [NSString stringWithUTF8String: message.toRawUTF8()]];

        // Create text input field
        NSTextField* input = [[NSTextField alloc] initWithFrame: NSMakeRect(0, 0, 250, 24)];
        [input setStringValue: [NSString stringWithUTF8String: currentText.toRawUTF8()]];

        // Add character limit observer if specified
        id observer = nil;
        if (maxLength > 0)
        {
            observer = [[NSNotificationCenter defaultCenter]
                addObserverForName: NSControlTextDidChangeNotification
                object: input
                queue: nil
                usingBlock: ^(NSNotification* note)
                {
                    NSString* text = [input stringValue];
                    if ([text length] > (NSUInteger)maxLength)
                    {
                        [input setStringValue: [text substringToIndex: maxLength]];
                    }
                }];
        }

        [alert setAccessoryView: input];
        [alert addButtonWithTitle: [NSString stringWithUTF8String: okButtonText.toRawUTF8()]];
        [alert addButtonWithTitle: [NSString stringWithUTF8String: cancelButtonText.toRawUTF8()]];

        // Store original window to restore focus (important for plugins)
        NSWindow* originalWindow = [[NSApplication sharedApplication] keyWindow];

        // Select all text when dialog appears
        dispatch_async(dispatch_get_main_queue(), ^{
            [input setEditable: YES];
            [input setSelectable: YES];
            [[alert window] makeFirstResponder: input];
            [input selectText: nil];
        });

        NSInteger result = [alert runModal];

        // Clean up observer
        if (observer != nil)
        {
            [[NSNotificationCenter defaultCenter] removeObserver: observer];
        }

        // Restore focus to original window (important for AU/VST plugins)
        if (originalWindow && [originalWindow isVisible])
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                [originalWindow makeKeyAndOrderFront: nil];
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.05 * NSEC_PER_SEC),
                              dispatch_get_main_queue(), ^{
                    [originalWindow makeKeyAndOrderFront: nil];
                });
            });
        }

        if (result == NSAlertFirstButtonReturn)
        {
            NSString* newText = [input stringValue];
            outText = juce::String::fromUTF8([newText UTF8String]);
            return true;
        }

        return false;
    }
}

//==============================================================================
void NativeMacDialogs::showInfoDialog (const juce::String& title,
                                       const juce::String& message,
                                       const juce::String& buttonText)
{
    @autoreleasepool
    {
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText: [NSString stringWithUTF8String: title.toRawUTF8()]];
        [alert setInformativeText: [NSString stringWithUTF8String: message.toRawUTF8()]];
        [alert setAlertStyle: NSAlertStyleInformational];
        [alert addButtonWithTitle: [NSString stringWithUTF8String: buttonText.toRawUTF8()]];

        // Store original window to restore focus
        NSWindow* originalWindow = [[NSApplication sharedApplication] keyWindow];

        [alert runModal];

        // Restore focus to original window
        if (originalWindow && [originalWindow isVisible])
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                [originalWindow makeKeyAndOrderFront: nil];
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.05 * NSEC_PER_SEC),
                              dispatch_get_main_queue(), ^{
                    [originalWindow makeKeyAndOrderFront: nil];
                });
            });
        }
    }
}

//==============================================================================
bool NativeMacDialogs::showConfirmDialog (const juce::String& title,
                                          const juce::String& message,
                                          const juce::String& button1Text,
                                          const juce::String& button2Text)
{
    @autoreleasepool
    {
        NSAlert* alert = [[NSAlert alloc] init];
        [alert setMessageText: [NSString stringWithUTF8String: title.toRawUTF8()]];
        [alert setInformativeText: [NSString stringWithUTF8String: message.toRawUTF8()]];
        [alert setAlertStyle: NSAlertStyleWarning];
        [alert addButtonWithTitle: [NSString stringWithUTF8String: button1Text.toRawUTF8()]];
        [alert addButtonWithTitle: [NSString stringWithUTF8String: button2Text.toRawUTF8()]];

        // Store original window to restore focus
        NSWindow* originalWindow = [[NSApplication sharedApplication] keyWindow];

        NSInteger result = [alert runModal];

        // Restore focus to original window
        if (originalWindow && [originalWindow isVisible])
        {
            dispatch_async(dispatch_get_main_queue(), ^{
                [originalWindow makeKeyAndOrderFront: nil];
                dispatch_after(dispatch_time(DISPATCH_TIME_NOW, 0.05 * NSEC_PER_SEC),
                              dispatch_get_main_queue(), ^{
                    [originalWindow makeKeyAndOrderFront: nil];
                });
            });
        }

        return (result == NSAlertFirstButtonReturn);
    }
}

//==============================================================================
// NativeMacPasteboard Implementation
//==============================================================================

#if JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD

void NativeMacPasteboard::copyDataToClipboard (const void* data, size_t size,
                                               const juce::String& typeUTI)
{
    @autoreleasepool
    {
        NSData* dataToCopy = [NSData dataWithBytes: data length: size];
        NSString* pasteboardType = [NSString stringWithUTF8String: typeUTI.toRawUTF8()];

        [[NSPasteboard generalPasteboard] declareTypes: @[pasteboardType] owner: nil];
        [[NSPasteboard generalPasteboard] setData: dataToCopy forType: pasteboardType];
    }
}

//==============================================================================
bool NativeMacPasteboard::clipboardContainsDataType (const juce::String& typeUTI)
{
    @autoreleasepool
    {
        NSString* pasteboardType = [NSString stringWithUTF8String: typeUTI.toRawUTF8()];
        return [[NSPasteboard generalPasteboard]
                canReadItemWithDataConformingToTypes: @[pasteboardType]];
    }
}

//==============================================================================
bool NativeMacPasteboard::fetchDataFromClipboard (juce::MemoryBlock& memoryBlock,
                                                  const juce::String& typeUTI)
{
    @autoreleasepool
    {
        NSString* pasteboardType = [NSString stringWithUTF8String: typeUTI.toRawUTF8()];
        NSData* data = [[NSPasteboard generalPasteboard] dataForType: pasteboardType];

        if (data != nil)
        {
            memoryBlock.replaceAll(data.bytes, data.length);
            return true;
        }

        return false;
    }
}

#endif // JUCE_NATIVE_MACOS_ENABLE_PASTEBOARD

} // namespace juce (temporarily close for Objective-C declarations)

//==============================================================================
// NativeMacPopupMenu Implementation - Objective-C declarations at global scope
//==============================================================================

// Static variable to store the selected menu item ID
static int gSelectedMenuItemID = 0;

// Target object to handle menu item selection - MUST be at global/file scope
@interface MenuItemTarget : NSObject
- (void)menuItemSelected:(id)sender;
@end

@implementation MenuItemTarget
- (void)menuItemSelected:(id)sender
{
    NSMenuItem* item = (NSMenuItem*)sender;
    gSelectedMenuItemID = (int)[item tag];
}
@end

namespace juce
{

//==============================================================================
// Helper function to recursively build NSMenu from JUCE PopupMenu
// Returns the menu and optionally the checked item (via output parameter)
static NSMenu* buildNSMenuFromJuceMenu (const juce::PopupMenu& juceMenu,
                                       MenuItemTarget* target,
                                       NSMenuItem** outCheckedItem = nullptr,
                                       const juce::String& menuTitle = juce::String(),
                                       bool useSmallSize = false)
{
    @autoreleasepool
    {
        NSMenu* nsMenu = [[NSMenu alloc] initWithTitle: [NSString stringWithUTF8String: menuTitle.toRawUTF8()]];
        [nsMenu setAutoenablesItems: NO];

        // Set small menu font size if requested
        if (useSmallSize)
        {
            NSFont* smallFont = [NSFont menuFontOfSize: [NSFont systemFontSizeForControlSize: NSControlSizeSmall]];
            [nsMenu setFont: smallFont];
        }

        // Get menu iterator
        juce::PopupMenu::MenuItemIterator iterator (juceMenu);

        while (iterator.next())
        {
            const auto& item = iterator.getItem();

            if (item.isSeparator)
            {
                [nsMenu addItem: [NSMenuItem separatorItem]];
            }
            else if (item.subMenu != nullptr)
            {
                // Create submenu recursively
                // IMPORTANT: DON'T pass outCheckedItem to submenus
                // We only want to track checked items at the top level, not in submenus
                // This prevents crashes when trying to position submenu items in parent menu
                NSMenu* subMenu = buildNSMenuFromJuceMenu (*item.subMenu, target, nullptr, item.text, useSmallSize);
                NSMenuItem* subMenuItem = [[NSMenuItem alloc]
                    initWithTitle: [NSString stringWithUTF8String: item.text.toRawUTF8()]
                    action: nil
                    keyEquivalent: @""];
                [subMenuItem setSubmenu: subMenu];
                [subMenuItem setEnabled: item.isEnabled];
                [nsMenu addItem: subMenuItem];
            }
            else
            {
                // Regular menu item
                NSString* title = [NSString stringWithUTF8String: item.text.toRawUTF8()];
                NSMenuItem* nsItem = [[NSMenuItem alloc]
                    initWithTitle: title
                    action: @selector(menuItemSelected:)
                    keyEquivalent: @""];

                // Store the item ID in the tag and set the target
                [nsItem setTag: item.itemID];
                [nsItem setTarget: target];
                [nsItem setEnabled: item.isEnabled];
                [nsItem setState: item.isTicked ? NSControlStateValueOn : NSControlStateValueOff];

                // If this item is checked and we haven't found a checked item yet, store it
                if (item.isTicked && outCheckedItem != nullptr && *outCheckedItem == nullptr)
                {
                    *outCheckedItem = nsItem;
                }

                [nsMenu addItem: nsItem];
            }
        }

        return nsMenu;
    }
}

//==============================================================================
int NativeMacPopupMenu::showPopupMenu (const juce::PopupMenu& menu,
                                      juce::Component* parentComponent,
                                      bool useSmallSize)
{
    @autoreleasepool
    {
        // Reset the selected item ID
        gSelectedMenuItemID = 0;

        // Create target object
        MenuItemTarget* target = [[MenuItemTarget alloc] init];

        // Get current mouse location in screen coordinates
        NSPoint mouseLocation = [NSEvent mouseLocation];

        // Convert to view coordinates if we have a parent component
        NSView* view = nullptr;
        if (parentComponent != nullptr)
        {
            view = (NSView*) parentComponent->getWindowHandle();
        }

        // Build the native menu and get the checked item if any
        NSMenuItem* checkedItem = nullptr;
        NSMenu* nsMenu = buildNSMenuFromJuceMenu (menu, target, &checkedItem, juce::String(), useSmallSize);

        // Get or create an event for the menu
        NSEvent* currentEvent = [NSApp currentEvent];
        if (currentEvent == nil)
        {
            currentEvent = [NSEvent mouseEventWithType: NSEventTypeLeftMouseDown
                                              location: mouseLocation
                                         modifierFlags: 0
                                             timestamp: [[NSProcessInfo processInfo] systemUptime]
                                          windowNumber: 0
                                               context: nil
                                           eventNumber: 0
                                            clickCount: 1
                                              pressure: 1.0];
        }

        // Show the menu
        // Use view if available, otherwise use popUpMenuPositioningItem for positioning
        if (view != nullptr)
        {
            [NSMenu popUpContextMenu: nsMenu withEvent: currentEvent forView: view];
        }
        else
        {
            // When no view is available, use the positioning method instead
            [nsMenu popUpMenuPositioningItem: checkedItem
                                  atLocation: mouseLocation
                                      inView: nil];
        }

        int result = gSelectedMenuItemID;

        // Clean up
        [nsMenu release];
        [target release];

        return result;
    }
}

//==============================================================================
int NativeMacPopupMenu::showPopupMenuAt (const juce::PopupMenu& menu,
                                        juce::Point<int> screenPosition,
                                        bool useSmallSize)
{
    @autoreleasepool
    {
        // Reset the selected item ID
        gSelectedMenuItemID = 0;

        // Create target object
        MenuItemTarget* target = [[MenuItemTarget alloc] init];

        // Build the native menu and get the checked item if any
        NSMenuItem* checkedItem = nullptr;
        NSMenu* nsMenu = buildNSMenuFromJuceMenu (menu, target, &checkedItem, juce::String(), useSmallSize);

        // Convert JUCE screen coordinates (top-left origin) to NSPoint (bottom-left origin)
        NSScreen* mainScreen = [NSScreen mainScreen];
        NSRect screenFrame = [mainScreen frame];

        // JUCE uses top-left origin, macOS uses bottom-left origin
        CGFloat yPos = screenFrame.size.height - (CGFloat) screenPosition.getY();

        // When positioning a specific item, macOS centers it at the given Y coordinate
        // Add a small offset (~10px, approximately half a menu item height) for proper alignment
        if (checkedItem != nullptr)
        {
            yPos += 10.0;
        }

        NSPoint nsPosition = NSMakePoint ((CGFloat) screenPosition.getX(), yPos);

        // Use popUpMenuPositioningItem:atLocation:inView: for proper positioning
        // If we have a checked item, position it at the cursor so the menu scrolls to show it
        // Otherwise, position the menu with its top at the cursor
        [nsMenu popUpMenuPositioningItem: checkedItem
                              atLocation: nsPosition
                                  inView: nil];

        int result = gSelectedMenuItemID;

        // Clean up
        [nsMenu release];
        [target release];

        return result;
    }
}

//==============================================================================
int NativeMacPopupMenu::showPopupMenuAtFixed (const juce::PopupMenu& menu,
                                              juce::Point<int> screenPosition,
                                              bool useSmallSize)
{
    @autoreleasepool
    {
        // Reset the selected item ID
        gSelectedMenuItemID = 0;

        // Create target object
        MenuItemTarget* target = [[MenuItemTarget alloc] init];

        // Build the native menu without tracking checked items
        // This ensures the menu appears at the exact position without centering
        NSMenu* nsMenu = buildNSMenuFromJuceMenu (menu, target, nullptr, juce::String(), useSmallSize);

        // Convert JUCE screen coordinates (top-left origin) to NSPoint (bottom-left origin)
        NSScreen* mainScreen = [NSScreen mainScreen];
        NSRect screenFrame = [mainScreen frame];

        // JUCE uses top-left origin, macOS uses bottom-left origin
        CGFloat yPos = screenFrame.size.height - (CGFloat) screenPosition.getY();

        NSPoint nsPosition = NSMakePoint ((CGFloat) screenPosition.getX(), yPos);

        // Use popUpMenuPositioningItem with nil to position the menu at the exact location
        // without centering on any checked item. Perfect for ComboBox-style dropdowns.
        [nsMenu popUpMenuPositioningItem: nil
                              atLocation: nsPosition
                                  inView: nil];

        int result = gSelectedMenuItemID;

        // Clean up
        [nsMenu release];
        [target release];

        return result;
    }
}

} // namespace juce

#endif // JUCE_MAC
