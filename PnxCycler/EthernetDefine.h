
enum EthernetStepMode
{
	EthernetStepModeNone = 0,
	EthernetStepModeNormal,
	EthernetStepModePattern,
	EthernetStepModeParallelNormal,
	EthernetStepModeParallelPattern
};

enum EthernetOperationMode
{
	EthernetOperationModeInputRelayOn = 1,
	EthernetOperationModeOutputRelayOn
};

enum StepEndReason
{
	StepEndReasonTime = 1,
	StepEndReasonAmpere,
	StepEndReasonCapacity,
	StepEndReasonWatt,
	StepEndReasonVoltage
	//StepEndReasonTah
};