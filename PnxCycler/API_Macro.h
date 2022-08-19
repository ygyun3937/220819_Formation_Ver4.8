#ifndef _COMIECATSDK_API_MACRO_H_
#define _COMIECATSDK_API_MACRO_H_

#define ADDR_OF(a) (UINT)a  
#define GEN_DSP_CMDIDX() (g_DspCmdIdx++)

// NET ID ���� ó�����ִ� ��ũ�� (Return ���� �ִ� �Լ����� ���) //
#define ASSERT_NET_ID(NetId, pErrCode, RetVal, DlogLevel)\
	if(!IsValidNetID(NetId, pErrCode)){\
	if(pErrCode) *pErrCode = ecERR_INVALID_NETID;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ecERR_INVALID_NETID);\
	dlogEndLogObject(pDlogObj, ecERR_INVALID_NETID);\
	pDlogObj = NULL;\
	return RetVal;\
	}

#define ASSERT_SLAVE_INDEX(NetID, SlaveIndex, pErrCode, RetVal, DlogLevel)\
	if(!IsValidSlaveIndex(NetID, SlaveIndex, pErrCode)){\
	if(pErrCode) *pErrCode = ecERR_INVALID_SLAVEID;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ecERR_INVALID_SLAVEID);\
	dlogEndLogObject(pDlogObj, ecERR_INVALID_SLAVEID);\
	pDlogObj = NULL;\
	return RetVal;\
	}

#define ASSERT_SLAVE_ID(NetID, SlaveID, pErrCode, RetVal, DlogLevel)\
	if(!IsValidSlaveID(NetID, SlaveID, pErrCode)){\
	if(pErrCode) *pErrCode = ecERR_INVALID_SLAVEID;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ecERR_INVALID_SLAVEID);\
	dlogEndLogObject(pDlogObj, ecERR_INVALID_SLAVEID);\
	pDlogObj = NULL;\
	return RetVal;\
	}

#define ASSERT_AXIS_IDX(NetID, Axis, pErrCode, _RetVal, DlogLevel)\
	if(!IsValidAxis(NetID, Axis, pErrCode)){\
	if(pErrCode) *pErrCode = ecERR_INVALID_CHANNEL;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", _RetVal, ecERR_INVALID_CHANNEL);\
	dlogEndLogObject(pDlogObj, ecERR_INVALID_CHANNEL);\
	pDlogObj = NULL;\
	return _RetVal;\
	}

// var ���� refval���� ������ �����ϴ� ��ũ��. �ٽ� ���ؼ� �� ���� ���� ������ ������ ��ȯ�Ѵ�. //
#define ASSERT_EQ(var, refval, pErrBuf, ErrCode, RetVal)\
	if(var != refval){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ErrCode);\
	dlogEndLogObject(pDlogObj, ErrCode);\
	pDlogObj = NULL;\
	return RetVal;\
	}

// var ���� refval���� ���� ������ �����ϴ� ��ũ��. �ٽ� ���ؼ� �� ���� ������ ������ ��ȯ�Ѵ�. //
#define ASSERT_NE(var, refval, pErrBuf, ErrCode, RetVal)\
	if(var == refval){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ErrCode);\
	dlogEndLogObject(pDlogObj, ErrCode);\
	pDlogObj = NULL;\
	return RetVal;\
	}

// var ���� refval������ ������ �����ϴ� ��ũ��. �ٽ� ���ؼ� var ���� refval������ ũ�ų� ������ ������ ��ȯ�Ѵ�. //
#define ASSERT_LT(var, refval, pErrBuf, ErrCode, RetVal)\
	if(var >= refval){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ErrCode);\
	dlogEndLogObject(pDlogObj, ErrCode);\
	pDlogObj = NULL;\
	return RetVal;\
	}

// var ���� refval������ �۰�(Less)�� ���� �����ϴ� ��ũ��. �ٽ� ���ؼ� var ���� refval������ ũ�� ������ ��ȯ�Ѵ�. //
#define ASSERT_LE(var, refval, pErrBuf, ErrCode, RetVal)\
	if(var > refval){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", (int)RetVal, ErrCode);\
	dlogEndLogObject(pDlogObj, ErrCode);\
	pDlogObj = NULL;\
	return RetVal;\
	}

// var ���� refval������ ŭ(Greater)�� �����ϴ� ��ũ��. �ٽ� ���ؼ� var ���� refval������ �۰ų� ������ ������ ��ȯ�Ѵ�. //
#define ASSERT_GT(var, refval, pErrBuf, ErrCode, RetVal)\
	if(var <= refval){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ErrCode);\
	return RetVal;\
	}

// var ���� refval������ ũ�ų� ������ �����ϴ� ��ũ��. �ٽ� ���ؼ� var ���� refval������ ������ ������ ��ȯ�Ѵ�. //
#define ASSERT_GE(var, refval, pErrBuf, ErrCode, RetVal)\
	if(var < refval){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ErrCode);\
	dlogEndLogObject(pDlogObj, ErrCode);\
	pDlogObj = NULL;\
	return RetVal;\
	}

#define ASSERT_RANGE(var, min, max, pErrBuf, ErrCode, RetVal)\
	if(var < min || var > max){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ErrCode);\
	dlogEndLogObject(pDlogObj, ErrCode);\
	pDlogObj = NULL;\
	return RetVal;\
	}

// float���� var���� 0�� �ƴϸ� ����ó���ϴ� ��ũ�� //
#define ASSERT_F32_NZ(var, pErrBuf, ErrCode, RetVal)\
	if(var  > -0.0000001 && var < 0.0000001){\
	if(pErrBuf) *pErrBuf = ErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, ErrCode);\
	dlogEndLogObject(pDlogObj, ErrCode);\
	pDlogObj = NULL;\
	return RetVal;\
	}

#define API_FUNC_PROLOG() \
	TDlogOject *pDlogObj = NULL;\
	if(dlogIsEnabled()){\
	pDlogObj = dlogInitLogObject(DlogLevel, __FUNCTION__);\
	strcpy_s(pDlogObj->szFuncName, 127, __FUNCTION__);\
	}

#define API_FUNC_ERR_RET(ErrCode_Set, RetVal_Set)	TmpErrCode = ErrCode_Set;\
	if(ErrCode) *ErrCode = TmpErrCode;\
	RetVal = RetVal_Set;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, TmpErrCode);\
	dlogEndLogObject(pDlogObj, TmpErrCode);\
	pDlogObj = NULL;\
	return RetVal

#define API_FUNC_EPILOG() \
	if(ErrCode) *ErrCode = TmpErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n\n", RetVal, TmpErrCode);\
	dlogEndLogObject(pDlogObj, TmpErrCode);\
	pDlogObj = NULL;\
	return RetVal

#define API_FUNC_EPILOG_HEX() \
	if(ErrCode) *ErrCode = TmpErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, TmpErrCode);\
	dlogEndLogObject(pDlogObj, TmpErrCode);\
	pDlogObj = NULL;\
	return RetVal

#define API_FUNC_EPILOG_FLT() \
	if(ErrCode) *ErrCode = TmpErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=%g, Err=%d\n", RetVal, TmpErrCode);\
	dlogEndLogObject(pDlogObj, TmpErrCode);\
	pDlogObj = NULL;\
	return RetVal

#define API_FUNC_EPILOG_NoErrRet() \
	dlogPrintf_T (pDlogObj, "Ret=%d, Err=%d\n", RetVal, TmpErrCode);\
	dlogEndLogObject(pDlogObj, TmpErrCode);\
	pDlogObj = NULL;\
	return RetVal

#define API_FUNC_EPILOG_VOID() \
	if(ErrCode) *ErrCode = TmpErrCode;\
	dlogPrintf_T (pDlogObj, "Ret=void, Err=%d\n", TmpErrCode);\
	dlogEndLogObject(pDlogObj, TmpErrCode);\
	pDlogObj = NULL

#define API_FUNC_EPILOG_VOID_NoErrRet() \
	dlogPrintf (pDlogObj, "Ret=void, Err=%d\n", TmpErrCode);\
	dlogEndLogObject(pDlogObj, TmpErrCode);\
	pDlogObj = NULL

#endif