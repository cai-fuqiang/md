struct sysrq_state {
	char resv_1[216];
	unsigned int alt;
	unsigned int alt_use;
	unsigned int shift;
	unsigned int shift_use;
	unsigned char active;
	unsigned char need_reinject;
	unsigned char reinjecting;
	unsigned char reset_canceled;
	unsigned char reset_requested;
};

