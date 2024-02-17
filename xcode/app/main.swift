// tomocy

import Cocoa

let app = NSApplication.shared

app.setActivationPolicy(.regular)

let delegate = AppDelegate.init()
app.delegate = delegate

app.menu = AppMenu.init()

_ = NSApplicationMain(CommandLine.argc, CommandLine.unsafeArgv)
