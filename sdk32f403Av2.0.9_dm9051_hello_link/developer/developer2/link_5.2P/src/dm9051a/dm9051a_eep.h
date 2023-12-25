#ifndef __DM9051A_EEP_H
#define __DM9051A_EEP_H

#ifdef __cplusplus
extern "C"
{
#endif

#define SZBUF 100

  void dm9051a_show_status(void);
  void dm9051a_show_e_fuse(void);
  uint16_t dm9051a_rd_eep(unsigned char addr);
  // int dm9051a_wr_eep(unsigned char reg_no, unsigned int wdata);
  void dm9051a_rd_e_fuse(unsigned char addr);
  void dm9051a_read_e_fuse_nbytes(uint8_t start_addr, uint8_t length, uint8_t *buf);
  uint16_t dm9051a_wr_e_fuse(unsigned int addr, unsigned int wdata);
  void dm9051a_wr_e_fuse_nbytes(uint8_t start_addr, uint8_t length, uint8_t *buf);
  int8_t dm9051a_write_e_fuse_nbytes(uint8_t start_addr, uint8_t length, uint8_t *buf);

#ifdef __cplusplus
}
#endif

#endif //__DM9051A_EEP_H
