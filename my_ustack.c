#include <stdio.h>
#include <rte_eal.h>
#include <rte_ethdev.h>
#include <rte_mempool.h>


#define NUM_MBUFS       4096
#define BURST_SIZE      128

int global_portid = 0;

static const struct rte_eth_conf port_conf_default = {
	.rxmode = { .max_rx_pkt_len = RTE_ETHER_MAX_LEN }
};

static int ustack_init_port(struct rte_mempool *mbuf_pool) {

    uint16_t nb_sys_ports = rte_eth_dev_count_avail();
    if (nb_sys_ports == 0) {
        rte_exit(EXIT_FAILURE, "No Supported eth found\n");
    }
    
// recv
    const int num_rx_queues = 1;
    const int num_tx_queues = 0;

    rte_eth_dev_configure(global_portid, num_rx_queues, num_tx_queues, &port_conf_default);
    
    if (rte_eth_rx_queue_setup(global_portid, 0, 128, rte_eth_dev_socket_id(global_portid), NULL, mbuf_pool)) {
        rte_exit(EXIT_FAILURE, "Could not setup RX queue\n");
    }

// start
    if (rte_eth_dev_start(global_portid) < 0) {
        rte_exit(EXIT_FAILURE, "Could not start\n");
    }
    
    return 0;
}

int main(int argc, char *argv[]) {
    // 1. 初始化 DPDK 环境抽象层 (EAL)
    // 这是所有 DPDK 程序必须调用的第一步
    int ret = rte_eal_init(argc, argv);
    if (ret < 0) {
        rte_exit(EXIT_FAILURE, "Error with EAL initialization\n");
    }

    // 2. 你的业务代码以后就写在这里
    printf("====== DPDK Environment Init Successful! ======\n");

    struct rte_mempool *mbuf_pool = rte_pktmbuf_pool_create("mbuf pool", NUM_MBUFS, 0, 0, RTE_MBUF_DEFAULT_BUF_SIZE, rte_socket_id());
	if (mbuf_pool == NULL) {
		rte_exit(EXIT_FAILURE, "Could not create mbuf pool\n");
	}

    ustack_init_port(mbuf_pool);

    while(1) {

        struct rte_mbuf *mbufs[BURST_SIZE] = {0};

        uint16_t num_recvd = rte_eth_rx_burst(global_portid, 0, mbufs, BURST_SIZE);
        if (num_recvd > BURST_SIZE) {
			rte_exit(EXIT_FAILURE, "Error receiving from eth\n");
		}

        for (int i = 0; i < num_recvd; i ++) {
            struct rte_ether_hdr *ethhdr = rte_pktmbuf_mtod(mbufs[i], struct rte_ether_hdr *);
            if (ethhdr->ether_type != rte_cpu_to_be_16(RTE_ETHER_TYPE_IPV4)) {
                continue;
            }

            struct rte_ipv4_hdr *iphdr = rte_pktmbuf_mtod_offset(mbufs[i], struct rte_ipv4_hdr *, sizeof(struct rte_ether_hdr));
            if (iphdr->next_proto_id == IPPROTO_UDP) {
                struct rte_udp_hdr *udphdr = (struct rte_udp_hdr *)(iphdr + 1);

                printf("udp : %s\n", (char *)(udphdr + 1));
            }

        }


    }

    return 0;
}