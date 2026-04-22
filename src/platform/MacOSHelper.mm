#include "MacOSHelper.h"
#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>
#include <functional>

namespace MacOSHelper {

void setWindowBackground(quintptr winId, bool dark) {
    NSView* view = reinterpret_cast<NSView*>(winId);
    NSWindow* window = [view window];
    if (!window) return;
    NSColor* color = dark
        ? [NSColor colorWithRed:25.0/255 green:25.0/255 blue:26.0/255 alpha:1.0]
        : [NSColor whiteColor];
    window.backgroundColor = color;
}

void setViewBackground(quintptr viewId, bool dark) {
    NSView* view = reinterpret_cast<NSView*>(viewId);
    view.wantsLayer = YES;
    CGColorRef color = dark
        ? [NSColor colorWithRed:25.0/255 green:25.0/255 blue:26.0/255 alpha:1.0].CGColor
        : [NSColor whiteColor].CGColor;
    view.layer.backgroundColor = color;
    for (CALayer* sub in view.layer.sublayers) {
        sub.backgroundColor = color;
    }
}

void setupFullSizeTitleBar(quintptr winId, bool dark) {
    NSView* view = reinterpret_cast<NSView*>(winId);
    NSWindow* window = [view window];
    if (!window) return;
    window.styleMask |= NSWindowStyleMaskFullSizeContentView;
    window.titlebarAppearsTransparent = YES;
    window.titleVisibility = NSWindowTitleHidden;

    // Force window appearance so the title bar vibrancy matches our theme
    NSAppearanceName appearanceName = dark
        ? NSAppearanceNameDarkAqua
        : NSAppearanceNameAqua;
    window.appearance = [NSAppearance appearanceNamed:appearanceName];

    // Defer to after AppKit's own layout pass
    dispatch_async(dispatch_get_main_queue(), ^{
        // Reposition traffic lights to center in our 36px topBar
        NSWindowButton types[3] = {
            NSWindowCloseButton,
            NSWindowMiniaturizeButton,
            NSWindowZoomButton
        };
        for (int i = 0; i < 3; i++) {
            NSButton* btn = [window standardWindowButton:types[i]];
            if (!btn) continue;
            NSRect f = btn.frame;
            f.origin.y -= 4;
            btn.frame = f;
        }
    });
}

}
