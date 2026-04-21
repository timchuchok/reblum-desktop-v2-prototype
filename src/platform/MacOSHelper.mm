#include "MacOSHelper.h"
#import <AppKit/AppKit.h>
#import <QuartzCore/QuartzCore.h>

namespace MacOSHelper {

void setWindowBackground(quintptr winId, bool dark) {
    NSView* view = reinterpret_cast<NSView*>(winId);
    NSWindow* window = [view window];
    if (!window) return;
    NSColor* color = dark ? [NSColor blackColor] : [NSColor whiteColor];
    window.backgroundColor = color;
    window.contentView.wantsLayer = YES;
    window.contentView.layer.backgroundColor = color.CGColor;
}

void setViewBackground(quintptr viewId, bool dark) {
    NSView* view = reinterpret_cast<NSView*>(viewId);
    view.wantsLayer = YES;
    CGColorRef color = dark ? [NSColor blackColor].CGColor
                            : [NSColor whiteColor].CGColor;
    view.layer.backgroundColor = color;
    for (CALayer* sub in view.layer.sublayers) {
        sub.backgroundColor = color;
    }
}

}
