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

    override func main() {
        let result = AppleScript.result(from: Self.startScript.run)
        switch result {
        case .success:
            NSLog("ccodoc: Completed")
            completion?()
        case .failure(let error):
            NSLog("ccodoc: Error: {Reason: \(error)}")
        }
    }

    override func cancel() {
        let result = AppleScript.result(from: Self.stopScript.run)
        if case .failure(let error) = result {
            NSLog("ccodoc: Error: {Reason: \(error)}")
        }
        
        NSLog("ccodoc: Canceled")
        super.cancel()
    }

    private var completion: Handler?
}

extension RunCcodoc {
    private static let name = "ccodoc"
    
    private static var startScript: AppleScript {
        let executablePath = Bundle.main.path(forAuxiliaryExecutable: "\(name).command")!

        let script = AppleScript.init(
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
        )

        try! script.compile()

        return script
    }

    private static var stopScript: AppleScript {
        let script = AppleScript.init(
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
        )

        try! script.compile()

        return script
    }
}
