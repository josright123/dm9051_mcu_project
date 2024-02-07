//with [tmr_ temp.c]
void func_link_timer(net_t *net, uint16_t bmsr); //can static.
void phy_link_timer(void *arg);
void net_new_task(net_t *net, net_link_cb_t cb, void *arg);
void net_renew_task(net_t *net, net_link_cb_t cb);
