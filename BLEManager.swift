//
//  BLEConneter.swift
//  BLEConneter
//
//  Created by 杰仔 on 2024/12/19.
//
import Foundation
import CoreBluetooth

class BLEManager: NSObject, ObservableObject, CBCentralManagerDelegate, CBPeripheralDelegate {
    
    private var centralManager: CBCentralManager?
    private var peripherals: [CBPeripheral] = []
    // 或者公开它，便于在外部访问
    public var connectedPeripheral: CBPeripheral? // 公开访问权限

    
    @Published var deviceNames: [String] = []  // 存储扫描到的设备名称
    @Published var receivedData: String = ""   // 用于显示接收到的数据
    @Published var isConnected: Bool = false
    
    private var characteristic: CBCharacteristic?
    
    override init() {
        super.init()
        // 在这里初始化并设置委托
        centralManager = CBCentralManager(delegate: self, queue: nil)
    }
    
    // CBCentralManagerDelegate：检查蓝牙状态
    func centralManagerDidUpdateState(_ central: CBCentralManager) {
        if central.state == .poweredOn {
            centralManager?.scanForPeripherals(withServices: nil, options: nil)
        } else {
            print("Bluetooth is not powered on")
        }
    }
    
    // 扫描到设备时的回调
    func centralManager(_ central: CBCentralManager, didDiscover peripheral: CBPeripheral, advertisementData: [String : Any], rssi: NSNumber) {
        if let peripheralName = peripheral.name, !deviceNames.contains(peripheralName) {
            deviceNames.append(peripheralName)
            peripherals.append(peripheral)
        }
    }
    
    // 点击按钮连接设备
    func connectToDevice(at index: Int) {
        let peripheral = peripherals[index]
        centralManager?.connect(peripheral, options: nil)
    }
    
    // 连接成功时的回调
    func centralManager(_ central: CBCentralManager, didConnect peripheral: CBPeripheral) {
        print("Connected to peripheral: \(peripheral.name ?? "Unknown")")
        connectedPeripheral = peripheral
        peripheral.delegate = self
        peripheral.discoverServices(nil)  // 探索服务
        isConnected = true
    }
    
    // 连接失败时的回调
    func centralManager(_ central: CBCentralManager, didFailToConnect peripheral: CBPeripheral, error: Error?) {
        print("Failed to connect to peripheral: \(peripheral.name ?? "Unknown")")
        isConnected = false
    }
    
    // 断开连接时的回调
    func centralManager(_ central: CBCentralManager, didDisconnectPeripheral peripheral: CBPeripheral, error: Error?) {
        print("Disconnected from peripheral: \(peripheral.name ?? "Unknown")")
        isConnected = false
    }
    
    // 服务发现后的回调
    func peripheral(_ peripheral: CBPeripheral, didDiscoverServices error: Error?) {
        guard error == nil else { return }
        for service in peripheral.services ?? [] {
            peripheral.discoverCharacteristics(nil, for: service)  // 探索特征
        }
    }
    
    
    
    // 特征发现后的回调
    func peripheral(_ peripheral: CBPeripheral, didDiscoverCharacteristicsFor service: CBService, error: Error?) {
        guard error == nil else { return }
        for characteristic in service.characteristics ?? [] {
            if characteristic.properties.contains(.notify) {
                peripheral.setNotifyValue(true, for: characteristic)  // 添加这行来订阅通知
            }
            if characteristic.properties.contains(.write) {
                self.characteristic = characteristic
            }
        }
    }
    
    
    
    // 在数据接收的回调中添加日志
    // 在 BLEManager 中
    func peripheral(_ peripheral: CBPeripheral, didUpdateValueFor characteristic: CBCharacteristic, error: Error?) {
        if let error = error {
            print("Error receiving data: \(error)")
            return
        }
        
        guard let value = characteristic.value else {
            print("No data received")
            return
        }
        
        print("Received raw data: \(value.map { String(format: "%02X", $0) }.joined())")
        
        // 直接从数据中获取整数值
        let receivedValue = value[0]  // 获取第一个字节
        print("Received value: \(receivedValue)")
        
        DispatchQueue.main.async {
            self.receivedData = String(receivedValue)  // 转换成字符串显示
        }
    }
    
    
    func sendData(_ data: Int) {
        guard let peripheral = connectedPeripheral, let characteristic = characteristic else {
            print("No peripheral or characteristic available")
            return
        }
        
        // 转换为1字节的数据
        let value: UInt8 = UInt8(data & 0xFF)
        let data = Data([value])
        
        peripheral.writeValue(data, for: characteristic, type: .withResponse)
        print("Sending data: \(data.map { String(format: "%02X", $0) }.joined())")
    }
    
}
