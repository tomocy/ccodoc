// tomocy

import Cocoa

class AppDelegate: NSObject {
    var runCcodoc: RunCcodoc?
}

extension AppDelegate: NSApplicationDelegate {
    func applicationDidFinishLaunching(_ notification: Notification) {
        NSLog("App: Launched")

        runCcodoc = .init(
            completion: { [weak self] in
                Task { @MainActor in
                    NSApplication.shared.terminate(self)
                }
            }
        )
        runCcodoc!.start()
    }

    func applicationWillTerminate(_ notification: Notification) {
        defer { NSLog("App: Terminate") }

        guard let runCcodoc = runCcodoc else { return }
        guard runCcodoc.isExecuting else { return }

        runCcodoc.cancel()
    }
}

class AppMenu: NSMenu {
    required init(coder: NSCoder) { super.init(coder: coder) }

    override init(title: String) {
        super.init(title: title)

        do {
            let item = NSMenuItem.init()
            item.submenu = NSMenu.init()

            item.submenu!.items.append(
                NSMenuItem(
                    title: "Quit",
                    action: #selector(NSApplication.shared.terminate(_:)),
                    keyEquivalent: "q"
                )
            )

            addItem(item)
        }
    }
}
