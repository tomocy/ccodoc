// tomocy

import Foundation

struct AppleScript {
    typealias Raw = NSAppleScript

    let raw: Raw
}

extension AppleScript {
    init(source: String) {
        self.init(
            raw: .init(source: source)!
        )
    }
}

extension AppleScript {
    static func result(from: () throws -> Void) -> Result<Void, Error> {
        return Result {
            try from()
        }.mapError { $0 as! Error }
    }
}

extension AppleScript {
    func compile() throws {
        var error: NSDictionary?
        raw.compileAndReturnError(&error)
        if let error = error {
            throw Error(reason: error as! [String: Any])
        }
    }

    func run() throws {
        var error: NSDictionary?
        raw.executeAndReturnError(&error)
        if let error = error {
            throw Error(reason: error as! [String: Any])
        }
    }
}

extension AppleScript {
    struct Error: Swift.Error {
        let reason: [String: Any]
    }
}
