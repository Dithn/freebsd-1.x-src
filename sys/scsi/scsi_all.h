/*
 * HISTORY
 * $Log: scsi_all.h,v $
 * Revision 1.3  1993/08/20 09:23:22  rgrimes
 * Update scsi code to the latest from Julian.  This code is now identical
 * to the last copy from Julian.  After this commit I will be commiting the
 * local fixes and makeing diffs to send back to Julian so he can update his
 * code.
 *
 * ----
 * From julian@jules.DIALix.oz.au  Thu Aug  5 09:25:23 1993
 * To: hd@world.std.com, julian@jules.DIALix.oz.au
 * Cc: nate@bsd.coe.montana.edu
 * Subject: Re: new scsi
 *
 * ----
 * From julian@jules.DIALix.oz.au  Sat Aug  7 04:13:17 1993
 * To: hd@world.std.com (HD Associates)
 * Cc: nate@bsd.coe.montana.edu
 * Subject: Re: timeout diffs
 *
 * Here are the diffs to take the scsi stuff to my latest tree from what
 * Nate and you received..
 *
 * the changes remove all the local timeout stuff and use (un)timeout(),
 *
 * ----
 * From julian@jules.DIALix.oz.au  Sat Aug  7 04:13:45 1993
 * To: hd@world.std.com (HD Associates)
 * Cc: nate@bsd.coe.montana.edu, briggs@csugrad.cs.vt.edu
 *
 * here is a fix for a silly bug in the scsiconf I just sent out
 * and a similar fix for st.c
 *
 * Revision 1.4  93/08/05  21:37:37  julian
 * fix the definitionof the last byte of each scsi command.
 * 
 * Revision 1.3  93/08/01  02:39:21  julian
 * compiles.. no mode bitfields
 * 
 * Revision 1.2  93/07/31  22:52:26  julian
 * removed all bitfields
 * 
 * Revision 1.1  93/04/12  21:51:01  root
 * checkin for 'jules'
 * 
 * Revision 1.2  1992/11/20  23:07:13  julian
 * add a definition for device type T_NODEVICE
 *
 * Revision 1.1  1992/09/26  22:14:02  julian
 * Initial revision
 *
 *
 * PATCHES MAGIC                LEVEL   PATCH THAT GOT US HERE
 * --------------------         -----   ----------------------
 * CURRENT PATCH LEVEL:         1       00098
 * --------------------         -----   ----------------------
 *
 * 16 Feb 93	Julian Elischer		ADDED for SCSI system
 * 
 */

/*
 * SCSI general  interface description
 */


/*
 * Largely written by Julian Elischer (julian@tfs.com)
 * for TRW Financial Systems.
 *
 * TRW Financial Systems, in accordance with their agreement with Carnegie
 * Mellon University, makes this software available to CMU to distribute
 * or use in any manner that they see fit as long as this message is kept with 
 * the software. For this reason TFS also grants any other persons or
 * organisations permission to use or modify this software.
 *
 * TFS supplies this software to be publicly redistributed
 * on the understanding that TFS is not responsible for the correct
 * functioning of this software in any circumstances.
 *
 */

/*
 * Ported to run under 386BSD by Julian Elischer (julian@tfs.com) Sept 1992
 */

/*
 * SCSI command format
 */

/*
 * Define dome bits that are in ALL (or a lot of) scsi commands
 */
#define SCSI_CTL_LINK	0x01
#define SCSI_CTL_FLAG	0x02
#define SCSI_CTL_VENDOR	0xC0
#define	SCSI_CMD_LUN	0xA0		/*these two should not be needed*/
#define	SCSI_CMD_LUN_SHIFT	5	/* LUN in the cmd is no longer SCSI */


struct scsi_generic
{
	u_char	opcode;
	u_char	bytes[11];
};

struct scsi_test_unit_ready
{
	u_char	op_code;
	u_char	byte2;
	u_char	unused[3];
	u_char	control;
};

struct scsi_send_diag
{
	u_char	op_code;
	u_char	byte2;
#define	SSD_UOL		0x01
#define	SSD_DOL		0x02
#define	SSD_SELFTEST	0x04
#define	SSD_PF		0x10
	u_char	unused[1];
	u_char	paramlen[2];
	u_char	control;
};

struct scsi_sense
{
	u_char	op_code;
	u_char	byte2;
	u_char	unused[2];
	u_char	length;
	u_char	control;
};

struct scsi_inquiry
{
	u_char	op_code;
	u_char	byte2;
	u_char	unused[2];
	u_char	length;
	u_char	control;
};

struct scsi_mode_sense
{
	u_char	op_code;
	u_char	byte2;
#define	SMS_DBD				0x08
	u_char	page;
#define	SMS_PAGE_CODE 			0x3F
#define	SMS_PAGE_CTRL 			0xC0
#define	SMS_PAGE_CTRL_CURRENT 		0x00
#define	SMS_PAGE_CTRL_CHANGEABLE 	0x40
#define	SMS_PAGE_CTRL_DEFAULT 		0x80
#define	SMS_PAGE_CTRL_SAVED 		0xC0
	u_char	unused;
	u_char	length;
	u_char	control;
};

struct scsi_mode_sense_big
{
	u_char	op_code;
	u_char	byte2; 	/* same bits as small version */
	u_char	page; 		/* same bits as small version */
	u_char	unused[4];
	u_char	length[2];
	u_char	control;
};

struct scsi_mode_select
{
	u_char	op_code;
	u_char	byte2;
#define	SMS_SP	0x01
#define	SMS_PF	0x10
	u_char	unused[2];
	u_char	length;
	u_char	control;
};

struct scsi_mode_select_big
{
	u_char	op_code;
	u_char	byte2;	/* same bits as small version */
	u_char	unused[5];
	u_char	length[2];
	u_char	control;
};

struct scsi_reserve
{
	u_char	op_code;
	u_char	byte2;
	u_char	unused[2];
	u_char	length;
	u_char	control;
};

struct scsi_release
{
	u_char	op_code;
	u_char	byte2;
	u_char	unused[2];
	u_char	length;
	u_char	control;
};

struct scsi_prevent
{
	u_char	op_code;
	u_char	byte2;
	u_char	unused[2];
	u_char	how;
	u_char	control;
};
#define	PR_PREVENT 0x01
#define PR_ALLOW   0x00

/*
 * Opcodes
 */

#define	TEST_UNIT_READY		0x00
#define REQUEST_SENSE		0x03
#define INQUIRY			0x12
#define MODE_SELECT		0x15
#define MODE_SENSE		0x1a
#define START_STOP		0x1b
#define RESERVE      		0x16
#define RELEASE      		0x17
#define PREVENT_ALLOW		0x1e
#define POSITION_TO_ELEMENT	0x2b
#define	MODE_SENSE_BIG		0x54
#define	MODE_SELECT_BIG		0x55
#define MOVE_MEDIUM     	0xa5
#define READ_ELEMENT_STATUS	0xb8


/*
 * sense data format
 */
#define T_DIRECT	0
#define T_SEQUENTIAL	1
#define T_PRINTER	2
#define T_PROCESSOR	3
#define T_WORM		4
#define T_READONLY	5
#define T_SCANNER 	6
#define T_OPTICAL 	7
#define T_NODEVICE	0x1F

#define T_CHANGER	8
#define T_COMM		9

#define T_REMOV		1
#define	T_FIXED		0

struct scsi_inquiry_data
{
	u_char	device;
#define	SID_TYPE	0x1F
#define	SID_QUAL	0xE0
#define	SID_QUAL_LU_OK	0x00
#define	SID_QUAL_LU_OFFLINE	0x20
#define	SID_QUAL_RSVD	0x40
#define	SID_QUAL_BAD_LU	0x60
	u_char	dev_qual2;
#define	SID_QUAL2	0x7F
#define	SID_REMOVABLE	0x80
	u_char	version;
#define SID_ANSII	0x07
#define SID_ECMA	0x38
#define SID_ISO		0xC0
	u_char	response_format;
	u_char	additional_length;
	u_char	unused[2];
	u_char	flags;
#define	SID_SftRe	0x01
#define	SID_CmdQue	0x02
#define	SID_Linked	0x08
#define	SID_Sync	0x10
#define	SID_WBus16	0x20
#define	SID_WBus32	0x40
#define	SID_RelAdr	0x80
	char	vendor[8];
	char	product[16];
	char	revision[4];
	u_char	extra[8];
};


struct	scsi_sense_data
{
	u_char	error_code;	/* same bits as new version */
	union
	{
		struct
		{
			u_char	blockhi;
			u_char	blockmed;
			u_char	blocklow;
		} unextended;
		struct
		{
			u_char	segment;
			u_char	flags;	/* same bits as new version */
			u_char	info[4];
			u_char	extra_len;
			/* allocate enough room to hold new stuff
			( by increasing 16 to 26 below) */
			u_char	extra_bytes[26];
		} extended;
	}ext;
};
struct	scsi_sense_data_new
{
	u_char	error_code;
#define	SSD_ERRCODE		0x7F
#define	SSD_ERRCODE_VALID	0x80
	union
	{
		struct	/* this is depreciated, the standard says "DON'T"*/
		{
			u_char	blockhi;
			u_char	blockmed;
			u_char	blocklow;
		} unextended;
		struct
		{
			u_char	segment;
			u_char	flags;
#define	SSD_KEY		0x0F
#define	SSD_ILI		0x20
#define	SSD_EOM		0x40
#define	SSD_FILEMARK	0x80
			u_char	info[4];
			u_char	extra_len;
			u_char	cmd_spec_info[4];
			u_char	add_sense_code;
			u_char	add_sense_code_qual;
			u_char	fru;
			u_char	sense_key_spec_1;
#define	SSD_SCS_VALID		0x80
			u_char	sense_key_spec_2;
			u_char	sense_key_spec_3;
			u_char	extra_bytes[16];
		} extended;
	}ext;
};

struct	blk_desc
{
	u_char	density;
	u_char	nblocks[3];
	u_char	reserved;
	u_char	blklen[3];
};

struct scsi_mode_header
{
	u_char	data_length;	/* Sense data length */
	u_char	medium_type;
	u_char	dev_spec;
	u_char	blk_desc_len;
};

struct scsi_mode_header_big
{
	u_char	data_length[2];	/* Sense data length */
	u_char	medium_type;
	u_char	dev_spec;
	u_char	unused[2];
	u_char	blk_desc_len[2];
};


/*
 * Status Byte
 */
#define	SCSI_OK		0x00
#define	SCSI_CHECK		0x02
#define	SCSI_BUSY		0x08	
#define SCSI_INTERM		0x10
