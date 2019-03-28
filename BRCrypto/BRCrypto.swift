//
//  BRCrypto.swift
//  BRCrypto
//
//  Created by Ed Gamble on 3/27/19.
//  Copyright © 2019 breadwallet. All rights reserved.
//

import Foundation
import BRCore
import BRCore.Ethereum


// A currency is a medium for exchange.
///
/// Each currency has a `baseUnit` and a `defaultUnit`.  Because a `Unit` holds a Currency this
/// sets up recursive definition for both `Currency` and `Unit` - thus these abstractions must be
/// represented as reference types using `class`
///
/// A Currency has a number of decimals which define the currency's defaultUnit relative to the
/// baseUnit.
///
public final class Currency: Hashable {

    /// A 'Unique Identifier
    internal let uids: String

    /// The code; e.g. BTC
    public let code: String

    /// The name; e.g. Bitcoin
    public let name: String

    /// The type:
    public let type: String

    internal init (uids: String,
                   name: String,
                   code: String,
                   type: String) {
        self.uids = uids
        self.code = code
        self.name = name
        self.type = type
    }

    public static func == (lhs: Currency, rhs: Currency) -> Bool {
        return lhs === rhs || lhs.uids == rhs.uids
    }

    public func hash (into hasher: inout Hasher) {
        hasher.combine (uids)
    }
}


///
/// A unit of measure for a currency.  There can be multiple units for a given currency (analogous
/// to 'System International' units of (meters, kilometers, miles, ...) for a dimension of
/// 'length').  For example, Ethereum has units of: WEI, GWEI, ETHER, METHER, ... and Bitcoin of:
/// BTC, SATOSHI, ...
///
/// Each Currency has a 'baseUnit' - which is defined as the 'integer-ish' unit - such as SATOSHI
/// ane WEI for Bitcoin and Ethereum, respectively.  There can be multiple 'derivedUnits' - which
/// are derived by scaling off of a baseUnit.
///
public final class Unit: Hashable {
     internal let uids: String

    public let currency: Currency

    public let name: String;

    public let symbol: String

    public private(set) unowned var base: Unit! = nil // unowned?

    public let decimals: UInt8

    public func isCompatible (with that: Unit) -> Bool {
        return self.currency == that.currency
    }

    public func hasCurrency (_ currency: Currency) -> Bool {
        return self.currency == currency
    }

    internal convenience init (currency: Currency,
                               uids: String,
                               name: String,
                               symbol: String) {
        self.init (currency: currency,
                   uids: uids,
                   name: name,
                   symbol: symbol,
                   base: nil,
                   decimals: 0)
    }

    internal init (currency: Currency,
                   uids: String,
                   name: String,
                   symbol: String,
                   base: Unit?,
                   decimals: UInt8) {
        self.currency = currency
        self.uids = uids
        self.name = name
        self.symbol = symbol
        self.decimals = decimals
        self.base = (nil == base ? self : base!)

    }

    public static func == (lhs: Unit, rhs: Unit) -> Bool {
        return lhs === rhs || lhs.uids == rhs.uids
    }

    public func hash (into hasher: inout Hasher) {
        hasher.combine (uids)
    }
}

///
/// An amount of currency.  This can be negative (as in, 'currency owed' rather then 'currency
/// owned').  Supports basic arithmetic operations (addition, subtraction, comparison); will
/// assert on !isCompatible for mismatched currency.
///
public final class Amount {

    // Value in the currency's base unit
    internal let value: UInt256

    // Sign
    public let isNegative: Bool

    /// The (default) unit.  Without this there is no reasonable implementation of
    /// CustomeStringConvertable.
    public let unit: Unit

    /// The currency
    public var currency: Currency {
        return unit.currency
    }

    internal func double (as unit: Unit) -> Double? {
        var overflow: Int32 = 0
        let result = coerceDouble(value, &overflow)
        return 1 == overflow ? nil : (isNegative ? -result : result)
    }

    public func string (as unit: Unit) -> String? {
        return double (as: unit)
            .flatMap { self.formatterWith (unit: unit).string(from: NSNumber(value: $0)) }
    }

    public func string (pair: CurrencyPair) -> String? {
        return pair.exchange (asBase: self)?.string (as: pair.quoteUnit)
    }

    public func isCompatible (with that: Amount) -> Bool {
        return self.currency == that.currency
    }

    public func hasCurrency (_ currency: Currency) -> Bool {
        return self.currency == currency
    }

    public func add (_ that: Amount) -> Amount? {
        precondition (isCompatible(with: that))
        var overflow: Int32 = 0
        let result = addUInt256_Overflow(self.value, that.value, &overflow)
        return 1 == overflow ? nil : Amount (value: result, isNegative: false, unit: unit)
    }

    public func sub (_ that: Amount) -> Amount? {
        precondition (isCompatible(with: that))

        var negative: Int32 = 0
        var overflow: Int32 = 0
        var value: UInt256

        if self.isNegative && !that.isNegative {
            // (-x) - y = - (x + y)
            value = addUInt256_Overflow (self.value, that.value, &overflow);
            if 1 == overflow { return nil }
            negative = 1
        }
        else if !self.isNegative && that.isNegative {
            // x - (-y) = x + y
            value = addUInt256_Overflow (self.value, that.value, &overflow);
            if 1 == overflow { return nil }
        }
        else if self.isNegative && that.isNegative {
            // (-x) - (-y) = y - x
            value = subUInt256_Negative (that.value, self.value, &negative);
        }
        else {
            // x - y
            value = subUInt256_Negative (self.value, that.value, &negative);
        }

        return Amount (value: value,
                       isNegative: 1 == negative,
                       unit: unit)
    }

    internal init (value: UInt256,
                   isNegative: Bool,
                   unit: Unit) {
        self.value = value
        self.isNegative = isNegative
        self.unit = unit
    }

    static func create (double value: Double, unit: Unit) -> Amount? {
        let v: Double = fabs(value) * pow (10.0, Double(unit.decimals))
        return (v > Double(INT64_MAX)
            ? nil
            : Amount (value: createUInt256(UInt64(v)),
                      isNegative: value < 0,
                      unit: unit))
    }

    static func create (integer value: Int64, unit: Unit) -> Amount? {
        var powOverflow: Int32 = 0
        var mulOverflow: Int32 = 0

        let v = createUInt256 (value < 0 ? UInt64(-value) : UInt64(value))
        let result = (0 == unit.decimals
            ? v
            : mulUInt256_Overflow(v, createUInt256Power(unit.decimals, &powOverflow), &mulOverflow))

        return (1 == powOverflow || 1 == mulOverflow
            ? nil
            : Amount (value: result,
                      isNegative: value < 0,
                      unit: unit))
    }

    // static func create (exactly: Double, unit: Unit) -> Amount  ==> No remainder
    //   nil == Amount.create (exactly: 1.5, unit: SATOSHI)  // remainder is 0.5

    private func formatterWith (unit: Unit) -> NumberFormatter {
        let formatter = NumberFormatter()
        formatter.locale = Locale.current
        formatter.numberStyle = .currency
        formatter.currencySymbol = unit.symbol;
        formatter.generatesDecimalNumbers = 0 != unit.decimals
        formatter.maximumFractionDigits = Int(unit.decimals)
        return formatter
    }
}

extension Amount {
    public static func + (lhs: Amount, rhs: Amount) -> Amount? {
        return lhs.add (rhs)
    }

    public static func - (lhs: Amount, rhs: Amount) -> Amount? {
        return lhs.sub (rhs)
    }
}

///
/// Note that incompatible units may return 'false' for all comparisons.  This violates the
/// expectation that `lhs` and `rhs` satisfy one of: ==, >, and <.  Caution.
///
extension Amount: Comparable {
    internal static func compare (_ lhs: Amount, _ rhs:Amount) -> Int32 {
        precondition (lhs.isCompatible(with: rhs))

        if       lhs.isNegative && !rhs.isNegative { return -1 }
        else if !lhs.isNegative &&  rhs.isNegative { return +1 }
        else if  lhs.isNegative &&  rhs.isNegative {
            // both negative -> swap comparison
            return compareUInt256 (rhs.value, lhs.value)
        }
        else {
            // both positive -> same comparison
            return compareUInt256(lhs.value, rhs.value)
        }
    }

    public static func == (lhs: Amount, rhs: Amount) -> Bool {
        return lhs.isNegative == rhs.isNegative && 1 == eqUInt256(lhs.value, rhs.value)
    }

    public static func < (lhs: Amount, rhs: Amount) -> Bool {
        return -1 == compare (lhs, rhs)
    }

    public static func > (lhs: Amount, rhs: Amount) -> Bool {
        return +1 == compare (lhs, rhs)
    }

    public static func != (lhs: Amount, rhs: Amount) -> Bool {
        return 0 != compare (lhs, rhs)
    }

    public static func <= (lhs: Amount, rhs: Amount) -> Bool {
        return +1 != compare (lhs, rhs)
    }

    public static func >= (lhs: Amount, rhs: Amount) -> Bool {
        return -1 != compare (lhs, rhs)
    }
}

extension Amount: CustomStringConvertible {
    public var description: String {
        return string (as: unit) ?? "<nan>"
    }
}

///
/// "A currency pair is the quotation of the relative value of a currency unit against the unit of
/// another currency in the foreign exchange market. The currency that is used as the reference is
/// called the counter currency, quote currency or currency and the currency that is quoted in
/// relation is called the base currency or transaction currency.
///
/// "The quotation EUR/USD 1.2500 means that one euro is exchanged for 1.2500 US dollars. Here, EUR
/// is the base currency and USD is the quote currency(counter currency)."
///
/// Ref: https://en.wikipedia.org/wiki/Currency_pair
///
/// Thus BTC/USD=1000 means that one BTC is changed for $1,000.  Here, BTC is the base currency
/// and USD is the quote currency.  You would create such an exchange with:
///
///    let BTC_USD_Pair = CurrencyPair (baseUnit:  Bitcoin.Units.BTC,
///                                     quoteUnit: Fiat.USD.Dollar,
///                                     exchangeRate: 1000.0)
///
/// and then use it to find the value of 2 BTC with:
///
///    BTC_USD_Pair.exchange (asBase: Amount (value: 2.0, unit: Bitcoin.Units.BTC))
///
/// which would return: $2,000  (as Amount of 2000.0 in Fiat.USD.Dollar)
///

//public struct CurrencyPair {
//    public func exchange (asBase amount: Amount) -> Amount? {
//        return nil
//    }
//}

public struct CurrencyPair {

    /// In EUR/USD=1.2500, the `baseCurrecny` is EUR.
    public let baseUnit: Unit

    /// In EUR/USD=1.250, the `quoteCurrecny` is USD.
    public let quoteUnit: Unit

    /// In EUR/USD=1.2500, the `exchangeRate` is 1.2500 which represents the number of USD that
    /// one EUR can be exchanged for.
    public let exchangeRate: Double

    ///
    /// Apply `self` CurrencyPair to convert `asBase` (in `baseCurrency`) to `quoteCurrency`.  This
    /// is essentially `asBase * exchangeRate`
    ///
    /// - Parameter amount: the amount of `baseCurrency`
    ///
    /// - Returns: the amount as `quoteCurrency`
    ///
    public func exchange(asBase amount: Amount) -> Amount? {
        return amount.double(as: baseUnit)
            .flatMap { Amount.create (double: $0 * exchangeRate, unit: quoteUnit) }
    }

    ///
    /// Apply `self` CurrencyPair to convert `asQuote` (in `quoteCurrency`) to `baseCurrency`.  This
    /// is essentially `asQuote / exchangeRate`.
    ///
    /// - Parameter amount: the amount of `quoteCurrency`
    ///
    /// - Returns: the amount as `baseCurrency`
    ///
    public func exchange(asQuote amount: Amount) -> Amount? {
        return amount.double (as: quoteUnit)
            .flatMap { Amount.create (double: $0 / exchangeRate, unit: baseUnit) }
    }
}

extension CurrencyPair: CustomStringConvertible {
    public var description: String {
        return "\(baseUnit.name)/\(quoteUnit.name)=\(exchangeRate)"
    }
}


public final class Account {
    /// The Bitcoin masterPublicKey
    internal let masterPublicKey: BRMasterPubKey

    /// The Ethereum account
    internal let ethereumAccount: BREthereumAccount

    internal init (seed: UInt512) {
        var seed = seed
        self.masterPublicKey = BRBIP32MasterPubKey (&seed, MemoryLayout<UInt512>.size)
        self.ethereumAccount = createAccountWithBIP32Seed (seed)
    }

    ///
    /// Initialize an Account from a seed.  The seed is not stored, only publically-accessible
    /// values, derived from the seed, are stored.
    ///
    /// - Parameter seed: The UInt512 seed
    ///
   static func createFrom (seed: UInt512) -> Account { // Data
        return Account (seed: seed)
    }

    ///
    /// Derive a 'seed' from a paperKey phrase.  Used when signing (Bitcoin) transactions.
    ///
    /// - Parameter phrase: The PaperKey
    /// - Returns: The UInt512 seed.
    ///
    static func deriveSeed (phrase: String) -> UInt512 { // Data
        var seed: UInt512 = zeroUInt512()
        BRBIP39DeriveKey (&seed.u8, phrase, nil); // no passphrase
        return seed
    }

    // Install words
}

extension Account {
    var timestamp: UInt32 {
        return 0
    }
}

///
/// A Blockchain Network.  Networks are created based from a cross-product of block chain and
/// network type.  Specifically {BTC, BCH, ETH, ...} x {Mainnet, Testnet, ...}.  Thus there will
/// be networks of [BTC-Mainnet, BTC-Testnet, ..., ETH-Mainnet, ETH-Testnet, ETH-Rinkeby, ...]
///
public protocol Network: CustomStringConvertible {
    /// The name
    var name: String { get }

    /// If 'mainnet' then true, otherwise false
    var isMainnet: Bool { get }

    /// The native currency.  Multiple networks will have the same currency; for example,
    /// BTC-Mainnet and BTC-Testnet share the BTC currency.
     var currency: Currency { get }

    /// All currencies.  Multiple networks will have the same currencies.
    var currencies: Set<Currency> { get }

    func currencyBy (code: String) -> Currency?

    ///
    /// if `currency` is in `currencies` then true; otherwise false
    ///
    /// - Parameter that: the currency
    ///
    /// - Returns:
    ///
    func hasCurrency (_ that: Currency) -> Bool

    func baseUnitFor (currency: Currency) -> Unit?

    func defaultUnitFor (currency: Currency) -> Unit?

    func unitsFor (currency: Currency) -> Set<Unit>?

    func hasUnitFor (currency: Currency, unit: Unit) -> Bool?

    // address schemes
}

extension Network {
    public var description: String {
        return name
    }
}

public enum NetworkEvent {
    case created
}

///
/// An Address for transferring an amount.
///
/// - bitcoin: A bitcon-specific address
/// - ethereum: An ethereum-specific address
///
public protocol Address: CustomStringConvertible {

    ///
    /// Create an Addres from `string` and `network`.  The provided `string` must be valid for
    /// the provided `network` - that is, an ETH address (as a string) differs from a BTC address
    /// and a BTC mainnet address differs from a BTC testnet address.
    ///
    /// In practice, 'target' addresses (for receiving crypto) are generated from the wallet and
    /// 'source' addresses (for sending crypto) are a User input.
    ///
    /// - Parameters:
    ///   - string: A string representing a crypto address
    ///   - network: The network for which the string is value
    ///
    /// - Returns: An address or nil if `string` is invalide for `network`
    ///
    static func create (string: String, network: Network) -> Address?
}

///
/// An AddressScheme generates addresses for a wallet.  Depending on the scheme, a given wallet may
/// generate different address.  For example, a Bitcoin wallet can have a 'Segwit/BECH32' address
/// scheme or a 'Legacy' address scheme.
///
public protocol AddressScheme {
//    associatedtype W: Wallet

    // Generate a 'receive' (aka target') address for wallet.
    func getAddress (for wallet: Wallet) -> Address
}

///
/// A Transfer represents the transfer of an `amount` of currency from `source` to `target`.  A
/// Transfer is held in a `Wallet` (holding the amount's currency); the Transfer requires a `fee`
/// to complete.  Once the transfer is signed/submitted it can be identified by a `TransferHash`.
/// Once the transfer has been included in the currency's blockchain it will have a
/// `TransferConfirmation`.
///
public protocol Transfer : class {

    /// The owning wallet
    var wallet: Wallet { get }

    /// The source pays the fee and sends the amount.
    var source: Address? { get }

    /// The target receives the amount
    var target: Address? { get }

    /// The amount to transfer
    var amount: Amount { get }

    /// The fee paid - before the transfer is confirmed, this is the estimated fee.
    var fee: Amount { get }

    /// The basis for the fee.
    var feeBasis: TransferFeeBasis { get }

    /// An optional confirmation.
    var confirmation: TransferConfirmation? { get }

    /// An optional hash
    var hash: TransferHash? { get }

    /// The current state
    var state: TransferState { get }

    var isSent: Bool { get }
    // var originator: Bool { get }
}

extension Transfer {
    public var confirmation: TransferConfirmation? {
        if case .included (let confirmation) = state { return confirmation }
        else { return nil }
    }
}

///
/// A TransferFeeBasis is use to estimate the fee to complete a transfer
///
public enum TransferFeeBasis {
    case bitcoin  (feePerKB: UInt64) // in satoshi
    case ethereum (gasPrice: Amount, gasLimit: UInt64)
}

///
/// A TransferConfirmation holds confirmation information.
///
public struct TransferConfirmation {
    public let blockNumber: UInt64
    public let transactionIndex: UInt64
    public let timestamp: UInt64
    public let fee: Amount
}

///
/// A TransferHash uniquely identifies a transfer *among* the owning wallet's transfers.
///
public enum TransferHash: Hashable, CustomStringConvertible {
    case bitcoin (UInt256)
    case ethereum (BREthereumHash)

    public func hash (into hasher: inout Hasher) {
        switch self {
        case .bitcoin (let core):
            hasher.combine (core.u32.0)
            break
        case .ethereum (var core):
            hasher.combine (Int(hashSetValue(&core)))
            break
        }
    }

    public static func == (lhs: TransferHash, rhs: TransferHash) -> Bool {
        switch (lhs, rhs) {
        case (.bitcoin(let c1), .bitcoin(let c2)):
            return 1 == eqUInt256 (c1, c2)
        case (.ethereum(var c1), .ethereum(var c2)):
            return 1 == hashSetEqual(&c1, &c2)
        default:
            return false
        }
    }

    public var description: String {
        switch self {
        case .bitcoin (let core):
            return asUTF8String (u256HashToString(core), true)
        case .ethereum(let core):
            return asUTF8String (hashAsString(core))
        }
    }
}

///
/// A TransferState represents the states in Transfer's 'life-cycle'
///
public enum TransferState {
    case created
    case signed
    case submitted
    case pending
    case included (confirmation: TransferConfirmation)
    case failed (reason:String)
    case deleted
}

extension TransferState: CustomStringConvertible {
    public var description: String {
        switch self {
        case .created:   return "Created"
        case .signed:    return "Signed"
        case .submitted: return "Submitted"
        case .pending:   return "Pending"
        case .included:  return "Included"
        case .failed:    return "Failed"
        case .deleted:   return "Deleted"
        }
    }
}

/// A TransferEvent represents a asynchronous announcment of a transfer's state change.
///
public enum TransferEvent {
    case created
    case changed (old: TransferState, new: TransferState)
    case deleted
}

///
/// A `TransferFectory` is a customization point for `Transfer` creation.
///
public protocol TransferFactory {
    /// associatedtype T: Transfer

    ///
    /// Create a transfer in `wallet`
    ///
    /// - Parameters:
    ///   - target: The target receives 'amount'
    ///   - amount: The amount
    ///   - feeBasis: The basis for the 'fee'
    ///
    /// - Returns: A new transfer
    ///
    func createTransfer (wallet: Wallet,
                         target: Address,
                         amount: Amount,
                         feeBasis: TransferFeeBasis) -> Transfer? // T
}


///
/// A Wallet holds the transfers and a balance for a single currency.
///
public protocol Wallet: class {

    /// The owning manager
    var manager: WalletManager { get }

    /// The name
    var name: String { get }

    /// The base unit for the wallet's network.  This is used for `balance` and to derive the
    /// currency and name
    var unit: Unit { get }

    /// The current balance for currency
    var balance: Amount { get }

    /// The transfers of currency yielding `balance`
    var transfers: [Transfer] { get }

    /// Use a hash to lookup a transfer
    func lookup (transfer: TransferHash) -> Transfer?

    /// The current state.
    var state: WalletState { get }

    /// The default TransferFeeBasis for created transfers.
    var defaultFeeBasis: TransferFeeBasis { get set }

    /// The default TransferFactory for creating transfers.
    var transferFactory: TransferFactory { get set }

    /// An address suitable for a transfer target (receiving).  Uses the default Address Scheme
    var target: Address { get }

    /// An address suitable for a transfer source (sending).  Uses the default AddressScheme
    var source: Address { get }

    // address scheme
}

extension Wallet {
    ///
    /// Create a transfer for wallet.  Invokes the wallet's transferFactory to create a transfer.
    /// Generates events: TransferEvent.created and WalletEvent.transferAdded(transfer).
    ///
    /// - Parameters:
    ///   - source: The source spends 'amount + fee'
    ///   - target: The target receives 'amount
    ///   - amount: The amount
    ///   - feeBasis: Teh basis for 'fee'
    ///
    /// - Returns: A new transfer
    ///
    public func createTransfer (target: Address,
                                amount: Amount,
                                feeBasis: TransferFeeBasis) -> Transfer? {
        return transferFactory.createTransfer (wallet: self,
                                               target: target,
                                               amount: amount,
                                               feeBasis: feeBasis)
    }

    ///
    /// Create a transfer for wallet using the `defaultFeeBasis`.  Invokes the wallet's
    /// `transferFactory` to create a transfer.  Generates events: TransferEvent.created and
    /// WalletEvent.transferAdded(transfer).
    ///
    /// - Parameters:
    ///   - source: The source spends 'amount + fee'
    ///   - target: The target receives 'amount'
    ///   - amount: The amouunt
    ///
    /// - Returns: A new transfer
    ///
    public func createTransfer (target: Address,
                                amount: Amount) -> Transfer? {
        return createTransfer (target: target,
                               amount: amount,
                               feeBasis: defaultFeeBasis)
    }

    /// The currency held in wallet.
    public var currency: Currency {
        return unit.currency
    }

    /// The (default) name derived from the currency.  For example: BTC, ETH, or BRD.
    public var name: String {
        return unit.currency.code
    }
}

///
/// The Wallet state
///
/// - created: The wallet was created (and remains in existence).
/// - deleted: The wallet was deleted.
///
public enum WalletState {
    case created
    case deleted
}

///
/// A WalletEvent represents a asynchronous announcment of a wallet's state change.
///
public enum WalletEvent {
    case created
    case transferAdded   (transfer: Transfer)
    case transferChanged (transfer: Transfer)
    case transferDeleted (transfer: Transfer)
    case balanceUpdated  (amount: Amount)
    case feeBasisUpdated (feeBasis: TransferFeeBasis)
    case deleted
}

extension WalletEvent: CustomStringConvertible {
    public var description: String {
        switch self {
        case .created:         return "Created"
        case .transferAdded:   return "TransferAdded"
        case .transferChanged: return "TransferChanged"
        case .transferDeleted: return "TransferDeleted"
        case .balanceUpdated:  return "BalanceUpdated"
        case .feeBasisUpdated: return "FeeBasisUpdated"
        case .deleted:         return "Deleted"
        }
    }
}

///
/// A WalletFactory is a customization point for Wallet creation.
/// TODO: ?? AND HOW DOES THIS FIT WITH CoreWallet w/ REQUIRED INTERFACE TO Core ??
///
public protocol WalletFactory {
    ///
    /// Create a Wallet managed by `manager` and holding `currency`.  The wallet is initialized
    /// with no balance, no transfers and some default feeBasis (appropriate for the `currency`).
    /// Generates events: WalletEvent.created (and maybe others).
    ///
    /// - Parameters:
    ///   - manager: the Wallet's manager
    ///   - currency: The currency held
    ///
    /// - Returns: A new wallet
    ///
    func createWallet (manager: WalletManager,
                       currency: Currency) -> Wallet
}

///
/// A WallettManager manages one or more wallets one of which is designated the `primaryWallet`.
/// (For example, an EthereumWalletManager will manage an ETH wallet and one wallet for each
/// ERC20Token; the ETH wallet will be the primaryWallet.  A BitcoinWalletManager manages one
/// and only one wallet holding BTC.).
///
/// At least conceptually, a WalletManager is an 'Active Object' (whereas Transfer and Wallet are
/// 'Passive Objects'
///
public protocol WalletManager : class {
    /// The account
    var account: Account { get }

    /// The network
    var network: Network { get }

    /// The primaryWallet
    var primaryWallet: Wallet { get }

    /// The managed wallets - often will just be [primaryWallet]
    var wallets: [Wallet] { get }

    // The mode determines how the manager manages the account and wallets on network
    var mode: WalletManagerMode { get }

    // The file-system path to use for persistent storage.
    var path: String { get }  // persistent storage

    var state: WalletManagerState { get }

    /// The default WalletFactory for creating wallets.
    var walletFactory: WalletFactory { get set }

    /// Connect to network and begin managing wallets for account
    func connect ()

    /// Disconnect from the network.
    func disconnect ()

    /// isConnected
    /// sync(...)
    /// isSyncing

    func sign (transfer: Transfer, paperKey: String)

    func submit (transfer: Transfer)

    func sync ()
}


extension WalletManager {
    ///
    /// Create a wallet for `currency`.  Invokdes the manager's `walletFactory` to create the
    /// wallet.  Generates events: Wallet.created, WalletManager.walletAdded(wallet), perhaps
    /// others.
    ///
    /// - Parameter currency: the wallet's currency
    ///
    /// - Returns: a new wallet.
    ///
    func createWallet (currency: Currency) -> Wallet {
        return walletFactory.createWallet (manager: self,
                                           currency: currency)
    }

    /// The primaryWallet's currency.
    var currency: Currency {
        return primaryWallet.currency
    }

    /// A manager `isActive` if connected or syncing
    var isActive: Bool {
        return state == .connected || state == .syncing
    }

    func signAndSubmit (transfer: Transfer, paperKey: String) {
        sign (transfer: transfer, paperKey: paperKey)
        submit (transfer: transfer)
    }
}

///
/// The WalletManager state.
///
public enum WalletManagerState {
    case created
    case disconnected
    case connected
    case syncing
    case deleted
}

///
/// The WalletManager's mode determines how the account and associated wallets are managed.
///
/// - api_only: Use only the defined 'Cloud-Based API' to synchronize the account's transfers.
///
/// - api_with_p2p_submit: Use the defined 'Cloud-Based API' to synchronize the account's transfers
///      but submit transfers using the network's Peer-to-Peer protocol.
///
/// - p2p_with_api_sync: Use the network's Peer-to-Peer protocol to synchronize the account's
///      recents transfers but use the 'Cloud-Based API' to synchronize older transfers.
///
/// - p2p_only: Use the network's Peer-to-Peer protocol to synchronize the account's transfers.
///
public enum WalletManagerMode {
    case api_only
    case api_with_p2p_submit
    case p2p_with_api_sync
    case p2p_only
}

///
/// A WalletManager Event represents a asynchronous announcment of a managera's state change.
///
public enum WalletManagerEvent {
    case created
    case changed (oldState: WalletManagerState, newState: WalletManagerState)
    case deleted

    case walletAdded (wallet: Wallet)
    case walletChanged (wallet: Wallet)
    case walletDeleted (wallet: Wallet)

    case syncStarted
    case syncProgress (percentComplete: Double)
    case syncEnded (error: String?)
}

public enum SystemEvent {
    case created
    case networkAdded (network: Network)
    case managerAdded (manager: WalletManager)
}
///
/// A SystemListener recieves asynchronous events announcing state changes to Networks, to Managers,
/// to Wallets and to Transfers.  This is an application's sole mechanism to learn of asynchronous
/// state changes.
///
/// Note: This must be 'class bound' as System  hold a 'weak' reference (for GC reasons).
///
public protocol SystemListener : class {

    func handleSystemEvent (system: System,
                            event: SystemEvent)
    
    ///
    /// Handle a WalletManagerEvent.
    ///
    /// - Parameters:
    ///   - system: the system
    ///   - manager: the manager
    ///   - event: the event
    ///
    func handleManagerEvent (system: System,
                             manager: WalletManager,
                             event: WalletManagerEvent)

    ///
    /// Handle a WalletEvent
    ///
    /// - Parameters:
    ///   - system: the system
    ///   - manager: the manager
    ///   - wallet: the wallet
    ///   - event: the wallet event.
    ///
    func handleWalletEvent (system: System,
                            manager: WalletManager,
                            wallet: Wallet,
                            event: WalletEvent)

    ///
    /// Handle a TranferEvent.
    ///
    /// - Parameters:
    ///   - system: the system
    ///   - manager: the manager
    ///   - wallet: the wallet
    ///   - transfer: the transfer
    ///   - event: the transfer event.
    ///
    func handleTransferEvent (system: System,
                              manager: WalletManager,
                              wallet: Wallet,
                              transfer: Transfer,
                              event: TransferEvent)


    // TODO: handlePeerEvent ()
    // TODO: handleBlockEvent ()

    ///
    /// Handle a NetworkEvent
    ///
    /// - Parameters:
    ///   - system: the system
    ///   - network: the network
    ///   - event: the event
    ///
    func handleNetworkEvent (system: System,
                             network: Network,
                             event: NetworkEvent)



//    /// System
//
//    func handleSystemAddedNetwork (system: System,
//                                   network: Network)
//
//    func handleSystemAddedManager (system: System,
//                                   manager: WalletManager)
//
//    /// Network
//
//    // added currency
//
//    /// Manager
//
//    func handleManagerChangedState (system: System,
//                                    manager: WalletManager,
//                                    old: WalletManagerState,
//                                    new: WalletManagerState)
//
//    func handleManagerAddedWallet (system: System,
//                                   manager: WalletManager,
//                                   wallet: Wallet)
//    // changed wallet
//    // deleted wallet
//
//    func handleManagerStartedSync (system: System,
//                                   manager: WalletManager)
//
//    func handleManagerProgressedSync (system: System,
//                                      manager: WalletManager,
//                                      percentage: Double)
//
//    func handleManagerStoppedSync (system: System,
//                                   manager: WalletManager)
//
//    /// Wallet
//
//    func handleWalletAddedTransfer (system: System,
//                                    manager: WalletManager,
//                                    wallet: Wallet,
//                                    transfer: Transfer)
//
//    // changed transfer
//    // deleted transfer
//
//    func handleWalletUpdatedBalance (system: System,
//                                     manager: WalletManager,
//                                     wallet: Wallet)
//
//    func handleWalletUpdatedFeeBasis (system: System,
//                                      manager: WalletManager,
//                                      wallet: Wallet)
//
//    /// Transfer
//
//    func handleTransferChangedState (system: System,
//                                     manager: WalletManager,
//                                     wallet: Wallet,
//                                     transfer: Transfer,
//                                     old: TransferState,
//                                     new: TransferState)
}



/// Singleton
public protocol System {
    
    /// The listener.  Gets all events for {Network, WalletManger, Wallet, Transfer}
    var listener: SystemListener? { get }

    var account: Account { get }

    /// The path for persistent storage
    var path: String { get }

    /// The 'blockchain DB'
    var query: BlockChainDB { get }

    /// Networks
    var networks: [Network] { get }

    /// Wallet Managers
    var managers: [WalletManager] { get }

    func start ()

    func stop ()

    func createWalletManager (network: Network,
                              mode: WalletManagerMode)

    func createWallet (manager: WalletManager,
                       currency: Currency)
    
    static func create (listener: SystemListener,
                        account: Account,
                        path: String,
                        query: BlockChainDB) -> System
}

