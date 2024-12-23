//
//  ContentView.swift
//  BLEConneter
//
//  Created by 杰仔 on 2024/12/19.
//
import SwiftUI

struct ContentView: View {
    @StateObject private var bleManager = BLEManager()
    @State private var inputData: Int = 0 // 用于发送的数据
    @State private var sidebarExpanded: Bool = true  // 控制边栏是否展开
    @State private var showSidebarButton: Bool = true // 控制边栏按钮的显示状态

    var body: some View {
        VStack(spacing: 0) {
            // 顶栏部分
            HStack {
                // 左侧：收起边栏的按钮
                Button(action: {
                    sidebarExpanded.toggle()  // 切换边栏的展开/收起
                }) {
                    Rectangle()
                        .fill(Color.blue)
                        .frame(width: 40, height: 40)  //设置按钮为一个蓝色的正方形块
                        .background(Color.clear)
                        .cornerRadius(10)
                        
                    
                }
                .buttonStyle(PlainButtonStyle())
                .padding(.leading, 10)
                
                // 中间部分：已连接设备的名称
                if let connectedDeviceName = bleManager.connectedPeripheral?.name {
                    Text("Data from \(connectedDeviceName)")
                        .font(.headline)
                        .padding()
                        .frame(maxWidth: .infinity, alignment: .center)
                }
                
                Spacer()
            }
            .frame(height: 50)  // 顶栏的高度
            .background(Color.gray.opacity(0.2))  // 顶栏的背景色
            .padding(.bottom, 5)
            
            // 主内容部分，包含边栏和数据展示区域
            HStack(spacing: 0) {
                // 左侧：扫描设备的边栏部分
                if sidebarExpanded {
                    VStack {
                        List(bleManager.deviceNames, id: \.self) { deviceName in
                            Button(action: {
                                if let index = bleManager.deviceNames.firstIndex(of: deviceName) {
                                    bleManager.connectToDevice(at: index)
                                }
                            }) {
                                Text("Connect to \(deviceName)")
                                    .padding()
                                    .frame(maxWidth: .infinity)
                                    .background(Color.blue)
                                    .foregroundColor(.white)
                                    .cornerRadius(10)
                            }
                            .buttonStyle(PlainButtonStyle())
                        }
                        .frame(width: 300)  // 左侧边栏宽度
                        .background(Color.gray)
                        .border(Color.clear)
                    }
                    
                    
                }
                
                // 右侧：数据接收和输入部分
                VStack {
                    // 显示接收到的数据
                    // 在 ContentView 中修改显示接收数据的部分
                    ScrollView {
                        Text("Received: \(bleManager.receivedData)")  // 直接显示数字
                            .padding()
                            .frame(maxWidth: .infinity, alignment: .leading)
                    }
                    .frame(maxHeight: .infinity)  // 占据右侧上半部分
                    
                    // 下半部分：输入框和发送按钮
                    VStack {
                        TextField("Enter integer", value: $inputData, formatter: NumberFormatter())
                                .padding()
                                .textFieldStyle(RoundedBorderTextFieldStyle())
                        
                        Button(action: {
                            // 发送数据到已连接设备
                            bleManager.sendData(inputData)
                            inputData = 0  // 清空输入框
                        }) {
                            Text("Send")
                                .padding()
                                .frame(maxWidth: .infinity)
                                .background(Color.green)
                                .foregroundColor(.white)
                                .cornerRadius(10)
                        }
                        .buttonStyle(PlainButtonStyle())
                        .padding(.top)
                    }
                    .frame(height: 150)  // 下半部分高度
                }
                .frame(maxWidth: .infinity)
            }
        }
        .frame(width: 900, height: 600)  // 主页面尺寸
    }
}

struct ContentView_Previews: PreviewProvider {
    static var previews: some View {
        ContentView()
    }
}
