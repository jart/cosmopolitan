#ifndef COSMOPOLITAN_LIBC_NT_ENUM_ACCESSMASK_H_
#define COSMOPOLITAN_LIBC_NT_ENUM_ACCESSMASK_H_

#define kNtGenericRead            0x80000000u
#define kNtGenericWrite           0x40000000u
#define kNtGenericExecute         0x20000000u
#define kNtGenericAll             0x10000000u
#define kNtDelete                 0x00010000u
#define kNtReadControl            0x00020000u
#define kNtWriteDac               0x00040000u
#define kNtWriteOwner             0x00080000u
#define kNtSynchronize            0x00100000u
#define kNtStandardRightsRequired 0x000F0000u
#define kNtStandardRightsRead     kNtReadControl
#define kNtStandardRightsWrite    kNtReadControl
#define kNtStandardRightsExecute  kNtReadControl
#define kNtStandardRightsAll      0x001F0000u
#define kNtSpecificRightsAll      0x0000FFFFu
#define kNtAccessSystemSecurity   0x01000000u
#define kNtMaximumAllowed         0x02000000u
#define kNtFileReadData           0x0001u
#define kNtFileListDirectory      0x0001u
#define kNtFileWriteData          0x0002u
#define kNtFileAddFile            0x0002u
#define kNtFileAppendData         0x0004u
#define kNtFileAddSubdirectory    0x0004u
#define kNtFileCreatePipeInstance 0x0004u
#define kNtFileReadEa             0x0008u
#define kNtFileWriteEa            0x0010u
#define kNtFileExecute            0x0020u
#define kNtFileTraverse           0x0020u
#define kNtFileDeleteChild        0x0040u
#define kNtFileReadAttributes     0x0080u
#define kNtFileWriteAttributes    0x0100u
#define kNtFileAllAccess          (kNtStandardRightsRequired | kNtSynchronize | 0x1FFu)
#define kNtFileGenericRead                                           \
  (kNtStandardRightsRead | kNtFileReadData | kNtFileReadAttributes | \
   kNtFileReadEa | kNtSynchronize)
#define kNtFileGenericWrite                                             \
  (kNtStandardRightsWrite | kNtFileWriteData | kNtFileWriteAttributes | \
   kNtFileWriteEa | kNtFileAppendData | kNtSynchronize)
#define kNtFileGenericExecute                                          \
  (kNtStandardRightsExecute | kNtFileReadAttributes | kNtFileExecute | \
   kNtSynchronize)
#define kNtTokenAssignPrimary    0x0001u
#define kNtTokenDuplicate        0x0002u
#define kNtTokenImpersonate      0x0004u
#define kNtTokenQuery            0x0008u
#define kNtTokenQuerySource      0x0010u
#define kNtTokenAdjustPrivileges 0x0020u
#define kNtTokenAdjustGroups     0x0040u
#define kNtTokenAdjustDefault    0x0080u
#define kNtTokenAdjustSessionid  0x0100u
#define kNtTokenAllAccessP                                                 \
  (kNtStandardRightsRequired | kNtTokenAssignPrimary | kNtTokenDuplicate | \
   kNtTokenImpersonate | kNtTokenQuery | kNtTokenQuerySource |             \
   kNtTokenAdjustPrivileges | kNtTokenAdjustGroups | kNtTokenAdjustDefault)
#define kNtTokenAllAccess kNtTokenAllAccessP | kNtTokenAdjustSessionid
#define kNtTokenRead      kNtStandardRightsRead | kNtTokenQuery
#define kNtTokenWrite                                                         \
  (kNtStandardRightsWrite | kNtTokenAdjustPrivileges | kNtTokenAdjustGroups | \
   kNtTokenAdjustDefault)
#define kNtTokenExecute kNtStandardRightsExecute
#define kNtTokenTrustConstraintMask \
  (kNtStandardRightsRead | kNtTokenQuery | kNtTokenQuerySource)
#define kNtTokenAccessPseudoHandleWin8 kNtTokenQuery | kNtTokenQuerySource
#define kNtTokenAccessPseudoHandle     kNtTokenAccessPseudoHandleWin8

#endif /* COSMOPOLITAN_LIBC_NT_ENUM_ACCESSMASK_H_ */
