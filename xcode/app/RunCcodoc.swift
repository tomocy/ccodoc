// tomocy

import Foundation

class RunCcodoc: Thread {
    typealias Handler = () -> Void

    init(completion: @escaping Handler) {
        super.init()
        self.completion = completion
    }

    override func start() {
        super.start()
        NSLog("ccodoc: Started")
    }

    override func cancel() {
        do {
            do {
                var error: NSDictionary?
                Self.stopScript.executeAndReturnError(&error)
                if error != nil {
                    return
                }
            }
        }

        NSLog("ccodoc: Canceled")

        super.cancel()
    }

    override func main() {
        do {
            var error: NSDictionary?
            Self.startScript.executeAndReturnError(&error)
            if error != nil {
                return
            }
        }

        NSLog("ccodoc: Completed")
        completion?()
    }

    private var completion: Handler?
}

extension RunCcodoc {
    private static let name = "ccodoc"

    private static var startScript: NSAppleScript {
        let executablePath = Bundle.main.path(forAuxiliaryExecutable: "\(name).command")!

        let script = NSAppleScript.init(
            source: """
            tell application "Terminal"
                activate
                set currentTab to do script "'\(executablePath)'"
                set custom title of currentTab to "\(name)"

                repeat
                    delay 0.5
                    try
                        if not busy of currentTab then exit repeat
                    on error
                        exit repeat
                    end try
                end repeat

                do script "exit" in currentTab
            end tell
            """
        )!

        do {
            var error: NSDictionary?
            let compiled = script.compileAndReturnError(&error)
            if let error = error {
                NSLog("error: \(error)")
                assert(compiled)
            }
        }

        return script
    }

    private static var stopScript: NSAppleScript {
        let script = NSAppleScript.init(
            source: """
            tell application "Terminal"
                set myWindows to my filterListByClass(windows as list, window)
                repeat with myWindow in myWindows
                    set myTabs to my filterListByClass((tabs of myWindow) as list, tab)
                    repeat with myTab in myTabs
                        if custom title of myTab = "\(name)" then
                            quit
                        end if
                    end repeat
                end repeat
            end tell

            on filterListByClass(values, key)
                set filtered to {}

                repeat with v in values
                    if class of v = key
                        set end of filtered to v
                    end if
                end repeat

                return filtered
            end filterListByClass
            """
        )!

        do {
            var error: NSDictionary?
            let compiled = script.compileAndReturnError(&error)
            if let error = error {
                NSLog("error: \(error)")
                assert(compiled)
            }
        }

        return script
    }
}
