#!/bin/bash

# ==========================================
# DPDK 实验环境一键初始化脚本
# ==========================================

# 1. 定义路径和网卡变量
DPDK_HOME="/home/yuyang/share/backend/02_Net/2.4-dpdk/dpdk-stable-19.08.2"
PCI_ADDR="0000:0b:00.0"
NIC_NAME="ens192"

echo "🚀 开始初始化 DPDK 环境..."

# 2. 停用实验网卡 (绕过路由表保护机制)
echo "   -> 正在 down 掉网卡 ${NIC_NAME}..."
ifconfig ${NIC_NAME} down 2>/dev/null

# 3. 加载 UIO 和 DPDK 底层驱动
echo "   -> 正在加载 igb_uio 驱动..."
modprobe uio
# 如果已经加载过，先卸载掉避免报错
rmmod igb_uio 2>/dev/null
insmod ${DPDK_HOME}/x86_64-native-linux-gcc/kmod/igb_uio.ko

# 4. 分配 2GB 的大页内存 (使用 2MB 标准页)
echo "   -> 正在分配 2GB 大页内存..."
# 写入 1024 个 2MB 大页 (共计 2GB)
echo 1024 > /sys/kernel/mm/hugepages/hugepages-2048kB/nr_hugepages

# 建立挂载点
mkdir -p /mnt/huge

# 强力卸载可能残留的挂载，防止资源占用报错 (-l 参数极度关键)
umount -l /mnt/huge 2>/dev/null

# 强制按照 2MB 尺寸挂载大页内存系统 (精准对齐暗号)
mount -t hugetlbfs -o pagesize=2M nodev /mnt/huge

# 5. 绑定网卡
echo "   -> 正在将 ${PCI_ADDR} 绑定至 igb_uio..."
${DPDK_HOME}/usertools/dpdk-devbind.py -b igb_uio ${PCI_ADDR}

echo "✅ DPDK 环境初始化完成！当前网卡状态如下："
echo "----------------------------------------------------"
${DPDK_HOME}/usertools/dpdk-devbind.py --status-dev net
echo "----------------------------------------------------"
