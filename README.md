# DPDK User-Space Protocol Stack (my_ustack)

## 📌 项目简介
本项目是一个基于 C 语言和 DPDK (Data Plane Development Kit) 框架实现的**极简版用户态网络协议栈**。
通过接管 Linux 内核的网卡驱动，实现了网络数据包的 Kernel Bypass（内核旁路），在用户态直接对以太网帧、IPv4、UDP 和 TCP 报文进行解析与封装。

本项目主要用于学习高性能网络编程、零拷贝技术以及底层网络协议栈的工作原理。

## 🚀 核心功能
* **DPDK 环境初始化**：接管网卡，分配大页内存（Hugepages），建立 mbuf 内存池。
* **无中断轮询收发**：使用 `rte_eth_rx_burst` / `rte_eth_tx_burst` 进行高性能的包收发。
* **UDP 协议处理**：支持 UDP 报文的解析、信息提取，以及源/目的 MAC、IP、端口的逆向封装与回发。
* **TCP 协议处理**：支持 TCP 报文解析，包含一个极简的 TCP 状态机雏形（目前支持侦听与基础的握手标志位打印）。
* **自动化脚本支持**：提供 `init_dpdk.sh` 脚本，一键完成驱动加载、大页内存挂载和网卡绑定。

## 🛠️ 环境依赖
* **操作系统**：Ubuntu / Linux
* **开发语言**：C 语言
* **核心框架**：DPDK (当前基于稳定版 19.08.2)
* **编译器**：GCC & Make

## ⚙️ 快速开始

### 1. 初始化 DPDK 环境
**注意：必须在 Root 权限下运行脚本！**
```bash
# 赋予脚本执行权限
chmod +x init_dpdk.sh

# 一键挂载大页内存并绑定网卡至 igb_uio 驱动
sudo ./init_dpdk.sh
