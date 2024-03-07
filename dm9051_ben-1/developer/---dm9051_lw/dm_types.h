//.#ifndef __DM_TYPES_H
//.#define __DM_TYPES_H

//dm_types.h

#if DM_TYPE == 0
	/* call */
	#define DM_FUNC(rtype, field) \
		dm9051opts_##rtype##field()
	#define DM_DESC(rtype, field) /* to be usage */ \
		dm9051opts_desc##field()

	/* extern API */
	#undef DM_MACRO
	#define DM_MACRO(rtype, field) \
		rtype dm9051opts_##rtype##field(void); \
		char *dm9051opts_desc##field(void);

#elif DM_TYPE == 1
	/* field */
	#undef DM_MACRO
	#define DM_MACRO(rtype,field) \
		rtype field; \
		char *desc##field;

#elif DM_TYPE == 2
	/* implement */
	#undef DM_MACRO
	#define DM_MACRO(rtype, field) \
		rtype dm9051opts_##rtype##field(void) { \
			return dm9051optsex[mstep_get_net_index()].##field; \
		} \
		char *dm9051opts_desc##field(void) { \
			return dm9051optsex[mstep_get_net_index()].desc##field##; \
		}
#endif

DM_MACRO(enable_t, test_plan_log) //.SG_FUNCTION(enable_t, test_plan_log);
DM_MACRO(uint8_t, iomode)
DM_MACRO(csmode_t, csmode)
DM_MACRO(ncrmode_t, ncrmode)
DM_MACRO(uint8_t, promismode)
DM_MACRO(enable_t, rxtypemode)
DM_MACRO(enable_t, rxmode_checksum_offload)
DM_MACRO(enable_t, flowcontrolmode)
DM_MACRO(enable_t, onlybytemode)
DM_MACRO(uint8_t, hdir_x2ms)
DM_MACRO(enable_t, hdlr_confrecv)
DM_MACRO(enable_t, tx_endbit)
DM_MACRO(enable_t, generic_core_rst)

#undef DM_TYPE

//.#endif //__DM_TYPES_H
