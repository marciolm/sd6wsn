typedef struct flow_s {
	uint8_t flowid;
	uip_ipaddr_t ipv6src;
	uint8_t srcmask;
	uip_ipaddr_t ipv6dst;
	uint8_t dstmask ;
	uint16_t srcport;
	uint16_t dstport;
	uint8_t ipproto;
	uint8_t action;
	uip_ipaddr_t nhipaddr;
	uint8_t txpwr;
}flow_s;

uip_ipaddr_t * get_next_hop_by_flow(uip_ipaddr_t *srcaddress,uip_ipaddr_t *dstaddress,uint16_t *srcport,uint16_t *dstport,uint8_t *proto);
