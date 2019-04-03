//
//  AppDelegate.swift
//  CoreXDemo
//
//  Created by Ed Gamble on 11/8/18.
//  Copyright © 2018 Breadwallet AG. All rights reserved.
//
//  See the LICENSE file at the project root for license information.
//  See the CONTRIBUTORS file at the project root for a list of contributors.
//

import UIKit
import BRCrypto

protocol SharedSystem {
    static var sharedSystem: System { get }
}

extension UIApplication: SharedSystem {
    static var sharedSystem : System {
        return (UIApplication.shared.delegate as! CoreDemoAppDelegate).system
    }
}

@UIApplicationMain
class CoreDemoAppDelegate: UIResponder, UIApplicationDelegate, UISplitViewControllerDelegate {

    var window: UIWindow?
    var paperKey: String!

    var listener: CoreDemoListener!
    var system: System!

    func application(_ application: UIApplication, didFinishLaunchingWithOptions launchOptions: [UIApplication.LaunchOptionsKey: Any]?) -> Bool {
        // Override point for customization after application launch.
        let splitViewController = window!.rootViewController as! UISplitViewController

        let summaryNavigationController = splitViewController.viewControllers[0] as! UINavigationController
        let walletNavigationController  = splitViewController.viewControllers[1] as! UINavigationController

        walletNavigationController.topViewController!.navigationItem.leftBarButtonItem = splitViewController.displayModeButtonItem
        splitViewController.delegate = self

        let summaryController = summaryNavigationController.topViewController as! SummaryViewController

        paperKey = (CommandLine.argc > 1
            ? CommandLine.arguments[1]
            : "0xa9de3dbd7d561e67527bc1ecb025c59d53b9f7ef");
        //                                         paperKey: "0xb0F225defEc7625C6B5E43126bdDE398bD90eF62")
        //                                         paperKey: "0x8975dbc1b8f25ec994815626d070899dda896511")
        //                                         paperKey: "0xb302B06FDB1348915599D21BD54A06832637E5E8")

        let timestamp:UInt64 = 1543190400 // Tue, 26 Nov 2018 00:00:00 GMT

        guard let account = Account.createFrom(phrase: paperKey) else {
            precondition(false, "No account")
            return false
        }

        // Ensure the storage path
        let storagePath = FileManager.default
            .urls(for: .documentDirectory, in: .userDomainMask)[0]
            .appendingPathComponent("Core").path

        do {
            if FileManager.default.fileExists(atPath: storagePath) {
                try FileManager.default.removeItem(atPath: storagePath)
            }

            try FileManager.default.createDirectory (atPath: storagePath,
                                                     withIntermediateDirectories: true,
                                                     attributes: nil)
        }
        catch let error as NSError {
            print("Error: \(error.localizedDescription)")
        }

        NSLog ("StoragePath: \(storagePath)");

        // Create the listener
        let listener = CoreDemoListener ()

        // Create the BlockChainDB
        let query = BlockChainDB ()

        // Create the system
        self.listener = listener
        self.system = SystemBase.create (listener: listener,
                                         account: account,
                                         path: storagePath,
                                         query: query)

//        self.btcManager = BitcoinWalletManager (listener: listener,
//                                                account: account,
//                                                network: Bitcoin.Networks.testnet,
//                                                mode: WalletManagerMode.p2p_only,
//                                                timestamp: timestamp,
//                                                storagePath: storagePath)
//
//        self.bchManager = BitcoinWalletManager (listener: listener,
//                                                account: account,
//                                                network: Bitcash.Networks.testnet,
//                                                mode: WalletManagerMode.p2p_only,
//                                                timestamp: timestamp,
//                                                storagePath: storagePath)
//
//        self.ethManager = EthereumWalletManager (listener: listener,
//                                                 account: account,
//                                                 network: Ethereum.Networks.mainnet,
//                                                 mode: WalletManagerMode.p2p_only, //  api_with_p2p_submit,
//                                                 timestamp: 0,
//                                                 storagePath: storagePath)

 //       UIApplication.sharedListener.addWalletListener(listener: summaryController)

        self.system.start (networksNeeded: ["ethereum-mainnet"]);

        return true
    }

    func applicationWillResignActive(_ application: UIApplication) {
        // Sent when the application is about to move from active to inactive state. This can occur for certain types of temporary interruptions (such as an incoming phone call or SMS message) or when the user quits the application and it begins the transition to the background state.
        // Use this method to pause ongoing tasks, disable timers, and invalidate graphics rendering callbacks. Games should use this method to pause the game.
    }

    func applicationDidEnterBackground(_ application: UIApplication) {
        // Use this method to release shared resources, save user data, invalidate timers, and store enough application state information to restore your application to its current state in case it is terminated later.
        // If your application supports background execution, this method is called instead of applicationWillTerminate: when the user quits.
    }

    func applicationWillEnterForeground(_ application: UIApplication) {
        // Called as part of the transition from the background to the active state; here you can undo many of the changes made on entering the background.
    }

    func applicationDidBecomeActive(_ application: UIApplication) {
        // Restart any tasks that were paused (or not yet started) while the application was inactive. If the application was previously in the background, optionally refresh the user interface.
        DispatchQueue.global().async {
            sleep (5)
//            self.ethManager.updateTokens()
//            self.btcManager.connect();
//            self.bchManager.connect();
//            self.ethManager.connect();
        }
    }

    func applicationWillTerminate(_ application: UIApplication) {
        // Called when the application is about to terminate. Save data if appropriate. See also applicationDidEnterBackground:.
    }

    // MARK: - Split view

    func splitViewController(_ splitViewController: UISplitViewController, collapseSecondary secondaryViewController:UIViewController, onto primaryViewController:UIViewController) -> Bool {
        guard let secondaryAsNavController = secondaryViewController as? UINavigationController else { return false }
        guard let topAsDetailController = secondaryAsNavController.topViewController as? WalletViewController else { return false }
        if topAsDetailController.wallet == nil {
            // Return true to indicate that we have handled the collapse by doing nothing; the secondary controller will be discarded.
            return true
        }
        return false
    }

}


extension UIApplication {
    static var paperKey: String {
        return (UIApplication.shared.delegate as! CoreDemoAppDelegate).paperKey
    }

    static func sync () {
        guard let app = UIApplication.shared.delegate as? CoreDemoAppDelegate else { return }
        app.system.managers.forEach { $0.sync() }
    }

    static func sleep() {
        guard let app = UIApplication.shared.delegate as? CoreDemoAppDelegate else { return }
        NSLog ("ETH: Disconnecting")
        app.system.managers.forEach { $0.disconnect() }
        DispatchQueue.main.asyncAfter(deadline: .now() + 15.0) {
            NSLog ("ETH: Connecting")
            app.system.managers.forEach { $0.connect() }
        }
    }
}

