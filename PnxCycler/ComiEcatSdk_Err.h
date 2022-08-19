#ifndef _COMIECAT_SDK_ERRCODE_H_
#define _COMIECAT_SDK_ERRCODE_H_

#define ecERR_NONE						0
#define ecERR_DEVICE_NOT_LOADED			-5  // ������ ����̽��� �ε���� ���� ���.
#define ecERR_WDM_VER_READ_FAIL			-6	// WDM ����̹����� ���� ������ �дµ� ������ ���.
#define ecERR_FW_VER_READ_FAIL			-7	// Firmware ���� ������ �дµ� ������ ���.
#define ecERR_DEV_BOOT_TIMEOUT			-8	// Master Device�� ������ �ð� �ȿ� ������ �Ϸ���� ���� ���.
#define ecERR_DEV_BOOT_NOT_COMPT		-9  // Master Device�� ���� ��Ʈ���� �Ϸ���� ���� ���.
#define ecERR_INVALID_BOARDID			-10
#define ecERR_INVALID_DEVIDX			-11
#define ecERR_INVALID_VERSION			-12	// SDK(DLL), WDM ����̹�, �߿���(Firmware) ��ȣ���� ������ ȣȯ���� �ʴ� ���.
#define ecERR_INVALID_NETID				-20
#define ecERR_INVALID_SLAVEID			-25 // �����̺� �ε��� �Ǵ� �����̺� �ּҰ� �߸��� ���...
#define ecERR_INVALID_CHANNEL			-30 // ���ȣ �Ǵ� ä�ι�ȣ ���� �߸��� ���...
#define ecERR_INVALID_IXMAP_IDX			-40 // Motion ���� ��ɿ��� Interpolation ���� �� ��ȣ�� �߸��� ���...
#define ecERR_INVALID_IXMAP_AXES		-50 // IXMAP�� ���Ե� ���� ������ �߸��� ���.  
#define ecERR_INVALID_LMMAP_IDX			-55	// ����Ʈ��� ���� �� ��ȣ�� �߸��� ���.
#define ecERR_INVALID_FUNC_ARG			-60 // �Լ��� �Ű������� ��ȿ���� ���� ���� ���.
#define ecERR_INVALID_HANDLE			-65 // Invalid handle value�� �Ű������� ���޵� ���.
#define ecERR_INVALID_RESULT_DATA		-66 // ��� �����Ͱ� ��ȿ���� ���� ���.
#define ecERR_INVALID_SIZE_INFO			-67 // ũ�� ���� ������ ������ ��ȿ���� ���� ���.
#define ecERR_NULL_DLLNETCTXT			-69
#define ecERR_NULL_WDMNETCTXT			-70 // WDM ����̹����� �����ϴ� �����޸� �����Ͱ� NULL�� ���.
#define ecERR_NULL_INPDOLMEM			-71 // WDM ����̹����� �����ϴ� InPDO�� ���޸� �����Ͱ� NULL�� ���.
#define ecERR_NULL_OUTPDOLMEM			-72 // WDM ����̹����� �����ϴ� InPDO�� ���޸� �����Ͱ� NULL�� ���.
#define ecERR_INVALID_AXIS_INPDO_TYPE	-73 // ���� �����Ǿ� �ִ� Axis InputPDO ������ ���ϴ� �����͸� �������� ����.
#define ecERR_INVALID_SECTION_ID		-80 // PDO Section ID�� �ùٸ��� �ʰų�, ȯ�漳���� ���� ����Ʈ�� ���� ���.
#define ecERR_INVALID_LOG_MEM_ADDR		-90 // Logical Memory�� �ּҰ��� �ùٸ��� ���� ���.
#define ecERR_INVALID_BUFFER			-95	// ������ �ּҰ��� ��ȿ���� ���� ���
#define ecERR_INVALID_IO_CHAN_MAP_DATA	-100 // I/O ä�� ���� �������� �Ϻ� ������� �ùٸ��� ���� ���.
#define ecERR_INVALID_FILE_PATH			-110 // ������ ������ ��ΰ� �߸��Ǿ��ų� ������ ���� ���.
#define ecERR_INTERNAL_PROCESS_FAULT	-111 // ���� ���� ���� 
#define ecERR_MUTEX_CREATE_FAIL			-112 // Mutex ���� ����
#define ecERR_FILE_OPEN_FAIL			-120
#define ecERR_FILE_NOT_FOUND			-125 // ������ ������ �˻����� ���� ���.
#define ecERR_FILE_READ_ERROR			-130 // ������ �д� �������� ������ �߻��� ���.
#define ecERR_FILE_VERIF_DATA_ERROR		-140 // CFG���� Verfication�� �������� �� ��ȣ���� �����Ͱ� ��ġ���� �ʴ� ���.
#define ecERR_MEM_ALLOC_FAIL			-150 // Memory Allocation fail
#define ecERR_XML_POS_SAVE_ERR			-160 // XML ������ �б��� �� ������ TREE position�� �����ϴµ� ������ �߻��� ���.
#define ecERR_XML_ELEM_NOT_FOUND		-165 // XML ������ �б��� �� ������ Element�� �˻����� ���� ���.
#define ecERR_INVALID_ODLIST_LEN_INFO	-166 // �����̺�κ��� ���޵� SDO Info. OD List�� ũ�� ������ ��ȿ���� ���� ���.
#define ecERR_INVALID_FIRMWARE_SIGN		-180 // ��������ġ�� �ٿ�ε� �Ǿ� �ִ� �߿����� signature�� ��ȿ���� ���� ���.
#define ecERR_FW_VERIF_DATA_MISMATCH	-181 // F/W Download�� �� �Ŀ� Verify �������� �����Ͱ� ��ġ���� ���� ���.
#define ecERR_EEPROM_WRITE_FAIL			-182 // ESC�� EEPROM�� ������ ���� ����
#define ecERR_IMPROPER_AL_STATE			-183 // AL-STATE�� �������� ���� ���.

// Motion ���� ���� ���� �ڵ� //
#define ecERR_MOTION_BASE				-1000  // Motion ���� �����ڵ� ���� ��ȣ 
#define ecERR_MOT_SERVO_ALARM			(ecERR_MOTION_BASE - 10) // Servo driver�� �˶��� �߻��� ���.
#define ecERR_MOT_SEQ_SKIPPED			(ecERR_MOTION_BASE - 20) // Motion command has been skipped because the axis is already running
#define ecERR_MOT_LM_QUE_FULL			(ecERR_MOTION_BASE - 30) //	ListMotion Queue�� ������ ����Ʈ��� Ŀ�ǵ带 Queueing�� �� ���� ���. 
#define ecERR_MOT_LM_INVALID_OWNERSHIP	(ecERR_MOTION_BASE - 40) //	������ ListMotion �ʿ� ���� ������� �ٸ� ���μ������� ����æ ��� 

//---------------------------------------------------------------------------------------------------------------------------
//			WDM ����̹� ���α׷����� �߻��ϴ� �����ڵ�
//---------------------------------------------------------------------------------------------------------------------------
#define ERR_COMMON_WITH_APP_BASE		-5000
#define ERR_INVALID_DSP_IDX				(ERR_COMMON_WITH_APP_BASE - 1) // �߸��� DSP Index�� �Ű������� Ŀ�ǵ带 ��û�� ���.
#define ERR_DSPCMD_IRQ_TIMEOUT			(ERR_COMMON_WITH_APP_BASE - 5) // DSP Command ��û ���ͷ�Ʈ ��û�� ���ѵ� �ð����� �������� ���� ���.
#define ERR_DSPCMD_ACK_CLR_TIMEOUT		(ERR_COMMON_WITH_APP_BASE - 6) // DSP �� PCI Command�� ���� Ack �÷��׸� ������ �ð� ���� Ŭ�������� ���� ��� 
#define ERR_DSPCMD_ACK_SET_TIMEOUT		(ERR_COMMON_WITH_APP_BASE - 7) // DSP �� PCI Command�� ���� Ack �÷��׸� ������ �ð� ���� ������ ���� ��� 
#define ERR_PENDEDIRP_ADDTOLIST_FAIL	(ERR_COMMON_WITH_APP_BASE - 10) // Ư�� IRP�� PendingList�� ����Ϸ������� ������. 
#define ERR_DSPCMD_ANS_TIMEOUT			(ERR_COMMON_WITH_APP_BASE - 20) // DSP Ŀ�ǵ尡 ���ѵ� �ð� ���� ������ ���� ����.


//---------------------------------------------------------------------------------------------------------------------------
//			ETHERCAT ��� ó�� ���� �����ڵ�
//---------------------------------------------------------------------------------------------------------------------------

#define ecERR_ECATNET_BASE				-10000  // DSP0(NET)���� ����ϴ� �����ڵ� ������ ���۰�.
#define ecERR_GEN_BASE					ecERR_ECATNET_BASE
#define ecERR_GEN_INVAL_ARGUMENT		(ecERR_GEN_BASE - 10)  // �Լ� ȣ�� �ÿ� �Ű��������� �߸��� ���.
#define ecERR_GEN_DGRAM_OBJ_ALLOC_FAIL	(ecERR_GEN_BASE - 20) // Slave���� DataGram �޸𸮸� �Ҵ��ϴµ� ������.
#define ecERR_GEN_MCMD_ALLOC_FAIL		(ecERR_GEN_BASE - 25) // Slave���� DataGram �޸𸮸� �Ҵ��ϴµ� ������.
#define ecERR_GEN_MCMD_TIMEOUT			(ecERR_GEN_BASE - 30) // MasterCmd ó�� Ÿ�Ӿƿ�
#define ecERR_GEN_MCMD_ITEM_NOT_FOUND	(ecERR_GEN_BASE - 40) // ������ MCmdIdx�� ���� MasterCmd Item�� ����Ʈ���� ã�� �� ���� ���
#define ecERR_GEN_BUF_SIZE_TOO_SMALL	(ecERR_GEN_BASE - 50)
#define ecERR_GEN_SLV_PHYSADDR_NOT_SET	(ecERR_GEN_BASE - 60) // Slave�� Physical Address�� ���õ��� ����.
#define ecERR_GEN_INVAL_REG_RW_SIZE		(ecERR_GEN_BASE - 70) // Slave Register Read/Write �ÿ� ���� ũ�⺸�� ū �����͸� Read/Write�� ���.
#define ecERR_GEN_INVAL_SLAVE_ID		(ecERR_GEN_BASE - 80)
#define ecERR_GEN_ECSLV_OBJ_NOT_FOUND	(ecERR_GEN_BASE - 90) // TEcSlave ��ü �����Ͱ� null�� ���.
#define ecERR_GEN_STATE_CHANGE_FAIL		(ecERR_GEN_BASE - 100) // TEcSlave ��ü �����Ͱ� null�� ���.

// Ŀ�ǵ�  ���� //
#define ecERR_CMD_BASE					(ecERR_ECATNET_BASE - 200) // -10200 ���� ����
#define ecERR_CMD_VENDID_READ_FAIL		(ecERR_CMD_BASE - 10)	// Vendor ID �б� ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_PRODCODE_READ_FAIL	(ecERR_CMD_BASE - 20)	// Product Code �б� ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_REVNO_READ_FAIL		(ecERR_CMD_BASE - 30)	// Revision No. �б� ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_SERNO_READ_FAIL		(ecERR_CMD_BASE - 40)	// Serial No. �б� ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_SET_PHY_ADDR_FAIL		(ecERR_CMD_BASE - 50) 	// Physical Address ���� ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_SET_DL_CTL_REG_FAIL	(ecERR_CMD_BASE - 60) 	// DL Control Register ���� ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_SET_AL_STATE_CHANGE_FAIL (ecERR_CMD_BASE - 65) // AL State ���� ���� �߿� ���� �߻�
#define ecERR_CMD_PDOMAP_ASSIGN_FAIL	(ecERR_CMD_BASE - 70) 	// In/Out PDO Map Assign ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_FMMU_SETTING_FAIL		(ecERR_CMD_BASE - 80) 	// In/Out PDO Map Assign ����. ���� ���� ������ ���굥���� ������ ��
#define ecERR_CMD_PDO_SYNC_MODE_SET_FAIL	(ecERR_CMD_BASE - 90) 	// In/Out PDO Map Assign ����. ���� ���� ������ ���굥���� ������ ��

// EtherCAT ��� ���� �Ϲ�  ���� //
#define ecERR_ECG_BASE					(ecERR_ECATNET_BASE - 400) // -10400 ���� ����
#define ecERR_ECG_DGRM_TIMEOUT			(ecERR_MBX_BASE - 5)  // Ư�� EC Ŀ�ǵ忡 ���ؼ� ���� �ð� �̳��� ��ȿ�� ������ �������� ����  ���
#define ecERR_ECG_STATE_CHANGE_TIMEOUT	(ecERR_MBX_BASE - 10) // Slave�� State Change�� ��û������ ���� �����ϴµ� ���ѵ� �ð��� �ʰ��� ���

// Mailbox ��� ���� ���� //
#define ecERR_MBX_BASE					(ecERR_ECATNET_BASE - 600) // -10600���� ����
#define ecERR_MBX_COE_NOT_SUPP			(ecERR_MBX_BASE - 10) // ������  �����̺갡 CoE ���������� �������� �ʴ� ���
#define ecERR_MBX_INVAL_SDO_RESP		(ecERR_MBX_BASE - 20) // SDO Command�� ���� ���� �������� �ùٸ��� ����
#define ecERR_MBX_INVAL_SDO_SIZE		(ecERR_MBX_BASE - 30) // SDO�� Size ������ �߸��� ���
#define ecERR_MBX_SDO_DATA_SIZE_ERR		(ecERR_MBX_BASE - 40) // SDO �����Ͱ� ���ѵ� ũ�⺸�� ū ���.
#define ecERR_MBX_INVAL_SLAVE_ID		(ecERR_MBX_BASE - 50) // Slave Id�� �߸� ������ ���
#define ecERR_MBX_TXFER_ABORTED			(ecERR_MBX_BASE - 60) // Mailbox ����� abort��, �� �� Abort Code ID�� �˷��� TEcSlave::LastErrSubData�� �����ϸ� �ȴ�. //

// PDO ��� ���� ���� //
#define ecERR_PDO_BASE					(ecERR_ECATNET_BASE - 800) // -10800���� ����
#define ecERR_INVALID_PDO_SIZE			(ecERR_PDO_BASE - 10) // PDO ũ�� ������ �߸��� ���.
#define ecERR_PDO_BUF_ALLOC_FAIL		(ecERR_PDO_BASE - 20) // PDO ���� �Ҵ� ����
#define ecERR_INVALID_OUTPDO_SM_IDX		(ecERR_PDO_BASE - 30) // OutPDO�� ���� SM �ε����� �ùٸ��� �ʴ�.
#define ecERR_INVALID_INPDO_SM_IDX		(ecERR_PDO_BASE - 40) // OutPDO�� ���� SM �ε����� �ùٸ��� �ʴ�.

//---------------------------------------------------------------------------------------------------------------------------
//			��������ġ������ APPLICATION Ŀ�ǵ� ó�� ���� �����ڵ�
//---------------------------------------------------------------------------------------------------------------------------

// EcatApp ���μ������� IPC ��� ���� ���� //
#define ecERR_IPC_BASE					(ecERR_ECATNET_BASE - 1000) // -11000���� ����
#define ecERR_IPC_MSGQ_HEAP_CRE_FAIL	(ecERR_IPC_BASE - 10) // IPC Message Que�� ���� Heap�� �����ϴµ� ������.
#define ecERR_IPC_MSGQ_CRE_FAIL			(ecERR_IPC_BASE - 20) // IPC Message Que�� �����ϴ��� ������.
#define ecERR_IPC_INVAL_ECM_DATASIZE	(ecERR_IPC_BASE - 30) // EcmCmd�� DataSize�� ������ ���� �ʹ� ū ���
#define ecERR_IPC_MSGQ_PUT_FAIL			(ecERR_IPC_BASE - 40) // IPC Message Que�� �޽����� Put�ϴµ� ������ (MessageQ_put() �Լ� ���� ����)
#define ecERR_IPC_NOTIEVT_FAIL			(ecERR_IPC_BASE - 50) // IPC Notification event�� ó���ϴµ� ������ �߻���
#define ecERR_IPC_WAITACK_TIMEOUT		(ecERR_IPC_BASE - 60) // IPC Message �� �� �޾Ҵٴ� EcatMast���� ACK �޽��� ���ſ� ���� Ÿ�Ӿƿ� �߻���

#define	eaERR_NONE						0
#define eaERR_ECATAPP_BASE				-20000  // DSP1(APP)���� ����ϴ� �����ڵ� ������ ���۰�.
#define eaERR_GEN_BASE					eaERR_ECATAPP_BASE
#define eaERR_GEN_MALLOC_FAIL			(eaERR_GEN_BASE - 10) // memory allocation fail
#define eaERR_FLASH_DEV_OPEN_FAIL		(eaERR_GEN_BASE - 30) // Flash �޸���ġ�� open�ϴµ� ������ �߻��� ���.
#define eaERR_FLASH_DEV_WRITE_FAIL		(eaERR_GEN_BASE - 31) // Flash �޸���ġ�� �����͸� �����ϴ� �������� ������ �߻��� ���.
#define eaERR_FLASH_DEV_READ_FAIL		(eaERR_GEN_BASE - 31) // Flash �޸���ġ�� �����͸� �б��ϴ� �������� ������ �߻��� ���.
#define eaERR_INVALID_FWU_SIGNATURE		(eaERR_GEN_BASE - 35) // Flash �޸���ġ�� ����� FW ������ ��ȿ���� ���� ���.
#define eaERR_INVALID_BUF_SIZE			(eaERR_GEN_BASE - 40) // Buffer size ������ �߸��� ���.
#define eaERR_INVALID_BUFFER			(eaERR_GEN_BASE - 41) // ������ ���۰� NULL�̰ų� �߸��� ���۰� ������ ���.

// PCI Ŀ�ǵ� ó�� ���� ���� //
#define eaERR_PCICMD_BASE				(eaERR_ECATAPP_BASE - 200) // - 20200 ���� ����.
#define eaERR_PCICMD_TIMEOUT			(eaERR_PCICMD_BASE - 10)	// PciCmd�� ó���ϴ� �������� Ÿ�Ӿƿ� �߻���. �̶� Error SubData�� CmdId�� ��Ÿ����.
#define eaERR_PCICMD_INVALID_CMDID		(eaERR_PCICMD_BASE - 20)	// PciCmd�� ó���ϴ� �������� Ÿ�Ӿƿ� �߻���. �̶� Error SubData�� CmdId�� ��Ÿ����.
#define eaERR_PCICMD_INVALID_ARG		(eaERR_PCICMD_BASE - 30)	// PciCmd�� Argument�� ��ȿ���� ����.
#define eaERR_PCICMD_INVALID_SLVIDX		(eaERR_PCICMD_BASE - 40)	// PciCmd�� SlaveIndex �� �߸� ���޵� ���.
#define eaERR_PCICMD_INVALID_AXIS		(eaERR_PCICMD_BASE - 50)	// Motion PciCmd�� Axis �Ű������� �߸� ���޵� ���.
#define eaERR_PCICMD_INVALID_IXMAP_IDX	(eaERR_PCICMD_BASE - 60)	// Motion PciCmd�� Axis �Ű������� �߸� ���޵� ���.
#define eaERR_PCICMD_HOME_START_TIMEOUT	(eaERR_PCICMD_BASE - 70)	// Homing�� ���۽������� ��������̹��� Homing�� �������� �ʴ� ���.
#define eaERR_PCICMD_INVALID_AL_STATE	(eaERR_PCICMD_BASE - 80)	// �ش� Ŀ�ǵ带 ������ �� �ִ� AL State�� �ƴ� ���.

// EcatMast(Net) ���μ������� IPC ��� ���� ���� //
#define eaERR_IPC_BASE					(eaERR_ECATAPP_BASE - 400) // - 20400 ���� ����.
#define eaERR_IPC_MSGQ_HEAP_CRE_FAIL	(eaERR_IPC_BASE - 10) // IPC Message Que�� ���� Heap�� �����ϴµ� ������.
#define eaERR_IPC_MSGQ_CRE_FAIL			(eaERR_IPC_BASE - 20) // IPC Message Que�� �����ϴ��� ������.
#define eaERR_IPC_INVAL_EAPC_DATASIZE	(eaERR_IPC_BASE - 30) // EapCmd�� DataSize�� ������ ���� �ʹ� ū ���
#define eaERR_IPC_MSGQ_PUT_FAIL			(eaERR_IPC_BASE - 40) // IPC Message Que�� �޽����� Put�ϴµ� ������ (MessageQ_put() �Լ� ���� ����)
#define eaERR_IPC_NOTIEVT_FAIL			(eaERR_IPC_BASE - 50) // IPC Notification event�� ó���ϴµ� ������ �߻���
#define eaERR_IPC_WAITACK_TIMEOUT		(eaERR_IPC_BASE - 60) // IPC Message �� �� �޾Ҵٴ� EcatMast���� ACK �޽��� ���ſ� ���� Ÿ�Ӿƿ� �߻���

// ������� ���� �����ڵ� //
#define eaERR_MOT_BASE					(eaERR_ECATAPP_BASE - 1000) // - 21000 ���� ����.
#define eaERR_AXIS_MOT_QUE_FULL			(eaERR_MOT_BASE - 2)  ///< axis-motion-que�� ������ ����� �������� ���Ͽ���.
#define eaERR_INVALID_PARAMETER			(eaERR_MOT_BASE - 10) ///< Some of the funcion parameters are invalid
#define eaERR_INVALID_AXIS				(eaERR_MOT_BASE - 11) ///< The axis setting parameter(s) is(are) invalid
#define eaERR_INVALID_SPEED_SET			(eaERR_MOT_BASE - 12) ///< Speed setting value is not valid
#define eaERR_INVALID_IXMAP				(eaERR_MOT_BASE - 13) ///< Invalid Interpolation Map
#define eaERR_INVALID_LMMAP				(eaERR_MOT_BASE - 14) ///< Invalid List-Motion Map
#define eaERR_INVALID_NUMAXIS			(eaERR_MOT_BASE - 15) ///< Invlaid number of axis(Mx)
#define eaERR_INVALID_MAST_AXIS			(eaERR_MOT_BASE - 16) ///< Invlaid master axis settings for Master/Slave Motion
#define eaERR_INVALID_SPLINE_SETUP		(eaERR_MOT_BASE - 17) ///< SpineSetup() �Լ��� ������� ���� ���¿��� Spline���� ���� �ٸ� �Լ��� ������ ���.
#define eaERR_INVALID_POS_DATA			(eaERR_MOT_BASE - 38) ///< Position �����Ͱ� ��ȿ���� ���� ���.
#define eaERR_IX_AXES_NOT_DEFINED		(eaERR_MOT_BASE - 30) ///< ���� �� ������ ������� ���� ���.
#define eaERR_IX_OBJ_POOL_FULL			(eaERR_MOT_BASE - 35) ///< spline/MPRLin2X ����  object ��� pool�� ���� ��ϵǾ� �ִ� object�� �ִ� ������ŭ ��ϵǾ �� �̻� �߰��� �� ���� ���.
#define eaERR_IX_OBJ_NOT_FOUND			(eaERR_MOT_BASE - 36) ///< spline/MPRLin2X ����  object pool���� ������ SPLINE OBJECT�� ã�� �� ����.
#define eaERR_IX_OBJ_NOT_BUILDED		(eaERR_MOT_BASE - 37) ///< spline/MPRLin2X ����  object Build�� �������� �ʰ� �̼� ������ ������ ���.
#define eaERR_STOP_BY_SLP				(eaERR_MOT_BASE - 50) ///< Abnormally stopped by positive soft limit
#define eaERR_STOP_BY_SLN				(eaERR_MOT_BASE - 51) ///< Abnormally stopped by negative soft limit
#define eaERR_STOP_BY_ELP				(eaERR_MOT_BASE - 52)	///< Abnormally stopped by (+) external limit
#define eaERR_STOP_BY_ELN				(eaERR_MOT_BASE - 53)	///< Abnormally stopped by (+) external limit
#define eaERR_STOP_BY_ALM				(eaERR_MOT_BASE - 54)	///< Abnormally stopped by alarm input signal
#define eaERR_STOP_BY_COMM_ERROR		(eaERR_MOT_BASE - 55)	///< Abnormally Stopped by communication error
#define eaERR_STOP_BY_OTHER_AXIS		(eaERR_MOT_BASE - 56)	///< �ٸ� ����� ���� �̼� �߿� �ٸ� ���� ������ ���ؼ� ������ ���.
#define eaERR_STOP_BY_SVOFF				(eaERR_MOT_BASE - 57)	///< SERVO-OFF�� ���¿��� �̼۸���� ������ ���, �Ǵ� �̼� �߿� SERVO-OFF�� ���.
#define eaERR_MOT_SEQ_SKIPPED			(eaERR_MOT_BASE - 60)	///< Motion command has been skipped because the axis is already running
#define eaERR_SKIPPED_BY_SERVO_FAULT	(eaERR_MOT_BASE - 62)	///< Motion Control Command is skipped because of servo-driver alarm.
#define eaERR_SKIPPED_BY_SERVO_OFF		(eaERR_MOT_BASE - 63)	///< Motion Control Command is skipped because of 'Operation Enable(Servo-ON)' state is off.
#define eaERR_SKIPPED_BY_SERVO_ON		(eaERR_MOT_BASE - 64)	///< �������� �Ǿ� �ִ� ���¿��� ó���� �� ���� ����� �ϴ޵Ǿ� �ش� ����� ������ ���� ���� ���.
#define eaERR_HOME_START_TIMEOUT		(eaERR_MOT_BASE - 70) // ��������̹��� Homming start ����� �������� ������ �ð� ����  Homming�� ���۵��� �ʴ� ���.
#define eaERR_HOME_COMPT_FAIL			(eaERR_MOT_BASE - 71) // Servo driver�� Homing �۾��� �Ϸ��ϴ� �������� ������ �߻���.
#define eaERR_HOME_TPROBE_NOT_TRIGGERED	(eaERR_MOT_BASE - 72) // 101�� �̻��� �������� ��带 ������ �� Touch Probe�� ���������� Trigger���� ���� ���.
#define eaERR_HOME_STEP_TIMEOUT0		(eaERR_MOT_BASE - 80) // Touch Probe ������ ���ѽð����� �Ϸ���� ���� ���.
// �� �߰��� �����ڵ�� HOME_STEP ��ȣ�� ���� �Ű����Ƿ� ���� �������� ���� //
#define eaERR_HOME_STEP_TIMEOUT19		(eaERR_MOT_BASE - 99) // Touch Probe ������ ���ѽð����� �Ϸ���� ���� ���.
#define eaERR_LM_QUE_FULL				(eaERR_MOT_BASE - 110) // ����Ʈ����� Ŀ�ǵ� ť�� ������ Ŀ�ǵ带 ������� ���� ���.
#define eaERR_PTM_QUE_FULL				(eaERR_MOT_BASE - 130) // ����Ʈ����� Ŀ�ǵ� ť�� ������ Ŀ�ǵ带 ������� ���� ���.
#define eaERR_PTM_MAP_DISABLED			(eaERR_MOT_BASE - 131) // PT��� ���� Begin(ecmLmCtl_Begin() �Լ��� ���ؼ� �����)���� ���� ���¿��� PT��� ���� �Լ����� ������ ���.
#define eaERR_INAVLID_PDO_MAP			(eaERR_MOT_BASE - 150) // ������ PDO Mapping���� �������� �ʴ� ����� ����Ϸ��� �ϴ� ���.


// SW EtherCAT Master Error Code ///////////////////////////////////////////////////////////////////////////////////////////////////////
#define secERR_SWECAT_BASE					-50000  // SW EtherCAT Master Error Code ���۰�
#define secERR_NULL_FILENAME				(secERR_SWECAT_BASE - 1) // ���� �޸� �����Ͱ� NULL �� ���.
#define secERR_NULL_ECATMAINTASK			(secERR_SWECAT_BASE - 2) // EcatCheckProc ������ ���� ������ ���.
#define secERR_NULL_ECATCHECKPROC			(secERR_SWECAT_BASE - 3) // EcatCheckProc ������ ���� ������ ���.
#define secERR_NONE_NETADAPTER				(secERR_SWECAT_BASE - 4) // Network Adapter�� �߰ߵ��� ���� ���.
#define secERR_NOT_OP_ALLSLAVE				(secERR_SWECAT_BASE - 5) // �Ϻ� �Ǵ� ��ü Slave�� OP ���¿� ���� �ʴ� ���.

#define secERR_NULL_BUFFER					(secERR_SWECAT_BASE - 6) // ���۰� NULL�� ���
#define secERR_EEPROM_READ					(secERR_SWECAT_BASE - 7) // EEPROM Read ������ ���
#define secERR_EEPROM_WRITE					(secERR_SWECAT_BASE - 8) // EEPROM Write ������ ���
#define secERR_EEPROM_FILE					(secERR_SWECAT_BASE - 9) // EEPROM File �бⰡ ������ ���
#define secERR_INVALID_EEPROM_RW_MODE		(secERR_SWECAT_BASE - 10) // �������� �ʴ� EEPROM Read/Write Mode ��û��

#define secERR_OVERSIZE_FIRMWARE			(secERR_SWECAT_BASE - 11) // �߿��� ũ�Ⱑ �ʹ� ū ���
#define secERR_FOE_WRITE					(secERR_SWECAT_BASE - 12) // FOE Write ������ ���
#define secERR_FIRMWARE_FILE				(secERR_SWECAT_BASE - 13) // Firmware ���� �бⰡ ������ ���
#define secERR_TIMEOUT_BOOTMODE				(secERR_SWECAT_BASE - 14) // BOOT ���� ��û�� ���� Timeout�� ���

#define secERR_SDO_READ						(secERR_SWECAT_BASE - 15) // SDO Read ���н�
#define secERR_SDO_READ_TIMEOUT				(secERR_SWECAT_BASE - 16) // SDO Read Timeout
#define secERR_SDO_WRITE					(secERR_SWECAT_BASE - 17) // SDO Write ���н�
#define secERR_SDO_WRITE_TIMEOUT			(secERR_SWECAT_BASE - 18) // SDO Write Timeout
#define secERR_NOT_SUPPORTED_DEVICE			(secERR_SWECAT_BASE - 19) // �������� �ʴ� Device

#define secERR_NET_CMD_SCAN					(secERR_SWECAT_BASE - 20) /// Scan Request ���н�
#define secERR_NET_CMD_SCAN_TIMEOUT			(secERR_SWECAT_BASE - 21) // Scan Response Timeout
#define secERR_SLV_CONFIG_CHANGED			(secERR_SWECAT_BASE - 22) // Scan �� Slave�� Configuration ������ ������ ��ġ���� �ʴ� ���

// AL status code list type definition(SW Master) //////////////////////////////////////////////////////////////////////////////////////
#define EC_MAXERRORNAME					127	// Error ���ڿ� �ִ����(SW Master)

// Software Master Only
typedef struct{
	t_ui16 ALstatuscode;									// AL status code
	t_byte ALstatuscodedescription[EC_MAXERRORNAME + 1];	// Readable description
} TALstatuscodelist;

/** AL status code list definition */
const TALstatuscodelist ec_ALstatuscodelist[] = {
	{0x0000 , "No error" },
	{0x0001 , "Unspecified error" },
	{0x0002 , "No memory" },
	{0x0011 , "Invalid requested state change" },
	{0x0012 , "Unknown requested state" },
	{0x0013 , "Bootstrap not supported" },
	{0x0014 , "No valid firmware" },
	{0x0015 , "Invalid mailbox configuration" },
	{0x0016 , "Invalid mailbox configuration" },
	{0x0017 , "Invalid sync manager configuration" },
	{0x0018 , "No valid inputs available" },
	{0x0019 , "No valid outputs" },
	{0x001A , "Synchronization error" },
	{0x001B , "Sync manager watchdog" },
	{0x001C , "Invalid sync Manager types" },
	{0x001D , "Invalid output configuration" },
	{0x001E , "Invalid input configuration" },
	{0x001F , "Invalid watchdog configuration" },
	{0x0020 , "Slave needs cold start" },
	{0x0021 , "Slave needs INIT" },
	{0x0022 , "Slave needs PREOP" },
	{0x0023 , "Slave needs SAFEOP" },
	{0x0024 , "Invalid input mapping" },
	{0x0025 , "Invalid output mapping" },
	{0x0026 , "Inconsistent settings" },
	{0x0027 , "Freerun not supported" },
	{0x0028 , "Synchronisation not supported" },
	{0x0029 , "Freerun needs 3buffer mode" },
	{0x002A , "Background watchdog" },
	{0x002B , "No valid Inputs and Outputs" },
	{0x002C , "Fatal sync error" },
	{0x002D , "No sync error" }, // was "Invalid Output FMMU Configuration"
	{0x002E , "Invalid input FMMU configuration" },
	{0x0030 , "Invalid DC SYNC configuration" },
	{0x0031 , "Invalid DC latch configuration" },
	{0x0032 , "PLL error" },
	{0x0033 , "DC sync IO error" },
	{0x0034 , "DC sync timeout error" },
	{0x0035 , "DC invalid sync cycle time" },
	{0x0035 , "DC invalid sync0 cycle time" },
	{0x0035 , "DC invalid sync1 cycle time" },
	{0x0042 , "MBX_EOE" },
	{0x0043 , "MBX_COE" },
	{0x0044 , "MBX_FOE" },
	{0x0045 , "MBX_SOE" },
	{0x004F , "MBX_VOE" },
	{0x0050 , "EEPROM no access" },
	{0x0051 , "EEPROM error" },
	{0x0060 , "Slave restarted locally" },
	{0x0061 , "Device identification value updated" },
	{0x00f0 , "Application controller available" },
	{0xffff , "Unknown" }
};

#endif