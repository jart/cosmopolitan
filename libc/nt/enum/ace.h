#ifndef COSMOPOLITAN_LIBC_NT_ENUM_ACE_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_ACE_H_

#define kNtAccessMinMsAceType   0x0
#define kNtAccessAllowedAceType 0x0
#define kNtAccessDeniedAceType  0x1
#define kNtSystemAuditAceType   0x2
#define kNtSystemAlarmAceType   0x3
#define kNtAccessMaxMsV2AceType 0x3

#define kNtAccessAllowedCompoundAceType 0x4
#define kNtAccessMaxMsV3AceType         0x4

#define kNtAccessMinMsObjectAceType   0x5
#define kNtAccessAllowedObjectAceType 0x5
#define kNtAccessDeniedObjectAceType  0x6
#define kNtSystemAuditObjectAceType   0x7
#define kNtSystemAlarmObjectAceType   0x8
#define kNtAccessMaxMsObjectAceType   0x8

#define kNtAccessMaxMsV4AceType 0x8
#define kNtAccessMaxMsAceType   0x8

#define kNtAccessAllowedCallbackAceType       0x9
#define kNtAccessDeniedCallbackAceType        0xA
#define kNtAccessAllowedCallbackObjectAceType 0xB
#define kNtAccessDeniedCallbackObjectAceType  0xC
#define kNtSystemAuditCallbackAceType         0xD
#define kNtSystemAlarmCallbackAceType         0xE
#define kNtSystemAuditCallbackObjectAceType   0xF
#define kNtSystemAlarmCallbackObjectAceType   0x10

#define kNtSystemMandatoryLabelAceType    0x11
#define kNtSystemResourceAttributeAceType 0x12
#define kNtSystemScopedPolicyIdAceType    0x13
#define kNtSystemProcessTrustLabelAceType 0x14
#define kNtSystemAccessFilterAceType      0x15
#define kNtAccessMaxMsV5AceType           0x15

#define kNtObjectInheritAce      0x1
#define kNtContainerInheritAce   0x2
#define kNtNoPropagateInheritAce 0x4
#define kNtInheritOnlyAce        0x8
#define kNtInheritedAce          0x10
#define kNtValidInheritFlags     0x1F

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_ACE_H_ */
