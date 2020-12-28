/*bin/echo   ' -*- mode:sh; indent-tabs-mode:nil; tab-width:8; coding:utf-8 -*-│
│vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8                               :vi│
╞══════════════════════════════════════════════════════════════════════════════╡
│ Copyright 2020 Justine Alexandra Roberts Tunney                              │
│                                                                              │
│ Permission to use, copy, modify, and/or distribute this software for         │
│ any purpose with or without fee is hereby granted, provided that the         │
│ above copyright notice and this permission notice appear in all copies.      │
│                                                                              │
│ THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL                │
│ WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED                │
│ WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE             │
│ AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL         │
│ DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR        │
│ PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER               │
│ TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR             │
│ PERFORMANCE OF THIS SOFTWARE.                                                │
╚────────────────────────────────────────────────────────────────'>/dev/null #*/
. libc/nt/codegen.sh

# The New Technology API
# » so many sections
#
#	Name							Actual							DLL		Hint	Arity
imp	'A_SHAInit'						A_SHAInit						ntdll		10
imp	'A_SHAUpdate'						A_SHAUpdate						ntdll		11
imp	'A_SHAFinal'						A_SHAFinal						ntdll		9
imp	'AbortDoc'						AbortDoc						gdi32		1011
imp	'AbortPath'						AbortPath						gdi32		1012
imp	'AbortSystemShutdownA'					AbortSystemShutdownA					advapi32	1005
imp	'AbortSystemShutdown'					AbortSystemShutdownW					advapi32	1006
imp	'AcceptEx'						AcceptEx						MsWSock		1	8
imp	'AccessCheck'						AccessCheck						advapi32	0	8	# KernelBase
imp	'AccessCheckAndAuditAlarm'				AccessCheckAndAuditAlarmW				advapi32	0		# KernelBase
imp	'AccessCheckAndAuditAlarmA'				AccessCheckAndAuditAlarmA				advapi32	1008
imp	'AccessCheckByType'					AccessCheckByType					advapi32	0		# KernelBase
imp	'AccessCheckByTypeAndAuditAlarm'			AccessCheckByTypeAndAuditAlarmW				advapi32	0		# KernelBase
imp	'AccessCheckByTypeAndAuditAlarmA'			AccessCheckByTypeAndAuditAlarmA				advapi32	1011
imp	'AccessCheckByTypeResultList'				AccessCheckByTypeResultList				advapi32	0		# KernelBase
imp	'AccessCheckByTypeResultListAndAuditAlarm'		AccessCheckByTypeResultListAndAuditAlarmW		advapi32	0		# KernelBase
imp	'AccessCheckByTypeResultListAndAuditAlarmA'		AccessCheckByTypeResultListAndAuditAlarmA		advapi32	1014
imp	'AccessCheckByTypeResultListAndAuditAlarmByHandle'	AccessCheckByTypeResultListAndAuditAlarmByHandleW	advapi32	0		# KernelBase
imp	'AccessCheckByTypeResultListAndAuditAlarmByHandleA'	AccessCheckByTypeResultListAndAuditAlarmByHandleA	advapi32	1015
imp	'AcquireStateLock'					AcquireStateLock					KernelBase	11
imp	'ActivateActCtx'					ActivateActCtx						kernel32	0		# KernelBase
imp	'ActivateActCtxWorker'					ActivateActCtxWorker					kernel32	4
imp	'ActivateKeyboardLayout'				ActivateKeyboardLayout					user32		1505
imp	'AddAccessAllowedAce'					AddAccessAllowedAce					advapi32	0		# KernelBase
imp	'AddAccessAllowedAceEx'					AddAccessAllowedAceEx					advapi32	0		# KernelBase
imp	'AddAccessAllowedObjectAce'				AddAccessAllowedObjectAce				advapi32	0		# KernelBase
imp	'AddAccessDeniedAce'					AddAccessDeniedAce					advapi32	0		# KernelBase
imp	'AddAccessDeniedAceEx'					AddAccessDeniedAceEx					advapi32	0		# KernelBase
imp	'AddAccessDeniedObjectAce'				AddAccessDeniedObjectAce				advapi32	0		# KernelBase
imp	'AddAce'						AddAce							advapi32	0		# KernelBase
imp	'AddAtomA'						AddAtomA						kernel32	5
imp	'AddAtom'						AddAtomW						kernel32	6
imp	'AddAuditAccessAce'					AddAuditAccessAce					advapi32	0		# KernelBase
imp	'AddAuditAccessAceEx'					AddAuditAccessAceEx					advapi32	0		# KernelBase
imp	'AddAuditAccessObjectAce'				AddAuditAccessObjectAce					advapi32	0		# KernelBase
imp	'AddClipboardFormatListener'				AddClipboardFormatListener				user32		1506
imp	'AddConditionalAce'					AddConditionalAce					advapi32	1028
imp	'AddConsoleAliasA'					AddConsoleAliasA					kernel32	0		# KernelBase
imp	'AddConsoleAlias'					AddConsoleAliasW					kernel32	0		# KernelBase
imp	'AddDllDirectory'					AddDllDirectory						kernel32	0		# KernelBase
imp	'AddExtensionProgId'					AddExtensionProgId					KernelBase	26
imp	'AddFontMemResourceEx'					AddFontMemResourceEx					gdi32		1017
imp	'AddFontResourceA'					AddFontResourceA					gdi32		1018
imp	'AddFontResourceExA'					AddFontResourceExA					gdi32		1019
imp	'AddFontResourceEx'					AddFontResourceExW					gdi32		1020
imp	'AddFontResourceTracking'				AddFontResourceTracking					gdi32		1021
imp	'AddFontResource'					AddFontResourceW					gdi32		1022
imp	'AddIntegrityLabelToBoundaryDescriptor'			AddIntegrityLabelToBoundaryDescriptor			kernel32	10
imp	'AddLocalAlternateComputerNameA'			AddLocalAlternateComputerNameA				kernel32	11
imp	'AddLocalAlternateComputerName'				AddLocalAlternateComputerNameW				kernel32	12
imp	'AddMIMEFileTypesPS'					AddMIMEFileTypesPS					url		102
imp	'AddMandatoryAce'					AddMandatoryAce						advapi32	0		# KernelBase
imp	'AddPackageToFamilyXref'				AddPackageToFamilyXref					KernelBase	28
imp	'AddRefActCtx'						AddRefActCtx						kernel32	0		# KernelBase
imp	'AddRefActCtxWorker'					AddRefActCtxWorker					kernel32	14
imp	'AddResourceAttributeAce'				AddResourceAttributeAce					kernel32	0		# KernelBase
imp	'AddSIDToBoundaryDescriptor'				AddSIDToBoundaryDescriptor				kernel32	0		# KernelBase
imp	'AddScopedPolicyIDAce'					AddScopedPolicyIDAce					kernel32	0		# KernelBase
imp	'AddSecureMemoryCacheCallback'				AddSecureMemoryCacheCallback				kernel32	18
imp	'AddUsersToEncryptedFile'				AddUsersToEncryptedFile					advapi32	1030
imp	'AddUsersToEncryptedFileEx'				AddUsersToEncryptedFileEx				advapi32	1031
imp	'AddVectoredExceptionHandler'				AddVectoredExceptionHandler				kernel32	0	2	# KernelBase
imp	'AddVectoredContinueHandler'				AddVectoredContinueHandler				kernel32	0	2	# KernelBase
imp	'RemoveVectoredExceptionHandler'			RemoveVectoredExceptionHandler				kernel32	0	1	# KernelBase
imp	'RemoveVectoredContinueHandler'				RemoveVectoredContinueHandler				kernel32	0	1	# KernelBase
imp	'AdjustCalendarDate'					AdjustCalendarDate					kernel32	21
imp	'AdjustTokenGroups'					AdjustTokenGroups					advapi32	0		# KernelBase
imp	'AdjustTokenPrivileges'					AdjustTokenPrivileges					advapi32	0	6	# KernelBase
imp	'AdjustWindowRect'					AdjustWindowRect					user32		1507
imp	'AdjustWindowRectEx'					AdjustWindowRectEx					user32		1508
imp	'AdjustWindowRectExForDpi'				AdjustWindowRectExForDpi				user32		1509
imp	'AlignRects'						AlignRects						user32		1510
imp	'AllocConsole'						AllocConsole						kernel32	0	0	# KernelBase
imp	'AllocateAndInitializeSid'				AllocateAndInitializeSid				advapi32	0		# KernelBase
imp	'AllocateLocallyUniqueId'				AllocateLocallyUniqueId					advapi32	0		# KernelBase
imp	'AllocateUserPhysicalPages'				AllocateUserPhysicalPages				kernel32	0		# KernelBase
imp	'AllocateUserPhysicalPagesNuma'				AllocateUserPhysicalPagesNuma				kernel32	0		# KernelBase
imp	'AllowForegroundActivation'				AllowForegroundActivation				user32		1511
imp	'AllowSetForegroundWindow'				AllowSetForegroundWindow				user32		1512
imp	'AlpcAdjustCompletionListConcurrencyCount'		AlpcAdjustCompletionListConcurrencyCount		ntdll		12
imp	'AlpcFreeCompletionListMessage'				AlpcFreeCompletionListMessage				ntdll		13
imp	'AlpcGetCompletionListLastMessageInformation'		AlpcGetCompletionListLastMessageInformation		ntdll		14
imp	'AlpcGetCompletionListMessageAttributes'		AlpcGetCompletionListMessageAttributes			ntdll		15
imp	'AlpcGetHeaderSize'					AlpcGetHeaderSize					ntdll		16
imp	'AlpcGetMessageAttribute'				AlpcGetMessageAttribute					ntdll		17
imp	'AlpcGetMessageFromCompletionList'			AlpcGetMessageFromCompletionList			ntdll		18
imp	'AlpcGetOutstandingCompletionListMessageCount'		AlpcGetOutstandingCompletionListMessageCount		ntdll		19
imp	'AlpcInitializeMessageAttribute'			AlpcInitializeMessageAttribute				ntdll		20
imp	'AlpcMaxAllowedMessageLength'				AlpcMaxAllowedMessageLength				ntdll		21
imp	'AlpcRegisterCompletionList'				AlpcRegisterCompletionList				ntdll		22
imp	'AlpcRegisterCompletionListWorkerThread'		AlpcRegisterCompletionListWorkerThread			ntdll		23
imp	'AlpcRundownCompletionList'				AlpcRundownCompletionList				ntdll		24
imp	'AlpcUnregisterCompletionList'				AlpcUnregisterCompletionList				ntdll		25
imp	'AlpcUnregisterCompletionListWorkerThread'		AlpcUnregisterCompletionListWorkerThread		ntdll		26
imp	'AngleArc'						AngleArc						gdi32		1023
imp	'AnimatePalette'					AnimatePalette						gdi32		1024
imp	'AnimateWindow'						AnimateWindow						user32		1513	3
imp	'AnyLinkedFonts'					AnyLinkedFonts						gdi32		1025
imp	'AnyPopup'						AnyPopup						user32		1514
imp	'ApiSetQueryApiSetPresence'				ApiSetQueryApiSetPresence				ntdll		27
imp	'AppCompat_RunDLLW'					AppCompat_RunDLLW					shell32		255
imp	'AppContainerDeriveSidFromMoniker'			AppContainerDeriveSidFromMoniker			KernelBase	42
imp	'AppContainerFreeMemory'				AppContainerFreeMemory					KernelBase	43
imp	'AppContainerLookupDisplayNameMrtReference'		AppContainerLookupDisplayNameMrtReference		KernelBase	44
imp	'AppContainerLookupMoniker'				AppContainerLookupMoniker				KernelBase	45
imp	'AppContainerRegisterSid'				AppContainerRegisterSid					KernelBase	46
imp	'AppContainerUnregisterSid'				AppContainerUnregisterSid				KernelBase	47
imp	'AppPolicyGetClrCompat'					AppPolicyGetClrCompat					KernelBase	48
imp	'AppPolicyGetCreateFileAccess'				AppPolicyGetCreateFileAccess				KernelBase	49
imp	'AppPolicyGetLifecycleManagement'			AppPolicyGetLifecycleManagement				KernelBase	50
imp	'AppPolicyGetMediaFoundationCodecLoading'		AppPolicyGetMediaFoundationCodecLoading			KernelBase	51
imp	'AppPolicyGetProcessTerminationMethod'			AppPolicyGetProcessTerminationMethod			KernelBase	52
imp	'AppPolicyGetShowDeveloperDiagnostic'			AppPolicyGetShowDeveloperDiagnostic			KernelBase	53
imp	'AppPolicyGetThreadInitializationType'			AppPolicyGetThreadInitializationType			KernelBase	54
imp	'AppPolicyGetWindowingModel'				AppPolicyGetWindowingModel				KernelBase	55
imp	'AppXFreeMemory'					AppXFreeMemory						KernelBase	56
imp	'AppXGetApplicationData'				AppXGetApplicationData					KernelBase	57
imp	'AppXGetDevelopmentMode'				AppXGetDevelopmentMode					KernelBase	58
imp	'AppXGetOSMaxVersionTested'				AppXGetOSMaxVersionTested				KernelBase	59
imp	'AppXGetOSMinVersion'					AppXGetOSMinVersion					KernelBase	60
imp	'AppXGetPackageCapabilities'				AppXGetPackageCapabilities				KernelBase	61
imp	'AppXGetPackageSid'					AppXGetPackageSid					KernelBase	62
imp	'AppXLookupDisplayName'					AppXLookupDisplayName					KernelBase	63
imp	'AppXLookupMoniker'					AppXLookupMoniker					KernelBase	64
imp	'AppXPostSuccessExtension'				AppXPostSuccessExtension				KernelBase	65
imp	'AppXPreCreationExtension'				AppXPreCreationExtension				KernelBase	66
imp	'AppXReleaseAppXContext'				AppXReleaseAppXContext					KernelBase	67
imp	'AppXUpdatePackageCapabilities'				AppXUpdatePackageCapabilities				KernelBase	68
imp	'AppendMenuA'						AppendMenuA						user32		1515	4
imp	'AppendMenu'						AppendMenuW						user32		1516	4
imp	'ApplicationRecoveryFinished'				ApplicationRecoveryFinished				kernel32	34
imp	'ApplicationRecoveryInProgress'				ApplicationRecoveryInProgress				kernel32	35
imp	'ApplicationUserModelIdFromProductId'			ApplicationUserModelIdFromProductId			KernelBase	69
imp	'Arc'							Arc							gdi32		1026
imp	'ArcTo'							ArcTo							gdi32		1027
imp	'AreAllAccessesGranted'					AreAllAccessesGranted					advapi32	0		# KernelBase
imp	'AreAnyAccessesGranted'					AreAnyAccessesGranted					advapi32	0		# KernelBase
imp	'AreDpiAwarenessContextsEqual'				AreDpiAwarenessContextsEqual				user32		1517
imp	'AreFileApisANSI'					AreFileApisANSI						kernel32	0		# KernelBase
imp	'AreThereVisibleLogoffScriptsInternal'			AreThereVisibleLogoffScriptsInternal			KernelBase	73
imp	'AreThereVisibleShutdownScriptsInternal'		AreThereVisibleShutdownScriptsInternal			KernelBase	74
imp	'ArrangeIconicWindows'					ArrangeIconicWindows					user32		1518
imp	'AssignProcessToJobObject'				AssignProcessToJobObject				kernel32	37
imp	'AssocCreateForClasses'					AssocCreateForClasses					shell32		263
imp	'AssocGetDetailsOfPropKey'				AssocGetDetailsOfPropKey				shell32		267
imp	'AttachConsole'						AttachConsole						kernel32	0	1	# KernelBase
imp	'AttachThreadInput'					AttachThreadInput					user32		1519
imp	'AuditComputeEffectivePolicyBySid'			AuditComputeEffectivePolicyBySid			advapi32	1038
imp	'AuditComputeEffectivePolicyByToken'			AuditComputeEffectivePolicyByToken			advapi32	1039
imp	'AuditEnumerateCategories'				AuditEnumerateCategories				advapi32	1040
imp	'AuditEnumeratePerUserPolicy'				AuditEnumeratePerUserPolicy				advapi32	1041
imp	'AuditEnumerateSubCategories'				AuditEnumerateSubCategories				advapi32	1042
imp	'AuditFree'						AuditFree						advapi32	1043
imp	'AuditLookupCategoryGuidFromCategoryId'			AuditLookupCategoryGuidFromCategoryId			advapi32	1044
imp	'AuditLookupCategoryIdFromCategoryGuid'			AuditLookupCategoryIdFromCategoryGuid			advapi32	1045
imp	'AuditLookupCategoryNameA'				AuditLookupCategoryNameA				advapi32	1046
imp	'AuditLookupCategoryName'				AuditLookupCategoryNameW				advapi32	1047
imp	'AuditLookupSubCategoryNameA'				AuditLookupSubCategoryNameA				advapi32	1048
imp	'AuditLookupSubCategoryName'				AuditLookupSubCategoryNameW				advapi32	1049
imp	'AuditQueryGlobalSaclA'					AuditQueryGlobalSaclA					advapi32	1050
imp	'AuditQueryGlobalSacl'					AuditQueryGlobalSaclW					advapi32	1051
imp	'AuditQueryPerUserPolicy'				AuditQueryPerUserPolicy					advapi32	1052
imp	'AuditQuerySecurity'					AuditQuerySecurity					advapi32	1053
imp	'AuditQuerySystemPolicy'				AuditQuerySystemPolicy					advapi32	1054
imp	'AuditSetGlobalSaclA'					AuditSetGlobalSaclA					advapi32	1055
imp	'AuditSetGlobalSacl'					AuditSetGlobalSaclW					advapi32	1056
imp	'AuditSetPerUserPolicy'					AuditSetPerUserPolicy					advapi32	1057
imp	'AuditSetSecurity'					AuditSetSecurity					advapi32	1058
imp	'AuditSetSystemPolicy'					AuditSetSystemPolicy					advapi32	1059
imp	'AutodialHookCallback'					AutodialHookCallback					url		103
imp	'BRUSHOBJ_hGetColorTransform'				BRUSHOBJ_hGetColorTransform				gdi32		1028
imp	'BRUSHOBJ_pvAllocRbrush'				BRUSHOBJ_pvAllocRbrush					gdi32		1029
imp	'BRUSHOBJ_pvGetRbrush'					BRUSHOBJ_pvGetRbrush					gdi32		1030
imp	'BRUSHOBJ_ulGetBrushColor'				BRUSHOBJ_ulGetBrushColor				gdi32		1031
imp	'BackupEventLogA'					BackupEventLogA						advapi32	1060
imp	'BackupEventLog'					BackupEventLogW						advapi32	1061
imp	'BackupRead'						BackupRead						kernel32	39
imp	'BackupSeek'						BackupSeek						kernel32	40
imp	'BackupWrite'						BackupWrite						kernel32	41
imp	'BaseCheckAppcompatCache'				BaseCheckAppcompatCache					KernelBase	76
imp	'BaseCheckAppcompatCacheEx'				BaseCheckAppcompatCacheEx				KernelBase	77
imp	'BaseCheckAppcompatCacheExWorker'			BaseCheckAppcompatCacheExWorker				kernel32	44
imp	'BaseCheckAppcompatCacheWorker'				BaseCheckAppcompatCacheWorker				kernel32	45
imp	'BaseCheckElevation'					BaseCheckElevation					kernel32	46
imp	'BaseCleanupAppcompatCacheSupport'			BaseCleanupAppcompatCacheSupport			KernelBase	78
imp	'BaseCleanupAppcompatCacheSupportWorker'		BaseCleanupAppcompatCacheSupportWorker			kernel32	48
imp	'BaseDestroyVDMEnvironment'				BaseDestroyVDMEnvironment				kernel32	49
imp	'BaseDllFreeResourceId'					BaseDllFreeResourceId					KernelBase	79
imp	'BaseDllMapResourceId'					BaseDllMapResourceIdW					KernelBase	80
imp	'BaseDllReadWriteIniFile'				BaseDllReadWriteIniFile					kernel32	50
imp	'BaseDumpAppcompatCache'				BaseDumpAppcompatCache					KernelBase	81
imp	'BaseDumpAppcompatCacheWorker'				BaseDumpAppcompatCacheWorker				kernel32	52
imp	'BaseElevationPostProcessing'				BaseElevationPostProcessing				kernel32	53
imp	'BaseFlushAppcompatCache'				BaseFlushAppcompatCache					KernelBase	82
imp	'BaseFlushAppcompatCacheWorker'				BaseFlushAppcompatCacheWorker				kernel32	55
imp	'BaseFormatObjectAttributes'				BaseFormatObjectAttributes				KernelBase	83
imp	'BaseFormatTimeOut'					BaseFormatTimeOut					kernel32	57
imp	'BaseFreeAppCompatDataForProcess'			BaseFreeAppCompatDataForProcess				KernelBase	84
imp	'BaseFreeAppCompatDataForProcessWorker'			BaseFreeAppCompatDataForProcessWorker			kernel32	58
imp	'BaseGenerateAppCompatData'				BaseGenerateAppCompatData				kernel32	59
imp	'BaseGetConsoleReference'				BaseGetConsoleReference					KernelBase	85
imp	'BaseGetNamedObjectDirectory'				BaseGetNamedObjectDirectory				KernelBase	86
imp	'BaseInitAppcompatCacheSupport'				BaseInitAppcompatCacheSupport				KernelBase	87
imp	'BaseInitAppcompatCacheSupportWorker'			BaseInitAppcompatCacheSupportWorker			kernel32	62
imp	'BaseIsAppcompatInfrastructureDisabled'			BaseIsAppcompatInfrastructureDisabled			KernelBase	88
imp	'BaseIsAppcompatInfrastructureDisabledWorker'		BaseIsAppcompatInfrastructureDisabledWorker		kernel32	64
imp	'BaseIsDosApplication'					BaseIsDosApplication					kernel32	65
imp	'BaseMarkFileForDelete'					BaseMarkFileForDelete					KernelBase	89
imp	'BaseQueryModuleData'					BaseQueryModuleData					kernel32	66
imp	'BaseReadAppCompatDataForProcess'			BaseReadAppCompatDataForProcess				KernelBase	90
imp	'BaseReadAppCompatDataForProcessWorker'			BaseReadAppCompatDataForProcessWorker			kernel32	67
imp	'BaseRegCloseKey'					BaseRegCloseKey						advapi32	1062
imp	'BaseRegCreateKey'					BaseRegCreateKey					advapi32	1063
imp	'BaseRegDeleteKeyEx'					BaseRegDeleteKeyEx					advapi32	1064
imp	'BaseRegDeleteValue'					BaseRegDeleteValue					advapi32	1065
imp	'BaseRegFlushKey'					BaseRegFlushKey						advapi32	1066
imp	'BaseRegGetVersion'					BaseRegGetVersion					advapi32	1067
imp	'BaseRegLoadKey'					BaseRegLoadKey						advapi32	1068
imp	'BaseRegOpenKey'					BaseRegOpenKey						advapi32	1069
imp	'BaseRegRestoreKey'					BaseRegRestoreKey					advapi32	1070
imp	'BaseRegSaveKeyEx'					BaseRegSaveKeyEx					advapi32	1071
imp	'BaseRegSetKeySecurity'					BaseRegSetKeySecurity					advapi32	1072
imp	'BaseRegSetValue'					BaseRegSetValue						advapi32	1073
imp	'BaseRegUnLoadKey'					BaseRegUnLoadKey					advapi32	1074
imp	'BaseSetLastNTError'					BaseSetLastNTError					kernel32	68
imp	'BaseThreadInitThunk'					BaseThreadInitThunk					kernel32	69
imp	'BaseUpdateAppcompatCache'				BaseUpdateAppcompatCache				KernelBase	91
imp	'BaseUpdateAppcompatCacheWorker'			BaseUpdateAppcompatCacheWorker				kernel32	71
imp	'BaseUpdateVDMEntry'					BaseUpdateVDMEntry					kernel32	72
imp	'BaseVerifyUnicodeString'				BaseVerifyUnicodeString					kernel32	73
imp	'BaseWriteErrorElevationRequiredEvent'			BaseWriteErrorElevationRequiredEvent			kernel32	74
imp	'Basep8BitStringToDynamicUnicodeString'			Basep8BitStringToDynamicUnicodeString			kernel32	75
imp	'BasepAdjustObjectAttributesForPrivateNamespace'	BasepAdjustObjectAttributesForPrivateNamespace		KernelBase	92
imp	'BasepAllocateActivationContextActivationBlock'		BasepAllocateActivationContextActivationBlock		kernel32	76
imp	'BasepAnsiStringToDynamicUnicodeString'			BasepAnsiStringToDynamicUnicodeString			kernel32	77
imp	'BasepAppContainerEnvironmentExtension'			BasepAppContainerEnvironmentExtension			kernel32	78
imp	'BasepAppXExtension'					BasepAppXExtension					kernel32	79
imp	'BasepCheckAppCompat'					BasepCheckAppCompat					kernel32	80
imp	'BasepCheckWebBladeHashes'				BasepCheckWebBladeHashes				kernel32	81
imp	'BasepCheckWinSaferRestrictions'			BasepCheckWinSaferRestrictions				kernel32	82
imp	'BasepConstructSxsCreateProcessMessage'			BasepConstructSxsCreateProcessMessage			kernel32	83
imp	'BasepCopyEncryption'					BasepCopyEncryption					kernel32	84
imp	'BasepCopyFileCallback'					BasepCopyFileCallback					KernelBase	93
imp	'BasepCopyFileEx'					BasepCopyFileExW					KernelBase	94
imp	'BasepFreeActivationContextActivationBlock'		BasepFreeActivationContextActivationBlock		kernel32	85
imp	'BasepFreeAppCompatData'				BasepFreeAppCompatData					kernel32	86
imp	'BasepGetAppCompatData'					BasepGetAppCompatData					kernel32	87
imp	'BasepGetComputerNameFromNtPath'			BasepGetComputerNameFromNtPath				kernel32	88
imp	'BasepGetExeArchType'					BasepGetExeArchType					kernel32	89
imp	'BasepInitAppCompatData'				BasepInitAppCompatData					kernel32	90
imp	'BasepIsProcessAllowed'					BasepIsProcessAllowed					kernel32	91
imp	'BasepMapModuleHandle'					BasepMapModuleHandle					kernel32	92
imp	'BasepNotifyLoadStringResource'				BasepNotifyLoadStringResource				kernel32	93
imp	'BasepNotifyTrackingService'				BasepNotifyTrackingService				KernelBase	95
imp	'BasepPostSuccessAppXExtension'				BasepPostSuccessAppXExtension				kernel32	94
imp	'BasepProcessInvalidImage'				BasepProcessInvalidImage				kernel32	95
imp	'BasepQueryAppCompat'					BasepQueryAppCompat					kernel32	96
imp	'BasepQueryModuleChpeSettings'				BasepQueryModuleChpeSettings				kernel32	97
imp	'BasepReleaseAppXContext'				BasepReleaseAppXContext					kernel32	98
imp	'BasepReleaseSxsCreateProcessUtilityStruct'		BasepReleaseSxsCreateProcessUtilityStruct		kernel32	99
imp	'BasepReportFault'					BasepReportFault					kernel32	100
imp	'BasepSetFileEncryptionCompression'			BasepSetFileEncryptionCompression			kernel32	101
imp	'Beep'							Beep							kernel32	0		# KernelBase
imp	'BeginDeferWindowPos'					BeginDeferWindowPos					user32		1520
imp	'BeginGdiRendering'					BeginGdiRendering					gdi32		1032
imp	'BeginPaint'						BeginPaint						user32		1521	2
imp	'BeginPath'						BeginPath						gdi32		1033
imp	'BeginUpdateResourceA'					BeginUpdateResourceA					kernel32	103
imp	'BeginUpdateResource'					BeginUpdateResourceW					kernel32	104
imp	'BindIoCompletionCallback'				BindIoCompletionCallback				kernel32	105
imp	'BitBlt'						BitBlt							gdi32		1034	9
imp	'BlockInput'						BlockInput						user32		1522
imp	'BringWindowToTop'					BringWindowToTop					user32		1523	1
imp	'BroadcastSystemMessageA'				BroadcastSystemMessageA					user32		1525
imp	'BroadcastSystemMessageExA'				BroadcastSystemMessageExA				user32		1526
imp	'BroadcastSystemMessageEx'				BroadcastSystemMessageExW				user32		1527
imp	'BroadcastSystemMessage'				BroadcastSystemMessageW					user32		1528
imp	'BuildCommDCBA'						BuildCommDCBA						kernel32	106
imp	'BuildCommDCBAndTimeoutsA'				BuildCommDCBAndTimeoutsA				kernel32	107
imp	'BuildCommDCBAndTimeouts'				BuildCommDCBAndTimeoutsW				kernel32	108
imp	'BuildCommDCBW'						BuildCommDCBW						kernel32	109
imp	'BuildExplicitAccessWithNameA'				BuildExplicitAccessWithNameA				advapi32	1075
imp	'BuildExplicitAccessWithName'				BuildExplicitAccessWithNameW				advapi32	1076
imp	'BuildImpersonateExplicitAccessWithNameA'		BuildImpersonateExplicitAccessWithNameA			advapi32	1077
imp	'BuildImpersonateExplicitAccessWithName'		BuildImpersonateExplicitAccessWithNameW			advapi32	1078
imp	'BuildImpersonateTrusteeA'				BuildImpersonateTrusteeA				advapi32	1079
imp	'BuildImpersonateTrustee'				BuildImpersonateTrusteeW				advapi32	1080
imp	'BuildReasonArray'					BuildReasonArray					user32		1529
imp	'BuildSecurityDescriptorA'				BuildSecurityDescriptorA				advapi32	1081
imp	'BuildSecurityDescriptor'				BuildSecurityDescriptorW				advapi32	1082
imp	'BuildTrusteeWithNameA'					BuildTrusteeWithNameA					advapi32	1083
imp	'BuildTrusteeWithName'					BuildTrusteeWithNameW					advapi32	1084
imp	'BuildTrusteeWithObjectsAndNameA'			BuildTrusteeWithObjectsAndNameA				advapi32	1085
imp	'BuildTrusteeWithObjectsAndName'			BuildTrusteeWithObjectsAndNameW				advapi32	1086
imp	'BuildTrusteeWithObjectsAndSidA'			BuildTrusteeWithObjectsAndSidA				advapi32	1087
imp	'BuildTrusteeWithObjectsAndSid'				BuildTrusteeWithObjectsAndSidW				advapi32	1088
imp	'BuildTrusteeWithSidA'					BuildTrusteeWithSidA					advapi32	1089
imp	'BuildTrusteeWithSid'					BuildTrusteeWithSidW					advapi32	1090
imp	'CDefFolderMenu_Create2'				CDefFolderMenu_Create2					shell32		701
imp	'CIDLData_CreateFromIDArray'				CIDLData_CreateFromIDArray				shell32		83
imp	'CLIPOBJ_bEnum'						CLIPOBJ_bEnum						gdi32		1035
imp	'CLIPOBJ_cEnumStart'					CLIPOBJ_cEnumStart					gdi32		1036
imp	'CLIPOBJ_ppoGetPath'					CLIPOBJ_ppoGetPath					gdi32		1037
imp	'CLOSE_LOCAL_HANDLE_INTERNAL'				CLOSE_LOCAL_HANDLE_INTERNAL				KernelBase	97
imp	'CStorageItem_GetValidatedStorageItemObject'		CStorageItem_GetValidatedStorageItemObject		shell32		937
imp	'CalcMenuBar'						CalcMenuBar						user32		1530
imp	'CalculatePopupWindowPosition'				CalculatePopupWindowPosition				user32		1531
imp	'CallEnclave'						CallEnclave						KernelBase	98
imp	'CallMsgFilter'						CallMsgFilterW						user32		1534
imp	'CallMsgFilterA'					CallMsgFilterA						user32		1533
imp	'CallNamedPipe'						CallNamedPipeW						kernel32	0	7	# KernelBase
imp	'CallNamedPipeA'					CallNamedPipeA						kernel32	110	7
imp	'CallNextHookEx'					CallNextHookEx						user32		1535	4
imp	'CallWindowProcA'					CallWindowProcA						user32		1536
imp	'CallWindowProc'					CallWindowProcW						user32		1537
imp	'CallbackMayRunLong'					CallbackMayRunLong					kernel32	0		# KernelBase
imp	'CancelDC'						CancelDC						gdi32		1038
imp	'CancelDeviceWakeupRequest'				CancelDeviceWakeupRequest				kernel32	113
imp	'CancelIo'						CancelIo						kernel32	0	1	# KernelBase
imp	'CancelIoEx'						CancelIoEx						kernel32	0	2	# KernelBase
imp	'CancelOverlappedAccess'				CancelOverlappedAccess					advapi32	1091
imp	'CancelShutdown'					CancelShutdown						user32		1538
imp	'CancelSynchronousIo'					CancelSynchronousIo					kernel32	0	1	# KernelBase
imp	'CancelTimerQueueTimer'					CancelTimerQueueTimer					kernel32	118
imp	'CancelWaitableTimer'					CancelWaitableTimer					kernel32	0		# KernelBase
imp	'CascadeChildWindows'					CascadeChildWindows					user32		1539
imp	'CascadeWindows'					CascadeWindows						user32		1540
imp	'CeipIsOptedIn'						CeipIsOptedIn						kernel32	0		# KernelBase
imp	'ChangeClipboardChain'					ChangeClipboardChain					user32		1541
imp	'ChangeDisplaySettingsA'				ChangeDisplaySettingsA					user32		1542
imp	'ChangeDisplaySettingsExA'				ChangeDisplaySettingsExA				user32		1543
imp	'ChangeDisplaySettingsEx'				ChangeDisplaySettingsExW				user32		1544
imp	'ChangeDisplaySettings'					ChangeDisplaySettingsW					user32		1545
imp	'ChangeMenuA'						ChangeMenuA						user32		1546
imp	'ChangeMenu'						ChangeMenuW						user32		1547
imp	'ChangeServiceConfig2A'					ChangeServiceConfig2A					advapi32	1092
imp	'ChangeServiceConfig2W'					ChangeServiceConfig2W					advapi32	1093
imp	'ChangeServiceConfigA'					ChangeServiceConfigA					advapi32	1094
imp	'ChangeServiceConfig'					ChangeServiceConfigW					advapi32	1095
imp	'ChangeTimerQueueTimer'					ChangeTimerQueueTimer					kernel32	0		# KernelBase
imp	'ChangeWindowMessageFilter'				ChangeWindowMessageFilter				user32		1548
imp	'ChangeWindowMessageFilterEx'				ChangeWindowMessageFilterEx				user32		1549
imp	'CharLowerA'						CharLowerA						KernelBase	108
imp	'CharLowerBuffA'					CharLowerBuffA						KernelBase	109
imp	'CharLowerBuff'						CharLowerBuffW						KernelBase	110
imp	'CharLower'						CharLowerW						KernelBase	111
imp	'CharNextA'						CharNextA						KernelBase	112
imp	'CharNextExA'						CharNextExA						KernelBase	113
imp	'CharNext'						CharNextW						KernelBase	114
imp	'CharPrevA'						CharPrevA						KernelBase	115
imp	'CharPrevExA'						CharPrevExA						KernelBase	116
imp	'CharPrev'						CharPrevW						KernelBase	117
imp	'CharToOemA'						CharToOemA						user32		1565
imp	'CharToOemBuff'						CharToOemBuffW						user32		1567
imp	'CharToOemBuffA'					CharToOemBuffA						user32		1566
imp	'CharToOem'						CharToOemW						user32		1568
imp	'CharUpper'						CharUpperW						KernelBase	121
imp	'CharUpperA'						CharUpperA						KernelBase	118
imp	'CharUpperBuffA'					CharUpperBuffA						KernelBase	119
imp	'CharUpperBuff'						CharUpperBuffW						KernelBase	120
imp	'CheckAllowDecryptedRemoteDestinationPolicy'		CheckAllowDecryptedRemoteDestinationPolicy		KernelBase	122
imp	'CheckColorsInGamut'					CheckColorsInGamut					gdi32		1039
imp	'CheckDBCSEnabledExt'					CheckDBCSEnabledExt					user32		1573
imp	'CheckDlgButton'					CheckDlgButton						user32		1574
imp	'CheckElevation'					CheckElevation						kernel32	123
imp	'CheckElevationEnabled'					CheckElevationEnabled					kernel32	124
imp	'CheckEscapes'						CheckEscapesW						shell32		268
imp	'CheckForHiberboot'					CheckForHiberboot					advapi32	1096
imp	'CheckForReadOnlyResource'				CheckForReadOnlyResource				kernel32	125
imp	'CheckForReadOnlyResourceFilter'			CheckForReadOnlyResourceFilter				kernel32	126
imp	'CheckGroupPolicyEnabled'				CheckGroupPolicyEnabled					KernelBase	123
imp	'CheckIfStateChangeNotificationExists'			CheckIfStateChangeNotificationExists			KernelBase	124
imp	'CheckMenuItem'						CheckMenuItem						user32		1575
imp	'CheckMenuRadioItem'					CheckMenuRadioItem					user32		1576
imp	'CheckNameLegalDOS8Dot3A'				CheckNameLegalDOS8Dot3A					kernel32	127
imp	'CheckNameLegalDOS8Dot3W'				CheckNameLegalDOS8Dot3W					kernel32	128
imp	'CheckProcessForClipboardAccess'			CheckProcessForClipboardAccess				user32		1577
imp	'CheckProcessSession'					CheckProcessSession					user32		1578
imp	'CheckRadioButton'					CheckRadioButton					user32		1579
imp	'CheckRemoteDebuggerPresent'				CheckRemoteDebuggerPresent				kernel32	0	2	# KernelBase
imp	'CheckTokenCapability'					CheckTokenCapability					kernel32	0		# KernelBase
imp	'CheckTokenMembership'					CheckTokenMembership					advapi32	0		# KernelBase
imp	'CheckTokenMembershipEx'				CheckTokenMembershipEx					kernel32	0		# KernelBase
imp	'CheckWindowThreadDesktop'				CheckWindowThreadDesktop				user32		1580
imp	'ChildWindowFromPoint'					ChildWindowFromPoint					user32		1581
imp	'ChildWindowFromPointEx'				ChildWindowFromPointEx					user32		1582
imp	'ChooseColorA'						ChooseColorA						comdlg32	102	1
imp	'ChooseColor'						ChooseColorW						comdlg32	103	1
imp	'ChooseFontA'						ChooseFontA						comdlg32	104	1
imp	'ChooseFont'						ChooseFontW						comdlg32	105	1
imp	'ChoosePixelFormat'					ChoosePixelFormat					gdi32		1040
imp	'Chord'							Chord							gdi32		1041
imp	'ChrCmpIA'						ChrCmpIA						KernelBase	129
imp	'ChrCmpIW'						ChrCmpIW						KernelBase	130
imp	'ClearBitmapAttributes'					ClearBitmapAttributes					gdi32		1042
imp	'ClearBrushAttributes'					ClearBrushAttributes					gdi32		1043
imp	'ClearCommBreak'					ClearCommBreak						kernel32	0		# KernelBase
imp	'ClearCommError'					ClearCommError						kernel32	0		# KernelBase
imp	'ClearEventLogA'					ClearEventLogA						advapi32	1098
imp	'ClearEventLog'						ClearEventLogW						advapi32	1099
imp	'CliImmSetHotKey'					CliImmSetHotKey						user32		1583
imp	'ClientThreadSetup'					ClientThreadSetup					user32		1584
imp	'ClientToScreen'					ClientToScreen						user32		1585
imp	'ClipCursor'						ClipCursor						user32		1586
imp	'CloseClipboard'					CloseClipboard						user32		1587
imp	'CloseCodeAuthzLevel'					CloseCodeAuthzLevel					advapi32	1100
imp	'CloseConsoleHandle'					CloseConsoleHandle					kernel32	134
imp	'CloseDesktop'						CloseDesktop						user32		1588
imp	'CloseEncryptedFileRaw'					CloseEncryptedFileRaw					advapi32	1101
imp	'CloseEnhMetaFile'					CloseEnhMetaFile					gdi32		1044
imp	'CloseEventLog'						CloseEventLog						advapi32	1102
imp	'CloseFigure'						CloseFigure						gdi32		1045
imp	'CloseGestureInfoHandle'				CloseGestureInfoHandle					user32		1589
imp	'CloseGlobalizationUserSettingsKey'			CloseGlobalizationUserSettingsKey			KernelBase	133
imp	'CloseHandle'						CloseHandle						kernel32	0	1	# KernelBase
imp	'CloseMetaFile'						CloseMetaFile						gdi32		1046
imp	'ClosePackageInfo'					ClosePackageInfo					kernel32	0		# KernelBase
imp	'ClosePrivateNamespace'					ClosePrivateNamespace					kernel32	0		# KernelBase
imp	'CloseProfileUserMapping'				CloseProfileUserMapping					kernel32	138
imp	'CloseServiceHandle'					CloseServiceHandle					advapi32	1103
imp	'CloseState'						CloseState						KernelBase	137
imp	'CloseStateAtom'					CloseStateAtom						KernelBase	138
imp	'CloseStateChangeNotification'				CloseStateChangeNotification				KernelBase	139
imp	'CloseStateContainer'					CloseStateContainer					KernelBase	140
imp	'CloseStateLock'					CloseStateLock						KernelBase	141
imp	'CloseThreadWaitChainSession'				CloseThreadWaitChainSession				advapi32	1104
imp	'CloseTouchInputHandle'					CloseTouchInputHandle					user32		1590
imp	'CloseTrace'						CloseTrace						advapi32	1105
imp	'CloseWindow'						CloseWindow						user32		1591	1
imp	'CloseWindowStation'					CloseWindowStation					user32		1592
imp	'CmdBatNotification'					CmdBatNotification					kernel32	147
imp	'ColorCorrectPalette'					ColorCorrectPalette					gdi32		1047
imp	'ColorMatchToTarget'					ColorMatchToTarget					gdi32		1048
imp	'CombineRgn'						CombineRgn						gdi32		1049
imp	'CombineTransform'					CombineTransform					gdi32		1050
imp	'CommConfigDialogA'					CommConfigDialogA					kernel32	148
imp	'CommConfigDialog'					CommConfigDialogW					kernel32	149
imp	'CommDlgExtendedError'					CommDlgExtendedError					comdlg32	106
imp	'CommandLineFromMsiDescriptor'				CommandLineFromMsiDescriptor				advapi32	1106
imp	'CommandLineToArgv'					CommandLineToArgvW					shell32		269
imp	'CommitStateAtom'					CommitStateAtom						KernelBase	149
imp	'CompareCalendarDates'					CompareCalendarDates					kernel32	150
imp	'CompareFileTime'					CompareFileTime						kernel32	0		# KernelBase
imp	'CompareObjectHandles'					CompareObjectHandles					KernelBase	151
imp	'CompareStringA'					CompareStringA						kernel32	0		# KernelBase
imp	'CompareStringEx'					CompareStringEx						kernel32	0		# KernelBase
imp	'CompareStringOrdinal'					CompareStringOrdinal					kernel32	0		# KernelBase
imp	'CompareString'						CompareStringW						kernel32	0		# KernelBase
imp	'ComputeAccessTokenFromCodeAuthzLevel'			ComputeAccessTokenFromCodeAuthzLevel			advapi32	1107
imp	'ConfigureOPMProtectedOutput'				ConfigureOPMProtectedOutput				gdi32		1051
imp	'ConnectNamedPipe'					ConnectNamedPipe					kernel32	0	2	# KernelBase
imp	'ConsoleControl'					ConsoleControl						user32		1593
imp	'ConsoleMenuControl'					ConsoleMenuControl					kernel32	157
imp	'ContinueDebugEvent'					ContinueDebugEvent					kernel32	0	3	# KernelBase
imp	'ControlMagnification'					ControlMagnification					user32		1594
imp	'ControlService'					ControlService						advapi32	1108
imp	'ControlServiceExA'					ControlServiceExA					advapi32	1109
imp	'ControlServiceEx'					ControlServiceExW					advapi32	1110
imp	'ControlTraceA'						ControlTraceA						advapi32	1111
imp	'ControlTrace'						ControlTraceW						advapi32	1112
imp	'Control_RunDLL'					Control_RunDLL						shell32		272
imp	'Control_RunDLLA'					Control_RunDLLA						shell32		273
imp	'Control_RunDLLAsUser'					Control_RunDLLAsUserW					shell32		274
imp	'Control_RunDLLW'					Control_RunDLLW						shell32		275
imp	'ConvertAccessToSecurityDescriptorA'			ConvertAccessToSecurityDescriptorA			advapi32	1113
imp	'ConvertAccessToSecurityDescriptor'			ConvertAccessToSecurityDescriptorW			advapi32	1114
imp	'ConvertAuxiliaryCounterToPerformanceCounter'		ConvertAuxiliaryCounterToPerformanceCounter		KernelBase	158
imp	'ConvertCalDateTimeToSystemTime'			ConvertCalDateTimeToSystemTime				kernel32	159
imp	'ConvertDefaultLocale'					ConvertDefaultLocale					kernel32	0		# KernelBase
imp	'ConvertFiberToThread'					ConvertFiberToThread					kernel32	0		# KernelBase
imp	'ConvertNLSDayOfWeekToWin32DayOfWeek'			ConvertNLSDayOfWeekToWin32DayOfWeek			kernel32	162
imp	'ConvertPerformanceCounterToAuxiliaryCounter'		ConvertPerformanceCounterToAuxiliaryCounter		KernelBase	161
imp	'ConvertSDToStringSDDomain'				ConvertSDToStringSDDomainW				advapi32	1115
imp	'ConvertSDToStringSDRootDomainA'			ConvertSDToStringSDRootDomainA				advapi32	1116
imp	'ConvertSDToStringSDRootDomain'				ConvertSDToStringSDRootDomainW				advapi32	1117
imp	'ConvertSecurityDescriptorToAccessA'			ConvertSecurityDescriptorToAccessA			advapi32	1118
imp	'ConvertSecurityDescriptorToAccessNamedA'		ConvertSecurityDescriptorToAccessNamedA			advapi32	1119
imp	'ConvertSecurityDescriptorToAccessNamed'		ConvertSecurityDescriptorToAccessNamedW			advapi32	1120
imp	'ConvertSecurityDescriptorToAccess'			ConvertSecurityDescriptorToAccessW			advapi32	1121
imp	'ConvertSecurityDescriptorToStringSecurityDescriptorA'	ConvertSecurityDescriptorToStringSecurityDescriptorA	advapi32	1122
imp	'ConvertSecurityDescriptorToStringSecurityDescriptor'	ConvertSecurityDescriptorToStringSecurityDescriptorW	advapi32	1123
imp	'ConvertSidToStringSidA'				ConvertSidToStringSidA					advapi32	1124
imp	'ConvertSidToStringSid'					ConvertSidToStringSidW					advapi32	1125
imp	'ConvertStringSDToSDDomainA'				ConvertStringSDToSDDomainA				advapi32	1126
imp	'ConvertStringSDToSDDomain'				ConvertStringSDToSDDomainW				advapi32	1127
imp	'ConvertStringSDToSDRootDomainA'			ConvertStringSDToSDRootDomainA				advapi32	1128
imp	'ConvertStringSDToSDRootDomain'				ConvertStringSDToSDRootDomainW				advapi32	1129
imp	'ConvertStringSecurityDescriptorToSecurityDescriptorA'	ConvertStringSecurityDescriptorToSecurityDescriptorA	advapi32	1130
imp	'ConvertStringSecurityDescriptorToSecurityDescriptor'	ConvertStringSecurityDescriptorToSecurityDescriptorW	advapi32	1131
imp	'ConvertStringSidToSidA'				ConvertStringSidToSidA					advapi32	1132
imp	'ConvertStringSidToSid'					ConvertStringSidToSidW					advapi32	1133
imp	'ConvertSystemTimeToCalDateTime'			ConvertSystemTimeToCalDateTime				kernel32	163
imp	'ConvertThreadToFiber'					ConvertThreadToFiber					kernel32	0		# KernelBase
imp	'ConvertThreadToFiberEx'				ConvertThreadToFiberEx					kernel32	0		# KernelBase
imp	'ConvertToAutoInheritPrivateObjectSecurity'		ConvertToAutoInheritPrivateObjectSecurity		advapi32	0		# KernelBase
imp	'CopyAcceleratorTable'					CopyAcceleratorTableW					user32		1596
imp	'CopyAcceleratorTableA'					CopyAcceleratorTableA					user32		1595
imp	'CopyContext'						CopyContext						kernel32	0		# KernelBase
imp	'CopyEnhMetaFile'					CopyEnhMetaFileW					gdi32		1053
imp	'CopyEnhMetaFileA'					CopyEnhMetaFileA					gdi32		1052
imp	'CopyFile2'						CopyFile2						kernel32	0		# KernelBase
imp	'CopyFile'						CopyFileW						kernel32	0	3	# KernelBase
imp	'CopyFileA'						CopyFileA						kernel32	168	3
imp	'CopyFileEx'						CopyFileExW						kernel32	0		# KernelBase
imp	'CopyFileExA'						CopyFileExA						kernel32	169
imp	'CopyFileTransacted'					CopyFileTransactedW					kernel32	172
imp	'CopyFileTransactedA'					CopyFileTransactedA					kernel32	171
imp	'CopyIcon'						CopyIcon						user32		1597
imp	'CopyImage'						CopyImage						user32		1598
imp	'CopyLZFile'						CopyLZFile						kernel32	174
imp	'CopyMetaFileA'						CopyMetaFileA						gdi32		1054
imp	'CopyMetaFile'						CopyMetaFileW						gdi32		1055
imp	'CopyRect'						CopyRect						user32		1599
imp	'CopySid'						CopySid							advapi32	0		# KernelBase
imp	'CouldMultiUserAppsBehaviorBePossibleForPackage'	CouldMultiUserAppsBehaviorBePossibleForPackage		KernelBase	171
imp	'CountClipboardFormats'					CountClipboardFormats					user32		1600
imp	'CreateAcceleratorTableA'				CreateAcceleratorTableA					user32		1601
imp	'CreateAcceleratorTable'				CreateAcceleratorTableW					user32		1602
imp	'CreateActCtxA'						CreateActCtxA						kernel32	175
imp	'CreateActCtx'						CreateActCtxW						kernel32	0		# KernelBase
imp	'CreateActCtxWWorker'					CreateActCtxWWorker					kernel32	177
imp	'CreateAppContainerToken'				CreateAppContainerToken					KernelBase	173
imp	'CreateAppContainerTokenForUser'			CreateAppContainerTokenForUser				KernelBase	174
imp	'CreateBitmap'						CreateBitmap						gdi32		1056
imp	'CreateBitmapFromDxSurface'				CreateBitmapFromDxSurface				gdi32		1057
imp	'CreateBitmapFromDxSurface2'				CreateBitmapFromDxSurface2				gdi32		1058
imp	'CreateBitmapIndirect'					CreateBitmapIndirect					gdi32		1059
imp	'CreateBoundaryDescriptorA'				CreateBoundaryDescriptorA				kernel32	178
imp	'CreateBoundaryDescriptor'				CreateBoundaryDescriptorW				kernel32	0		# KernelBase
imp	'CreateBrushIndirect'					CreateBrushIndirect					gdi32		1060
imp	'CreateCaret'						CreateCaret						user32		1603
imp	'CreateCodeAuthzLevel'					CreateCodeAuthzLevel					advapi32	1136
imp	'CreateColorSpaceA'					CreateColorSpaceA					gdi32		1061
imp	'CreateColorSpace'					CreateColorSpaceW					gdi32		1062
imp	'CreateCompatibleBitmap'				CreateCompatibleBitmap					gdi32		1063	3
imp	'CreateCompatibleDC'					CreateCompatibleDC					gdi32		1064	1
imp	'CreateConsoleScreenBuffer'				CreateConsoleScreenBuffer				kernel32	0		# KernelBase
imp	'CreateCursor'						CreateCursor						user32		1604
imp	'CreateDCA'						CreateDCA						gdi32		1065
imp	'CreateDCEx'						CreateDCExW						gdi32		2000
imp	'CreateDCW'						CreateDCW						gdi32		1066
imp	'CreateDCompositionHwndTarget'				CreateDCompositionHwndTarget				user32		1605
imp	'CreateDIBPatternBrush'					CreateDIBPatternBrush					gdi32		1067
imp	'CreateDIBPatternBrushPt'				CreateDIBPatternBrushPt					gdi32		1068
imp	'CreateDIBSection'					CreateDIBSection					gdi32		1069
imp	'CreateDIBitmap'					CreateDIBitmap						gdi32		1070
imp	'CreateDPIScaledDIBSection'				CreateDPIScaledDIBSection				gdi32		1071
imp	'CreateDesktopA'					CreateDesktopA						user32		1606
imp	'CreateDesktopEx'					CreateDesktopExW					user32		1608
imp	'CreateDesktopExA'					CreateDesktopExA					user32		1607
imp	'CreateDesktop'						CreateDesktopW						user32		1609
imp	'CreateDialogIndirectParamA'				CreateDialogIndirectParamA				user32		1610
imp	'CreateDialogIndirectParamAor'				CreateDialogIndirectParamAorW				user32		1611
imp	'CreateDialogIndirectParam'				CreateDialogIndirectParamW				user32		1612
imp	'CreateDialogParamA'					CreateDialogParamA					user32		1613
imp	'CreateDialogParam'					CreateDialogParamW					user32		1614
imp	'CreateDirectory'					CreateDirectoryW					kernel32	0	2	# KernelBase
imp	'CreateDirectoryA'					CreateDirectoryA					kernel32	0	2	# KernelBase
imp	'CreateDirectoryExA'					CreateDirectoryExA					kernel32	182
imp	'CreateDirectoryEx'					CreateDirectoryExW					kernel32	0		# KernelBase
imp	'CreateDirectoryTransactedA'				CreateDirectoryTransactedA				kernel32	184
imp	'CreateDirectoryTransacted'				CreateDirectoryTransactedW				kernel32	185
imp	'CreateDiscardableBitmap'				CreateDiscardableBitmap					gdi32		1072
imp	'CreateEllipticRgn'					CreateEllipticRgn					gdi32		1073
imp	'CreateEllipticRgnIndirect'				CreateEllipticRgnIndirect				gdi32		1074
imp	'CreateEnclave'						CreateEnclave						KernelBase	180
imp	'CreateEnhMetaFileA'					CreateEnhMetaFileA					gdi32		1075
imp	'CreateEnhMetaFile'					CreateEnhMetaFileW					gdi32		1076
imp	'CreateEventA'						CreateEventA						kernel32	0		# KernelBase
imp	'CreateEventExA'					CreateEventExA						kernel32	0		# KernelBase
imp	'CreateEventEx'						CreateEventExW						kernel32	0		# KernelBase
imp	'CreateEvent'						CreateEventW						kernel32	0		# KernelBase
imp	'CreateFiber'						CreateFiber						kernel32	0		# KernelBase
imp	'CreateFiberEx'						CreateFiberEx						kernel32	0		# KernelBase
imp	'CreateFile'						CreateFileW						kernel32	0	7	# KernelBase
imp	'CreateFileA'						CreateFileA						kernel32	0	7	# KernelBase
imp	'CreateFileMappingNuma'					CreateFileMappingNumaW					kernel32	0	7	# Kernelbase
imp	'CreateFileMappingNumaA'				CreateFileMappingNumaA					kernel32	198	7
imp	'CreateFileMapping'					CreateFileMappingW					kernel32	0	7	# KernelBase
imp	'CreateFileMappingA'					CreateFileMappingA					kernel32	196	7
imp	'CreateFileTransacted'					CreateFileTransactedW					kernel32	202
imp	'CreateFileTransactedA'					CreateFileTransactedA					kernel32	201
imp	'CreateFile2'						CreateFile2						kernel32	0		# KernelBase
imp	'CreateFileMappingFromApp'				CreateFileMappingFromApp				kernel32	0		# KernelBase
imp	'CreateFontA'						CreateFontA						gdi32		1077
imp	'CreateFontIndirectA'					CreateFontIndirectA					gdi32		1078
imp	'CreateFontIndirectExA'					CreateFontIndirectExA					gdi32		1079
imp	'CreateFontIndirectEx'					CreateFontIndirectExW					gdi32		1080
imp	'CreateFontIndirect'					CreateFontIndirectW					gdi32		1081
imp	'CreateFont'						CreateFontW						gdi32		1082
imp	'CreateHalftonePalette'					CreateHalftonePalette					gdi32		1083
imp	'CreateHardLink'					CreateHardLinkW						kernel32	0	3	# KernelBase
imp	'CreateHardLinkA'					CreateHardLinkA						kernel32	0	3	# KernelBase
imp	'CreateHardLinkTransactedA'				CreateHardLinkTransactedA				kernel32	205
imp	'CreateHardLinkTransacted'				CreateHardLinkTransactedW				kernel32	206
imp	'CreateHatchBrush'					CreateHatchBrush					gdi32		1084
imp	'CreateICA'						CreateICA						gdi32		1085
imp	'CreateICW'						CreateICW						gdi32		1086
imp	'CreateIcon'						CreateIcon						user32		1615
imp	'CreateIconFromResource'				CreateIconFromResource					user32		1616
imp	'CreateIconFromResourceEx'				CreateIconFromResourceEx				user32		1617
imp	'CreateIconIndirect'					CreateIconIndirect					user32		1618
imp	'CreateIoCompletionPort'				CreateIoCompletionPort					kernel32	0	4	# KernelBase
imp	'CreateJobObjectA'					CreateJobObjectA					kernel32	209
imp	'CreateJobObject'					CreateJobObjectW					kernel32	210
imp	'CreateJobSet'						CreateJobSet						kernel32	211
imp	'CreateMDIWindowA'					CreateMDIWindowA					user32		1619
imp	'CreateMDIWindow'					CreateMDIWindowW					user32		1620
imp	'CreateMailslotA'					CreateMailslotA						kernel32	212
imp	'CreateMailslot'					CreateMailslotW						kernel32	213
imp	'CreateMemoryResourceNotification'			CreateMemoryResourceNotification			kernel32	0		# KernelBase
imp	'CreateMenu'						CreateMenu						user32		1621	0
imp	'CreateMetaFileA'					CreateMetaFileA						gdi32		1087
imp	'CreateMetaFile'					CreateMetaFileW						gdi32		1088
imp	'CreateMutex'						CreateMutexW						kernel32	0		# KernelBase
imp	'CreateMutexA'						CreateMutexA						kernel32	0		# KernelBase
imp	'CreateMutexEx'						CreateMutexExW						kernel32	0		# KernelBase
imp	'CreateMutexExA'					CreateMutexExA						kernel32	0		# KernelBase
imp	'CreateNamedPipe'					CreateNamedPipeW					kernel32	0	8	# KernelBase
imp	'CreateNamedPipeA'					CreateNamedPipeA					kernel32	219	8
imp	'CreateOPMProtectedOutput'				CreateOPMProtectedOutput				gdi32		1089
imp	'CreateOPMProtectedOutputs'				CreateOPMProtectedOutputs				gdi32		1090
imp	'CreatePalette'						CreatePalette						gdi32		1091
imp	'CreatePalmRejectionDelayZone'				CreatePalmRejectionDelayZone				user32		1503
imp	'CreatePatternBrush'					CreatePatternBrush					gdi32		1092
imp	'CreatePen'						CreatePen						gdi32		1093
imp	'CreatePenIndirect'					CreatePenIndirect					gdi32		1094
imp	'CreatePipe'						CreatePipe						kernel32	0	4	# KernelBase
imp	'CreatePolyPolygonRgn'					CreatePolyPolygonRgn					gdi32		1095
imp	'CreatePolygonRgn'					CreatePolygonRgn					gdi32		1096
imp	'CreatePopupMenu'					CreatePopupMenu						user32		1622	0
imp	'CreatePrivateNamespaceA'				CreatePrivateNamespaceA					kernel32	222
imp	'CreatePrivateNamespace'				CreatePrivateNamespaceW					kernel32	0		# KernelBase
imp	'CreatePrivateObjectSecurity'				CreatePrivateObjectSecurity				advapi32	0		# KernelBase
imp	'CreatePrivateObjectSecurityEx'				CreatePrivateObjectSecurityEx				advapi32	0		# KernelBase
imp	'CreatePrivateObjectSecurityWithMultipleInheritance'	CreatePrivateObjectSecurityWithMultipleInheritance	advapi32	0		# KernelBase
imp	'CreateProcessA'					CreateProcessA						kernel32	0	10	# KernelBase
imp	'CreateProcess'						CreateProcessW						kernel32	0	10	# KernelBase
imp	'CreateProcessAsUserA'					CreateProcessAsUserA					advapi32	0	11	# KernelBase
imp	'CreateProcessAsUser'					CreateProcessAsUserW					advapi32	0	11	# KernelBase
imp	'CreateProcessInternal'					CreateProcessInternalW					KernelBase	211
imp	'CreateProcessInternalA'				CreateProcessInternalA					KernelBase	210
imp	'CreateProcessWithLogon'				CreateProcessWithLogonW					advapi32	1142
imp	'CreateProcessWithToken'				CreateProcessWithTokenW					advapi32	1143
imp	'CreateRectRgn'						CreateRectRgn						gdi32		1097
imp	'CreateRectRgnIndirect'					CreateRectRgnIndirect					gdi32		1098
imp	'CreateRemoteThread'					CreateRemoteThread					kernel32	0		# KernelBase
imp	'CreateRemoteThreadEx'					CreateRemoteThreadEx					kernel32	0		# KernelBase
imp	'CreateRestrictedToken'					CreateRestrictedToken					advapi32	0		# KernelBase
imp	'CreateRoundRectRgn'					CreateRoundRectRgn					gdi32		1099
imp	'CreateScalableFontResourceA'				CreateScalableFontResourceA				gdi32		1100
imp	'CreateScalableFontResource'				CreateScalableFontResourceW				gdi32		1101
imp	'CreateSemaphoreA'					CreateSemaphoreA					kernel32	232
imp	'CreateSemaphoreExA'					CreateSemaphoreExA					kernel32	233
imp	'CreateSemaphoreEx'					CreateSemaphoreExW					kernel32	0		# KernelBase
imp	'CreateSemaphore'					CreateSemaphoreW					kernel32	0		# KernelBase
imp	'CreateServiceA'					CreateServiceA						advapi32	1145
imp	'CreateServiceEx'					CreateServiceEx						advapi32	1146
imp	'CreateService'						CreateServiceW						advapi32	1147
imp	'CreateSessionMappedDIBSection'				CreateSessionMappedDIBSection				gdi32		1102
imp	'CreateSolidBrush'					CreateSolidBrush					gdi32		1103
imp	'CreateStateAtom'					CreateStateAtom						KernelBase	218
imp	'CreateStateChangeNotification'				CreateStateChangeNotification				KernelBase	219
imp	'CreateStateContainer'					CreateStateContainer					KernelBase	220
imp	'CreateStateLock'					CreateStateLock						KernelBase	221
imp	'CreateStateSubcontainer'				CreateStateSubcontainer					KernelBase	222
imp	'CreateStorageItemFromPath_FullTrustCaller'		CreateStorageItemFromPath_FullTrustCaller		shell32		935
imp	'CreateStorageItemFromPath_FullTrustCaller_ForPackage'	CreateStorageItemFromPath_FullTrustCaller_ForPackage	shell32		936
imp	'CreateStorageItemFromPath_PartialTrustCaller'		CreateStorageItemFromPath_PartialTrustCaller		shell32		920
imp	'CreateStorageItemFromShellItem_FullTrustCaller'	CreateStorageItemFromShellItem_FullTrustCaller		shell32		921
imp	'CreateStorageItemFromShellItem_FullTrustCaller_ForPackage'	CreateStorageItemFromShellItem_FullTrustCaller_ForPackage	shell32		925
imp	'CreateStorageItemFromShellItem_FullTrustCaller_ForPackage_WithProcessHandle'	CreateStorageItemFromShellItem_FullTrustCaller_ForPackage_WithProcessHandle	shell32		929
imp	'CreateStorageItemFromShellItem_FullTrustCaller_UseImplicitFlagsAndPackage'	CreateStorageItemFromShellItem_FullTrustCaller_UseImplicitFlagsAndPackage	shell32		931
imp	'CreateSymbolicLink'					CreateSymbolicLinkW					kernel32	0	3	# KernelBase
imp	'CreateSymbolicLinkA'					CreateSymbolicLinkA					kernel32	0	3	# KernelBase
imp	'CreateSymbolicLinkTransactedA'				CreateSymbolicLinkTransactedA				kernel32	237
imp	'CreateSymbolicLinkTransacted'				CreateSymbolicLinkTransactedW				kernel32	238
imp	'CreateSystemThreads'					CreateSystemThreads					user32		1623
imp	'CreateTapePartition'					CreateTapePartition					kernel32	240
imp	'CreateThread'						CreateThread						kernel32	0	6	# KernelBase
imp	'CreateThreadpool'					CreateThreadpool					kernel32	0		# KernelBase
imp	'CreateThreadpoolCleanupGroup'				CreateThreadpoolCleanupGroup				kernel32	0		# KernelBase
imp	'CreateThreadpoolIo'					CreateThreadpoolIo					kernel32	0		# KernelBase
imp	'CreateThreadpoolTimer'					CreateThreadpoolTimer					kernel32	0		# KernelBase
imp	'CreateThreadpoolWait'					CreateThreadpoolWait					kernel32	0		# KernelBase
imp	'CreateThreadpoolWork'					CreateThreadpoolWork					kernel32	0		# KernelBase
imp	'CreateTimerQueue'					CreateTimerQueue					kernel32	0		# KernelBase
imp	'CreateTimerQueueTimer'					CreateTimerQueueTimer					kernel32	0		# KernelBase
imp	'CreateToolhelp32Snapshot'				CreateToolhelp32Snapshot				kernel32	250
imp	'CreateUmsCompletionList'				CreateUmsCompletionList					kernel32	251
imp	'CreateUmsThreadContext'				CreateUmsThreadContext					kernel32	252
imp	'CreateWaitableTimerA'					CreateWaitableTimerA					kernel32	253	3
imp	'CreateWaitableTimerExA'				CreateWaitableTimerExA					kernel32	254	4
imp	'CreateWaitableTimerEx'					CreateWaitableTimerExW					kernel32	0	4	# KernelBase
imp	'CreateWaitableTimer'					CreateWaitableTimerW					kernel32	0	3	# KernelBase
imp	'CreateWellKnownSid'					CreateWellKnownSid					advapi32	0		# KernelBase
imp	'CreateWindowEx'					CreateWindowExW						user32		1625	12
imp	'CreateWindowExA'					CreateWindowExA						user32		1624	12
imp	'CreateWindowInBand'					CreateWindowInBand					user32		1626
imp	'CreateWindowInBandEx'					CreateWindowInBandEx					user32		1627
imp	'CreateWindowIndirect'					CreateWindowIndirect					user32		1628
imp	'CreateWindowStationA'					CreateWindowStationA					user32		1629
imp	'CreateWindowStation'					CreateWindowStationW					user32		1630
imp	'CredBackupCredentials'					CredBackupCredentials					advapi32	1150
imp	'CredDeleteA'						CredDeleteA						advapi32	1151
imp	'CredDelete'						CredDeleteW						advapi32	1152
imp	'CredEncryptAndMarshalBinaryBlob'			CredEncryptAndMarshalBinaryBlob				advapi32	1153
imp	'CredEnumerateA'					CredEnumerateA						advapi32	1154
imp	'CredEnumerate'						CredEnumerateW						advapi32	1155
imp	'CredFindBestCredentialA'				CredFindBestCredentialA					advapi32	1156
imp	'CredFindBestCredential'				CredFindBestCredentialW					advapi32	1157
imp	'CredFree'						CredFree						advapi32	1158
imp	'CredGetSessionTypes'					CredGetSessionTypes					advapi32	1159
imp	'CredGetTargetInfoA'					CredGetTargetInfoA					advapi32	1160
imp	'CredGetTargetInfo'					CredGetTargetInfoW					advapi32	1161
imp	'CredIsMarshaledCredentialA'				CredIsMarshaledCredentialA				advapi32	1162
imp	'CredIsMarshaledCredential'				CredIsMarshaledCredentialW				advapi32	1163
imp	'CredIsProtectedA'					CredIsProtectedA					advapi32	1164
imp	'CredIsProtected'					CredIsProtectedW					advapi32	1165
imp	'CredMarshalCredentialA'				CredMarshalCredentialA					advapi32	1166
imp	'CredMarshalCredential'					CredMarshalCredentialW					advapi32	1167
imp	'CredProfileLoaded'					CredProfileLoaded					advapi32	1168
imp	'CredProfileLoadedEx'					CredProfileLoadedEx					advapi32	1169
imp	'CredProfileUnloaded'					CredProfileUnloaded					advapi32	1170
imp	'CredProtectA'						CredProtectA						advapi32	1171
imp	'CredProtect'						CredProtectW						advapi32	1172
imp	'CredReadA'						CredReadA						advapi32	1173
imp	'CredReadByTokenHandle'					CredReadByTokenHandle					advapi32	1174
imp	'CredReadDomainCredentialsA'				CredReadDomainCredentialsA				advapi32	1175
imp	'CredReadDomainCredentials'				CredReadDomainCredentialsW				advapi32	1176
imp	'CredRead'						CredReadW						advapi32	1177
imp	'CredRenameA'						CredRenameA						advapi32	1178
imp	'CredRename'						CredRenameW						advapi32	1179
imp	'CredRestoreCredentials'				CredRestoreCredentials					advapi32	1180
imp	'CredUnmarshalCredentialA'				CredUnmarshalCredentialA				advapi32	1181
imp	'CredUnmarshalCredential'				CredUnmarshalCredentialW				advapi32	1182
imp	'CredUnprotectA'					CredUnprotectA						advapi32	1183
imp	'CredUnprotect'						CredUnprotectW						advapi32	1184
imp	'CredWriteA'						CredWriteA						advapi32	1185
imp	'CredWriteDomainCredentialsA'				CredWriteDomainCredentialsA				advapi32	1186
imp	'CredWriteDomainCredentials'				CredWriteDomainCredentialsW				advapi32	1187
imp	'CredWrite'						CredWriteW						advapi32	1188
imp	'CredpConvertCredential'				CredpConvertCredential					advapi32	1189
imp	'CredpConvertOneCredentialSize'				CredpConvertOneCredentialSize				advapi32	1190
imp	'CredpConvertTargetInfo'				CredpConvertTargetInfo					advapi32	1191
imp	'CredpDecodeCredential'					CredpDecodeCredential					advapi32	1192
imp	'CredpEncodeCredential'					CredpEncodeCredential					advapi32	1193
imp	'CredpEncodeSecret'					CredpEncodeSecret					advapi32	1194
imp	'CryptAcquireContextA'					CryptAcquireContextA					advapi32	1195
imp	'CryptAcquireContext'					CryptAcquireContextW					advapi32	1196
imp	'CryptContextAddRef'					CryptContextAddRef					advapi32	1197
imp	'CryptCreateHash'					CryptCreateHash						advapi32	1198
imp	'CryptDecrypt'						CryptDecrypt						advapi32	1199
imp	'CryptDeriveKey'					CryptDeriveKey						advapi32	1200
imp	'CryptDestroyHash'					CryptDestroyHash					advapi32	1201
imp	'CryptDestroyKey'					CryptDestroyKey						advapi32	1202
imp	'CryptDuplicateHash'					CryptDuplicateHash					advapi32	1203
imp	'CryptDuplicateKey'					CryptDuplicateKey					advapi32	1204
imp	'CryptEncrypt'						CryptEncrypt						advapi32	1205
imp	'CryptEnumProviderTypesA'				CryptEnumProviderTypesA					advapi32	1206
imp	'CryptEnumProviderTypes'				CryptEnumProviderTypesW					advapi32	1207
imp	'CryptEnumProvidersA'					CryptEnumProvidersA					advapi32	1208
imp	'CryptEnumProviders'					CryptEnumProvidersW					advapi32	1209
imp	'CryptExportKey'					CryptExportKey						advapi32	1210
imp	'CryptGenKey'						CryptGenKey						advapi32	1211
imp	'CryptGenRandom'					CryptGenRandom						advapi32	1212
imp	'CryptGetDefaultProviderA'				CryptGetDefaultProviderA				advapi32	1213
imp	'CryptGetDefaultProvider'				CryptGetDefaultProviderW				advapi32	1214
imp	'CryptGetHashParam'					CryptGetHashParam					advapi32	1215
imp	'CryptGetKeyParam'					CryptGetKeyParam					advapi32	1216
imp	'CryptGetProvParam'					CryptGetProvParam					advapi32	1217
imp	'CryptGetUserKey'					CryptGetUserKey						advapi32	1218
imp	'CryptHashData'						CryptHashData						advapi32	1219
imp	'CryptHashSessionKey'					CryptHashSessionKey					advapi32	1220
imp	'CryptImportKey'					CryptImportKey						advapi32	1221
imp	'CryptReleaseContext'					CryptReleaseContext					advapi32	1222
imp	'CryptSetHashParam'					CryptSetHashParam					advapi32	1223
imp	'CryptSetKeyParam'					CryptSetKeyParam					advapi32	1224
imp	'CryptSetProvParam'					CryptSetProvParam					advapi32	1225
imp	'CryptSetProviderA'					CryptSetProviderA					advapi32	1226
imp	'CryptSetProviderExA'					CryptSetProviderExA					advapi32	1227
imp	'CryptSetProviderEx'					CryptSetProviderExW					advapi32	1228
imp	'CryptSetProvider'					CryptSetProviderW					advapi32	1229
imp	'CryptSignHashA'					CryptSignHashA						advapi32	1230
imp	'CryptSignHash'						CryptSignHashW						advapi32	1231
imp	'CryptVerifySignatureA'					CryptVerifySignatureA					advapi32	1232
imp	'CryptVerifySignature'					CryptVerifySignatureW					advapi32	1233
imp	'CsrAllocateCaptureBuffer'				CsrAllocateCaptureBuffer				ntdll		28
imp	'CsrAllocateMessagePointer'				CsrAllocateMessagePointer				ntdll		29
imp	'CsrBroadcastSystemMessageEx'				CsrBroadcastSystemMessageExW				user32		1631
imp	'CsrCaptureMessageBuffer'				CsrCaptureMessageBuffer					ntdll		30
imp	'CsrCaptureMessageMultiUnicodeStringsInPlace'		CsrCaptureMessageMultiUnicodeStringsInPlace		ntdll		31
imp	'CsrCaptureMessageString'				CsrCaptureMessageString					ntdll		32
imp	'CsrCaptureTimeout'					CsrCaptureTimeout					ntdll		33
imp	'CsrClientCallServer'					CsrClientCallServer					ntdll		34	4
imp	'CsrClientConnectToServer'				CsrClientConnectToServer				ntdll		35
imp	'CsrFreeCaptureBuffer'					CsrFreeCaptureBuffer					ntdll		36
imp	'CsrGetProcessId'					CsrGetProcessId						ntdll		37
imp	'CsrIdentifyAlertableThread'				CsrIdentifyAlertableThread				ntdll		38
imp	'CsrSetPriorityClass'					CsrSetPriorityClass					ntdll		39
imp	'CsrVerifyRegion'					CsrVerifyRegion						ntdll		40
imp	'CtrlRoutine'						CtrlRoutine						KernelBase	236
imp	'CtxInitUser32'						CtxInitUser32						user32		1632
imp	'CveEventWrite'						CveEventWrite						KernelBase	237
imp	'D3DKMTAbandonSwapChain'				D3DKMTAbandonSwapChain					gdi32		1104
imp	'D3DKMTAcquireKeyedMutex'				D3DKMTAcquireKeyedMutex					gdi32		1105
imp	'D3DKMTAcquireKeyedMutex2'				D3DKMTAcquireKeyedMutex2				gdi32		1106
imp	'D3DKMTAcquireSwapChain'				D3DKMTAcquireSwapChain					gdi32		1107
imp	'D3DKMTAddSurfaceToSwapChain'				D3DKMTAddSurfaceToSwapChain				gdi32		1108
imp	'D3DKMTAdjustFullscreenGamma'				D3DKMTAdjustFullscreenGamma				gdi32		1109
imp	'D3DKMTCacheHybridQueryValue'				D3DKMTCacheHybridQueryValue				gdi32		1110
imp	'D3DKMTChangeVideoMemoryReservation'			D3DKMTChangeVideoMemoryReservation			gdi32		1111
imp	'D3DKMTCheckExclusiveOwnership'				D3DKMTCheckExclusiveOwnership				gdi32		1112
imp	'D3DKMTCheckMonitorPowerState'				D3DKMTCheckMonitorPowerState				gdi32		1113
imp	'D3DKMTCheckMultiPlaneOverlaySupport'			D3DKMTCheckMultiPlaneOverlaySupport			gdi32		1114
imp	'D3DKMTCheckMultiPlaneOverlaySupport2'			D3DKMTCheckMultiPlaneOverlaySupport2			gdi32		1115
imp	'D3DKMTCheckMultiPlaneOverlaySupport3'			D3DKMTCheckMultiPlaneOverlaySupport3			gdi32		1116
imp	'D3DKMTCheckOcclusion'					D3DKMTCheckOcclusion					gdi32		1117
imp	'D3DKMTCheckSharedResourceAccess'			D3DKMTCheckSharedResourceAccess				gdi32		1118
imp	'D3DKMTCheckVidPnExclusiveOwnership'			D3DKMTCheckVidPnExclusiveOwnership			gdi32		1119
imp	'D3DKMTCloseAdapter'					D3DKMTCloseAdapter					gdi32		1120
imp	'D3DKMTConfigureSharedResource'				D3DKMTConfigureSharedResource				gdi32		1121
imp	'D3DKMTCreateAllocation'				D3DKMTCreateAllocation					gdi32		1122
imp	'D3DKMTCreateAllocation2'				D3DKMTCreateAllocation2					gdi32		1123
imp	'D3DKMTCreateBundleObject'				D3DKMTCreateBundleObject				gdi32		1124
imp	'D3DKMTCreateContext'					D3DKMTCreateContext					gdi32		1125
imp	'D3DKMTCreateContextVirtual'				D3DKMTCreateContextVirtual				gdi32		1126
imp	'D3DKMTCreateDCFromMemory'				D3DKMTCreateDCFromMemory				gdi32		1127
imp	'D3DKMTCreateDevice'					D3DKMTCreateDevice					gdi32		1128
imp	'D3DKMTCreateHwContext'					D3DKMTCreateHwContext					gdi32		1129
imp	'D3DKMTCreateHwQueue'					D3DKMTCreateHwQueue					gdi32		1130
imp	'D3DKMTCreateKeyedMutex'				D3DKMTCreateKeyedMutex					gdi32		1131
imp	'D3DKMTCreateKeyedMutex2'				D3DKMTCreateKeyedMutex2					gdi32		1132
imp	'D3DKMTCreateOutputDupl'				D3DKMTCreateOutputDupl					gdi32		1133
imp	'D3DKMTCreateOverlay'					D3DKMTCreateOverlay					gdi32		1134
imp	'D3DKMTCreatePagingQueue'				D3DKMTCreatePagingQueue					gdi32		1135
imp	'D3DKMTCreateProtectedSession'				D3DKMTCreateProtectedSession				gdi32		1136
imp	'D3DKMTCreateSwapChain'					D3DKMTCreateSwapChain					gdi32		1137
imp	'D3DKMTCreateSynchronizationObject'			D3DKMTCreateSynchronizationObject			gdi32		1138
imp	'D3DKMTCreateSynchronizationObject2'			D3DKMTCreateSynchronizationObject2			gdi32		1139
imp	'D3DKMTDDisplayEnum'					D3DKMTDDisplayEnum					gdi32		1140
imp	'D3DKMTDestroyAllocation'				D3DKMTDestroyAllocation					gdi32		1141
imp	'D3DKMTDestroyAllocation2'				D3DKMTDestroyAllocation2				gdi32		1142
imp	'D3DKMTDestroyContext'					D3DKMTDestroyContext					gdi32		1143
imp	'D3DKMTDestroyDCFromMemory'				D3DKMTDestroyDCFromMemory				gdi32		1144
imp	'D3DKMTDestroyDevice'					D3DKMTDestroyDevice					gdi32		1145
imp	'D3DKMTDestroyHwContext'				D3DKMTDestroyHwContext					gdi32		1146
imp	'D3DKMTDestroyHwQueue'					D3DKMTDestroyHwQueue					gdi32		1147
imp	'D3DKMTDestroyKeyedMutex'				D3DKMTDestroyKeyedMutex					gdi32		1148
imp	'D3DKMTDestroyOutputDupl'				D3DKMTDestroyOutputDupl					gdi32		1149
imp	'D3DKMTDestroyOverlay'					D3DKMTDestroyOverlay					gdi32		1150
imp	'D3DKMTDestroyPagingQueue'				D3DKMTDestroyPagingQueue				gdi32		1151
imp	'D3DKMTDestroyProtectedSession'				D3DKMTDestroyProtectedSession				gdi32		1152
imp	'D3DKMTDestroySynchronizationObject'			D3DKMTDestroySynchronizationObject			gdi32		1153
imp	'D3DKMTDispMgrCreate'					D3DKMTDispMgrCreate					gdi32		1154
imp	'D3DKMTDispMgrSourceOperation'				D3DKMTDispMgrSourceOperation				gdi32		1155
imp	'D3DKMTDispMgrTargetOperation'				D3DKMTDispMgrTargetOperation				gdi32		1156
imp	'D3DKMTEnumAdapters'					D3DKMTEnumAdapters					gdi32		1157
imp	'D3DKMTEnumAdapters2'					D3DKMTEnumAdapters2					gdi32		1158
imp	'D3DKMTEscape'						D3DKMTEscape						gdi32		1159
imp	'D3DKMTEvict'						D3DKMTEvict						gdi32		1160
imp	'D3DKMTExtractBundleObject'				D3DKMTExtractBundleObject				gdi32		1161
imp	'D3DKMTFlipOverlay'					D3DKMTFlipOverlay					gdi32		1162
imp	'D3DKMTFlushHeapTransitions'				D3DKMTFlushHeapTransitions				gdi32		1163
imp	'D3DKMTFreeGpuVirtualAddress'				D3DKMTFreeGpuVirtualAddress				gdi32		1164
imp	'D3DKMTGetAllocationPriority'				D3DKMTGetAllocationPriority				gdi32		1165
imp	'D3DKMTGetCachedHybridQueryValue'			D3DKMTGetCachedHybridQueryValue				gdi32		1166
imp	'D3DKMTGetContextInProcessSchedulingPriority'		D3DKMTGetContextInProcessSchedulingPriority		gdi32		1167
imp	'D3DKMTGetContextSchedulingPriority'			D3DKMTGetContextSchedulingPriority			gdi32		1168
imp	'D3DKMTGetDWMVerticalBlankEvent'			D3DKMTGetDWMVerticalBlankEvent				gdi32		1169
imp	'D3DKMTGetDeviceState'					D3DKMTGetDeviceState					gdi32		1170
imp	'D3DKMTGetDisplayModeList'				D3DKMTGetDisplayModeList				gdi32		1171
imp	'D3DKMTGetMemoryBudgetTarget'				D3DKMTGetMemoryBudgetTarget				gdi32		1172
imp	'D3DKMTGetMultiPlaneOverlayCaps'			D3DKMTGetMultiPlaneOverlayCaps				gdi32		1173
imp	'D3DKMTGetMultisampleMethodList'			D3DKMTGetMultisampleMethodList				gdi32		1174
imp	'D3DKMTGetOverlayState'					D3DKMTGetOverlayState					gdi32		1175
imp	'D3DKMTGetPostCompositionCaps'				D3DKMTGetPostCompositionCaps				gdi32		1176
imp	'D3DKMTGetPresentHistory'				D3DKMTGetPresentHistory					gdi32		1177
imp	'D3DKMTGetPresentQueueEvent'				D3DKMTGetPresentQueueEvent				gdi32		1178
imp	'D3DKMTGetProcessDeviceRemovalSupport'			D3DKMTGetProcessDeviceRemovalSupport			gdi32		1179
imp	'D3DKMTGetProcessList'					D3DKMTGetProcessList					gdi32		1180
imp	'D3DKMTGetProcessSchedulingPriorityBand'		D3DKMTGetProcessSchedulingPriorityBand			gdi32		1181
imp	'D3DKMTGetProcessSchedulingPriorityClass'		D3DKMTGetProcessSchedulingPriorityClass			gdi32		1182
imp	'D3DKMTGetResourcePresentPrivateDriverData'		D3DKMTGetResourcePresentPrivateDriverData		gdi32		1183
imp	'D3DKMTGetRuntimeData'					D3DKMTGetRuntimeData					gdi32		1184
imp	'D3DKMTGetScanLine'					D3DKMTGetScanLine					gdi32		1185
imp	'D3DKMTGetSetSwapChainMetadata'				D3DKMTGetSetSwapChainMetadata				gdi32		1186
imp	'D3DKMTGetSharedPrimaryHandle'				D3DKMTGetSharedPrimaryHandle				gdi32		1187
imp	'D3DKMTGetSharedResourceAdapterLuid'			D3DKMTGetSharedResourceAdapterLuid			gdi32		1188
imp	'D3DKMTGetYieldPercentage'				D3DKMTGetYieldPercentage				gdi32		1189
imp	'D3DKMTInvalidateActiveVidPn'				D3DKMTInvalidateActiveVidPn				gdi32		1190
imp	'D3DKMTInvalidateCache'					D3DKMTInvalidateCache					gdi32		1191
imp	'D3DKMTLock'						D3DKMTLock						gdi32		1192
imp	'D3DKMTLock2'						D3DKMTLock2						gdi32		1193
imp	'D3DKMTMakeResident'					D3DKMTMakeResident					gdi32		1194
imp	'D3DKMTMapGpuVirtualAddress'				D3DKMTMapGpuVirtualAddress				gdi32		1195
imp	'D3DKMTMarkDeviceAsError'				D3DKMTMarkDeviceAsError					gdi32		1196
imp	'D3DKMTNetDispGetNextChunkInfo'				D3DKMTNetDispGetNextChunkInfo				gdi32		1197
imp	'D3DKMTNetDispQueryMiracastDisplayDeviceStatus'		D3DKMTNetDispQueryMiracastDisplayDeviceStatus		gdi32		1198
imp	'D3DKMTNetDispQueryMiracastDisplayDeviceSupport'	D3DKMTNetDispQueryMiracastDisplayDeviceSupport		gdi32		1199
imp	'D3DKMTNetDispStartMiracastDisplayDevice'		D3DKMTNetDispStartMiracastDisplayDevice			gdi32		1200
imp	'D3DKMTNetDispStartMiracastDisplayDevice2'		D3DKMTNetDispStartMiracastDisplayDevice2		gdi32		1201
imp	'D3DKMTNetDispStartMiracastDisplayDeviceEx'		D3DKMTNetDispStartMiracastDisplayDeviceEx		gdi32		1202
imp	'D3DKMTNetDispStopMiracastDisplayDevice'		D3DKMTNetDispStopMiracastDisplayDevice			gdi32		1203
imp	'D3DKMTNetDispStopSessions'				D3DKMTNetDispStopSessions				gdi32		1204
imp	'D3DKMTOfferAllocations'				D3DKMTOfferAllocations					gdi32		1205
imp	'D3DKMTOpenAdapterFromDeviceName'			D3DKMTOpenAdapterFromDeviceName				gdi32		1206
imp	'D3DKMTOpenAdapterFromGdiDisplayName'			D3DKMTOpenAdapterFromGdiDisplayName			gdi32		1207
imp	'D3DKMTOpenAdapterFromHdc'				D3DKMTOpenAdapterFromHdc				gdi32		1208
imp	'D3DKMTOpenAdapterFromLuid'				D3DKMTOpenAdapterFromLuid				gdi32		1209
imp	'D3DKMTOpenBundleObjectNtHandleFromName'		D3DKMTOpenBundleObjectNtHandleFromName			gdi32		1210
imp	'D3DKMTOpenKeyedMutex'					D3DKMTOpenKeyedMutex					gdi32		1211
imp	'D3DKMTOpenKeyedMutex2'					D3DKMTOpenKeyedMutex2					gdi32		1212
imp	'D3DKMTOpenKeyedMutexFromNtHandle'			D3DKMTOpenKeyedMutexFromNtHandle			gdi32		1213
imp	'D3DKMTOpenNtHandleFromName'				D3DKMTOpenNtHandleFromName				gdi32		1214
imp	'D3DKMTOpenProtectedSessionFromNtHandle'		D3DKMTOpenProtectedSessionFromNtHandle			gdi32		1215
imp	'D3DKMTOpenResource'					D3DKMTOpenResource					gdi32		1216
imp	'D3DKMTOpenResource2'					D3DKMTOpenResource2					gdi32		1217
imp	'D3DKMTOpenResourceFromNtHandle'			D3DKMTOpenResourceFromNtHandle				gdi32		1218
imp	'D3DKMTOpenSwapChain'					D3DKMTOpenSwapChain					gdi32		1219
imp	'D3DKMTOpenSyncObjectFromNtHandle'			D3DKMTOpenSyncObjectFromNtHandle			gdi32		1220
imp	'D3DKMTOpenSyncObjectFromNtHandle2'			D3DKMTOpenSyncObjectFromNtHandle2			gdi32		1221
imp	'D3DKMTOpenSyncObjectNtHandleFromName'			D3DKMTOpenSyncObjectNtHandleFromName			gdi32		1222
imp	'D3DKMTOpenSynchronizationObject'			D3DKMTOpenSynchronizationObject				gdi32		1223
imp	'D3DKMTOutputDuplGetFrameInfo'				D3DKMTOutputDuplGetFrameInfo				gdi32		1224
imp	'D3DKMTOutputDuplGetMetaData'				D3DKMTOutputDuplGetMetaData				gdi32		1225
imp	'D3DKMTOutputDuplGetPointerShapeData'			D3DKMTOutputDuplGetPointerShapeData			gdi32		1226
imp	'D3DKMTOutputDuplPresent'				D3DKMTOutputDuplPresent					gdi32		1227
imp	'D3DKMTOutputDuplReleaseFrame'				D3DKMTOutputDuplReleaseFrame				gdi32		1228
imp	'D3DKMTPinDirectFlipResources'				D3DKMTPinDirectFlipResources				gdi32		1229
imp	'D3DKMTPollDisplayChildren'				D3DKMTPollDisplayChildren				gdi32		1230
imp	'D3DKMTPresent'						D3DKMTPresent						gdi32		1231
imp	'D3DKMTPresentMultiPlaneOverlay'			D3DKMTPresentMultiPlaneOverlay				gdi32		1232
imp	'D3DKMTPresentMultiPlaneOverlay2'			D3DKMTPresentMultiPlaneOverlay2				gdi32		1233
imp	'D3DKMTPresentMultiPlaneOverlay3'			D3DKMTPresentMultiPlaneOverlay3				gdi32		1234
imp	'D3DKMTPresentRedirected'				D3DKMTPresentRedirected					gdi32		1235
imp	'D3DKMTQueryAdapterInfo'				D3DKMTQueryAdapterInfo					gdi32		1236
imp	'D3DKMTQueryAllocationResidency'			D3DKMTQueryAllocationResidency				gdi32		1237
imp	'D3DKMTQueryClockCalibration'				D3DKMTQueryClockCalibration				gdi32		1238
imp	'D3DKMTQueryFSEBlock'					D3DKMTQueryFSEBlock					gdi32		1239
imp	'D3DKMTQueryProcessOfferInfo'				D3DKMTQueryProcessOfferInfo				gdi32		1240
imp	'D3DKMTQueryProtectedSessionInfoFromNtHandle'		D3DKMTQueryProtectedSessionInfoFromNtHandle		gdi32		1241
imp	'D3DKMTQueryProtectedSessionStatus'			D3DKMTQueryProtectedSessionStatus			gdi32		1242
imp	'D3DKMTQueryRemoteVidPnSourceFromGdiDisplayName'	D3DKMTQueryRemoteVidPnSourceFromGdiDisplayName		gdi32		1243
imp	'D3DKMTQueryResourceInfo'				D3DKMTQueryResourceInfo					gdi32		1244
imp	'D3DKMTQueryResourceInfoFromNtHandle'			D3DKMTQueryResourceInfoFromNtHandle			gdi32		1245
imp	'D3DKMTQueryStatistics'					D3DKMTQueryStatistics					gdi32		1246
imp	'D3DKMTQueryVidPnExclusiveOwnership'			D3DKMTQueryVidPnExclusiveOwnership			gdi32		1247
imp	'D3DKMTQueryVideoMemoryInfo'				D3DKMTQueryVideoMemoryInfo				gdi32		1248
imp	'D3DKMTReclaimAllocations'				D3DKMTReclaimAllocations				gdi32		1249
imp	'D3DKMTReclaimAllocations2'				D3DKMTReclaimAllocations2				gdi32		1250
imp	'D3DKMTRegisterTrimNotification'			D3DKMTRegisterTrimNotification				gdi32		1251
imp	'D3DKMTRegisterVailProcess'				D3DKMTRegisterVailProcess				gdi32		1252
imp	'D3DKMTReleaseKeyedMutex'				D3DKMTReleaseKeyedMutex					gdi32		1253
imp	'D3DKMTReleaseKeyedMutex2'				D3DKMTReleaseKeyedMutex2				gdi32		1254
imp	'D3DKMTReleaseProcessVidPnSourceOwners'			D3DKMTReleaseProcessVidPnSourceOwners			gdi32		1255
imp	'D3DKMTReleaseSwapChain'				D3DKMTReleaseSwapChain					gdi32		1256
imp	'D3DKMTRemoveSurfaceFromSwapChain'			D3DKMTRemoveSurfaceFromSwapChain			gdi32		1257
imp	'D3DKMTRender'						D3DKMTRender						gdi32		1258
imp	'D3DKMTReserveGpuVirtualAddress'			D3DKMTReserveGpuVirtualAddress				gdi32		1259
imp	'D3DKMTSetAllocationPriority'				D3DKMTSetAllocationPriority				gdi32		1260
imp	'D3DKMTSetContextInProcessSchedulingPriority'		D3DKMTSetContextInProcessSchedulingPriority		gdi32		1261
imp	'D3DKMTSetContextSchedulingPriority'			D3DKMTSetContextSchedulingPriority			gdi32		1262
imp	'D3DKMTSetDisplayMode'					D3DKMTSetDisplayMode					gdi32		1263
imp	'D3DKMTSetDisplayPrivateDriverFormat'			D3DKMTSetDisplayPrivateDriverFormat			gdi32		1264
imp	'D3DKMTSetDodIndirectSwapchain'				D3DKMTSetDodIndirectSwapchain				gdi32		1265
imp	'D3DKMTSetFSEBlock'					D3DKMTSetFSEBlock					gdi32		1266
imp	'D3DKMTSetGammaRamp'					D3DKMTSetGammaRamp					gdi32		1267
imp	'D3DKMTSetHwProtectionTeardownRecovery'			D3DKMTSetHwProtectionTeardownRecovery			gdi32		1268
imp	'D3DKMTSetMemoryBudgetTarget'				D3DKMTSetMemoryBudgetTarget				gdi32		1269
imp	'D3DKMTSetMonitorColorSpaceTransform'			D3DKMTSetMonitorColorSpaceTransform			gdi32		1270
imp	'D3DKMTSetProcessDeviceRemovalSupport'			D3DKMTSetProcessDeviceRemovalSupport			gdi32		1271
imp	'D3DKMTSetProcessSchedulingPriorityBand'		D3DKMTSetProcessSchedulingPriorityBand			gdi32		1272
imp	'D3DKMTSetProcessSchedulingPriorityClass'		D3DKMTSetProcessSchedulingPriorityClass			gdi32		1273
imp	'D3DKMTSetQueuedLimit'					D3DKMTSetQueuedLimit					gdi32		1274
imp	'D3DKMTSetStablePowerState'				D3DKMTSetStablePowerState				gdi32		1275
imp	'D3DKMTSetStereoEnabled'				D3DKMTSetStereoEnabled					gdi32		1276
imp	'D3DKMTSetSyncRefreshCountWaitTarget'			D3DKMTSetSyncRefreshCountWaitTarget			gdi32		1277
imp	'D3DKMTSetVidPnSourceHwProtection'			D3DKMTSetVidPnSourceHwProtection			gdi32		1278
imp	'D3DKMTSetVidPnSourceOwner'				D3DKMTSetVidPnSourceOwner				gdi32		1279
imp	'D3DKMTSetVidPnSourceOwner1'				D3DKMTSetVidPnSourceOwner1				gdi32		1280
imp	'D3DKMTSetVidPnSourceOwner2'				D3DKMTSetVidPnSourceOwner2				gdi32		1281
imp	'D3DKMTSetYieldPercentage'				D3DKMTSetYieldPercentage				gdi32		1282
imp	'D3DKMTShareObjects'					D3DKMTShareObjects					gdi32		1283
imp	'D3DKMTSharedPrimaryLockNotification'			D3DKMTSharedPrimaryLockNotification			gdi32		1284
imp	'D3DKMTSharedPrimaryUnLockNotification'			D3DKMTSharedPrimaryUnLockNotification			gdi32		1285
imp	'D3DKMTSignalSynchronizationObject'			D3DKMTSignalSynchronizationObject			gdi32		1286
imp	'D3DKMTSignalSynchronizationObject2'			D3DKMTSignalSynchronizationObject2			gdi32		1287
imp	'D3DKMTSignalSynchronizationObjectFromCpu'		D3DKMTSignalSynchronizationObjectFromCpu		gdi32		1288
imp	'D3DKMTSignalSynchronizationObjectFromGpu'		D3DKMTSignalSynchronizationObjectFromGpu		gdi32		1289
imp	'D3DKMTSignalSynchronizationObjectFromGpu2'		D3DKMTSignalSynchronizationObjectFromGpu2		gdi32		1290
imp	'D3DKMTSubmitCommand'					D3DKMTSubmitCommand					gdi32		1291
imp	'D3DKMTSubmitCommandToHwQueue'				D3DKMTSubmitCommandToHwQueue				gdi32		1292
imp	'D3DKMTSubmitPresentBltToHwQueue'			D3DKMTSubmitPresentBltToHwQueue				gdi32		1293
imp	'D3DKMTSubmitSignalSyncObjectsToHwQueue'		D3DKMTSubmitSignalSyncObjectsToHwQueue			gdi32		1294
imp	'D3DKMTSubmitWaitForSyncObjectsToHwQueue'		D3DKMTSubmitWaitForSyncObjectsToHwQueue			gdi32		1295
imp	'D3DKMTTrimProcessCommitment'				D3DKMTTrimProcessCommitment				gdi32		1296
imp	'D3DKMTUnOrderedPresentSwapChain'			D3DKMTUnOrderedPresentSwapChain				gdi32		1297
imp	'D3DKMTUnlock'						D3DKMTUnlock						gdi32		1298
imp	'D3DKMTUnlock2'						D3DKMTUnlock2						gdi32		1299
imp	'D3DKMTUnpinDirectFlipResources'			D3DKMTUnpinDirectFlipResources				gdi32		1300
imp	'D3DKMTUnregisterTrimNotification'			D3DKMTUnregisterTrimNotification			gdi32		1301
imp	'D3DKMTUpdateAllocationProperty'			D3DKMTUpdateAllocationProperty				gdi32		1302
imp	'D3DKMTUpdateGpuVirtualAddress'				D3DKMTUpdateGpuVirtualAddress				gdi32		1303
imp	'D3DKMTUpdateOverlay'					D3DKMTUpdateOverlay					gdi32		1304
imp	'D3DKMTVailConnect'					D3DKMTVailConnect					gdi32		1305
imp	'D3DKMTVailDisconnect'					D3DKMTVailDisconnect					gdi32		1306
imp	'D3DKMTVailPromoteCompositionSurface'			D3DKMTVailPromoteCompositionSurface			gdi32		1307
imp	'D3DKMTWaitForIdle'					D3DKMTWaitForIdle					gdi32		1308
imp	'D3DKMTWaitForSynchronizationObject'			D3DKMTWaitForSynchronizationObject			gdi32		1309
imp	'D3DKMTWaitForSynchronizationObject2'			D3DKMTWaitForSynchronizationObject2			gdi32		1310
imp	'D3DKMTWaitForSynchronizationObjectFromCpu'		D3DKMTWaitForSynchronizationObjectFromCpu		gdi32		1311
imp	'D3DKMTWaitForSynchronizationObjectFromGpu'		D3DKMTWaitForSynchronizationObjectFromGpu		gdi32		1312
imp	'D3DKMTWaitForVerticalBlankEvent'			D3DKMTWaitForVerticalBlankEvent				gdi32		1313
imp	'D3DKMTWaitForVerticalBlankEvent2'			D3DKMTWaitForVerticalBlankEvent2			gdi32		1314
imp	'DAD_AutoScroll'					DAD_AutoScroll						shell32		129
imp	'DAD_DragEnterEx'					DAD_DragEnterEx						shell32		131
imp	'DAD_DragEnterEx2'					DAD_DragEnterEx2					shell32		22
imp	'DAD_DragLeave'						DAD_DragLeave						shell32		132
imp	'DAD_DragMove'						DAD_DragMove						shell32		134
imp	'DAD_SetDragImage'					DAD_SetDragImage					shell32		136
imp	'DAD_ShowDragImage'					DAD_ShowDragImage					shell32		137
imp	'DDCCIGetCapabilitiesString'				DDCCIGetCapabilitiesString				gdi32		1315
imp	'DDCCIGetCapabilitiesStringLength'			DDCCIGetCapabilitiesStringLength			gdi32		1316
imp	'DDCCIGetTimingReport'					DDCCIGetTimingReport					gdi32		1317
imp	'DDCCIGetVCPFeature'					DDCCIGetVCPFeature					gdi32		1318
imp	'DDCCISaveCurrentSettings'				DDCCISaveCurrentSettings				gdi32		1319
imp	'DDCCISetVCPFeature'					DDCCISetVCPFeature					gdi32		1320
imp	'DPtoLP'						DPtoLP							gdi32		1321
imp	'DWMBindCursorToOutputConfig'				DWMBindCursorToOutputConfig				user32		1633
imp	'DWMCommitInputSystemOutputConfig'			DWMCommitInputSystemOutputConfig			user32		1634
imp	'DWMSetCursorOrientation'				DWMSetCursorOrientation					user32		1635
imp	'DWMSetInputSystemOutputConfig'				DWMSetInputSystemOutputConfig				user32		1636
imp	'DbgBreakPoint'						DbgBreakPoint						ntdll		41
imp	'DbgPrint'						DbgPrint						ntdll		42
imp	'DbgPrintEx'						DbgPrintEx						ntdll		43
imp	'DbgPrintReturnControlC'				DbgPrintReturnControlC					ntdll		44
imp	'DbgPrompt'						DbgPrompt						ntdll		45
imp	'DbgQueryDebugFilterState'				DbgQueryDebugFilterState				ntdll		46
imp	'DbgSetDebugFilterState'				DbgSetDebugFilterState					ntdll		47
imp	'DbgUiConnectToDbg'					DbgUiConnectToDbg					ntdll		48
imp	'DbgUiContinue'						DbgUiContinue						ntdll		49
imp	'DbgUiConvertStateChangeStructure'			DbgUiConvertStateChangeStructure			ntdll		50
imp	'DbgUiConvertStateChangeStructureEx'			DbgUiConvertStateChangeStructureEx			ntdll		51
imp	'DbgUiDebugActiveProcess'				DbgUiDebugActiveProcess					ntdll		52
imp	'DbgUiGetThreadDebugObject'				DbgUiGetThreadDebugObject				ntdll		53
imp	'DbgUiIssueRemoteBreakin'				DbgUiIssueRemoteBreakin					ntdll		54
imp	'DbgUiRemoteBreakin'					DbgUiRemoteBreakin					ntdll		55
imp	'DbgUiSetThreadDebugObject'				DbgUiSetThreadDebugObject				ntdll		56
imp	'DbgUiStopDebugging'					DbgUiStopDebugging					ntdll		57
imp	'DbgUiWaitStateChange'					DbgUiWaitStateChange					ntdll		58
imp	'DbgUserBreakPoint'					DbgUserBreakPoint					ntdll		59
imp	'DdCreateFullscreenSprite'				DdCreateFullscreenSprite				gdi32		1322
imp	'DdDestroyFullscreenSprite'				DdDestroyFullscreenSprite				gdi32		1323
imp	'DdEntry0'						DdEntry0						gdi32		1324
imp	'DdEntry1'						DdEntry1						gdi32		1325
imp	'DdEntry10'						DdEntry10						gdi32		1326
imp	'DdEntry11'						DdEntry11						gdi32		1327
imp	'DdEntry12'						DdEntry12						gdi32		1328
imp	'DdEntry13'						DdEntry13						gdi32		1329
imp	'DdEntry14'						DdEntry14						gdi32		1330
imp	'DdEntry15'						DdEntry15						gdi32		1331
imp	'DdEntry16'						DdEntry16						gdi32		1332
imp	'DdEntry17'						DdEntry17						gdi32		1333
imp	'DdEntry18'						DdEntry18						gdi32		1334
imp	'DdEntry19'						DdEntry19						gdi32		1335
imp	'DdEntry2'						DdEntry2						gdi32		1336
imp	'DdEntry20'						DdEntry20						gdi32		1337
imp	'DdEntry21'						DdEntry21						gdi32		1338
imp	'DdEntry22'						DdEntry22						gdi32		1339
imp	'DdEntry23'						DdEntry23						gdi32		1340
imp	'DdEntry24'						DdEntry24						gdi32		1341
imp	'DdEntry25'						DdEntry25						gdi32		1342
imp	'DdEntry26'						DdEntry26						gdi32		1343
imp	'DdEntry27'						DdEntry27						gdi32		1344
imp	'DdEntry28'						DdEntry28						gdi32		1345
imp	'DdEntry29'						DdEntry29						gdi32		1346
imp	'DdEntry3'						DdEntry3						gdi32		1347
imp	'DdEntry30'						DdEntry30						gdi32		1348
imp	'DdEntry31'						DdEntry31						gdi32		1349
imp	'DdEntry32'						DdEntry32						gdi32		1350
imp	'DdEntry33'						DdEntry33						gdi32		1351
imp	'DdEntry34'						DdEntry34						gdi32		1352
imp	'DdEntry35'						DdEntry35						gdi32		1353
imp	'DdEntry36'						DdEntry36						gdi32		1354
imp	'DdEntry37'						DdEntry37						gdi32		1355
imp	'DdEntry38'						DdEntry38						gdi32		1356
imp	'DdEntry39'						DdEntry39						gdi32		1357
imp	'DdEntry4'						DdEntry4						gdi32		1358
imp	'DdEntry40'						DdEntry40						gdi32		1359
imp	'DdEntry41'						DdEntry41						gdi32		1360
imp	'DdEntry42'						DdEntry42						gdi32		1361
imp	'DdEntry43'						DdEntry43						gdi32		1362
imp	'DdEntry44'						DdEntry44						gdi32		1363
imp	'DdEntry45'						DdEntry45						gdi32		1364
imp	'DdEntry46'						DdEntry46						gdi32		1365
imp	'DdEntry47'						DdEntry47						gdi32		1366
imp	'DdEntry48'						DdEntry48						gdi32		1367
imp	'DdEntry49'						DdEntry49						gdi32		1368
imp	'DdEntry5'						DdEntry5						gdi32		1369
imp	'DdEntry50'						DdEntry50						gdi32		1370
imp	'DdEntry51'						DdEntry51						gdi32		1371
imp	'DdEntry52'						DdEntry52						gdi32		1372
imp	'DdEntry53'						DdEntry53						gdi32		1373
imp	'DdEntry54'						DdEntry54						gdi32		1374
imp	'DdEntry55'						DdEntry55						gdi32		1375
imp	'DdEntry56'						DdEntry56						gdi32		1376
imp	'DdEntry6'						DdEntry6						gdi32		1377
imp	'DdEntry7'						DdEntry7						gdi32		1378
imp	'DdEntry8'						DdEntry8						gdi32		1379
imp	'DdEntry9'						DdEntry9						gdi32		1380
imp	'DdNotifyFullscreenSpriteUpdate'			DdNotifyFullscreenSpriteUpdate				gdi32		1381
imp	'DdQueryVisRgnUniqueness'				DdQueryVisRgnUniqueness					gdi32		1382
imp	'DdeAbandonTransaction'					DdeAbandonTransaction					user32		1637
imp	'DdeAccessData'						DdeAccessData						user32		1638
imp	'DdeAddData'						DdeAddData						user32		1639
imp	'DdeClientTransaction'					DdeClientTransaction					user32		1640
imp	'DdeCmpStringHandles'					DdeCmpStringHandles					user32		1641
imp	'DdeConnect'						DdeConnect						user32		1642
imp	'DdeConnectList'					DdeConnectList						user32		1643
imp	'DdeCreateDataHandle'					DdeCreateDataHandle					user32		1644
imp	'DdeCreateStringHandleA'				DdeCreateStringHandleA					user32		1645
imp	'DdeCreateStringHandle'					DdeCreateStringHandleW					user32		1646
imp	'DdeDisconnect'						DdeDisconnect						user32		1647
imp	'DdeDisconnectList'					DdeDisconnectList					user32		1648
imp	'DdeEnableCallback'					DdeEnableCallback					user32		1649
imp	'DdeFreeDataHandle'					DdeFreeDataHandle					user32		1650
imp	'DdeFreeStringHandle'					DdeFreeStringHandle					user32		1651
imp	'DdeGetData'						DdeGetData						user32		1652
imp	'DdeGetLastError'					DdeGetLastError						user32		1653
imp	'DdeGetQualityOfService'				DdeGetQualityOfService					user32		1654
imp	'DdeImpersonateClient'					DdeImpersonateClient					user32		1655
imp	'DdeInitializeA'					DdeInitializeA						user32		1656
imp	'DdeInitialize'						DdeInitializeW						user32		1657
imp	'DdeKeepStringHandle'					DdeKeepStringHandle					user32		1658
imp	'DdeNameService'					DdeNameService						user32		1659
imp	'DdePostAdvise'						DdePostAdvise						user32		1660
imp	'DdeQueryConvInfo'					DdeQueryConvInfo					user32		1661
imp	'DdeQueryNextServer'					DdeQueryNextServer					user32		1662
imp	'DdeQueryStringA'					DdeQueryStringA						user32		1663
imp	'DdeQueryString'					DdeQueryStringW						user32		1664
imp	'DdeReconnect'						DdeReconnect						user32		1665
imp	'DdeSetQualityOfService'				DdeSetQualityOfService					user32		1666
imp	'DdeSetUserHandle'					DdeSetUserHandle					user32		1667
imp	'DdeUnaccessData'					DdeUnaccessData						user32		1668
imp	'DdeUninitialize'					DdeUninitialize						user32		1669
imp	'DeactivateActCtx'					DeactivateActCtx					kernel32	0		# KernelBase
imp	'DeactivateActCtxWorker'				DeactivateActCtxWorker					kernel32	259
imp	'DebugActiveProcess'					DebugActiveProcess					kernel32	0	1	# KernelBase
imp	'DebugActiveProcessStop'				DebugActiveProcessStop					kernel32	0	1	# KernelBase
imp	'DebugBreakProcess'					DebugBreakProcess					kernel32	263	1
imp	'DebugSetProcessKillOnExit'				DebugSetProcessKillOnExit				kernel32	264
imp	'DecryptFileA'						DecryptFileA						advapi32	1235
imp	'DecryptFile'						DecryptFileW						advapi32	1236
imp	'DefFrameProcA'						DefFrameProcA						user32		1672
imp	'DefFrameProc'						DefFrameProcW						user32		1673
imp	'DefMDIChildProcA'					DefMDIChildProcA					user32		1674
imp	'DefMDIChildProc'					DefMDIChildProcW					user32		1675
imp	'DefRawInputProc'					DefRawInputProc						user32		1676
imp	'DeferWindowPos'					DeferWindowPos						user32		1679
imp	'DeferWindowPosAndBand'					DeferWindowPosAndBand					user32		1680
imp	'DefineDosDeviceA'					DefineDosDeviceA					kernel32	267
imp	'DefineDosDevice'					DefineDosDeviceW					kernel32	0		# KernelBase
imp	'DelayLoadFailureHook'					DelayLoadFailureHook					KernelBase	246
imp	'DelayLoadFailureHookLookup'				DelayLoadFailureHookLookup				KernelBase	247
imp	'DelegateInput'						DelegateInput						user32		2503
imp	'DeleteAce'						DeleteAce						advapi32	0		# KernelBase
imp	'DeleteAtom'						DeleteAtom						kernel32	270
imp	'DeleteBoundaryDescriptor'				DeleteBoundaryDescriptor				kernel32	0		# KernelBase
imp	'DeleteColorSpace'					DeleteColorSpace					gdi32		1383
imp	'DeleteDC'						DeleteDC						gdi32		1384	1
imp	'DeleteEnclave'						DeleteEnclave						KernelBase	251
imp	'DeleteEnhMetaFile'					DeleteEnhMetaFile					gdi32		1385
imp	'DeleteFiber'						DeleteFiber						kernel32	0		# KernelBase
imp	'DeleteFile'						DeleteFileW						kernel32	0	1	# KernelBase
imp	'DeleteFileA'						DeleteFileA						kernel32	0	1	# KernelBase
imp	'DeleteFileTransactedA'					DeleteFileTransactedA					kernel32	275
imp	'DeleteFileTransacted'					DeleteFileTransactedW					kernel32	276
imp	'DeleteMenu'						DeleteMenu						user32		1681	3
imp	'DeleteMetaFile'					DeleteMetaFile						gdi32		1386
imp	'DeleteObject'						DeleteObject						gdi32		1387	1
imp	'DeleteProcThreadAttributeList'				DeleteProcThreadAttributeList				kernel32	0	1	# KernelBase
imp	'DeleteService'						DeleteService						advapi32	1238
imp	'DeleteStateAtomValue'					DeleteStateAtomValue					KernelBase	256
imp	'DeleteStateContainer'					DeleteStateContainer					KernelBase	257
imp	'DeleteStateContainerValue'				DeleteStateContainerValue				KernelBase	258
imp	'DeleteSynchronizationBarrier'				DeleteSynchronizationBarrier				kernel32	279
imp	'DeleteTimerQueue'					DeleteTimerQueue					kernel32	280
imp	'DeleteTimerQueueEx'					DeleteTimerQueueEx					kernel32	0		# KernelBase
imp	'DeleteTimerQueueTimer'					DeleteTimerQueueTimer					kernel32	0		# KernelBase
imp	'DeleteUmsCompletionList'				DeleteUmsCompletionList					kernel32	283
imp	'DeleteUmsThreadContext'				DeleteUmsThreadContext					kernel32	284
imp	'DeleteVolumeMountPointA'				DeleteVolumeMountPointA					kernel32	285
imp	'DeleteVolumeMountPoint'				DeleteVolumeMountPointW					kernel32	0		# KernelBase
imp	'DequeueUmsCompletionListItems'				DequeueUmsCompletionListItems				kernel32	287
imp	'DeregisterEventSource'					DeregisterEventSource					advapi32	1239
imp	'DeregisterShellHookWindow'				DeregisterShellHookWindow				user32		1682
imp	'DeriveCapabilitySidsFromName'				DeriveCapabilitySidsFromName				KernelBase	263
imp	'DescribePixelFormat'					DescribePixelFormat					gdi32		1388
imp	'DestroyAcceleratorTable'				DestroyAcceleratorTable					user32		1683
imp	'DestroyCaret'						DestroyCaret						user32		1684
imp	'DestroyCursor'						DestroyCursor						user32		1685
imp	'DestroyDCompositionHwndTarget'				DestroyDCompositionHwndTarget				user32		1686
imp	'DestroyIcon'						DestroyIcon						user32		1687
imp	'DestroyMenu'						DestroyMenu						user32		1688	1
imp	'DestroyOPMProtectedOutput'				DestroyOPMProtectedOutput				gdi32		1389
imp	'DestroyPalmRejectionDelayZone'				DestroyPalmRejectionDelayZone				user32		1504
imp	'DestroyPhysicalMonitorInternal'			DestroyPhysicalMonitorInternal				gdi32		1390
imp	'DestroyPrivateObjectSecurity'				DestroyPrivateObjectSecurity				advapi32	0		# KernelBase
imp	'DestroyReasons'					DestroyReasons						user32		1689
imp	'DestroyWindow'						DestroyWindow						user32		1690	1
imp	'DeviceCapabilitiesExA'					DeviceCapabilitiesExA					gdi32		1391
imp	'DeviceIoControl'					DeviceIoControl						kernel32	0	8	# KernelBase
imp	'DialogBoxIndirectParamA'				DialogBoxIndirectParamA					user32		1691
imp	'DialogBoxIndirectParamAor'				DialogBoxIndirectParamAorW				user32		1692
imp	'DialogBoxIndirectParam'				DialogBoxIndirectParamW					user32		1693
imp	'DialogBoxParamA'					DialogBoxParamA						user32		1694
imp	'DialogBoxParam'					DialogBoxParamW						user32		1695
imp	'DisablePredefinedHandleTableInternal'			DisablePredefinedHandleTableInternal			KernelBase	266
imp	'DisableProcessWindowsGhosting'				DisableProcessWindowsGhosting				user32		1696
imp	'DisableThreadLibraryCalls'				DisableThreadLibraryCalls				kernel32	0		# KernelBase
imp	'DisableThreadProfiling'				DisableThreadProfiling					kernel32	290
imp	'DiscardVirtualMemory'					DiscardVirtualMemory					kernel32	0		# KernelBase
imp	'DisconnectNamedPipe'					DisconnectNamedPipe					kernel32	0	1	# KernelBase
imp	'DispatchMessage'					DispatchMessageW					user32		1698	1
imp	'DispatchMessageA'					DispatchMessageA					user32		1697	1
imp	'DisplayConfigGetDeviceInfo'				DisplayConfigGetDeviceInfo				user32		1699
imp	'DisplayConfigSetDeviceInfo'				DisplayConfigSetDeviceInfo				user32		1700
imp	'DisplayExitWindowsWarnings'				DisplayExitWindowsWarnings				user32		1701
imp	'DlgDirListA'						DlgDirListA						user32		1702
imp	'DlgDirListComboBoxA'					DlgDirListComboBoxA					user32		1703
imp	'DlgDirListComboBox'					DlgDirListComboBoxW					user32		1704
imp	'DlgDirList'						DlgDirListW						user32		1705
imp	'DlgDirSelectComboBoxExA'				DlgDirSelectComboBoxExA					user32		1706
imp	'DlgDirSelectComboBoxEx'				DlgDirSelectComboBoxExW					user32		1707
imp	'DlgDirSelectExA'					DlgDirSelectExA						user32		1708
imp	'DlgDirSelectEx'					DlgDirSelectExW						user32		1709
imp	'DllCanUnloadNow'					DllCanUnloadNow						comdlg32	107
imp	'DllGetActivationFactory'				DllGetActivationFactory					shell32		277
imp	'DllGetClassObject'					DllGetClassObject					comdlg32	108
imp	'DllGetVersion'						DllGetVersion						shell32		279
imp	'DllInstall'						DllInstall						shell32		280
imp	'DllRegisterServer'					DllRegisterServer					shell32		281
imp	'DllUnregisterServer'					DllUnregisterServer					shell32		282
imp	'DnsHostnameToComputerNameA'				DnsHostnameToComputerNameA				kernel32	294
imp	'DnsHostnameToComputerNameEx'				DnsHostnameToComputerNameExW				KernelBase	271
imp	'DnsHostnameToComputerName'				DnsHostnameToComputerNameW				kernel32	296
imp	'DoEnvironmentSubstA'					DoEnvironmentSubstA					shell32		283
imp	'DoEnvironmentSubst'					DoEnvironmentSubstW					shell32		284
imp	'DoSoundConnect'					DoSoundConnect						user32		1710
imp	'DoSoundDisconnect'					DoSoundDisconnect					user32		1711
imp	'DosDateTimeToFileTime'					DosDateTimeToFileTime					kernel32	297
imp	'DosPathToSessionPathA'					DosPathToSessionPathA					kernel32	298
imp	'DosPathToSessionPath'					DosPathToSessionPathW					kernel32	299
imp	'DragAcceptFiles'					DragAcceptFiles						shell32		285
imp	'DragDetect'						DragDetect						user32		1712
imp	'DragFinish'						DragFinish						shell32		286
imp	'DragObject'						DragObject						user32		1713
imp	'DragQueryFileA'					DragQueryFileA						shell32		288
imp	'DragQueryFileAor'					DragQueryFileAorW					shell32		289
imp	'DragQueryFile'						DragQueryFileW						shell32		290
imp	'DragQueryPoint'					DragQueryPoint						shell32		291
imp	'DrawAnimatedRects'					DrawAnimatedRects					user32		1714
imp	'DrawCaption'						DrawCaption						user32		1715
imp	'DrawCaptionTempA'					DrawCaptionTempA					user32		1716
imp	'DrawCaptionTemp'					DrawCaptionTempW					user32		1717
imp	'DrawEdge'						DrawEdge						user32		1718
imp	'DrawEscape'						DrawEscape						gdi32		1393
imp	'DrawFocusRect'						DrawFocusRect						user32		1719
imp	'DrawFrame'						DrawFrame						user32		1720
imp	'DrawFrameControl'					DrawFrameControl					user32		1721
imp	'DrawIcon'						DrawIcon						user32		1722
imp	'DrawIconEx'						DrawIconEx						user32		1723
imp	'DrawMenuBar'						DrawMenuBar						user32		1724
imp	'DrawMenuBarTemp'					DrawMenuBarTemp						user32		1725
imp	'DrawStateA'						DrawStateA						user32		1726
imp	'DrawState'						DrawStateW						user32		1727
imp	'DrawText'						DrawTextW						user32		1731	5
imp	'DrawTextA'						DrawTextA						user32		1728	5
imp	'DrawTextEx'						DrawTextExW						user32		1730	6
imp	'DrawTextExA'						DrawTextExA						user32		1729	6
imp	'DriveType'						DriveType						shell32		64
imp	'DsBindWithSpnEx'					DsBindWithSpnExW					KernelBase	272
imp	'DsCrackNames'						DsCrackNamesW						KernelBase	273
imp	'DsFreeDomainControllerInfo'				DsFreeDomainControllerInfoW				KernelBase	274
imp	'DsFreeNameResult'					DsFreeNameResultW					KernelBase	275
imp	'DsFreeNgcKey'						DsFreeNgcKey						KernelBase	276
imp	'DsFreePasswordCredentials'				DsFreePasswordCredentials				KernelBase	277
imp	'DsGetDomainControllerInfo'				DsGetDomainControllerInfoW				KernelBase	278
imp	'DsMakePasswordCredentials'				DsMakePasswordCredentialsW				KernelBase	279
imp	'DsReadNgcKey'						DsReadNgcKeyW						KernelBase	280
imp	'DsUnBind'						DsUnBindW						KernelBase	281
imp	'DsWriteNgcKey'						DsWriteNgcKeyW						KernelBase	282
imp	'DuplicateConsoleHandle'				DuplicateConsoleHandle					kernel32	300
imp	'DuplicateEncryptionInfoFile'				DuplicateEncryptionInfoFile				advapi32	1241
imp	'DuplicateEncryptionInfoFileExt'			DuplicateEncryptionInfoFileExt				kernel32	301
imp	'DuplicateHandle'					DuplicateHandle						kernel32	0	7	# KernelBase
imp	'DuplicateIcon'						DuplicateIcon						shell32		292
imp	'DuplicateStateContainerHandle'				DuplicateStateContainerHandle				KernelBase	284
imp	'DuplicateToken'					DuplicateToken						advapi32	0	3	# KernelBase
imp	'DuplicateTokenEx'					DuplicateTokenEx					advapi32	0	6	# KernelBase
imp	'DwmCreatedBitmapRemotingOutput'			DwmCreatedBitmapRemotingOutput				gdi32		1014
imp	'DwmGetDxRgn'						DwmGetDxRgn						user32		1553
imp	'DwmGetDxSharedSurface'					DwmGetDxSharedSurface					user32		1732
imp	'DwmGetRemoteSessionOcclusionEvent'			DwmGetRemoteSessionOcclusionEvent			user32		1733
imp	'DwmGetRemoteSessionOcclusionState'			DwmGetRemoteSessionOcclusionState			user32		1734
imp	'DwmKernelShutdown'					DwmKernelShutdown					user32		1735
imp	'DwmKernelStartup'					DwmKernelStartup					user32		1736
imp	'DwmLockScreenUpdates'					DwmLockScreenUpdates					user32		1737
imp	'DwmValidateWindow'					DwmValidateWindow					user32		1738
imp	'DxTrimNotificationListHead'				DxTrimNotificationListHead				gdi32		1394
imp	'EditWndProc'						EditWndProc						user32		1739
imp	'ElfBackupEventLogFileA'				ElfBackupEventLogFileA					advapi32	1244
imp	'ElfBackupEventLogFile'					ElfBackupEventLogFileW					advapi32	1245
imp	'ElfChangeNotify'					ElfChangeNotify						advapi32	1246
imp	'ElfClearEventLogFileA'					ElfClearEventLogFileA					advapi32	1247
imp	'ElfClearEventLogFile'					ElfClearEventLogFileW					advapi32	1248
imp	'ElfCloseEventLog'					ElfCloseEventLog					advapi32	1249
imp	'ElfDeregisterEventSource'				ElfDeregisterEventSource				advapi32	1250
imp	'ElfFlushEventLog'					ElfFlushEventLog					advapi32	1251
imp	'ElfNumberOfRecords'					ElfNumberOfRecords					advapi32	1252
imp	'ElfOldestRecord'					ElfOldestRecord						advapi32	1253
imp	'ElfOpenBackupEventLogA'				ElfOpenBackupEventLogA					advapi32	1254
imp	'ElfOpenBackupEventLog'					ElfOpenBackupEventLogW					advapi32	1255
imp	'ElfOpenEventLogA'					ElfOpenEventLogA					advapi32	1256
imp	'ElfOpenEventLog'					ElfOpenEventLogW					advapi32	1257
imp	'ElfReadEventLogA'					ElfReadEventLogA					advapi32	1258
imp	'ElfReadEventLog'					ElfReadEventLogW					advapi32	1259
imp	'ElfRegisterEventSourceA'				ElfRegisterEventSourceA					advapi32	1260
imp	'ElfRegisterEventSource'				ElfRegisterEventSourceW					advapi32	1261
imp	'ElfReportEventA'					ElfReportEventA						advapi32	1262
imp	'ElfReportEventAndSource'				ElfReportEventAndSourceW				advapi32	1263
imp	'ElfReportEvent'					ElfReportEventW						advapi32	1264
imp	'Ellipse'						Ellipse							gdi32		1395
imp	'EmptyClipboard'					EmptyClipboard						user32		1740
imp	'EmptyWorkingSet'					EmptyWorkingSet						KernelBase	287
imp	'EnableEUDC'						EnableEUDC						gdi32		1396
imp	'EnableMenuItem'					EnableMenuItem						user32		1741
imp	'EnableMouseInPointer'					EnableMouseInPointer					user32		1742
imp	'EnableNonClientDpiScaling'				EnableNonClientDpiScaling				user32		1743
imp	'EnableOneCoreTransformMode'				EnableOneCoreTransformMode				user32		1744
imp	'EnableScrollBar'					EnableScrollBar						user32		1745
imp	'EnableSessionForMMCSS'					EnableSessionForMMCSS					user32		1746
imp	'EnableThreadProfiling'					EnableThreadProfiling					kernel32	303
imp	'EnableTrace'						EnableTrace						advapi32	1265
imp	'EnableTraceEx'						EnableTraceEx						advapi32	1266
imp	'EnableTraceEx2'					EnableTraceEx2						advapi32	1267
imp	'EnableWindow'						EnableWindow						user32		1747
imp	'EncryptFileA'						EncryptFileA						advapi32	1268
imp	'EncryptFile'						EncryptFileW						advapi32	1269
imp	'EncryptedFileKeyInfo'					EncryptedFileKeyInfo					advapi32	1270
imp	'EncryptionDisable'					EncryptionDisable					advapi32	1271
imp	'EndDeferWindowPos'					EndDeferWindowPos					user32		1748
imp	'EndDeferWindowPosEx'					EndDeferWindowPosEx					user32		1749
imp	'EndDialog'						EndDialog						user32		1750
imp	'EndDoc'						EndDoc							gdi32		1397
imp	'EndFormPage'						EndFormPage						gdi32		1398
imp	'EndGdiRendering'					EndGdiRendering						gdi32		1399
imp	'EndMenu'						EndMenu							user32		1751
imp	'EndPage'						EndPage							gdi32		1400
imp	'EndPaint'						EndPaint						user32		1752	2
imp	'EndPath'						EndPath							gdi32		1401
imp	'EndTask'						EndTask							user32		1753
imp	'EndUpdateResourceA'					EndUpdateResourceA					kernel32	306
imp	'EndUpdateResource'					EndUpdateResourceW					kernel32	307
imp	'EngAcquireSemaphore'					EngAcquireSemaphore					gdi32		1402
imp	'EngAlphaBlend'						EngAlphaBlend						gdi32		1403
imp	'EngAssociateSurface'					EngAssociateSurface					gdi32		1404
imp	'EngBitBlt'						EngBitBlt						gdi32		1405
imp	'EngCheckAbort'						EngCheckAbort						gdi32		1406
imp	'EngComputeGlyphSet'					EngComputeGlyphSet					gdi32		1407
imp	'EngCopyBits'						EngCopyBits						gdi32		1408
imp	'EngCreateBitmap'					EngCreateBitmap						gdi32		1409
imp	'EngCreateClip'						EngCreateClip						gdi32		1410
imp	'EngCreateDeviceBitmap'					EngCreateDeviceBitmap					gdi32		1411
imp	'EngCreateDeviceSurface'				EngCreateDeviceSurface					gdi32		1412
imp	'EngCreatePalette'					EngCreatePalette					gdi32		1413
imp	'EngCreateSemaphore'					EngCreateSemaphore					gdi32		1414
imp	'EngDeleteClip'						EngDeleteClip						gdi32		1415
imp	'EngDeletePalette'					EngDeletePalette					gdi32		1416
imp	'EngDeletePath'						EngDeletePath						gdi32		1417
imp	'EngDeleteSemaphore'					EngDeleteSemaphore					gdi32		1418
imp	'EngDeleteSurface'					EngDeleteSurface					gdi32		1419
imp	'EngEraseSurface'					EngEraseSurface						gdi32		1420
imp	'EngFillPath'						EngFillPath						gdi32		1421
imp	'EngFindResource'					EngFindResource						gdi32		1422
imp	'EngFreeModule'						EngFreeModule						gdi32		1423
imp	'EngGetCurrentCodePage'					EngGetCurrentCodePage					gdi32		1424
imp	'EngGetDriverName'					EngGetDriverName					gdi32		1425
imp	'EngGetPrinterDataFileName'				EngGetPrinterDataFileName				gdi32		1426
imp	'EngGradientFill'					EngGradientFill						gdi32		1427
imp	'EngLineTo'						EngLineTo						gdi32		1428
imp	'EngLoadModule'						EngLoadModule						gdi32		1429
imp	'EngLockSurface'					EngLockSurface						gdi32		1430
imp	'EngMarkBandingSurface'					EngMarkBandingSurface					gdi32		1431
imp	'EngMultiByteToUnicodeN'				EngMultiByteToUnicodeN					gdi32		1432
imp	'EngMultiByteToWideChar'				EngMultiByteToWideChar					gdi32		1433
imp	'EngPaint'						EngPaint						gdi32		1434
imp	'EngPlgBlt'						EngPlgBlt						gdi32		1435
imp	'EngQueryEMFInfo'					EngQueryEMFInfo						gdi32		1436
imp	'EngQueryLocalTime'					EngQueryLocalTime					gdi32		1437
imp	'EngReleaseSemaphore'					EngReleaseSemaphore					gdi32		1438
imp	'EngStretchBlt'						EngStretchBlt						gdi32		1439
imp	'EngStretchBltROP'					EngStretchBltROP					gdi32		1440
imp	'EngStrokeAndFillPath'					EngStrokeAndFillPath					gdi32		1441
imp	'EngStrokePath'						EngStrokePath						gdi32		1442
imp	'EngTextOut'						EngTextOut						gdi32		1443
imp	'EngTransparentBlt'					EngTransparentBlt					gdi32		1444
imp	'EngUnicodeToMultiByteN'				EngUnicodeToMultiByteN					gdi32		1445
imp	'EngUnlockSurface'					EngUnlockSurface					gdi32		1446
imp	'EngWideCharToMultiByte'				EngWideCharToMultiByte					gdi32		1447
imp	'EnterCriticalPolicySectionInternal'			EnterCriticalPolicySectionInternal			KernelBase	291
imp	'EnterReaderModeHelper'					EnterReaderModeHelper					user32		1754
imp	'EnterSynchronizationBarrier'				EnterSynchronizationBarrier				kernel32	0		# KernelBase
imp	'EnterUmsSchedulingMode'				EnterUmsSchedulingMode					kernel32	310
imp	'EnumCalendarInfoA'					EnumCalendarInfoA					kernel32	311
imp	'EnumCalendarInfoExA'					EnumCalendarInfoExA					kernel32	312
imp	'EnumCalendarInfoExEx'					EnumCalendarInfoExEx					kernel32	0		# KernelBase
imp	'EnumCalendarInfoEx'					EnumCalendarInfoExW					kernel32	0		# KernelBase
imp	'EnumCalendarInfo'					EnumCalendarInfoW					kernel32	0		# KernelBase
imp	'EnumChildWindows'					EnumChildWindows					user32		1755	3
imp	'EnumClipboardFormats'					EnumClipboardFormats					user32		1756
imp	'EnumDateFormatsA'					EnumDateFormatsA					kernel32	316
imp	'EnumDateFormatsExA'					EnumDateFormatsExA					kernel32	317
imp	'EnumDateFormatsExEx'					EnumDateFormatsExEx					kernel32	0		# KernelBase
imp	'EnumDateFormatsEx'					EnumDateFormatsExW					kernel32	0		# KernelBase
imp	'EnumDateFormats'					EnumDateFormatsW					kernel32	0		# KernelBase
imp	'EnumDependentServicesA'				EnumDependentServicesA					advapi32	1272
imp	'EnumDependentServices'					EnumDependentServicesW					advapi32	1273
imp	'EnumDesktopWindows'					EnumDesktopWindows					user32		1757
imp	'EnumDesktopsA'						EnumDesktopsA						user32		1758
imp	'EnumDesktops'						EnumDesktopsW						user32		1759
imp	'EnumDeviceDrivers'					EnumDeviceDrivers					KernelBase	300
imp	'EnumDisplayDevicesA'					EnumDisplayDevicesA					user32		1760
imp	'EnumDisplayDevices'					EnumDisplayDevicesW					user32		1761
imp	'EnumDisplayMonitors'					EnumDisplayMonitors					user32		1762
imp	'EnumDisplaySettingsA'					EnumDisplaySettingsA					user32		1763
imp	'EnumDisplaySettingsExA'				EnumDisplaySettingsExA					user32		1764
imp	'EnumDisplaySettingsEx'					EnumDisplaySettingsExW					user32		1765
imp	'EnumDisplaySettings'					EnumDisplaySettingsW					user32		1766
imp	'EnumDynamicTimeZoneInformation'			EnumDynamicTimeZoneInformation				KernelBase	301
imp	'EnumEnhMetaFile'					EnumEnhMetaFile						gdi32		1448
imp	'EnumFontFamiliesA'					EnumFontFamiliesA					gdi32		1449
imp	'EnumFontFamiliesExA'					EnumFontFamiliesExA					gdi32		1450
imp	'EnumFontFamiliesEx'					EnumFontFamiliesExW					gdi32		1451
imp	'EnumFontFamilies'					EnumFontFamiliesW					gdi32		1452
imp	'EnumFontsA'						EnumFontsA						gdi32		1453
imp	'EnumFonts'						EnumFontsW						gdi32		1454
imp	'EnumICMProfilesA'					EnumICMProfilesA					gdi32		1455
imp	'EnumICMProfiles'					EnumICMProfilesW					gdi32		1456
imp	'EnumLanguageGroupLocalesA'				EnumLanguageGroupLocalesA				kernel32	321
imp	'EnumLanguageGroupLocales'				EnumLanguageGroupLocalesW				kernel32	0		# KernelBase
imp	'EnumMetaFile'						EnumMetaFile						gdi32		1457
imp	'EnumObjects'						EnumObjects						gdi32		1458
imp	'EnumPageFilesA'					EnumPageFilesA						KernelBase	303
imp	'EnumPageFiles'						EnumPageFilesW						KernelBase	304
imp	'EnumProcessModules'					EnumProcessModules					KernelBase	305
imp	'EnumProcessModulesEx'					EnumProcessModulesEx					KernelBase	306
imp	'EnumProcesses'						EnumProcesses						KernelBase	307
imp	'EnumPropsA'						EnumPropsA						user32		1767
imp	'EnumPropsExA'						EnumPropsExA						user32		1768
imp	'EnumPropsEx'						EnumPropsExW						user32		1769
imp	'EnumProps'						EnumPropsW						user32		1770
imp	'EnumResourceLanguagesA'				EnumResourceLanguagesA					kernel32	323
imp	'EnumResourceLanguagesExA'				EnumResourceLanguagesExA				kernel32	0		# KernelBase
imp	'EnumResourceLanguagesEx'				EnumResourceLanguagesExW				kernel32	0		# KernelBase
imp	'EnumResourceLanguages'					EnumResourceLanguagesW					kernel32	326
imp	'EnumResourceNamesA'					EnumResourceNamesA					kernel32	327
imp	'EnumResourceNamesExA'					EnumResourceNamesExA					kernel32	0		# KernelBase
imp	'EnumResourceNamesEx'					EnumResourceNamesExW					kernel32	0		# KernelBase
imp	'EnumResourceNames'					EnumResourceNamesW					kernel32	0		# KernelBase
imp	'EnumResourceTypesA'					EnumResourceTypesA					kernel32	331
imp	'EnumResourceTypesExA'					EnumResourceTypesExA					kernel32	0		# KernelBase
imp	'EnumResourceTypesEx'					EnumResourceTypesExW					kernel32	0		# KernelBase
imp	'EnumResourceTypes'					EnumResourceTypesW					kernel32	334
imp	'EnumServiceGroup'					EnumServiceGroupW					advapi32	1275
imp	'EnumServicesStatusA'					EnumServicesStatusA					advapi32	1276
imp	'EnumServicesStatusExA'					EnumServicesStatusExA					advapi32	1277
imp	'EnumServicesStatusEx'					EnumServicesStatusExW					advapi32	1278
imp	'EnumServicesStatus'					EnumServicesStatusW					advapi32	1279
imp	'EnumSystemCodePagesA'					EnumSystemCodePagesA					kernel32	335
imp	'EnumSystemCodePages'					EnumSystemCodePagesW					kernel32	0		# KernelBase
imp	'EnumSystemFirmwareTables'				EnumSystemFirmwareTables				kernel32	0		# KernelBase
imp	'EnumSystemGeoID'					EnumSystemGeoID						kernel32	0		# KernelBase
imp	'EnumSystemGeoNames'					EnumSystemGeoNames					KernelBase	318
imp	'EnumSystemLanguageGroupsA'				EnumSystemLanguageGroupsA				kernel32	340
imp	'EnumSystemLanguageGroups'				EnumSystemLanguageGroupsW				kernel32	0		# KernelBase
imp	'EnumSystemLocalesA'					EnumSystemLocalesA					kernel32	0		# KernelBase
imp	'EnumSystemLocalesEx'					EnumSystemLocalesEx					kernel32	0		# KernelBase
imp	'EnumSystemLocales'					EnumSystemLocalesW					kernel32	0		# KernelBase
imp	'EnumThreadWindows'					EnumThreadWindows					user32		1771
imp	'EnumTimeFormatsA'					EnumTimeFormatsA					kernel32	345
imp	'EnumTimeFormatsEx'					EnumTimeFormatsEx					kernel32	0		# KernelBase
imp	'EnumTimeFormats'					EnumTimeFormatsW					kernel32	0		# KernelBase
imp	'EnumUILanguagesA'					EnumUILanguagesA					kernel32	348
imp	'EnumUILanguages'					EnumUILanguagesW					kernel32	0		# KernelBase
imp	'EnumWindowStationsA'					EnumWindowStationsA					user32		1772
imp	'EnumWindowStations'					EnumWindowStationsW					user32		1773
imp	'EnumWindows'						EnumWindows						user32		1774
imp	'EnumerateExtensionNames'				EnumerateExtensionNames					KernelBase	326
imp	'EnumerateLocalComputerNamesA'				EnumerateLocalComputerNamesA				kernel32	350
imp	'EnumerateLocalComputerNames'				EnumerateLocalComputerNamesW				kernel32	351
imp	'EnumerateStateAtomValues'				EnumerateStateAtomValues				KernelBase	327
imp	'EnumerateStateContainerItems'				EnumerateStateContainerItems				KernelBase	328
imp	'EnumerateTraceGuids'					EnumerateTraceGuids					advapi32	1280
imp	'EnumerateTraceGuidsEx'					EnumerateTraceGuidsEx					advapi32	1281
imp	'EqualDomainSid'					EqualDomainSid						advapi32	0		# KernelBase
imp	'EqualPrefixSid'					EqualPrefixSid						advapi32	0		# KernelBase
imp	'EqualRect'						EqualRect						user32		1775
imp	'EqualRgn'						EqualRgn						gdi32		1459
imp	'EqualSid'						EqualSid						advapi32	0		# KernelBase
imp	'EraseTape'						EraseTape						kernel32	352
imp	'Escape'						Escape							gdi32		1460
imp	'EscapeCommFunction'					EscapeCommFunction					kernel32	0		# KernelBase
imp	'EtwCheckCoverage'					EtwCheckCoverage					ntdll		60
imp	'EtwCreateTraceInstanceId'				EtwCreateTraceInstanceId				ntdll		61
imp	'EtwDeliverDataBlock'					EtwDeliverDataBlock					ntdll		62
imp	'EtwEnumerateProcessRegGuids'				EtwEnumerateProcessRegGuids				ntdll		63
imp	'EtwEventActivityIdControl'				EtwEventActivityIdControl				ntdll		64
imp	'EtwEventEnabled'					EtwEventEnabled						ntdll		65
imp	'EtwEventProviderEnabled'				EtwEventProviderEnabled					ntdll		66
imp	'EtwEventRegister'					EtwEventRegister					ntdll		67
imp	'EtwEventSetInformation'				EtwEventSetInformation					ntdll		68
imp	'EtwEventUnregister'					EtwEventUnregister					ntdll		69
imp	'EtwEventWrite'						EtwEventWrite						ntdll		70
imp	'EtwEventWriteEndScenario'				EtwEventWriteEndScenario				ntdll		71
imp	'EtwEventWriteEx'					EtwEventWriteEx						ntdll		72
imp	'EtwEventWriteFull'					EtwEventWriteFull					ntdll		73
imp	'EtwEventWriteNoRegistration'				EtwEventWriteNoRegistration				ntdll		74
imp	'EtwEventWriteStartScenario'				EtwEventWriteStartScenario				ntdll		75
imp	'EtwEventWriteString'					EtwEventWriteString					ntdll		76
imp	'EtwEventWriteTransfer'					EtwEventWriteTransfer					ntdll		77
imp	'EtwGetTraceEnableFlags'				EtwGetTraceEnableFlags					ntdll		78
imp	'EtwGetTraceEnableLevel'				EtwGetTraceEnableLevel					ntdll		79
imp	'EtwGetTraceLoggerHandle'				EtwGetTraceLoggerHandle					ntdll		80
imp	'EtwLogTraceEvent'					EtwLogTraceEvent					ntdll		81
imp	'EtwNotificationRegister'				EtwNotificationRegister					ntdll		82
imp	'EtwNotificationUnregister'				EtwNotificationUnregister				ntdll		83
imp	'EtwProcessPrivateLoggerRequest'			EtwProcessPrivateLoggerRequest				ntdll		84
imp	'EtwRegisterSecurityProvider'				EtwRegisterSecurityProvider				ntdll		85
imp	'EtwRegisterTraceGuidsA'				EtwRegisterTraceGuidsA					ntdll		86
imp	'EtwRegisterTraceGuids'					EtwRegisterTraceGuidsW					ntdll		87
imp	'EtwReplyNotification'					EtwReplyNotification					ntdll		88
imp	'EtwSendNotification'					EtwSendNotification					ntdll		89
imp	'EtwSetMark'						EtwSetMark						ntdll		90
imp	'EtwTraceEventInstance'					EtwTraceEventInstance					ntdll		91
imp	'EtwTraceMessage'					EtwTraceMessage						ntdll		92
imp	'EtwTraceMessageVa'					EtwTraceMessageVa					ntdll		93
imp	'EtwUnregisterTraceGuids'				EtwUnregisterTraceGuids					ntdll		94
imp	'EtwWriteUMSecurityEvent'				EtwWriteUMSecurityEvent					ntdll		95
imp	'EtwpCreateEtwThread'					EtwpCreateEtwThread					ntdll		96
imp	'EtwpGetCpuSpeed'					EtwpGetCpuSpeed						ntdll		97
imp	'EudcLoadLink'						EudcLoadLinkW						gdi32		1461
imp	'EudcUnloadLink'					EudcUnloadLinkW						gdi32		1462
imp	'EvaluateProximityToPolygon'				EvaluateProximityToPolygon				user32		1776
imp	'EvaluateProximityToRect'				EvaluateProximityToRect					user32		1777
imp	'EventAccessControl'					EventAccessControl					advapi32	1285
imp	'EventAccessQuery'					EventAccessQuery					advapi32	1286
imp	'EventAccessRemove'					EventAccessRemove					advapi32	1287
imp	'EvtIntReportAuthzEventAndSourceAsync'			EvtIntReportAuthzEventAndSourceAsync			ntdll		98
imp	'EvtIntReportEventAndSourceAsync'			EvtIntReportEventAndSourceAsync				ntdll		99
imp	'ExcludeClipRect'					ExcludeClipRect						gdi32		1463
imp	'ExcludeUpdateRgn'					ExcludeUpdateRgn					user32		1778
imp	'ExecuteUmsThread'					ExecuteUmsThread					kernel32	354
imp	'ExitProcess'						ExitProcess						kernel32	0	1	# a.k.a. RtlExitUserProcess	# KernelBase
imp	'ExitVDM'						ExitVDM							kernel32	357
imp	'ExitWindowsEx'						ExitWindowsEx						user32		1779
imp	'ExpInterlockedPopEntrySListEnd'			ExpInterlockedPopEntrySListEnd				ntdll		100
imp	'ExpInterlockedPopEntrySListFault'			ExpInterlockedPopEntrySListFault			ntdll		101
imp	'ExpInterlockedPopEntrySListResume'			ExpInterlockedPopEntrySListResume			ntdll		102
imp	'ExpandEnvironmentStringsA'				ExpandEnvironmentStringsA				kernel32	0		# KernelBase
imp	'ExpandEnvironmentStrings'				ExpandEnvironmentStringsW				kernel32	0		# KernelBase
imp	'ExpungeConsoleCommandHistoryA'				ExpungeConsoleCommandHistoryA				KernelBase	347
imp	'ExpungeConsoleCommandHistory'				ExpungeConsoleCommandHistoryW				KernelBase	348
imp	'ExtCreatePen'						ExtCreatePen						gdi32		1464
imp	'ExtCreateRegion'					ExtCreateRegion						gdi32		1465
imp	'ExtEscape'						ExtEscape						gdi32		1466
imp	'ExtFloodFill'						ExtFloodFill						gdi32		1467
imp	'ExtSelectClipRgn'					ExtSelectClipRgn					gdi32		1468
imp	'ExtTextOutA'						ExtTextOutA						gdi32		1469
imp	'ExtTextOut'						ExtTextOutW						gdi32		1470
imp	'ExtensionProgIdExists'					ExtensionProgIdExists					KernelBase	349
imp	'ExtractAssociatedIconA'				ExtractAssociatedIconA					shell32		293
imp	'ExtractAssociatedIconExA'				ExtractAssociatedIconExA				shell32		294
imp	'ExtractAssociatedIconEx'				ExtractAssociatedIconExW				shell32		295
imp	'ExtractAssociatedIcon'					ExtractAssociatedIconW					shell32		296
imp	'ExtractIconA'						ExtractIconA						shell32		297
imp	'ExtractIconExA'					ExtractIconExA						shell32		299
imp	'ExtractIconEx'						ExtractIconExW						shell32		300
imp	'ExtractIcon'						ExtractIconW						shell32		301
imp	'FONTOBJ_cGetAllGlyphHandles'				FONTOBJ_cGetAllGlyphHandles				gdi32		1471
imp	'FONTOBJ_cGetGlyphs'					FONTOBJ_cGetGlyphs					gdi32		1472
imp	'FONTOBJ_pQueryGlyphAttrs'				FONTOBJ_pQueryGlyphAttrs				gdi32		1473
imp	'FONTOBJ_pfdg'						FONTOBJ_pfdg						gdi32		1474
imp	'FONTOBJ_pifi'						FONTOBJ_pifi						gdi32		1475
imp	'FONTOBJ_pvTrueTypeFontFile'				FONTOBJ_pvTrueTypeFontFile				gdi32		1476
imp	'FONTOBJ_pxoGetXform'					FONTOBJ_pxoGetXform					gdi32		1477
imp	'FONTOBJ_vGetInfo'					FONTOBJ_vGetInfo					gdi32		1478
imp	'FatalAppExitA'						FatalAppExitA						kernel32	0		# KernelBase
imp	'FatalAppExit'						FatalAppExitW						kernel32	0		# KernelBase
imp	'FatalExit'						FatalExit						kernel32	364	1
imp	'FileEncryptionStatusA'					FileEncryptionStatusA					advapi32	1300
imp	'FileEncryptionStatus'					FileEncryptionStatusW					advapi32	1301
imp	'FileProtocolHandler'					FileProtocolHandler					url		104
imp	'FileProtocolHandlerA'					FileProtocolHandlerA					url		105
imp	'FileTimeToDosDateTime'					FileTimeToDosDateTime					kernel32	365
imp	'FileTimeToLocalFileTime'				FileTimeToLocalFileTime					kernel32	0		# KernelBase
imp	'FileTimeToSystemTime'					FileTimeToSystemTime					kernel32	0		# KernelBase
imp	'FillConsoleOutputAttribute'				FillConsoleOutputAttribute				kernel32	0	5	# KernelBase
imp	'FillConsoleOutputCharacter'				FillConsoleOutputCharacterW				kernel32	0	5	# KernelBase
imp	'FillConsoleOutputCharacterA'				FillConsoleOutputCharacterA				kernel32	0	5	# KernelBase
imp	'FillPath'						FillPath						gdi32		1479
imp	'FillRect'						FillRect						user32		1780	3
imp	'FillRgn'						FillRgn							gdi32		1480
imp	'FindActCtxSectionGuid'					FindActCtxSectionGuid					kernel32	0		# KernelBase
imp	'FindActCtxSectionGuidWorker'				FindActCtxSectionGuidWorker				kernel32	372
imp	'FindActCtxSectionStringA'				FindActCtxSectionStringA				kernel32	373
imp	'FindActCtxSectionString'				FindActCtxSectionStringW				kernel32	0		# KernelBase
imp	'FindActCtxSectionStringWWorker'			FindActCtxSectionStringWWorker				kernel32	375
imp	'FindAtomA'						FindAtomA						kernel32	376
imp	'FindAtom'						FindAtomW						kernel32	377
imp	'FindClose'						FindClose						kernel32	0	1	# KernelBase
imp	'FindCloseChangeNotification'				FindCloseChangeNotification				kernel32	0		# KernelBase
imp	'FindExecutableA'					FindExecutableA						shell32		302
imp	'FindExecutable'					FindExecutableW						shell32		303
imp	'FindFirstChangeNotificationA'				FindFirstChangeNotificationA				kernel32	0		# KernelBase
imp	'FindFirstChangeNotification'				FindFirstChangeNotificationW				kernel32	0		# KernelBase
imp	'FindFirstFile'						FindFirstFileW						kernel32	0	2	# KernelBase
imp	'FindFirstFileA'					FindFirstFileA						kernel32	0	2	# KernelBase
imp	'FindFirstFileEx'					FindFirstFileExW					kernel32	0	6	# KernelBase
imp	'FindFirstFileExA'					FindFirstFileExA					kernel32	0	6	# KernelBase
imp	'FindFirstFileNameTransacted'				FindFirstFileNameTransactedW				kernel32	385
imp	'FindFirstFileName'					FindFirstFileNameW					kernel32	0		# KernelBase
imp	'FindFirstFileTransactedA'				FindFirstFileTransactedA				kernel32	387
imp	'FindFirstFileTransacted'				FindFirstFileTransactedW				kernel32	388
imp	'FindFirstFreeAce'					FindFirstFreeAce					advapi32	0		# KernelBase
imp	'FindFirstStream'					FindFirstStreamW					kernel32	0		# KernelBase
imp	'FindFirstStreamTransacted'				FindFirstStreamTransactedW				kernel32	390
imp	'FindFirstVolume'					FindFirstVolumeW					kernel32	0	2	# KernelBase
imp	'FindFirstVolumeA'					FindFirstVolumeA					kernel32	392	2
imp	'FindFirstVolumeMountPointA'				FindFirstVolumeMountPointA				kernel32	393
imp	'FindFirstVolumeMountPoint'				FindFirstVolumeMountPointW				kernel32	394
imp	'FindNLSString'						FindNLSString						kernel32	0		# KernelBase
imp	'FindNLSStringEx'					FindNLSStringEx						kernel32	0		# KernelBase
imp	'FindNextChangeNotification'				FindNextChangeNotification				kernel32	0		# KernelBase
imp	'FindNextFile'						FindNextFileW						kernel32	0	2	# KernelBase
imp	'FindNextFileA'						FindNextFileA						kernel32	0	2	# KernelBase
imp	'FindNextFileName'					FindNextFileNameW					kernel32	0		# KernelBase
imp	'FindNextStream'					FindNextStreamW						kernel32	0		# KernelBase
imp	'FindNextVolume'					FindNextVolumeW						kernel32	0	3	# KernelBase
imp	'FindNextVolumeA'					FindNextVolumeA						kernel32	403	3
imp	'FindNextVolumeMountPointA'				FindNextVolumeMountPointA				kernel32	404
imp	'FindNextVolumeMountPoint'				FindNextVolumeMountPointW				kernel32	405
imp	'FindPackagesByPackageFamily'				FindPackagesByPackageFamily				kernel32	0		# KernelBase
imp	'FindResourceA'						FindResourceA						kernel32	408
imp	'FindResourceExA'					FindResourceExA						kernel32	409
imp	'FindResourceEx'					FindResourceExW						kernel32	0		# KernelBase
imp	'FindResource'						FindResourceW						kernel32	0		# KernelBase
imp	'FindStringOrdinal'					FindStringOrdinal					kernel32	0		# KernelBase
imp	'FindTextA'						FindTextA						comdlg32	109
imp	'FindText'						FindTextW						comdlg32	110
imp	'FindVolumeClose'					FindVolumeClose						kernel32	0	1	# KernelBase
imp	'FindVolumeMountPointClose'				FindVolumeMountPointClose				kernel32	414
imp	'FindWindow'						FindWindowW						user32		1784	2
imp	'FindWindowA'						FindWindowA						user32		1781	2
imp	'FindWindowEx'						FindWindowExW						user32		1783	4
imp	'FindWindowExA'						FindWindowExA						user32		1782	4
imp	'FixBrushOrgEx'						FixBrushOrgEx						gdi32		1481
imp	'FlashWindow'						FlashWindow						user32		1785
imp	'FlashWindowEx'						FlashWindowEx						user32		1786
imp	'FlattenPath'						FlattenPath						gdi32		1482
imp	'FloodFill'						FloodFill						gdi32		1483
imp	'FlsAlloc'						FlsAlloc						kernel32	0		# KernelBase
imp	'FlsFree'						FlsFree							kernel32	0		# KernelBase
imp	'FlsGetValue'						FlsGetValue						kernel32	0		# KernelBase
imp	'FlsSetValue'						FlsSetValue						kernel32	0		# KernelBase
imp	'FlushConsoleInputBuffer'				FlushConsoleInputBuffer					kernel32	0	1	# KernelBase
imp	'FlushEfsCache'						FlushEfsCache						advapi32	1303
imp	'FlushFileBuffers'					FlushFileBuffers					kernel32	0	1	# KernelBase
imp	'FlushInstructionCache'					FlushInstructionCache					kernel32	0		# KernelBase
imp	'FlushTraceA'						FlushTraceA						advapi32	1304
imp	'FlushTrace'						FlushTraceW						advapi32	1305
imp	'FlushViewOfFile'					FlushViewOfFile						kernel32	0	2	# KernelBase
imp	'FoldStringA'						FoldStringA						kernel32	424
imp	'FoldString'						FoldStringW						kernel32	0		# KernelBase
imp	'FontIsLinked'						FontIsLinked						gdi32		1484
imp	'ForceSyncFgPolicyInternal'				ForceSyncFgPolicyInternal				KernelBase	394
imp	'FormatApplicationUserModelId'				FormatApplicationUserModelId				kernel32	0		# KernelBase
imp	'FormatApplicationUserModelIdA'				FormatApplicationUserModelIdA				KernelBase	396
imp	'FormatMessage'						FormatMessageW						kernel32	0	7	# KernelBase
imp	'FormatMessageA'					FormatMessageA						kernel32	0	7	# KernelBase
imp	'FrameRect'						FrameRect						user32		1787
imp	'FrameRgn'						FrameRgn						gdi32		1485
imp	'FreeAddrInfoEx'					FreeAddrInfoExW						ws2_32		26
imp	'FreeAddrInfo'						FreeAddrInfoW						ws2_32		27
imp	'FreeConsole'						FreeConsole						kernel32	0	0	# KernelBase
imp	'FreeDDElParam'						FreeDDElParam						user32		1788
imp	'FreeEncryptedFileKeyInfo'				FreeEncryptedFileKeyInfo				advapi32	1306
imp	'FreeEncryptedFileMetadata'				FreeEncryptedFileMetadata				advapi32	1307
imp	'FreeEncryptionCertificateHashList'			FreeEncryptionCertificateHashList			advapi32	1308
imp	'FreeEnvironmentStrings'				FreeEnvironmentStringsW					kernel32	0	1	# KernelBase
imp	'FreeEnvironmentStringsA'				FreeEnvironmentStringsA					kernel32	0	1	# KernelBase
imp	'FreeGPOListInternalA'					FreeGPOListInternalA					KernelBase	402
imp	'FreeGPOListInternal'					FreeGPOListInternalW					KernelBase	403
imp	'FreeIconList'						FreeIconList						shell32		304
imp	'FreeInheritedFromArray'				FreeInheritedFromArray					advapi32	1309
imp	'FreeLibrary'						FreeLibrary						kernel32	0	1	# KernelBase
imp	'FreeLibraryAndExitThread'				FreeLibraryAndExitThread				kernel32	0		# KernelBase
imp	'FreeMemoryJobObject'					FreeMemoryJobObject					kernel32	435
imp	'FreeResource'						FreeResource						kernel32	0	1	# KernelBase
imp	'FreeSid'						FreeSid							advapi32	0		# KernelBase
imp	'FreeUserPhysicalPages'					FreeUserPhysicalPages					kernel32	0		# KernelBase
imp	'FrostCrashedWindow'					FrostCrashedWindow					user32		1789
imp	'Gdi32DllInitialize'					Gdi32DllInitialize					gdi32		1486
imp	'GdiAddFontResource'					GdiAddFontResourceW					gdi32		1487
imp	'GdiAddGlsBounds'					GdiAddGlsBounds						gdi32		1488
imp	'GdiAddGlsRecord'					GdiAddGlsRecord						gdi32		1489
imp	'GdiAddInitialFonts'					GdiAddInitialFonts					gdi32		1490
imp	'GdiAlphaBlend'						GdiAlphaBlend						gdi32		1491
imp	'GdiArtificialDecrementDriver'				GdiArtificialDecrementDriver				gdi32		1492
imp	'GdiBatchLimit'						GdiBatchLimit						gdi32		1493
imp	'GdiCleanCacheDC'					GdiCleanCacheDC						gdi32		1494
imp	'GdiComment'						GdiComment						gdi32		1495
imp	'GdiConsoleTextOut'					GdiConsoleTextOut					gdi32		1496
imp	'GdiConvertAndCheckDC'					GdiConvertAndCheckDC					gdi32		1497
imp	'GdiConvertBitmap'					GdiConvertBitmap					gdi32		1498
imp	'GdiConvertBitmapV5'					GdiConvertBitmapV5					gdi32		1499
imp	'GdiConvertBrush'					GdiConvertBrush						gdi32		1500
imp	'GdiConvertDC'						GdiConvertDC						gdi32		1501
imp	'GdiConvertEnhMetaFile'					GdiConvertEnhMetaFile					gdi32		1502
imp	'GdiConvertFont'					GdiConvertFont						gdi32		1503
imp	'GdiConvertMetaFilePict'				GdiConvertMetaFilePict					gdi32		1504
imp	'GdiConvertPalette'					GdiConvertPalette					gdi32		1505
imp	'GdiConvertRegion'					GdiConvertRegion					gdi32		1506
imp	'GdiConvertToDevmode'					GdiConvertToDevmodeW					gdi32		1507
imp	'GdiCreateLocalEnhMetaFile'				GdiCreateLocalEnhMetaFile				gdi32		1508
imp	'GdiCreateLocalMetaFilePict'				GdiCreateLocalMetaFilePict				gdi32		1509
imp	'GdiCurrentProcessSplWow64'				GdiCurrentProcessSplWow64				gdi32		1510
imp	'GdiDeleteLocalDC'					GdiDeleteLocalDC					gdi32		1511
imp	'GdiDeleteSpoolFileHandle'				GdiDeleteSpoolFileHandle				gdi32		1512
imp	'GdiDescribePixelFormat'				GdiDescribePixelFormat					gdi32		1513
imp	'GdiDllInitialize'					GdiDllInitialize					gdi32		1514
imp	'GdiDrawStream'						GdiDrawStream						gdi32		1515
imp	'GdiEndDocEMF'						GdiEndDocEMF						gdi32		1516
imp	'GdiEndPageEMF'						GdiEndPageEMF						gdi32		1517
imp	'GdiEntry1'						GdiEntry1						gdi32		1518
imp	'GdiEntry10'						GdiEntry10						gdi32		1519
imp	'GdiEntry11'						GdiEntry11						gdi32		1520
imp	'GdiEntry12'						GdiEntry12						gdi32		1521
imp	'GdiEntry13'						GdiEntry13						gdi32		1522
imp	'GdiEntry14'						GdiEntry14						gdi32		1523
imp	'GdiEntry15'						GdiEntry15						gdi32		1524
imp	'GdiEntry16'						GdiEntry16						gdi32		1525
imp	'GdiEntry2'						GdiEntry2						gdi32		1526
imp	'GdiEntry3'						GdiEntry3						gdi32		1527
imp	'GdiEntry4'						GdiEntry4						gdi32		1528
imp	'GdiEntry5'						GdiEntry5						gdi32		1529
imp	'GdiEntry6'						GdiEntry6						gdi32		1530
imp	'GdiEntry7'						GdiEntry7						gdi32		1531
imp	'GdiEntry8'						GdiEntry8						gdi32		1532
imp	'GdiEntry9'						GdiEntry9						gdi32		1533
imp	'GdiFixUpHandle'					GdiFixUpHandle						gdi32		1534
imp	'GdiFlush'						GdiFlush						gdi32		1535
imp	'GdiFullscreenControl'					GdiFullscreenControl					gdi32		1536
imp	'GdiGetBatchLimit'					GdiGetBatchLimit					gdi32		1537
imp	'GdiGetBitmapBitsSize'					GdiGetBitmapBitsSize					gdi32		1538
imp	'GdiGetCharDimensions'					GdiGetCharDimensions					gdi32		1539
imp	'GdiGetCodePage'					GdiGetCodePage						gdi32		1540
imp	'GdiGetDC'						GdiGetDC						gdi32		1541
imp	'GdiGetDevmodeForPage'					GdiGetDevmodeForPage					gdi32		1542
imp	'GdiGetEntry'						GdiGetEntry						gdi32		1543
imp	'GdiGetLocalBrush'					GdiGetLocalBrush					gdi32		1544
imp	'GdiGetLocalDC'						GdiGetLocalDC						gdi32		1545
imp	'GdiGetLocalFont'					GdiGetLocalFont						gdi32		1546
imp	'GdiGetPageCount'					GdiGetPageCount						gdi32		1547
imp	'GdiGetPageHandle'					GdiGetPageHandle					gdi32		1548
imp	'GdiGetSpoolFileHandle'					GdiGetSpoolFileHandle					gdi32		1549
imp	'GdiGetSpoolMessage'					GdiGetSpoolMessage					gdi32		1550
imp	'GdiGetVariationStoreDelta'				GdiGetVariationStoreDelta				gdi32		1551
imp	'GdiGradientFill'					GdiGradientFill						gdi32		1552
imp	'GdiInitSpool'						GdiInitSpool						gdi32		1553
imp	'GdiInitializeLanguagePack'				GdiInitializeLanguagePack				gdi32		1554
imp	'GdiIsMetaFileDC'					GdiIsMetaFileDC						gdi32		1555
imp	'GdiIsMetaPrintDC'					GdiIsMetaPrintDC					gdi32		1556
imp	'GdiIsPlayMetafileDC'					GdiIsPlayMetafileDC					gdi32		1557
imp	'GdiIsScreenDC'						GdiIsScreenDC						gdi32		1558
imp	'GdiIsTrackingEnabled'					GdiIsTrackingEnabled					gdi32		1559
imp	'GdiIsUMPDSandboxingEnabled'				GdiIsUMPDSandboxingEnabled				gdi32		1560
imp	'GdiLoadType1Fonts'					GdiLoadType1Fonts					gdi32		1561
imp	'GdiPlayDCScript'					GdiPlayDCScript						gdi32		1562
imp	'GdiPlayEMF'						GdiPlayEMF						gdi32		1563
imp	'GdiPlayJournal'					GdiPlayJournal						gdi32		1564
imp	'GdiPlayPageEMF'					GdiPlayPageEMF						gdi32		1565
imp	'GdiPlayPrivatePageEMF'					GdiPlayPrivatePageEMF					gdi32		1566
imp	'GdiPlayScript'						GdiPlayScript						gdi32		1567
imp	'GdiPrinterThunk'					GdiPrinterThunk						gdi32		1568
imp	'GdiProcessSetup'					GdiProcessSetup						gdi32		1569
imp	'GdiQueryFonts'						GdiQueryFonts						gdi32		1570
imp	'GdiQueryTable'						GdiQueryTable						gdi32		1571
imp	'GdiRealizationInfo'					GdiRealizationInfo					gdi32		1572
imp	'GdiReleaseDC'						GdiReleaseDC						gdi32		1573
imp	'GdiReleaseLocalDC'					GdiReleaseLocalDC					gdi32		1574
imp	'GdiResetDCEMF'						GdiResetDCEMF						gdi32		1575
imp	'GdiSetAttrs'						GdiSetAttrs						gdi32		1576
imp	'GdiSetBatchLimit'					GdiSetBatchLimit					gdi32		1577
imp	'GdiSetLastError'					GdiSetLastError						gdi32		1578
imp	'GdiSetPixelFormat'					GdiSetPixelFormat					gdi32		1579
imp	'GdiSetServerAttr'					GdiSetServerAttr					gdi32		1580
imp	'GdiStartDocEMF'					GdiStartDocEMF						gdi32		1581
imp	'GdiStartPageEMF'					GdiStartPageEMF						gdi32		1582
imp	'GdiSupportsFontChangeEvent'				GdiSupportsFontChangeEvent				gdi32		1583
imp	'GdiSwapBuffers'					GdiSwapBuffers						gdi32		1584
imp	'GdiTrackHCreate'					GdiTrackHCreate						gdi32		1585
imp	'GdiTrackHDelete'					GdiTrackHDelete						gdi32		1586
imp	'GdiTransparentBlt'					GdiTransparentBlt					gdi32		1587
imp	'GdiValidateHandle'					GdiValidateHandle					gdi32		1588
imp	'GenerateConsoleCtrlEvent'				GenerateConsoleCtrlEvent				kernel32	0	2	# KernelBase
imp	'GenerateGPNotificationInternal'			GenerateGPNotificationInternal				KernelBase	411
imp	'GetACP'						GetACP							kernel32	0		# KernelBase
imp	'GetAcceptExSockaddrs'					GetAcceptExSockaddrs					MsWSock		4	8
imp	'GetAcceptLanguagesA'					GetAcceptLanguagesA					KernelBase	413
imp	'GetAcceptLanguages'					GetAcceptLanguagesW					KernelBase	414
imp	'GetAccessPermissionsForObjectA'			GetAccessPermissionsForObjectA				advapi32	1311
imp	'GetAccessPermissionsForObject'				GetAccessPermissionsForObjectW				advapi32	1312
imp	'GetAce'						GetAce							advapi32	0		# KernelBase
imp	'GetAclInformation'					GetAclInformation					advapi32	0		# KernelBase
imp	'GetActiveProcessorCount'				GetActiveProcessorCount					kernel32	440
imp	'GetActiveProcessorGroupCount'				GetActiveProcessorGroupCount				kernel32	441
imp	'GetActiveWindow'					GetActiveWindow						user32		1790
imp	'GetAddrInfoExA'					GetAddrInfoExA						ws2_32		28
imp	'GetAddrInfoExCancel'					GetAddrInfoExCancel					ws2_32		29
imp	'GetAddrInfoExOverlappedResult'				GetAddrInfoExOverlappedResult				ws2_32		30
imp	'GetAddrInfoEx'						GetAddrInfoExW						ws2_32		31
imp	'GetAddrInfo'						GetAddrInfoW						ws2_32		32
imp	'GetAdjustObjectAttributesForPrivateNamespaceRoutine'	GetAdjustObjectAttributesForPrivateNamespaceRoutine	KernelBase	417
imp	'GetAltTabInfoA'					GetAltTabInfoA						user32		1792
imp	'GetAltTabInfo'						GetAltTabInfoW						user32		1793
imp	'GetAlternatePackageRoots'				GetAlternatePackageRoots				KernelBase	418
imp	'GetAncestor'						GetAncestor						user32		1794
imp	'GetAppCompatFlags'					GetAppCompatFlags					user32		1795
imp	'GetAppCompatFlags2'					GetAppCompatFlags2					user32		1796
imp	'GetAppContainerAce'					GetAppContainerAce					KernelBase	419
imp	'GetAppContainerNamedObjectPath'			GetAppContainerNamedObjectPath				kernel32	0		# KernelBase
imp	'GetAppDataFolder'					GetAppDataFolder					KernelBase	421
imp	'GetAppModelVersion'					GetAppModelVersion					KernelBase	422
imp	'GetApplicationRecoveryCallback'			GetApplicationRecoveryCallback				kernel32	0		# KernelBase
imp	'GetApplicationRecoveryCallbackWorker'			GetApplicationRecoveryCallbackWorker			kernel32	445
imp	'GetApplicationRestartSettings'				GetApplicationRestartSettings				kernel32	0		# KernelBase
imp	'GetApplicationRestartSettingsWorker'			GetApplicationRestartSettingsWorker			kernel32	447
imp	'GetApplicationUserModelId'				GetApplicationUserModelId				kernel32	0		# KernelBase
imp	'GetApplicationUserModelIdFromToken'			GetApplicationUserModelIdFromToken			KernelBase	426
imp	'GetAppliedGPOListInternalA'				GetAppliedGPOListInternalA				KernelBase	427
imp	'GetAppliedGPOListInternal'				GetAppliedGPOListInternalW				KernelBase	428
imp	'GetArcDirection'					GetArcDirection						gdi32		1589
imp	'GetAspectRatioFilterEx'				GetAspectRatioFilterEx					gdi32		1590
imp	'GetAsyncKeyState'					GetAsyncKeyState					user32		1797
imp	'GetAtomNameA'						GetAtomNameA						kernel32	449
imp	'GetAtomName'						GetAtomNameW						kernel32	450
imp	'GetAuditedPermissionsFromAclA'				GetAuditedPermissionsFromAclA				advapi32	1315
imp	'GetAuditedPermissionsFromAcl'				GetAuditedPermissionsFromAclW				advapi32	1316
imp	'GetAutoRotationState'					GetAutoRotationState					user32		1798
imp	'GetAwarenessFromDpiAwarenessContext'			GetAwarenessFromDpiAwarenessContext			user32		1799
imp	'GetBinaryTypeA'					GetBinaryTypeA						kernel32	452
imp	'GetBinaryType'						GetBinaryTypeW						kernel32	453
imp	'GetBitmapAttributes'					GetBitmapAttributes					gdi32		1591
imp	'GetBitmapBits'						GetBitmapBits						gdi32		1592
imp	'GetBitmapDimensionEx'					GetBitmapDimensionEx					gdi32		1593
imp	'GetBitmapDpiScaleValue'				GetBitmapDpiScaleValue					gdi32		1594
imp	'GetBkColor'						GetBkColor						gdi32		1595
imp	'GetBkMode'						GetBkMode						gdi32		1596
imp	'GetBoundsRect'						GetBoundsRect						gdi32		1597
imp	'GetBrushAttributes'					GetBrushAttributes					gdi32		1598
imp	'GetBrushOrgEx'						GetBrushOrgEx						gdi32		1599
imp	'GetCIMSSM'						GetCIMSSM						user32		1800
imp	'GetCOPPCompatibleOPMInformation'			GetCOPPCompatibleOPMInformation				gdi32		1600
imp	'GetCPFileNameFromRegistry'				GetCPFileNameFromRegistry				KernelBase	429
imp	'GetCPHashNode'						GetCPHashNode						KernelBase	430
imp	'GetCPInfo'						GetCPInfo						kernel32	0		# KernelBase
imp	'GetCPInfoExA'						GetCPInfoExA						kernel32	455
imp	'GetCPInfoEx'						GetCPInfoExW						kernel32	0		# KernelBase
imp	'GetCachedSigningLevel'					GetCachedSigningLevel					KernelBase	433
imp	'GetCalendar'						GetCalendar						KernelBase	434
imp	'GetCalendarDateFormat'					GetCalendarDateFormat					kernel32	458
imp	'GetCalendarDateFormatEx'				GetCalendarDateFormatEx					kernel32	459
imp	'GetCalendarDaysInMonth'				GetCalendarDaysInMonth					kernel32	460
imp	'GetCalendarDifferenceInDays'				GetCalendarDifferenceInDays				kernel32	461
imp	'GetCalendarInfoA'					GetCalendarInfoA					kernel32	462
imp	'GetCalendarInfoEx'					GetCalendarInfoEx					kernel32	0		# KernelBase
imp	'GetCalendarInfo'					GetCalendarInfoW					kernel32	0		# KernelBase
imp	'GetCalendarMonthsInYear'				GetCalendarMonthsInYear					kernel32	465
imp	'GetCalendarSupportedDateRange'				GetCalendarSupportedDateRange				kernel32	466
imp	'GetCalendarWeekNumber'					GetCalendarWeekNumber					kernel32	467
imp	'GetCapture'						GetCapture						user32		1801
imp	'GetCaretBlinkTime'					GetCaretBlinkTime					user32		1802
imp	'GetCaretPos'						GetCaretPos						user32		1803
imp	'GetCertificate'					GetCertificate						gdi32		1601
imp	'GetCertificateByHandle'				GetCertificateByHandle					gdi32		1602
imp	'GetCertificateSize'					GetCertificateSize					gdi32		1603
imp	'GetCertificateSizeByHandle'				GetCertificateSizeByHandle				gdi32		1604
imp	'GetCharABCWidthsA'					GetCharABCWidthsA					gdi32		1605
imp	'GetCharABCWidthsFloatA'				GetCharABCWidthsFloatA					gdi32		1606
imp	'GetCharABCWidthsFloatI'				GetCharABCWidthsFloatI					gdi32		1607
imp	'GetCharABCWidthsFloat'					GetCharABCWidthsFloatW					gdi32		1608
imp	'GetCharABCWidthsI'					GetCharABCWidthsI					gdi32		1609
imp	'GetCharABCWidths'					GetCharABCWidthsW					gdi32		1610
imp	'GetCharWidth32A'					GetCharWidth32A						gdi32		1611
imp	'GetCharWidth32W'					GetCharWidth32W						gdi32		1612
imp	'GetCharWidthA'						GetCharWidthA						gdi32		1613
imp	'GetCharWidthFloatA'					GetCharWidthFloatA					gdi32		1614
imp	'GetCharWidthFloat'					GetCharWidthFloatW					gdi32		1615
imp	'GetCharWidthI'						GetCharWidthI						gdi32		1616
imp	'GetCharWidthInfo'					GetCharWidthInfo					gdi32		1617
imp	'GetCharWidth'						GetCharWidthW						gdi32		1618
imp	'GetCharacterPlacementA'				GetCharacterPlacementA					gdi32		1619
imp	'GetCharacterPlacement'					GetCharacterPlacementW					gdi32		1620
imp	'GetClassInfoA'						GetClassInfoA						user32		1804
imp	'GetClassInfoExA'					GetClassInfoExA						user32		1805
imp	'GetClassInfoEx'					GetClassInfoExW						user32		1806
imp	'GetClassInfo'						GetClassInfoW						user32		1807
imp	'GetClassLongA'						GetClassLongA						user32		1808
imp	'GetClassLongPtrA'					GetClassLongPtrA					user32		1809
imp	'GetClassLongPtr'					GetClassLongPtrW					user32		1810
imp	'GetClassLong'						GetClassLongW						user32		1811
imp	'GetClassNameA'						GetClassNameA						user32		1812
imp	'GetClassName'						GetClassNameW						user32		1813
imp	'GetClassWord'						GetClassWord						user32		1814
imp	'GetClientRect'						GetClientRect						user32		1815	2
imp	'GetClipBox'						GetClipBox						gdi32		1621
imp	'GetClipCursor'						GetClipCursor						user32		1816
imp	'GetClipRgn'						GetClipRgn						gdi32		1622
imp	'GetClipboardAccessToken'				GetClipboardAccessToken					user32		1817
imp	'GetClipboardData'					GetClipboardData					user32		1818
imp	'GetClipboardFormatNameA'				GetClipboardFormatNameA					user32		1819
imp	'GetClipboardFormatName'				GetClipboardFormatNameW					user32		1820
imp	'GetClipboardOwner'					GetClipboardOwner					user32		1821
imp	'GetClipboardSequenceNumber'				GetClipboardSequenceNumber				user32		1822
imp	'GetClipboardViewer'					GetClipboardViewer					user32		1823
imp	'GetColorAdjustment'					GetColorAdjustment					gdi32		1623
imp	'GetColorSpace'						GetColorSpace						gdi32		1624
imp	'GetComPlusPackageInstallStatus'			GetComPlusPackageInstallStatus				kernel32	468
imp	'GetComboBoxInfo'					GetComboBoxInfo						user32		1824
imp	'GetCommConfig'						GetCommConfig						kernel32	0		# KernelBase
imp	'GetCommMask'						GetCommMask						kernel32	0		# KernelBase
imp	'GetCommModemStatus'					GetCommModemStatus					kernel32	0		# KernelBase
imp	'GetCommPorts'						GetCommPorts						KernelBase	440
imp	'GetCommProperties'					GetCommProperties					kernel32	0		# KernelBase
imp	'GetCommState'						GetCommState						kernel32	0		# KernelBase
imp	'GetCommTimeouts'					GetCommTimeouts						kernel32	0		# KernelBase
imp	'GetCommandLine'					GetCommandLineW						kernel32	0	0	# KernelBase
imp	'GetCommandLineA'					GetCommandLineA						kernel32	0	0	# KernelBase
imp	'GetCompressedFileSize'					GetCompressedFileSizeW					kernel32	0	2	# KernelBase
imp	'GetCompressedFileSizeA'				GetCompressedFileSizeA					kernel32	0	2	# KernelBase
imp	'GetCompressedFileSizeTransactedA'			GetCompressedFileSizeTransactedA			kernel32	478
imp	'GetCompressedFileSizeTransacted'			GetCompressedFileSizeTransactedW			kernel32	479
imp	'GetComputerNameA'					GetComputerNameA					kernel32	481
imp	'GetComputerNameExA'					GetComputerNameExA					kernel32	0	3	# KernelBase
imp	'GetComputerNameEx'					GetComputerNameExW					kernel32	0	3	# KernelBase
imp	'GetComputerName'					GetComputerNameW					kernel32	484
imp	'GetConsoleAliasA'					GetConsoleAliasA					kernel32	0		# KernelBase
imp	'GetConsoleAliasExesA'					GetConsoleAliasExesA					kernel32	0		# KernelBase
imp	'GetConsoleAliasExesLengthA'				GetConsoleAliasExesLengthA				kernel32	0		# KernelBase
imp	'GetConsoleAliasExesLength'				GetConsoleAliasExesLengthW				kernel32	0		# KernelBase
imp	'GetConsoleAliasExes'					GetConsoleAliasExesW					kernel32	0		# KernelBase
imp	'GetConsoleAlias'					GetConsoleAliasW					kernel32	0		# KernelBase
imp	'GetConsoleAliasesA'					GetConsoleAliasesA					kernel32	0		# KernelBase
imp	'GetConsoleAliasesLengthA'				GetConsoleAliasesLengthA				kernel32	0		# KernelBase
imp	'GetConsoleAliasesLength'				GetConsoleAliasesLengthW				kernel32	0		# KernelBase
imp	'GetConsoleAliases'					GetConsoleAliasesW					kernel32	0		# KernelBase
imp	'GetConsoleCP'						GetConsoleCP						kernel32	0	0	# KernelBase
imp	'GetConsoleCharType'					GetConsoleCharType					kernel32	496
imp	'GetConsoleCommandHistoryA'				GetConsoleCommandHistoryA				KernelBase	461
imp	'GetConsoleCommandHistoryLengthA'			GetConsoleCommandHistoryLengthA				KernelBase	462
imp	'GetConsoleCommandHistoryLength'			GetConsoleCommandHistoryLengthW				KernelBase	463
imp	'GetConsoleCommandHistory'				GetConsoleCommandHistoryW				KernelBase	464
imp	'GetConsoleCursorInfo'					GetConsoleCursorInfo					kernel32	0	2	# KernelBase
imp	'GetConsoleCursorMode'					GetConsoleCursorMode					kernel32	502
imp	'GetConsoleDisplayMode'					GetConsoleDisplayMode					kernel32	0		# KernelBase
imp	'GetConsoleFontInfo'					GetConsoleFontInfo					kernel32	504
imp	'GetConsoleFontSize'					GetConsoleFontSize					kernel32	0		# KernelBase
imp	'GetConsoleHardwareState'				GetConsoleHardwareState					kernel32	506
imp	'GetConsoleHistoryInfo'					GetConsoleHistoryInfo					kernel32	0		# KernelBase
imp	'GetConsoleInputExeNameA'				GetConsoleInputExeNameA					KernelBase	469
imp	'GetConsoleInputExeName'				GetConsoleInputExeNameW					KernelBase	470
imp	'GetConsoleInputWaitHandle'				GetConsoleInputWaitHandle				kernel32	510
imp	'GetConsoleKeyboardLayoutNameA'				GetConsoleKeyboardLayoutNameA				kernel32	511
imp	'GetConsoleKeyboardLayoutName'				GetConsoleKeyboardLayoutNameW				kernel32	512
imp	'GetConsoleMode'					GetConsoleMode						kernel32	0	2	# KernelBase
imp	'GetConsoleNlsMode'					GetConsoleNlsMode					kernel32	514
imp	'GetConsoleOriginalTitleA'				GetConsoleOriginalTitleA				kernel32	0		# KernelBase
imp	'GetConsoleOriginalTitle'				GetConsoleOriginalTitleW				kernel32	0		# KernelBase
imp	'GetConsoleOutputCP'					GetConsoleOutputCP					kernel32	0	0	# KernelBase
imp	'GetConsoleProcessList'					GetConsoleProcessList					kernel32	0		# KernelBase
imp	'GetConsoleScreenBufferInfo'				GetConsoleScreenBufferInfo				kernel32	0	2	# KernelBase
imp	'GetConsoleScreenBufferInfoEx'				GetConsoleScreenBufferInfoEx				kernel32	0	2	# KernelBase
imp	'GetConsoleSelectionInfo'				GetConsoleSelectionInfo					kernel32	0	1	# KernelBase
imp	'GetConsoleTitleA'					GetConsoleTitleA					kernel32	0	2	# KernelBase
imp	'GetConsoleTitle'					GetConsoleTitleW					kernel32	0	2	# KernelBase
imp	'GetConsoleWindow'					GetConsoleWindow					kernel32	0	0	# KernelBase
imp	'GetCurrencyFormatA'					GetCurrencyFormatA					kernel32	525
imp	'GetCurrencyFormatEx'					GetCurrencyFormatEx					kernel32	0		# KernelBase
imp	'GetCurrencyFormat'					GetCurrencyFormatW					kernel32	0		# KernelBase
imp	'GetCurrentActCtx'					GetCurrentActCtx					kernel32	0		# KernelBase
imp	'GetCurrentActCtxWorker'				GetCurrentActCtxWorker					kernel32	529
imp	'GetCurrentApplicationUserModelId'			GetCurrentApplicationUserModelId			kernel32	0		# KernelBase
imp	'GetCurrentConsoleFont'					GetCurrentConsoleFont					kernel32	0		# KernelBase
imp	'GetCurrentConsoleFontEx'				GetCurrentConsoleFontEx					kernel32	0		# KernelBase
imp	'GetCurrentDirectory'					GetCurrentDirectoryW					kernel32	0	2	# KernelBase
imp	'GetCurrentDirectoryA'					GetCurrentDirectoryA					kernel32	0	2	# KernelBase
imp	'GetCurrentDpiInfo'					GetCurrentDpiInfo					gdi32		1625
imp	'GetCurrentHwProfileA'					GetCurrentHwProfileA					advapi32	1317
imp	'GetCurrentHwProfile'					GetCurrentHwProfileW					advapi32	1318
imp	'GetCurrentInputMessageSource'				GetCurrentInputMessageSource				user32		1825
imp	'GetCurrentObject'					GetCurrentObject					gdi32		1626
imp	'GetCurrentPackageApplicationContext'			GetCurrentPackageApplicationContext			KernelBase	490
imp	'GetCurrentPackageApplicationResourcesContext'		GetCurrentPackageApplicationResourcesContext		KernelBase	491
imp	'GetCurrentPackageContext'				GetCurrentPackageContext				KernelBase	492
imp	'GetCurrentPackageFamilyName'				GetCurrentPackageFamilyName				kernel32	0		# KernelBase
imp	'GetCurrentPackageFullName'				GetCurrentPackageFullName				kernel32	0		# KernelBase
imp	'GetCurrentPackageId'					GetCurrentPackageId					kernel32	0		# KernelBase
imp	'GetCurrentPackageInfo'					GetCurrentPackageInfo					kernel32	0		# KernelBase
imp	'GetCurrentPackagePath'					GetCurrentPackagePath					kernel32	0		# KernelBase
imp	'GetCurrentPackageResourcesContext'			GetCurrentPackageResourcesContext			KernelBase	498
imp	'GetCurrentPackageSecurityContext'			GetCurrentPackageSecurityContext			KernelBase	499
imp	'GetCurrentPositionEx'					GetCurrentPositionEx					gdi32		1627
imp	'GetCurrentProcess'					GetCurrentProcess					kernel32	0	0	# KernelBase
imp	'GetCurrentProcessExplicitAppUserModelID'		GetCurrentProcessExplicitAppUserModelID			shell32		305
imp	'GetCurrentProcessId'					GetCurrentProcessId					kernel32	0	0	# KernelBase
imp	'GetCurrentTargetPlatformContext'			GetCurrentTargetPlatformContext				KernelBase	504
imp	'GetCurrentThread'					GetCurrentThread					kernel32	0	0	# KernelBase
imp	'GetCurrentThreadId'					GetCurrentThreadId					kernel32	0	0	# KernelBase
imp	'GetCurrentThreadStackLimits'				GetCurrentThreadStackLimits				kernel32	0		# KernelBase
imp	'GetCurrentUmsThread'					GetCurrentUmsThread					kernel32	547
imp	'GetCursor'						GetCursor						user32		1826	0
imp	'GetCursorFrameInfo'					GetCursorFrameInfo					user32		1827
imp	'GetCursorInfo'						GetCursorInfo						user32		1828
imp	'GetCursorPos'						GetCursorPos						user32		1829	1
imp	'GetDC'							GetDC							user32		1830
imp	'GetDCBrushColor'					GetDCBrushColor						gdi32		1628
imp	'GetDCDpiScaleValue'					GetDCDpiScaleValue					gdi32		1629
imp	'GetDCEx'						GetDCEx							user32		1831
imp	'GetDCOrgEx'						GetDCOrgEx						gdi32		1630
imp	'GetDCPenColor'						GetDCPenColor						gdi32		1631
imp	'GetDIBColorTable'					GetDIBColorTable					gdi32		1632
imp	'GetDIBits'						GetDIBits						gdi32		1633
imp	'GetDateFormatA'					GetDateFormatA						kernel32	0		# KernelBase
imp	'GetDateFormatAWorker'					GetDateFormatAWorker					kernel32	549
imp	'GetDateFormatEx'					GetDateFormatEx						kernel32	0		# KernelBase
imp	'GetDateFormat'						GetDateFormatW						kernel32	0		# KernelBase
imp	'GetDateFormatWWorker'					GetDateFormatWWorker					kernel32	552
imp	'GetDefaultCommConfigA'					GetDefaultCommConfigA					kernel32	553
imp	'GetDefaultCommConfig'					GetDefaultCommConfigW					kernel32	554
imp	'GetDesktopID'						GetDesktopID						user32		1832
imp	'GetDesktopWindow'					GetDesktopWindow					user32		1833	0
imp	'GetDeviceCaps'						GetDeviceCaps						gdi32		1634
imp	'GetDeviceDriverBaseNameA'				GetDeviceDriverBaseNameA				KernelBase	511
imp	'GetDeviceDriverBaseName'				GetDeviceDriverBaseNameW				KernelBase	512
imp	'GetDeviceDriverFileNameA'				GetDeviceDriverFileNameA				KernelBase	513
imp	'GetDeviceDriverFileName'				GetDeviceDriverFileNameW				KernelBase	514
imp	'GetDeviceGammaRamp'					GetDeviceGammaRamp					gdi32		1635
imp	'GetDevicePowerState'					GetDevicePowerState					kernel32	555
imp	'GetDialogBaseUnits'					GetDialogBaseUnits					user32		1834
imp	'GetDialogControlDpiChangeBehavior'			GetDialogControlDpiChangeBehavior			user32		1835
imp	'GetDialogDpiChangeBehavior'				GetDialogDpiChangeBehavior				user32		1836
imp	'GetDiskFreeSpaceA'					GetDiskFreeSpaceA					kernel32	0		# KernelBase
imp	'GetDiskFreeSpaceExA'					GetDiskFreeSpaceExA					kernel32	0		# KernelBase
imp	'GetDiskFreeSpaceEx'					GetDiskFreeSpaceExW					kernel32	0		# KernelBase
imp	'GetDiskFreeSpace'					GetDiskFreeSpaceW					kernel32	0		# KernelBase
imp	'GetDisplayAutoRotationPreferences'			GetDisplayAutoRotationPreferences			user32		1837
imp	'GetDisplayConfigBufferSizes'				GetDisplayConfigBufferSizes				user32		1838
imp	'GetDlgCtrlID'						GetDlgCtrlID						user32		1839
imp	'GetDlgItem'						GetDlgItem						user32		1840
imp	'GetDlgItemInt'						GetDlgItemInt						user32		1841
imp	'GetDlgItemTextA'					GetDlgItemTextA						user32		1842
imp	'GetDlgItemText'					GetDlgItemTextW						user32		1843
imp	'GetDllDirectoryA'					GetDllDirectoryA					kernel32	560
imp	'GetDllDirectory'					GetDllDirectoryW					kernel32	561
imp	'GetDoubleClickTime'					GetDoubleClickTime					user32		1844
imp	'GetDpiForMonitorInternal'				GetDpiForMonitorInternal				user32		1845
imp	'GetDpiForSystem'					GetDpiForSystem						user32		1846
imp	'GetDpiForWindow'					GetDpiForWindow						user32		1847
imp	'GetDpiFromDpiAwarenessContext'				GetDpiFromDpiAwarenessContext				user32		1848
imp	'GetDriveTypeA'						GetDriveTypeA						kernel32	0		# KernelBase
imp	'GetDriveType'						GetDriveTypeW						kernel32	0		# KernelBase
imp	'GetDurationFormat'					GetDurationFormat					kernel32	564
imp	'GetDurationFormatEx'					GetDurationFormatEx					kernel32	0		# KernelBase
imp	'GetDynamicTimeZoneInformation'				GetDynamicTimeZoneInformation				kernel32	0		# KernelBase
imp	'GetDynamicTimeZoneInformationEffectiveYears'		GetDynamicTimeZoneInformationEffectiveYears		KernelBase	523
imp	'GetETM'						GetETM							gdi32		1636
imp	'GetEUDCTimeStamp'					GetEUDCTimeStamp					gdi32		1637
imp	'GetEUDCTimeStampEx'					GetEUDCTimeStampExW					gdi32		1638
imp	'GetEffectivePackageStatusForUser'			GetEffectivePackageStatusForUser			KernelBase	524
imp	'GetEffectivePackageStatusForUserSid'			GetEffectivePackageStatusForUserSid			KernelBase	525
imp	'GetEffectiveRightsFromAclA'				GetEffectiveRightsFromAclA				advapi32	1320
imp	'GetEffectiveRightsFromAcl'				GetEffectiveRightsFromAclW				advapi32	1321
imp	'GetEightBitStringToUnicodeSizeRoutine'			GetEightBitStringToUnicodeSizeRoutine			KernelBase	526
imp	'GetEightBitStringToUnicodeStringRoutine'		GetEightBitStringToUnicodeStringRoutine			KernelBase	527
imp	'GetEnabledXStateFeatures'				GetEnabledXStateFeatures				kernel32	0		# KernelBase
imp	'GetEncryptedFileMetadata'				GetEncryptedFileMetadata				advapi32	1322
imp	'GetEncryptedFileVersionExt'				GetEncryptedFileVersionExt				kernel32	568
imp	'GetEnhMetaFileA'					GetEnhMetaFileA						gdi32		1639
imp	'GetEnhMetaFileBits'					GetEnhMetaFileBits					gdi32		1640
imp	'GetEnhMetaFileDescriptionA'				GetEnhMetaFileDescriptionA				gdi32		1641
imp	'GetEnhMetaFileDescription'				GetEnhMetaFileDescriptionW				gdi32		1642
imp	'GetEnhMetaFileHeader'					GetEnhMetaFileHeader					gdi32		1643
imp	'GetEnhMetaFilePaletteEntries'				GetEnhMetaFilePaletteEntries				gdi32		1644
imp	'GetEnhMetaFilePixelFormat'				GetEnhMetaFilePixelFormat				gdi32		1645
imp	'GetEnhMetaFile'					GetEnhMetaFileW						gdi32		1646
imp	'GetEnvironmentStrings'					GetEnvironmentStringsW					kernel32	0	1	# KernelBase
imp	'GetEnvironmentStringsA'				GetEnvironmentStringsA					kernel32	0	1	# KernelBase
imp	'GetEnvironmentVariable'				GetEnvironmentVariableW					kernel32	0	3	# KernelBase
imp	'GetEnvironmentVariableA'				GetEnvironmentVariableA					kernel32	0	3	# KernelBase
imp	'GetEraNameCountedString'				GetEraNameCountedString					KernelBase	534
imp	'GetErrorMode'						GetErrorMode						kernel32	0		# KernelBase
imp	'GetEventLogInformation'				GetEventLogInformation					advapi32	1323
imp	'GetExitCodeProcess'					GetExitCodeProcess					kernel32	0	2	# KernelBase
imp	'GetExitCodeThread'					GetExitCodeThread					kernel32	0	2	# KernelBase
imp	'GetExpandedNameA'					GetExpandedNameA					kernel32	578
imp	'GetExpandedName'					GetExpandedNameW					kernel32	579
imp	'GetExplicitEntriesFromAclA'				GetExplicitEntriesFromAclA				advapi32	1324
imp	'GetExplicitEntriesFromAcl'				GetExplicitEntriesFromAclW				advapi32	1325
imp	'GetExtensionApplicationUserModelId'			GetExtensionApplicationUserModelId			KernelBase	538
imp	'GetExtensionProgIds'					GetExtensionProgIds					KernelBase	539
imp	'GetExtensionProperty'					GetExtensionProperty					KernelBase	540
imp	'GetExtensionProperty2'					GetExtensionProperty2					KernelBase	541
imp	'GetFallbackDisplayName'				GetFallbackDisplayName					KernelBase	542
imp	'GetFileAttributesA'					GetFileAttributesA					kernel32	0	1	# KernelBase
imp	'GetFileAttributes'					GetFileAttributesW					kernel32	0	1	# KernelBase
imp	'GetFileAttributesExA'					GetFileAttributesExA					kernel32	0	3	# KernelBase
imp	'GetFileAttributesEx'					GetFileAttributesExW					kernel32	0	3	# KernelBase
imp	'GetFileAttributesTransactedA'				GetFileAttributesTransactedA				kernel32	583
imp	'GetFileAttributesTransacted'				GetFileAttributesTransactedW				kernel32	584
imp	'GetFileBandwidthReservation'				GetFileBandwidthReservation				kernel32	586
imp	'GetFileInformationByHandle'				GetFileInformationByHandle				kernel32	0	2	# KernelBase
imp	'GetFileInformationByHandleEx'				GetFileInformationByHandleEx				kernel32	0	4	# KernelBase
imp	'GetFileMUIInfo'					GetFileMUIInfo						kernel32	0		# KernelBase
imp	'GetFileMUIPath'					GetFileMUIPath						kernel32	0		# KernelBase
imp	'GetFileNameFromBrowse'					GetFileNameFromBrowse					shell32		63
imp	'GetFileSecurity'					GetFileSecurityW					advapi32	0	5	# KernelBase
imp	'GetFileSecurityA'					GetFileSecurityA					advapi32	1326	5
imp	'GetFileSize'						GetFileSize						kernel32	0	2	# KernelBase
imp	'GetFileSizeEx'						GetFileSizeEx						kernel32	0	2	# KernelBase
imp	'GetFileTime'						GetFileTime						kernel32	0	4	# KernelBase
imp	'GetFileTitleA'						GetFileTitleA						comdlg32	111	3
imp	'GetFileTitle'						GetFileTitleW						comdlg32	112	3
imp	'GetFileType'						GetFileType						kernel32	0	1	# KernelBase
imp	'GetFileVersionInfoA'					GetFileVersionInfoA					KernelBase	556
imp	'GetFileVersionInfoByHandle'				GetFileVersionInfoByHandle				KernelBase	557
imp	'GetFileVersionInfoExA'					GetFileVersionInfoExA					KernelBase	558
imp	'GetFileVersionInfoEx'					GetFileVersionInfoExW					KernelBase	559
imp	'GetFileVersionInfoSizeA'				GetFileVersionInfoSizeA					KernelBase	560
imp	'GetFileVersionInfoSizeExA'				GetFileVersionInfoSizeExA				KernelBase	561
imp	'GetFileVersionInfoSizeEx'				GetFileVersionInfoSizeExW				KernelBase	562
imp	'GetFileVersionInfoSize'				GetFileVersionInfoSizeW					KernelBase	563
imp	'GetFileVersionInfo'					GetFileVersionInfoW					KernelBase	564
imp	'GetFinalPathNameByHandleA'				GetFinalPathNameByHandleA				kernel32	0		# KernelBase
imp	'GetFinalPathNameByHandle'				GetFinalPathNameByHandleW				kernel32	0		# KernelBase
imp	'GetFirmwareEnvironmentVariableA'			GetFirmwareEnvironmentVariableA				kernel32	597
imp	'GetFirmwareEnvironmentVariableExA'			GetFirmwareEnvironmentVariableExA			kernel32	598
imp	'GetFirmwareEnvironmentVariableEx'			GetFirmwareEnvironmentVariableExW			kernel32	599
imp	'GetFirmwareEnvironmentVariable'			GetFirmwareEnvironmentVariableW				kernel32	600
imp	'GetFirmwareType'					GetFirmwareType						kernel32	601
imp	'GetFocus'						GetFocus						user32		1849
imp	'GetFontAssocStatus'					GetFontAssocStatus					gdi32		1647
imp	'GetFontData'						GetFontData						gdi32		1648
imp	'GetFontFileData'					GetFontFileData						gdi32		1649
imp	'GetFontFileInfo'					GetFontFileInfo						gdi32		1650
imp	'GetFontLanguageInfo'					GetFontLanguageInfo					gdi32		1651
imp	'GetFontRealizationInfo'				GetFontRealizationInfo					gdi32		1652
imp	'GetFontResourceInfo'					GetFontResourceInfoW					gdi32		1653
imp	'GetFontUnicodeRanges'					GetFontUnicodeRanges					gdi32		1654
imp	'GetForegroundWindow'					GetForegroundWindow					user32		1850
imp	'GetFullPathNameA'					GetFullPathNameA					kernel32	0		# KernelBase
imp	'GetFullPathNameTransactedA'				GetFullPathNameTransactedA				kernel32	603
imp	'GetFullPathNameTransacted'				GetFullPathNameTransactedW				kernel32	604
imp	'GetFullPathName'					GetFullPathNameW					kernel32	0		# KernelBase
imp	'GetGPOListInternalA'					GetGPOListInternalA					KernelBase	569
imp	'GetGPOListInternal'					GetGPOListInternalW					KernelBase	570
imp	'GetGUIThreadInfo'					GetGUIThreadInfo					user32		1851
imp	'GetGamingDeviceModelInformation'			GetGamingDeviceModelInformation				KernelBase	571
imp	'GetGeoInfoA'						GetGeoInfoA						kernel32	606
imp	'GetGeoInfoEx'						GetGeoInfoEx						KernelBase	572
imp	'GetGeoInfo'						GetGeoInfoW						kernel32	0		# KernelBase
imp	'GetGestureConfig'					GetGestureConfig					user32		1852
imp	'GetGestureExtraArgs'					GetGestureExtraArgs					user32		1853
imp	'GetGestureInfo'					GetGestureInfo						user32		1854
imp	'GetGlyphIndicesA'					GetGlyphIndicesA					gdi32		1655
imp	'GetGlyphIndices'					GetGlyphIndicesW					gdi32		1656
imp	'GetGlyphOutlineA'					GetGlyphOutlineA					gdi32		1658
imp	'GetGlyphOutline'					GetGlyphOutlineW					gdi32		1659
imp	'GetGlyphOutlineWow'					GetGlyphOutlineWow					gdi32		1660
imp	'GetGraphicsMode'					GetGraphicsMode						gdi32		1661
imp	'GetGuiResources'					GetGuiResources						user32		1855
imp	'GetHFONT'						GetHFONT						gdi32		1662
imp	'GetHandleInformation'					GetHandleInformation					kernel32	0	2	# KernelBase
imp	'GetHivePath'						GetHivePath						KernelBase	575
imp	'GetHostName'						GetHostNameW						ws2_32		33
imp	'GetICMProfileA'					GetICMProfileA						gdi32		1663
imp	'GetICMProfile'						GetICMProfileW						gdi32		1664
imp	'GetIconInfo'						GetIconInfo						user32		1856
imp	'GetIconInfoExA'					GetIconInfoExA						user32		1857
imp	'GetIconInfoEx'						GetIconInfoExW						user32		1858
imp	'GetInformationCodeAuthzLevel'				GetInformationCodeAuthzLevelW				advapi32	1328
imp	'GetInformationCodeAuthzPolicy'				GetInformationCodeAuthzPolicyW				advapi32	1329
imp	'GetInheritanceSourceA'					GetInheritanceSourceA					advapi32	1330
imp	'GetInheritanceSource'					GetInheritanceSourceW					advapi32	1331
imp	'GetInputDesktop'					GetInputDesktop						user32		1859
imp	'GetInputLocaleInfo'					GetInputLocaleInfo					user32		1860
imp	'GetInputState'						GetInputState						user32		1861
imp	'GetIntegratedDisplaySize'				GetIntegratedDisplaySize				KernelBase	576
imp	'GetInternalWindowPos'					GetInternalWindowPos					user32		1862
imp	'GetIsEdpEnabled'					GetIsEdpEnabled						KernelBase	577
imp	'GetKBCodePage'						GetKBCodePage						user32		1863
imp	'GetKernelObjectSecurity'				GetKernelObjectSecurity					advapi32	0		# KernelBase
imp	'GetKerningPairsA'					GetKerningPairsA					gdi32		1666
imp	'GetKerningPairs'					GetKerningPairsW					gdi32		1667
imp	'GetKeyNameTextA'					GetKeyNameTextA						user32		1864
imp	'GetKeyNameText'					GetKeyNameTextW						user32		1865
imp	'GetKeyState'						GetKeyState						user32		1866	1
imp	'GetKeyboardLayout'					GetKeyboardLayout					user32		1867
imp	'GetKeyboardLayoutList'					GetKeyboardLayoutList					user32		1868
imp	'GetKeyboardLayoutNameA'				GetKeyboardLayoutNameA					user32		1869
imp	'GetKeyboardLayoutName'					GetKeyboardLayoutNameW					user32		1870
imp	'GetKeyboardState'					GetKeyboardState					user32		1871
imp	'GetKeyboardType'					GetKeyboardType						user32		1872
imp	'GetLargePageMinimum'					GetLargePageMinimum					kernel32	0		# KernelBase
imp	'GetLargestConsoleWindowSize'				GetLargestConsoleWindowSize				kernel32	0	1	# KernelBase
imp	'GetLastActivePopup'					GetLastActivePopup					user32		1873
imp	'GetLastError'						GetLastError						kernel32	0	0	# KernelBase
imp	'GetLastInputInfo'					GetLastInputInfo					user32		1874
imp	'GetLayeredWindowAttributes'				GetLayeredWindowAttributes				user32		1875
imp	'GetLayout'						GetLayout						gdi32		1668
imp	'GetLengthSid'						GetLengthSid						advapi32	0		# KernelBase
imp	'GetListBoxInfo'					GetListBoxInfo						user32		1876
imp	'GetLocalManagedApplicationData'			GetLocalManagedApplicationData				advapi32	1334
imp	'GetLocalManagedApplications'				GetLocalManagedApplications				advapi32	1335
imp	'GetLocalTime'						GetLocalTime						kernel32	0		# KernelBase
imp	'GetLocaleInfoA'					GetLocaleInfoA						kernel32	0		# KernelBase
imp	'GetLocaleInfoEx'					GetLocaleInfoEx						kernel32	0		# KernelBase
imp	'GetLocaleInfoHelper'					GetLocaleInfoHelper					KernelBase	586
imp	'GetLocaleInfo'						GetLocaleInfoW						kernel32	0		# KernelBase
imp	'GetLogColorSpaceA'					GetLogColorSpaceA					gdi32		1669
imp	'GetLogColorSpace'					GetLogColorSpaceW					gdi32		1670
imp	'GetLogicalDriveStringsA'				GetLogicalDriveStringsA					kernel32	617
imp	'GetLogicalDriveStrings'				GetLogicalDriveStringsW					kernel32	0		# KernelBase
imp	'GetLogicalDrives'					GetLogicalDrives					kernel32	0	0	# KernelBase
imp	'GetLogicalProcessorInformation'			GetLogicalProcessorInformation				kernel32	0		# KernelBase
imp	'GetLogicalProcessorInformationEx'			GetLogicalProcessorInformationEx			kernel32	0		# KernelBase
imp	'GetLongPathNameA'					GetLongPathNameA					kernel32	0		# KernelBase
imp	'GetLongPathNameTransactedA'				GetLongPathNameTransactedA				kernel32	623
imp	'GetLongPathNameTransacted'				GetLongPathNameTransactedW				kernel32	624
imp	'GetLongPathName'					GetLongPathNameW					kernel32	0		# KernelBase
imp	'GetMagnificationDesktopColorEffect'			GetMagnificationDesktopColorEffect			user32		1877
imp	'GetMagnificationDesktopMagnification'			GetMagnificationDesktopMagnification			user32		1878
imp	'GetMagnificationDesktopSamplingMode'			GetMagnificationDesktopSamplingMode			user32		1879
imp	'GetMagnificationLensCtxInformation'			GetMagnificationLensCtxInformation			user32		1880
imp	'GetMailslotInfo'					GetMailslotInfo						kernel32	626
imp	'GetManagedApplicationCategories'			GetManagedApplicationCategories				advapi32	1336
imp	'GetManagedApplications'				GetManagedApplications					advapi32	1337
imp	'GetMapMode'						GetMapMode						gdi32		1671
imp	'GetMappedFileNameA'					GetMappedFileNameA					KernelBase	594
imp	'GetMappedFileName'					GetMappedFileNameW					KernelBase	595
imp	'GetMaximumProcessorCount'				GetMaximumProcessorCount				kernel32	627
imp	'GetMaximumProcessorGroupCount'				GetMaximumProcessorGroupCount				kernel32	628
imp	'GetMemoryErrorHandlingCapabilities'			GetMemoryErrorHandlingCapabilities			kernel32	0		# KernelBase
imp	'GetMenu'						GetMenu							user32		1881	1
imp	'GetMenuBarInfo'					GetMenuBarInfo						user32		1882
imp	'GetMenuCheckMarkDimensions'				GetMenuCheckMarkDimensions				user32		1883
imp	'GetMenuContextHelpId'					GetMenuContextHelpId					user32		1884
imp	'GetMenuDefaultItem'					GetMenuDefaultItem					user32		1885
imp	'GetMenuInfo'						GetMenuInfo						user32		1886
imp	'GetMenuItemCount'					GetMenuItemCount					user32		1887
imp	'GetMenuItemID'						GetMenuItemID						user32		1888
imp	'GetMenuItemInfoA'					GetMenuItemInfoA					user32		1889
imp	'GetMenuItemInfo'					GetMenuItemInfoW					user32		1890
imp	'GetMenuItemRect'					GetMenuItemRect						user32		1891
imp	'GetMenuState'						GetMenuState						user32		1892
imp	'GetMenuStringA'					GetMenuStringA						user32		1893
imp	'GetMenuString'						GetMenuStringW						user32		1894
imp	'GetMessage'						GetMessageW						user32		1899	4
imp	'GetMessageA'						GetMessageA						user32		1895	4
imp	'GetMessageExtraInfo'					GetMessageExtraInfo					user32		1896
imp	'GetMessagePos'						GetMessagePos						user32		1897
imp	'GetMessageTime'					GetMessageTime						user32		1898
imp	'GetMetaFileA'						GetMetaFileA						gdi32		1672
imp	'GetMetaFileBitsEx'					GetMetaFileBitsEx					gdi32		1673
imp	'GetMetaFile'						GetMetaFileW						gdi32		1674
imp	'GetMetaRgn'						GetMetaRgn						gdi32		1675
imp	'GetMiterLimit'						GetMiterLimit						gdi32		1676
imp	'GetModuleBaseNameA'					GetModuleBaseNameA					KernelBase	597
imp	'GetModuleBaseName'					GetModuleBaseNameW					KernelBase	598
imp	'GetModuleFileName'					GetModuleFileNameW					kernel32	0	3	# KernelBase
imp	'GetModuleFileNameA'					GetModuleFileNameA					kernel32	0	3	# KernelBase
imp	'GetModuleFileNameExA'					GetModuleFileNameExA					KernelBase	600
imp	'GetModuleFileNameEx'					GetModuleFileNameExW					KernelBase	601
imp	'GetModuleHandle'					GetModuleHandleA					kernel32	0	1	# KernelBase
imp	'GetModuleHandleW'					GetModuleHandleW					kernel32	0	1	# KernelBase
imp	'GetModuleHandleExA'					GetModuleHandleExA					kernel32	0	3	# KernelBase
imp	'GetModuleHandleEx'					GetModuleHandleExW					kernel32	0	3	# KernelBase
imp	'GetModuleInformation'					GetModuleInformation					KernelBase	607
imp	'GetMonitorInfoA'					GetMonitorInfoA						user32		1900
imp	'GetMonitorInfo'					GetMonitorInfoW						user32		1901
imp	'GetMouseMovePointsEx'					GetMouseMovePointsEx					user32		1902
imp	'GetMultipleTrusteeA'					GetMultipleTrusteeA					advapi32	1338
imp	'GetMultipleTrusteeOperationA'				GetMultipleTrusteeOperationA				advapi32	1339
imp	'GetMultipleTrusteeOperation'				GetMultipleTrusteeOperationW				advapi32	1340
imp	'GetMultipleTrustee'					GetMultipleTrusteeW					advapi32	1341
imp	'GetNLSVersion'						GetNLSVersion						kernel32	0		# KernelBase
imp	'GetNLSVersionEx'					GetNLSVersionEx						kernel32	0		# KernelBase
imp	'GetNameInfo'						GetNameInfoW						ws2_32		34
imp	'GetNamedLocaleHashNode'				GetNamedLocaleHashNode					KernelBase	610
imp	'GetNamedPipeAttribute'					GetNamedPipeAttribute					KernelBase	611
imp	'GetNamedPipeClientComputerNameA'			GetNamedPipeClientComputerNameA				kernel32	639
imp	'GetNamedPipeClientComputerName'			GetNamedPipeClientComputerNameW				kernel32	0		# KernelBase
imp	'GetNamedPipeClientProcessId'				GetNamedPipeClientProcessId				kernel32	641
imp	'GetNamedPipeClientSessionId'				GetNamedPipeClientSessionId				kernel32	642
imp	'GetNamedPipeHandleStateA'				GetNamedPipeHandleStateA				kernel32	643
imp	'GetNamedPipeHandleState'				GetNamedPipeHandleStateW				kernel32	0		# KernelBase
imp	'GetNamedPipeInfo'					GetNamedPipeInfo					kernel32	0		# KernelBase
imp	'GetNamedPipeServerProcessId'				GetNamedPipeServerProcessId				kernel32	646
imp	'GetNamedPipeServerSessionId'				GetNamedPipeServerSessionId				kernel32	647
imp	'GetNamedSecurityInfoA'					GetNamedSecurityInfoA					advapi32	1342
imp	'GetNamedSecurityInfoExA'				GetNamedSecurityInfoExA					advapi32	1343
imp	'GetNamedSecurityInfoEx'				GetNamedSecurityInfoExW					advapi32	1344
imp	'GetNamedSecurityInfo'					GetNamedSecurityInfoW					advapi32	1345
imp	'GetNativeSystemInfo'					GetNativeSystemInfo					kernel32	0		# KernelBase
imp	'GetNearestColor'					GetNearestColor						gdi32		1677
imp	'GetNearestPaletteIndex'				GetNearestPaletteIndex					gdi32		1678
imp	'GetNextDlgGroupItem'					GetNextDlgGroupItem					user32		1903
imp	'GetNextDlgTabItem'					GetNextDlgTabItem					user32		1904
imp	'GetNextFgPolicyRefreshInfoInternal'			GetNextFgPolicyRefreshInfoInternal			KernelBase	616
imp	'GetNextUmsListItem'					GetNextUmsListItem					kernel32	649
imp	'GetNextVDMCommand'					GetNextVDMCommand					kernel32	650
imp	'GetNumaAvailableMemoryNode'				GetNumaAvailableMemoryNode				kernel32	651
imp	'GetNumaAvailableMemoryNodeEx'				GetNumaAvailableMemoryNodeEx				kernel32	652
imp	'GetNumaHighestNodeNumber'				GetNumaHighestNodeNumber				kernel32	0		# KernelBase
imp	'GetNumaNodeNumberFromHandle'				GetNumaNodeNumberFromHandle				kernel32	654
imp	'GetNumaNodeProcessorMask'				GetNumaNodeProcessorMask				kernel32	655
imp	'GetNumaNodeProcessorMaskEx'				GetNumaNodeProcessorMaskEx				kernel32	0		# KernelBase
imp	'GetNumaProcessorNode'					GetNumaProcessorNode					kernel32	657
imp	'GetNumaProcessorNodeEx'				GetNumaProcessorNodeEx					kernel32	658
imp	'GetNumaProximityNode'					GetNumaProximityNode					kernel32	659
imp	'GetNumaProximityNodeEx'				GetNumaProximityNodeEx					kernel32	0		# KernelBase
imp	'GetNumberFormatA'					GetNumberFormatA					kernel32	661
imp	'GetNumberFormatEx'					GetNumberFormatEx					kernel32	0		# KernelBase
imp	'GetNumberFormat'					GetNumberFormatW					kernel32	0		# KernelBase
imp	'GetNumberOfConsoleFonts'				GetNumberOfConsoleFonts					kernel32	664
imp	'GetNumberOfConsoleInputEvents'				GetNumberOfConsoleInputEvents				kernel32	0	2	# KernelBase
imp	'GetNumberOfConsoleMouseButtons'			GetNumberOfConsoleMouseButtons				kernel32	0	1	# KernelBase
imp	'GetNumberOfEventLogRecords'				GetNumberOfEventLogRecords				advapi32	1346
imp	'GetNumberOfPhysicalMonitors'				GetNumberOfPhysicalMonitors				gdi32		1679
imp	'GetOEMCP'						GetOEMCP						kernel32	0		# KernelBase
imp	'GetOPMInformation'					GetOPMInformation					gdi32		1680
imp	'GetOPMRandomNumber'					GetOPMRandomNumber					gdi32		1681
imp	'GetObjectA'						GetObjectA						gdi32		1682
imp	'GetObjectType'						GetObjectType						gdi32		1683
imp	'GetObject'						GetObjectW						gdi32		1684
imp	'GetOldestEventLogRecord'				GetOldestEventLogRecord					advapi32	1347
imp	'GetOpenClipboardWindow'				GetOpenClipboardWindow					user32		1905
imp	'GetOpenFileNameA'					GetOpenFileNameA					comdlg32	113	1
imp	'GetOpenFileName'					GetOpenFileNameW					comdlg32	114	1
imp	'GetOsManufacturingMode'				GetOsManufacturingMode					KernelBase	625
imp	'GetOsSafeBootMode'					GetOsSafeBootMode					KernelBase	626
imp	'GetOutlineTextMetricsA'				GetOutlineTextMetricsA					gdi32		1685
imp	'GetOutlineTextMetrics'					GetOutlineTextMetricsW					gdi32		1686
imp	'GetOverlappedAccessResults'				GetOverlappedAccessResults				advapi32	1348
imp	'GetOverlappedResult'					GetOverlappedResult					kernel32	0		# KernelBase
imp	'GetOverlappedResultEx'					GetOverlappedResultEx					kernel32	0		# KernelBase
imp	'GetPackageApplicationContext'				GetPackageApplicationContext				KernelBase	629
imp	'GetPackageApplicationIds'				GetPackageApplicationIds				kernel32	0		# KernelBase
imp	'GetPackageApplicationProperty'				GetPackageApplicationProperty				KernelBase	631
imp	'GetPackageApplicationPropertyString'			GetPackageApplicationPropertyString			KernelBase	632
imp	'GetPackageApplicationResourcesContext'			GetPackageApplicationResourcesContext			KernelBase	633
imp	'GetPackageContext'					GetPackageContext					KernelBase	634
imp	'GetPackageFamilyName'					GetPackageFamilyName					kernel32	0		# KernelBase
imp	'GetPackageFamilyNameFromProgId'			GetPackageFamilyNameFromProgId				KernelBase	636
imp	'GetPackageFamilyNameFromToken'				GetPackageFamilyNameFromToken				KernelBase	637
imp	'GetPackageFullName'					GetPackageFullName					kernel32	0		# KernelBase
imp	'GetPackageFullNameFromToken'				GetPackageFullNameFromToken				KernelBase	639
imp	'GetPackageId'						GetPackageId						kernel32	0		# KernelBase
imp	'GetPackageInfo'					GetPackageInfo						kernel32	0		# KernelBase
imp	'GetPackageInstallTime'					GetPackageInstallTime					KernelBase	642
imp	'GetPackageOSMaxVersionTested'				GetPackageOSMaxVersionTested				KernelBase	643
imp	'GetPackagePath'					GetPackagePath						kernel32	0		# KernelBase
imp	'GetPackagePathByFullName'				GetPackagePathByFullName				kernel32	0		# KernelBase
imp	'GetPackagePathOnVolume'				GetPackagePathOnVolume					KernelBase	646
imp	'GetPackageProperty'					GetPackageProperty					KernelBase	647
imp	'GetPackagePropertyString'				GetPackagePropertyString				KernelBase	648
imp	'GetPackageResourcesContext'				GetPackageResourcesContext				KernelBase	649
imp	'GetPackageResourcesProperty'				GetPackageResourcesProperty				KernelBase	650
imp	'GetPackageSecurityContext'				GetPackageSecurityContext				KernelBase	651
imp	'GetPackageSecurityProperty'				GetPackageSecurityProperty				KernelBase	652
imp	'GetPackageStatus'					GetPackageStatus					KernelBase	653
imp	'GetPackageStatusForUser'				GetPackageStatusForUser					KernelBase	654
imp	'GetPackageStatusForUserSid'				GetPackageStatusForUserSid				KernelBase	655
imp	'GetPackageTargetPlatformProperty'			GetPackageTargetPlatformProperty			KernelBase	656
imp	'GetPackageVolumeSisPath'				GetPackageVolumeSisPath					KernelBase	657
imp	'GetPackagesByPackageFamily'				GetPackagesByPackageFamily				kernel32	0		# KernelBase
imp	'GetPaletteEntries'					GetPaletteEntries					gdi32		1687
imp	'GetParent'						GetParent						user32		1906	1
imp	'GetPath'						GetPath							gdi32		1688
imp	'GetPerformanceInfo'					GetPerformanceInfo					KernelBase	659
imp	'GetPersistedFileLocation'				GetPersistedFileLocationW				KernelBase	660
imp	'GetPersistedRegistryLocation'				GetPersistedRegistryLocationW				KernelBase	661
imp	'GetPersistedRegistryValue'				GetPersistedRegistryValueW				KernelBase	662
imp	'GetPhysicalCursorPos'					GetPhysicalCursorPos					user32		1907
imp	'GetPhysicalMonitorDescription'				GetPhysicalMonitorDescription				gdi32		1689
imp	'GetPhysicalMonitors'					GetPhysicalMonitors					gdi32		1690
imp	'GetPhysicallyInstalledSystemMemory'			GetPhysicallyInstalledSystemMemory			kernel32	0		# KernelBase
imp	'GetPixel'						GetPixel						gdi32		1691	3
imp	'GetPixelFormat'					GetPixelFormat						gdi32		1692
imp	'GetPointerCursorId'					GetPointerCursorId					user32		1908
imp	'GetPointerDevice'					GetPointerDevice					user32		1909
imp	'GetPointerDeviceCursors'				GetPointerDeviceCursors					user32		1910
imp	'GetPointerDeviceProperties'				GetPointerDeviceProperties				user32		1911
imp	'GetPointerDeviceRects'					GetPointerDeviceRects					user32		1912
imp	'GetPointerDevices'					GetPointerDevices					user32		1913
imp	'GetPointerFrameArrivalTimes'				GetPointerFrameArrivalTimes				user32		1914
imp	'GetPointerFrameInfo'					GetPointerFrameInfo					user32		1915
imp	'GetPointerFrameInfoHistory'				GetPointerFrameInfoHistory				user32		1916
imp	'GetPointerFramePenInfo'				GetPointerFramePenInfo					user32		1917
imp	'GetPointerFramePenInfoHistory'				GetPointerFramePenInfoHistory				user32		1918
imp	'GetPointerFrameTouchInfo'				GetPointerFrameTouchInfo				user32		1919
imp	'GetPointerFrameTouchInfoHistory'			GetPointerFrameTouchInfoHistory				user32		1920
imp	'GetPointerInfo'					GetPointerInfo						user32		1921
imp	'GetPointerInfoHistory'					GetPointerInfoHistory					user32		1922
imp	'GetPointerInputTransform'				GetPointerInputTransform				user32		1923
imp	'GetPointerPenInfo'					GetPointerPenInfo					user32		1924
imp	'GetPointerPenInfoHistory'				GetPointerPenInfoHistory				user32		1925
imp	'GetPointerTouchInfo'					GetPointerTouchInfo					user32		1926
imp	'GetPointerTouchInfoHistory'				GetPointerTouchInfoHistory				user32		1927
imp	'GetPointerType'					GetPointerType						user32		1928
imp	'GetPolyFillMode'					GetPolyFillMode						gdi32		1693
imp	'GetPreviousFgPolicyRefreshInfoInternal'		GetPreviousFgPolicyRefreshInfoInternal			KernelBase	664
imp	'GetPriorityClass'					GetPriorityClass					kernel32	0	1	# KernelBase
imp	'GetPriorityClipboardFormat'				GetPriorityClipboardFormat				user32		1929
imp	'GetPrivateObjectSecurity'				GetPrivateObjectSecurity				advapi32	0		# KernelBase
imp	'GetPrivateProfileIntA'					GetPrivateProfileIntA					kernel32	680
imp	'GetPrivateProfileInt'					GetPrivateProfileIntW					kernel32	681
imp	'GetPrivateProfileSectionA'				GetPrivateProfileSectionA				kernel32	682
imp	'GetPrivateProfileSectionNamesA'			GetPrivateProfileSectionNamesA				kernel32	683
imp	'GetPrivateProfileSectionNames'				GetPrivateProfileSectionNamesW				kernel32	684
imp	'GetPrivateProfileSection'				GetPrivateProfileSectionW				kernel32	685
imp	'GetPrivateProfileStringA'				GetPrivateProfileStringA				kernel32	686
imp	'GetPrivateProfileString'				GetPrivateProfileStringW				kernel32	687
imp	'GetPrivateProfileStructA'				GetPrivateProfileStructA				kernel32	688
imp	'GetPrivateProfileStruct'				GetPrivateProfileStructW				kernel32	689
imp	'GetProcAddress'					GetProcAddress						kernel32	0	2	# KernelBase
imp	'GetProcAddressForCaller'				GetProcAddressForCaller					KernelBase	668
imp	'GetProcessAffinityMask'				GetProcessAffinityMask					kernel32	0	3	# KernelBase
imp	'GetProcessDEPPolicy'					GetProcessDEPPolicy					kernel32	692
imp	'GetProcessDefaultCpuSets'				GetProcessDefaultCpuSets				kernel32	0		# KernelBase
imp	'GetProcessDefaultLayout'				GetProcessDefaultLayout					user32		1930
imp	'GetProcessDpiAwarenessInternal'			GetProcessDpiAwarenessInternal				user32		1931
imp	'GetProcessGroupAffinity'				GetProcessGroupAffinity					kernel32	0		# KernelBase
imp	'GetProcessHandleCount'					GetProcessHandleCount					kernel32	0	2	# KernelBase
imp	'GetProcessHeap'					GetProcessHeap						kernel32	0		# KernelBase
imp	'GetProcessHeaps'					GetProcessHeaps						kernel32	0		# KernelBase
imp	'GetProcessId'						GetProcessId						kernel32	0	1	# KernelBase
imp	'GetProcessIdOfThread'					GetProcessIdOfThread					kernel32	0	1	# KernelBase
imp	'GetProcessImageFileNameA'				GetProcessImageFileNameA				KernelBase	676
imp	'GetProcessImageFileName'				GetProcessImageFileNameW				KernelBase	677
imp	'GetProcessInformation'					GetProcessInformation					kernel32	0	4	# KernelBase
imp	'GetProcessIoCounters'					GetProcessIoCounters					kernel32	701	2
imp	'GetProcessMemoryInfo'					GetProcessMemoryInfo					KernelBase	679
imp	'GetProcessMitigationPolicy'				GetProcessMitigationPolicy				kernel32	0		# KernelBase
imp	'GetProcessPreferredUILanguages'			GetProcessPreferredUILanguages				kernel32	0		# KernelBase
imp	'GetProcessPriorityBoost'				GetProcessPriorityBoost					kernel32	0	2	# KernelBase
imp	'GetProcessSessionFonts'				GetProcessSessionFonts					gdi32		1694
imp	'GetProcessShutdownParameters'				GetProcessShutdownParameters				kernel32	0		# KernelBase
imp	'GetProcessTimes'					GetProcessTimes						kernel32	0	5	# KernelBase
imp	'GetProcessUIContextInformation'			GetProcessUIContextInformation				user32		2521
imp	'GetProcessVersion'					GetProcessVersion					kernel32	0		# KernelBase
imp	'GetProcessWindowStation'				GetProcessWindowStation					user32		1932
imp	'GetProcessWorkingSetSize'				GetProcessWorkingSetSize				kernel32	708	3
imp	'GetProcessWorkingSetSizeEx'				GetProcessWorkingSetSizeEx				kernel32	0	4	# KernelBase
imp	'GetProcessorSystemCycleTime'				GetProcessorSystemCycleTime				kernel32	0		# KernelBase
imp	'GetProductInfo'					GetProductInfo						kernel32	0		# KernelBase
imp	'GetProfileIntA'					GetProfileIntA						kernel32	712
imp	'GetProfileInt'						GetProfileIntW						kernel32	713
imp	'GetProfileSectionA'					GetProfileSectionA					kernel32	714
imp	'GetProfileSection'					GetProfileSectionW					kernel32	715
imp	'GetProfileStringA'					GetProfileStringA					kernel32	716
imp	'GetProfileString'					GetProfileStringW					kernel32	717
imp	'GetProgmanWindow'					GetProgmanWindow					user32		1933
imp	'GetPropA'						GetPropA						user32		1934
imp	'GetProp'						GetPropW						user32		1935
imp	'GetProtocolAumid'					GetProtocolAumid					KernelBase	689
imp	'GetProtocolProperty'					GetProtocolProperty					KernelBase	690
imp	'GetPtrCalData'						GetPtrCalData						KernelBase	691
imp	'GetPtrCalDataArray'					GetPtrCalDataArray					KernelBase	692
imp	'GetPublisherCacheFolder'				GetPublisherCacheFolder					KernelBase	693
imp	'GetPublisherRootFolder'				GetPublisherRootFolder					KernelBase	694
imp	'GetQueueStatus'					GetQueueStatus						user32		1936
imp	'GetQueuedCompletionStatus'				GetQueuedCompletionStatus				kernel32	0	5	# KernelBase
imp	'GetQueuedCompletionStatusEx'				GetQueuedCompletionStatusEx				kernel32	0	6	# KernelBase
imp	'GetROP2'						GetROP2							gdi32		1695
imp	'GetRandomRgn'						GetRandomRgn						gdi32		1696
imp	'GetRasterizerCaps'					GetRasterizerCaps					gdi32		1697
imp	'GetRawInputBuffer'					GetRawInputBuffer					user32		1937
imp	'GetRawInputData'					GetRawInputData						user32		1938
imp	'GetRawInputDeviceInfoA'				GetRawInputDeviceInfoA					user32		1939
imp	'GetRawInputDeviceInfo'					GetRawInputDeviceInfoW					user32		1940
imp	'GetRawInputDeviceList'					GetRawInputDeviceList					user32		1941
imp	'GetRawPointerDeviceData'				GetRawPointerDeviceData					user32		1942
imp	'GetReasonTitleFromReasonCode'				GetReasonTitleFromReasonCode				user32		1943
imp	'GetRegionData'						GetRegionData						gdi32		1698
imp	'GetRegisteredRawInputDevices'				GetRegisteredRawInputDevices				user32		1944
imp	'GetRegistryExtensionFlags'				GetRegistryExtensionFlags				KernelBase	697
imp	'GetRegistryValueWithFallback'				GetRegistryValueWithFallbackW				KernelBase	698
imp	'GetRelAbs'						GetRelAbs						gdi32		1699
imp	'GetRgnBox'						GetRgnBox						gdi32		1700
imp	'GetRoamingLastObservedChangeTime'			GetRoamingLastObservedChangeTime			KernelBase	699
imp	'GetSaveFileNameA'					GetSaveFileNameA					comdlg32	115	1
imp	'GetSaveFileName'					GetSaveFileNameW					comdlg32	116	1
imp	'GetScrollBarInfo'					GetScrollBarInfo					user32		1945
imp	'GetScrollInfo'						GetScrollInfo						user32		1946
imp	'GetScrollPos'						GetScrollPos						user32		1947
imp	'GetScrollRange'					GetScrollRange						user32		1948
imp	'GetSecureSystemAppDataFolder'				GetSecureSystemAppDataFolder				KernelBase	700
imp	'GetSecurityDescriptorControl'				GetSecurityDescriptorControl				advapi32	0		# KernelBase
imp	'GetSecurityDescriptorDacl'				GetSecurityDescriptorDacl				advapi32	0		# KernelBase
imp	'GetSecurityDescriptorGroup'				GetSecurityDescriptorGroup				advapi32	0		# KernelBase
imp	'GetSecurityDescriptorLength'				GetSecurityDescriptorLength				advapi32	0		# KernelBase
imp	'GetSecurityDescriptorOwner'				GetSecurityDescriptorOwner				advapi32	0		# KernelBase
imp	'GetSecurityDescriptorRMControl'			GetSecurityDescriptorRMControl				advapi32	0		# KernelBase
imp	'GetSecurityDescriptorSacl'				GetSecurityDescriptorSacl				advapi32	0		# KernelBase
imp	'GetSecurityInfo'					GetSecurityInfo						advapi32	1357
imp	'GetSecurityInfoExA'					GetSecurityInfoExA					advapi32	1358
imp	'GetSecurityInfoEx'					GetSecurityInfoExW					advapi32	1359
imp	'GetSendMessageReceiver'				GetSendMessageReceiver					user32		1949
imp	'GetSerializedAtomBytes'				GetSerializedAtomBytes					KernelBase	708
imp	'GetServiceDisplayNameA'				GetServiceDisplayNameA					advapi32	1360
imp	'GetServiceDisplayName'					GetServiceDisplayNameW					advapi32	1361
imp	'GetServiceKeyNameA'					GetServiceKeyNameA					advapi32	1362
imp	'GetServiceKeyName'					GetServiceKeyNameW					advapi32	1363
imp	'GetSharedLocalFolder'					GetSharedLocalFolder					KernelBase	709
imp	'GetShellWindow'					GetShellWindow						user32		1950	0
imp	'GetShortPathNameA'					GetShortPathNameA					kernel32	720
imp	'GetShortPathName'					GetShortPathNameW					kernel32	0		# KernelBase
imp	'GetSidIdentifierAuthority'				GetSidIdentifierAuthority				advapi32	0		# KernelBase
imp	'GetSidLengthRequired'					GetSidLengthRequired					advapi32	0		# KernelBase
imp	'GetSidSubAuthority'					GetSidSubAuthority					advapi32	0		# KernelBase
imp	'GetSidSubAuthorityCount'				GetSidSubAuthorityCount					advapi32	0		# KernelBase
imp	'GetStagedPackageOrigin'				GetStagedPackageOrigin					KernelBase	715
imp	'GetStagedPackagePathByFullName'			GetStagedPackagePathByFullName				kernel32	0		# KernelBase
imp	'GetStartupInfo'					GetStartupInfoW						kernel32	0	1	# KernelBase
imp	'GetStartupInfoA'					GetStartupInfoA						kernel32	723	1
imp	'GetStateContainerDepth'				GetStateContainerDepth					KernelBase	718
imp	'GetStateFolder'					GetStateFolder						KernelBase	719
imp	'GetStateRootFolder'					GetStateRootFolder					KernelBase	720
imp	'GetStateRootFolderBase'				GetStateRootFolderBase					KernelBase	721
imp	'GetStateSettingsFolder'				GetStateSettingsFolder					KernelBase	722
imp	'GetStateVersion'					GetStateVersion						KernelBase	723
imp	'GetStdHandle'						GetStdHandle						kernel32	0	1	# KernelBase
imp	'GetStockObject'					GetStockObject						gdi32		1701
imp	'GetStretchBltMode'					GetStretchBltMode					gdi32		1702
imp	'GetStringBitmapA'					GetStringBitmapA					gdi32		1703
imp	'GetStringBitmap'					GetStringBitmapW					gdi32		1704
imp	'GetStringConditionFromBinary'				GetStringConditionFromBinary				advapi32	1368
imp	'GetStringScripts'					GetStringScripts					kernel32	0		# KernelBase
imp	'GetStringTableEntry'					GetStringTableEntry					KernelBase	726
imp	'GetStringTypeA'					GetStringTypeA						kernel32	0		# KernelBase
imp	'GetStringTypeExA'					GetStringTypeExA					kernel32	729
imp	'GetStringTypeEx'					GetStringTypeExW					kernel32	0		# KernelBase
imp	'GetStringType'						GetStringTypeW						kernel32	0		# KernelBase
imp	'GetSubMenu'						GetSubMenu						user32		1951
imp	'GetSuggestedOPMProtectedOutputArraySize'		GetSuggestedOPMProtectedOutputArraySize			gdi32		1705
imp	'GetSysColor'						GetSysColor						user32		1952
imp	'GetSysColorBrush'					GetSysColorBrush					user32		1953
imp	'GetSystemAppDataFolder'				GetSystemAppDataFolder					KernelBase	730
imp	'GetSystemAppDataKey'					GetSystemAppDataKey					KernelBase	731
imp	'GetSystemCpuSetInformation'				GetSystemCpuSetInformation				kernel32	0		# KernelBase
imp	'GetSystemDEPPolicy'					GetSystemDEPPolicy					kernel32	734
imp	'GetSystemDefaultLCID'					GetSystemDefaultLCID					kernel32	0		# KernelBase
imp	'GetSystemDefaultLangID'				GetSystemDefaultLangID					kernel32	0		# KernelBase
imp	'GetSystemDefaultLocaleName'				GetSystemDefaultLocaleName				kernel32	0		# KernelBase
imp	'GetSystemDefaultUILanguage'				GetSystemDefaultUILanguage				kernel32	0		# KernelBase
imp	'GetSystemDirectory'					GetSystemDirectoryW					kernel32	0	2	# KernelBase
imp	'GetSystemDirectoryA'					GetSystemDirectoryA					kernel32	0	2	# KernelBase
imp	'GetSystemDpiForProcess'				GetSystemDpiForProcess					user32		1954
imp	'GetSystemFileCacheSize'				GetSystemFileCacheSize					kernel32	0		# KernelBase
imp	'GetSystemFirmwareTable'				GetSystemFirmwareTable					kernel32	0		# KernelBase
imp	'GetSystemInfo'						GetSystemInfo						kernel32	0	1	# KernelBase
imp	'GetSystemMenu'						GetSystemMenu						user32		1955	2
imp	'GetSystemMetadataPath'					GetSystemMetadataPath					KernelBase	742
imp	'GetSystemMetadataPathForPackage'			GetSystemMetadataPathForPackage				KernelBase	743
imp	'GetSystemMetadataPathForPackageFamily'			GetSystemMetadataPathForPackageFamily			KernelBase	744
imp	'GetSystemMetrics'					GetSystemMetrics					user32		1956
imp	'GetSystemMetricsForDpi'				GetSystemMetricsForDpi					user32		1957
imp	'GetSystemPaletteEntries'				GetSystemPaletteEntries					gdi32		1706
imp	'GetSystemPaletteUse'					GetSystemPaletteUse					gdi32		1707
imp	'GetSystemPersistedStorageItemList'			GetSystemPersistedStorageItemList			shell32		919
imp	'GetSystemPowerStatus'					GetSystemPowerStatus					kernel32	744
imp	'GetSystemPreferredUILanguages'				GetSystemPreferredUILanguages				kernel32	0		# KernelBase
imp	'GetSystemRegistryQuota'				GetSystemRegistryQuota					kernel32	746
imp	'GetSystemStateRootFolder'				GetSystemStateRootFolder				KernelBase	746
imp	'GetSystemTime'						GetSystemTime						kernel32	0	1	# KernelBase
imp	'GetSystemTimeAdjustment'				GetSystemTimeAdjustment					kernel32	0		# KernelBase
imp	'GetSystemTimeAdjustmentPrecise'			GetSystemTimeAdjustmentPrecise				KernelBase	749
imp	'GetSystemTimeAsFileTime'				GetSystemTimeAsFileTime					kernel32	0	1	# KernelBase
imp	'GetSystemTimePreciseAsFileTime'			GetSystemTimePreciseAsFileTime				kernel32	0	1	# KernelBase
imp	'GetSystemTimes'					GetSystemTimes						kernel32	0		# KernelBase
imp	'GetSystemWindowsDirectoryA'				GetSystemWindowsDirectoryA				kernel32	0		# KernelBase
imp	'GetSystemWindowsDirectory'				GetSystemWindowsDirectoryW				kernel32	0		# KernelBase
imp	'GetSystemWow64Directory2A'				GetSystemWow64Directory2A				KernelBase	755
imp	'GetSystemWow64Directory2W'				GetSystemWow64Directory2W				KernelBase	756
imp	'GetSystemWow64DirectoryA'				GetSystemWow64DirectoryA				kernel32	0		# KernelBase
imp	'GetSystemWow64Directory'				GetSystemWow64DirectoryW				kernel32	0		# KernelBase
imp	'GetTabbedTextExtentA'					GetTabbedTextExtentA					user32		1958
imp	'GetTabbedTextExtent'					GetTabbedTextExtentW					user32		1959
imp	'GetTapeParameters'					GetTapeParameters					kernel32	756
imp	'GetTapePosition'					GetTapePosition						kernel32	757
imp	'GetTapeStatus'						GetTapeStatus						kernel32	758
imp	'GetTargetPlatformContext'				GetTargetPlatformContext				KernelBase	759
imp	'GetTaskmanWindow'					GetTaskmanWindow					user32		1960
imp	'GetTempFileNameA'					GetTempFileNameA					kernel32	0		# KernelBase
imp	'GetTempFileName'					GetTempFileNameW					kernel32	0		# KernelBase
imp	'GetTempPath'						GetTempPathW						kernel32	0	2	# KernelBase
imp	'GetTempPathA'						GetTempPathA						kernel32	0	2	# KernelBase
imp	'GetTextAlign'						GetTextAlign						gdi32		1708
imp	'GetTextCharacterExtra'					GetTextCharacterExtra					gdi32		1709
imp	'GetTextCharset'					GetTextCharset						gdi32		1710
imp	'GetTextCharsetInfo'					GetTextCharsetInfo					gdi32		1711
imp	'GetTextColor'						GetTextColor						gdi32		1712
imp	'GetTextExtentExPointA'					GetTextExtentExPointA					gdi32		1713
imp	'GetTextExtentExPointI'					GetTextExtentExPointI					gdi32		1714
imp	'GetTextExtentExPoint'					GetTextExtentExPointW					gdi32		1715
imp	'GetTextExtentExPointWPri'				GetTextExtentExPointWPri				gdi32		1716
imp	'GetTextExtentPoint32A'					GetTextExtentPoint32A					gdi32		1717
imp	'GetTextExtentPoint32W'					GetTextExtentPoint32W					gdi32		1718
imp	'GetTextExtentPointA'					GetTextExtentPointA					gdi32		1719
imp	'GetTextExtentPointI'					GetTextExtentPointI					gdi32		1720
imp	'GetTextExtentPoint'					GetTextExtentPointW					gdi32		1721
imp	'GetTextFaceA'						GetTextFaceA						gdi32		1722
imp	'GetTextFaceAlias'					GetTextFaceAliasW					gdi32		1723
imp	'GetTextFace'						GetTextFaceW						gdi32		1724
imp	'GetTextMetricsA'					GetTextMetricsA						gdi32		1725
imp	'GetTextMetrics'					GetTextMetricsW						gdi32		1726
imp	'GetThreadContext'					GetThreadContext					kernel32	0		# KernelBase
imp	'GetThreadDescription'					GetThreadDescription					KernelBase	765
imp	'GetThreadDesktop'					GetThreadDesktop					user32		1961
imp	'GetThreadDpiAwarenessContext'				GetThreadDpiAwarenessContext				user32		1962
imp	'GetThreadDpiHostingBehavior'				GetThreadDpiHostingBehavior				user32		1963
imp	'GetThreadErrorMode'					GetThreadErrorMode					kernel32	0		# KernelBase
imp	'GetThreadGroupAffinity'				GetThreadGroupAffinity					kernel32	0		# KernelBase
imp	'GetThreadIOPendingFlag'				GetThreadIOPendingFlag					kernel32	0	2	# KernelBase
imp	'GetThreadId'						GetThreadId						kernel32	0	1	# KernelBase
imp	'GetThreadIdealProcessorEx'				GetThreadIdealProcessorEx				kernel32	0		# KernelBase
imp	'GetThreadInformation'					GetThreadInformation					kernel32	0		# KernelBase
imp	'GetThreadLocale'					GetThreadLocale						kernel32	0		# KernelBase
imp	'GetThreadPreferredUILanguages'				GetThreadPreferredUILanguages				kernel32	0		# KernelBase
imp	'GetThreadPriority'					GetThreadPriority					kernel32	0	1	# KernelBase
imp	'GetThreadPriorityBoost'				GetThreadPriorityBoost					kernel32	0	2	# KernelBase
imp	'GetThreadSelectedCpuSets'				GetThreadSelectedCpuSets				kernel32	0		# KernelBase
imp	'GetThreadSelectorEntry'				GetThreadSelectorEntry					kernel32	776
imp	'GetThreadTimes'					GetThreadTimes						kernel32	0	5	# KernelBase
imp	'GetThreadUILanguage'					GetThreadUILanguage					kernel32	0		# KernelBase
imp	'GetThreadWaitChain'					GetThreadWaitChain					advapi32	1369
imp	'GetTickCount'						GetTickCount						kernel32	0		# KernelBase
imp	'GetTickCount64'					GetTickCount64						kernel32	0	0	# KernelBase
imp	'GetTimeFormatA'					GetTimeFormatA						kernel32	0		# KernelBase
imp	'GetTimeFormatAWorker'					GetTimeFormatAWorker					kernel32	782
imp	'GetTimeFormatEx'					GetTimeFormatEx						kernel32	0		# KernelBase
imp	'GetTimeFormat'						GetTimeFormatW						kernel32	0		# KernelBase
imp	'GetTimeFormatWWorker'					GetTimeFormatWWorker					kernel32	785
imp	'GetTimeZoneInformation'				GetTimeZoneInformation					kernel32	0		# KernelBase
imp	'GetTimeZoneInformationForYear'				GetTimeZoneInformationForYear				kernel32	0		# KernelBase
imp	'GetTitleBarInfo'					GetTitleBarInfo						user32		1964
imp	'GetTokenInformation'					GetTokenInformation					advapi32	0		# KernelBase
imp	'GetTopLevelWindow'					GetTopLevelWindow					user32		1965
imp	'GetTopWindow'						GetTopWindow						user32		1966
imp	'GetTouchInputInfo'					GetTouchInputInfo					user32		1967
imp	'GetTransform'						GetTransform						gdi32		1727
imp	'GetTrusteeFormA'					GetTrusteeFormA						advapi32	1374
imp	'GetTrusteeForm'					GetTrusteeFormW						advapi32	1375
imp	'GetTrusteeNameA'					GetTrusteeNameA						advapi32	1376
imp	'GetTrusteeName'					GetTrusteeNameW						advapi32	1377
imp	'GetTrusteeTypeA'					GetTrusteeTypeA						advapi32	1378
imp	'GetTrusteeType'					GetTrusteeTypeW						advapi32	1379
imp	'GetUILanguageInfo'					GetUILanguageInfo					kernel32	0		# KernelBase
imp	'GetUmsCompletionListEvent'				GetUmsCompletionListEvent				kernel32	789
imp	'GetUmsSystemThreadInformation'				GetUmsSystemThreadInformation				kernel32	790
imp	'GetUnicodeStringToEightBitSizeRoutine'			GetUnicodeStringToEightBitSizeRoutine			KernelBase	791
imp	'GetUnicodeStringToEightBitStringRoutine'		GetUnicodeStringToEightBitStringRoutine			KernelBase	792
imp	'GetUnpredictedMessagePos'				GetUnpredictedMessagePos				user32		1968
imp	'GetUpdateRect'						GetUpdateRect						user32		1969
imp	'GetUpdateRgn'						GetUpdateRgn						user32		1970
imp	'GetUpdatedClipboardFormats'				GetUpdatedClipboardFormats				user32		1971
imp	'GetUserDefaultGeoName'					GetUserDefaultGeoName					KernelBase	793
imp	'GetUserDefaultLCID'					GetUserDefaultLCID					kernel32	0		# KernelBase
imp	'GetUserDefaultLangID'					GetUserDefaultLangID					kernel32	0		# KernelBase
imp	'GetUserDefaultLocaleName'				GetUserDefaultLocaleName				kernel32	0		# KernelBase
imp	'GetUserDefaultUILanguage'				GetUserDefaultUILanguage				kernel32	0		# KernelBase
imp	'GetUserGeoID'						GetUserGeoID						kernel32	0		# KernelBase
imp	'GetUserInfo'						GetUserInfo						KernelBase	799
imp	'GetUserInfoWord'					GetUserInfoWord						KernelBase	800
imp	'GetUserName'						GetUserNameW						advapi32	1381	2
imp	'GetUserNameA'						GetUserNameA						advapi32	1380	2
imp	'GetUserObjectInformationA'				GetUserObjectInformationA				user32		1972
imp	'GetUserObjectInformation'				GetUserObjectInformationW				user32		1973
imp	'GetUserObjectSecurity'					GetUserObjectSecurity					user32		1974
imp	'GetUserOverrideString'					GetUserOverrideString					KernelBase	801
imp	'GetUserOverrideWord'					GetUserOverrideWord					KernelBase	802
imp	'GetUserPreferredUILanguages'				GetUserPreferredUILanguages				kernel32	0		# KernelBase
imp	'GetVDMCurrentDirectories'				GetVDMCurrentDirectories				kernel32	798
imp	'GetVersion'						GetVersion						kernel32	0		# KernelBase
imp	'GetVersionExA'						GetVersionExA						kernel32	0		# KernelBase
imp	'GetVersionEx'						GetVersionExW						kernel32	0		# KernelBase
imp	'GetViewportExtEx'					GetViewportExtEx					gdi32		1728
imp	'GetViewportOrgEx'					GetViewportOrgEx					gdi32		1729
imp	'GetVolumeInformationA'					GetVolumeInformationA					kernel32	0		# KernelBase
imp	'GetVolumeInformationByHandle'				GetVolumeInformationByHandleW				kernel32	0		# KernelBase
imp	'GetVolumeInformation'					GetVolumeInformationW					kernel32	0		# KernelBase
imp	'GetVolumeNameForVolumeMountPointA'			GetVolumeNameForVolumeMountPointA			kernel32	805
imp	'GetVolumeNameForVolumeMountPoint'			GetVolumeNameForVolumeMountPointW			kernel32	0		# KernelBase
imp	'GetVolumePathNameA'					GetVolumePathNameA					kernel32	807
imp	'GetVolumePathName'					GetVolumePathNameW					kernel32	0		# KernelBase
imp	'GetVolumePathNamesForVolumeNameA'			GetVolumePathNamesForVolumeNameA			kernel32	809
imp	'GetVolumePathNamesForVolumeName'			GetVolumePathNamesForVolumeNameW			kernel32	0		# KernelBase
imp	'GetWinMetaFileBits'					GetWinMetaFileBits					gdi32		1730
imp	'GetWinStationInfo'					GetWinStationInfo					user32		1975
imp	'GetWindow'						GetWindow						user32		1976	2
imp	'GetWindowBand'						GetWindowBand						user32		1977
imp	'GetWindowCompositionAttribute'				GetWindowCompositionAttribute				user32		1978
imp	'GetWindowCompositionInfo'				GetWindowCompositionInfo				user32		1979
imp	'GetWindowContextHelpId'				GetWindowContextHelpId					user32		1980
imp	'GetWindowDC'						GetWindowDC						user32		1981
imp	'GetWindowDisplayAffinity'				GetWindowDisplayAffinity				user32		1982
imp	'GetWindowDpiAwarenessContext'				GetWindowDpiAwarenessContext				user32		1983
imp	'GetWindowDpiHostingBehavior'				GetWindowDpiHostingBehavior				user32		1984
imp	'GetWindowExtEx'					GetWindowExtEx						gdi32		1731
imp	'GetWindowFeedbackSetting'				GetWindowFeedbackSetting				user32		1985
imp	'GetWindowInfo'						GetWindowInfo						user32		1986
imp	'GetWindowLongA'					GetWindowLongA						user32		1987
imp	'GetWindowLongPtrA'					GetWindowLongPtrA					user32		1988
imp	'GetWindowLongPtr'					GetWindowLongPtrW					user32		1989
imp	'GetWindowLong'						GetWindowLongW						user32		1990
imp	'GetWindowMinimizeRect'					GetWindowMinimizeRect					user32		1991
imp	'GetWindowModuleFileNameA'				GetWindowModuleFileNameA				user32		1993
imp	'GetWindowModuleFileName'				GetWindowModuleFileNameW				user32		1994
imp	'GetWindowOrgEx'					GetWindowOrgEx						gdi32		1732
imp	'GetWindowPlacement'					GetWindowPlacement					user32		1995	2
imp	'GetWindowProcessHandle'				GetWindowProcessHandle					user32		1996
imp	'GetWindowRect'						GetWindowRect						user32		1997	2
imp	'GetWindowRgn'						GetWindowRgn						user32		1998
imp	'GetWindowRgnBox'					GetWindowRgnBox						user32		1999
imp	'GetWindowRgnEx'					GetWindowRgnEx						user32		2000
imp	'GetWindowText'						GetWindowTextW						user32		2007	3
imp	'GetWindowTextA'					GetWindowTextA						user32		2003	3
imp	'GetWindowTextLengthA'					GetWindowTextLengthA					user32		2004
imp	'GetWindowTextLength'					GetWindowTextLengthW					user32		2006
imp	'GetWindowThreadProcessId'				GetWindowThreadProcessId				user32		2008
imp	'GetWindowWord'						GetWindowWord						user32		2009
imp	'GetWindowsAccountDomainSid'				GetWindowsAccountDomainSid				advapi32	0		# KernelBase
imp	'GetWindowsDirectory'					GetWindowsDirectoryW					kernel32	0	2	# KernelBase
imp	'GetWindowsDirectoryA'					GetWindowsDirectoryA					kernel32	0	2	# KernelBase
imp	'GetWorldTransform'					GetWorldTransform					gdi32		1733
imp	'GetWriteWatch'						GetWriteWatch						kernel32	0		# KernelBase
imp	'GetWsChanges'						GetWsChanges						KernelBase	817
imp	'GetWsChangesEx'					GetWsChangesEx						KernelBase	818
imp	'GetXStateFeaturesMask'					GetXStateFeaturesMask					kernel32	0		# KernelBase
imp	'GhostWindowFromHungWindow'				GhostWindowFromHungWindow				user32		2011
imp	'GlobalAddAtomA'					GlobalAddAtomA						kernel32	815
imp	'GlobalAddAtomExA'					GlobalAddAtomExA					kernel32	816
imp	'GlobalAddAtomEx'					GlobalAddAtomExW					kernel32	817
imp	'GlobalAddAtom'						GlobalAddAtomW						kernel32	818
imp	'GlobalAlloc'						GlobalAlloc						kernel32	0		# KernelBase
imp	'GlobalCompact'						GlobalCompact						kernel32	820
imp	'GlobalDeleteAtom'					GlobalDeleteAtom					kernel32	821
imp	'GlobalFindAtomA'					GlobalFindAtomA						kernel32	822
imp	'GlobalFindAtom'					GlobalFindAtomW						kernel32	823
imp	'GlobalFix'						GlobalFix						kernel32	824
imp	'GlobalFlags'						GlobalFlags						kernel32	825
imp	'GlobalFree'						GlobalFree						kernel32	0		# KernelBase
imp	'GlobalGetAtomNameA'					GlobalGetAtomNameA					kernel32	827
imp	'GlobalGetAtomName'					GlobalGetAtomNameW					kernel32	828
imp	'GlobalHandle'						GlobalHandle						kernel32	829
imp	'GlobalLock'						GlobalLock						kernel32	830
imp	'GlobalMemoryStatus'					GlobalMemoryStatus					kernel32	831
imp	'GlobalMemoryStatusEx'					GlobalMemoryStatusEx					kernel32	0	1	# KernelBase
imp	'GlobalReAlloc'						GlobalReAlloc						kernel32	833
imp	'GlobalSize'						GlobalSize						kernel32	834
imp	'GlobalUnWire'						GlobalUnWire						kernel32	835
imp	'GlobalUnfix'						GlobalUnfix						kernel32	836
imp	'GlobalUnlock'						GlobalUnlock						kernel32	837
imp	'GlobalWire'						GlobalWire						kernel32	838
imp	'GrayStringA'						GrayStringA						user32		2012
imp	'GrayString'						GrayStringW						user32		2013
imp	'GuardCheckLongJumpTarget'				GuardCheckLongJumpTarget				KernelBase	823
imp	'HT_Get8BPPFormatPalette'				HT_Get8BPPFormatPalette					gdi32		1734
imp	'HT_Get8BPPMaskPalette'					HT_Get8BPPMaskPalette					gdi32		1735
imp	'HandleDelegatedInput'					HandleDelegatedInput					user32		2505
imp	'HasPolicyForegroundProcessingCompletedInternal'	HasPolicyForegroundProcessingCompletedInternal		KernelBase	824
imp	'HashData'						HashData						KernelBase	825
imp	'Heap32First'						Heap32First						kernel32	839
imp	'Heap32ListFirst'					Heap32ListFirst						kernel32	840
imp	'Heap32ListNext'					Heap32ListNext						kernel32	841
imp	'Heap32Next'						Heap32Next						kernel32	842
imp	'HeapCompact'						HeapCompact						kernel32	0		# KernelBase
imp	'HeapCreate'						HeapCreate						kernel32	0		# KernelBase
imp	'HeapDestroy'						HeapDestroy						kernel32	0		# KernelBase
imp	'HeapFree'						HeapFree						kernel32	847
imp	'HeapLock'						HeapLock						kernel32	0		# KernelBase
imp	'HeapQueryInformation'					HeapQueryInformation					kernel32	0		# KernelBase
imp	'HeapSetInformation'					HeapSetInformation					kernel32	0		# KernelBase
imp	'HeapSummary'						HeapSummary						KernelBase	836
imp	'HeapUnlock'						HeapUnlock						kernel32	0		# KernelBase
imp	'HeapValidate'						HeapValidate						kernel32	0		# KernelBase
imp	'HeapWalk'						HeapWalk						kernel32	0		# KernelBase
imp	'HideCaret'						HideCaret						user32		2014
imp	'HiliteMenuItem'					HiliteMenuItem						user32		2015
imp	'HungWindowFromGhostWindow'				HungWindowFromGhostWindow				user32		2016
imp	'ILAppendID'						ILAppendID						shell32		154
imp	'ILClone'						ILClone							shell32		18
imp	'ILCloneFirst'						ILCloneFirst						shell32		19
imp	'ILCombine'						ILCombine						shell32		25
imp	'ILCreateFromPathA'					ILCreateFromPathA					shell32		189
imp	'ILCreateFromPath'					ILCreateFromPathW					shell32		190
imp	'ILFindChild'						ILFindChild						shell32		24
imp	'ILFindLastID'						ILFindLastID						shell32		16
imp	'ILFree'						ILFree							shell32		155
imp	'ILGetNext'						ILGetNext						shell32		153
imp	'ILGetSize'						ILGetSize						shell32		152
imp	'ILIsEqual'						ILIsEqual						shell32		21
imp	'ILIsParent'						ILIsParent						shell32		23
imp	'ILLoadFromStreamEx'					ILLoadFromStreamEx					shell32		846
imp	'ILRemoveLastID'					ILRemoveLastID						shell32		17
imp	'ILSaveToStream'					ILSaveToStream						shell32		27
imp	'IMPGetIMEA'						IMPGetIMEA						user32		2017
imp	'IMPGetIMEW'						IMPGetIMEW						user32		2018
imp	'IMPQueryIMEA'						IMPQueryIMEA						user32		2019
imp	'IMPQueryIMEW'						IMPQueryIMEW						user32		2020
imp	'IMPSetIMEA'						IMPSetIMEA						user32		2021
imp	'IMPSetIMEW'						IMPSetIMEW						user32		2022
imp	'I_BrowserSetNetlogonState'				I_BrowserSetNetlogonState				netapi32	34
imp	'I_ScGetCurrentGroupState'				I_ScGetCurrentGroupStateW				advapi32	1001
imp	'I_ScReparseServiceDatabase'				I_ScReparseServiceDatabase				advapi32	1388
imp	'I_ScSetServiceBitsA'					I_ScSetServiceBitsA					advapi32	1391
imp	'I_ScSetServiceBits'					I_ScSetServiceBitsW					advapi32	1392
imp	'IdentifyCodeAuthzLevel'				IdentifyCodeAuthzLevelW					advapi32	1394
imp	'IdnToAscii'						IdnToAscii						kernel32	0		# KernelBase
imp	'IdnToNameprepUnicode'					IdnToNameprepUnicode					kernel32	0		# KernelBase
imp	'IdnToUnicode'						IdnToUnicode						kernel32	0		# KernelBase
imp	'ImpersonateAnonymousToken'				ImpersonateAnonymousToken				advapi32	0		# KernelBase
imp	'ImpersonateDdeClientWindow'				ImpersonateDdeClientWindow				user32		2023
imp	'ImpersonateLoggedOnUser'				ImpersonateLoggedOnUser					advapi32	0		# KernelBase
imp	'ImpersonateNamedPipeClient'				ImpersonateNamedPipeClient				advapi32	0		# KernelBase
imp	'ImpersonateSelf'					ImpersonateSelf						advapi32	0		# KernelBase
imp	'InSendMessage'						InSendMessage						user32		2024
imp	'InSendMessageEx'					InSendMessageEx						user32		2025
imp	'IncrementPackageStatusVersion'				IncrementPackageStatusVersion				KernelBase	847
imp	'InetIsOffline'						InetIsOffline						url		106
imp	'InetNtop'						InetNtopW						ws2_32		35
imp	'InetPton'						InetPtonW						ws2_32		36
imp	'InflateRect'						InflateRect						user32		2026
imp	'InheritWindowMonitor'					InheritWindowMonitor					user32		2027
imp	'InitAtomTable'						InitAtomTable						kernel32	860
imp	'InitDManipHook'					InitDManipHook						user32		2028
imp	'InitNetworkAddressControl'				InitNetworkAddressControl				shell32		306
imp	'InitOnceBeginInitialize'				InitOnceBeginInitialize					kernel32	0		# KernelBase
imp	'InitOnceComplete'					InitOnceComplete					kernel32	0		# KernelBase
imp	'InitOnceExecuteOnce'					InitOnceExecuteOnce					kernel32	0		# KernelBase
imp	'InitializeAcl'						InitializeAcl						advapi32	0		# KernelBase
imp	'InitializeContext'					InitializeContext					kernel32	0		# KernelBase
imp	'InitializeCriticalSectionAndSpinCount'			InitializeCriticalSectionAndSpinCount			kernel32	0	2	# KernelBase
imp	'InitializeCriticalSectionEx'				InitializeCriticalSectionEx				kernel32	0		# KernelBase
imp	'InitializeEnclave'					InitializeEnclave					KernelBase	858
imp	'InitializeGenericHidInjection'				InitializeGenericHidInjection				user32		2029
imp	'InitializeInputDeviceInjection'			InitializeInputDeviceInjection				user32		2030
imp	'InitializeLpkHooks'					InitializeLpkHooks					user32		2031
imp	'InitializePointerDeviceInjection'			InitializePointerDeviceInjection			user32		2032
imp	'InitializePointerDeviceInjectionEx'			InitializePointerDeviceInjectionEx			user32		2033
imp	'InitializeProcThreadAttributeList'			InitializeProcThreadAttributeList			kernel32	0	4	# KernelBase
imp	'InitializeProcessForWsWatch'				InitializeProcessForWsWatch				KernelBase	860
imp	'InitializeSecurityDescriptor'				InitializeSecurityDescriptor				advapi32	0		# KernelBase
imp	'InitializeSid'						InitializeSid						advapi32	0		# KernelBase
imp	'InitializeSynchronizationBarrier'			InitializeSynchronizationBarrier			kernel32	0		# KernelBase
imp	'InitializeTouchInjection'				InitializeTouchInjection				user32		2034
imp	'InitiateShutdownA'					InitiateShutdownA					advapi32	1402
imp	'InitiateShutdown'					InitiateShutdownW					advapi32	1403
imp	'InitiateSystemShutdownA'				InitiateSystemShutdownA					advapi32	1404
imp	'InitiateSystemShutdownExA'				InitiateSystemShutdownExA				advapi32	1405
imp	'InitiateSystemShutdownEx'				InitiateSystemShutdownExW				advapi32	1406
imp	'InitiateSystemShutdown'				InitiateSystemShutdownW					advapi32	1407
imp	'InjectDeviceInput'					InjectDeviceInput					user32		2035
imp	'InjectGenericHidInput'					InjectGenericHidInput					user32		2036
imp	'InjectKeyboardInput'					InjectKeyboardInput					user32		2037
imp	'InjectMouseInput'					InjectMouseInput					user32		2038
imp	'InjectPointerInput'					InjectPointerInput					user32		2039
imp	'InjectTouchInput'					InjectTouchInput					user32		2040
imp	'InsertMenuA'						InsertMenuA						user32		2041	5
imp	'InsertMenuItemA'					InsertMenuItemA						user32		2042
imp	'InsertMenuItem'					InsertMenuItemW						user32		2043
imp	'InsertMenu'						InsertMenuW						user32		2044	5
imp	'InstallApplication'					InstallApplication					advapi32	1408
imp	'InstallELAMCertificateInfo'				InstallELAMCertificateInfo				kernel32	0		# KernelBase
imp	'InternalDeleteDC'					InternalDeleteDC					gdi32		1736
imp	'InternalExtractIconListA'				InternalExtractIconListA				shell32		307
imp	'InternalExtractIconList'				InternalExtractIconListW				shell32		308
imp	'InternalGetWindowIcon'					InternalGetWindowIcon					user32		2045
imp	'InternalGetWindowText'					InternalGetWindowText					user32		2046
imp	'InternalLcidToName'					InternalLcidToName					KernelBase	872
imp	'Internal_EnumCalendarInfo'				Internal_EnumCalendarInfo				KernelBase	873
imp	'Internal_EnumDateFormats'				Internal_EnumDateFormats				KernelBase	874
imp	'Internal_EnumLanguageGroupLocales'			Internal_EnumLanguageGroupLocales			KernelBase	875
imp	'Internal_EnumSystemCodePages'				Internal_EnumSystemCodePages				KernelBase	876
imp	'Internal_EnumSystemLanguageGroups'			Internal_EnumSystemLanguageGroups			KernelBase	877
imp	'Internal_EnumSystemLocales'				Internal_EnumSystemLocales				KernelBase	878
imp	'Internal_EnumTimeFormats'				Internal_EnumTimeFormats				KernelBase	879
imp	'Internal_EnumUILanguages'				Internal_EnumUILanguages				KernelBase	880
imp	'InternetTimeFromSystemTimeA'				InternetTimeFromSystemTimeA				KernelBase	881
imp	'InternetTimeFromSystemTime'				InternetTimeFromSystemTimeW				KernelBase	882
imp	'InternetTimeToSystemTimeA'				InternetTimeToSystemTimeA				KernelBase	883
imp	'InternetTimeToSystemTime'				InternetTimeToSystemTimeW				KernelBase	884
imp	'IntersectClipRect'					IntersectClipRect					gdi32		1737
imp	'IntersectRect'						IntersectRect						user32		2047
imp	'InvalidateAppModelVersionCache'			InvalidateAppModelVersionCache				KernelBase	885
imp	'InvalidateConsoleDIBits'				InvalidateConsoleDIBits					kernel32	881
imp	'InvalidateRect'					InvalidateRect						user32		2048
imp	'InvalidateRgn'						InvalidateRgn						user32		2049
imp	'InvertRect'						InvertRect						user32		2050
imp	'InvertRgn'						InvertRgn						gdi32		1738
imp	'IsBadCodePtr'						IsBadCodePtr						kernel32	882
imp	'IsBadHugeReadPtr'					IsBadHugeReadPtr					kernel32	883
imp	'IsBadHugeWritePtr'					IsBadHugeWritePtr					kernel32	884
imp	'IsBadReadPtr'						IsBadReadPtr						kernel32	885
imp	'IsBadStringPtrA'					IsBadStringPtrA						kernel32	886
imp	'IsBadStringPtr'					IsBadStringPtrW						kernel32	887
imp	'IsBadWritePtr'						IsBadWritePtr						kernel32	888
imp	'IsCalendarLeapDay'					IsCalendarLeapDay					kernel32	889
imp	'IsCalendarLeapMonth'					IsCalendarLeapMonth					kernel32	890
imp	'IsCalendarLeapYear'					IsCalendarLeapYear					kernel32	891
imp	'IsCharAlphaA'						IsCharAlphaA						KernelBase	886
imp	'IsCharAlphaNumericA'					IsCharAlphaNumericA					KernelBase	887
imp	'IsCharAlphaNumeric'					IsCharAlphaNumericW					KernelBase	888
imp	'IsCharAlpha'						IsCharAlphaW						KernelBase	889
imp	'IsCharBlank'						IsCharBlankW						KernelBase	890
imp	'IsCharCntrl'						IsCharCntrlW						KernelBase	891
imp	'IsCharDigit'						IsCharDigitW						KernelBase	892
imp	'IsCharLowerA'						IsCharLowerA						KernelBase	893
imp	'IsCharLower'						IsCharLowerW						KernelBase	894
imp	'IsCharPunct'						IsCharPunctW						KernelBase	895
imp	'IsCharSpaceA'						IsCharSpaceA						KernelBase	896
imp	'IsCharSpace'						IsCharSpaceW						KernelBase	897
imp	'IsCharUpperA'						IsCharUpperA						KernelBase	898
imp	'IsCharUpper'						IsCharUpperW						KernelBase	899
imp	'IsCharXDigit'						IsCharXDigitW						KernelBase	900
imp	'IsChild'						IsChild							user32		2059	2
imp	'IsClipboardFormatAvailable'				IsClipboardFormatAvailable				user32		2060
imp	'IsDBCSLeadByte'					IsDBCSLeadByte						kernel32	0		# KernelBase
imp	'IsDBCSLeadByteEx'					IsDBCSLeadByteEx					kernel32	0		# KernelBase
imp	'IsDebuggerPresent$nt'					IsDebuggerPresent					KernelBase	903
imp	'IsDesktopExplorerProcess'				IsDesktopExplorerProcess				shell32		942
imp	'IsDeveloperModeEnabled'				IsDeveloperModeEnabled					KernelBase	904
imp	'IsDeveloperModePolicyApplied'				IsDeveloperModePolicyApplied				KernelBase	905
imp	'IsDialogMessageA'					IsDialogMessageA					user32		2062
imp	'IsDialogMessage'					IsDialogMessageW					user32		2063
imp	'IsDlgButtonChecked'					IsDlgButtonChecked					user32		2064
imp	'IsEnclaveTypeSupported'				IsEnclaveTypeSupported					KernelBase	906
imp	'IsGUIThread'						IsGUIThread						user32		2065
imp	'IsGlobalizationUserSettingsKeyRedirected'		IsGlobalizationUserSettingsKeyRedirected		KernelBase	907
imp	'IsHungAppWindow'					IsHungAppWindow						user32		2066
imp	'IsIconic'						IsIconic						user32		2067	1
imp	'IsImmersiveProcess'					IsImmersiveProcess					user32		2068
imp	'IsInDesktopWindowBand'					IsInDesktopWindowBand					user32		2069
imp	'IsInternetESCEnabled'					IsInternetESCEnabled					KernelBase	908
imp	'IsLFNDriveA'						IsLFNDriveA						shell32		41
imp	'IsLFNDrive'						IsLFNDriveW						shell32		42
imp	'IsMenu'						IsMenu							user32		2070	1
imp	'IsMouseInPointerEnabled'				IsMouseInPointerEnabled					user32		2071
imp	'IsNLSDefinedString'					IsNLSDefinedString					kernel32	0		# KernelBase
imp	'IsNativeVhdBoot'					IsNativeVhdBoot						kernel32	897
imp	'IsNetDrive'						IsNetDrive						shell32		66
imp	'IsNormalizedString'					IsNormalizedString					kernel32	0		# KernelBase
imp	'IsOnDemandRegistrationSupportedForExtensionCategory'	IsOnDemandRegistrationSupportedForExtensionCategory	KernelBase	911
imp	'IsOneCoreTransformMode'				IsOneCoreTransformMode					user32		2072
imp	'IsProcessAnExplorer'					IsProcessAnExplorer					shell32		941
imp	'IsProcessCritical'					IsProcessCritical					kernel32	0		# KernelBase
imp	'IsProcessDPIAware'					IsProcessDPIAware					user32		2073
imp	'IsProcessInJob'					IsProcessInJob						kernel32	0		# KernelBase
imp	'IsProcessorFeaturePresent'				IsProcessorFeaturePresent				kernel32	0		# KernelBase
imp	'IsQueueAttached'					IsQueueAttached						user32		2074
imp	'IsRectEmpty'						IsRectEmpty						user32		2075
imp	'IsSETEnabled'						IsSETEnabled						user32		2076
imp	'IsServerSideWindow'					IsServerSideWindow					user32		2077
imp	'IsSideloadingEnabled'					IsSideloadingEnabled					KernelBase	915
imp	'IsSideloadingPolicyApplied'				IsSideloadingPolicyApplied				KernelBase	916
imp	'IsSyncForegroundPolicyRefresh'				IsSyncForegroundPolicyRefresh				KernelBase	917
imp	'IsSystemResumeAutomatic'				IsSystemResumeAutomatic					kernel32	902
imp	'IsTextUnicode'						IsTextUnicode						advapi32	1409
imp	'IsThreadAFiber'					IsThreadAFiber						kernel32	0		# KernelBase
imp	'IsThreadDesktopComposited'				IsThreadDesktopComposited				user32		2078
imp	'IsThreadMessageQueueAttached'				IsThreadMessageQueueAttached				user32		2528
imp	'IsThreadTSFEventAware'					IsThreadTSFEventAware					user32		2079
imp	'IsTimeZoneRedirectionEnabled'				IsTimeZoneRedirectionEnabled				KernelBase	920
imp	'IsTokenRestricted'					IsTokenRestricted					advapi32	0		# KernelBase
imp	'IsTokenUntrusted'					IsTokenUntrusted					advapi32	1411
imp	'IsTopLevelWindow'					IsTopLevelWindow					user32		2080
imp	'IsTouchWindow'						IsTouchWindow						user32		2081
imp	'IsUserAnAdmin'						IsUserAnAdmin						shell32		680
imp	'IsValidAcl'						IsValidAcl						advapi32	0		# KernelBase
imp	'IsValidCalDateTime'					IsValidCalDateTime					kernel32	905
imp	'IsValidCodePage'					IsValidCodePage						kernel32	0		# KernelBase
imp	'IsValidDpiAwarenessContext'				IsValidDpiAwarenessContext				user32		2082
imp	'IsValidEnhMetaRecord'					IsValidEnhMetaRecord					gdi32		1739
imp	'IsValidEnhMetaRecordOffExt'				IsValidEnhMetaRecordOffExt				gdi32		1740
imp	'IsValidLanguageGroup'					IsValidLanguageGroup					kernel32	0		# KernelBase
imp	'IsValidLocale'						IsValidLocale						kernel32	0		# KernelBase
imp	'IsValidLocaleName'					IsValidLocaleName					kernel32	0		# KernelBase
imp	'IsValidNLSVersion'					IsValidNLSVersion					kernel32	0		# KernelBase
imp	'IsValidRelativeSecurityDescriptor'			IsValidRelativeSecurityDescriptor			KernelBase	928
imp	'IsValidSecurityDescriptor'				IsValidSecurityDescriptor				advapi32	0		# KernelBase
imp	'IsValidSid'						IsValidSid						advapi32	0		# KernelBase
imp	'IsWellKnownSid'					IsWellKnownSid						advapi32	0		# KernelBase
imp	'IsWinEventHookInstalled'				IsWinEventHookInstalled					user32		2083
imp	'IsWindow'						IsWindow						user32		2084	1
imp	'IsWindowArranged'					IsWindowArranged					user32		2085
imp	'IsWindowEnabled'					IsWindowEnabled						user32		2086
imp	'IsWindowInDestroy'					IsWindowInDestroy					user32		2087
imp	'IsWindowRedirectedForPrint'				IsWindowRedirectedForPrint				user32		2088
imp	'IsWindowUnicode'					IsWindowUnicode						user32		2089
imp	'IsWindowVisible'					IsWindowVisible						user32		2090	1
imp	'IsWow64GuestMachineSupported'				IsWow64GuestMachineSupported				KernelBase	932
imp	'IsWow64Message'					IsWow64Message						user32		2091
imp	'IsWow64Process'					IsWow64Process						kernel32	0		# KernelBase
imp	'IsWow64Process2'					IsWow64Process2						KernelBase	934
imp	'IsZoomed'						IsZoomed						user32		2092	1
imp	'K32EmptyWorkingSet'					K32EmptyWorkingSet					kernel32	0		# KernelBase
imp	'K32EnumDeviceDrivers'					K32EnumDeviceDrivers					kernel32	0		# KernelBase
imp	'K32EnumPageFilesA'					K32EnumPageFilesA					kernel32	0		# KernelBase
imp	'K32EnumPageFiles'					K32EnumPageFilesW					kernel32	0		# KernelBase
imp	'K32EnumProcessModules'					K32EnumProcessModules					kernel32	0		# KernelBase
imp	'K32EnumProcessModulesEx'				K32EnumProcessModulesEx					kernel32	0		# KernelBase
imp	'K32EnumProcesses'					K32EnumProcesses					kernel32	0		# KernelBase
imp	'K32GetDeviceDriverBaseNameA'				K32GetDeviceDriverBaseNameA				kernel32	0		# KernelBase
imp	'K32GetDeviceDriverBaseName'				K32GetDeviceDriverBaseNameW				kernel32	0		# KernelBase
imp	'K32GetDeviceDriverFileNameA'				K32GetDeviceDriverFileNameA				kernel32	0		# KernelBase
imp	'K32GetDeviceDriverFileName'				K32GetDeviceDriverFileNameW				kernel32	0		# KernelBase
imp	'K32GetMappedFileNameA'					K32GetMappedFileNameA					kernel32	0		# KernelBase
imp	'K32GetMappedFileName'					K32GetMappedFileNameW					kernel32	0		# KernelBase
imp	'K32GetModuleBaseNameA'					K32GetModuleBaseNameA					kernel32	0		# KernelBase
imp	'K32GetModuleBaseName'					K32GetModuleBaseNameW					kernel32	0		# KernelBase
imp	'K32GetModuleFileNameExA'				K32GetModuleFileNameExA					kernel32	0		# KernelBase
imp	'K32GetModuleFileNameEx'				K32GetModuleFileNameExW					kernel32	0		# KernelBase
imp	'K32GetModuleInformation'				K32GetModuleInformation					kernel32	0		# KernelBase
imp	'K32GetPerformanceInfo'					K32GetPerformanceInfo					kernel32	0		# KernelBase
imp	'K32GetProcessImageFileNameA'				K32GetProcessImageFileNameA				kernel32	0		# KernelBase
imp	'K32GetProcessImageFileName'				K32GetProcessImageFileNameW				kernel32	0		# KernelBase
imp	'K32GetProcessMemoryInfo'				K32GetProcessMemoryInfo					kernel32	0		# KernelBase
imp	'K32GetWsChanges'					K32GetWsChanges						kernel32	0		# KernelBase
imp	'K32GetWsChangesEx'					K32GetWsChangesEx					kernel32	0		# KernelBase
imp	'K32InitializeProcessForWsWatch'			K32InitializeProcessForWsWatch				kernel32	0		# KernelBase
imp	'K32QueryWorkingSet'					K32QueryWorkingSet					kernel32	0		# KernelBase
imp	'K32QueryWorkingSetEx'					K32QueryWorkingSetEx					kernel32	0		# KernelBase
imp	'KernelBaseGetGlobalData'				KernelBaseGetGlobalData					KernelBase	962
imp	'KernelbasePostInit'					KernelbasePostInit					KernelBase	963
imp	'KiRaiseUserExceptionDispatcher'			KiRaiseUserExceptionDispatcher				ntdll		103
imp	'KiUserApcDispatcher'					KiUserApcDispatcher					ntdll		104
imp	'KiUserCallbackDispatcher'				KiUserCallbackDispatcher				ntdll		105
imp	'KiUserExceptionDispatcher'				KiUserExceptionDispatcher				ntdll		106
imp	'KiUserInvertedFunctionTable'				KiUserInvertedFunctionTable				ntdll		107
imp	'KillTimer'						KillTimer						user32		2093	2
imp	'LCIDToLocaleName'					LCIDToLocaleName					kernel32	0		# KernelBase
imp	'LCMapStringA'						LCMapStringA						kernel32	0		# KernelBase
imp	'LCMapStringEx'						LCMapStringEx						kernel32	0		# KernelBase
imp	'LCMapString'						LCMapStringW						kernel32	0		# KernelBase
imp	'LPtoDP'						LPtoDP							gdi32		1741
imp	'LZClose'						LZClose							kernel32	945
imp	'LZCloseFile'						LZCloseFile						kernel32	946
imp	'LZCopy'						LZCopy							kernel32	947
imp	'LZCreateFile'						LZCreateFileW						kernel32	948
imp	'LZDone'						LZDone							kernel32	949
imp	'LZInit'						LZInit							kernel32	950
imp	'LZOpenFileA'						LZOpenFileA						kernel32	951
imp	'LZOpenFile'						LZOpenFileW						kernel32	952
imp	'LZRead'						LZRead							kernel32	953
imp	'LZSeek'						LZSeek							kernel32	954
imp	'LZStart'						LZStart							kernel32	955
imp	'LaunchMSHelp_RunDLLW'					LaunchMSHelp_RunDLLW					shell32		309
imp	'LdrAccessResource'					LdrAccessResource					ntdll		108
imp	'LdrAddDllDirectory'					LdrAddDllDirectory					ntdll		109
imp	'LdrAddLoadAsDataTable'					LdrAddLoadAsDataTable					ntdll		110
imp	'LdrAddRefDll'						LdrAddRefDll						ntdll		111
imp	'LdrAppxHandleIntegrityFailure'				LdrAppxHandleIntegrityFailure				ntdll		112
imp	'LdrCallEnclave'					LdrCallEnclave						ntdll		113
imp	'LdrControlFlowGuardEnforced'				LdrControlFlowGuardEnforced				ntdll		114
imp	'LdrCreateEnclave'					LdrCreateEnclave					ntdll		115
imp	'LdrDeleteEnclave'					LdrDeleteEnclave					ntdll		116
imp	'LdrDisableThreadCalloutsForDll'			LdrDisableThreadCalloutsForDll				ntdll		117
imp	'LdrEnumResources'					LdrEnumResources					ntdll		118
imp	'LdrEnumerateLoadedModules'				LdrEnumerateLoadedModules				ntdll		119
imp	'LdrFastFailInLoaderCallout'				LdrFastFailInLoaderCallout				ntdll		120
imp	'LdrFindEntryForAddress'				LdrFindEntryForAddress					ntdll		121
imp	'LdrFindResourceDirectory_U'				LdrFindResourceDirectory_U				ntdll		122
imp	'LdrFindResourceEx_U'					LdrFindResourceEx_U					ntdll		123
imp	'LdrFindResource_U'					LdrFindResource_U					ntdll		124
imp	'LdrFlushAlternateResourceModules'			LdrFlushAlternateResourceModules			ntdll		125
imp	'LdrGetDllDirectory'					LdrGetDllDirectory					ntdll		126
imp	'LdrGetDllFullName'					LdrGetDllFullName					ntdll		127
imp	'LdrGetDllHandle'					LdrGetDllHandle						ntdll		128	4
imp	'LdrGetDllHandleByMapping'				LdrGetDllHandleByMapping				ntdll		129
imp	'LdrGetDllHandleByName'					LdrGetDllHandleByName					ntdll		130
imp	'LdrGetDllHandleEx'					LdrGetDllHandleEx					ntdll		131
imp	'LdrGetDllPath'						LdrGetDllPath						ntdll		132
imp	'LdrGetFailureData'					LdrGetFailureData					ntdll		133
imp	'LdrGetFileNameFromLoadAsDataTable'			LdrGetFileNameFromLoadAsDataTable			ntdll		134
imp	'LdrGetKnownDllSectionHandle'				LdrGetKnownDllSectionHandle				ntdll		135
imp	'LdrGetProcedureAddress'				LdrGetProcedureAddress					ntdll		136	4
imp	'LdrGetProcedureAddressEx'				LdrGetProcedureAddressEx				ntdll		137
imp	'LdrGetProcedureAddressForCaller'			LdrGetProcedureAddressForCaller				ntdll		138
imp	'LdrInitShimEngineDynamic'				LdrInitShimEngineDynamic				ntdll		139
imp	'LdrInitializeEnclave'					LdrInitializeEnclave					ntdll		140
imp	'LdrInitializeThunk'					LdrInitializeThunk					ntdll		141
imp	'LdrLoadAlternateResourceModule'			LdrLoadAlternateResourceModule				ntdll		142
imp	'LdrLoadAlternateResourceModuleEx'			LdrLoadAlternateResourceModuleEx			ntdll		143
imp	'LdrLoadDll'						LdrLoadDll						ntdll		144	4
imp	'LdrLoadEnclaveModule'					LdrLoadEnclaveModule					ntdll		145
imp	'LdrLockLoaderLock'					LdrLockLoaderLock					ntdll		146
imp	'LdrOpenImageFileOptionsKey'				LdrOpenImageFileOptionsKey				ntdll		147
imp	'LdrProcessInitializationComplete'			LdrProcessInitializationComplete			ntdll		148
imp	'LdrProcessRelocationBlock'				LdrProcessRelocationBlock				ntdll		149
imp	'LdrProcessRelocationBlockEx'				LdrProcessRelocationBlockEx				ntdll		150
imp	'LdrQueryImageFileExecutionOptions'			LdrQueryImageFileExecutionOptions			ntdll		151
imp	'LdrQueryImageFileExecutionOptionsEx'			LdrQueryImageFileExecutionOptionsEx			ntdll		152
imp	'LdrQueryImageFileKeyOption'				LdrQueryImageFileKeyOption				ntdll		153
imp	'LdrQueryModuleServiceTags'				LdrQueryModuleServiceTags				ntdll		154
imp	'LdrQueryOptionalDelayLoadedAPI'			LdrQueryOptionalDelayLoadedAPI				ntdll		155
imp	'LdrQueryProcessModuleInformation'			LdrQueryProcessModuleInformation			ntdll		156
imp	'LdrRegisterDllNotification'				LdrRegisterDllNotification				ntdll		157
imp	'LdrRemoveDllDirectory'					LdrRemoveDllDirectory					ntdll		158
imp	'LdrRemoveLoadAsDataTable'				LdrRemoveLoadAsDataTable				ntdll		159
imp	'LdrResFindResource'					LdrResFindResource					ntdll		160
imp	'LdrResFindResourceDirectory'				LdrResFindResourceDirectory				ntdll		161
imp	'LdrResGetRCConfig'					LdrResGetRCConfig					ntdll		162
imp	'LdrResRelease'						LdrResRelease						ntdll		163
imp	'LdrResSearchResource'					LdrResSearchResource					ntdll		164
imp	'LdrResolveDelayLoadedAPI'				LdrResolveDelayLoadedAPI				ntdll		165
imp	'LdrResolveDelayLoadsFromDll'				LdrResolveDelayLoadsFromDll				ntdll		166
imp	'LdrRscIsTypeExist'					LdrRscIsTypeExist					ntdll		167
imp	'LdrSetAppCompatDllRedirectionCallback'			LdrSetAppCompatDllRedirectionCallback			ntdll		168
imp	'LdrSetDefaultDllDirectories'				LdrSetDefaultDllDirectories				ntdll		169
imp	'LdrSetDllDirectory'					LdrSetDllDirectory					ntdll		170
imp	'LdrSetDllManifestProber'				LdrSetDllManifestProber					ntdll		171
imp	'LdrSetImplicitPathOptions'				LdrSetImplicitPathOptions				ntdll		172
imp	'LdrSetMUICacheType'					LdrSetMUICacheType					ntdll		173
imp	'LdrShutdownProcess'					LdrShutdownProcess					ntdll		174
imp	'LdrShutdownThread'					LdrShutdownThread					ntdll		175
imp	'LdrStandardizeSystemPath'				LdrStandardizeSystemPath				ntdll		176
imp	'LdrSystemDllInitBlock'					LdrSystemDllInitBlock					ntdll		177
imp	'LdrUnloadAlternateResourceModule'			LdrUnloadAlternateResourceModule			ntdll		178
imp	'LdrUnloadAlternateResourceModuleEx'			LdrUnloadAlternateResourceModuleEx			ntdll		179
imp	'LdrUnloadDll'						LdrUnloadDll						ntdll		180	1
imp	'LdrUnlockLoaderLock'					LdrUnlockLoaderLock					ntdll		181
imp	'LdrUnregisterDllNotification'				LdrUnregisterDllNotification				ntdll		182
imp	'LdrUpdatePackageSearchPath'				LdrUpdatePackageSearchPath				ntdll		183
imp	'LdrVerifyImageMatchesChecksum'				LdrVerifyImageMatchesChecksum				ntdll		184
imp	'LdrVerifyImageMatchesChecksumEx'			LdrVerifyImageMatchesChecksumEx				ntdll		185
imp	'LdrpResGetMappingSize'					LdrpResGetMappingSize					ntdll		186
imp	'LdrpResGetResourceDirectory'				LdrpResGetResourceDirectory				ntdll		187
imp	'LeaveCriticalPolicySectionInternal'			LeaveCriticalPolicySectionInternal			KernelBase	968
imp	'LineDDA'						LineDDA							gdi32		1742
imp	'LineTo'						LineTo							gdi32		1743
imp	'LoadAcceleratorsA'					LoadAcceleratorsA					user32		2094
imp	'LoadAccelerators'					LoadAcceleratorsW					user32		2095
imp	'LoadAlterBitmap'					LoadAlterBitmap						comdlg32	117
imp	'LoadAppInitDlls'					LoadAppInitDlls						KernelBase	971
imp	'LoadBitmapA'						LoadBitmapA						user32		2096
imp	'LoadBitmap'						LoadBitmapW						user32		2097
imp	'LoadCursorA'						LoadCursorA						user32		2098	2
imp	'LoadCursorFromFileA'					LoadCursorFromFileA					user32		2099
imp	'LoadCursorFromFile'					LoadCursorFromFileW					user32		2100
imp	'LoadCursor'						LoadCursorW						user32		2101	2
imp	'LoadEnclaveData'					LoadEnclaveData						KernelBase	972
imp	'LoadEnclaveImageA'					LoadEnclaveImageA					KernelBase	973
imp	'LoadEnclaveImage'					LoadEnclaveImageW					KernelBase	974
imp	'LoadIcon'						LoadIconW						user32		2103	2
imp	'LoadIconA'						LoadIconA						user32		2102	2
imp	'LoadImageA'						LoadImageA						user32		2104	6
imp	'LoadImage'						LoadImageW						user32		2105	6
imp	'LoadKeyboardLayoutA'					LoadKeyboardLayoutA					user32		2106
imp	'LoadKeyboardLayoutEx'					LoadKeyboardLayoutEx					user32		2107
imp	'LoadKeyboardLayout'					LoadKeyboardLayoutW					user32		2108
imp	'LoadLibrary'						LoadLibraryW						kernel32	0	1	# KernelBase
imp	'LoadLibraryA'						LoadLibraryA						kernel32	0	1	# KernelBase
imp	'LoadLibraryExA'					LoadLibraryExA						kernel32	0	3	# KernelBase
imp	'LoadLibraryEx'						LoadLibraryExW						kernel32	0	3	# KernelBase
imp	'LoadLocalFonts'					LoadLocalFonts						user32		2109
imp	'LoadMenuA'						LoadMenuA						user32		2110
imp	'LoadMenuIndirectA'					LoadMenuIndirectA					user32		2111
imp	'LoadMenuIndirect'					LoadMenuIndirectW					user32		2112
imp	'LoadMenu'						LoadMenuW						user32		2113
imp	'LoadModule'						LoadModule						kernel32	964
imp	'LoadPackagedLibrary'					LoadPackagedLibrary					kernel32	0		# KernelBase
imp	'LoadRemoteFonts'					LoadRemoteFonts						user32		2114
imp	'LoadResource'						LoadResource						kernel32	0	2	# KernelBase
imp	'LoadStringA'						LoadStringA						KernelBase	981
imp	'LoadStringBaseEx'					LoadStringBaseExW					KernelBase	982
imp	'LoadStringBase'					LoadStringBaseW						kernel32	968
imp	'LoadStringByReference'					LoadStringByReference					KernelBase	983
imp	'LoadString'						LoadStringW						KernelBase	984
imp	'LocalAlloc'						LocalAlloc						kernel32	0		# KernelBase
imp	'LocalCompact'						LocalCompact						kernel32	970
imp	'LocalFileTimeToFileTime'				LocalFileTimeToFileTime					kernel32	0		# KernelBase
imp	'LocalFlags'						LocalFlags						kernel32	972
imp	'LocalFree'						LocalFree						kernel32	0		# KernelBase
imp	'LocalHandle'						LocalHandle						kernel32	974
imp	'LocalLock'						LocalLock						kernel32	0		# KernelBase
imp	'LocalReAlloc'						LocalReAlloc						kernel32	0		# KernelBase
imp	'LocalShrink'						LocalShrink						kernel32	977
imp	'LocalSize'						LocalSize						kernel32	978
imp	'LocalUnlock'						LocalUnlock						kernel32	0		# KernelBase
imp	'LocaleNameToLCID'					LocaleNameToLCID					kernel32	0		# KernelBase
imp	'LocateXStateFeature'					LocateXStateFeature					kernel32	0		# KernelBase
imp	'LockFile'						LockFile						kernel32	0	5	# KernelBase
imp	'LockFileEx'						LockFileEx						kernel32	0	6	# KernelBase
imp	'LockResource'						LockResource						kernel32	0	1	# KernelBase
imp	'LockServiceDatabase'					LockServiceDatabase					advapi32	1417
imp	'LockSetForegroundWindow'				LockSetForegroundWindow					user32		2117
imp	'LockWindowStation'					LockWindowStation					user32		2118
imp	'LockWindowUpdate'					LockWindowUpdate					user32		2119
imp	'LockWorkStation'					LockWorkStation						user32		2120
imp	'LogicalToPhysicalPoint'				LogicalToPhysicalPoint					user32		2121
imp	'LogicalToPhysicalPointForPerMonitorDPI'		LogicalToPhysicalPointForPerMonitorDPI			user32		2122
imp	'LogonUserA'						LogonUserA						advapi32	1418
imp	'LogonUserExA'						LogonUserExA						advapi32	1419
imp	'LogonUserExEx'						LogonUserExExW						advapi32	1420
imp	'LogonUserEx'						LogonUserExW						advapi32	1421
imp	'LogonUser'						LogonUserW						advapi32	1422
imp	'LookupAccountNameA'					LookupAccountNameA					advapi32	1423
imp	'LookupAccountName'					LookupAccountNameW					advapi32	1424
imp	'LookupAccountSidA'					LookupAccountSidA					advapi32	1425
imp	'LookupAccountSid'					LookupAccountSidW					advapi32	1426
imp	'LookupIconIdFromDirectory'				LookupIconIdFromDirectory				user32		2123
imp	'LookupIconIdFromDirectoryEx'				LookupIconIdFromDirectoryEx				user32		2124
imp	'LookupPrivilegeDisplayNameA'				LookupPrivilegeDisplayNameA				advapi32	1427
imp	'LookupPrivilegeDisplayName'				LookupPrivilegeDisplayNameW				advapi32	1428
imp	'LookupPrivilegeNameA'					LookupPrivilegeNameA					advapi32	1429
imp	'LookupPrivilegeName'					LookupPrivilegeNameW					advapi32	1430
imp	'LookupPrivilegeValue'					LookupPrivilegeValueW					advapi32	1432	3
imp	'LookupPrivilegeValueA'					LookupPrivilegeValueA					advapi32	1431	3
imp	'LookupSecurityDescriptorPartsA'			LookupSecurityDescriptorPartsA				advapi32	1433
imp	'LookupSecurityDescriptorParts'				LookupSecurityDescriptorPartsW				advapi32	1434
imp	'LpkEditControl'					LpkEditControl						gdi32		1745
imp	'LpkGetEditControl'					LpkGetEditControl					gdi32		1748
imp	'LpkpEditControlSize'					LpkpEditControlSize					gdi32		1755
imp	'LpkpInitializeEditControl'				LpkpInitializeEditControl				gdi32		1756
imp	'LsaAddAccountRights'					LsaAddAccountRights					advapi32	1435
imp	'LsaAddPrivilegesToAccount'				LsaAddPrivilegesToAccount				advapi32	1436
imp	'LsaClearAuditLog'					LsaClearAuditLog					advapi32	1437
imp	'LsaClose'						LsaClose						advapi32	1438
imp	'LsaCreateAccount'					LsaCreateAccount					advapi32	1439
imp	'LsaCreateSecret'					LsaCreateSecret						advapi32	1440
imp	'LsaCreateTrustedDomain'				LsaCreateTrustedDomain					advapi32	1441
imp	'LsaCreateTrustedDomainEx'				LsaCreateTrustedDomainEx				advapi32	1442
imp	'LsaDelete'						LsaDelete						advapi32	1443
imp	'LsaDeleteTrustedDomain'				LsaDeleteTrustedDomain					advapi32	1444
imp	'LsaEnumerateAccountRights'				LsaEnumerateAccountRights				advapi32	1445
imp	'LsaEnumerateAccounts'					LsaEnumerateAccounts					advapi32	1446
imp	'LsaEnumerateAccountsWithUserRight'			LsaEnumerateAccountsWithUserRight			advapi32	1447
imp	'LsaEnumeratePrivileges'				LsaEnumeratePrivileges					advapi32	1448
imp	'LsaEnumeratePrivilegesOfAccount'			LsaEnumeratePrivilegesOfAccount				advapi32	1449
imp	'LsaEnumerateTrustedDomains'				LsaEnumerateTrustedDomains				advapi32	1450
imp	'LsaEnumerateTrustedDomainsEx'				LsaEnumerateTrustedDomainsEx				advapi32	1451
imp	'LsaFreeMemory'						LsaFreeMemory						advapi32	1452
imp	'LsaGetAppliedCAPIDs'					LsaGetAppliedCAPIDs					advapi32	1453
imp	'LsaGetQuotasForAccount'				LsaGetQuotasForAccount					advapi32	1454
imp	'LsaGetRemoteUserName'					LsaGetRemoteUserName					advapi32	1455
imp	'LsaGetSystemAccessAccount'				LsaGetSystemAccessAccount				advapi32	1456
imp	'LsaGetUserName'					LsaGetUserName						advapi32	1457
imp	'LsaICLookupNames'					LsaICLookupNames					advapi32	1458
imp	'LsaICLookupNamesWithCreds'				LsaICLookupNamesWithCreds				advapi32	1459
imp	'LsaICLookupSids'					LsaICLookupSids						advapi32	1460
imp	'LsaICLookupSidsWithCreds'				LsaICLookupSidsWithCreds				advapi32	1461
imp	'LsaLookupNames'					LsaLookupNames						advapi32	1462
imp	'LsaLookupNames2'					LsaLookupNames2						advapi32	1463
imp	'LsaLookupPrivilegeDisplayName'				LsaLookupPrivilegeDisplayName				advapi32	1464
imp	'LsaLookupPrivilegeName'				LsaLookupPrivilegeName					advapi32	1465
imp	'LsaLookupPrivilegeValue'				LsaLookupPrivilegeValue					advapi32	1466
imp	'LsaLookupSids'						LsaLookupSids						advapi32	1467
imp	'LsaLookupSids2'					LsaLookupSids2						advapi32	1468
imp	'LsaManageSidNameMapping'				LsaManageSidNameMapping					advapi32	1469
imp	'LsaNtStatusToWinError'					LsaNtStatusToWinError					advapi32	1470
imp	'LsaOpenAccount'					LsaOpenAccount						advapi32	1471
imp	'LsaOpenPolicy'						LsaOpenPolicy						advapi32	1472
imp	'LsaOpenPolicySce'					LsaOpenPolicySce					advapi32	1473
imp	'LsaOpenSecret'						LsaOpenSecret						advapi32	1474
imp	'LsaOpenTrustedDomain'					LsaOpenTrustedDomain					advapi32	1475
imp	'LsaOpenTrustedDomainByName'				LsaOpenTrustedDomainByName				advapi32	1476
imp	'LsaQueryCAPs'						LsaQueryCAPs						advapi32	1477
imp	'LsaQueryDomainInformationPolicy'			LsaQueryDomainInformationPolicy				advapi32	1478
imp	'LsaQueryForestTrustInformation'			LsaQueryForestTrustInformation				advapi32	1479
imp	'LsaQueryInfoTrustedDomain'				LsaQueryInfoTrustedDomain				advapi32	1480
imp	'LsaQueryInformationPolicy'				LsaQueryInformationPolicy				advapi32	1481
imp	'LsaQuerySecret'					LsaQuerySecret						advapi32	1482
imp	'LsaQuerySecurityObject'				LsaQuerySecurityObject					advapi32	1483
imp	'LsaQueryTrustedDomainInfo'				LsaQueryTrustedDomainInfo				advapi32	1484
imp	'LsaQueryTrustedDomainInfoByName'			LsaQueryTrustedDomainInfoByName				advapi32	1485
imp	'LsaRemoveAccountRights'				LsaRemoveAccountRights					advapi32	1486
imp	'LsaRemovePrivilegesFromAccount'			LsaRemovePrivilegesFromAccount				advapi32	1487
imp	'LsaRetrievePrivateData'				LsaRetrievePrivateData					advapi32	1488
imp	'LsaSetCAPs'						LsaSetCAPs						advapi32	1489
imp	'LsaSetDomainInformationPolicy'				LsaSetDomainInformationPolicy				advapi32	1490
imp	'LsaSetForestTrustInformation'				LsaSetForestTrustInformation				advapi32	1491
imp	'LsaSetInformationPolicy'				LsaSetInformationPolicy					advapi32	1492
imp	'LsaSetInformationTrustedDomain'			LsaSetInformationTrustedDomain				advapi32	1493
imp	'LsaSetQuotasForAccount'				LsaSetQuotasForAccount					advapi32	1494
imp	'LsaSetSecret'						LsaSetSecret						advapi32	1495
imp	'LsaSetSecurityObject'					LsaSetSecurityObject					advapi32	1496
imp	'LsaSetSystemAccessAccount'				LsaSetSystemAccessAccount				advapi32	1497
imp	'LsaSetTrustedDomainInfoByName'				LsaSetTrustedDomainInfoByName				advapi32	1498
imp	'LsaSetTrustedDomainInformation'			LsaSetTrustedDomainInformation				advapi32	1499
imp	'LsaStorePrivateData'					LsaStorePrivateData					advapi32	1500
imp	'MBToWCSEx'						MBToWCSEx						user32		2125
imp	'MBToWCSExt'						MBToWCSExt						user32		2126
imp	'MB_GetString'						MB_GetString						user32		2127
imp	'MD4Final'						MD4Final						ntdll		188
imp	'MD4Init'						MD4Init							ntdll		189
imp	'MD4Update'						MD4Update						ntdll		190
imp	'MD5Final$nt'						MD5Final						ntdll		191
imp	'MD5Init$nt'						MD5Init							ntdll		192
imp	'MD5Update$nt'						MD5Update						ntdll		193
imp	'MIDL_user_free_Ext'					MIDL_user_free_Ext					advapi32	1507
imp	'MIMEAssociationDialogA'				MIMEAssociationDialogA					url		107
imp	'MIMEAssociationDialog'					MIMEAssociationDialogW					url		108
imp	'MITActivateInputProcessing'				MITActivateInputProcessing				user32		2128
imp	'MITBindInputTypeToMonitors'				MITBindInputTypeToMonitors				user32		2129
imp	'MITCoreMsgKGetConnectionHandle'			MITCoreMsgKGetConnectionHandle				user32		2130
imp	'MITCoreMsgKOpenConnectionTo'				MITCoreMsgKOpenConnectionTo				user32		2131
imp	'MITCoreMsgKSend'					MITCoreMsgKSend						user32		2132
imp	'MITDeactivateInputProcessing'				MITDeactivateInputProcessing				user32		2133
imp	'MITDisableMouseIntercept'				MITDisableMouseIntercept				user32		2134
imp	'MITDispatchCompletion'					MITDispatchCompletion					user32		2135
imp	'MITEnableMouseIntercept'				MITEnableMouseIntercept					user32		2136
imp	'MITGetCursorUpdateHandle'				MITGetCursorUpdateHandle				user32		2137
imp	'MITInjectLegacyISMTouchFrame'				MITInjectLegacyISMTouchFrame				user32		2138
imp	'MITRegisterManipulationThread'				MITRegisterManipulationThread				user32		2139
imp	'MITSetForegroundRoutingInfo'				MITSetForegroundRoutingInfo				user32		2140
imp	'MITSetInputCallbacks'					MITSetInputCallbacks					user32		2141
imp	'MITSetInputDelegationMode'				MITSetInputDelegationMode				user32		2142
imp	'MITSetLastInputRecipient'				MITSetLastInputRecipient				user32		2143
imp	'MITSetManipulationInputTarget'				MITSetManipulationInputTarget				user32		2144
imp	'MITStopAndEndInertia'					MITStopAndEndInertia					user32		2145
imp	'MITSynthesizeMouseInput'				MITSynthesizeMouseInput					user32		2146
imp	'MITSynthesizeMouseWheel'				MITSynthesizeMouseWheel					user32		2147
imp	'MITSynthesizeTouchInput'				MITSynthesizeTouchInput					user32		2148
imp	'MITUpdateInputGlobals'					MITUpdateInputGlobals					user32		2149
imp	'MITWaitForMultipleObjectsEx'				MITWaitForMultipleObjectsEx				user32		2150
imp	'MSChapSrvChangePassword'				MSChapSrvChangePassword					advapi32	1508
imp	'MSChapSrvChangePassword2'				MSChapSrvChangePassword2				advapi32	1509
imp	'MailToProtocolHandler'					MailToProtocolHandler					url		109
imp	'MailToProtocolHandlerA'				MailToProtocolHandlerA					url		110
imp	'MakeAbsoluteSD'					MakeAbsoluteSD						advapi32	0		# KernelBase
imp	'MakeAbsoluteSD2'					MakeAbsoluteSD2						KernelBase	997
imp	'MakeSelfRelativeSD'					MakeSelfRelativeSD					advapi32	0		# KernelBase
imp	'MakeThreadTSFEventAware'				MakeThreadTSFEventAware					user32		2151
imp	'MapDialogRect'						MapDialogRect						user32		2152
imp	'MapGenericMask'					MapGenericMask						advapi32	0	2	# KernelBase
imp	'MapPredefinedHandleInternal'				MapPredefinedHandleInternal				KernelBase	1000
imp	'MapUserPhysicalPages'					MapUserPhysicalPages					kernel32	0		# KernelBase
imp	'MapUserPhysicalPagesScatter'				MapUserPhysicalPagesScatter				kernel32	986
imp	'MapViewOfFile'						MapViewOfFile						kernel32	0		# KernelBase
imp	'MapViewOfFile3'					MapViewOfFile3						KernelBase	1003
imp	'MapViewOfFile3FromApp'					MapViewOfFile3FromApp					KernelBase	1004
imp	'MapViewOfFileEx'					MapViewOfFileEx						kernel32	0		# KernelBase
imp	'MapViewOfFileExNuma'					MapViewOfFileExNuma					kernel32	0	7	# KernelBase
imp	'MapViewOfFileFromApp'					MapViewOfFileFromApp					kernel32	0		# KernelBase
imp	'MapViewOfFileNuma2'					MapViewOfFileNuma2					KernelBase	1008
imp	'MapVirtualKeyA'					MapVirtualKeyA						user32		2153
imp	'MapVirtualKeyExA'					MapVirtualKeyExA					user32		2154
imp	'MapVirtualKeyEx'					MapVirtualKeyExW					user32		2155
imp	'MapVirtualKey'						MapVirtualKeyW						user32		2156
imp	'MapVisualRelativePoints'				MapVisualRelativePoints					user32		2157
imp	'MapWindowPoints'					MapWindowPoints						user32		2158
imp	'MaskBlt'						MaskBlt							gdi32		1757
imp	'MenuItemFromPoint'					MenuItemFromPoint					user32		2159
imp	'MenuWindowProcA'					MenuWindowProcA						user32		2160
imp	'MenuWindowProc'					MenuWindowProcW						user32		2161
imp	'MessageBeep'						MessageBeep						user32		2162
imp	'MessageBox'						MessageBoxW						user32		2170	4
imp	'MessageBoxA'						MessageBoxA						user32		2163	4
imp	'MessageBoxEx'						MessageBoxExW						user32		2165	5
imp	'MessageBoxExA'						MessageBoxExA						user32		2164	5
imp	'MessageBoxIndirectA'					MessageBoxIndirectA					user32		2166
imp	'MessageBoxIndirect'					MessageBoxIndirectW					user32		2167
imp	'MessageBoxTimeoutA'					MessageBoxTimeoutA					user32		2168
imp	'MessageBoxTimeout'					MessageBoxTimeoutW					user32		2169
imp	'MirrorRgn'						MirrorRgn						gdi32		1758
imp	'ModerncoreGdiInit'					ModerncoreGdiInit					gdi32		1759
imp	'ModifyMenuA'						ModifyMenuA						user32		2171
imp	'ModifyMenu'						ModifyMenuW						user32		2172
imp	'ModifyWorldTransform'					ModifyWorldTransform					gdi32		1760
imp	'Module32First'						Module32FirstW						kernel32	992
imp	'Module32Next'						Module32NextW						kernel32	994
imp	'MonitorFromPoint'					MonitorFromPoint					user32		2173
imp	'MonitorFromRect'					MonitorFromRect						user32		2174
imp	'MonitorFromWindow'					MonitorFromWindow					user32		2175
imp	'MoveFile'						MoveFileW						kernel32	1000	2
imp	'MoveFileA'						MoveFileA						kernel32	995	2
imp	'MoveFileEx'						MoveFileExW						kernel32	0	3	# KernelBase
imp	'MoveFileExA'						MoveFileExA						kernel32	996	3
imp	'MoveFileTransactedA'					MoveFileTransactedA					kernel32	998
imp	'MoveFileTransacted'					MoveFileTransactedW					kernel32	999
imp	'MoveFileWithProgressA'					MoveFileWithProgressA					kernel32	1001
imp	'MoveFileWithProgressTransacted'			MoveFileWithProgressTransactedW				KernelBase	1010
imp	'MoveFileWithProgress'					MoveFileWithProgressW					kernel32	0		# KernelBase
imp	'MoveToEx'						MoveToEx						gdi32		1761
imp	'MoveWindow'						MoveWindow						user32		2176	6
imp	'MsgWaitForMultipleObjects'				MsgWaitForMultipleObjects				user32		2177
imp	'MsgWaitForMultipleObjectsEx'				MsgWaitForMultipleObjectsEx				user32		2178
imp	'MulDiv'						MulDiv							kernel32	0		# KernelBase
imp	'MultiByteToWideChar'					MultiByteToWideChar					kernel32	0		# KernelBase
imp	'NamedEscape'						NamedEscape						gdi32		1762
imp	'NamedPipeEventEnum'					NamedPipeEventEnum					KernelBase	1014
imp	'NamedPipeEventSelect'					NamedPipeEventSelect					KernelBase	1015
imp	'NeedCurrentDirectoryForExePathA'			NeedCurrentDirectoryForExePathA				kernel32	0		# KernelBase
imp	'NeedCurrentDirectoryForExePath'			NeedCurrentDirectoryForExePathW				kernel32	0		# KernelBase
imp	'NetAccessAdd'						NetAccessAdd						netapi32	71
imp	'NetAccessDel'						NetAccessDel						netapi32	72
imp	'NetAccessEnum'						NetAccessEnum						netapi32	73
imp	'NetAccessGetInfo'					NetAccessGetInfo					netapi32	74
imp	'NetAccessGetUserPerms'					NetAccessGetUserPerms					netapi32	75
imp	'NetAccessSetInfo'					NetAccessSetInfo					netapi32	76
imp	'NetAlertRaise'						NetAlertRaise						netapi32	79
imp	'NetAlertRaiseEx'					NetAlertRaiseEx						netapi32	80
imp	'NetAuditClear'						NetAuditClear						netapi32	85
imp	'NetAuditRead'						NetAuditRead						netapi32	86
imp	'NetAuditWrite'						NetAuditWrite						netapi32	87
imp	'NetConfigGet'						NetConfigGet						netapi32	88
imp	'NetConfigGetAll'					NetConfigGetAll						netapi32	89
imp	'NetConfigSet'						NetConfigSet						netapi32	90
imp	'NetErrorLogClear'					NetErrorLogClear					netapi32	124
imp	'NetErrorLogRead'					NetErrorLogRead						netapi32	125
imp	'NetErrorLogWrite'					NetErrorLogWrite					netapi32	126
imp	'NetMessageBufferSend'					NetMessageBufferSend					netapi32	161
imp	'NetMessageNameAdd'					NetMessageNameAdd					netapi32	162
imp	'NetMessageNameDel'					NetMessageNameDel					netapi32	163
imp	'NetMessageNameEnum'					NetMessageNameEnum					netapi32	164
imp	'NetMessageNameGetInfo'					NetMessageNameGetInfo					netapi32	165
imp	'NetRegisterDomainNameChangeNotification'		NetRegisterDomainNameChangeNotification			netapi32	169
imp	'NetReplExportDirAdd'					NetReplExportDirAdd					netapi32	175
imp	'NetReplExportDirDel'					NetReplExportDirDel					netapi32	176
imp	'NetReplExportDirEnum'					NetReplExportDirEnum					netapi32	177
imp	'NetReplExportDirGetInfo'				NetReplExportDirGetInfo					netapi32	178
imp	'NetReplExportDirLock'					NetReplExportDirLock					netapi32	179
imp	'NetReplExportDirSetInfo'				NetReplExportDirSetInfo					netapi32	180
imp	'NetReplExportDirUnlock'				NetReplExportDirUnlock					netapi32	181
imp	'NetReplGetInfo'					NetReplGetInfo						netapi32	182
imp	'NetReplImportDirAdd'					NetReplImportDirAdd					netapi32	183
imp	'NetReplImportDirDel'					NetReplImportDirDel					netapi32	184
imp	'NetReplImportDirEnum'					NetReplImportDirEnum					netapi32	185
imp	'NetReplImportDirGetInfo'				NetReplImportDirGetInfo					netapi32	186
imp	'NetReplImportDirLock'					NetReplImportDirLock					netapi32	187
imp	'NetReplImportDirUnlock'				NetReplImportDirUnlock					netapi32	188
imp	'NetReplSetInfo'					NetReplSetInfo						netapi32	189
imp	'NetServerEnum'						NetServerEnum						netapi32	202
imp	'NetServerEnumEx'					NetServerEnumEx						netapi32	203
imp	'NetServiceControl'					NetServiceControl					netapi32	210
imp	'NetServiceEnum'					NetServiceEnum						netapi32	211
imp	'NetServiceGetInfo'					NetServiceGetInfo					netapi32	212
imp	'NetServiceInstall'					NetServiceInstall					netapi32	213
imp	'NetStatisticsGet'					NetStatisticsGet					netapi32	227
imp	'NetUnregisterDomainNameChangeNotification'		NetUnregisterDomainNameChangeNotification		netapi32	229
imp	'NetWkstaGetInfo'					NetWkstaGetInfo						netapi32	248
imp	'NetWkstaSetInfo'					NetWkstaSetInfo						netapi32	249
imp	'Netbios'						Netbios							netapi32	257
imp	'NetpAddTlnFtinfoEntry'					NetpAddTlnFtinfoEntry					netapi32	258
imp	'NetpAllocFtinfoEntry'					NetpAllocFtinfoEntry					netapi32	259
imp	'NetpAssertFailed'					NetpAssertFailed					netapi32	260
imp	'NetpCleanFtinfoContext'				NetpCleanFtinfoContext					netapi32	261
imp	'NetpCloseConfigData'					NetpCloseConfigData					netapi32	262
imp	'NetpCopyFtinfoContext'					NetpCopyFtinfoContext					netapi32	263
imp	'NetpDbgPrint'						NetpDbgPrint						netapi32	264
imp	'NetpGetConfigBool'					NetpGetConfigBool					netapi32	265
imp	'NetpGetConfigDword'					NetpGetConfigDword					netapi32	266
imp	'NetpGetConfigTStrArray'				NetpGetConfigTStrArray					netapi32	267
imp	'NetpGetConfigValue'					NetpGetConfigValue					netapi32	268
imp	'NetpGetFileSecurity'					NetpGetFileSecurity					netapi32	269
imp	'NetpHexDump'						NetpHexDump						netapi32	270
imp	'NetpInitFtinfoContext'					NetpInitFtinfoContext					netapi32	271
imp	'NetpIsUncComputerNameValid'				NetpIsUncComputerNameValid				netapi32	273
imp	'NetpMergeFtinfo'					NetpMergeFtinfo						netapi32	274
imp	'NetpNetBiosReset'					NetpNetBiosReset					netapi32	275
imp	'NetpNetBiosStatusToApiStatus'				NetpNetBiosStatusToApiStatus				netapi32	276
imp	'NetpOpenConfigData'					NetpOpenConfigData					netapi32	277
imp	'NetpSetFileSecurity'					NetpSetFileSecurity					netapi32	278
imp	'NlsAnsiCodePage'					NlsAnsiCodePage						ntdll		194
imp	'NlsCheckPolicy'					NlsCheckPolicy						KernelBase	1018
imp	'NlsDispatchAnsiEnumProc'				NlsDispatchAnsiEnumProc					KernelBase	1019
imp	'NlsEventDataDescCreate'				NlsEventDataDescCreate					KernelBase	1020
imp	'NlsGetACPFromLocale'					NlsGetACPFromLocale					KernelBase	1021
imp	'NlsGetCacheUpdateCount'				NlsGetCacheUpdateCount					KernelBase	1022
imp	'NlsIsUserDefaultLocale'				NlsIsUserDefaultLocale					KernelBase	1023
imp	'NlsMbCodePageTag'					NlsMbCodePageTag					ntdll		195
imp	'NlsMbOemCodePageTag'					NlsMbOemCodePageTag					ntdll		196
imp	'NlsUpdateLocale'					NlsUpdateLocale						KernelBase	1024
imp	'NlsUpdateSystemLocale'					NlsUpdateSystemLocale					KernelBase	1025
imp	'NlsValidateLocale'					NlsValidateLocale					KernelBase	1026
imp	'NlsWriteEtwEvent'					NlsWriteEtwEvent					KernelBase	1027
imp	'NormalizeString'					NormalizeString						kernel32	0		# KernelBase
imp	'NotifyBootConfigStatus'				NotifyBootConfigStatus					advapi32	1514
imp	'NotifyChangeEventLog'					NotifyChangeEventLog					advapi32	1515
imp	'NotifyMountMgr'					NotifyMountMgr						KernelBase	1029
imp	'NotifyOverlayWindow'					NotifyOverlayWindow					user32		2179
imp	'NotifyRedirectedStringChange'				NotifyRedirectedStringChange				KernelBase	1030
imp	'NotifyServiceStatusChangeA'				NotifyServiceStatusChangeA				advapi32	1517
imp	'NotifyServiceStatusChange'				NotifyServiceStatusChangeW				advapi32	1518
imp	'NotifyUILanguageChange'				NotifyUILanguageChange					kernel32	1015
imp	'NotifyWinEvent'					NotifyWinEvent						user32		2180
imp	'NpGetUserName'						NpGetUserName						advapi32	1519
imp	'NtAcceptConnectPort'					NtAcceptConnectPort					ntdll		197
imp	'NtAccessCheck'						NtAccessCheck						ntdll		198
imp	'NtAccessCheckAndAuditAlarm'				NtAccessCheckAndAuditAlarm				ntdll		199
imp	'NtAccessCheckByType'					NtAccessCheckByType					ntdll		200
imp	'NtAccessCheckByTypeAndAuditAlarm'			NtAccessCheckByTypeAndAuditAlarm			ntdll		201
imp	'NtAccessCheckByTypeResultList'				NtAccessCheckByTypeResultList				ntdll		202
imp	'NtAccessCheckByTypeResultListAndAuditAlarm'		NtAccessCheckByTypeResultListAndAuditAlarm		ntdll		203
imp	'NtAccessCheckByTypeResultListAndAuditAlarmByHandle'	NtAccessCheckByTypeResultListAndAuditAlarmByHandle	ntdll		204
imp	'NtAcquireProcessActivityReference'			NtAcquireProcessActivityReference			ntdll		205
imp	'NtAddAtom'						NtAddAtom						ntdll		206
imp	'NtAddAtomEx'						NtAddAtomEx						ntdll		207
imp	'NtAddBootEntry'					NtAddBootEntry						ntdll		208
imp	'NtAddDriverEntry'					NtAddDriverEntry					ntdll		209
imp	'NtAdjustGroupsToken'					NtAdjustGroupsToken					ntdll		210
imp	'NtAdjustPrivilegesToken'				NtAdjustPrivilegesToken					ntdll		211
imp	'NtAdjustTokenClaimsAndDeviceGroups'			NtAdjustTokenClaimsAndDeviceGroups			ntdll		212
imp	'NtAlertResumeThread'					NtAlertResumeThread					ntdll		213
imp	'NtAlertThread'						NtAlertThread						ntdll		214
imp	'NtAlertThreadByThreadId'				NtAlertThreadByThreadId					ntdll		215
imp	'NtAllocateLocallyUniqueId'				NtAllocateLocallyUniqueId				ntdll		216
imp	'NtAllocateReserveObject'				NtAllocateReserveObject					ntdll		217
imp	'NtAllocateUserPhysicalPages'				NtAllocateUserPhysicalPages				ntdll		218
imp	'NtAllocateUuids'					NtAllocateUuids						ntdll		219
imp	'NtAllocateVirtualMemory'				NtAllocateVirtualMemory					ntdll		220	6
imp	'NtAllocateVirtualMemoryEx'				NtAllocateVirtualMemoryEx				ntdll		221
imp	'NtAlpcAcceptConnectPort'				NtAlpcAcceptConnectPort					ntdll		222
imp	'NtAlpcCancelMessage'					NtAlpcCancelMessage					ntdll		223
imp	'NtAlpcConnectPort'					NtAlpcConnectPort					ntdll		224
imp	'NtAlpcConnectPortEx'					NtAlpcConnectPortEx					ntdll		225
imp	'NtAlpcCreatePort'					NtAlpcCreatePort					ntdll		226
imp	'NtAlpcCreatePortSection'				NtAlpcCreatePortSection					ntdll		227
imp	'NtAlpcCreateResourceReserve'				NtAlpcCreateResourceReserve				ntdll		228
imp	'NtAlpcCreateSectionView'				NtAlpcCreateSectionView					ntdll		229
imp	'NtAlpcCreateSecurityContext'				NtAlpcCreateSecurityContext				ntdll		230
imp	'NtAlpcDeletePortSection'				NtAlpcDeletePortSection					ntdll		231
imp	'NtAlpcDeleteResourceReserve'				NtAlpcDeleteResourceReserve				ntdll		232
imp	'NtAlpcDeleteSectionView'				NtAlpcDeleteSectionView					ntdll		233
imp	'NtAlpcDeleteSecurityContext'				NtAlpcDeleteSecurityContext				ntdll		234
imp	'NtAlpcDisconnectPort'					NtAlpcDisconnectPort					ntdll		235
imp	'NtAlpcImpersonateClientContainerOfPort'		NtAlpcImpersonateClientContainerOfPort			ntdll		236
imp	'NtAlpcImpersonateClientOfPort'				NtAlpcImpersonateClientOfPort				ntdll		237
imp	'NtAlpcOpenSenderProcess'				NtAlpcOpenSenderProcess					ntdll		238
imp	'NtAlpcOpenSenderThread'				NtAlpcOpenSenderThread					ntdll		239
imp	'NtAlpcQueryInformation'				NtAlpcQueryInformation					ntdll		240
imp	'NtAlpcQueryInformationMessage'				NtAlpcQueryInformationMessage				ntdll		241
imp	'NtAlpcRevokeSecurityContext'				NtAlpcRevokeSecurityContext				ntdll		242
imp	'NtAlpcSendWaitReceivePort'				NtAlpcSendWaitReceivePort				ntdll		243
imp	'NtAlpcSetInformation'					NtAlpcSetInformation					ntdll		244
imp	'NtApphelpCacheControl'					NtApphelpCacheControl					ntdll		245
imp	'NtAreMappedFilesTheSame'				NtAreMappedFilesTheSame					ntdll		246
imp	'NtAssignProcessToJobObject'				NtAssignProcessToJobObject				ntdll		247
imp	'NtAssociateWaitCompletionPacket'			NtAssociateWaitCompletionPacket				ntdll		248
imp	'NtCallEnclave'						NtCallEnclave						ntdll		249
imp	'NtCallbackReturn'					NtCallbackReturn					ntdll		250	3
imp	'NtCancelIoFile'					NtCancelIoFile						ntdll		251	2
imp	'NtCancelIoFileEx'					NtCancelIoFileEx					ntdll		252	3
imp	'NtCancelSynchronousIoFile'				NtCancelSynchronousIoFile				ntdll		253
imp	'NtCancelTimer'						NtCancelTimer						ntdll		254
imp	'NtCancelTimer2'					NtCancelTimer2						ntdll		255
imp	'NtCancelWaitCompletionPacket'				NtCancelWaitCompletionPacket				ntdll		256
imp	'NtClearEvent'						NtClearEvent						ntdll		257	1
imp	'NtClose'						NtClose							ntdll		258	1
imp	'NtCloseObjectAuditAlarm'				NtCloseObjectAuditAlarm					ntdll		259
imp	'NtCommitComplete'					NtCommitComplete					ntdll		260
imp	'NtCommitEnlistment'					NtCommitEnlistment					ntdll		261
imp	'NtCommitRegistryTransaction'				NtCommitRegistryTransaction				ntdll		262
imp	'NtCommitTransaction'					NtCommitTransaction					ntdll		263
imp	'NtCompactKeys'						NtCompactKeys						ntdll		264
imp	'NtCompareObjects'					NtCompareObjects					ntdll		265
imp	'NtCompareSigningLevels'				NtCompareSigningLevels					ntdll		266
imp	'NtCompareTokens'					NtCompareTokens						ntdll		267
imp	'NtCompleteConnectPort'					NtCompleteConnectPort					ntdll		268
imp	'NtCompressKey'						NtCompressKey						ntdll		269
imp	'NtConnectPort'						NtConnectPort						ntdll		270
imp	'NtContinue'						NtContinue						ntdll		271	2
imp	'NtConvertBetweenAuxiliaryCounterAndPerformanceCounter'	NtConvertBetweenAuxiliaryCounterAndPerformanceCounter	ntdll		272
imp	'NtCreateDebugObject'					NtCreateDebugObject					ntdll		273
imp	'NtCreateDirectoryObject'				NtCreateDirectoryObject					ntdll		274	3
imp	'NtCreateDirectoryObjectEx'				NtCreateDirectoryObjectEx				ntdll		275
imp	'NtCreateEnclave'					NtCreateEnclave						ntdll		276
imp	'NtCreateEnlistment'					NtCreateEnlistment					ntdll		277
imp	'NtCreateEvent'						NtCreateEvent						ntdll		278	5
imp	'NtCreateEventPair'					NtCreateEventPair					ntdll		279
imp	'NtCreateFile'						NtCreateFile						ntdll		280	11
imp	'NtCreateIRTimer'					NtCreateIRTimer						ntdll		281
imp	'NtCreateIoCompletion'					NtCreateIoCompletion					ntdll		282	4
imp	'NtCreateJobObject'					NtCreateJobObject					ntdll		283
imp	'NtCreateJobSet'					NtCreateJobSet						ntdll		284
imp	'NtCreateKey'						NtCreateKey						ntdll		285	7
imp	'NtCreateKeyTransacted'					NtCreateKeyTransacted					ntdll		286
imp	'NtCreateKeyedEvent'					NtCreateKeyedEvent					ntdll		287	4
imp	'NtCreateLowBoxToken'					NtCreateLowBoxToken					ntdll		288
imp	'NtCreateMailslotFile'					NtCreateMailslotFile					ntdll		289
imp	'NtCreateMutant'					NtCreateMutant						ntdll		290
imp	'NtCreateNamedPipeFile'					NtCreateNamedPipeFile					ntdll		291	14
imp	'NtCreatePagingFile'					NtCreatePagingFile					ntdll		292
imp	'NtCreatePartition'					NtCreatePartition					ntdll		293
imp	'NtCreatePort'						NtCreatePort						ntdll		294
imp	'NtCreatePrivateNamespace'				NtCreatePrivateNamespace				ntdll		295
imp	'NtCreateProcess'					NtCreateProcess						ntdll		296	8
imp	'NtCreateProcessEx'					NtCreateProcessEx					ntdll		297
imp	'NtCreateProfile'					NtCreateProfile						ntdll		298	9
imp	'NtCreateProfileEx'					NtCreateProfileEx					ntdll		299
imp	'NtCreateRegistryTransaction'				NtCreateRegistryTransaction				ntdll		300
imp	'NtCreateResourceManager'				NtCreateResourceManager					ntdll		301
imp	'NtCreateSection'					NtCreateSection						ntdll		302	7
imp	'NtCreateSemaphore'					NtCreateSemaphore					ntdll		303
imp	'NtCreateSymbolicLinkObject'				NtCreateSymbolicLinkObject				ntdll		304
imp	'NtCreateThread'					NtCreateThread						ntdll		305	8
imp	'NtCreateThreadEx'					NtCreateThreadEx					ntdll		306
imp	'NtCreateTimer'						NtCreateTimer						ntdll		307	4
imp	'NtCreateTimer2'					NtCreateTimer2						ntdll		308
imp	'NtCreateToken'						NtCreateToken						ntdll		309
imp	'NtCreateTokenEx'					NtCreateTokenEx						ntdll		310
imp	'NtCreateTransaction'					NtCreateTransaction					ntdll		311
imp	'NtCreateTransactionManager'				NtCreateTransactionManager				ntdll		312
imp	'NtCreateUserProcess'					NtCreateUserProcess					ntdll		313
imp	'NtCreateWaitCompletionPacket'				NtCreateWaitCompletionPacket				ntdll		314
imp	'NtCreateWaitablePort'					NtCreateWaitablePort					ntdll		315
imp	'NtCreateWnfStateName'					NtCreateWnfStateName					ntdll		316
imp	'NtCreateWorkerFactory'					NtCreateWorkerFactory					ntdll		317
imp	'NtDebugActiveProcess'					NtDebugActiveProcess					ntdll		318
imp	'NtDebugContinue'					NtDebugContinue						ntdll		319
imp	'NtDelayExecution'					NtDelayExecution					ntdll		320	2
imp	'NtDeleteAtom'						NtDeleteAtom						ntdll		321
imp	'NtDeleteBootEntry'					NtDeleteBootEntry					ntdll		322
imp	'NtDeleteDriverEntry'					NtDeleteDriverEntry					ntdll		323
imp	'NtDeleteFile'						NtDeleteFile						ntdll		324	1
imp	'NtDeleteKey'						NtDeleteKey						ntdll		325	1
imp	'NtDeleteObjectAuditAlarm'				NtDeleteObjectAuditAlarm				ntdll		326
imp	'NtDeletePrivateNamespace'				NtDeletePrivateNamespace				ntdll		327
imp	'NtDeleteValueKey'					NtDeleteValueKey					ntdll		328
imp	'NtDeleteWnfStateData'					NtDeleteWnfStateData					ntdll		329
imp	'NtDeleteWnfStateName'					NtDeleteWnfStateName					ntdll		330
imp	'NtDeviceIoControlFile'					NtDeviceIoControlFile					ntdll		331	10
imp	'NtDisableLastKnownGood'				NtDisableLastKnownGood					ntdll		332
imp	'NtDisplayString'					NtDisplayString						ntdll		333
imp	'NtDrawText'						NtDrawText						ntdll		334
imp	'NtDuplicateObject'					NtDuplicateObject					ntdll		335	7
imp	'NtDuplicateToken'					NtDuplicateToken					ntdll		336
imp	'NtEnableLastKnownGood'					NtEnableLastKnownGood					ntdll		337
imp	'NtEnumerateBootEntries'				NtEnumerateBootEntries					ntdll		338
imp	'NtEnumerateDriverEntries'				NtEnumerateDriverEntries				ntdll		339
imp	'NtEnumerateKey'					NtEnumerateKey						ntdll		340	6
imp	'NtEnumerateSystemEnvironmentValuesEx'			NtEnumerateSystemEnvironmentValuesEx			ntdll		341
imp	'NtEnumerateTransactionObject'				NtEnumerateTransactionObject				ntdll		342
imp	'NtEnumerateValueKey'					NtEnumerateValueKey					ntdll		343	6
imp	'NtExtendSection'					NtExtendSection						ntdll		344
imp	'NtFilterBootOption'					NtFilterBootOption					ntdll		345
imp	'NtFilterToken'						NtFilterToken						ntdll		346
imp	'NtFilterTokenEx'					NtFilterTokenEx						ntdll		347
imp	'NtFindAtom'						NtFindAtom						ntdll		348
imp	'NtFlushBuffersFile'					NtFlushBuffersFile					ntdll		349	2
imp	'NtFlushBuffersFileEx'					NtFlushBuffersFileEx					ntdll		350
imp	'NtFlushInstallUILanguage'				NtFlushInstallUILanguage				ntdll		351
imp	'NtFlushInstructionCache'				NtFlushInstructionCache					ntdll		352	3
imp	'NtFlushKey'						NtFlushKey						ntdll		353	1
imp	'NtFlushProcessWriteBuffers'				NtFlushProcessWriteBuffers				ntdll		354
imp	'NtFlushVirtualMemory'					NtFlushVirtualMemory					ntdll		355	4
imp	'NtFlushWriteBuffer'					NtFlushWriteBuffer					ntdll		356
imp	'NtFreeUserPhysicalPages'				NtFreeUserPhysicalPages					ntdll		357
imp	'NtFreeVirtualMemory'					NtFreeVirtualMemory					ntdll		358	4
imp	'NtFreezeRegistry'					NtFreezeRegistry					ntdll		359
imp	'NtFreezeTransactions'					NtFreezeTransactions					ntdll		360
imp	'NtFsControlFile'					NtFsControlFile						ntdll		361	10
imp	'NtGetCachedSigningLevel'				NtGetCachedSigningLevel					ntdll		362
imp	'NtGetCompleteWnfStateSubscription'			NtGetCompleteWnfStateSubscription			ntdll		363
imp	'NtGetContextThread'					NtGetContextThread					ntdll		364	2
imp	'NtGetCurrentProcessorNumber'				NtGetCurrentProcessorNumber				ntdll		365
imp	'NtGetCurrentProcessorNumberEx'				NtGetCurrentProcessorNumberEx				ntdll		366
imp	'NtGetDevicePowerState'					NtGetDevicePowerState					ntdll		367
imp	'NtGetMUIRegistryInfo'					NtGetMUIRegistryInfo					ntdll		368
imp	'NtGetNextProcess'					NtGetNextProcess					ntdll		369
imp	'NtGetNextThread'					NtGetNextThread						ntdll		370
imp	'NtGetNlsSectionPtr'					NtGetNlsSectionPtr					ntdll		371
imp	'NtGetNotificationResourceManager'			NtGetNotificationResourceManager			ntdll		372
imp	'NtGetTickCount'					NtGetTickCount						ntdll		373
imp	'NtGetWriteWatch'					NtGetWriteWatch						ntdll		374
imp	'NtImpersonateAnonymousToken'				NtImpersonateAnonymousToken				ntdll		375
imp	'NtImpersonateClientOfPort'				NtImpersonateClientOfPort				ntdll		376
imp	'NtImpersonateThread'					NtImpersonateThread					ntdll		377
imp	'NtInitializeEnclave'					NtInitializeEnclave					ntdll		378
imp	'NtInitializeNlsFiles'					NtInitializeNlsFiles					ntdll		379
imp	'NtInitializeRegistry'					NtInitializeRegistry					ntdll		380
imp	'NtInitiatePowerAction'					NtInitiatePowerAction					ntdll		381
imp	'NtIsProcessInJob'					NtIsProcessInJob					ntdll		382
imp	'NtIsSystemResumeAutomatic'				NtIsSystemResumeAutomatic				ntdll		383
imp	'NtIsUILanguageComitted'				NtIsUILanguageComitted					ntdll		384
imp	'NtListenPort'						NtListenPort						ntdll		385
imp	'NtLoadDriver'						NtLoadDriver						ntdll		386
imp	'NtLoadEnclaveData'					NtLoadEnclaveData					ntdll		387
imp	'NtLoadHotPatch'					NtLoadHotPatch						ntdll		388
imp	'NtLoadKey'						NtLoadKey						ntdll		389
imp	'NtLoadKey2'						NtLoadKey2						ntdll		390
imp	'NtLoadKeyEx'						NtLoadKeyEx						ntdll		391
imp	'NtLockFile'						NtLockFile						ntdll		392
imp	'NtLockProductActivationKeys'				NtLockProductActivationKeys				ntdll		393
imp	'NtLockRegistryKey'					NtLockRegistryKey					ntdll		394
imp	'NtLockVirtualMemory'					NtLockVirtualMemory					ntdll		395
imp	'NtMakePermanentObject'					NtMakePermanentObject					ntdll		396
imp	'NtMakeTemporaryObject'					NtMakeTemporaryObject					ntdll		397
imp	'NtManagePartition'					NtManagePartition					ntdll		398
imp	'NtMapCMFModule'					NtMapCMFModule						ntdll		399
imp	'NtMapUserPhysicalPages'				NtMapUserPhysicalPages					ntdll		400
imp	'NtMapUserPhysicalPagesScatter'				NtMapUserPhysicalPagesScatter				ntdll		401
imp	'NtMapViewOfSection'					NtMapViewOfSection					ntdll		402	10
imp	'NtMapViewOfSectionEx'					NtMapViewOfSectionEx					ntdll		403
imp	'NtModifyBootEntry'					NtModifyBootEntry					ntdll		404
imp	'NtModifyDriverEntry'					NtModifyDriverEntry					ntdll		405
imp	'NtNotifyChangeDirectoryFile'				NtNotifyChangeDirectoryFile				ntdll		406
imp	'NtNotifyChangeDirectoryFileEx'				NtNotifyChangeDirectoryFileEx				ntdll		407
imp	'NtNotifyChangeKey'					NtNotifyChangeKey					ntdll		408
imp	'NtNotifyChangeMultipleKeys'				NtNotifyChangeMultipleKeys				ntdll		409
imp	'NtNotifyChangeSession'					NtNotifyChangeSession					ntdll		410
imp	'NtOpenDirectoryObject'					NtOpenDirectoryObject					ntdll		411	3
imp	'NtOpenEnlistment'					NtOpenEnlistment					ntdll		412
imp	'NtOpenEvent'						NtOpenEvent						ntdll		413
imp	'NtOpenEventPair'					NtOpenEventPair						ntdll		414
imp	'NtOpenFile'						NtOpenFile						ntdll		415	6
imp	'NtOpenIoCompletion'					NtOpenIoCompletion					ntdll		416
imp	'NtOpenJobObject'					NtOpenJobObject						ntdll		417
imp	'NtOpenKey'						NtOpenKey						ntdll		418	3
imp	'NtOpenKeyEx'						NtOpenKeyEx						ntdll		419
imp	'NtOpenKeyTransacted'					NtOpenKeyTransacted					ntdll		420
imp	'NtOpenKeyTransactedEx'					NtOpenKeyTransactedEx					ntdll		421
imp	'NtOpenKeyedEvent'					NtOpenKeyedEvent					ntdll		422
imp	'NtOpenMutant'						NtOpenMutant						ntdll		423
imp	'NtOpenObjectAuditAlarm'				NtOpenObjectAuditAlarm					ntdll		424
imp	'NtOpenPartition'					NtOpenPartition						ntdll		425
imp	'NtOpenPrivateNamespace'				NtOpenPrivateNamespace					ntdll		426
imp	'NtOpenProcess'						NtOpenProcess						ntdll		427	4
imp	'NtOpenProcessToken'					NtOpenProcessToken					ntdll		428	3
imp	'NtOpenProcessTokenEx'					NtOpenProcessTokenEx					ntdll		429
imp	'NtOpenRegistryTransaction'				NtOpenRegistryTransaction				ntdll		430
imp	'NtOpenResourceManager'					NtOpenResourceManager					ntdll		431
imp	'NtOpenSection'						NtOpenSection						ntdll		432	3
imp	'NtOpenSemaphore'					NtOpenSemaphore						ntdll		433
imp	'NtOpenSession'						NtOpenSession						ntdll		434
imp	'NtOpenSymbolicLinkObject'				NtOpenSymbolicLinkObject				ntdll		435	3
imp	'NtOpenThread'						NtOpenThread						ntdll		436	4
imp	'NtOpenThreadToken'					NtOpenThreadToken					ntdll		437	4
imp	'NtOpenThreadTokenEx'					NtOpenThreadTokenEx					ntdll		438
imp	'NtOpenTimer'						NtOpenTimer						ntdll		439
imp	'NtOpenTransaction'					NtOpenTransaction					ntdll		440
imp	'NtOpenTransactionManager'				NtOpenTransactionManager				ntdll		441
imp	'NtPlugPlayControl'					NtPlugPlayControl					ntdll		442
imp	'NtPowerInformation'					NtPowerInformation					ntdll		443
imp	'NtPrePrepareComplete'					NtPrePrepareComplete					ntdll		444
imp	'NtPrePrepareEnlistment'				NtPrePrepareEnlistment					ntdll		445
imp	'NtPrepareComplete'					NtPrepareComplete					ntdll		446
imp	'NtPrepareEnlistment'					NtPrepareEnlistment					ntdll		447
imp	'NtPrivilegeCheck'					NtPrivilegeCheck					ntdll		448
imp	'NtPrivilegeObjectAuditAlarm'				NtPrivilegeObjectAuditAlarm				ntdll		449
imp	'NtPrivilegedServiceAuditAlarm'				NtPrivilegedServiceAuditAlarm				ntdll		450
imp	'NtPropagationComplete'					NtPropagationComplete					ntdll		451
imp	'NtPropagationFailed'					NtPropagationFailed					ntdll		452
imp	'NtProtectVirtualMemory'				NtProtectVirtualMemory					ntdll		453	5
imp	'NtPulseEvent'						NtPulseEvent						ntdll		454
imp	'NtQueryAttributesFile'					NtQueryAttributesFile					ntdll		455	2
imp	'NtQueryAuxiliaryCounterFrequency'			NtQueryAuxiliaryCounterFrequency			ntdll		456
imp	'NtQueryBootEntryOrder'					NtQueryBootEntryOrder					ntdll		457
imp	'NtQueryBootOptions'					NtQueryBootOptions					ntdll		458
imp	'NtQueryDebugFilterState'				NtQueryDebugFilterState					ntdll		459
imp	'NtQueryDefaultLocale'					NtQueryDefaultLocale					ntdll		460
imp	'NtQueryDefaultUILanguage'				NtQueryDefaultUILanguage				ntdll		461
imp	'NtQueryDirectoryFile'					NtQueryDirectoryFile					ntdll		462	11
imp	'NtQueryDirectoryFileEx'				NtQueryDirectoryFileEx					ntdll		463
imp	'NtQueryDirectoryObject'				NtQueryDirectoryObject					ntdll		464
imp	'NtQueryDriverEntryOrder'				NtQueryDriverEntryOrder					ntdll		465
imp	'NtQueryEaFile'						NtQueryEaFile						ntdll		466
imp	'NtQueryEvent'						NtQueryEvent						ntdll		467
imp	'NtQueryFullAttributesFile'				NtQueryFullAttributesFile				ntdll		468	2
imp	'NtQueryInformationAtom'				NtQueryInformationAtom					ntdll		469
imp	'NtQueryInformationByName'				NtQueryInformationByName				ntdll		470
imp	'NtQueryInformationEnlistment'				NtQueryInformationEnlistment				ntdll		471
imp	'NtQueryInformationFile'				NtQueryInformationFile					ntdll		472	5
imp	'NtQueryInformationJobObject'				NtQueryInformationJobObject				ntdll		473	5
imp	'NtQueryInformationPort'				NtQueryInformationPort					ntdll		474
imp	'NtQueryInformationProcess'				NtQueryInformationProcess				ntdll		475	5
imp	'NtQueryInformationResourceManager'			NtQueryInformationResourceManager			ntdll		476
imp	'NtQueryInformationThread'				NtQueryInformationThread				ntdll		477	5
imp	'NtQueryInformationToken'				NtQueryInformationToken					ntdll		478	5
imp	'NtQueryInformationTransaction'				NtQueryInformationTransaction				ntdll		479
imp	'NtQueryInformationTransactionManager'			NtQueryInformationTransactionManager			ntdll		480
imp	'NtQueryInformationWorkerFactory'			NtQueryInformationWorkerFactory				ntdll		481
imp	'NtQueryInstallUILanguage'				NtQueryInstallUILanguage				ntdll		482
imp	'NtQueryIntervalProfile'				NtQueryIntervalProfile					ntdll		483	2
imp	'NtQueryIoCompletion'					NtQueryIoCompletion					ntdll		484
imp	'NtQueryKey'						NtQueryKey						ntdll		485
imp	'NtQueryLicenseValue'					NtQueryLicenseValue					ntdll		486
imp	'NtQueryMultipleValueKey'				NtQueryMultipleValueKey					ntdll		487
imp	'NtQueryMutant'						NtQueryMutant						ntdll		488
imp	'NtQueryObject'						NtQueryObject						ntdll		489	5
imp	'NtQueryOpenSubKeys'					NtQueryOpenSubKeys					ntdll		490
imp	'NtQueryOpenSubKeysEx'					NtQueryOpenSubKeysEx					ntdll		491
imp	'NtQueryPerformanceCounter'				NtQueryPerformanceCounter				ntdll		492	2
imp	'NtQueryPortInformationProcess'				NtQueryPortInformationProcess				ntdll		493
imp	'NtQueryQuotaInformationFile'				NtQueryQuotaInformationFile				ntdll		494
imp	'NtQuerySection'					NtQuerySection						ntdll		495	5
imp	'NtQuerySecurityAttributesToken'			NtQuerySecurityAttributesToken				ntdll		496
imp	'NtQuerySecurityObject'					NtQuerySecurityObject					ntdll		497	5
imp	'NtQuerySecurityPolicy'					NtQuerySecurityPolicy					ntdll		498
imp	'NtQuerySemaphore'					NtQuerySemaphore					ntdll		499
imp	'NtQuerySymbolicLinkObject'				NtQuerySymbolicLinkObject				ntdll		500	3
imp	'NtQuerySystemEnvironmentValue'				NtQuerySystemEnvironmentValue				ntdll		501
imp	'NtQuerySystemEnvironmentValueEx'			NtQuerySystemEnvironmentValueEx				ntdll		502
imp	'NtQuerySystemInformation'				NtQuerySystemInformation				ntdll		503	4
imp	'NtQuerySystemInformationEx'				NtQuerySystemInformationEx				ntdll		504
imp	'NtQuerySystemTime'					NtQuerySystemTime					ntdll		505	1
imp	'NtQueryTimer'						NtQueryTimer						ntdll		506
imp	'NtQueryTimerResolution'				NtQueryTimerResolution					ntdll		507
imp	'NtQueryValueKey'					NtQueryValueKey						ntdll		508	6
imp	'NtQueryVirtualMemory'					NtQueryVirtualMemory					ntdll		509	6
imp	'NtQueryVolumeInformationFile'				NtQueryVolumeInformationFile				ntdll		510	5
imp	'NtQueryWnfStateData'					NtQueryWnfStateData					ntdll		511
imp	'NtQueryWnfStateNameInformation'			NtQueryWnfStateNameInformation				ntdll		512
imp	'NtQueueApcThread'					NtQueueApcThread					ntdll		513	5
imp	'NtQueueApcThreadEx'					NtQueueApcThreadEx					ntdll		514
imp	'NtRaiseException'					NtRaiseException					ntdll		515	3
imp	'NtRaiseHardError'					NtRaiseHardError					ntdll		516	6
imp	'NtReadFile'						NtReadFile						ntdll		517	9
imp	'NtReadFileScatter'					NtReadFileScatter					ntdll		518
imp	'NtReadOnlyEnlistment'					NtReadOnlyEnlistment					ntdll		519
imp	'NtReadRequestData'					NtReadRequestData					ntdll		520
imp	'NtReadVirtualMemory'					NtReadVirtualMemory					ntdll		521	5
imp	'NtRecoverEnlistment'					NtRecoverEnlistment					ntdll		522
imp	'NtRecoverResourceManager'				NtRecoverResourceManager				ntdll		523
imp	'NtRecoverTransactionManager'				NtRecoverTransactionManager				ntdll		524
imp	'NtRegisterProtocolAddressInformation'			NtRegisterProtocolAddressInformation			ntdll		525
imp	'NtRegisterThreadTerminatePort'				NtRegisterThreadTerminatePort				ntdll		526
imp	'NtReleaseKeyedEvent'					NtReleaseKeyedEvent					ntdll		527	4
imp	'NtReleaseMutant'					NtReleaseMutant						ntdll		528
imp	'NtReleaseSemaphore'					NtReleaseSemaphore					ntdll		529
imp	'NtReleaseWorkerFactoryWorker'				NtReleaseWorkerFactoryWorker				ntdll		530
imp	'NtRemoveIoCompletion'					NtRemoveIoCompletion					ntdll		531
imp	'NtRemoveIoCompletionEx'				NtRemoveIoCompletionEx					ntdll		532
imp	'NtRemoveProcessDebug'					NtRemoveProcessDebug					ntdll		533
imp	'NtRenameKey'						NtRenameKey						ntdll		534
imp	'NtRenameTransactionManager'				NtRenameTransactionManager				ntdll		535
imp	'NtReplaceKey'						NtReplaceKey						ntdll		536
imp	'NtReplacePartitionUnit'				NtReplacePartitionUnit					ntdll		537
imp	'NtReplyPort'						NtReplyPort						ntdll		538
imp	'NtReplyWaitReceivePort'				NtReplyWaitReceivePort					ntdll		539
imp	'NtReplyWaitReceivePortEx'				NtReplyWaitReceivePortEx				ntdll		540
imp	'NtReplyWaitReplyPort'					NtReplyWaitReplyPort					ntdll		541
imp	'NtRequestPort'						NtRequestPort						ntdll		542
imp	'NtRequestWaitReplyPort'				NtRequestWaitReplyPort					ntdll		543
imp	'NtResetEvent'						NtResetEvent						ntdll		544
imp	'NtResetWriteWatch'					NtResetWriteWatch					ntdll		545
imp	'NtRestoreKey'						NtRestoreKey						ntdll		546
imp	'NtResumeProcess'					NtResumeProcess						ntdll		547
imp	'NtResumeThread'					NtResumeThread						ntdll		548	2
imp	'NtRevertContainerImpersonation'			NtRevertContainerImpersonation				ntdll		549
imp	'NtRollbackComplete'					NtRollbackComplete					ntdll		550
imp	'NtRollbackEnlistment'					NtRollbackEnlistment					ntdll		551
imp	'NtRollbackRegistryTransaction'				NtRollbackRegistryTransaction				ntdll		552
imp	'NtRollbackTransaction'					NtRollbackTransaction					ntdll		553
imp	'NtRollforwardTransactionManager'			NtRollforwardTransactionManager				ntdll		554
imp	'NtSaveKey'						NtSaveKey						ntdll		555
imp	'NtSaveKeyEx'						NtSaveKeyEx						ntdll		556
imp	'NtSaveMergedKeys'					NtSaveMergedKeys					ntdll		557
imp	'NtSecureConnectPort'					NtSecureConnectPort					ntdll		558
imp	'NtSerializeBoot'					NtSerializeBoot						ntdll		559
imp	'NtSetBootEntryOrder'					NtSetBootEntryOrder					ntdll		560
imp	'NtSetBootOptions'					NtSetBootOptions					ntdll		561
imp	'NtSetCachedSigningLevel'				NtSetCachedSigningLevel					ntdll		562
imp	'NtSetCachedSigningLevel2'				NtSetCachedSigningLevel2				ntdll		563
imp	'NtSetContextThread'					NtSetContextThread					ntdll		564	2
imp	'NtSetDebugFilterState'					NtSetDebugFilterState					ntdll		565
imp	'NtSetDefaultHardErrorPort'				NtSetDefaultHardErrorPort				ntdll		566
imp	'NtSetDefaultLocale'					NtSetDefaultLocale					ntdll		567
imp	'NtSetDefaultUILanguage'				NtSetDefaultUILanguage					ntdll		568
imp	'NtSetDriverEntryOrder'					NtSetDriverEntryOrder					ntdll		569
imp	'NtSetEaFile'						NtSetEaFile						ntdll		570
imp	'NtSetEvent'						NtSetEvent						ntdll		571	2
imp	'NtSetEventBoostPriority'				NtSetEventBoostPriority					ntdll		572
imp	'NtSetHighEventPair'					NtSetHighEventPair					ntdll		573
imp	'NtSetHighWaitLowEventPair'				NtSetHighWaitLowEventPair				ntdll		574
imp	'NtSetIRTimer'						NtSetIRTimer						ntdll		575
imp	'NtSetInformationDebugObject'				NtSetInformationDebugObject				ntdll		576
imp	'NtSetInformationEnlistment'				NtSetInformationEnlistment				ntdll		577
imp	'NtSetInformationFile'					NtSetInformationFile					ntdll		578	5
imp	'NtSetInformationJobObject'				NtSetInformationJobObject				ntdll		579
imp	'NtSetInformationKey'					NtSetInformationKey					ntdll		580
imp	'NtSetInformationObject'				NtSetInformationObject					ntdll		581
imp	'NtSetInformationProcess'				NtSetInformationProcess					ntdll		582
imp	'NtSetInformationResourceManager'			NtSetInformationResourceManager				ntdll		583
imp	'NtSetInformationSymbolicLink'				NtSetInformationSymbolicLink				ntdll		584
imp	'NtSetInformationThread'				NtSetInformationThread					ntdll		585	4
imp	'NtSetInformationToken'					NtSetInformationToken					ntdll		586
imp	'NtSetInformationTransaction'				NtSetInformationTransaction				ntdll		587
imp	'NtSetInformationTransactionManager'			NtSetInformationTransactionManager			ntdll		588
imp	'NtSetInformationVirtualMemory'				NtSetInformationVirtualMemory				ntdll		589
imp	'NtSetInformationWorkerFactory'				NtSetInformationWorkerFactory				ntdll		590
imp	'NtSetIntervalProfile'					NtSetIntervalProfile					ntdll		591	2
imp	'NtSetIoCompletion'					NtSetIoCompletion					ntdll		592
imp	'NtSetIoCompletionEx'					NtSetIoCompletionEx					ntdll		593
imp	'NtSetLdtEntries'					NtSetLdtEntries						ntdll		594
imp	'NtSetLowEventPair'					NtSetLowEventPair					ntdll		595
imp	'NtSetLowWaitHighEventPair'				NtSetLowWaitHighEventPair				ntdll		596
imp	'NtSetQuotaInformationFile'				NtSetQuotaInformationFile				ntdll		597
imp	'NtSetSecurityObject'					NtSetSecurityObject					ntdll		598
imp	'NtSetSystemEnvironmentValue'				NtSetSystemEnvironmentValue				ntdll		599
imp	'NtSetSystemEnvironmentValueEx'				NtSetSystemEnvironmentValueEx				ntdll		600
imp	'NtSetSystemInformation'				NtSetSystemInformation					ntdll		601
imp	'NtSetSystemPowerState'					NtSetSystemPowerState					ntdll		602
imp	'NtSetSystemTime'					NtSetSystemTime						ntdll		603
imp	'NtSetThreadExecutionState'				NtSetThreadExecutionState				ntdll		604
imp	'NtSetTimer'						NtSetTimer						ntdll		605	7
imp	'NtSetTimer2'						NtSetTimer2						ntdll		606
imp	'NtSetTimerEx'						NtSetTimerEx						ntdll		607
imp	'NtSetTimerResolution'					NtSetTimerResolution					ntdll		608
imp	'NtSetUuidSeed'						NtSetUuidSeed						ntdll		609
imp	'NtSetValueKey'						NtSetValueKey						ntdll		610	6
imp	'NtSetVolumeInformationFile'				NtSetVolumeInformationFile				ntdll		611
imp	'NtSetWnfProcessNotificationEvent'			NtSetWnfProcessNotificationEvent			ntdll		612
imp	'NtShutdownSystem'					NtShutdownSystem					ntdll		613
imp	'NtShutdownWorkerFactory'				NtShutdownWorkerFactory					ntdll		614
imp	'NtSignalAndWaitForSingleObject'			NtSignalAndWaitForSingleObject				ntdll		615	4
imp	'NtSinglePhaseReject'					NtSinglePhaseReject					ntdll		616
imp	'NtStartProfile'					NtStartProfile						ntdll		617	1
imp	'NtStopProfile'						NtStopProfile						ntdll		618	1
imp	'NtSubscribeWnfStateChange'				NtSubscribeWnfStateChange				ntdll		619
imp	'NtSuspendProcess'					NtSuspendProcess					ntdll		620
imp	'NtSuspendThread'					NtSuspendThread						ntdll		621	2
imp	'NtSystemDebugControl'					NtSystemDebugControl					ntdll		622
imp	'NtTerminateEnclave'					NtTerminateEnclave					ntdll		623
imp	'NtTerminateJobObject'					NtTerminateJobObject					ntdll		624
imp	'NtTerminateProcess'					NtTerminateProcess					ntdll		625	2
imp	'NtTerminateThread'					NtTerminateThread					ntdll		626	2
imp	'NtTestAlert'						NtTestAlert						ntdll		627	0
imp	'NtThawRegistry'					NtThawRegistry						ntdll		628
imp	'NtThawTransactions'					NtThawTransactions					ntdll		629
imp	'NtTraceControl'					NtTraceControl						ntdll		630
imp	'NtTraceEvent'						NtTraceEvent						ntdll		631
imp	'NtTranslateFilePath'					NtTranslateFilePath					ntdll		632
imp	'NtUmsThreadYield'					NtUmsThreadYield					ntdll		633
imp	'NtUnloadDriver'					NtUnloadDriver						ntdll		634
imp	'NtUnloadKey'						NtUnloadKey						ntdll		635
imp	'NtUnloadKey2'						NtUnloadKey2						ntdll		636
imp	'NtUnloadKeyEx'						NtUnloadKeyEx						ntdll		637
imp	'NtUnlockFile'						NtUnlockFile						ntdll		638
imp	'NtUnlockVirtualMemory'					NtUnlockVirtualMemory					ntdll		639
imp	'NtUnmapViewOfSection'					NtUnmapViewOfSection					ntdll		640	2
imp	'NtUnmapViewOfSectionEx'				NtUnmapViewOfSectionEx					ntdll		641
imp	'NtUnsubscribeWnfStateChange'				NtUnsubscribeWnfStateChange				ntdll		642
imp	'NtUpdateWnfStateData'					NtUpdateWnfStateData					ntdll		643
imp	'NtVdm64CreateProcessInternal'				NtVdm64CreateProcessInternalW				kernel32	1016
imp	'NtVdmControl'						NtVdmControl						ntdll		644
imp	'NtWaitForAlertByThreadId'				NtWaitForAlertByThreadId				ntdll		645
imp	'NtWaitForDebugEvent'					NtWaitForDebugEvent					ntdll		646
imp	'NtWaitForKeyedEvent'					NtWaitForKeyedEvent					ntdll		647	4
imp	'NtWaitForMultipleObjects'				NtWaitForMultipleObjects				ntdll		648
imp	'NtWaitForMultipleObjects32'				NtWaitForMultipleObjects32				ntdll		649
imp	'NtWaitForSingleObject'					NtWaitForSingleObject					ntdll		650	3
imp	'NtWaitForWorkViaWorkerFactory'				NtWaitForWorkViaWorkerFactory				ntdll		651
imp	'NtWaitHighEventPair'					NtWaitHighEventPair					ntdll		652
imp	'NtWaitLowEventPair'					NtWaitLowEventPair					ntdll		653
imp	'NtWorkerFactoryWorkerReady'				NtWorkerFactoryWorkerReady				ntdll		654
imp	'NtWriteFile'						NtWriteFile						ntdll		655	9
imp	'NtWriteFileGather'					NtWriteFileGather					ntdll		656
imp	'NtWriteRequestData'					NtWriteRequestData					ntdll		657
imp	'NtWriteVirtualMemory'					NtWriteVirtualMemory					ntdll		658	5
imp	'NtYieldExecution'					NtYieldExecution					ntdll		659	0
imp	'DefWindowProcA'					DefWindowProcA						user32		173	4
imp	'DefWindowProc'						DefWindowProcW						user32		174	4
imp	'NtdllDefWindowProc_A'					NtdllDefWindowProc_A					ntdll		660
imp	'NtdllDefWindowProc_W'					NtdllDefWindowProc_W					ntdll		661
imp	'NtdllDialogWndProc_A'					NtdllDialogWndProc_A					ntdll		662
imp	'NtdllDialogWndProc_W'					NtdllDialogWndProc_W					ntdll		663
imp	'OOBEComplete'						OOBEComplete						kernel32	1017
imp	'ObjectCloseAuditAlarmA'				ObjectCloseAuditAlarmA					advapi32	1520
imp	'ObjectCloseAuditAlarm'					ObjectCloseAuditAlarmW					advapi32	0		# KernelBase
imp	'ObjectDeleteAuditAlarmA'				ObjectDeleteAuditAlarmA					advapi32	1522
imp	'ObjectDeleteAuditAlarm'				ObjectDeleteAuditAlarmW					advapi32	0		# KernelBase
imp	'ObjectOpenAuditAlarmA'					ObjectOpenAuditAlarmA					advapi32	1524
imp	'ObjectOpenAuditAlarm'					ObjectOpenAuditAlarmW					advapi32	0		# KernelBase
imp	'ObjectPrivilegeAuditAlarmA'				ObjectPrivilegeAuditAlarmA				advapi32	1526
imp	'ObjectPrivilegeAuditAlarm'				ObjectPrivilegeAuditAlarmW				advapi32	0		# KernelBase
imp	'OemKeyScan'						OemKeyScan						user32		2181
imp	'OemToCharA'						OemToCharA						user32		2182
imp	'OemToCharBuffA'					OemToCharBuffA						user32		2183
imp	'OemToCharBuff'						OemToCharBuffW						user32		2184
imp	'OemToChar'						OemToCharW						user32		2185
imp	'OfferVirtualMemory'					OfferVirtualMemory					kernel32	0	3	# KernelBase
imp	'OffsetClipRgn'						OffsetClipRgn						gdi32		1763
imp	'OffsetRect'						OffsetRect						user32		2186
imp	'OffsetRgn'						OffsetRgn						gdi32		1764
imp	'OffsetViewportOrgEx'					OffsetViewportOrgEx					gdi32		1765
imp	'OffsetWindowOrgEx'					OffsetWindowOrgEx					gdi32		1766
imp	'OpenAs_RunDLL'						OpenAs_RunDLL						shell32		81
imp	'OpenAs_RunDLLA'					OpenAs_RunDLLA						shell32		125
imp	'OpenAs_RunDLLW'					OpenAs_RunDLLW						shell32		133
imp	'OpenBackupEventLogA'					OpenBackupEventLogA					advapi32	1528
imp	'OpenBackupEventLog'					OpenBackupEventLogW					advapi32	1529
imp	'OpenClipboard'						OpenClipboard						user32		2187
imp	'OpenCommPort'						OpenCommPort						KernelBase	1036
imp	'OpenConsole'						OpenConsoleW						kernel32	1019
imp	'OpenConsoleWStub'					OpenConsoleWStub					kernel32	1020
imp	'OpenDesktopA'						OpenDesktopA						user32		2188
imp	'OpenDesktop'						OpenDesktopW						user32		2189
imp	'OpenEncryptedFileRawA'					OpenEncryptedFileRawA					advapi32	1530
imp	'OpenEncryptedFileRaw'					OpenEncryptedFileRawW					advapi32	1531
imp	'OpenEventA'						OpenEventA						kernel32	0		# KernelBase
imp	'OpenEventLogA'						OpenEventLogA						advapi32	1532
imp	'OpenEventLog'						OpenEventLogW						advapi32	1533
imp	'OpenEvent'						OpenEventW						kernel32	0		# KernelBase
imp	'OpenFile'						OpenFile						kernel32	1023
imp	'OpenFileById'						OpenFileById						kernel32	0		# KernelBase
imp	'OpenFileMappingA'					OpenFileMappingA					kernel32	1025
imp	'OpenFileMappingFromApp'				OpenFileMappingFromApp					KernelBase	1040
imp	'OpenFileMapping'					OpenFileMappingW					kernel32	0		# KernelBase
imp	'OpenGlobalizationUserSettingsKey'			OpenGlobalizationUserSettingsKey			KernelBase	1042
imp	'OpenIcon'						OpenIcon						user32		2190
imp	'OpenInputDesktop'					OpenInputDesktop					user32		2191
imp	'OpenJobObjectA'					OpenJobObjectA						kernel32	1027
imp	'OpenJobObject'						OpenJobObjectW						kernel32	1028
imp	'OpenMutexA'						OpenMutexA						kernel32	1029
imp	'OpenMutex'						OpenMutexW						kernel32	0		# KernelBase
imp	'OpenPackageInfoByFullName'				OpenPackageInfoByFullName				kernel32	0		# KernelBase
imp	'OpenPackageInfoByFullNameForUser'			OpenPackageInfoByFullNameForUser			KernelBase	1045
imp	'OpenPrivateNamespaceA'					OpenPrivateNamespaceA					kernel32	1032
imp	'OpenPrivateNamespace'					OpenPrivateNamespaceW					kernel32	0		# KernelBase
imp	'OpenProcess'						OpenProcess						kernel32	0	3	# KernelBase
imp	'OpenProcessToken'					OpenProcessToken					advapi32	0	3	# KernelBase
imp	'OpenProfileUserMapping'				OpenProfileUserMapping					kernel32	1036
imp	'OpenRegKey'						OpenRegKey						KernelBase	1049
imp	'OpenRegStream'						OpenRegStream						shell32		85
imp	'OpenSCManagerA'					OpenSCManagerA						advapi32	1535
imp	'OpenSCManager'						OpenSCManagerW						advapi32	1536
imp	'OpenSemaphoreA'					OpenSemaphoreA						kernel32	1037
imp	'OpenSemaphore'						OpenSemaphoreW						kernel32	0		# KernelBase
imp	'OpenServiceA'						OpenServiceA						advapi32	1537
imp	'OpenService'						OpenServiceW						advapi32	1538
imp	'OpenState'						OpenState						KernelBase	1051
imp	'OpenStateAtom'						OpenStateAtom						KernelBase	1052
imp	'OpenStateExplicit'					OpenStateExplicit					KernelBase	1053
imp	'OpenStateExplicitForUserSid'				OpenStateExplicitForUserSid				KernelBase	1054
imp	'OpenStateExplicitForUserSidString'			OpenStateExplicitForUserSidString			KernelBase	1055
imp	'OpenThread'						OpenThread						kernel32	0	3	# KernelBase
imp	'OpenThreadDesktop'					OpenThreadDesktop					user32		2192
imp	'OpenThreadToken'					OpenThreadToken						advapi32	0	4	# KernelBase
imp	'OpenThreadWaitChainSession'				OpenThreadWaitChainSession				advapi32	1540
imp	'OpenTraceA'						OpenTraceA						advapi32	1541
imp	'OpenTrace'						OpenTraceW						advapi32	1542
imp	'OpenURL'						OpenURL							url		111
imp	'OpenURLA'						OpenURLA						url		112
imp	'OpenWaitableTimerA'					OpenWaitableTimerA					kernel32	1043
imp	'OpenWaitableTimer'					OpenWaitableTimerW					kernel32	0		# KernelBase
imp	'OpenWindowStationA'					OpenWindowStationA					user32		2193
imp	'OpenWindowStation'					OpenWindowStationW					user32		2194
imp	'OperationEnd'						OperationEnd						advapi32	1543
imp	'OperationStart'					OperationStart						advapi32	1544
imp	'Options_RunDLL'					Options_RunDLL						shell32		310
imp	'Options_RunDLLA'					Options_RunDLLA						shell32		311
imp	'Options_RunDLLW'					Options_RunDLLW						shell32		312
imp	'OutputDebugStringA'					OutputDebugStringA					kernel32	0		# KernelBase
imp	'OutputDebugString'					OutputDebugStringW					kernel32	0		# KernelBase
imp	'OverrideRoamingDataModificationTimesInRange'		OverrideRoamingDataModificationTimesInRange		KernelBase	1061
imp	'PATHOBJ_bEnum'						PATHOBJ_bEnum						gdi32		1767
imp	'PATHOBJ_bEnumClipLines'				PATHOBJ_bEnumClipLines					gdi32		1768
imp	'PATHOBJ_vEnumStart'					PATHOBJ_vEnumStart					gdi32		1769
imp	'PATHOBJ_vEnumStartClipLines'				PATHOBJ_vEnumStartClipLines				gdi32		1770
imp	'PATHOBJ_vGetBounds'					PATHOBJ_vGetBounds					gdi32		1771
imp	'PackDDElParam'						PackDDElParam						user32		2195
imp	'PackTouchHitTestingProximityEvaluation'		PackTouchHitTestingProximityEvaluation			user32		2196
imp	'PackageFamilyNameFromFullName'				PackageFamilyNameFromFullName				kernel32	0		# KernelBase
imp	'PackageFamilyNameFromFullNameA'			PackageFamilyNameFromFullNameA				KernelBase	1063
imp	'PackageFamilyNameFromId'				PackageFamilyNameFromId					kernel32	0		# KernelBase
imp	'PackageFamilyNameFromIdA'				PackageFamilyNameFromIdA				KernelBase	1065
imp	'PackageFamilyNameFromProductId'			PackageFamilyNameFromProductId				KernelBase	1066
imp	'PackageFullNameFromId'					PackageFullNameFromId					kernel32	0		# KernelBase
imp	'PackageFullNameFromIdA'				PackageFullNameFromIdA					KernelBase	1068
imp	'PackageFullNameFromProductId'				PackageFullNameFromProductId				KernelBase	1069
imp	'PackageIdFromFullName'					PackageIdFromFullName					kernel32	0		# KernelBase
imp	'PackageIdFromFullNameA'				PackageIdFromFullNameA					KernelBase	1071
imp	'PackageIdFromProductId'				PackageIdFromProductId					KernelBase	1072
imp	'PackageNameAndPublisherIdFromFamilyName'		PackageNameAndPublisherIdFromFamilyName			kernel32	0		# KernelBase
imp	'PackageNameAndPublisherIdFromFamilyNameA'		PackageNameAndPublisherIdFromFamilyNameA		KernelBase	1074
imp	'PackageRelativeApplicationIdFromProductId'		PackageRelativeApplicationIdFromProductId		KernelBase	1075
imp	'PackageSidFromFamilyName'				PackageSidFromFamilyName				KernelBase	1076
imp	'PackageSidFromProductId'				PackageSidFromProductId					KernelBase	1
imp	'PageSetupDlgA'						PageSetupDlgA						comdlg32	118
imp	'PageSetupDlg'						PageSetupDlgW						comdlg32	119
imp	'PaintDesktop'						PaintDesktop						user32		2197
imp	'PaintMenuBar'						PaintMenuBar						user32		2198
imp	'PaintMonitor'						PaintMonitor						user32		2199
imp	'PaintRgn'						PaintRgn						gdi32		1772
imp	'ParseApplicationUserModelId'				ParseApplicationUserModelId				kernel32	0		# KernelBase
imp	'ParseApplicationUserModelIdA'				ParseApplicationUserModelIdA				KernelBase	1078
imp	'ParseURLA'						ParseURLA						KernelBase	1079
imp	'ParseURLW'						ParseURLW						KernelBase	1080
imp	'PatBlt'						PatBlt							gdi32		1773
imp	'PathAddBackslashA'					PathAddBackslashA					KernelBase	1081
imp	'PathAddBackslash'					PathAddBackslashW					KernelBase	1082
imp	'PathAddExtensionA'					PathAddExtensionA					KernelBase	1083
imp	'PathAddExtension'					PathAddExtensionW					KernelBase	1084
imp	'PathAllocCanonicalize'					PathAllocCanonicalize					KernelBase	1085
imp	'PathAllocCombine'					PathAllocCombine					KernelBase	1086
imp	'PathAppendA'						PathAppendA						KernelBase	1087
imp	'PathAppend'						PathAppendW						KernelBase	1088
imp	'PathCanonicalizeA'					PathCanonicalizeA					KernelBase	1089
imp	'PathCanonicalize'					PathCanonicalizeW					KernelBase	1090
imp	'PathCchAddBackslash'					PathCchAddBackslash					KernelBase	1091
imp	'PathCchAddBackslashEx'					PathCchAddBackslashEx					KernelBase	1092
imp	'PathCchAddExtension'					PathCchAddExtension					KernelBase	1093
imp	'PathCchAppend'						PathCchAppend						KernelBase	1094
imp	'PathCchAppendEx'					PathCchAppendEx						KernelBase	1095
imp	'PathCchCanonicalize'					PathCchCanonicalize					KernelBase	1096
imp	'PathCchCanonicalizeEx'					PathCchCanonicalizeEx					KernelBase	1097
imp	'PathCchCombine'					PathCchCombine						KernelBase	1098
imp	'PathCchCombineEx'					PathCchCombineEx					KernelBase	1099
imp	'PathCchFindExtension'					PathCchFindExtension					KernelBase	1100
imp	'PathCchIsRoot'						PathCchIsRoot						KernelBase	1101
imp	'PathCchRemoveBackslash'				PathCchRemoveBackslash					KernelBase	1102
imp	'PathCchRemoveBackslashEx'				PathCchRemoveBackslashEx				KernelBase	1103
imp	'PathCchRemoveExtension'				PathCchRemoveExtension					KernelBase	1104
imp	'PathCchRemoveFileSpec'					PathCchRemoveFileSpec					KernelBase	1105
imp	'PathCchRenameExtension'				PathCchRenameExtension					KernelBase	1106
imp	'PathCchSkipRoot'					PathCchSkipRoot						KernelBase	1107
imp	'PathCchStripPrefix'					PathCchStripPrefix					KernelBase	1108
imp	'PathCchStripToRoot'					PathCchStripToRoot					KernelBase	1109
imp	'PathCleanupSpec'					PathCleanupSpec						shell32		171
imp	'PathCombineA'						PathCombineA						KernelBase	1110
imp	'PathCombine'						PathCombineW						KernelBase	1111
imp	'PathCommonPrefixA'					PathCommonPrefixA					KernelBase	1112
imp	'PathCommonPrefix'					PathCommonPrefixW					KernelBase	1113
imp	'PathCreateFromUrlA'					PathCreateFromUrlA					KernelBase	1114
imp	'PathCreateFromUrlAlloc'				PathCreateFromUrlAlloc					KernelBase	1115
imp	'PathCreateFromUrl'					PathCreateFromUrlW					KernelBase	1116
imp	'PathFileExistsA'					PathFileExistsA						KernelBase	1117
imp	'PathFileExists'					PathFileExistsW						KernelBase	1118
imp	'PathFindExtensionA'					PathFindExtensionA					KernelBase	1119
imp	'PathFindExtension'					PathFindExtensionW					KernelBase	1120
imp	'PathFindFileNameA'					PathFindFileNameA					KernelBase	1121
imp	'PathFindFileName'					PathFindFileNameW					KernelBase	1122
imp	'PathFindNextComponentA'				PathFindNextComponentA					KernelBase	1123
imp	'PathFindNextComponent'					PathFindNextComponentW					KernelBase	1124
imp	'PathGetArgsA'						PathGetArgsA						KernelBase	1125
imp	'PathGetArgs'						PathGetArgsW						KernelBase	1126
imp	'PathGetCharTypeA'					PathGetCharTypeA					KernelBase	1127
imp	'PathGetCharType'					PathGetCharTypeW					KernelBase	1128
imp	'PathGetDriveNumberA'					PathGetDriveNumberA					KernelBase	1129
imp	'PathGetDriveNumber'					PathGetDriveNumberW					KernelBase	1130
imp	'PathGetShortPath'					PathGetShortPath					shell32		92
imp	'PathIsExe'						PathIsExe						shell32		43
imp	'PathIsFileSpecA'					PathIsFileSpecA						KernelBase	1131
imp	'PathIsFileSpec'					PathIsFileSpecW						KernelBase	1132
imp	'PathIsLFNFileSpecA'					PathIsLFNFileSpecA					KernelBase	1133
imp	'PathIsLFNFileSpec'					PathIsLFNFileSpecW					KernelBase	1134
imp	'PathIsPrefixA'						PathIsPrefixA						KernelBase	1135
imp	'PathIsPrefix'						PathIsPrefixW						KernelBase	1136
imp	'PathIsRelativeA'					PathIsRelativeA						KernelBase	1137
imp	'PathIsRelative'					PathIsRelativeW						KernelBase	1138
imp	'PathIsRootA'						PathIsRootA						KernelBase	1139
imp	'PathIsRoot'						PathIsRootW						KernelBase	1140
imp	'PathIsSameRootA'					PathIsSameRootA						KernelBase	1141
imp	'PathIsSameRoot'					PathIsSameRootW						KernelBase	1142
imp	'PathIsSlowA'						PathIsSlowA						shell32		240
imp	'PathIsSlow'						PathIsSlowW						shell32		239
imp	'PathIsUNCA'						PathIsUNCA						KernelBase	1143
imp	'PathIsUNCEx'						PathIsUNCEx						KernelBase	1144
imp	'PathIsUNCServerA'					PathIsUNCServerA					KernelBase	1145
imp	'PathIsUNCServerShareA'					PathIsUNCServerShareA					KernelBase	1146
imp	'PathIsUNCServerShare'					PathIsUNCServerShareW					KernelBase	1147
imp	'PathIsUNCServer'					PathIsUNCServerW					KernelBase	1148
imp	'PathIsUNCW'						PathIsUNCW						KernelBase	1149
imp	'PathIsURLA'						PathIsURLA						KernelBase	1150
imp	'PathIsURLW'						PathIsURLW						KernelBase	1151
imp	'PathIsValidCharA'					PathIsValidCharA					KernelBase	1152
imp	'PathIsValidChar'					PathIsValidCharW					KernelBase	1153
imp	'PathMakeUniqueName'					PathMakeUniqueName					shell32		47
imp	'PathMatchSpecA'					PathMatchSpecA						KernelBase	1154
imp	'PathMatchSpecExA'					PathMatchSpecExA					KernelBase	1155
imp	'PathMatchSpecEx'					PathMatchSpecExW					KernelBase	1156
imp	'PathMatchSpec'						PathMatchSpecW						KernelBase	1157
imp	'PathParseIconLocationA'				PathParseIconLocationA					KernelBase	1158
imp	'PathParseIconLocation'					PathParseIconLocationW					KernelBase	1159
imp	'PathQualify'						PathQualify						shell32		49
imp	'PathQuoteSpacesA'					PathQuoteSpacesA					KernelBase	1160
imp	'PathQuoteSpaces'					PathQuoteSpacesW					KernelBase	1161
imp	'PathRelativePathToA'					PathRelativePathToA					KernelBase	1162
imp	'PathRelativePathTo'					PathRelativePathToW					KernelBase	1163
imp	'PathRemoveBackslashA'					PathRemoveBackslashA					KernelBase	1164
imp	'PathRemoveBackslash'					PathRemoveBackslashW					KernelBase	1165
imp	'PathRemoveBlanksA'					PathRemoveBlanksA					KernelBase	1166
imp	'PathRemoveBlanks'					PathRemoveBlanksW					KernelBase	1167
imp	'PathRemoveExtensionA'					PathRemoveExtensionA					KernelBase	1168
imp	'PathRemoveExtension'					PathRemoveExtensionW					KernelBase	1169
imp	'PathRemoveFileSpecA'					PathRemoveFileSpecA					KernelBase	1170
imp	'PathRemoveFileSpec'					PathRemoveFileSpecW					KernelBase	1171
imp	'PathRenameExtensionA'					PathRenameExtensionA					KernelBase	1172
imp	'PathRenameExtension'					PathRenameExtensionW					KernelBase	1173
imp	'PathResolve'						PathResolve						shell32		51
imp	'PathSearchAndQualifyA'					PathSearchAndQualifyA					KernelBase	1174
imp	'PathSearchAndQualify'					PathSearchAndQualifyW					KernelBase	1175
imp	'PathSkipRootA'						PathSkipRootA						KernelBase	1176
imp	'PathSkipRoot'						PathSkipRootW						KernelBase	1177
imp	'PathStripPathA'					PathStripPathA						KernelBase	1178
imp	'PathStripPath'						PathStripPathW						KernelBase	1179
imp	'PathStripToRootA'					PathStripToRootA					KernelBase	1180
imp	'PathStripToRoot'					PathStripToRootW					KernelBase	1181
imp	'PathToRegion'						PathToRegion						gdi32		1774
imp	'PathUnExpandEnvStringsA'				PathUnExpandEnvStringsA					KernelBase	1182
imp	'PathUnExpandEnvStrings'				PathUnExpandEnvStringsW					KernelBase	1183
imp	'PathUnquoteSpacesA'					PathUnquoteSpacesA					KernelBase	1184
imp	'PathUnquoteSpaces'					PathUnquoteSpacesW					KernelBase	1185
imp	'PathYetAnotherMakeUniqueName'				PathYetAnotherMakeUniqueName				shell32		75
imp	'PcwAddQueryItem'					PcwAddQueryItem						KernelBase	1186
imp	'PcwClearCounterSetSecurity'				PcwClearCounterSetSecurity				KernelBase	1187
imp	'PcwCollectData'					PcwCollectData						KernelBase	1188
imp	'PcwCompleteNotification'				PcwCompleteNotification					KernelBase	1189
imp	'PcwCreateNotifier'					PcwCreateNotifier					KernelBase	1190
imp	'PcwCreateQuery'					PcwCreateQuery						KernelBase	1191
imp	'PcwDisconnectCounterSet'				PcwDisconnectCounterSet					KernelBase	1192
imp	'PcwEnumerateInstances'					PcwEnumerateInstances					KernelBase	1193
imp	'PcwIsNotifierAlive'					PcwIsNotifierAlive					KernelBase	1194
imp	'PcwQueryCounterSetSecurity'				PcwQueryCounterSetSecurity				KernelBase	1195
imp	'PcwReadNotificationData'				PcwReadNotificationData					KernelBase	1196
imp	'PcwRegisterCounterSet'					PcwRegisterCounterSet					KernelBase	1197
imp	'PcwRemoveQueryItem'					PcwRemoveQueryItem					KernelBase	1198
imp	'PcwSendNotification'					PcwSendNotification					KernelBase	1199
imp	'PcwSendStatelessNotification'				PcwSendStatelessNotification				KernelBase	1200
imp	'PcwSetCounterSetSecurity'				PcwSetCounterSetSecurity				KernelBase	1201
imp	'PcwSetQueryItemUserData'				PcwSetQueryItemUserData					KernelBase	1202
imp	'PeekConsoleInput'					PeekConsoleInputW					kernel32	0	4	# KernelBase
imp	'PeekConsoleInputA'					PeekConsoleInputA					kernel32	0	4	# KernelBase
imp	'PeekMessageA'						PeekMessageA						user32		2200
imp	'PeekMessage'						PeekMessageW						user32		2201
imp	'PeekNamedPipe'						PeekNamedPipe						kernel32	0	6	# KernelBase
imp	'PerfAddCounters'					PerfAddCounters						advapi32	1545
imp	'PerfCloseQueryHandle'					PerfCloseQueryHandle					advapi32	1546
imp	'PerfCreateInstance'					PerfCreateInstance					advapi32	0		# KernelBase
imp	'PerfDecrementULongCounterValue'			PerfDecrementULongCounterValue				advapi32	0		# KernelBase
imp	'PerfDecrementULongLongCounterValue'			PerfDecrementULongLongCounterValue			advapi32	0		# KernelBase
imp	'PerfDeleteCounters'					PerfDeleteCounters					advapi32	1550
imp	'PerfDeleteInstance'					PerfDeleteInstance					advapi32	0		# KernelBase
imp	'PerfEnumerateCounterSet'				PerfEnumerateCounterSet					advapi32	1552
imp	'PerfEnumerateCounterSetInstances'			PerfEnumerateCounterSetInstances			advapi32	1553
imp	'PerfIncrementULongCounterValue'			PerfIncrementULongCounterValue				advapi32	0		# KernelBase
imp	'PerfIncrementULongLongCounterValue'			PerfIncrementULongLongCounterValue			advapi32	0		# KernelBase
imp	'PerfOpenQueryHandle'					PerfOpenQueryHandle					advapi32	1556
imp	'PerfQueryCounterData'					PerfQueryCounterData					advapi32	1557
imp	'PerfQueryCounterInfo'					PerfQueryCounterInfo					advapi32	1558
imp	'PerfQueryCounterSetRegistrationInfo'			PerfQueryCounterSetRegistrationInfo			advapi32	1559
imp	'PerfQueryInstance'					PerfQueryInstance					advapi32	0		# KernelBase
imp	'PerfRegCloseKey'					PerfRegCloseKey						advapi32	1561
imp	'PerfRegEnumKey'					PerfRegEnumKey						advapi32	1562
imp	'PerfRegEnumValue'					PerfRegEnumValue					advapi32	1563
imp	'PerfRegQueryInfoKey'					PerfRegQueryInfoKey					advapi32	1564
imp	'PerfRegQueryValue'					PerfRegQueryValue					advapi32	1565
imp	'PerfRegSetValue'					PerfRegSetValue						advapi32	1566
imp	'PerfSetCounterRefValue'				PerfSetCounterRefValue					advapi32	0		# KernelBase
imp	'PerfSetCounterSetInfo'					PerfSetCounterSetInfo					advapi32	0		# KernelBase
imp	'PerfSetULongCounterValue'				PerfSetULongCounterValue				advapi32	0		# KernelBase
imp	'PerfSetULongLongCounterValue'				PerfSetULongLongCounterValue				advapi32	0		# KernelBase
imp	'PerfStartProvider'					PerfStartProvider					advapi32	0		# KernelBase
imp	'PerfStartProviderEx'					PerfStartProviderEx					advapi32	0		# KernelBase
imp	'PerfStopProvider'					PerfStopProvider					advapi32	0		# KernelBase
imp	'PfxFindPrefix'						PfxFindPrefix						ntdll		664
imp	'PfxInitialize'						PfxInitialize						ntdll		665
imp	'PfxInsertPrefix'					PfxInsertPrefix						ntdll		666
imp	'PfxRemovePrefix'					PfxRemovePrefix						ntdll		667
imp	'PhysicalToLogicalPoint'				PhysicalToLogicalPoint					user32		2202
imp	'PhysicalToLogicalPointForPerMonitorDPI'		PhysicalToLogicalPointForPerMonitorDPI			user32		2203
imp	'PickIconDlg'						PickIconDlg						shell32		62
imp	'Pie'							Pie							gdi32		1775
imp	'PifMgr_CloseProperties'				PifMgr_CloseProperties					shell32		13
imp	'PifMgr_GetProperties'					PifMgr_GetProperties					shell32		10
imp	'PifMgr_OpenProperties'					PifMgr_OpenProperties					shell32		9
imp	'PifMgr_SetProperties'					PifMgr_SetProperties					shell32		11
imp	'PlayEnhMetaFile'					PlayEnhMetaFile						gdi32		1776
imp	'PlayEnhMetaFileRecord'					PlayEnhMetaFileRecord					gdi32		1777
imp	'PlayMetaFile'						PlayMetaFile						gdi32		1778
imp	'PlayMetaFileRecord'					PlayMetaFileRecord					gdi32		1779
imp	'PlgBlt'						PlgBlt							gdi32		1780
imp	'PolyBezier'						PolyBezier						gdi32		1781
imp	'PolyBezierTo'						PolyBezierTo						gdi32		1782
imp	'PolyDraw'						PolyDraw						gdi32		1783
imp	'PolyPatBlt'						PolyPatBlt						gdi32		1784
imp	'PolyPolygon'						PolyPolygon						gdi32		1785
imp	'PolyPolyline'						PolyPolyline						gdi32		1786
imp	'PolyTextOutA'						PolyTextOutA						gdi32		1787
imp	'PolyTextOut'						PolyTextOutW						gdi32		1788
imp	'Polygon'						Polygon							gdi32		1789
imp	'Polyline'						Polyline						gdi32		1790
imp	'PolylineTo'						PolylineTo						gdi32		1791
imp	'PoolPerAppKeyStateInternal'				PoolPerAppKeyStateInternal				KernelBase	1220
imp	'PostMessageA'						PostMessageA						user32		2204
imp	'PostMessage'						PostMessageW						user32		2205
imp	'PostQueuedCompletionStatus'				PostQueuedCompletionStatus				kernel32	0	4	# KernelBase
imp	'PostQuitMessage'					PostQuitMessage						user32		2206	1
imp	'PostThreadMessageA'					PostThreadMessageA					user32		2207
imp	'PostThreadMessage'					PostThreadMessageW					user32		2208
imp	'PowerClearRequest'					PowerClearRequest					kernel32	1057
imp	'PowerCreateRequest'					PowerCreateRequest					kernel32	1058
imp	'PowerSetRequest'					PowerSetRequest						kernel32	1059
imp	'PrefetchVirtualMemory'					PrefetchVirtualMemory					kernel32	0	4	# KernelBase
imp	'PrepareDiscForBurnRunDll'				PrepareDiscForBurnRunDllW				shell32		135
imp	'PrepareTape'						PrepareTape						kernel32	1061
imp	'PrintDlgA'						PrintDlgA						comdlg32	120	1
imp	'PrintDlgExA'						PrintDlgExA						comdlg32	121
imp	'PrintDlgEx'						PrintDlgExW						comdlg32	122
imp	'PrintDlg'						PrintDlgW						comdlg32	123	1
imp	'PrintWindow'						PrintWindow						user32		2209
imp	'PrintersGetCommand_RunDLL'				PrintersGetCommand_RunDLL				shell32		138
imp	'PrintersGetCommand_RunDLLA'				PrintersGetCommand_RunDLLA				shell32		139
imp	'PrintersGetCommand_RunDLLW'				PrintersGetCommand_RunDLLW				shell32		150
imp	'PrivCopyFileEx'					PrivCopyFileExW						KernelBase	1223
imp	'PrivMoveFileIdentity'					PrivMoveFileIdentityW					kernel32	1063
imp	'PrivateExtractIconExA'					PrivateExtractIconExA					user32		2210
imp	'PrivateExtractIconEx'					PrivateExtractIconExW					user32		2211
imp	'PrivateExtractIconsA'					PrivateExtractIconsA					user32		2212
imp	'PrivateExtractIcons'					PrivateExtractIconsW					user32		2213
imp	'PrivateRegisterICSProc'				PrivateRegisterICSProc					user32		2214
imp	'PrivilegeCheck'					PrivilegeCheck						advapi32	0		# KernelBase
imp	'PrivilegedServiceAuditAlarmA'				PrivilegedServiceAuditAlarmA				advapi32	1575
imp	'PrivilegedServiceAuditAlarm'				PrivilegedServiceAuditAlarmW				advapi32	0		# KernelBase
imp	'Process32First'					Process32FirstW						kernel32	1065
imp	'Process32Next'						Process32NextW						kernel32	1067
imp	'ProcessIdToSessionId'					ProcessIdToSessionId					kernel32	0		# KernelBase
imp	'ProcessIdleTasks'					ProcessIdleTasksW					advapi32	1578
imp	'ProcessTrace'						ProcessTrace						advapi32	1579
imp	'ProductIdFromPackageFamilyName'			ProductIdFromPackageFamilyName				KernelBase	1227
imp	'PsmCreateKey'						PsmCreateKey						KernelBase	1228
imp	'PsmCreateKeyWithDynamicId'				PsmCreateKeyWithDynamicId				KernelBase	1229
imp	'PsmEqualApplication'					PsmEqualApplication					KernelBase	1230
imp	'PsmEqualPackage'					PsmEqualPackage						KernelBase	1231
imp	'PsmGetApplicationNameFromKey'				PsmGetApplicationNameFromKey				KernelBase	1232
imp	'PsmGetKeyFromProcess'					PsmGetKeyFromProcess					KernelBase	1233
imp	'PsmGetKeyFromToken'					PsmGetKeyFromToken					KernelBase	1234
imp	'PsmGetPackageFullNameFromKey'				PsmGetPackageFullNameFromKey				KernelBase	1235
imp	'PsmIsChildKey'						PsmIsChildKey						KernelBase	1236
imp	'PsmIsDynamicKey'					PsmIsDynamicKey						KernelBase	1237
imp	'PsmIsValidKey'						PsmIsValidKey						KernelBase	1238
imp	'PssCaptureSnapshot'					PssCaptureSnapshot					kernel32	0		# KernelBase
imp	'PssDuplicateSnapshot'					PssDuplicateSnapshot					kernel32	0		# KernelBase
imp	'PssFreeSnapshot'					PssFreeSnapshot						kernel32	0		# KernelBase
imp	'PssNtCaptureSnapshot'					PssNtCaptureSnapshot					ntdll		668
imp	'PssNtDuplicateSnapshot'				PssNtDuplicateSnapshot					ntdll		669
imp	'PssNtFreeRemoteSnapshot'				PssNtFreeRemoteSnapshot					ntdll		670
imp	'PssNtFreeSnapshot'					PssNtFreeSnapshot					ntdll		671
imp	'PssNtFreeWalkMarker'					PssNtFreeWalkMarker					ntdll		672
imp	'PssNtQuerySnapshot'					PssNtQuerySnapshot					ntdll		673
imp	'PssNtValidateDescriptor'				PssNtValidateDescriptor					ntdll		674
imp	'PssNtWalkSnapshot'					PssNtWalkSnapshot					ntdll		675
imp	'PssQuerySnapshot'					PssQuerySnapshot					kernel32	0		# KernelBase
imp	'PssWalkMarkerCreate'					PssWalkMarkerCreate					kernel32	0		# KernelBase
imp	'PssWalkMarkerFree'					PssWalkMarkerFree					kernel32	0		# KernelBase
imp	'PssWalkMarkerGetPosition'				PssWalkMarkerGetPosition				kernel32	0		# KernelBase
imp	'PssWalkMarkerRewind'					PssWalkMarkerRewind					kernel32	1076
imp	'PssWalkMarkerSeek'					PssWalkMarkerSeek					kernel32	1077
imp	'PssWalkMarkerSeekToBeginning'				PssWalkMarkerSeekToBeginning				kernel32	0		# KernelBase
imp	'PssWalkMarkerSetPosition'				PssWalkMarkerSetPosition				kernel32	0		# KernelBase
imp	'PssWalkMarkerTell'					PssWalkMarkerTell					kernel32	1080
imp	'PssWalkSnapshot'					PssWalkSnapshot						kernel32	0		# KernelBase
imp	'PtInRect'						PtInRect						user32		2215
imp	'PtInRegion'						PtInRegion						gdi32		1792
imp	'PtVisible'						PtVisible						gdi32		1793
imp	'PublishStateChangeNotification'			PublishStateChangeNotification				KernelBase	1249
imp	'PulseEvent'						PulseEvent						kernel32	0	1	# KernelBase
imp	'PurgeComm'						PurgeComm						kernel32	0		# KernelBase
imp	'QISearch'						QISearch						KernelBase	1252
imp	'QueryActCtxSettings'					QueryActCtxSettingsW					kernel32	0		# KernelBase
imp	'QueryActCtxSettingsWWorker'				QueryActCtxSettingsWWorker				kernel32	1085
imp	'QueryActCtx'						QueryActCtxW						kernel32	0		# KernelBase
imp	'QueryActCtxWWorker'					QueryActCtxWWorker					kernel32	1087
imp	'QueryAllTracesA'					QueryAllTracesA						advapi32	1580
imp	'QueryAllTraces'					QueryAllTracesW						advapi32	1581
imp	'QueryAuxiliaryCounterFrequency'			QueryAuxiliaryCounterFrequency				KernelBase	1255
imp	'QueryBSDRWindow'					QueryBSDRWindow						user32		2216
imp	'QueryDisplayConfig'					QueryDisplayConfig					user32		2217
imp	'QueryDosDeviceA'					QueryDosDeviceA						kernel32	1089
imp	'QueryDosDevice'					QueryDosDeviceW						kernel32	0		# KernelBase
imp	'QueryFontAssocStatus'					QueryFontAssocStatus					gdi32		1794
imp	'QueryFullProcessImageNameA'				QueryFullProcessImageNameA				kernel32	0		# KernelBase
imp	'QueryFullProcessImageName'				QueryFullProcessImageNameW				kernel32	0		# KernelBase
imp	'QueryIdleProcessorCycleTime'				QueryIdleProcessorCycleTime				kernel32	0		# KernelBase
imp	'QueryIdleProcessorCycleTimeEx'				QueryIdleProcessorCycleTimeEx				kernel32	0		# KernelBase
imp	'QueryInformationJobObject'				QueryInformationJobObject				kernel32	1095
imp	'QueryInterruptTime'					QueryInterruptTime					KernelBase	1262
imp	'QueryInterruptTimePrecise'				QueryInterruptTimePrecise				KernelBase	1263
imp	'QueryIoRateControlInformationJobObject'		QueryIoRateControlInformationJobObject			kernel32	1096
imp	'QueryLocalUserServiceName'				QueryLocalUserServiceName				advapi32	1582
imp	'QueryMemoryResourceNotification'			QueryMemoryResourceNotification				kernel32	0		# KernelBase
imp	'QueryOptionalDelayLoadedAPI'				QueryOptionalDelayLoadedAPI				KernelBase	1265
imp	'QueryPerformanceCounter'				QueryPerformanceCounter					kernel32	1098
imp	'QueryPerformanceFrequency'				QueryPerformanceFrequency				kernel32	1099
imp	'QueryProcessAffinityUpdateMode'			QueryProcessAffinityUpdateMode				kernel32	0		# KernelBase
imp	'QueryProcessCycleTime'					QueryProcessCycleTime					kernel32	0		# KernelBase
imp	'QueryProtectedPolicy'					QueryProtectedPolicy					kernel32	0		# KernelBase
imp	'QueryRecoveryAgentsOnEncryptedFile'			QueryRecoveryAgentsOnEncryptedFile			advapi32	1583
imp	'QuerySecurityAccessMask'				QuerySecurityAccessMask					advapi32	0		# KernelBase
imp	'QuerySendMessage'					QuerySendMessage					user32		2218
imp	'QueryServiceConfig2A'					QueryServiceConfig2A					advapi32	1585
imp	'QueryServiceConfig2W'					QueryServiceConfig2W					advapi32	1586
imp	'QueryServiceConfigA'					QueryServiceConfigA					advapi32	1587
imp	'QueryServiceConfig'					QueryServiceConfigW					advapi32	1588
imp	'QueryServiceDynamicInformation'			QueryServiceDynamicInformation				advapi32	1589
imp	'QueryServiceLockStatusA'				QueryServiceLockStatusA					advapi32	1590
imp	'QueryServiceLockStatus'				QueryServiceLockStatusW					advapi32	1591
imp	'QueryServiceObjectSecurity'				QueryServiceObjectSecurity				advapi32	1592
imp	'QueryServiceStatus'					QueryServiceStatus					advapi32	1593
imp	'QueryServiceStatusEx'					QueryServiceStatusEx					advapi32	1594
imp	'QueryStateAtomValueInfo'				QueryStateAtomValueInfo					KernelBase	1272
imp	'QueryStateContainerCreatedNew'				QueryStateContainerCreatedNew				KernelBase	1273
imp	'QueryStateContainerItemInfo'				QueryStateContainerItemInfo				KernelBase	1274
imp	'QueryThreadCycleTime'					QueryThreadCycleTime					kernel32	0		# KernelBase
imp	'QueryThreadProfiling'					QueryThreadProfiling					kernel32	1104
imp	'QueryThreadpoolStackInformation'			QueryThreadpoolStackInformation				kernel32	0		# KernelBase
imp	'QueryTraceA'						QueryTraceA						advapi32	1595
imp	'QueryTraceProcessingHandle'				QueryTraceProcessingHandle				advapi32	1596
imp	'QueryTrace'						QueryTraceW						advapi32	1597
imp	'QueryUmsThreadInformation'				QueryUmsThreadInformation				kernel32	1106
imp	'QueryUnbiasedInterruptTime'				QueryUnbiasedInterruptTime				kernel32	1107
imp	'QueryUnbiasedInterruptTimePrecise'			QueryUnbiasedInterruptTimePrecise			KernelBase	1278
imp	'QueryUserServiceName'					QueryUserServiceName					advapi32	1598
imp	'QueryUserServiceNameForContext'			QueryUserServiceNameForContext				advapi32	1599
imp	'QueryUsersOnEncryptedFile'				QueryUsersOnEncryptedFile				advapi32	1600
imp	'QueryVirtualMemoryInformation'				QueryVirtualMemoryInformation				KernelBase	1279
imp	'QueryWorkingSet'					QueryWorkingSet						KernelBase	1280
imp	'QueryWorkingSetEx'					QueryWorkingSetEx					KernelBase	1281
imp	'QueueUserAPC'						QueueUserAPC						kernel32	0		# KernelBase
imp	'QueueUserWorkItem'					QueueUserWorkItem					kernel32	0		# KernelBase
imp	'QuirkGetData'						QuirkGetData						KernelBase	1284
imp	'QuirkGetData2'						QuirkGetData2						KernelBase	1285
imp	'QuirkGetData2Worker'					QuirkGetData2Worker					kernel32	1110
imp	'QuirkGetDataWorker'					QuirkGetDataWorker					kernel32	1111
imp	'QuirkIsEnabled'					QuirkIsEnabled						KernelBase	1286
imp	'QuirkIsEnabled2'					QuirkIsEnabled2						KernelBase	1287
imp	'QuirkIsEnabled2Worker'					QuirkIsEnabled2Worker					kernel32	1112
imp	'QuirkIsEnabled3'					QuirkIsEnabled3						KernelBase	1288
imp	'QuirkIsEnabled3Worker'					QuirkIsEnabled3Worker					kernel32	1113
imp	'QuirkIsEnabledForPackage'				QuirkIsEnabledForPackage				KernelBase	1289
imp	'QuirkIsEnabledForPackage2'				QuirkIsEnabledForPackage2				KernelBase	1290
imp	'QuirkIsEnabledForPackage2Worker'			QuirkIsEnabledForPackage2Worker				kernel32	1114
imp	'QuirkIsEnabledForPackage3'				QuirkIsEnabledForPackage3				KernelBase	1291
imp	'QuirkIsEnabledForPackage3Worker'			QuirkIsEnabledForPackage3Worker				kernel32	1115
imp	'QuirkIsEnabledForPackage4'				QuirkIsEnabledForPackage4				KernelBase	1292
imp	'QuirkIsEnabledForPackage4Worker'			QuirkIsEnabledForPackage4Worker				kernel32	1116
imp	'QuirkIsEnabledForPackageWorker'			QuirkIsEnabledForPackageWorker				kernel32	1117
imp	'QuirkIsEnabledForProcess'				QuirkIsEnabledForProcess				KernelBase	1293
imp	'QuirkIsEnabledForProcessWorker'			QuirkIsEnabledForProcessWorker				kernel32	1118
imp	'QuirkIsEnabledWorker'					QuirkIsEnabledWorker					kernel32	1119
imp	'RIMAddInputObserver'					RIMAddInputObserver					user32		2219
imp	'RIMAreSiblingDevices'					RIMAreSiblingDevices					user32		2220
imp	'RIMDeviceIoControl'					RIMDeviceIoControl					user32		2221
imp	'RIMEnableMonitorMappingForDevice'			RIMEnableMonitorMappingForDevice			user32		2222
imp	'RIMFreeInputBuffer'					RIMFreeInputBuffer					user32		2223
imp	'RIMGetDevicePreparsedData'				RIMGetDevicePreparsedData				user32		2224
imp	'RIMGetDevicePreparsedDataLockfree'			RIMGetDevicePreparsedDataLockfree			user32		2225
imp	'RIMGetDeviceProperties'				RIMGetDeviceProperties					user32		2226
imp	'RIMGetDevicePropertiesLockfree'			RIMGetDevicePropertiesLockfree				user32		2227
imp	'RIMGetPhysicalDeviceRect'				RIMGetPhysicalDeviceRect				user32		2228
imp	'RIMGetSourceProcessId'					RIMGetSourceProcessId					user32		2229
imp	'RIMObserveNextInput'					RIMObserveNextInput					user32		2230
imp	'RIMOnPnpNotification'					RIMOnPnpNotification					user32		2231
imp	'RIMOnTimerNotification'				RIMOnTimerNotification					user32		2232
imp	'RIMReadInput'						RIMReadInput						user32		2233
imp	'RIMRegisterForInput'					RIMRegisterForInput					user32		2234
imp	'RIMRemoveInputObserver'				RIMRemoveInputObserver					user32		2235
imp	'RIMSetTestModeStatus'					RIMSetTestModeStatus					user32		2236
imp	'RIMUnregisterForInput'					RIMUnregisterForInput					user32		2237
imp	'RIMUpdateInputObserverRegistration'			RIMUpdateInputObserverRegistration			user32		2238
imp	'RaiseCustomSystemEventTrigger'				RaiseCustomSystemEventTrigger				KernelBase	1294
imp	'RaiseException'					RaiseException						kernel32	0		# KernelBase
imp	'RaiseFailFastException'				RaiseFailFastException					kernel32	0		# KernelBase
imp	'RaiseInvalid16BitExeError'				RaiseInvalid16BitExeError				kernel32	1122
imp	'ReOpenFile'						ReOpenFile						kernel32	0	4	# TODO(jart): 6.2 and higher	# KernelBase
imp	'ReadCabinetState'					ReadCabinetState					shell32		654
imp	'ReadConsole'						ReadConsoleW						kernel32	0	5	# KernelBase
imp	'ReadConsoleA'						ReadConsoleA						kernel32	0	5	# KernelBase
imp	'ReadConsoleInput'					ReadConsoleInputW					kernel32	0	4	# KernelBase
imp	'ReadConsoleInputA'					ReadConsoleInputA					kernel32	0	4	# KernelBase
imp	'ReadConsoleInputExA'					ReadConsoleInputExA					KernelBase	1300
imp	'ReadConsoleInputEx'					ReadConsoleInputExW					KernelBase	1301
imp	'ReadConsoleOutput'					ReadConsoleOutputW					kernel32	0	5	# KernelBase
imp	'ReadConsoleOutputA'					ReadConsoleOutputA					kernel32	0	5	# KernelBase
imp	'ReadConsoleOutputAttribute'				ReadConsoleOutputAttribute				kernel32	0	5	# KernelBase
imp	'ReadConsoleOutputCharacter'				ReadConsoleOutputCharacterW				kernel32	0	5	# KernelBase
imp	'ReadConsoleOutputCharacterA'				ReadConsoleOutputCharacterA				kernel32	0	5	# KernelBase
imp	'ReadDirectoryChangesEx'				ReadDirectoryChangesExW					KernelBase	1309
imp	'ReadDirectoryChanges'					ReadDirectoryChangesW					kernel32	0		# KernelBase
imp	'ReadEncryptedFileRaw'					ReadEncryptedFileRaw					advapi32	1601
imp	'ReadEventLogA'						ReadEventLogA						advapi32	1602
imp	'ReadEventLog'						ReadEventLogW						advapi32	1603
imp	'ReadFile'						ReadFile						kernel32	0	5	# KernelBase
imp	'ReadFileEx'						ReadFileEx						kernel32	0	5	# KernelBase
imp	'ReadFileScatter'					ReadFileScatter						kernel32	0	5	# KernelBase
imp	'ReadProcessMemory'					ReadProcessMemory					kernel32	0		# KernelBase
imp	'ReadStateAtomValue'					ReadStateAtomValue					KernelBase	1315
imp	'ReadStateContainerValue'				ReadStateContainerValue					KernelBase	1316
imp	'ReadThreadProfilingData'				ReadThreadProfilingData					kernel32	1141
imp	'RealChildWindowFromPoint'				RealChildWindowFromPoint				user32		2239
imp	'RealDriveType'						RealDriveType						shell32		524
imp	'RealGetWindowClassA'					RealGetWindowClassA					user32		2241
imp	'RealGetWindowClass'					RealGetWindowClassW					user32		2242
imp	'RealShellExecuteA'					RealShellExecuteA					shell32		199
imp	'RealShellExecuteExA'					RealShellExecuteExA					shell32		207
imp	'RealShellExecuteEx'					RealShellExecuteExW					shell32		208
imp	'RealShellExecute'					RealShellExecuteW					shell32		226
imp	'RealizePalette'					RealizePalette						gdi32		1795
imp	'ReasonCodeNeedsBugID'					ReasonCodeNeedsBugID					user32		2243
imp	'ReasonCodeNeedsComment'				ReasonCodeNeedsComment					user32		2244
imp	'ReclaimVirtualMemory'					ReclaimVirtualMemory					kernel32	0		# KernelBase
imp	'RecordShutdownReason'					RecordShutdownReason					user32		2245
imp	'RectInRegion'						RectInRegion						gdi32		1796
imp	'RectVisible'						RectVisible						gdi32		1797
imp	'Rectangle'						Rectangle						gdi32		1798
imp	'RedrawWindow'						RedrawWindow						user32		2246	4
imp	'RefreshPackageInfo'					RefreshPackageInfo					KernelBase	1318
imp	'RefreshPolicyExInternal'				RefreshPolicyExInternal					KernelBase	1319
imp	'RefreshPolicyInternal'					RefreshPolicyInternal					KernelBase	1320
imp	'RegCloseKey'						RegCloseKey						advapi32	0	1	# KernelBase
imp	'RegConnectRegistry'					RegConnectRegistryW					advapi32	1608	3
imp	'RegConnectRegistryA'					RegConnectRegistryA					advapi32	1605	3
imp	'RegConnectRegistryEx'					RegConnectRegistryExW					advapi32	1607	4
imp	'RegConnectRegistryExA'					RegConnectRegistryExA					advapi32	1606	4
imp	'RegCopyTreeA'						RegCopyTreeA						advapi32	1609
imp	'RegCopyTree'						RegCopyTreeW						advapi32	0		# KernelBase
imp	'RegCreateKey'						RegCreateKeyW						advapi32	1616	3
imp	'RegCreateKeyA'						RegCreateKeyA						advapi32	1611	3
imp	'RegCreateKeyEx'					RegCreateKeyExW						advapi32	0	9	# KernelBase
imp	'RegCreateKeyExA'					RegCreateKeyExA						advapi32	0	9	# KernelBase
imp	'RegCreateKeyExInternalA'				RegCreateKeyExInternalA					KernelBase	1324
imp	'RegCreateKeyExInternal'				RegCreateKeyExInternalW					KernelBase	1325
imp	'RegCreateKeyTransactedA'				RegCreateKeyTransactedA					advapi32	1614
imp	'RegCreateKeyTransacted'				RegCreateKeyTransactedW					advapi32	1615
imp	'RegDeleteKey'						RegDeleteKeyW						advapi32	1624	2
imp	'RegDeleteKeyA'						RegDeleteKeyA						advapi32	1617	2
imp	'RegDeleteKeyEx'					RegDeleteKeyExW						advapi32	0	4	# KernelBase
imp	'RegDeleteKeyExA'					RegDeleteKeyExA						advapi32	0	4	# KernelBase
imp	'RegDeleteKeyExInternalA'				RegDeleteKeyExInternalA					KernelBase	1328
imp	'RegDeleteKeyExInternal'				RegDeleteKeyExInternalW					KernelBase	1329
imp	'RegDeleteKeyTransactedA'				RegDeleteKeyTransactedA					advapi32	1620
imp	'RegDeleteKeyTransacted'				RegDeleteKeyTransactedW					advapi32	1621
imp	'RegDeleteKeyValueA'					RegDeleteKeyValueA					advapi32	0		# KernelBase
imp	'RegDeleteKeyValue'					RegDeleteKeyValueW					advapi32	0		# KernelBase
imp	'RegDeleteTree'						RegDeleteTreeW						advapi32	0	2	# KernelBase
imp	'RegDeleteTreeA'					RegDeleteTreeA						advapi32	0	2	# KernelBase
imp	'RegDeleteValue'					RegDeleteValueW						advapi32	0	2	# KernelBase
imp	'RegDeleteValueA'					RegDeleteValueA						advapi32	0	2	# KernelBase
imp	'RegDisablePredefinedCache'				RegDisablePredefinedCache				advapi32	1629	1
imp	'RegDisablePredefinedCacheEx'				RegDisablePredefinedCacheEx				kernel32	0		# KernelBase
imp	'RegDisableReflectionKey'				RegDisableReflectionKey					advapi32	1631	1
imp	'RegEnableReflectionKey'				RegEnableReflectionKey					advapi32	1632	1
imp	'RegEnumKey'						RegEnumKeyW						advapi32	1636	4
imp	'RegEnumKeyA'						RegEnumKeyA						advapi32	1633	4
imp	'RegEnumKeyEx'						RegEnumKeyExW						advapi32	0	8	# KernelBase
imp	'RegEnumKeyExA'						RegEnumKeyExA						advapi32	0	8	# KernelBase
imp	'RegEnumValue'						RegEnumValueW						advapi32	0	8	# KernelBase
imp	'RegEnumValueA'						RegEnumValueA						advapi32	0	8	# KernelBase
imp	'RegFlushKey'						RegFlushKey						advapi32	0	1	# KernelBase
imp	'RegGetKeySecurity'					RegGetKeySecurity					advapi32	0	4	# KernelBase
imp	'RegGetValue'						RegGetValueW						advapi32	0	7	# KernelBase
imp	'RegGetValueA'						RegGetValueA						advapi32	0	7	# KernelBase
imp	'RegKrnGetAppKeyEventAddressInternal'			RegKrnGetAppKeyEventAddressInternal			KernelBase	1346
imp	'RegKrnGetAppKeyLoaded'					RegKrnGetAppKeyLoaded					KernelBase	1347
imp	'RegKrnGetClassesEnumTableAddressInternal'		RegKrnGetClassesEnumTableAddressInternal		KernelBase	1348
imp	'RegKrnGetHKEY_ClassesRootAddress'			RegKrnGetHKEY_ClassesRootAddress			KernelBase	1349
imp	'RegKrnGetTermsrvRegistryExtensionFlags'		RegKrnGetTermsrvRegistryExtensionFlags			KernelBase	1350
imp	'RegKrnResetAppKeyLoaded'				RegKrnResetAppKeyLoaded					KernelBase	1351
imp	'RegKrnSetDllHasThreadStateGlobal'			RegKrnSetDllHasThreadStateGlobal			KernelBase	1352
imp	'RegKrnSetTermsrvRegistryExtensionFlags'		RegKrnSetTermsrvRegistryExtensionFlags			KernelBase	1353
imp	'RegLoadAppKeyA'					RegLoadAppKeyA						advapi32	0		# KernelBase
imp	'RegLoadAppKey'						RegLoadAppKeyW						advapi32	0		# KernelBase
imp	'RegLoadKey'						RegLoadKeyW						advapi32	0	3	# KernelBase
imp	'RegLoadKeyA'						RegLoadKeyA						advapi32	0	3	# KernelBase
imp	'RegLoadMUIStringA'					RegLoadMUIStringA					kernel32	0		# KernelBase
imp	'RegLoadMUIString'					RegLoadMUIStringW					advapi32	0		# KernelBase
imp	'RegNotifyChangeKeyValue'				RegNotifyChangeKeyValue					advapi32	0	5	# KernelBase
imp	'RegOpenCurrentUser'					RegOpenCurrentUser					advapi32	0	2	# KernelBase
imp	'RegOpenKeyA'						RegOpenKeyA						advapi32	1651	3
imp	'RegOpenKeyEx'						RegOpenKeyExW						advapi32	0	5	# KernelBase
imp	'RegOpenKeyExA'						RegOpenKeyExA						advapi32	0	5	# KernelBase
imp	'RegOpenKeyExInternalA'					RegOpenKeyExInternalA					KernelBase	1363
imp	'RegOpenKeyExInternal'					RegOpenKeyExInternalW					KernelBase	1364
imp	'RegOpenKeyTransactedA'					RegOpenKeyTransactedA					advapi32	1654
imp	'RegOpenKeyTransacted'					RegOpenKeyTransactedW					advapi32	1655
imp	'RegOpenKey'						RegOpenKeyW						advapi32	1656
imp	'RegOpenUserClassesRoot'				RegOpenUserClassesRoot					advapi32	0	4	# KernelBase
imp	'RegOverridePredefKey'					RegOverridePredefKey					advapi32	1658	2
imp	'RegQueryInfoKey'					RegQueryInfoKeyW					advapi32	0	12	# KernelBase
imp	'RegQueryInfoKeyA'					RegQueryInfoKeyA					advapi32	0	12	# KernelBase
imp	'RegQueryMultipleValues'				RegQueryMultipleValuesW					advapi32	0	5	# KernelBase
imp	'RegQueryMultipleValuesA'				RegQueryMultipleValuesA					advapi32	0	5	# KernelBase
imp	'RegQueryReflectionKey'					RegQueryReflectionKey					advapi32	1663	2
imp	'RegQueryValue'						RegQueryValueW						advapi32	1667	4
imp	'RegQueryValueA'					RegQueryValueA						advapi32	1664	4
imp	'RegQueryValueEx'					RegQueryValueExW					advapi32	0	6	# KernelBase
imp	'RegQueryValueExA'					RegQueryValueExA					advapi32	0	6	# KernelBase
imp	'RegRenameKey'						RegRenameKey						advapi32	1668
imp	'RegReplaceKey'						RegReplaceKeyW						advapi32	1670	4
imp	'RegReplaceKeyA'					RegReplaceKeyA						advapi32	1669	4
imp	'RegRestoreKey'						RegRestoreKeyW						advapi32	0	3	# KernelBase
imp	'RegRestoreKeyA'					RegRestoreKeyA						advapi32	0	3	# KernelBase
imp	'RegSaveKey'						RegSaveKeyW						advapi32	1676	3
imp	'RegSaveKeyA'						RegSaveKeyA						advapi32	1673	3
imp	'RegSaveKeyExA'						RegSaveKeyExA						kernel32	0		# KernelBase
imp	'RegSaveKeyEx'						RegSaveKeyExW						advapi32	0		# KernelBase
imp	'RegSetKeySecurity'					RegSetKeySecurity					advapi32	0	3	# KernelBase
imp	'RegSetKeyValueA'					RegSetKeyValueA						advapi32	0		# KernelBase
imp	'RegSetKeyValue'					RegSetKeyValueW						advapi32	0		# KernelBase
imp	'RegSetValue'						RegSetValueW						advapi32	1683	5
imp	'RegSetValueA'						RegSetValueA						advapi32	1680	5
imp	'RegSetValueEx'						RegSetValueExW						advapi32	0	6	# KernelBase
imp	'RegSetValueExA'					RegSetValueExA						advapi32	0	6	# KernelBase
imp	'RegUnLoadKey'						RegUnLoadKeyW						advapi32	0	2	# KernelBase
imp	'RegUnLoadKeyA'						RegUnLoadKeyA						advapi32	0	2	# KernelBase
imp	'RegenerateUserEnvironment'				RegenerateUserEnvironment				shell32		313
imp	'RegisterApplicationRecoveryCallback'			RegisterApplicationRecoveryCallback			kernel32	1184
imp	'RegisterApplicationRestart'				RegisterApplicationRestart				kernel32	1185
imp	'RegisterBSDRWindow'					RegisterBSDRWindow					user32		2247
imp	'RegisterBadMemoryNotification'				RegisterBadMemoryNotification				kernel32	0		# KernelBase
imp	'RegisterClassA'					RegisterClassA						user32		2248	1
imp	'RegisterClassExA'					RegisterClassExA					user32		2249	1
imp	'RegisterClassEx'					RegisterClassExW					user32		2250	1
imp	'RegisterClass'						RegisterClassW						user32		2251	1
imp	'RegisterClipboardFormatA'				RegisterClipboardFormatA				user32		2252
imp	'RegisterClipboardFormat'				RegisterClipboardFormatW				user32		2253
imp	'RegisterConsoleIME'					RegisterConsoleIME					kernel32	1187
imp	'RegisterConsoleOS2'					RegisterConsoleOS2					kernel32	1188
imp	'RegisterConsoleVDM'					RegisterConsoleVDM					kernel32	1189
imp	'RegisterDManipHook'					RegisterDManipHook					user32		2254
imp	'RegisterDeviceNotificationA'				RegisterDeviceNotificationA				user32		2255
imp	'RegisterDeviceNotification'				RegisterDeviceNotificationW				user32		2256
imp	'RegisterErrorReportingDialog'				RegisterErrorReportingDialog				user32		2257
imp	'RegisterEventSourceA'					RegisterEventSourceA					advapi32	1686
imp	'RegisterEventSource'					RegisterEventSourceW					advapi32	1687
imp	'RegisterFrostWindow'					RegisterFrostWindow					user32		2258
imp	'RegisterGPNotificationInternal'			RegisterGPNotificationInternal				KernelBase	1385
imp	'RegisterGhostWindow'					RegisterGhostWindow					user32		2259
imp	'RegisterHotKey'					RegisterHotKey						user32		2260
imp	'RegisterIdleTask'					RegisterIdleTask					advapi32	1688
imp	'RegisterLogonProcess'					RegisterLogonProcess					user32		2261
imp	'RegisterMessagePumpHook'				RegisterMessagePumpHook					user32		2262
imp	'RegisterPointerDeviceNotifications'			RegisterPointerDeviceNotifications			user32		2263
imp	'RegisterPointerInputTarget'				RegisterPointerInputTarget				user32		2264
imp	'RegisterPointerInputTargetEx'				RegisterPointerInputTargetEx				user32		2265
imp	'RegisterPowerSettingNotification'			RegisterPowerSettingNotification			user32		2266
imp	'RegisterRawInputDevices'				RegisterRawInputDevices					user32		2267
imp	'RegisterServiceCtrlHandlerA'				RegisterServiceCtrlHandlerA				advapi32	1689
imp	'RegisterServiceCtrlHandlerExA'				RegisterServiceCtrlHandlerExA				advapi32	1690
imp	'RegisterServiceCtrlHandlerEx'				RegisterServiceCtrlHandlerExW				advapi32	1691
imp	'RegisterServiceCtrlHandler'				RegisterServiceCtrlHandlerW				advapi32	1692
imp	'RegisterServicesProcess'				RegisterServicesProcess					user32		2268
imp	'RegisterSessionPort'					RegisterSessionPort					user32		2269
imp	'RegisterShellHookWindow'				RegisterShellHookWindow					user32		2270
imp	'RegisterStateChangeNotification'			RegisterStateChangeNotification				KernelBase	1386
imp	'RegisterStateLock'					RegisterStateLock					KernelBase	1387
imp	'RegisterSuspendResumeNotification'			RegisterSuspendResumeNotification			user32		2271
imp	'RegisterSystemThread'					RegisterSystemThread					user32		2272
imp	'RegisterTasklist'					RegisterTasklist					user32		2273
imp	'RegisterTouchHitTestingWindow'				RegisterTouchHitTestingWindow				user32		2274
imp	'RegisterTouchWindow'					RegisterTouchWindow					user32		2275
imp	'RegisterUserApiHook'					RegisterUserApiHook					user32		2276
imp	'RegisterWaitChainCOMCallback'				RegisterWaitChainCOMCallback				advapi32	1695
imp	'RegisterWaitForInputIdle'				RegisterWaitForInputIdle				kernel32	1190
imp	'RegisterWaitForSingleObject'				RegisterWaitForSingleObject				kernel32	1191
imp	'RegisterWaitForSingleObjectEx'				RegisterWaitForSingleObjectEx				KernelBase	1389
imp	'RegisterWaitUntilOOBECompleted'			RegisterWaitUntilOOBECompleted				kernel32	1193
imp	'RegisterWindowMessageA'				RegisterWindowMessageA					user32		2277
imp	'RegisterWindowMessage'					RegisterWindowMessageW					user32		2278
imp	'RegisterWowBaseHandlers'				RegisterWowBaseHandlers					kernel32	1194
imp	'RegisterWowExec'					RegisterWowExec						kernel32	1195
imp	'ReleaseActCtx'						ReleaseActCtx						kernel32	0		# KernelBase
imp	'ReleaseActCtxWorker'					ReleaseActCtxWorker					kernel32	1197
imp	'ReleaseCapture'					ReleaseCapture						user32		2279	0
imp	'ReleaseDC'						ReleaseDC						user32		2280
imp	'ReleaseDwmHitTestWaiters'				ReleaseDwmHitTestWaiters				user32		2281
imp	'ReleaseMutex'						ReleaseMutex						kernel32	0	1	# KernelBase
imp	'ReleaseSemaphore'					ReleaseSemaphore					kernel32	0	3	# KernelBase
imp	'ReleaseStateLock'					ReleaseStateLock					KernelBase	1397
imp	'RemapPredefinedHandleInternal'				RemapPredefinedHandleInternal				KernelBase	1398
imp	'RemoteRegEnumKeyWrapper'				RemoteRegEnumKeyWrapper					advapi32	1696
imp	'RemoteRegEnumValueWrapper'				RemoteRegEnumValueWrapper				advapi32	1697
imp	'RemoteRegQueryInfoKeyWrapper'				RemoteRegQueryInfoKeyWrapper				advapi32	1698
imp	'RemoteRegQueryMultipleValues2Wrapper'			RemoteRegQueryMultipleValues2Wrapper			advapi32	1699
imp	'RemoteRegQueryMultipleValuesWrapper'			RemoteRegQueryMultipleValuesWrapper			advapi32	1700
imp	'RemoteRegQueryValueWrapper'				RemoteRegQueryValueWrapper				advapi32	1701
imp	'RemoveClipboardFormatListener'				RemoveClipboardFormatListener				user32		2282
imp	'RemoveDirectory'					RemoveDirectoryW					kernel32	0	1	# KernelBase
imp	'RemoveDirectoryA'					RemoveDirectoryA					kernel32	0	1	# KernelBase
imp	'RemoveDirectoryTransactedA'				RemoveDirectoryTransactedA				kernel32	1205
imp	'RemoveDirectoryTransacted'				RemoveDirectoryTransactedW				kernel32	1206
imp	'RemoveDllDirectory'					RemoveDllDirectory					kernel32	0		# KernelBase
imp	'RemoveExtensionProgIds'				RemoveExtensionProgIds					KernelBase	1402
imp	'RemoveFontMemResourceEx'				RemoveFontMemResourceEx					gdi32		1799
imp	'RemoveFontResourceA'					RemoveFontResourceA					gdi32		1800
imp	'RemoveFontResourceExA'					RemoveFontResourceExA					gdi32		1801
imp	'RemoveFontResourceEx'					RemoveFontResourceExW					gdi32		1802
imp	'RemoveFontResourceTracking'				RemoveFontResourceTracking				gdi32		1803
imp	'RemoveFontResource'					RemoveFontResourceW					gdi32		1804
imp	'RemoveInjectionDevice'					RemoveInjectionDevice					user32		2283
imp	'RemoveLocalAlternateComputerNameA'			RemoveLocalAlternateComputerNameA			kernel32	1209
imp	'RemoveLocalAlternateComputerName'			RemoveLocalAlternateComputerNameW			kernel32	1210
imp	'RemoveMenu'						RemoveMenu						user32		2284
imp	'RemovePackageFromFamilyXref'				RemovePackageFromFamilyXref				KernelBase	1403
imp	'RemovePackageStatus'					RemovePackageStatus					KernelBase	1404
imp	'RemovePackageStatusForUser'				RemovePackageStatusForUser				KernelBase	1405
imp	'RemovePropA'						RemovePropA						user32		2285
imp	'RemoveProp'						RemovePropW						user32		2286
imp	'RemoveSecureMemoryCacheCallback'			RemoveSecureMemoryCacheCallback				kernel32	1211
imp	'RemoveThreadTSFEventAwareness'				RemoveThreadTSFEventAwareness				user32		2287
imp	'RemoveUsersFromEncryptedFile'				RemoveUsersFromEncryptedFile				advapi32	1703
imp	'ReplaceFileA'						ReplaceFileA						kernel32	1215
imp	'ReplaceFileExInternal'					ReplaceFileExInternal					KernelBase	1408
imp	'ReplaceFile'						ReplaceFileW						kernel32	0		# KernelBase
imp	'ReplacePartitionUnit'					ReplacePartitionUnit					kernel32	1217
imp	'ReplaceTextA'						ReplaceTextA						comdlg32	124	1
imp	'ReplaceText'						ReplaceTextW						comdlg32	125	1
imp	'ReplyMessage'						ReplyMessage						user32		2288
imp	'ReportEventA'						ReportEventA						advapi32	1704
imp	'ReportEvent'						ReportEventW						advapi32	1705
imp	'ReportInertia'						ReportInertia						user32		2551
imp	'RequestDeviceWakeup'					RequestDeviceWakeup					kernel32	1218
imp	'RequestWakeupLatency'					RequestWakeupLatency					kernel32	1219
imp	'ResetDCA'						ResetDCA						gdi32		1805
imp	'ResetDCW'						ResetDCW						gdi32		1806
imp	'ResetEvent'						ResetEvent						kernel32	0	1	# KernelBase
imp	'ResetState'						ResetState						KernelBase	1411
imp	'ResetWriteWatch'					ResetWriteWatch						kernel32	0		# KernelBase
imp	'ResizePalette'						ResizePalette						gdi32		1807
imp	'ResolveDelayLoadedAPI'					ResolveDelayLoadedAPI					kernel32	0		# KernelBase
imp	'ResolveDelayLoadsFromDll'				ResolveDelayLoadsFromDll				kernel32	0		# KernelBase
imp	'ResolveDesktopForWOW'					ResolveDesktopForWOW					user32		2289
imp	'ResolveLocaleName'					ResolveLocaleName					kernel32	0		# KernelBase
imp	'RestartDialog'						RestartDialog						shell32		59
imp	'RestartDialogEx'					RestartDialogEx						shell32		730
imp	'RestoreDC'						RestoreDC						gdi32		1808	2
imp	'ResumeThread'						ResumeThread						kernel32	0		# KernelBase
imp	'ReuseDDElParam'					ReuseDDElParam						user32		2290
imp	'RevertToSelf'						RevertToSelf						advapi32	0		# KernelBase
imp	'RoundRect'						RoundRect						gdi32		1809
imp	'RsopLoggingEnabledInternal'				RsopLoggingEnabledInternal				KernelBase	1419
imp	'RtlAbortRXact'						RtlAbortRXact						ntdll		676
imp	'RtlAbsoluteToSelfRelativeSD'				RtlAbsoluteToSelfRelativeSD				ntdll		677
imp	'RtlAcquirePebLock'					RtlAcquirePebLock					ntdll		678
imp	'RtlAcquirePrivilege'					RtlAcquirePrivilege					ntdll		679
imp	'RtlAcquireReleaseSRWLockExclusive'			RtlAcquireReleaseSRWLockExclusive			ntdll		680
imp	'RtlAcquireResourceExclusive'				RtlAcquireResourceExclusive				ntdll		681
imp	'RtlAcquireResourceShared'				RtlAcquireResourceShared				ntdll		682
imp	'RtlAcquireSRWLockExclusive'				RtlAcquireSRWLockExclusive				ntdll		683
imp	'RtlAcquireSRWLockShared'				RtlAcquireSRWLockShared					ntdll		684
imp	'RtlActivateActivationContext'				RtlActivateActivationContext				ntdll		685
imp	'RtlActivateActivationContextEx'			RtlActivateActivationContextEx				ntdll		686
imp	'RtlActivateActivationContextUnsafeFast'		RtlActivateActivationContextUnsafeFast			ntdll		687
imp	'RtlAddAccessAllowedAce'				RtlAddAccessAllowedAce					ntdll		688
imp	'RtlAddAccessAllowedAceEx'				RtlAddAccessAllowedAceEx				ntdll		689
imp	'RtlAddAccessAllowedObjectAce'				RtlAddAccessAllowedObjectAce				ntdll		690
imp	'RtlAddAccessDeniedAce'					RtlAddAccessDeniedAce					ntdll		691
imp	'RtlAddAccessDeniedAceEx'				RtlAddAccessDeniedAceEx					ntdll		692
imp	'RtlAddAccessDeniedObjectAce'				RtlAddAccessDeniedObjectAce				ntdll		693
imp	'RtlAddAccessFilterAce'					RtlAddAccessFilterAce					ntdll		694
imp	'RtlAddAce'						RtlAddAce						ntdll		695
imp	'RtlAddActionToRXact'					RtlAddActionToRXact					ntdll		696
imp	'RtlAddAtomToAtomTable'					RtlAddAtomToAtomTable					ntdll		697
imp	'RtlAddAttributeActionToRXact'				RtlAddAttributeActionToRXact				ntdll		698
imp	'RtlAddAuditAccessAce'					RtlAddAuditAccessAce					ntdll		699
imp	'RtlAddAuditAccessAceEx'				RtlAddAuditAccessAceEx					ntdll		700
imp	'RtlAddAuditAccessObjectAce'				RtlAddAuditAccessObjectAce				ntdll		701
imp	'RtlAddCompoundAce'					RtlAddCompoundAce					ntdll		702
imp	'RtlAddFunctionTable'					RtlAddFunctionTable					ntdll		703
imp	'RtlAddGrowableFunctionTable'				RtlAddGrowableFunctionTable				ntdll		704
imp	'RtlAddIntegrityLabelToBoundaryDescriptor'		RtlAddIntegrityLabelToBoundaryDescriptor		ntdll		705
imp	'RtlAddMandatoryAce'					RtlAddMandatoryAce					ntdll		706
imp	'RtlAddProcessTrustLabelAce'				RtlAddProcessTrustLabelAce				ntdll		707
imp	'RtlAddRefActivationContext'				RtlAddRefActivationContext				ntdll		708
imp	'RtlAddRefMemoryStream'					RtlAddRefMemoryStream					ntdll		709
imp	'RtlAddResourceAttributeAce'				RtlAddResourceAttributeAce				ntdll		710
imp	'RtlAddSIDToBoundaryDescriptor'				RtlAddSIDToBoundaryDescriptor				ntdll		711
imp	'RtlAddScopedPolicyIDAce'				RtlAddScopedPolicyIDAce					ntdll		712
imp	'RtlAddVectoredContinueHandler'				RtlAddVectoredContinueHandler				ntdll		713
imp	'RtlAddVectoredExceptionHandler'			RtlAddVectoredExceptionHandler				ntdll		714
imp	'RtlAddressInSectionTable'				RtlAddressInSectionTable				ntdll		715
imp	'RtlAdjustPrivilege'					RtlAdjustPrivilege					ntdll		716
imp	'RtlAllocateActivationContextStack'			RtlAllocateActivationContextStack			ntdll		717
imp	'RtlAllocateAndInitializeSid'				RtlAllocateAndInitializeSid				ntdll		718
imp	'RtlAllocateAndInitializeSidEx'				RtlAllocateAndInitializeSidEx				ntdll		719
imp	'RtlAllocateHandle'					RtlAllocateHandle					ntdll		720
imp	'RtlAllocateHeap'					RtlAllocateHeap						ntdll		721	3
imp	'RtlAllocateMemoryBlockLookaside'			RtlAllocateMemoryBlockLookaside				ntdll		722
imp	'RtlAllocateMemoryZone'					RtlAllocateMemoryZone					ntdll		723
imp	'RtlAllocateWnfSerializationGroup'			RtlAllocateWnfSerializationGroup			ntdll		724
imp	'RtlAnsiCharToUnicodeChar'				RtlAnsiCharToUnicodeChar				ntdll		725
imp	'RtlAnsiStringToUnicodeSize'				RtlAnsiStringToUnicodeSize				ntdll		726
imp	'RtlAnsiStringToUnicodeString'				RtlAnsiStringToUnicodeString				ntdll		727
imp	'RtlAppendAsciizToString'				RtlAppendAsciizToString					ntdll		728
imp	'RtlAppendPathElement'					RtlAppendPathElement					ntdll		729
imp	'RtlAppendStringToString'				RtlAppendStringToString					ntdll		730
imp	'RtlAppendUnicodeStringToString'			RtlAppendUnicodeStringToString				ntdll		731
imp	'RtlAppendUnicodeToString'				RtlAppendUnicodeToString				ntdll		732
imp	'RtlApplicationVerifierStop'				RtlApplicationVerifierStop				ntdll		733
imp	'RtlApplyRXact'						RtlApplyRXact						ntdll		734
imp	'RtlApplyRXactNoFlush'					RtlApplyRXactNoFlush					ntdll		735
imp	'RtlAppxIsFileOwnedByTrustedInstaller'			RtlAppxIsFileOwnedByTrustedInstaller			ntdll		736
imp	'RtlAreAllAccessesGranted'				RtlAreAllAccessesGranted				ntdll		737
imp	'RtlAreAnyAccessesGranted'				RtlAreAnyAccessesGranted				ntdll		738
imp	'RtlAreBitsClear'					RtlAreBitsClear						ntdll		739
imp	'RtlAreBitsSet'						RtlAreBitsSet						ntdll		740
imp	'RtlAreLongPathsEnabled'				RtlAreLongPathsEnabled					ntdll		741
imp	'RtlAssert'						RtlAssert						ntdll		742
imp	'RtlAvlInsertNodeEx'					RtlAvlInsertNodeEx					ntdll		743
imp	'RtlAvlRemoveNode'					RtlAvlRemoveNode					ntdll		744
imp	'RtlBarrier'						RtlBarrier						ntdll		745
imp	'RtlBarrierForDelete'					RtlBarrierForDelete					ntdll		746
imp	'RtlCallEnclaveReturn'					RtlCallEnclaveReturn					ntdll		747
imp	'RtlCancelTimer'					RtlCancelTimer						ntdll		748
imp	'RtlCanonicalizeDomainName'				RtlCanonicalizeDomainName				ntdll		749
imp	'RtlCapabilityCheck'					RtlCapabilityCheck					ntdll		750
imp	'RtlCapabilityCheckForSingleSessionSku'			RtlCapabilityCheckForSingleSessionSku			ntdll		751
imp	'RtlCaptureContext'					RtlCaptureContext					ntdll		752
imp	'RtlCaptureStackBackTrace'				RtlCaptureStackBackTrace				ntdll		753
imp	'RtlCharToInteger'					RtlCharToInteger					ntdll		754
imp	'RtlCheckBootStatusIntegrity'				RtlCheckBootStatusIntegrity				ntdll		755
imp	'RtlCheckForOrphanedCriticalSections'			RtlCheckForOrphanedCriticalSections			ntdll		756
imp	'RtlCheckPortableOperatingSystem'			RtlCheckPortableOperatingSystem				ntdll		757
imp	'RtlCheckRegistryKey'					RtlCheckRegistryKey					ntdll		758
imp	'RtlCheckSandboxedToken'				RtlCheckSandboxedToken					ntdll		759
imp	'RtlCheckSystemBootStatusIntegrity'			RtlCheckSystemBootStatusIntegrity			ntdll		760
imp	'RtlCheckTokenCapability'				RtlCheckTokenCapability					ntdll		761
imp	'RtlCheckTokenMembership'				RtlCheckTokenMembership					ntdll		762
imp	'RtlCheckTokenMembershipEx'				RtlCheckTokenMembershipEx				ntdll		763
imp	'RtlCleanUpTEBLangLists'				RtlCleanUpTEBLangLists					ntdll		764
imp	'RtlClearAllBits'					RtlClearAllBits						ntdll		765
imp	'RtlClearBit'						RtlClearBit						ntdll		766
imp	'RtlClearBits'						RtlClearBits						ntdll		767
imp	'RtlClearThreadWorkOnBehalfTicket'			RtlClearThreadWorkOnBehalfTicket			ntdll		768
imp	'RtlCloneMemoryStream'					RtlCloneMemoryStream					ntdll		769
imp	'RtlCloneUserProcess'					RtlCloneUserProcess					ntdll		770	5
imp	'RtlCmDecodeMemIoResource'				RtlCmDecodeMemIoResource				ntdll		771
imp	'RtlCmEncodeMemIoResource'				RtlCmEncodeMemIoResource				ntdll		772
imp	'RtlCommitDebugInfo'					RtlCommitDebugInfo					ntdll		773
imp	'RtlCommitMemoryStream'					RtlCommitMemoryStream					ntdll		774
imp	'RtlCompactHeap'					RtlCompactHeap						ntdll		775
imp	'RtlCompareAltitudes'					RtlCompareAltitudes					ntdll		776
imp	'RtlCompareMemory'					RtlCompareMemory					ntdll		777
imp	'RtlCompareMemoryUlong'					RtlCompareMemoryUlong					ntdll		778
imp	'RtlCompareString'					RtlCompareString					ntdll		779
imp	'RtlCompareUnicodeString'				RtlCompareUnicodeString					ntdll		780
imp	'RtlCompareUnicodeStrings'				RtlCompareUnicodeStrings				ntdll		781
imp	'RtlCompleteProcessCloning'				RtlCompleteProcessCloning				ntdll		782
imp	'RtlCompressBuffer'					RtlCompressBuffer					ntdll		783
imp	'RtlComputeCrc32'					RtlComputeCrc32						ntdll		784
imp	'RtlComputeImportTableHash'				RtlComputeImportTableHash				ntdll		785
imp	'RtlComputePrivatizedDllName_U'				RtlComputePrivatizedDllName_U				ntdll		786
imp	'RtlConnectToSm'					RtlConnectToSm						ntdll		787
imp	'RtlConsoleMultiByteToUnicodeN'				RtlConsoleMultiByteToUnicodeN				ntdll		788
imp	'RtlContractHashTable'					RtlContractHashTable					ntdll		789
imp	'RtlConvertDeviceFamilyInfoToString'			RtlConvertDeviceFamilyInfoToString			ntdll		790
imp	'RtlConvertExclusiveToShared'				RtlConvertExclusiveToShared				ntdll		791
imp	'RtlConvertLCIDToString'				RtlConvertLCIDToString					ntdll		792
imp	'RtlConvertSRWLockExclusiveToShared'			RtlConvertSRWLockExclusiveToShared			ntdll		793
imp	'RtlConvertSharedToExclusive'				RtlConvertSharedToExclusive				ntdll		794
imp	'RtlConvertSidToUnicodeString'				RtlConvertSidToUnicodeString				ntdll		795	3
imp	'RtlConvertToAutoInheritSecurityObject'			RtlConvertToAutoInheritSecurityObject			ntdll		796
imp	'RtlCopyBitMap'						RtlCopyBitMap						ntdll		797
imp	'RtlCopyContext'					RtlCopyContext						ntdll		798
imp	'RtlCopyExtendedContext'				RtlCopyExtendedContext					ntdll		799
imp	'RtlCopyLuid'						RtlCopyLuid						ntdll		800
imp	'RtlCopyLuidAndAttributesArray'				RtlCopyLuidAndAttributesArray				ntdll		801
imp	'RtlCopyMappedMemory'					RtlCopyMappedMemory					ntdll		802
imp	'RtlCopyMemory'						RtlCopyMemory						ntdll		803
imp	'RtlCopyMemoryNonTemporal'				RtlCopyMemoryNonTemporal				ntdll		804
imp	'RtlCopyMemoryStreamTo'					RtlCopyMemoryStreamTo					ntdll		805
imp	'RtlCopyOutOfProcessMemoryStreamTo'			RtlCopyOutOfProcessMemoryStreamTo			ntdll		806
imp	'RtlCopySecurityDescriptor'				RtlCopySecurityDescriptor				ntdll		807
imp	'RtlCopySid'						RtlCopySid						ntdll		808
imp	'RtlCopySidAndAttributesArray'				RtlCopySidAndAttributesArray				ntdll		809
imp	'RtlCopyString'						RtlCopyString						ntdll		810
imp	'RtlCopyUnicodeString'					RtlCopyUnicodeString					ntdll		811
imp	'RtlCrc32'						RtlCrc32						ntdll		812
imp	'RtlCrc64'						RtlCrc64						ntdll		813
imp	'RtlCreateAcl'						RtlCreateAcl						ntdll		814
imp	'RtlCreateActivationContext'				RtlCreateActivationContext				ntdll		815
imp	'RtlCreateAndSetSD'					RtlCreateAndSetSD					ntdll		816
imp	'RtlCreateAtomTable'					RtlCreateAtomTable					ntdll		817
imp	'RtlCreateBootStatusDataFile'				RtlCreateBootStatusDataFile				ntdll		818
imp	'RtlCreateBoundaryDescriptor'				RtlCreateBoundaryDescriptor				ntdll		819
imp	'RtlCreateEnvironment'					RtlCreateEnvironment					ntdll		820
imp	'RtlCreateEnvironmentEx'				RtlCreateEnvironmentEx					ntdll		821
imp	'RtlCreateHashTable'					RtlCreateHashTable					ntdll		822
imp	'RtlCreateHashTableEx'					RtlCreateHashTableEx					ntdll		823
imp	'RtlCreateHeap'						RtlCreateHeap						ntdll		824	6
imp	'RtlCreateMemoryBlockLookaside'				RtlCreateMemoryBlockLookaside				ntdll		825
imp	'RtlCreateMemoryZone'					RtlCreateMemoryZone					ntdll		826
imp	'RtlCreateProcessParameters'				RtlCreateProcessParameters				ntdll		827	10
imp	'RtlCreateProcessParametersEx'				RtlCreateProcessParametersEx				ntdll		828
imp	'RtlCreateProcessReflection'				RtlCreateProcessReflection				ntdll		829
imp	'RtlCreateQueryDebugBuffer'				RtlCreateQueryDebugBuffer				ntdll		830
imp	'RtlCreateRegistryKey'					RtlCreateRegistryKey					ntdll		831
imp	'RtlCreateSecurityDescriptor'				RtlCreateSecurityDescriptor				ntdll		832
imp	'RtlCreateServiceSid'					RtlCreateServiceSid					ntdll		833
imp	'RtlCreateSystemVolumeInformationFolder'		RtlCreateSystemVolumeInformationFolder			ntdll		834
imp	'RtlCreateTagHeap'					RtlCreateTagHeap					ntdll		835
imp	'RtlCreateTimer'					RtlCreateTimer						ntdll		836
imp	'RtlCreateTimerQueue'					RtlCreateTimerQueue					ntdll		837
imp	'RtlCreateUmsCompletionList'				RtlCreateUmsCompletionList				ntdll		838
imp	'RtlCreateUmsThreadContext'				RtlCreateUmsThreadContext				ntdll		839
imp	'RtlCreateUnicodeString'				RtlCreateUnicodeString					ntdll		840
imp	'RtlCreateUnicodeStringFromAsciiz'			RtlCreateUnicodeStringFromAsciiz			ntdll		841
imp	'RtlCreateUserProcess'					RtlCreateUserProcess					ntdll		842
imp	'RtlCreateUserProcessEx'				RtlCreateUserProcessEx					ntdll		843
imp	'RtlCreateUserSecurityObject'				RtlCreateUserSecurityObject				ntdll		844
imp	'RtlCreateUserStack'					RtlCreateUserStack					ntdll		845
imp	'RtlCreateUserThread'					RtlCreateUserThread					ntdll		846
imp	'RtlCreateVirtualAccountSid'				RtlCreateVirtualAccountSid				ntdll		847
imp	'RtlCultureNameToLCID'					RtlCultureNameToLCID					ntdll		848
imp	'RtlCustomCPToUnicodeN'					RtlCustomCPToUnicodeN					ntdll		849
imp	'RtlCutoverTimeToSystemTime'				RtlCutoverTimeToSystemTime				ntdll		850
imp	'RtlDeCommitDebugInfo'					RtlDeCommitDebugInfo					ntdll		851
imp	'RtlDeNormalizeProcessParams'				RtlDeNormalizeProcessParams				ntdll		852
imp	'RtlDeactivateActivationContext'			RtlDeactivateActivationContext				ntdll		853
imp	'RtlDeactivateActivationContextUnsafeFast'		RtlDeactivateActivationContextUnsafeFast		ntdll		854
imp	'RtlDebugPrintTimes'					RtlDebugPrintTimes					ntdll		855
imp	'RtlDecodePointer'					RtlDecodePointer					ntdll		856
imp	'RtlDecodeRemotePointer'				RtlDecodeRemotePointer					ntdll		857
imp	'RtlDecodeSystemPointer'				RtlDecodeSystemPointer					ntdll		858
imp	'RtlDecompressBuffer'					RtlDecompressBuffer					ntdll		859
imp	'RtlDecompressBufferEx'					RtlDecompressBufferEx					ntdll		860
imp	'RtlDecompressFragment'					RtlDecompressFragment					ntdll		861
imp	'RtlDefaultNpAcl'					RtlDefaultNpAcl						ntdll		862
imp	'RtlDelete'						RtlDelete						ntdll		863
imp	'RtlDeleteAce'						RtlDeleteAce						ntdll		864
imp	'RtlDeleteAtomFromAtomTable'				RtlDeleteAtomFromAtomTable				ntdll		865
imp	'RtlDeleteBarrier'					RtlDeleteBarrier					ntdll		866
imp	'RtlDeleteBoundaryDescriptor'				RtlDeleteBoundaryDescriptor				ntdll		867
imp	'RtlDeleteCriticalSection'				RtlDeleteCriticalSection				ntdll		868	1
imp	'RtlDeleteElementGenericTable'				RtlDeleteElementGenericTable				ntdll		869
imp	'RtlDeleteElementGenericTableAvl'			RtlDeleteElementGenericTableAvl				ntdll		870
imp	'RtlDeleteElementGenericTableAvlEx'			RtlDeleteElementGenericTableAvlEx			ntdll		871
imp	'RtlDeleteFunctionTable'				RtlDeleteFunctionTable					ntdll		872
imp	'RtlDeleteGrowableFunctionTable'			RtlDeleteGrowableFunctionTable				ntdll		873
imp	'RtlDeleteHashTable'					RtlDeleteHashTable					ntdll		874
imp	'RtlDeleteNoSplay'					RtlDeleteNoSplay					ntdll		875
imp	'RtlDeleteRegistryValue'				RtlDeleteRegistryValue					ntdll		876
imp	'RtlDeleteResource'					RtlDeleteResource					ntdll		877
imp	'RtlDeleteSecurityObject'				RtlDeleteSecurityObject					ntdll		878
imp	'RtlDeleteTimer'					RtlDeleteTimer						ntdll		879
imp	'RtlDeleteTimerQueue'					RtlDeleteTimerQueue					ntdll		880
imp	'RtlDeleteTimerQueueEx'					RtlDeleteTimerQueueEx					ntdll		881
imp	'RtlDeleteUmsCompletionList'				RtlDeleteUmsCompletionList				ntdll		882
imp	'RtlDeleteUmsThreadContext'				RtlDeleteUmsThreadContext				ntdll		883
imp	'RtlDequeueUmsCompletionListItems'			RtlDequeueUmsCompletionListItems			ntdll		884
imp	'RtlDeregisterSecureMemoryCacheCallback'		RtlDeregisterSecureMemoryCacheCallback			ntdll		885
imp	'RtlDeregisterWait'					RtlDeregisterWait					ntdll		886
imp	'RtlDeregisterWaitEx'					RtlDeregisterWaitEx					ntdll		887
imp	'RtlDeriveCapabilitySidsFromName'			RtlDeriveCapabilitySidsFromName				ntdll		888
imp	'RtlDestroyAtomTable'					RtlDestroyAtomTable					ntdll		889
imp	'RtlDestroyEnvironment'					RtlDestroyEnvironment					ntdll		890
imp	'RtlDestroyHandleTable'					RtlDestroyHandleTable					ntdll		891
imp	'RtlDestroyHeap'					RtlDestroyHeap						ntdll		892	1
imp	'RtlDestroyMemoryBlockLookaside'			RtlDestroyMemoryBlockLookaside				ntdll		893
imp	'RtlDestroyMemoryZone'					RtlDestroyMemoryZone					ntdll		894
imp	'RtlDestroyProcessParameters'				RtlDestroyProcessParameters				ntdll		895	1
imp	'RtlDestroyQueryDebugBuffer'				RtlDestroyQueryDebugBuffer				ntdll		896
imp	'RtlDetectHeapLeaks'					RtlDetectHeapLeaks					ntdll		897
imp	'RtlDetermineDosPathNameType_U'				RtlDetermineDosPathNameType_U				ntdll		898
imp	'RtlDisableThreadProfiling'				RtlDisableThreadProfiling				ntdll		899
imp	'RtlDllShutdownInProgress'				RtlDllShutdownInProgress				ntdll		900
imp	'RtlDnsHostNameToComputerName'				RtlDnsHostNameToComputerName				ntdll		901
imp	'RtlDoesFileExists_U'					RtlDoesFileExists_U					ntdll		902
imp	'RtlDosApplyFileIsolationRedirection_Ustr'		RtlDosApplyFileIsolationRedirection_Ustr		ntdll		903
imp	'RtlDosLongPathNameToNtPathName_U_WithStatus'		RtlDosLongPathNameToNtPathName_U_WithStatus		ntdll		904
imp	'RtlDosLongPathNameToRelativeNtPathName_U_WithStatus'	RtlDosLongPathNameToRelativeNtPathName_U_WithStatus	ntdll		905
imp	'RtlDosPathNameToNtPathName_U'				RtlDosPathNameToNtPathName_U				ntdll		906
imp	'RtlDosPathNameToNtPathName_U_WithStatus'		RtlDosPathNameToNtPathName_U_WithStatus			ntdll		907
imp	'RtlDosPathNameToRelativeNtPathName_U'			RtlDosPathNameToRelativeNtPathName_U			ntdll		908
imp	'RtlDosPathNameToRelativeNtPathName_U_WithStatus'	RtlDosPathNameToRelativeNtPathName_U_WithStatus		ntdll		909
imp	'RtlDosSearchPath_U'					RtlDosSearchPath_U					ntdll		910
imp	'RtlDosSearchPath_Ustr'					RtlDosSearchPath_Ustr					ntdll		911
imp	'RtlDowncaseUnicodeChar'				RtlDowncaseUnicodeChar					ntdll		912
imp	'RtlDowncaseUnicodeString'				RtlDowncaseUnicodeString				ntdll		913
imp	'RtlDrainNonVolatileFlush'				RtlDrainNonVolatileFlush				ntdll		914
imp	'RtlDumpResource'					RtlDumpResource						ntdll		915
imp	'RtlDuplicateUnicodeString'				RtlDuplicateUnicodeString				ntdll		916
imp	'RtlEmptyAtomTable'					RtlEmptyAtomTable					ntdll		917
imp	'RtlEnableEarlyCriticalSectionEventCreation'		RtlEnableEarlyCriticalSectionEventCreation		ntdll		918
imp	'RtlEnableThreadProfiling'				RtlEnableThreadProfiling				ntdll		919
imp	'RtlEnclaveCallDispatch'				RtlEnclaveCallDispatch					ntdll		920
imp	'RtlEnclaveCallDispatchReturn'				RtlEnclaveCallDispatchReturn				ntdll		921
imp	'RtlEncodePointer'					RtlEncodePointer					ntdll		922
imp	'RtlEncodeRemotePointer'				RtlEncodeRemotePointer					ntdll		923
imp	'RtlEncodeSystemPointer'				RtlEncodeSystemPointer					ntdll		924
imp	'RtlEndEnumerationHashTable'				RtlEndEnumerationHashTable				ntdll		925
imp	'RtlEndStrongEnumerationHashTable'			RtlEndStrongEnumerationHashTable			ntdll		926
imp	'RtlEndWeakEnumerationHashTable'			RtlEndWeakEnumerationHashTable				ntdll		927
imp	'RtlEnterCriticalSection'				RtlEnterCriticalSection					ntdll		928	1
imp	'RtlEnterUmsSchedulingMode'				RtlEnterUmsSchedulingMode				ntdll		929
imp	'RtlEnumProcessHeaps'					RtlEnumProcessHeaps					ntdll		930
imp	'RtlEnumerateEntryHashTable'				RtlEnumerateEntryHashTable				ntdll		931
imp	'RtlEnumerateGenericTable'				RtlEnumerateGenericTable				ntdll		932
imp	'RtlEnumerateGenericTableAvl'				RtlEnumerateGenericTableAvl				ntdll		933
imp	'RtlEnumerateGenericTableLikeADirectory'		RtlEnumerateGenericTableLikeADirectory			ntdll		934
imp	'RtlEnumerateGenericTableWithoutSplaying'		RtlEnumerateGenericTableWithoutSplaying			ntdll		935
imp	'RtlEnumerateGenericTableWithoutSplayingAvl'		RtlEnumerateGenericTableWithoutSplayingAvl		ntdll		936
imp	'RtlEqualComputerName'					RtlEqualComputerName					ntdll		937
imp	'RtlEqualDomainName'					RtlEqualDomainName					ntdll		938
imp	'RtlEqualLuid'						RtlEqualLuid						ntdll		939
imp	'RtlEqualPrefixSid'					RtlEqualPrefixSid					ntdll		940
imp	'RtlEqualSid'						RtlEqualSid						ntdll		941
imp	'RtlEqualString'					RtlEqualString						ntdll		942
imp	'RtlEqualUnicodeString'					RtlEqualUnicodeString					ntdll		943
imp	'RtlEqualWnfChangeStamps'				RtlEqualWnfChangeStamps					ntdll		944
imp	'RtlEraseUnicodeString'					RtlEraseUnicodeString					ntdll		945
imp	'RtlEthernetAddressToStringA'				RtlEthernetAddressToStringA				ntdll		946
imp	'RtlEthernetAddressToString'				RtlEthernetAddressToStringW				ntdll		947
imp	'RtlEthernetStringToAddressA'				RtlEthernetStringToAddressA				ntdll		948
imp	'RtlEthernetStringToAddress'				RtlEthernetStringToAddressW				ntdll		949
imp	'RtlExecuteUmsThread'					RtlExecuteUmsThread					ntdll		950
imp	'RtlExitUserProcess'					RtlExitUserProcess					ntdll		951
imp	'RtlExitUserThread'					RtlExitUserThread					ntdll		952
imp	'RtlExpandEnvironmentStrings'				RtlExpandEnvironmentStrings				ntdll		953
imp	'RtlExpandEnvironmentStrings_U'				RtlExpandEnvironmentStrings_U				ntdll		954
imp	'RtlExpandHashTable'					RtlExpandHashTable					ntdll		955
imp	'RtlExtendCorrelationVector'				RtlExtendCorrelationVector				ntdll		956
imp	'RtlExtendMemoryBlockLookaside'				RtlExtendMemoryBlockLookaside				ntdll		957
imp	'RtlExtendMemoryZone'					RtlExtendMemoryZone					ntdll		958
imp	'RtlExtractBitMap'					RtlExtractBitMap					ntdll		959
imp	'RtlFillMemory'						RtlFillMemory						ntdll		960
imp	'RtlFinalReleaseOutOfProcessMemoryStream'		RtlFinalReleaseOutOfProcessMemoryStream			ntdll		961
imp	'RtlFindAceByType'					RtlFindAceByType					ntdll		962
imp	'RtlFindActivationContextSectionGuid'			RtlFindActivationContextSectionGuid			ntdll		963
imp	'RtlFindActivationContextSectionString'			RtlFindActivationContextSectionString			ntdll		964
imp	'RtlFindCharInUnicodeString'				RtlFindCharInUnicodeString				ntdll		965
imp	'RtlFindClearBits'					RtlFindClearBits					ntdll		966
imp	'RtlFindClearBitsAndSet'				RtlFindClearBitsAndSet					ntdll		967
imp	'RtlFindClearRuns'					RtlFindClearRuns					ntdll		968
imp	'RtlFindClosestEncodableLength'				RtlFindClosestEncodableLength				ntdll		969
imp	'RtlFindExportedRoutineByName'				RtlFindExportedRoutineByName				ntdll		970
imp	'RtlFindLastBackwardRunClear'				RtlFindLastBackwardRunClear				ntdll		971
imp	'RtlFindLeastSignificantBit'				RtlFindLeastSignificantBit				ntdll		972
imp	'RtlFindLongestRunClear'				RtlFindLongestRunClear					ntdll		973
imp	'RtlFindMessage'					RtlFindMessage						ntdll		974
imp	'RtlFindMostSignificantBit'				RtlFindMostSignificantBit				ntdll		975
imp	'RtlFindNextForwardRunClear'				RtlFindNextForwardRunClear				ntdll		976
imp	'RtlFindSetBits'					RtlFindSetBits						ntdll		977
imp	'RtlFindSetBitsAndClear'				RtlFindSetBitsAndClear					ntdll		978
imp	'RtlFindUnicodeSubstring'				RtlFindUnicodeSubstring					ntdll		979
imp	'RtlFirstEntrySList'					RtlFirstEntrySList					ntdll		980
imp	'RtlFirstFreeAce'					RtlFirstFreeAce						ntdll		981
imp	'RtlFlsAlloc'						RtlFlsAlloc						ntdll		982
imp	'RtlFlsFree'						RtlFlsFree						ntdll		983
imp	'RtlFlushHeaps'						RtlFlushHeaps						ntdll		984
imp	'RtlFlushNonVolatileMemory'				RtlFlushNonVolatileMemory				ntdll		985
imp	'RtlFlushNonVolatileMemoryRanges'			RtlFlushNonVolatileMemoryRanges				ntdll		986
imp	'RtlFlushSecureMemoryCache'				RtlFlushSecureMemoryCache				ntdll		987
imp	'RtlFormatCurrentUserKeyPath'				RtlFormatCurrentUserKeyPath				ntdll		988
imp	'RtlFormatMessage'					RtlFormatMessage					ntdll		989
imp	'RtlFormatMessageEx'					RtlFormatMessageEx					ntdll		990
imp	'RtlFreeActivationContextStack'				RtlFreeActivationContextStack				ntdll		991
imp	'RtlFreeAnsiString'					RtlFreeAnsiString					ntdll		992
imp	'RtlFreeHandle'						RtlFreeHandle						ntdll		993
imp	'RtlFreeHeap'						RtlFreeHeap						ntdll		994	3
imp	'RtlFreeMemoryBlockLookaside'				RtlFreeMemoryBlockLookaside				ntdll		995
imp	'RtlFreeNonVolatileToken'				RtlFreeNonVolatileToken					ntdll		996
imp	'RtlFreeOemString'					RtlFreeOemString					ntdll		997
imp	'RtlFreeSid'						RtlFreeSid						ntdll		998
imp	'RtlFreeThreadActivationContextStack'			RtlFreeThreadActivationContextStack			ntdll		999
imp	'RtlFreeUnicodeString'					RtlFreeUnicodeString					ntdll		1000	1
imp	'RtlFreeUserStack'					RtlFreeUserStack					ntdll		1001
imp	'RtlGUIDFromString'					RtlGUIDFromString					ntdll		1002
imp	'RtlGenerate8dot3Name'					RtlGenerate8dot3Name					ntdll		1003
imp	'RtlGetAce'						RtlGetAce						ntdll		1004
imp	'RtlGetActiveActivationContext'				RtlGetActiveActivationContext				ntdll		1005
imp	'RtlGetActiveConsoleId'					RtlGetActiveConsoleId					ntdll		1006
imp	'RtlGetAppContainerNamedObjectPath'			RtlGetAppContainerNamedObjectPath			ntdll		1007
imp	'RtlGetAppContainerParent'				RtlGetAppContainerParent				ntdll		1008
imp	'RtlGetAppContainerSidType'				RtlGetAppContainerSidType				ntdll		1009
imp	'RtlGetCallersAddress'					RtlGetCallersAddress					ntdll		1010
imp	'RtlGetCompressionWorkSpaceSize'			RtlGetCompressionWorkSpaceSize				ntdll		1011
imp	'RtlGetConsoleSessionForegroundProcessId'		RtlGetConsoleSessionForegroundProcessId			ntdll		1012
imp	'RtlGetControlSecurityDescriptor'			RtlGetControlSecurityDescriptor				ntdll		1013
imp	'RtlGetCriticalSectionRecursionCount'			RtlGetCriticalSectionRecursionCount			ntdll		1014
imp	'RtlGetCurrentDirectory_U'				RtlGetCurrentDirectory_U				ntdll		1015
imp	'RtlGetCurrentPeb'					RtlGetCurrentPeb					ntdll		1016
imp	'RtlGetCurrentProcessorNumber'				RtlGetCurrentProcessorNumber				ntdll		1017
imp	'RtlGetCurrentProcessorNumberEx'			RtlGetCurrentProcessorNumberEx				ntdll		1018
imp	'RtlGetCurrentServiceSessionId'				RtlGetCurrentServiceSessionId				ntdll		1019
imp	'RtlGetCurrentTransaction'				RtlGetCurrentTransaction				ntdll		1020
imp	'RtlGetCurrentUmsThread'				RtlGetCurrentUmsThread					ntdll		1021
imp	'RtlGetDaclSecurityDescriptor'				RtlGetDaclSecurityDescriptor				ntdll		1022
imp	'RtlGetDeviceFamilyInfoEnum'				RtlGetDeviceFamilyInfoEnum				ntdll		1023
imp	'RtlGetElementGenericTable'				RtlGetElementGenericTable				ntdll		1024
imp	'RtlGetElementGenericTableAvl'				RtlGetElementGenericTableAvl				ntdll		1025
imp	'RtlGetEnabledExtendedFeatures'				RtlGetEnabledExtendedFeatures				ntdll		1026
imp	'RtlGetExePath'						RtlGetExePath						ntdll		1027
imp	'RtlGetExtendedContextLength'				RtlGetExtendedContextLength				ntdll		1028
imp	'RtlGetExtendedFeaturesMask'				RtlGetExtendedFeaturesMask				ntdll		1029
imp	'RtlGetFileMUIPath'					RtlGetFileMUIPath					ntdll		1030
imp	'RtlGetFrame'						RtlGetFrame						ntdll		1031
imp	'RtlGetFullPathName_U'					RtlGetFullPathName_U					ntdll		1032
imp	'RtlGetFullPathName_UEx'				RtlGetFullPathName_UEx					ntdll		1033
imp	'RtlGetFullPathName_UstrEx'				RtlGetFullPathName_UstrEx				ntdll		1034
imp	'RtlGetFunctionTableListHead'				RtlGetFunctionTableListHead				ntdll		1035
imp	'RtlGetGroupSecurityDescriptor'				RtlGetGroupSecurityDescriptor				ntdll		1036
imp	'RtlGetIntegerAtom'					RtlGetIntegerAtom					ntdll		1037
imp	'RtlGetInterruptTimePrecise'				RtlGetInterruptTimePrecise				ntdll		1038
imp	'RtlGetLastNtStatus'					RtlGetLastNtStatus					ntdll		1039
imp	'RtlGetLastWin32Error'					RtlGetLastWin32Error					ntdll		1040
imp	'RtlGetLengthWithoutLastFullDosOrNtPathElement'		RtlGetLengthWithoutLastFullDosOrNtPathElement		ntdll		1041
imp	'RtlGetLengthWithoutTrailingPathSeperators'		RtlGetLengthWithoutTrailingPathSeperators		ntdll		1042
imp	'RtlGetLocaleFileMappingAddress'			RtlGetLocaleFileMappingAddress				ntdll		1043
imp	'RtlGetLongestNtPathLength'				RtlGetLongestNtPathLength				ntdll		1044
imp	'RtlGetNativeSystemInformation'				RtlGetNativeSystemInformation				ntdll		1045
imp	'RtlGetNextEntryHashTable'				RtlGetNextEntryHashTable				ntdll		1046
imp	'RtlGetNextUmsListItem'					RtlGetNextUmsListItem					ntdll		1047
imp	'RtlGetNonVolatileToken'				RtlGetNonVolatileToken					ntdll		1048
imp	'RtlGetNtGlobalFlags'					RtlGetNtGlobalFlags					ntdll		1049
imp	'RtlGetNtProductType'					RtlGetNtProductType					ntdll		1050
imp	'RtlGetNtSystemRoot'					RtlGetNtSystemRoot					ntdll		1051
imp	'RtlGetNtVersionNumbers'				RtlGetNtVersionNumbers					ntdll		1052
imp	'RtlGetOwnerSecurityDescriptor'				RtlGetOwnerSecurityDescriptor				ntdll		1053
imp	'RtlGetParentLocaleName'				RtlGetParentLocaleName					ntdll		1054
imp	'RtlGetPersistedStateLocation'				RtlGetPersistedStateLocation				ntdll		1055
imp	'RtlGetProcessHeaps'					RtlGetProcessHeaps					ntdll		1056	2
imp	'RtlGetProcessPreferredUILanguages'			RtlGetProcessPreferredUILanguages			ntdll		1057
imp	'RtlGetProductInfo'					RtlGetProductInfo					ntdll		1058
imp	'RtlGetSaclSecurityDescriptor'				RtlGetSaclSecurityDescriptor				ntdll		1059
imp	'RtlGetSearchPath'					RtlGetSearchPath					ntdll		1060
imp	'RtlGetSecurityDescriptorRMControl'			RtlGetSecurityDescriptorRMControl			ntdll		1061
imp	'RtlGetSessionProperties'				RtlGetSessionProperties					ntdll		1062
imp	'RtlGetSetBootStatusData'				RtlGetSetBootStatusData					ntdll		1063
imp	'RtlGetSuiteMask'					RtlGetSuiteMask						ntdll		1064
imp	'RtlGetSystemBootStatus'				RtlGetSystemBootStatus					ntdll		1065
imp	'RtlGetSystemBootStatusEx'				RtlGetSystemBootStatusEx				ntdll		1066
imp	'RtlGetSystemPreferredUILanguages'			RtlGetSystemPreferredUILanguages			ntdll		1067
imp	'RtlGetSystemTimePrecise'				RtlGetSystemTimePrecise					ntdll		1068
imp	'RtlGetThreadErrorMode'					RtlGetThreadErrorMode					ntdll		1069
imp	'RtlGetThreadLangIdByIndex'				RtlGetThreadLangIdByIndex				ntdll		1070
imp	'RtlGetThreadPreferredUILanguages'			RtlGetThreadPreferredUILanguages			ntdll		1071
imp	'RtlGetThreadWorkOnBehalfTicket'			RtlGetThreadWorkOnBehalfTicket				ntdll		1072
imp	'RtlGetTokenNamedObjectPath'				RtlGetTokenNamedObjectPath				ntdll		1073
imp	'RtlGetUILanguageInfo'					RtlGetUILanguageInfo					ntdll		1074
imp	'RtlGetUmsCompletionListEvent'				RtlGetUmsCompletionListEvent				ntdll		1075
imp	'RtlGetUnloadEventTrace'				RtlGetUnloadEventTrace					ntdll		1076
imp	'RtlGetUnloadEventTraceEx'				RtlGetUnloadEventTraceEx				ntdll		1077
imp	'RtlGetUserInfoHeap'					RtlGetUserInfoHeap					ntdll		1078
imp	'RtlGetUserPreferredUILanguages'			RtlGetUserPreferredUILanguages				ntdll		1079
imp	'RtlGetVersion'						RtlGetVersion						ntdll		1080
imp	'RtlGrowFunctionTable'					RtlGrowFunctionTable					ntdll		1081
imp	'RtlGuardCheckLongJumpTarget'				RtlGuardCheckLongJumpTarget				ntdll		1082
imp	'RtlHashUnicodeString'					RtlHashUnicodeString					ntdll		1083
imp	'RtlHeapTrkInitialize'					RtlHeapTrkInitialize					ntdll		1084
imp	'RtlIdentifierAuthoritySid'				RtlIdentifierAuthoritySid				ntdll		1085
imp	'RtlIdnToAscii'						RtlIdnToAscii						ntdll		1086
imp	'RtlIdnToNameprepUnicode'				RtlIdnToNameprepUnicode					ntdll		1087
imp	'RtlIdnToUnicode'					RtlIdnToUnicode						ntdll		1088
imp	'RtlImageDirectoryEntryToData'				RtlImageDirectoryEntryToData				ntdll		1089
imp	'RtlImageNtHeader'					RtlImageNtHeader					ntdll		1090
imp	'RtlImageNtHeaderEx'					RtlImageNtHeaderEx					ntdll		1091
imp	'RtlImageRvaToSection'					RtlImageRvaToSection					ntdll		1092
imp	'RtlImageRvaToVa'					RtlImageRvaToVa						ntdll		1093
imp	'RtlImpersonateSelf'					RtlImpersonateSelf					ntdll		1094
imp	'RtlImpersonateSelfEx'					RtlImpersonateSelfEx					ntdll		1095
imp	'RtlIncrementCorrelationVector'				RtlIncrementCorrelationVector				ntdll		1096
imp	'RtlInitAnsiString'					RtlInitAnsiString					ntdll		1097
imp	'RtlInitAnsiStringEx'					RtlInitAnsiStringEx					ntdll		1098
imp	'RtlInitBarrier'					RtlInitBarrier						ntdll		1099
imp	'RtlInitCodePageTable'					RtlInitCodePageTable					ntdll		1100
imp	'RtlInitEnumerationHashTable'				RtlInitEnumerationHashTable				ntdll		1101
imp	'RtlInitMemoryStream'					RtlInitMemoryStream					ntdll		1102
imp	'RtlInitNlsTables'					RtlInitNlsTables					ntdll		1103
imp	'RtlInitOutOfProcessMemoryStream'			RtlInitOutOfProcessMemoryStream				ntdll		1104
imp	'RtlInitString'						RtlInitString						ntdll		1105
imp	'RtlInitStringEx'					RtlInitStringEx						ntdll		1106
imp	'RtlInitStrongEnumerationHashTable'			RtlInitStrongEnumerationHashTable			ntdll		1107
imp	'RtlInitUnicodeString'					RtlInitUnicodeString					ntdll		1108	2
imp	'RtlInitUnicodeStringEx'				RtlInitUnicodeStringEx					ntdll		1109
imp	'RtlInitWeakEnumerationHashTable'			RtlInitWeakEnumerationHashTable				ntdll		1110
imp	'RtlInitializeAtomPackage'				RtlInitializeAtomPackage				ntdll		1111
imp	'RtlInitializeBitMap'					RtlInitializeBitMap					ntdll		1112
imp	'RtlInitializeBitMapEx'					RtlInitializeBitMapEx					ntdll		1113
imp	'RtlInitializeConditionVariable'			RtlInitializeConditionVariable				ntdll		1114
imp	'RtlInitializeContext'					RtlInitializeContext					ntdll		1115
imp	'RtlInitializeCorrelationVector'			RtlInitializeCorrelationVector				ntdll		1116
imp	'RtlInitializeCriticalSection'				RtlInitializeCriticalSection				ntdll		1117	1
imp	'RtlInitializeCriticalSectionAndSpinCount'		RtlInitializeCriticalSectionAndSpinCount		ntdll		1118
imp	'RtlInitializeCriticalSectionEx'			RtlInitializeCriticalSectionEx				ntdll		1119
imp	'RtlInitializeExtendedContext'				RtlInitializeExtendedContext				ntdll		1120
imp	'RtlInitializeGenericTable'				RtlInitializeGenericTable				ntdll		1121
imp	'RtlInitializeGenericTableAvl'				RtlInitializeGenericTableAvl				ntdll		1122
imp	'RtlInitializeHandleTable'				RtlInitializeHandleTable				ntdll		1123
imp	'RtlInitializeNtUserPfn'				RtlInitializeNtUserPfn					ntdll		1124
imp	'RtlInitializeRXact'					RtlInitializeRXact					ntdll		1125
imp	'RtlInitializeResource'					RtlInitializeResource					ntdll		1126
imp	'RtlInitializeSListHead'				RtlInitializeSListHead					ntdll		1127
imp	'RtlInitializeSRWLock'					RtlInitializeSRWLock					ntdll		1128
imp	'RtlInitializeSid'					RtlInitializeSid					ntdll		1129
imp	'RtlInitializeSidEx'					RtlInitializeSidEx					ntdll		1130
imp	'RtlInsertElementGenericTable'				RtlInsertElementGenericTable				ntdll		1131
imp	'RtlInsertElementGenericTableAvl'			RtlInsertElementGenericTableAvl				ntdll		1132
imp	'RtlInsertElementGenericTableFull'			RtlInsertElementGenericTableFull			ntdll		1133
imp	'RtlInsertElementGenericTableFullAvl'			RtlInsertElementGenericTableFullAvl			ntdll		1134
imp	'RtlInsertEntryHashTable'				RtlInsertEntryHashTable					ntdll		1135
imp	'RtlInstallFunctionTableCallback'			RtlInstallFunctionTableCallback				ntdll		1136
imp	'RtlInt64ToUnicodeString'				RtlInt64ToUnicodeString					ntdll		1137
imp	'RtlIntegerToChar'					RtlIntegerToChar					ntdll		1138
imp	'RtlIntegerToUnicodeString'				RtlIntegerToUnicodeString				ntdll		1139
imp	'RtlInterlockedClearBitRun'				RtlInterlockedClearBitRun				ntdll		1140
imp	'RtlInterlockedFlushSList'				RtlInterlockedFlushSList				ntdll		1141
imp	'RtlInterlockedPopEntrySList'				RtlInterlockedPopEntrySList				ntdll		1142
imp	'RtlInterlockedPushEntrySList'				RtlInterlockedPushEntrySList				ntdll		1143
imp	'RtlInterlockedPushListSList'				RtlInterlockedPushListSList				ntdll		1144
imp	'RtlInterlockedPushListSListEx'				RtlInterlockedPushListSListEx				ntdll		1145
imp	'RtlInterlockedSetBitRun'				RtlInterlockedSetBitRun					ntdll		1146
imp	'RtlIoDecodeMemIoResource'				RtlIoDecodeMemIoResource				ntdll		1147
imp	'RtlIoEncodeMemIoResource'				RtlIoEncodeMemIoResource				ntdll		1148
imp	'RtlIpv4AddressToStringA'				RtlIpv4AddressToStringA					ntdll		1149
imp	'RtlIpv4AddressToStringExA'				RtlIpv4AddressToStringExA				ntdll		1150
imp	'RtlIpv4AddressToStringEx'				RtlIpv4AddressToStringExW				ntdll		1151
imp	'RtlIpv4AddressToString'				RtlIpv4AddressToStringW					ntdll		1152
imp	'RtlIpv4StringToAddressA'				RtlIpv4StringToAddressA					ntdll		1153
imp	'RtlIpv4StringToAddressExA'				RtlIpv4StringToAddressExA				ntdll		1154
imp	'RtlIpv4StringToAddressEx'				RtlIpv4StringToAddressExW				ntdll		1155
imp	'RtlIpv4StringToAddress'				RtlIpv4StringToAddressW					ntdll		1156
imp	'RtlIpv6AddressToStringA'				RtlIpv6AddressToStringA					ntdll		1157
imp	'RtlIpv6AddressToStringExA'				RtlIpv6AddressToStringExA				ntdll		1158
imp	'RtlIpv6AddressToStringEx'				RtlIpv6AddressToStringExW				ntdll		1159
imp	'RtlIpv6AddressToString'				RtlIpv6AddressToStringW					ntdll		1160
imp	'RtlIpv6StringToAddressA'				RtlIpv6StringToAddressA					ntdll		1161
imp	'RtlIpv6StringToAddressExA'				RtlIpv6StringToAddressExA				ntdll		1162
imp	'RtlIpv6StringToAddressEx'				RtlIpv6StringToAddressExW				ntdll		1163
imp	'RtlIpv6StringToAddress'				RtlIpv6StringToAddressW					ntdll		1164
imp	'RtlIsActivationContextActive'				RtlIsActivationContextActive				ntdll		1165
imp	'RtlIsCapabilitySid'					RtlIsCapabilitySid					ntdll		1166
imp	'RtlIsCloudFilesPlaceholder'				RtlIsCloudFilesPlaceholder				ntdll		1167
imp	'RtlIsCriticalSectionLocked'				RtlIsCriticalSectionLocked				ntdll		1168
imp	'RtlIsCriticalSectionLockedByThread'			RtlIsCriticalSectionLockedByThread			ntdll		1169
imp	'RtlIsCurrentProcess'					RtlIsCurrentProcess					ntdll		1170
imp	'RtlIsCurrentThread'					RtlIsCurrentThread					ntdll		1171
imp	'RtlIsCurrentThreadAttachExempt'			RtlIsCurrentThreadAttachExempt				ntdll		1172
imp	'RtlIsDosDeviceName_U'					RtlIsDosDeviceName_U					ntdll		1173
imp	'RtlIsElevatedRid'					RtlIsElevatedRid					ntdll		1174
imp	'RtlIsGenericTableEmpty'				RtlIsGenericTableEmpty					ntdll		1175
imp	'RtlIsGenericTableEmptyAvl'				RtlIsGenericTableEmptyAvl				ntdll		1176
imp	'RtlIsMultiSessionSku'					RtlIsMultiSessionSku					ntdll		1177
imp	'RtlIsMultiUsersInSessionSku'				RtlIsMultiUsersInSessionSku				ntdll		1178
imp	'RtlIsNameInExpression'					RtlIsNameInExpression					ntdll		1179
imp	'RtlIsNameInUnUpcasedExpression'			RtlIsNameInUnUpcasedExpression				ntdll		1180
imp	'RtlIsNameLegalDOS8Dot3'				RtlIsNameLegalDOS8Dot3					ntdll		1181
imp	'RtlIsNonEmptyDirectoryReparsePointAllowed'		RtlIsNonEmptyDirectoryReparsePointAllowed		ntdll		1182
imp	'RtlIsNormalizedString'					RtlIsNormalizedString					ntdll		1183
imp	'RtlIsPackageSid'					RtlIsPackageSid						ntdll		1184
imp	'RtlIsParentOfChildAppContainer'			RtlIsParentOfChildAppContainer				ntdll		1185
imp	'RtlIsPartialPlaceholder'				RtlIsPartialPlaceholder					ntdll		1186
imp	'RtlIsPartialPlaceholderFileHandle'			RtlIsPartialPlaceholderFileHandle			ntdll		1187
imp	'RtlIsPartialPlaceholderFileInfo'			RtlIsPartialPlaceholderFileInfo				ntdll		1188
imp	'RtlIsProcessorFeaturePresent'				RtlIsProcessorFeaturePresent				ntdll		1189
imp	'RtlIsStateSeparationEnabled'				RtlIsStateSeparationEnabled				ntdll		1190
imp	'RtlIsTextUnicode'					RtlIsTextUnicode					ntdll		1191
imp	'RtlIsThreadWithinLoaderCallout'			RtlIsThreadWithinLoaderCallout				ntdll		1192
imp	'RtlIsUntrustedObject'					RtlIsUntrustedObject					ntdll		1193
imp	'RtlIsValidHandle'					RtlIsValidHandle					ntdll		1194
imp	'RtlIsValidIndexHandle'					RtlIsValidIndexHandle					ntdll		1195
imp	'RtlIsValidLocaleName'					RtlIsValidLocaleName					ntdll		1196
imp	'RtlIsValidProcessTrustLabelSid'			RtlIsValidProcessTrustLabelSid				ntdll		1197
imp	'RtlKnownExceptionFilter'				RtlKnownExceptionFilter					ntdll		1198
imp	'RtlLCIDToCultureName'					RtlLCIDToCultureName					ntdll		1199
imp	'RtlLargeIntegerToChar'					RtlLargeIntegerToChar					ntdll		1200
imp	'RtlLcidToLocaleName'					RtlLcidToLocaleName					ntdll		1201
imp	'RtlLeaveCriticalSection'				RtlLeaveCriticalSection					ntdll		1202	1
imp	'RtlLengthRequiredSid'					RtlLengthRequiredSid					ntdll		1203
imp	'RtlLengthSecurityDescriptor'				RtlLengthSecurityDescriptor				ntdll		1204
imp	'RtlLengthSid'						RtlLengthSid						ntdll		1205
imp	'RtlLengthSidAsUnicodeString'				RtlLengthSidAsUnicodeString				ntdll		1206
imp	'RtlLoadString'						RtlLoadString						ntdll		1207
imp	'RtlLocalTimeToSystemTime'				RtlLocalTimeToSystemTime				ntdll		1208
imp	'RtlLocaleNameToLcid'					RtlLocaleNameToLcid					ntdll		1209
imp	'RtlLocateExtendedFeature'				RtlLocateExtendedFeature				ntdll		1210
imp	'RtlLocateExtendedFeature2'				RtlLocateExtendedFeature2				ntdll		1211
imp	'RtlLocateLegacyContext'				RtlLocateLegacyContext					ntdll		1212
imp	'RtlLockBootStatusData'					RtlLockBootStatusData					ntdll		1213
imp	'RtlLockCurrentThread'					RtlLockCurrentThread					ntdll		1214
imp	'RtlLockHeap'						RtlLockHeap						ntdll		1215	1
imp	'RtlLockMemoryBlockLookaside'				RtlLockMemoryBlockLookaside				ntdll		1216
imp	'RtlLockMemoryStreamRegion'				RtlLockMemoryStreamRegion				ntdll		1217
imp	'RtlLockMemoryZone'					RtlLockMemoryZone					ntdll		1218
imp	'RtlLockModuleSection'					RtlLockModuleSection					ntdll		1219
imp	'RtlLogStackBackTrace'					RtlLogStackBackTrace					ntdll		1220
imp	'RtlLookupAtomInAtomTable'				RtlLookupAtomInAtomTable				ntdll		1221
imp	'RtlLookupElementGenericTable'				RtlLookupElementGenericTable				ntdll		1222
imp	'RtlLookupElementGenericTableAvl'			RtlLookupElementGenericTableAvl				ntdll		1223
imp	'RtlLookupElementGenericTableFull'			RtlLookupElementGenericTableFull			ntdll		1224
imp	'RtlLookupElementGenericTableFullAvl'			RtlLookupElementGenericTableFullAvl			ntdll		1225
imp	'RtlLookupEntryHashTable'				RtlLookupEntryHashTable					ntdll		1226
imp	'RtlLookupFirstMatchingElementGenericTableAvl'		RtlLookupFirstMatchingElementGenericTableAvl		ntdll		1227
imp	'RtlLookupFunctionEntry'				RtlLookupFunctionEntry					ntdll		1228
imp	'RtlLookupFunctionTable'				RtlLookupFunctionTable					ntdll		1229
imp	'RtlMakeSelfRelativeSD'					RtlMakeSelfRelativeSD					ntdll		1230
imp	'RtlMapGenericMask'					RtlMapGenericMask					ntdll		1231
imp	'RtlMapSecurityErrorToNtStatus'				RtlMapSecurityErrorToNtStatus				ntdll		1232
imp	'RtlMoveMemory'						RtlMoveMemory						ntdll		1233
imp	'RtlMultiAppendUnicodeStringBuffer'			RtlMultiAppendUnicodeStringBuffer			ntdll		1234
imp	'RtlMultiByteToUnicodeN'				RtlMultiByteToUnicodeN					ntdll		1235
imp	'RtlMultiByteToUnicodeSize'				RtlMultiByteToUnicodeSize				ntdll		1236
imp	'RtlMultipleAllocateHeap'				RtlMultipleAllocateHeap					ntdll		1237
imp	'RtlMultipleFreeHeap'					RtlMultipleFreeHeap					ntdll		1238
imp	'RtlNewInstanceSecurityObject'				RtlNewInstanceSecurityObject				ntdll		1239
imp	'RtlNewSecurityGrantedAccess'				RtlNewSecurityGrantedAccess				ntdll		1240
imp	'RtlNewSecurityObject'					RtlNewSecurityObject					ntdll		1241
imp	'RtlNewSecurityObjectEx'				RtlNewSecurityObjectEx					ntdll		1242
imp	'RtlNewSecurityObjectWithMultipleInheritance'		RtlNewSecurityObjectWithMultipleInheritance		ntdll		1243
imp	'RtlNormalizeProcessParams'				RtlNormalizeProcessParams				ntdll		1244
imp	'RtlNormalizeString'					RtlNormalizeString					ntdll		1245
imp	'RtlNtPathNameToDosPathName'				RtlNtPathNameToDosPathName				ntdll		1246
imp	'RtlNtStatusToDosError'					RtlNtStatusToDosError					ntdll		1247	1
imp	'RtlNtStatusToDosErrorNoTeb'				RtlNtStatusToDosErrorNoTeb				ntdll		1248
imp	'RtlNtdllName'						RtlNtdllName						ntdll		1249
imp	'RtlNumberGenericTableElements'				RtlNumberGenericTableElements				ntdll		1250
imp	'RtlNumberGenericTableElementsAvl'			RtlNumberGenericTableElementsAvl			ntdll		1251
imp	'RtlNumberOfClearBits'					RtlNumberOfClearBits					ntdll		1252
imp	'RtlNumberOfClearBitsInRange'				RtlNumberOfClearBitsInRange				ntdll		1253
imp	'RtlNumberOfSetBits'					RtlNumberOfSetBits					ntdll		1254
imp	'RtlNumberOfSetBitsInRange'				RtlNumberOfSetBitsInRange				ntdll		1255
imp	'RtlNumberOfSetBitsUlongPtr'				RtlNumberOfSetBitsUlongPtr				ntdll		1256
imp	'RtlOemStringToUnicodeSize'				RtlOemStringToUnicodeSize				ntdll		1257
imp	'RtlOemStringToUnicodeString'				RtlOemStringToUnicodeString				ntdll		1258
imp	'RtlOemToUnicodeN'					RtlOemToUnicodeN					ntdll		1259
imp	'RtlOpenCurrentUser'					RtlOpenCurrentUser					ntdll		1260
imp	'RtlOsDeploymentState'					RtlOsDeploymentState					ntdll		1261
imp	'RtlOwnerAcesPresent'					RtlOwnerAcesPresent					ntdll		1262
imp	'RtlPcToFileHeader'					RtlPcToFileHeader					ntdll		1263
imp	'RtlPinAtomInAtomTable'					RtlPinAtomInAtomTable					ntdll		1264
imp	'RtlPopFrame'						RtlPopFrame						ntdll		1265
imp	'RtlPrefixString'					RtlPrefixString						ntdll		1266
imp	'RtlPrefixUnicodeString'				RtlPrefixUnicodeString					ntdll		1267
imp	'RtlPrepareForProcessCloning'				RtlPrepareForProcessCloning				ntdll		1268
imp	'RtlProcessFlsData'					RtlProcessFlsData					ntdll		1269
imp	'RtlProtectHeap'					RtlProtectHeap						ntdll		1270
imp	'RtlPublishWnfStateData'				RtlPublishWnfStateData					ntdll		1271
imp	'RtlPushFrame'						RtlPushFrame						ntdll		1272
imp	'RtlQueryActivationContextApplicationSettings'		RtlQueryActivationContextApplicationSettings		ntdll		1273
imp	'RtlQueryAtomInAtomTable'				RtlQueryAtomInAtomTable					ntdll		1274
imp	'RtlQueryCriticalSectionOwner'				RtlQueryCriticalSectionOwner				ntdll		1275
imp	'RtlQueryDepthSList'					RtlQueryDepthSList					ntdll		1276
imp	'RtlQueryDynamicTimeZoneInformation'			RtlQueryDynamicTimeZoneInformation			ntdll		1277
imp	'RtlQueryElevationFlags'				RtlQueryElevationFlags					ntdll		1278
imp	'RtlQueryEnvironmentVariable'				RtlQueryEnvironmentVariable				ntdll		1279	3
imp	'RtlQueryEnvironmentVariable_U'				RtlQueryEnvironmentVariable_U				ntdll		1280
imp	'RtlQueryHeapInformation'				RtlQueryHeapInformation					ntdll		1281
imp	'RtlQueryImageMitigationPolicy'				RtlQueryImageMitigationPolicy				ntdll		1282
imp	'RtlQueryInformationAcl'				RtlQueryInformationAcl					ntdll		1283
imp	'RtlQueryInformationActivationContext'			RtlQueryInformationActivationContext			ntdll		1284
imp	'RtlQueryInformationActiveActivationContext'		RtlQueryInformationActiveActivationContext		ntdll		1285
imp	'RtlQueryInterfaceMemoryStream'				RtlQueryInterfaceMemoryStream				ntdll		1286
imp	'RtlQueryModuleInformation'				RtlQueryModuleInformation				ntdll		1287
imp	'RtlQueryPackageClaims'					RtlQueryPackageClaims					ntdll		1288
imp	'RtlQueryPackageIdentity'				RtlQueryPackageIdentity					ntdll		1289
imp	'RtlQueryPackageIdentityEx'				RtlQueryPackageIdentityEx				ntdll		1290
imp	'RtlQueryPerformanceCounter'				RtlQueryPerformanceCounter				ntdll		1291
imp	'RtlQueryPerformanceFrequency'				RtlQueryPerformanceFrequency				ntdll		1292
imp	'RtlQueryProcessBackTraceInformation'			RtlQueryProcessBackTraceInformation			ntdll		1293
imp	'RtlQueryProcessDebugInformation'			RtlQueryProcessDebugInformation				ntdll		1294
imp	'RtlQueryProcessHeapInformation'			RtlQueryProcessHeapInformation				ntdll		1295
imp	'RtlQueryProcessLockInformation'			RtlQueryProcessLockInformation				ntdll		1296
imp	'RtlQueryProcessPlaceholderCompatibilityMode'		RtlQueryProcessPlaceholderCompatibilityMode		ntdll		1297
imp	'RtlQueryProtectedPolicy'				RtlQueryProtectedPolicy					ntdll		1298
imp	'RtlQueryRegistryValueWithFallback'			RtlQueryRegistryValueWithFallback			ntdll		1299
imp	'RtlQueryRegistryValues'				RtlQueryRegistryValues					ntdll		1300
imp	'RtlQueryRegistryValuesEx'				RtlQueryRegistryValuesEx				ntdll		1301
imp	'RtlQueryResourcePolicy'				RtlQueryResourcePolicy					ntdll		1302
imp	'RtlQuerySecurityObject'				RtlQuerySecurityObject					ntdll		1303
imp	'RtlQueryTagHeap'					RtlQueryTagHeap						ntdll		1304
imp	'RtlQueryThreadPlaceholderCompatibilityMode'		RtlQueryThreadPlaceholderCompatibilityMode		ntdll		1305
imp	'RtlQueryThreadProfiling'				RtlQueryThreadProfiling					ntdll		1306
imp	'RtlQueryTimeZoneInformation'				RtlQueryTimeZoneInformation				ntdll		1307
imp	'RtlQueryTokenHostIdAsUlong64'				RtlQueryTokenHostIdAsUlong64				ntdll		1308
imp	'RtlQueryUmsThreadInformation'				RtlQueryUmsThreadInformation				ntdll		1309
imp	'RtlQueryUnbiasedInterruptTime'				RtlQueryUnbiasedInterruptTime				ntdll		1310
imp	'RtlQueryValidationRunlevel'				RtlQueryValidationRunlevel				ntdll		1311
imp	'RtlQueryWnfMetaNotification'				RtlQueryWnfMetaNotification				ntdll		1312
imp	'RtlQueryWnfStateData'					RtlQueryWnfStateData					ntdll		1313
imp	'RtlQueryWnfStateDataWithExplicitScope'			RtlQueryWnfStateDataWithExplicitScope			ntdll		1314
imp	'RtlQueueApcWow64Thread'				RtlQueueApcWow64Thread					ntdll		1315
imp	'RtlQueueWorkItem'					RtlQueueWorkItem					ntdll		1316
imp	'RtlRaiseCustomSystemEventTrigger'			RtlRaiseCustomSystemEventTrigger			ntdll		1317
imp	'RtlRaiseException'					RtlRaiseException					ntdll		1318
imp	'RtlRaiseStatus'					RtlRaiseStatus						ntdll		1319
imp	'RtlRandom'						RtlRandom						ntdll		1320
imp	'RtlRandomEx'						RtlRandomEx						ntdll		1321
imp	'RtlRbInsertNodeEx'					RtlRbInsertNodeEx					ntdll		1322
imp	'RtlRbRemoveNode'					RtlRbRemoveNode						ntdll		1323
imp	'RtlReAllocateHeap'					RtlReAllocateHeap					ntdll		1324	4
imp	'RtlReadMemoryStream'					RtlReadMemoryStream					ntdll		1325
imp	'RtlReadOutOfProcessMemoryStream'			RtlReadOutOfProcessMemoryStream				ntdll		1326
imp	'RtlReadThreadProfilingData'				RtlReadThreadProfilingData				ntdll		1327
imp	'RtlRealPredecessor'					RtlRealPredecessor					ntdll		1328
imp	'RtlRealSuccessor'					RtlRealSuccessor					ntdll		1329
imp	'RtlRegisterForWnfMetaNotification'			RtlRegisterForWnfMetaNotification			ntdll		1330
imp	'RtlRegisterSecureMemoryCacheCallback'			RtlRegisterSecureMemoryCacheCallback			ntdll		1331
imp	'RtlRegisterThreadWithCsrss'				RtlRegisterThreadWithCsrss				ntdll		1332
imp	'RtlRegisterWait'					RtlRegisterWait						ntdll		1333
imp	'RtlReleaseActivationContext'				RtlReleaseActivationContext				ntdll		1334
imp	'RtlReleaseMemoryStream'				RtlReleaseMemoryStream					ntdll		1335
imp	'RtlReleasePath'					RtlReleasePath						ntdll		1336
imp	'RtlReleasePebLock'					RtlReleasePebLock					ntdll		1337
imp	'RtlReleasePrivilege'					RtlReleasePrivilege					ntdll		1338
imp	'RtlReleaseRelativeName'				RtlReleaseRelativeName					ntdll		1339
imp	'RtlReleaseResource'					RtlReleaseResource					ntdll		1340
imp	'RtlReleaseSRWLockExclusive'				RtlReleaseSRWLockExclusive				ntdll		1341
imp	'RtlReleaseSRWLockShared'				RtlReleaseSRWLockShared					ntdll		1342
imp	'RtlRemoteCall'						RtlRemoteCall						ntdll		1343
imp	'RtlRemoveEntryHashTable'				RtlRemoveEntryHashTable					ntdll		1344
imp	'RtlRemovePrivileges'					RtlRemovePrivileges					ntdll		1345
imp	'RtlRemoveVectoredContinueHandler'			RtlRemoveVectoredContinueHandler			ntdll		1346
imp	'RtlRemoveVectoredExceptionHandler'			RtlRemoveVectoredExceptionHandler			ntdll		1347
imp	'RtlReplaceSidInSd'					RtlReplaceSidInSd					ntdll		1348
imp	'RtlReplaceSystemDirectoryInPath'			RtlReplaceSystemDirectoryInPath				ntdll		1349
imp	'RtlReportException'					RtlReportException					ntdll		1350
imp	'RtlReportExceptionEx'					RtlReportExceptionEx					ntdll		1351
imp	'RtlReportSilentProcessExit'				RtlReportSilentProcessExit				ntdll		1352
imp	'RtlReportSqmEscalation'				RtlReportSqmEscalation					ntdll		1353
imp	'RtlResetMemoryBlockLookaside'				RtlResetMemoryBlockLookaside				ntdll		1354
imp	'RtlResetMemoryZone'					RtlResetMemoryZone					ntdll		1355
imp	'RtlResetNtUserPfn'					RtlResetNtUserPfn					ntdll		1356
imp	'RtlResetRtlTranslations'				RtlResetRtlTranslations					ntdll		1357
imp	'RtlRestoreBootStatusDefaults'				RtlRestoreBootStatusDefaults				ntdll		1358
imp	'RtlRestoreContext'					RtlRestoreContext					ntdll		1359
imp	'RtlRestoreLastWin32Error'				RtlRestoreLastWin32Error				ntdll		1360
imp	'RtlRestoreSystemBootStatusDefaults'			RtlRestoreSystemBootStatusDefaults			ntdll		1361
imp	'RtlRetrieveNtUserPfn'					RtlRetrieveNtUserPfn					ntdll		1362
imp	'RtlRevertMemoryStream'					RtlRevertMemoryStream					ntdll		1363
imp	'RtlRunDecodeUnicodeString'				RtlRunDecodeUnicodeString				ntdll		1364
imp	'RtlRunEncodeUnicodeString'				RtlRunEncodeUnicodeString				ntdll		1365
imp	'RtlRunOnceBeginInitialize'				RtlRunOnceBeginInitialize				ntdll		1366
imp	'RtlRunOnceComplete'					RtlRunOnceComplete					ntdll		1367
imp	'RtlRunOnceExecuteOnce'					RtlRunOnceExecuteOnce					ntdll		1368
imp	'RtlRunOnceInitialize'					RtlRunOnceInitialize					ntdll		1369
imp	'RtlSecondsSince1970ToTime'				RtlSecondsSince1970ToTime				ntdll		1370
imp	'RtlSecondsSince1980ToTime'				RtlSecondsSince1980ToTime				ntdll		1371
imp	'RtlSeekMemoryStream'					RtlSeekMemoryStream					ntdll		1372
imp	'RtlSelfRelativeToAbsoluteSD'				RtlSelfRelativeToAbsoluteSD				ntdll		1373
imp	'RtlSelfRelativeToAbsoluteSD2'				RtlSelfRelativeToAbsoluteSD2				ntdll		1374
imp	'RtlSendMsgToSm'					RtlSendMsgToSm						ntdll		1375
imp	'RtlSetAllBits'						RtlSetAllBits						ntdll		1376
imp	'RtlSetAttributesSecurityDescriptor'			RtlSetAttributesSecurityDescriptor			ntdll		1377
imp	'RtlSetBit'						RtlSetBit						ntdll		1378
imp	'RtlSetBits'						RtlSetBits						ntdll		1379
imp	'RtlSetControlSecurityDescriptor'			RtlSetControlSecurityDescriptor				ntdll		1380
imp	'RtlSetCriticalSectionSpinCount'			RtlSetCriticalSectionSpinCount				ntdll		1381
imp	'RtlSetCurrentDirectory_U'				RtlSetCurrentDirectory_U				ntdll		1382
imp	'RtlSetCurrentEnvironment'				RtlSetCurrentEnvironment				ntdll		1383
imp	'RtlSetCurrentTransaction'				RtlSetCurrentTransaction				ntdll		1384
imp	'RtlSetDaclSecurityDescriptor'				RtlSetDaclSecurityDescriptor				ntdll		1385
imp	'RtlSetDynamicTimeZoneInformation'			RtlSetDynamicTimeZoneInformation			ntdll		1386
imp	'RtlSetEnvironmentStrings'				RtlSetEnvironmentStrings				ntdll		1387
imp	'RtlSetEnvironmentVar'					RtlSetEnvironmentVar					ntdll		1388
imp	'RtlSetEnvironmentVariable'				RtlSetEnvironmentVariable				ntdll		1389   
imp	'RtlSetExtendedFeaturesMask'				RtlSetExtendedFeaturesMask				ntdll		1390
imp	'RtlSetGroupSecurityDescriptor'				RtlSetGroupSecurityDescriptor				ntdll		1391
imp	'RtlSetHeapInformation'					RtlSetHeapInformation					ntdll		1392
imp	'RtlSetImageMitigationPolicy'				RtlSetImageMitigationPolicy				ntdll		1393
imp	'RtlSetInformationAcl'					RtlSetInformationAcl					ntdll		1394
imp	'RtlSetIoCompletionCallback'				RtlSetIoCompletionCallback				ntdll		1395
imp	'RtlSetLastWin32Error'					RtlSetLastWin32Error					ntdll		1396
imp	'RtlSetLastWin32ErrorAndNtStatusFromNtStatus'		RtlSetLastWin32ErrorAndNtStatusFromNtStatus		ntdll		1397
imp	'RtlSetMemoryStreamSize'				RtlSetMemoryStreamSize					ntdll		1398
imp	'RtlSetOwnerSecurityDescriptor'				RtlSetOwnerSecurityDescriptor				ntdll		1399
imp	'RtlSetPortableOperatingSystem'				RtlSetPortableOperatingSystem				ntdll		1400
imp	'RtlSetProcessDebugInformation'				RtlSetProcessDebugInformation				ntdll		1401
imp	'RtlSetProcessIsCritical'				RtlSetProcessIsCritical					ntdll		1402
imp	'RtlSetProcessPlaceholderCompatibilityMode'		RtlSetProcessPlaceholderCompatibilityMode		ntdll		1403
imp	'RtlSetProcessPreferredUILanguages'			RtlSetProcessPreferredUILanguages			ntdll		1404
imp	'RtlSetProtectedPolicy'					RtlSetProtectedPolicy					ntdll		1405
imp	'RtlSetProxiedProcessId'				RtlSetProxiedProcessId					ntdll		1406
imp	'RtlSetSaclSecurityDescriptor'				RtlSetSaclSecurityDescriptor				ntdll		1407
imp	'RtlSetSearchPathMode'					RtlSetSearchPathMode					ntdll		1408
imp	'RtlSetSecurityDescriptorRMControl'			RtlSetSecurityDescriptorRMControl			ntdll		1409
imp	'RtlSetSecurityObject'					RtlSetSecurityObject					ntdll		1410
imp	'RtlSetSecurityObjectEx'				RtlSetSecurityObjectEx					ntdll		1411
imp	'RtlSetSystemBootStatus'				RtlSetSystemBootStatus					ntdll		1412
imp	'RtlSetSystemBootStatusEx'				RtlSetSystemBootStatusEx				ntdll		1413
imp	'RtlSetThreadErrorMode'					RtlSetThreadErrorMode					ntdll		1414
imp	'RtlSetThreadIsCritical'				RtlSetThreadIsCritical					ntdll		1415
imp	'RtlSetThreadPlaceholderCompatibilityMode'		RtlSetThreadPlaceholderCompatibilityMode		ntdll		1416
imp	'RtlSetThreadPoolStartFunc'				RtlSetThreadPoolStartFunc				ntdll		1417
imp	'RtlSetThreadPreferredUILanguages'			RtlSetThreadPreferredUILanguages			ntdll		1418
imp	'RtlSetThreadSubProcessTag'				RtlSetThreadSubProcessTag				ntdll		1419
imp	'RtlSetThreadWorkOnBehalfTicket'			RtlSetThreadWorkOnBehalfTicket				ntdll		1420
imp	'RtlSetTimeZoneInformation'				RtlSetTimeZoneInformation				ntdll		1421
imp	'RtlSetTimer'						RtlSetTimer						ntdll		1422
imp	'RtlSetUmsThreadInformation'				RtlSetUmsThreadInformation				ntdll		1423
imp	'RtlSetUnhandledExceptionFilter'			RtlSetUnhandledExceptionFilter				ntdll		1424
imp	'RtlSetUserFlagsHeap'					RtlSetUserFlagsHeap					ntdll		1425
imp	'RtlSetUserValueHeap'					RtlSetUserValueHeap					ntdll		1426
imp	'RtlSidDominates'					RtlSidDominates						ntdll		1427
imp	'RtlSidDominatesForTrust'				RtlSidDominatesForTrust					ntdll		1428
imp	'RtlSidEqualLevel'					RtlSidEqualLevel					ntdll		1429
imp	'RtlSidHashInitialize'					RtlSidHashInitialize					ntdll		1430
imp	'RtlSidHashLookup'					RtlSidHashLookup					ntdll		1431
imp	'RtlSidIsHigherLevel'					RtlSidIsHigherLevel					ntdll		1432
imp	'RtlSizeHeap'						RtlSizeHeap						ntdll		1433	3
imp	'RtlSleepConditionVariableCS'				RtlSleepConditionVariableCS				ntdll		1434
imp	'RtlSleepConditionVariableSRW'				RtlSleepConditionVariableSRW				ntdll		1435
imp	'RtlSplay'						RtlSplay						ntdll		1436
imp	'RtlStartRXact'						RtlStartRXact						ntdll		1437
imp	'RtlStatMemoryStream'					RtlStatMemoryStream					ntdll		1438
imp	'RtlStringFromGUID'					RtlStringFromGUID					ntdll		1439
imp	'RtlStringFromGUIDEx'					RtlStringFromGUIDEx					ntdll		1440
imp	'RtlStronglyEnumerateEntryHashTable'			RtlStronglyEnumerateEntryHashTable			ntdll		1441
imp	'RtlSubAuthorityCountSid'				RtlSubAuthorityCountSid					ntdll		1442
imp	'RtlSubAuthoritySid'					RtlSubAuthoritySid					ntdll		1443
imp	'RtlSubscribeWnfStateChangeNotification'		RtlSubscribeWnfStateChangeNotification			ntdll		1444
imp	'RtlSubtreePredecessor'					RtlSubtreePredecessor					ntdll		1445
imp	'RtlSubtreeSuccessor'					RtlSubtreeSuccessor					ntdll		1446
imp	'RtlSwitchedVVI'					RtlSwitchedVVI						ntdll		1447
imp	'RtlSystemTimeToLocalTime'				RtlSystemTimeToLocalTime				ntdll		1448
imp	'RtlTestAndPublishWnfStateData'				RtlTestAndPublishWnfStateData				ntdll		1449
imp	'RtlTestBit'						RtlTestBit						ntdll		1450
imp	'RtlTestBitEx'						RtlTestBitEx						ntdll		1451
imp	'RtlTestProtectedAccess'				RtlTestProtectedAccess					ntdll		1452
imp	'RtlTimeFieldsToTime'					RtlTimeFieldsToTime					ntdll		1453
imp	'RtlTimeToElapsedTimeFields'				RtlTimeToElapsedTimeFields				ntdll		1454
imp	'RtlTimeToSecondsSince1970'				RtlTimeToSecondsSince1970				ntdll		1455
imp	'RtlTimeToSecondsSince1980'				RtlTimeToSecondsSince1980				ntdll		1456
imp	'RtlTimeToTimeFields'					RtlTimeToTimeFields					ntdll		1457
imp	'RtlTraceDatabaseAdd'					RtlTraceDatabaseAdd					ntdll		1458
imp	'RtlTraceDatabaseCreate'				RtlTraceDatabaseCreate					ntdll		1459
imp	'RtlTraceDatabaseDestroy'				RtlTraceDatabaseDestroy					ntdll		1460
imp	'RtlTraceDatabaseEnumerate'				RtlTraceDatabaseEnumerate				ntdll		1461
imp	'RtlTraceDatabaseFind'					RtlTraceDatabaseFind					ntdll		1462
imp	'RtlTraceDatabaseLock'					RtlTraceDatabaseLock					ntdll		1463
imp	'RtlTraceDatabaseUnlock'				RtlTraceDatabaseUnlock					ntdll		1464
imp	'RtlTraceDatabaseValidate'				RtlTraceDatabaseValidate				ntdll		1465
imp	'RtlTryAcquirePebLock'					RtlTryAcquirePebLock					ntdll		1466
imp	'RtlTryAcquireSRWLockExclusive'				RtlTryAcquireSRWLockExclusive				ntdll		1467
imp	'RtlTryAcquireSRWLockShared'				RtlTryAcquireSRWLockShared				ntdll		1468
imp	'RtlTryConvertSRWLockSharedToExclusiveOrRelease'	RtlTryConvertSRWLockSharedToExclusiveOrRelease		ntdll		1469
imp	'RtlTryEnterCriticalSection'				RtlTryEnterCriticalSection				ntdll		1470	1
imp	'RtlUTF8ToUnicodeN'					RtlUTF8ToUnicodeN					ntdll		1471
imp	'RtlUmsThreadYield'					RtlUmsThreadYield					ntdll		1472
imp	'RtlUnhandledExceptionFilter'				RtlUnhandledExceptionFilter				ntdll		1473
imp	'RtlUnhandledExceptionFilter2'				RtlUnhandledExceptionFilter2				ntdll		1474
imp	'RtlUnicodeStringToAnsiSize'				RtlUnicodeStringToAnsiSize				ntdll		1475
imp	'RtlUnicodeStringToAnsiString'				RtlUnicodeStringToAnsiString				ntdll		1476
imp	'RtlUnicodeStringToCountedOemString'			RtlUnicodeStringToCountedOemString			ntdll		1477
imp	'RtlUnicodeStringToInteger'				RtlUnicodeStringToInteger				ntdll		1478
imp	'RtlUnicodeStringToOemSize'				RtlUnicodeStringToOemSize				ntdll		1479
imp	'RtlUnicodeStringToOemString'				RtlUnicodeStringToOemString				ntdll		1480
imp	'RtlUnicodeToCustomCPN'					RtlUnicodeToCustomCPN					ntdll		1481
imp	'RtlUnicodeToMultiByteN'				RtlUnicodeToMultiByteN					ntdll		1482
imp	'RtlUnicodeToMultiByteSize'				RtlUnicodeToMultiByteSize				ntdll		1483
imp	'RtlUnicodeToOemN'					RtlUnicodeToOemN					ntdll		1484
imp	'RtlUnicodeToUTF8N'					RtlUnicodeToUTF8N					ntdll		1485
imp	'RtlUniform'						RtlUniform						ntdll		1486
imp	'RtlUnlockBootStatusData'				RtlUnlockBootStatusData					ntdll		1487
imp	'RtlUnlockCurrentThread'				RtlUnlockCurrentThread					ntdll		1488
imp	'RtlUnlockHeap'						RtlUnlockHeap						ntdll		1489	1
imp	'RtlUnlockMemoryBlockLookaside'				RtlUnlockMemoryBlockLookaside				ntdll		1490
imp	'RtlUnlockMemoryStreamRegion'				RtlUnlockMemoryStreamRegion				ntdll		1491
imp	'RtlUnlockMemoryZone'					RtlUnlockMemoryZone					ntdll		1492
imp	'RtlUnlockModuleSection'				RtlUnlockModuleSection					ntdll		1493
imp	'RtlUnsubscribeWnfNotificationWaitForCompletion'	RtlUnsubscribeWnfNotificationWaitForCompletion		ntdll		1494
imp	'RtlUnsubscribeWnfNotificationWithCompletionCallback'	RtlUnsubscribeWnfNotificationWithCompletionCallback	ntdll		1495
imp	'RtlUnsubscribeWnfStateChangeNotification'		RtlUnsubscribeWnfStateChangeNotification		ntdll		1496
imp	'RtlUnwind'						RtlUnwind						ntdll		1497
imp	'RtlUnwindEx'						RtlUnwindEx						ntdll		1498
imp	'RtlUpcaseUnicodeChar'					RtlUpcaseUnicodeChar					ntdll		1499
imp	'RtlUpcaseUnicodeString'				RtlUpcaseUnicodeString					ntdll		1500
imp	'RtlUpcaseUnicodeStringToAnsiString'			RtlUpcaseUnicodeStringToAnsiString			ntdll		1501
imp	'RtlUpcaseUnicodeStringToCountedOemString'		RtlUpcaseUnicodeStringToCountedOemString		ntdll		1502
imp	'RtlUpcaseUnicodeStringToOemString'			RtlUpcaseUnicodeStringToOemString			ntdll		1503
imp	'RtlUpcaseUnicodeToCustomCPN'				RtlUpcaseUnicodeToCustomCPN				ntdll		1504
imp	'RtlUpcaseUnicodeToMultiByteN'				RtlUpcaseUnicodeToMultiByteN				ntdll		1505
imp	'RtlUpcaseUnicodeToOemN'				RtlUpcaseUnicodeToOemN					ntdll		1506
imp	'RtlUpdateClonedCriticalSection'			RtlUpdateClonedCriticalSection				ntdll		1507
imp	'RtlUpdateClonedSRWLock'				RtlUpdateClonedSRWLock					ntdll		1508
imp	'RtlUpdateTimer'					RtlUpdateTimer						ntdll		1509
imp	'RtlUpperChar'						RtlUpperChar						ntdll		1510
imp	'RtlUpperString'					RtlUpperString						ntdll		1511
imp	'RtlUserThreadStart'					RtlUserThreadStart					ntdll		1512
imp	'RtlValidAcl'						RtlValidAcl						ntdll		1513
imp	'RtlValidProcessProtection'				RtlValidProcessProtection				ntdll		1514
imp	'RtlValidRelativeSecurityDescriptor'			RtlValidRelativeSecurityDescriptor			ntdll		1515
imp	'RtlValidSecurityDescriptor'				RtlValidSecurityDescriptor				ntdll		1516
imp	'RtlValidSid'						RtlValidSid						ntdll		1517
imp	'RtlValidateCorrelationVector'				RtlValidateCorrelationVector				ntdll		1518
imp	'RtlValidateHeap'					RtlValidateHeap						ntdll		1519	3
imp	'RtlValidateProcessHeaps'				RtlValidateProcessHeaps					ntdll		1520
imp	'RtlValidateUnicodeString'				RtlValidateUnicodeString				ntdll		1521
imp	'RtlVerifyVersionInfo'					RtlVerifyVersionInfo					ntdll		1522
imp	'RtlVirtualUnwind'					RtlVirtualUnwind					ntdll		1523
imp	'RtlWaitForWnfMetaNotification'				RtlWaitForWnfMetaNotification				ntdll		1524
imp	'RtlWaitOnAddress'					RtlWaitOnAddress					ntdll		1525
imp	'RtlWakeAddressAll'					RtlWakeAddressAll					ntdll		1526
imp	'RtlWakeAddressAllNoFence'				RtlWakeAddressAllNoFence				ntdll		1527
imp	'RtlWakeAddressSingle'					RtlWakeAddressSingle					ntdll		1528
imp	'RtlWakeAddressSingleNoFence'				RtlWakeAddressSingleNoFence				ntdll		1529
imp	'RtlWakeAllConditionVariable'				RtlWakeAllConditionVariable				ntdll		1530
imp	'RtlWakeConditionVariable'				RtlWakeConditionVariable				ntdll		1531
imp	'RtlWalkFrameChain'					RtlWalkFrameChain					ntdll		1532
imp	'RtlWalkHeap'						RtlWalkHeap						ntdll		1533	2
imp	'RtlWeaklyEnumerateEntryHashTable'			RtlWeaklyEnumerateEntryHashTable			ntdll		1534
imp	'RtlWerpReportException'				RtlWerpReportException					ntdll		1535
imp	'RtlWnfCompareChangeStamp'				RtlWnfCompareChangeStamp				ntdll		1536
imp	'RtlWnfDllUnloadCallback'				RtlWnfDllUnloadCallback					ntdll		1537
imp	'RtlWow64CallFunction64'				RtlWow64CallFunction64					ntdll		1538
imp	'RtlWow64EnableFsRedirection'				RtlWow64EnableFsRedirection				ntdll		1539
imp	'RtlWow64EnableFsRedirectionEx'				RtlWow64EnableFsRedirectionEx				ntdll		1540
imp	'RtlWow64GetCpuAreaInfo'				RtlWow64GetCpuAreaInfo					ntdll		1541
imp	'RtlWow64GetCurrentCpuArea'				RtlWow64GetCurrentCpuArea				ntdll		1542
imp	'RtlWow64GetCurrentMachine'				RtlWow64GetCurrentMachine				ntdll		1543
imp	'RtlWow64GetEquivalentMachineCHPE'			RtlWow64GetEquivalentMachineCHPE			ntdll		1544
imp	'RtlWow64GetProcessMachines'				RtlWow64GetProcessMachines				ntdll		1545
imp	'RtlWow64GetSharedInfoProcess'				RtlWow64GetSharedInfoProcess				ntdll		1546
imp	'RtlWow64GetThreadContext'				RtlWow64GetThreadContext				ntdll		1547
imp	'RtlWow64GetThreadSelectorEntry'			RtlWow64GetThreadSelectorEntry				ntdll		1548
imp	'RtlWow64IsWowGuestMachineSupported'			RtlWow64IsWowGuestMachineSupported			ntdll		1549
imp	'RtlWow64LogMessageInEventLogger'			RtlWow64LogMessageInEventLogger				ntdll		1550
imp	'RtlWow64PopAllCrossProcessWork'			RtlWow64PopAllCrossProcessWork				ntdll		1551
imp	'RtlWow64PopCrossProcessWork'				RtlWow64PopCrossProcessWork				ntdll		1552
imp	'RtlWow64PushCrossProcessWork'				RtlWow64PushCrossProcessWork				ntdll		1553
imp	'RtlWow64SetThreadContext'				RtlWow64SetThreadContext				ntdll		1554
imp	'RtlWow64SuspendThread'					RtlWow64SuspendThread					ntdll		1555
imp	'RtlWriteMemoryStream'					RtlWriteMemoryStream					ntdll		1556
imp	'RtlWriteNonVolatileMemory'				RtlWriteNonVolatileMemory				ntdll		1557
imp	'RtlWriteRegistryValue'					RtlWriteRegistryValue					ntdll		1558
imp	'RtlZeroHeap'						RtlZeroHeap						ntdll		1559
imp	'RtlZeroMemory'						RtlZeroMemory						ntdll		1560
imp	'RtlZombifyActivationContext'				RtlZombifyActivationContext				ntdll		1561
imp	'RtlpApplyLengthFunction'				RtlpApplyLengthFunction					ntdll		1562
imp	'RtlpCheckDynamicTimeZoneInformation'			RtlpCheckDynamicTimeZoneInformation			ntdll		1563
imp	'RtlpCleanupRegistryKeys'				RtlpCleanupRegistryKeys					ntdll		1564
imp	'RtlpConvertAbsoluteToRelativeSecurityAttribute'	RtlpConvertAbsoluteToRelativeSecurityAttribute		ntdll		1565
imp	'RtlpConvertCultureNamesToLCIDs'			RtlpConvertCultureNamesToLCIDs				ntdll		1566
imp	'RtlpConvertLCIDsToCultureNames'			RtlpConvertLCIDsToCultureNames				ntdll		1567
imp	'RtlpConvertRelativeToAbsoluteSecurityAttribute'	RtlpConvertRelativeToAbsoluteSecurityAttribute		ntdll		1568
imp	'RtlpCreateProcessRegistryInfo'				RtlpCreateProcessRegistryInfo				ntdll		1569
imp	'RtlpEnsureBufferSize'					RtlpEnsureBufferSize					ntdll		1570
imp	'RtlpExecuteUmsThread'					RtlpExecuteUmsThread					ntdll		1571
imp	'RtlpFreezeTimeBias'					RtlpFreezeTimeBias					ntdll		1572
imp	'RtlpGetDeviceFamilyInfoEnum'				RtlpGetDeviceFamilyInfoEnum				ntdll		1573
imp	'RtlpGetLCIDFromLangInfoNode'				RtlpGetLCIDFromLangInfoNode				ntdll		1574
imp	'RtlpGetNameFromLangInfoNode'				RtlpGetNameFromLangInfoNode				ntdll		1575
imp	'RtlpGetSystemDefaultUILanguage'			RtlpGetSystemDefaultUILanguage				ntdll		1576
imp	'RtlpGetUserOrMachineUILanguage4NLS'			RtlpGetUserOrMachineUILanguage4NLS			ntdll		1577
imp	'RtlpInitializeLangRegistryInfo'			RtlpInitializeLangRegistryInfo				ntdll		1578
imp	'RtlpIsQualifiedLanguage'				RtlpIsQualifiedLanguage					ntdll		1579
imp	'RtlpLoadMachineUIByPolicy'				RtlpLoadMachineUIByPolicy				ntdll		1580
imp	'RtlpLoadUserUIByPolicy'				RtlpLoadUserUIByPolicy					ntdll		1581
imp	'RtlpMergeSecurityAttributeInformation'			RtlpMergeSecurityAttributeInformation			ntdll		1582
imp	'RtlpMuiFreeLangRegistryInfo'				RtlpMuiFreeLangRegistryInfo				ntdll		1583
imp	'RtlpMuiRegCreateRegistryInfo'				RtlpMuiRegCreateRegistryInfo				ntdll		1584
imp	'RtlpMuiRegFreeRegistryInfo'				RtlpMuiRegFreeRegistryInfo				ntdll		1585
imp	'RtlpMuiRegLoadRegistryInfo'				RtlpMuiRegLoadRegistryInfo				ntdll		1586
imp	'RtlpNotOwnerCriticalSection'				RtlpNotOwnerCriticalSection				ntdll		1587
imp	'RtlpNtCreateKey'					RtlpNtCreateKey						ntdll		1588
imp	'RtlpNtEnumerateSubKey'					RtlpNtEnumerateSubKey					ntdll		1589
imp	'RtlpNtMakeTemporaryKey'				RtlpNtMakeTemporaryKey					ntdll		1590
imp	'RtlpNtOpenKey'						RtlpNtOpenKey						ntdll		1591
imp	'RtlpNtQueryValueKey'					RtlpNtQueryValueKey					ntdll		1592
imp	'RtlpNtSetValueKey'					RtlpNtSetValueKey					ntdll		1593
imp	'RtlpQueryDefaultUILanguage'				RtlpQueryDefaultUILanguage				ntdll		1594
imp	'RtlpQueryProcessDebugInformationFromWow64'		RtlpQueryProcessDebugInformationFromWow64		ntdll		1595
imp	'RtlpQueryProcessDebugInformationRemote'		RtlpQueryProcessDebugInformationRemote			ntdll		1596
imp	'RtlpRefreshCachedUILanguage'				RtlpRefreshCachedUILanguage				ntdll		1597
imp	'RtlpSetInstallLanguage'				RtlpSetInstallLanguage					ntdll		1598
imp	'RtlpSetPreferredUILanguages'				RtlpSetPreferredUILanguages				ntdll		1599
imp	'RtlpSetUserPreferredUILanguages'			RtlpSetUserPreferredUILanguages				ntdll		1600
imp	'RtlpUmsExecuteYieldThreadEnd'				RtlpUmsExecuteYieldThreadEnd				ntdll		1601
imp	'RtlpUmsThreadYield'					RtlpUmsThreadYield					ntdll		1602
imp	'RtlpUnWaitCriticalSection'				RtlpUnWaitCriticalSection				ntdll		1603
imp	'RtlpVerifyAndCommitUILanguageSettings'			RtlpVerifyAndCommitUILanguageSettings			ntdll		1604
imp	'RtlpWaitForCriticalSection'				RtlpWaitForCriticalSection				ntdll		1605
imp	'RtlxAnsiStringToUnicodeSize'				RtlxAnsiStringToUnicodeSize				ntdll		1606
imp	'RtlxOemStringToUnicodeSize'				RtlxOemStringToUnicodeSize				ntdll		1607
imp	'RtlxUnicodeStringToAnsiSize'				RtlxUnicodeStringToAnsiSize				ntdll		1608
imp	'RtlxUnicodeStringToOemSize'				RtlxUnicodeStringToOemSize				ntdll		1609
imp	'RunAsNewUser_RunDLLW'					RunAsNewUser_RunDLLW					shell32		314
imp	'RxNetAccessAdd'					RxNetAccessAdd						netapi32	288
imp	'RxNetAccessDel'					RxNetAccessDel						netapi32	289
imp	'RxNetAccessEnum'					RxNetAccessEnum						netapi32	290
imp	'RxNetAccessGetInfo'					RxNetAccessGetInfo					netapi32	291
imp	'RxNetAccessGetUserPerms'				RxNetAccessGetUserPerms					netapi32	292
imp	'RxNetAccessSetInfo'					RxNetAccessSetInfo					netapi32	293
imp	'RxNetServerEnum'					RxNetServerEnum						netapi32	294
imp	'RxNetUserPasswordSet'					RxNetUserPasswordSet					netapi32	295
imp	'RxRemoteApi'						RxRemoteApi						netapi32	296
imp	'SHAddDefaultPropertiesByExt'				SHAddDefaultPropertiesByExt				shell32		315
imp	'SHAddFromPropSheetExtArray'				SHAddFromPropSheetExtArray				shell32		167
imp	'SHAddToRecentDocs'					SHAddToRecentDocs					shell32		316
imp	'SHAlloc'						SHAlloc							shell32		196
imp	'SHAppBarMessage'					SHAppBarMessage						shell32		317
imp	'SHAssocEnumHandlers'					SHAssocEnumHandlers					shell32		318
imp	'SHAssocEnumHandlersForProtocolByApplication'		SHAssocEnumHandlersForProtocolByApplication		shell32		319
imp	'SHBindToFolderIDListParent'				SHBindToFolderIDListParent				shell32		320
imp	'SHBindToFolderIDListParentEx'				SHBindToFolderIDListParentEx				shell32		321
imp	'SHBindToObject'					SHBindToObject						shell32		322
imp	'SHBindToParent'					SHBindToParent						shell32		323
imp	'SHBrowseForFolderA'					SHBrowseForFolderA					shell32		325
imp	'SHBrowseForFolder'					SHBrowseForFolderW					shell32		326
imp	'SHCLSIDFromString'					SHCLSIDFromString					shell32		147
imp	'SHChangeNotification_Lock'				SHChangeNotification_Lock				shell32		644
imp	'SHChangeNotification_Unlock'				SHChangeNotification_Unlock				shell32		645
imp	'SHChangeNotify'					SHChangeNotify						shell32		327
imp	'SHChangeNotifyDeregister'				SHChangeNotifyDeregister				shell32		4
imp	'SHChangeNotifyRegister'				SHChangeNotifyRegister					shell32		2
imp	'SHChangeNotifyRegisterThread'				SHChangeNotifyRegisterThread				shell32		328
imp	'SHChangeNotifySuspendResume'				SHChangeNotifySuspendResume				shell32		329
imp	'SHCloneSpecialIDList'					SHCloneSpecialIDList					shell32		89
imp	'SHCoCreateInstance'					SHCoCreateInstance					KernelBase	1420
imp	'SHCoCreateInstanceWorker'				SHCoCreateInstanceWorker				shell32		330
imp	'SHCreateAssociationRegistration'			SHCreateAssociationRegistration				shell32		331
imp	'SHCreateCategoryEnum'					SHCreateCategoryEnum					shell32		332
imp	'SHCreateDataObject'					SHCreateDataObject					shell32		333
imp	'SHCreateDefaultContextMenu'				SHCreateDefaultContextMenu				shell32		334
imp	'SHCreateDefaultExtractIcon'				SHCreateDefaultExtractIcon				shell32		335
imp	'SHCreateDefaultPropertiesOp'				SHCreateDefaultPropertiesOp				shell32		336
imp	'SHCreateDirectory'					SHCreateDirectory					shell32		165
imp	'SHCreateDirectoryExA'					SHCreateDirectoryExA					shell32		337
imp	'SHCreateDirectoryEx'					SHCreateDirectoryExW					shell32		338
imp	'SHCreateDrvExtIcon'					SHCreateDrvExtIcon					shell32		339
imp	'SHCreateFileExtractIcon'				SHCreateFileExtractIconW				shell32		743
imp	'SHCreateItemFromIDList'				SHCreateItemFromIDList					shell32		340
imp	'SHCreateItemFromParsingName'				SHCreateItemFromParsingName				shell32		341
imp	'SHCreateItemFromRelativeName'				SHCreateItemFromRelativeName				shell32		342
imp	'SHCreateItemInKnownFolder'				SHCreateItemInKnownFolder				shell32		343
imp	'SHCreateItemWithParent'				SHCreateItemWithParent					shell32		344
imp	'SHCreateLocalServerRunDll'				SHCreateLocalServerRunDll				shell32		345
imp	'SHCreateProcessAsUser'					SHCreateProcessAsUserW					shell32		346
imp	'SHCreatePropSheetExtArray'				SHCreatePropSheetExtArray				shell32		168
imp	'SHCreateQueryCancelAutoPlayMoniker'			SHCreateQueryCancelAutoPlayMoniker			shell32		347
imp	'SHCreateShellFolderView'				SHCreateShellFolderView					shell32		256
imp	'SHCreateShellFolderViewEx'				SHCreateShellFolderViewEx				shell32		174
imp	'SHCreateShellItem'					SHCreateShellItem					shell32		348
imp	'SHCreateShellItemArray'				SHCreateShellItemArray					shell32		349
imp	'SHCreateShellItemArrayFromDataObject'			SHCreateShellItemArrayFromDataObject			shell32		350
imp	'SHCreateShellItemArrayFromIDLists'			SHCreateShellItemArrayFromIDLists			shell32		351
imp	'SHCreateShellItemArrayFromShellItem'			SHCreateShellItemArrayFromShellItem			shell32		352
imp	'SHCreateStdEnumFmtEtc'					SHCreateStdEnumFmtEtc					shell32		74
imp	'SHDefExtractIconA'					SHDefExtractIconA					shell32		3
imp	'SHDefExtractIcon'					SHDefExtractIconW					shell32		6
imp	'SHDestroyPropSheetExtArray'				SHDestroyPropSheetExtArray				shell32		169
imp	'SHDoDragDrop'						SHDoDragDrop						shell32		88
imp	'SHELL32_AddToBackIconTable'				SHELL32_AddToBackIconTable				shell32		353
imp	'SHELL32_AddToFrontIconTable'				SHELL32_AddToFrontIconTable				shell32		354
imp	'SHELL32_AreAllItemsAvailable'				SHELL32_AreAllItemsAvailable				shell32		355
imp	'SHELL32_BindToFilePlaceholderHandler'			SHELL32_BindToFilePlaceholderHandler			shell32		356
imp	'SHELL32_CCommonPlacesFolder_CreateInstance'		SHELL32_CCommonPlacesFolder_CreateInstance		shell32		357
imp	'SHELL32_CDBurn_CloseSession'				SHELL32_CDBurn_CloseSession				shell32		358
imp	'SHELL32_CDBurn_DriveSupportedForDataBurn'		SHELL32_CDBurn_DriveSupportedForDataBurn		shell32		359
imp	'SHELL32_CDBurn_Erase'					SHELL32_CDBurn_Erase					shell32		360
imp	'SHELL32_CDBurn_GetCDInfo'				SHELL32_CDBurn_GetCDInfo				shell32		361
imp	'SHELL32_CDBurn_GetLiveFSDiscInfo'			SHELL32_CDBurn_GetLiveFSDiscInfo			shell32		362
imp	'SHELL32_CDBurn_GetStagingPathOrNormalPath'		SHELL32_CDBurn_GetStagingPathOrNormalPath		shell32		363
imp	'SHELL32_CDBurn_GetTaskInfo'				SHELL32_CDBurn_GetTaskInfo				shell32		364
imp	'SHELL32_CDBurn_IsBlankDisc'				SHELL32_CDBurn_IsBlankDisc				shell32		365
imp	'SHELL32_CDBurn_IsBlankDisc2'				SHELL32_CDBurn_IsBlankDisc2				shell32		366
imp	'SHELL32_CDBurn_IsLiveFS'				SHELL32_CDBurn_IsLiveFS					shell32		367
imp	'SHELL32_CDBurn_OnDeviceChange'				SHELL32_CDBurn_OnDeviceChange				shell32		368
imp	'SHELL32_CDBurn_OnEject'				SHELL32_CDBurn_OnEject					shell32		369
imp	'SHELL32_CDBurn_OnMediaChange'				SHELL32_CDBurn_OnMediaChange				shell32		370
imp	'SHELL32_CDefFolderMenu_Create2'			SHELL32_CDefFolderMenu_Create2				shell32		371
imp	'SHELL32_CDefFolderMenu_Create2Ex'			SHELL32_CDefFolderMenu_Create2Ex			shell32		372
imp	'SHELL32_CDefFolderMenu_MergeMenu'			SHELL32_CDefFolderMenu_MergeMenu			shell32		373
imp	'SHELL32_CDrivesContextMenu_Create'			SHELL32_CDrivesContextMenu_Create			shell32		374
imp	'SHELL32_CDrivesDropTarget_Create'			SHELL32_CDrivesDropTarget_Create			shell32		375
imp	'SHELL32_CDrives_CreateSFVCB'				SHELL32_CDrives_CreateSFVCB				shell32		376
imp	'SHELL32_CFSDropTarget_CreateInstance'			SHELL32_CFSDropTarget_CreateInstance			shell32		377
imp	'SHELL32_CFSFolderCallback_Create'			SHELL32_CFSFolderCallback_Create			shell32		378
imp	'SHELL32_CFillPropertiesTask_CreateInstance'		SHELL32_CFillPropertiesTask_CreateInstance		shell32		379
imp	'SHELL32_CLibraryDropTarget_CreateInstance'		SHELL32_CLibraryDropTarget_CreateInstance		shell32		380
imp	'SHELL32_CLocationContextMenu_Create'			SHELL32_CLocationContextMenu_Create			shell32		381
imp	'SHELL32_CLocationFolderUI_CreateInstance'		SHELL32_CLocationFolderUI_CreateInstance		shell32		382
imp	'SHELL32_CMountPoint_DoAutorun'				SHELL32_CMountPoint_DoAutorun				shell32		383
imp	'SHELL32_CMountPoint_DoAutorunPrompt'			SHELL32_CMountPoint_DoAutorunPrompt			shell32		384
imp	'SHELL32_CMountPoint_IsAutoRunDriveAndEnabledByPolicy'	SHELL32_CMountPoint_IsAutoRunDriveAndEnabledByPolicy	shell32		385
imp	'SHELL32_CMountPoint_ProcessAutoRunFile'		SHELL32_CMountPoint_ProcessAutoRunFile			shell32		386
imp	'SHELL32_CMountPoint_WantAutorunUI'			SHELL32_CMountPoint_WantAutorunUI			shell32		387
imp	'SHELL32_CMountPoint_WantAutorunUIGetReady'		SHELL32_CMountPoint_WantAutorunUIGetReady		shell32		388
imp	'SHELL32_CPL_CategoryIdArrayFromVariant'		SHELL32_CPL_CategoryIdArrayFromVariant			shell32		389
imp	'SHELL32_CPL_IsLegacyCanonicalNameListedUnderKey'	SHELL32_CPL_IsLegacyCanonicalNameListedUnderKey		shell32		390
imp	'SHELL32_CPL_ModifyWowDisplayName'			SHELL32_CPL_ModifyWowDisplayName			shell32		391
imp	'SHELL32_CRecentDocsContextMenu_CreateInstance'		SHELL32_CRecentDocsContextMenu_CreateInstance		shell32		392
imp	'SHELL32_CSyncRootManager_CreateInstance'		SHELL32_CSyncRootManager_CreateInstance			shell32		393
imp	'SHELL32_CTransferConfirmation_CreateInstance'		SHELL32_CTransferConfirmation_CreateInstance		shell32		394
imp	'SHELL32_CallFileCopyHooks'				SHELL32_CallFileCopyHooks				shell32		395
imp	'SHELL32_CanDisplayWin8CopyDialog'			SHELL32_CanDisplayWin8CopyDialog			shell32		396
imp	'SHELL32_CloseAutoplayPrompt'				SHELL32_CloseAutoplayPrompt				shell32		397
imp	'SHELL32_CommandLineFromMsiDescriptor'			SHELL32_CommandLineFromMsiDescriptor			shell32		398
imp	'SHELL32_CopyFilePlaceholderToNewFile'			SHELL32_CopyFilePlaceholderToNewFile			shell32		399
imp	'SHELL32_CopySecondaryTiles'				SHELL32_CopySecondaryTiles				shell32		400
imp	'SHELL32_CreateConfirmationInterrupt'			SHELL32_CreateConfirmationInterrupt			shell32		401
imp	'SHELL32_CreateConflictInterrupt'			SHELL32_CreateConflictInterrupt				shell32		402
imp	'SHELL32_CreateDefaultOperationDataProvider'		SHELL32_CreateDefaultOperationDataProvider		shell32		403
imp	'SHELL32_CreateFileFolderContextMenu'			SHELL32_CreateFileFolderContextMenu			shell32		404
imp	'SHELL32_CreateLinkInfo'				SHELL32_CreateLinkInfoW					shell32		405
imp	'SHELL32_CreatePlaceholderFile'				SHELL32_CreatePlaceholderFile				shell32		406
imp	'SHELL32_CreateQosRecorder'				SHELL32_CreateQosRecorder				shell32		407
imp	'SHELL32_CreateSharePointView'				SHELL32_CreateSharePointView				shell32		408
imp	'SHELL32_Create_IEnumUICommand'				SHELL32_Create_IEnumUICommand				shell32		409
imp	'SHELL32_DestroyLinkInfo'				SHELL32_DestroyLinkInfo					shell32		410
imp	'SHELL32_EncryptDirectory'				SHELL32_EncryptDirectory				shell32		411
imp	'SHELL32_EncryptedFileKeyInfo'				SHELL32_EncryptedFileKeyInfo				shell32		412
imp	'SHELL32_EnumCommonTasks'				SHELL32_EnumCommonTasks					shell32		413
imp	'SHELL32_FilePlaceholder_BindToPrimaryStream'		SHELL32_FilePlaceholder_BindToPrimaryStream		shell32		414
imp	'SHELL32_FilePlaceholder_CreateInstance'		SHELL32_FilePlaceholder_CreateInstance			shell32		415
imp	'SHELL32_FreeEncryptedFileKeyInfo'			SHELL32_FreeEncryptedFileKeyInfo			shell32		416
imp	'SHELL32_GenerateAppID'					SHELL32_GenerateAppID					shell32		417
imp	'SHELL32_GetAppIDRoot'					SHELL32_GetAppIDRoot					shell32		418
imp	'SHELL32_GetCommandProviderForFolderType'		SHELL32_GetCommandProviderForFolderType			shell32		419
imp	'SHELL32_GetDPIAdjustedLogicalSize'			SHELL32_GetDPIAdjustedLogicalSize			shell32		420
imp	'SHELL32_GetDiskCleanupPath'				SHELL32_GetDiskCleanupPath				shell32		421
imp	'SHELL32_GetFileNameFromBrowse'				SHELL32_GetFileNameFromBrowse				shell32		422
imp	'SHELL32_GetIconOverlayManager'				SHELL32_GetIconOverlayManager				shell32		423
imp	'SHELL32_GetLinkInfoData'				SHELL32_GetLinkInfoData					shell32		424
imp	'SHELL32_GetPlaceholderStatesFromFileAttributesAndReparsePointTag'	SHELL32_GetPlaceholderStatesFromFileAttributesAndReparsePointTag	shell32		425
imp	'SHELL32_GetRatingBucket'				SHELL32_GetRatingBucket					shell32		426
imp	'SHELL32_GetSkyDriveNetworkStates'			SHELL32_GetSkyDriveNetworkStates			shell32		427
imp	'SHELL32_GetSqmableFileName'				SHELL32_GetSqmableFileName				shell32		428
imp	'SHELL32_GetThumbnailAdornerFromFactory'		SHELL32_GetThumbnailAdornerFromFactory			shell32		429
imp	'SHELL32_GetThumbnailAdornerFromFactory2'		SHELL32_GetThumbnailAdornerFromFactory2			shell32		430
imp	'SHELL32_HandleUnrecognizedFileSystem'			SHELL32_HandleUnrecognizedFileSystem			shell32		431
imp	'SHELL32_IconCacheCreate'				SHELL32_IconCacheCreate					shell32		432
imp	'SHELL32_IconCacheDestroy'				SHELL32_IconCacheDestroy				shell32		433
imp	'SHELL32_IconCacheHandleAssociationChanged'		SHELL32_IconCacheHandleAssociationChanged		shell32		434
imp	'SHELL32_IconCacheRestore'				SHELL32_IconCacheRestore				shell32		435
imp	'SHELL32_IconCache_AboutToExtractIcons'			SHELL32_IconCache_AboutToExtractIcons			shell32		436
imp	'SHELL32_IconCache_DoneExtractingIcons'			SHELL32_IconCache_DoneExtractingIcons			shell32		437
imp	'SHELL32_IconCache_ExpandEnvAndSearchPath'		SHELL32_IconCache_ExpandEnvAndSearchPath		shell32		438
imp	'SHELL32_IconCache_RememberRecentlyExtractedIcons'	SHELL32_IconCache_RememberRecentlyExtractedIconsW	shell32		439
imp	'SHELL32_IconOverlayManagerInit'			SHELL32_IconOverlayManagerInit				shell32		440
imp	'SHELL32_IsGetKeyboardLayoutPresent'			SHELL32_IsGetKeyboardLayoutPresent			shell32		441
imp	'SHELL32_IsSystemUpgradeInProgress'			SHELL32_IsSystemUpgradeInProgress			shell32		442
imp	'SHELL32_IsValidLinkInfo'				SHELL32_IsValidLinkInfo					shell32		443
imp	'SHELL32_LegacyEnumSpecialTasksByType'			SHELL32_LegacyEnumSpecialTasksByType			shell32		444
imp	'SHELL32_LegacyEnumTasks'				SHELL32_LegacyEnumTasks					shell32		445
imp	'SHELL32_LookupBackIconIndex'				SHELL32_LookupBackIconIndex				shell32		446
imp	'SHELL32_LookupFrontIconIndex'				SHELL32_LookupFrontIconIndex				shell32		447
imp	'SHELL32_NormalizeRating'				SHELL32_NormalizeRating					shell32		448
imp	'SHELL32_NotifyLinkTrackingServiceOfMove'		SHELL32_NotifyLinkTrackingServiceOfMove			shell32		449
imp	'SHELL32_PifMgr_CloseProperties'			SHELL32_PifMgr_CloseProperties				shell32		450
imp	'SHELL32_PifMgr_GetProperties'				SHELL32_PifMgr_GetProperties				shell32		451
imp	'SHELL32_PifMgr_OpenProperties'				SHELL32_PifMgr_OpenProperties				shell32		452
imp	'SHELL32_PifMgr_SetProperties'				SHELL32_PifMgr_SetProperties				shell32		453
imp	'SHELL32_Printers_CreateBindInfo'			SHELL32_Printers_CreateBindInfo				shell32		454
imp	'SHELL32_Printjob_GetPidl'				SHELL32_Printjob_GetPidl				shell32		455
imp	'SHELL32_PurgeSystemIcon'				SHELL32_PurgeSystemIcon					shell32		456
imp	'SHELL32_RefreshOverlayImages'				SHELL32_RefreshOverlayImages				shell32		457
imp	'SHELL32_ResolveLinkInfo'				SHELL32_ResolveLinkInfoW				shell32		458
imp	'SHELL32_SHAddSparseIcon'				SHELL32_SHAddSparseIcon					shell32		459
imp	'SHELL32_SHCreateByValueOperationInterrupt'		SHELL32_SHCreateByValueOperationInterrupt		shell32		460
imp	'SHELL32_SHCreateDefaultContextMenu'			SHELL32_SHCreateDefaultContextMenu			shell32		461
imp	'SHELL32_SHCreateLocalServer'				SHELL32_SHCreateLocalServer				shell32		462
imp	'SHELL32_SHCreateShellFolderView'			SHELL32_SHCreateShellFolderView				shell32		463
imp	'SHELL32_SHDuplicateEncryptionInfoFile'			SHELL32_SHDuplicateEncryptionInfoFile			shell32		464
imp	'SHELL32_SHEncryptFile'					SHELL32_SHEncryptFile					shell32		465
imp	'SHELL32_SHFormatDriveAsync'				SHELL32_SHFormatDriveAsync				shell32		466
imp	'SHELL32_SHGetThreadUndoManager'			SHELL32_SHGetThreadUndoManager				shell32		467
imp	'SHELL32_SHGetUserName'					SHELL32_SHGetUserNameW					shell32		468
imp	'SHELL32_SHIsVirtualDevice'				SHELL32_SHIsVirtualDevice				shell32		469
imp	'SHELL32_SHLaunchPropSheet'				SHELL32_SHLaunchPropSheet				shell32		470
imp	'SHELL32_SHLogILFromFSIL'				SHELL32_SHLogILFromFSIL					shell32		471
imp	'SHELL32_SHOpenWithDialog'				SHELL32_SHOpenWithDialog				shell32		472
imp	'SHELL32_SHStartNetConnectionDialog'			SHELL32_SHStartNetConnectionDialogW			shell32		473
imp	'SHELL32_SHUICommandFromGUID'				SHELL32_SHUICommandFromGUID				shell32		474
imp	'SHELL32_SendToMenu_InvokeTargetedCommand'		SHELL32_SendToMenu_InvokeTargetedCommand		shell32		475
imp	'SHELL32_SendToMenu_VerifyTargetedCommand'		SHELL32_SendToMenu_VerifyTargetedCommand		shell32		476
imp	'SHELL32_SetPlaceholderReparsePointAttribute'		SHELL32_SetPlaceholderReparsePointAttribute		shell32		477
imp	'SHELL32_SetPlaceholderReparsePointAttribute2'		SHELL32_SetPlaceholderReparsePointAttribute2		shell32		478
imp	'SHELL32_ShowHideIconOnlyOnDesktop'			SHELL32_ShowHideIconOnlyOnDesktop			shell32		479
imp	'SHELL32_SimpleRatingToFilterCondition'			SHELL32_SimpleRatingToFilterCondition			shell32		480
imp	'SHELL32_StampIconForFile'				SHELL32_StampIconForFile				shell32		481
imp	'SHELL32_SuspendUndo'					SHELL32_SuspendUndo					shell32		482
imp	'SHELL32_TryVirtualDiscImageDriveEject'			SHELL32_TryVirtualDiscImageDriveEject			shell32		483
imp	'SHELL32_UpdateFilePlaceholderStates'			SHELL32_UpdateFilePlaceholderStates			shell32		484
imp	'SHELL32_VerifySaferTrust'				SHELL32_VerifySaferTrust				shell32		485
imp	'SHEmptyRecycleBinA'					SHEmptyRecycleBinA					shell32		486
imp	'SHEmptyRecycleBin'					SHEmptyRecycleBinW					shell32		487
imp	'SHEnableServiceObject'					SHEnableServiceObject					shell32		488
imp	'SHEnumerateUnreadMailAccounts'				SHEnumerateUnreadMailAccountsW				shell32		489
imp	'SHEvaluateSystemCommandTemplate'			SHEvaluateSystemCommandTemplate				shell32		490
imp	'SHExpandEnvironmentStringsA'				SHExpandEnvironmentStringsA				KernelBase	1421
imp	'SHExpandEnvironmentStrings'				SHExpandEnvironmentStringsW				KernelBase	1422
imp	'SHExtractIcons'					SHExtractIconsW						shell32		491
imp	'SHFileOperationA'					SHFileOperationA					shell32		493
imp	'SHFileOperation'					SHFileOperationW					shell32		494
imp	'SHFindFiles'						SHFindFiles						shell32		90
imp	'SHFind_InitMenuPopup'					SHFind_InitMenuPopup					shell32		149
imp	'SHFlushSFCache'					SHFlushSFCache						shell32		526
imp	'SHFormatDrive'						SHFormatDrive						shell32		495
imp	'SHFree'						SHFree							shell32		195
imp	'SHFreeNameMappings'					SHFreeNameMappings					shell32		496
imp	'SHGetAttributesFromDataObject'				SHGetAttributesFromDataObject				shell32		750
imp	'SHGetDataFromIDListA'					SHGetDataFromIDListA					shell32		497
imp	'SHGetDataFromIDList'					SHGetDataFromIDListW					shell32		498
imp	'SHGetDesktopFolder'					SHGetDesktopFolder					shell32		499
imp	'SHGetDiskFreeSpaceA'					SHGetDiskFreeSpaceA					shell32		500
imp	'SHGetDiskFreeSpaceExA'					SHGetDiskFreeSpaceExA					shell32		501
imp	'SHGetDiskFreeSpaceEx'					SHGetDiskFreeSpaceExW					shell32		502
imp	'SHGetDriveMedia'					SHGetDriveMedia						shell32		503
imp	'SHGetFileInfoA'					SHGetFileInfoA						shell32		505
imp	'SHGetFileInfo'						SHGetFileInfoW						shell32		506
imp	'SHGetFolderLocation'					SHGetFolderLocation					shell32		507
imp	'SHGetFolderPathA'					SHGetFolderPathA					shell32		508
imp	'SHGetFolderPathAndSubDirA'				SHGetFolderPathAndSubDirA				shell32		509
imp	'SHGetFolderPathAndSubDir'				SHGetFolderPathAndSubDirW				shell32		510
imp	'SHGetFolderPathEx'					SHGetFolderPathEx					shell32		511
imp	'SHGetFolderPath'					SHGetFolderPathW					shell32		512
imp	'SHGetIDListFromObject'					SHGetIDListFromObject					shell32		513
imp	'SHGetIconOverlayIndexA'				SHGetIconOverlayIndexA					shell32		514
imp	'SHGetIconOverlayIndex'					SHGetIconOverlayIndexW					shell32		515
imp	'SHGetImageList'					SHGetImageList						shell32		727
imp	'SHGetInstanceExplorer'					SHGetInstanceExplorer					shell32		516
imp	'SHGetItemFromDataObject'				SHGetItemFromDataObject					shell32		517
imp	'SHGetItemFromObject'					SHGetItemFromObject					shell32		518
imp	'SHGetKnownFolderIDList'				SHGetKnownFolderIDList					shell32		519
imp	'SHGetKnownFolderItem'					SHGetKnownFolderItem					shell32		527
imp	'SHGetKnownFolderPath'					SHGetKnownFolderPath					shell32		528
imp	'SHGetLocalizedName'					SHGetLocalizedName					shell32		529
imp	'SHGetMalloc'						SHGetMalloc						shell32		530
imp	'SHGetNameFromIDList'					SHGetNameFromIDList					shell32		531
imp	'SHGetNewLinkInfoA'					SHGetNewLinkInfoA					shell32		179
imp	'SHGetNewLinkInfo'					SHGetNewLinkInfoW					shell32		180
imp	'SHGetPathFromIDListA'					SHGetPathFromIDListA					shell32		534
imp	'SHGetPathFromIDListEx'					SHGetPathFromIDListEx					shell32		535
imp	'SHGetPathFromIDList'					SHGetPathFromIDListW					shell32		536
imp	'SHGetPropertyStoreForWindow'				SHGetPropertyStoreForWindow				shell32		537
imp	'SHGetPropertyStoreFromIDList'				SHGetPropertyStoreFromIDList				shell32		538
imp	'SHGetPropertyStoreFromParsingName'			SHGetPropertyStoreFromParsingName			shell32		539
imp	'SHGetRealIDL'						SHGetRealIDL						shell32		98
imp	'SHGetSetFolderCustomSettings'				SHGetSetFolderCustomSettings				shell32		709
imp	'SHGetSetSettings'					SHGetSetSettings					shell32		68
imp	'SHGetSettings'						SHGetSettings						shell32		540
imp	'SHGetSpecialFolderLocation'				SHGetSpecialFolderLocation				shell32		541
imp	'SHGetSpecialFolderPathA'				SHGetSpecialFolderPathA					shell32		542
imp	'SHGetSpecialFolderPath'				SHGetSpecialFolderPathW					shell32		543
imp	'SHGetStockIconInfo'					SHGetStockIconInfo					shell32		544
imp	'SHGetTemporaryPropertyForItem'				SHGetTemporaryPropertyForItem				shell32		545
imp	'SHGetUnreadMailCount'					SHGetUnreadMailCountW					shell32		546
imp	'SHHandleUpdateImage'					SHHandleUpdateImage					shell32		193
imp	'SHHelpShortcuts_RunDLL'				SHHelpShortcuts_RunDLL					shell32		228
imp	'SHHelpShortcuts_RunDLLA'				SHHelpShortcuts_RunDLLA					shell32		229
imp	'SHHelpShortcuts_RunDLLW'				SHHelpShortcuts_RunDLLW					shell32		238
imp	'SHILCreateFromPath'					SHILCreateFromPath					shell32		28
imp	'SHInvokePrinterCommandA'				SHInvokePrinterCommandA					shell32		547
imp	'SHInvokePrinterCommand'				SHInvokePrinterCommandW					shell32		548
imp	'SHIsFileAvailableOffline'				SHIsFileAvailableOffline				shell32		549
imp	'SHLimitInputEdit'					SHLimitInputEdit					shell32		747
imp	'SHLoadInProc'						SHLoadInProc						shell32		550
imp	'SHLoadIndirectString'					SHLoadIndirectString					KernelBase	1423
imp	'SHLoadIndirectStringInternal'				SHLoadIndirectStringInternal				KernelBase	1424
imp	'SHLoadNonloadedIconOverlayIdentifiers'			SHLoadNonloadedIconOverlayIdentifiers			shell32		551
imp	'SHMapPIDLToSystemImageListIndex'			SHMapPIDLToSystemImageListIndex				shell32		77
imp	'SHMultiFileProperties'					SHMultiFileProperties					shell32		716
imp	'SHObjectProperties'					SHObjectProperties					shell32		178
imp	'SHOpenFolderAndSelectItems'				SHOpenFolderAndSelectItems				shell32		552
imp	'SHOpenPropSheet'					SHOpenPropSheetW					shell32		80
imp	'SHOpenWithDialog'					SHOpenWithDialog					shell32		553
imp	'SHParseDisplayName'					SHParseDisplayName					shell32		554
imp	'SHPathPrepareForWriteA'				SHPathPrepareForWriteA					shell32		555
imp	'SHPathPrepareForWrite'					SHPathPrepareForWriteW					shell32		556
imp	'SHPropStgCreate'					SHPropStgCreate						shell32		685
imp	'SHPropStgReadMultiple'					SHPropStgReadMultiple					shell32		688
imp	'SHPropStgWriteMultiple'				SHPropStgWriteMultiple					shell32		689
imp	'SHQueryRecycleBinA'					SHQueryRecycleBinA					shell32		557
imp	'SHQueryRecycleBin'					SHQueryRecycleBinW					shell32		558
imp	'SHQueryUserNotificationState'				SHQueryUserNotificationState				shell32		559
imp	'SHRegCloseUSKey'					SHRegCloseUSKey						KernelBase	1425
imp	'SHRegCreateUSKeyA'					SHRegCreateUSKeyA					KernelBase	1426
imp	'SHRegCreateUSKey'					SHRegCreateUSKeyW					KernelBase	1427
imp	'SHRegDeleteEmptyUSKeyA'				SHRegDeleteEmptyUSKeyA					KernelBase	1428
imp	'SHRegDeleteEmptyUSKey'					SHRegDeleteEmptyUSKeyW					KernelBase	1429
imp	'SHRegDeleteUSValueA'					SHRegDeleteUSValueA					KernelBase	1430
imp	'SHRegDeleteUSValue'					SHRegDeleteUSValueW					KernelBase	1431
imp	'SHRegEnumUSKeyA'					SHRegEnumUSKeyA						KernelBase	1432
imp	'SHRegEnumUSKey'					SHRegEnumUSKeyW						KernelBase	1433
imp	'SHRegEnumUSValueA'					SHRegEnumUSValueA					KernelBase	1434
imp	'SHRegEnumUSValue'					SHRegEnumUSValueW					KernelBase	1435
imp	'SHRegGetBoolUSValueA'					SHRegGetBoolUSValueA					KernelBase	1436
imp	'SHRegGetBoolUSValue'					SHRegGetBoolUSValueW					KernelBase	1437
imp	'SHRegGetUSValueA'					SHRegGetUSValueA					KernelBase	1438
imp	'SHRegGetUSValue'					SHRegGetUSValueW					KernelBase	1439
imp	'SHRegOpenUSKeyA'					SHRegOpenUSKeyA						KernelBase	1440
imp	'SHRegOpenUSKey'					SHRegOpenUSKeyW						KernelBase	1441
imp	'SHRegQueryInfoUSKeyA'					SHRegQueryInfoUSKeyA					KernelBase	1442
imp	'SHRegQueryInfoUSKey'					SHRegQueryInfoUSKeyW					KernelBase	1443
imp	'SHRegQueryUSValueA'					SHRegQueryUSValueA					KernelBase	1444
imp	'SHRegQueryUSValue'					SHRegQueryUSValueW					KernelBase	1445
imp	'SHRegSetUSValueA'					SHRegSetUSValueA					KernelBase	1446
imp	'SHRegSetUSValue'					SHRegSetUSValueW					KernelBase	1447
imp	'SHRegWriteUSValueA'					SHRegWriteUSValueA					KernelBase	1448
imp	'SHRegWriteUSValue'					SHRegWriteUSValueW					KernelBase	1449
imp	'SHRemoveLocalizedName'					SHRemoveLocalizedName					shell32		560
imp	'SHReplaceFromPropSheetExtArray'			SHReplaceFromPropSheetExtArray				shell32		170
imp	'SHResolveLibrary'					SHResolveLibrary					shell32		561
imp	'SHRestricted'						SHRestricted						shell32		100
imp	'SHSetDefaultProperties'				SHSetDefaultProperties					shell32		562
imp	'SHSetFolderPathA'					SHSetFolderPathA					shell32		231
imp	'SHSetFolderPath'					SHSetFolderPathW					shell32		232
imp	'SHSetInstanceExplorer'					SHSetInstanceExplorer					shell32		176
imp	'SHSetKnownFolderPath'					SHSetKnownFolderPath					shell32		563
imp	'SHSetLocalizedName'					SHSetLocalizedName					shell32		564
imp	'SHSetTemporaryPropertyForItem'				SHSetTemporaryPropertyForItem				shell32		565
imp	'SHSetUnreadMailCount'					SHSetUnreadMailCountW					shell32		566
imp	'SHShellFolderView_Message'				SHShellFolderView_Message				shell32		73
imp	'SHShowManageLibraryUI'					SHShowManageLibraryUI					shell32		567
imp	'SHSimpleIDListFromPath'				SHSimpleIDListFromPath					shell32		162
imp	'SHStartNetConnectionDialog'				SHStartNetConnectionDialogW				shell32		14
imp	'SHTestTokenMembership'					SHTestTokenMembership					shell32		245
imp	'SHTruncateString'					SHTruncateString					KernelBase	1450
imp	'SHUpdateImageA'					SHUpdateImageA						shell32		191
imp	'SHUpdateImage'						SHUpdateImageW						shell32		192
imp	'SHUpdateRecycleBinIcon'				SHUpdateRecycleBinIcon					shell32		568
imp	'SHValidateUNC'						SHValidateUNC						shell32		173
imp	'STROBJ_bEnum'						STROBJ_bEnum						gdi32		1810
imp	'STROBJ_bEnumPositionsOnly'				STROBJ_bEnumPositionsOnly				gdi32		1811
imp	'STROBJ_bGetAdvanceWidths'				STROBJ_bGetAdvanceWidths				gdi32		1812
imp	'STROBJ_dwGetCodePage'					STROBJ_dwGetCodePage					gdi32		1813
imp	'STROBJ_vEnumStart'					STROBJ_vEnumStart					gdi32		1814
imp	'SafeBaseRegGetKeySecurity'				SafeBaseRegGetKeySecurity				advapi32	1707
imp	'SaferCloseLevel'					SaferCloseLevel						advapi32	1708
imp	'SaferComputeTokenFromLevel'				SaferComputeTokenFromLevel				advapi32	1709
imp	'SaferCreateLevel'					SaferCreateLevel					advapi32	1710
imp	'SaferGetLevelInformation'				SaferGetLevelInformation				advapi32	1711
imp	'SaferGetPolicyInformation'				SaferGetPolicyInformation				advapi32	1712
imp	'SaferIdentifyLevel'					SaferIdentifyLevel					advapi32	1713
imp	'SaferRecordEventLogEntry'				SaferRecordEventLogEntry				advapi32	1714
imp	'SaferSetLevelInformation'				SaferSetLevelInformation				advapi32	1715
imp	'SaferSetPolicyInformation'				SaferSetPolicyInformation				advapi32	1716
imp	'SaferiChangeRegistryScope'				SaferiChangeRegistryScope				advapi32	1717
imp	'SaferiCompareTokenLevels'				SaferiCompareTokenLevels				advapi32	1718
imp	'SaferiIsDllAllowed'					SaferiIsDllAllowed					advapi32	1719
imp	'SaferiIsExecutableFileType'				SaferiIsExecutableFileType				advapi32	1720
imp	'SaferiPopulateDefaultsInRegistry'			SaferiPopulateDefaultsInRegistry			advapi32	1721
imp	'SaferiRecordEventLogEntry'				SaferiRecordEventLogEntry				advapi32	1722
imp	'SaferiSearchMatchingHashRules'				SaferiSearchMatchingHashRules				advapi32	1723
imp	'SaveAlternatePackageRootPath'				SaveAlternatePackageRootPath				KernelBase	1451
imp	'SaveDC'						SaveDC							gdi32		1815	1
imp	'SaveStateRootFolderPath'				SaveStateRootFolderPath					KernelBase	1452
imp	'SbExecuteProcedure'					SbExecuteProcedure					ntdll		1610
imp	'SbSelectProcedure'					SbSelectProcedure					ntdll		1611
imp	'ScaleRgn'						ScaleRgn						gdi32		1816
imp	'ScaleValues'						ScaleValues						gdi32		1817
imp	'ScaleViewportExtEx'					ScaleViewportExtEx					gdi32		1818
imp	'ScaleWindowExtEx'					ScaleWindowExtEx					gdi32		1819
imp	'ScreenToClient'					ScreenToClient						user32		2291
imp	'ScrollChildren'					ScrollChildren						user32		2292
imp	'ScrollConsoleScreenBufferA'				ScrollConsoleScreenBufferA				kernel32	0		# KernelBase
imp	'ScrollConsoleScreenBuffer'				ScrollConsoleScreenBufferW				kernel32	0		# KernelBase
imp	'ScrollDC'						ScrollDC						user32		2293
imp	'ScrollWindow'						ScrollWindow						user32		2294
imp	'ScrollWindowEx'					ScrollWindowEx						user32		2295
imp	'SearchPathA'						SearchPathA						kernel32	0		# KernelBase
imp	'SearchPath'						SearchPathW						kernel32	0		# KernelBase
imp	'SelectBrushLocal'					SelectBrushLocal					gdi32		1860
imp	'SelectClipPath'					SelectClipPath						gdi32		1861
imp	'SelectClipRgn'						SelectClipRgn						gdi32		1862
imp	'SelectFontLocal'					SelectFontLocal						gdi32		1863
imp	'SelectObject'						SelectObject						gdi32		1864	2
imp	'SelectPalette'						SelectPalette						gdi32		1865
imp	'SendDlgItemMessageA'					SendDlgItemMessageA					user32		2296
imp	'SendDlgItemMessage'					SendDlgItemMessageW					user32		2297
imp	'SendIMEMessageExA'					SendIMEMessageExA					user32		2298
imp	'SendIMEMessageEx'					SendIMEMessageExW					user32		2299
imp	'SendInput'						SendInput						user32		2300
imp	'SendMessageA'						SendMessageA						user32		2301
imp	'SendMessageCallbackA'					SendMessageCallbackA					user32		2302
imp	'SendMessageCallback'					SendMessageCallbackW					user32		2303
imp	'SendMessageTimeoutA'					SendMessageTimeoutA					user32		2304
imp	'SendMessageTimeout'					SendMessageTimeoutW					user32		2305
imp	'SendMessage'						SendMessageW						user32		2306	4
imp	'SendNotifyMessageA'					SendNotifyMessageA					user32		2307
imp	'SendNotifyMessage'					SendNotifyMessageW					user32		2308
imp	'SetAbortProc'						SetAbortProc						gdi32		1866
imp	'SetAclInformation'					SetAclInformation					advapi32	0		# KernelBase
imp	'SetActiveWindow'					SetActiveWindow						user32		2309
imp	'SetAddrInfoExA'					SetAddrInfoExA						ws2_32		37
imp	'SetAddrInfoEx'						SetAddrInfoExW						ws2_32		38
imp	'SetArcDirection'					SetArcDirection						gdi32		1867
imp	'SetBitmapAttributes'					SetBitmapAttributes					gdi32		1868
imp	'SetBitmapBits'						SetBitmapBits						gdi32		1869
imp	'SetBitmapDimensionEx'					SetBitmapDimensionEx					gdi32		1870
imp	'SetBkColor'						SetBkColor						gdi32		1871
imp	'SetBkMode'						SetBkMode						gdi32		1872	2
imp	'SetBoundsRect'						SetBoundsRect						gdi32		1873
imp	'SetBrushAttributes'					SetBrushAttributes					gdi32		1874
imp	'SetBrushOrgEx'						SetBrushOrgEx						gdi32		1875
imp	'SetCachedSigningLevel'					SetCachedSigningLevel					KernelBase	1458
imp	'SetCalendarInfoA'					SetCalendarInfoA					kernel32	1249
imp	'SetCalendarInfo'					SetCalendarInfoW					kernel32	0		# KernelBase
imp	'SetCapture'						SetCapture						user32		2310	1
imp	'SetCaretBlinkTime'					SetCaretBlinkTime					user32		2311
imp	'SetCaretPos'						SetCaretPos						user32		2312
imp	'SetClassLongA'						SetClassLongA						user32		2313	3
imp	'SetClassLongPtrA'					SetClassLongPtrA					user32		2314
imp	'SetClassLongPtr'					SetClassLongPtrW					user32		2315
imp	'SetClassLong'						SetClassLongW						user32		2316	3
imp	'SetClassWord'						SetClassWord						user32		2317
imp	'SetClientDynamicTimeZoneInformation'			SetClientDynamicTimeZoneInformation			KernelBase	1460
imp	'SetClientTimeZoneInformation'				SetClientTimeZoneInformation				KernelBase	1461
imp	'SetClipboardData'					SetClipboardData					user32		2318
imp	'SetClipboardViewer'					SetClipboardViewer					user32		2319
imp	'SetCoalescableTimer'					SetCoalescableTimer					user32		2320
imp	'SetColorAdjustment'					SetColorAdjustment					gdi32		1876
imp	'SetColorSpace'						SetColorSpace						gdi32		1877
imp	'SetComPlusPackageInstallStatus'			SetComPlusPackageInstallStatus				kernel32	1251
imp	'SetCommBreak'						SetCommBreak						kernel32	0		# KernelBase
imp	'SetCommConfig'						SetCommConfig						kernel32	0		# KernelBase
imp	'SetCommMask'						SetCommMask						kernel32	0		# KernelBase
imp	'SetCommState'						SetCommState						kernel32	0		# KernelBase
imp	'SetCommTimeouts'					SetCommTimeouts						kernel32	0		# KernelBase
imp	'SetComputerNameA'					SetComputerNameA					kernel32	0		# KernelBase
imp	'SetComputerNameEx2W'					SetComputerNameEx2W					KernelBase	1468
imp	'SetComputerNameExA'					SetComputerNameExA					kernel32	0		# KernelBase
imp	'SetComputerNameEx'					SetComputerNameExW					kernel32	0		# KernelBase
imp	'SetComputerName'					SetComputerNameW					kernel32	0		# KernelBase
imp	'SetConsoleActiveScreenBuffer'				SetConsoleActiveScreenBuffer				kernel32	0	1	# TODO(jart): 6.2 and higher	# KernelBase
imp	'SetConsoleCP'						SetConsoleCP						kernel32	0	1	# TODO(jart): 6.2 and higher	# KernelBase
imp	'SetConsoleCtrlHandler'					SetConsoleCtrlHandler					kernel32	0	2	# KernelBase
imp	'SetConsoleCursor'					SetConsoleCursor					kernel32	1265
imp	'SetConsoleCursorInfo'					SetConsoleCursorInfo					kernel32	0	2	# KernelBase
imp	'SetConsoleCursorMode'					SetConsoleCursorMode					kernel32	1267
imp	'SetConsoleCursorPosition'				SetConsoleCursorPosition				kernel32	0	2	# KernelBase
imp	'SetConsoleDisplayMode'					SetConsoleDisplayMode					kernel32	0		# KernelBase
imp	'SetConsoleFont'					SetConsoleFont						kernel32	1270
imp	'SetConsoleHardwareState'				SetConsoleHardwareState					kernel32	1271
imp	'SetConsoleHistoryInfo'					SetConsoleHistoryInfo					kernel32	0		# KernelBase
imp	'SetConsoleIcon'					SetConsoleIcon						kernel32	1273
imp	'SetConsoleInputExeNameA'				SetConsoleInputExeNameA					KernelBase	1479
imp	'SetConsoleInputExeName'				SetConsoleInputExeNameW					KernelBase	1480
imp	'SetConsoleKeyShortcuts'				SetConsoleKeyShortcuts					kernel32	1276
imp	'SetConsoleLocalEUDC'					SetConsoleLocalEUDC					kernel32	1277
imp	'SetConsoleMaximumWindowSize'				SetConsoleMaximumWindowSize				kernel32	1278
imp	'SetConsoleMenuClose'					SetConsoleMenuClose					kernel32	1279
imp	'SetConsoleMode'					SetConsoleMode						kernel32	0	2	# KernelBase
imp	'SetConsoleNlsMode'					SetConsoleNlsMode					kernel32	1281
imp	'SetConsoleNumberOfCommandsA'				SetConsoleNumberOfCommandsA				KernelBase	1482
imp	'SetConsoleNumberOfCommands'				SetConsoleNumberOfCommandsW				KernelBase	1483
imp	'SetConsoleOS2OemFormat'				SetConsoleOS2OemFormat					kernel32	1284
imp	'SetConsoleOutputCP'					SetConsoleOutputCP					kernel32	0	1	# KernelBase
imp	'SetConsolePalette'					SetConsolePalette					kernel32	1286
imp	'SetConsoleScreenBufferInfoEx'				SetConsoleScreenBufferInfoEx				kernel32	0	2	# KernelBase
imp	'SetConsoleScreenBufferSize'				SetConsoleScreenBufferSize				kernel32	0	2	# KernelBase
imp	'SetConsoleTextAttribute'				SetConsoleTextAttribute					kernel32	0		# KernelBase
imp	'SetConsoleTitle'					SetConsoleTitleW					kernel32	0	1	# KernelBase
imp	'SetConsoleTitleA'					SetConsoleTitleA					kernel32	0	1	# KernelBase
imp	'SetConsoleWindowInfo'					SetConsoleWindowInfo					kernel32	0	3	# KernelBase
imp	'SetCoreWindow'						SetCoreWindow						user32		2571
imp	'SetCurrentConsoleFontEx'				SetCurrentConsoleFontEx					kernel32	0		# KernelBase
imp	'SetCurrentDirectory'					SetCurrentDirectoryW					kernel32	0	1	# KernelBase
imp	'SetCurrentDirectoryA'					SetCurrentDirectoryA					kernel32	0	1	# KernelBase
imp	'SetCurrentProcessExplicitAppUserModelID'		SetCurrentProcessExplicitAppUserModelID			shell32		569
imp	'SetCursor'						SetCursor						user32		2321	1
imp	'SetCursorContents'					SetCursorContents					user32		2322
imp	'SetCursorPos'						SetCursorPos						user32		2323
imp	'SetDCBrushColor'					SetDCBrushColor						gdi32		1878
imp	'SetDCDpiScaleValue'					SetDCDpiScaleValue					gdi32		1879
imp	'SetDCPenColor'						SetDCPenColor						gdi32		1880
imp	'SetDIBColorTable'					SetDIBColorTable					gdi32		1881
imp	'SetDIBits'						SetDIBits						gdi32		1882
imp	'SetDIBitsToDevice'					SetDIBitsToDevice					gdi32		1883
imp	'SetDebugErrorLevel'					SetDebugErrorLevel					user32		2324
imp	'SetDefaultCommConfigA'					SetDefaultCommConfigA					kernel32	1297
imp	'SetDefaultCommConfig'					SetDefaultCommConfigW					kernel32	1298
imp	'SetDefaultDllDirectories'				SetDefaultDllDirectories				kernel32	0	1	# KernelBase
imp	'SetDeskWallpaper'					SetDeskWallpaper					user32		2325
imp	'SetDesktopColorTransform'				SetDesktopColorTransform				user32		2326
imp	'SetDeviceGammaRamp'					SetDeviceGammaRamp					gdi32		1884
imp	'SetDialogControlDpiChangeBehavior'			SetDialogControlDpiChangeBehavior			user32		2327
imp	'SetDialogDpiChangeBehavior'				SetDialogDpiChangeBehavior				user32		2328
imp	'SetDisplayAutoRotationPreferences'			SetDisplayAutoRotationPreferences			user32		2329
imp	'SetDisplayConfig'					SetDisplayConfig					user32		2330
imp	'SetDlgItemInt'						SetDlgItemInt						user32		2331
imp	'SetDlgItemTextA'					SetDlgItemTextA						user32		2332
imp	'SetDlgItemText'					SetDlgItemTextW						user32		2333
imp	'SetDllDirectoryA'					SetDllDirectoryA					kernel32	1300
imp	'SetDllDirectory'					SetDllDirectoryW					kernel32	1301
imp	'SetDoubleClickTime'					SetDoubleClickTime					user32		2334
imp	'SetDynamicTimeZoneInformation'				SetDynamicTimeZoneInformation				kernel32	0		# KernelBase
imp	'SetEncryptedFileMetadata'				SetEncryptedFileMetadata				advapi32	1725
imp	'SetEndOfFile'						SetEndOfFile						kernel32	0	1	# KernelBase
imp	'SetEnhMetaFileBits'					SetEnhMetaFileBits					gdi32		1885
imp	'SetEntriesInAccessListA'				SetEntriesInAccessListA					advapi32	1726
imp	'SetEntriesInAccessList'				SetEntriesInAccessListW					advapi32	1727
imp	'SetEntriesInAclA'					SetEntriesInAclA					advapi32	1728
imp	'SetEntriesInAcl'					SetEntriesInAclW					advapi32	1729
imp	'SetEntriesInAuditListA'				SetEntriesInAuditListA					advapi32	1730
imp	'SetEntriesInAuditList'					SetEntriesInAuditListW					advapi32	1731
imp	'SetEnvironmentStrings'					SetEnvironmentStringsW					KernelBase	1498	1
imp	'SetEnvironmentStringsA'				SetEnvironmentStringsA					kernel32	1304	1
imp	'SetEnvironmentVariable'				SetEnvironmentVariableW					kernel32	0	2	# KernelBase
imp	'SetEnvironmentVariableA'				SetEnvironmentVariableA					kernel32	0	2	# KernelBase
imp	'SetErrorMode'						SetErrorMode						kernel32	0	1	# KernelBase
imp	'SetEvent'						SetEvent						kernel32	0	1	# KernelBase
imp	'SetExtensionProperty'					SetExtensionProperty					KernelBase	1504
imp	'SetFeatureReportResponse'				SetFeatureReportResponse				user32		2335
imp	'SetFileApisToANSI'					SetFileApisToANSI					kernel32	0		# KernelBase
imp	'SetFileApisToOEM'					SetFileApisToOEM					kernel32	0		# KernelBase
imp	'SetFileAttributes'					SetFileAttributesW					kernel32	0	2	# KernelBase
imp	'SetFileAttributesA'					SetFileAttributesA					kernel32	0	2	# KernelBase
imp	'SetFileAttributesTransactedA'				SetFileAttributesTransactedA				kernel32	1314
imp	'SetFileAttributesTransacted'				SetFileAttributesTransactedW				kernel32	1315
imp	'SetFileBandwidthReservation'				SetFileBandwidthReservation				kernel32	1317
imp	'SetFileCompletionNotificationModes'			SetFileCompletionNotificationModes			kernel32	1318	2
imp	'SetFileInformationByHandle'				SetFileInformationByHandle				kernel32	0		# KernelBase
imp	'SetFileIoOverlappedRange'				SetFileIoOverlappedRange				kernel32	0		# KernelBase
imp	'SetFilePointer'					SetFilePointer						kernel32	0	4	# KernelBase
imp	'SetFilePointerEx'					SetFilePointerEx					kernel32	0	4	# KernelBase
imp	'SetFileSecurityA'					SetFileSecurityA					advapi32	1732
imp	'SetFileSecurity'					SetFileSecurityW					advapi32	0		# KernelBase
imp	'SetFileShortNameA'					SetFileShortNameA					kernel32	1323
imp	'SetFileShortName'					SetFileShortNameW					kernel32	1324
imp	'SetFileTime'						SetFileTime						kernel32	0	4	# KernelBase
imp	'SetFileValidData'					SetFileValidData					kernel32	0	2	# KernelBase
imp	'SetFirmwareEnvironmentVariableA'			SetFirmwareEnvironmentVariableA				kernel32	1327
imp	'SetFirmwareEnvironmentVariableExA'			SetFirmwareEnvironmentVariableExA			kernel32	1328
imp	'SetFirmwareEnvironmentVariableEx'			SetFirmwareEnvironmentVariableExW			kernel32	1329
imp	'SetFirmwareEnvironmentVariable'			SetFirmwareEnvironmentVariableW				kernel32	1330
imp	'SetFocus'						SetFocus						user32		2336
imp	'SetFontEnumeration'					SetFontEnumeration					gdi32		1886
imp	'SetForegroundWindow'					SetForegroundWindow					user32		2337
imp	'SetGestureConfig'					SetGestureConfig					user32		2338
imp	'SetGraphicsMode'					SetGraphicsMode						gdi32		1887
imp	'SetHandleCount'					SetHandleCount						kernel32	0	1	# KernelBase
imp	'SetHandleInformation'					SetHandleInformation					kernel32	0	3	# KernelBase
imp	'SetICMMode'						SetICMMode						gdi32		1888
imp	'SetICMProfileA'					SetICMProfileA						gdi32		1889
imp	'SetICMProfile'						SetICMProfileW						gdi32		1890
imp	'SetInformationCodeAuthzLevel'				SetInformationCodeAuthzLevelW				advapi32	1734
imp	'SetInformationCodeAuthzPolicy'				SetInformationCodeAuthzPolicyW				advapi32	1735
imp	'SetInformationJobObject'				SetInformationJobObject					kernel32	1333
imp	'SetInternalWindowPos'					SetInternalWindowPos					user32		2339
imp	'SetIoRateControlInformationJobObject'			SetIoRateControlInformationJobObject			kernel32	1334
imp	'SetIsDeveloperModeEnabled'				SetIsDeveloperModeEnabled				KernelBase	1518
imp	'SetIsSideloadingEnabled'				SetIsSideloadingEnabled					KernelBase	1519
imp	'SetKernelObjectSecurity'				SetKernelObjectSecurity					advapi32	0		# KernelBase
imp	'SetKeyboardState'					SetKeyboardState					user32		2340
imp	'SetLastConsoleEventActive'				SetLastConsoleEventActive				KernelBase	1521
imp	'SetLastError'						SetLastError						kernel32	1336	1
imp	'SetLastErrorEx'					SetLastErrorEx						user32		2341
imp	'SetLayeredWindowAttributes'				SetLayeredWindowAttributes				user32		2342
imp	'SetLayout'						SetLayout						gdi32		1891
imp	'SetLayoutWidth'					SetLayoutWidth						gdi32		1892
imp	'SetLocalPrimaryComputerNameA'				SetLocalPrimaryComputerNameA				kernel32	1337
imp	'SetLocalPrimaryComputerName'				SetLocalPrimaryComputerNameW				kernel32	1338
imp	'SetLocalTime'						SetLocalTime						kernel32	0		# KernelBase
imp	'SetLocaleInfoA'					SetLocaleInfoA						kernel32	1340
imp	'SetLocaleInfo'						SetLocaleInfoW						kernel32	0		# KernelBase
imp	'SetMagicColors'					SetMagicColors						gdi32		1893
imp	'SetMagnificationDesktopColorEffect'			SetMagnificationDesktopColorEffect			user32		2343
imp	'SetMagnificationDesktopMagnification'			SetMagnificationDesktopMagnification			user32		2344
imp	'SetMagnificationDesktopSamplingMode'			SetMagnificationDesktopSamplingMode			user32		2345
imp	'SetMagnificationLensCtxInformation'			SetMagnificationLensCtxInformation			user32		2346
imp	'SetMailslotInfo'					SetMailslotInfo						kernel32	1342
imp	'SetMapMode'						SetMapMode						gdi32		1894
imp	'SetMapperFlags'					SetMapperFlags						gdi32		1895
imp	'SetMenu'						SetMenu							user32		2347
imp	'SetMenuContextHelpId'					SetMenuContextHelpId					user32		2348
imp	'SetMenuDefaultItem'					SetMenuDefaultItem					user32		2349
imp	'SetMenuInfo'						SetMenuInfo						user32		2350
imp	'SetMenuItemBitmaps'					SetMenuItemBitmaps					user32		2351
imp	'SetMenuItemInfoA'					SetMenuItemInfoA					user32		2352
imp	'SetMenuItemInfo'					SetMenuItemInfoW					user32		2353
imp	'SetMessageExtraInfo'					SetMessageExtraInfo					user32		2354
imp	'SetMessageQueue'					SetMessageQueue						user32		2355
imp	'SetMessageWaitingIndicator'				SetMessageWaitingIndicator				kernel32	1343
imp	'SetMetaFileBitsEx'					SetMetaFileBitsEx					gdi32		1896
imp	'SetMetaRgn'						SetMetaRgn						gdi32		1897
imp	'SetMirrorRendering'					SetMirrorRendering					user32		2356
imp	'SetMiterLimit'						SetMiterLimit						gdi32		1898
imp	'SetNamedPipeAttribute'					SetNamedPipeAttribute					kernel32	1344
imp	'SetNamedPipeHandleState'				SetNamedPipeHandleState					kernel32	0	4	# KernelBase
imp	'SetNamedSecurityInfoA'					SetNamedSecurityInfoA					advapi32	1737
imp	'SetNamedSecurityInfoExA'				SetNamedSecurityInfoExA					advapi32	1738
imp	'SetNamedSecurityInfoEx'				SetNamedSecurityInfoExW					advapi32	1739
imp	'SetNamedSecurityInfo'					SetNamedSecurityInfoW					advapi32	1740
imp	'SetOPMSigningKeyAndSequenceNumbers'			SetOPMSigningKeyAndSequenceNumbers			gdi32		1899
imp	'SetPaletteEntries'					SetPaletteEntries					gdi32		1900
imp	'SetParent'						SetParent						user32		2357	2
imp	'SetPhysicalCursorPos'					SetPhysicalCursorPos					user32		2358
imp	'SetPixel'						SetPixel						gdi32		1901	4
imp	'SetPixelFormat'					SetPixelFormat						gdi32		1902
imp	'SetPixelV'						SetPixelV						gdi32		1903
imp	'SetPolyFillMode'					SetPolyFillMode						gdi32		1904
imp	'SetPriorityClass'					SetPriorityClass					kernel32	0	2	# KernelBase
imp	'SetPrivateObjectSecurity'				SetPrivateObjectSecurity				advapi32	0		# KernelBase
imp	'SetPrivateObjectSecurityEx'				SetPrivateObjectSecurityEx				advapi32	0		# KernelBase
imp	'SetProcessAffinityMask'				SetProcessAffinityMask					kernel32	1347	2
imp	'SetProcessAffinityUpdateMode'				SetProcessAffinityUpdateMode				kernel32	0		# KernelBase
imp	'SetProcessDEPPolicy'					SetProcessDEPPolicy					kernel32	1349
imp	'SetProcessDPIAware'					SetProcessDPIAware					user32		2359
imp	'SetProcessDefaultCpuSets'				SetProcessDefaultCpuSets				kernel32	0		# KernelBase
imp	'SetProcessDefaultLayout'				SetProcessDefaultLayout					user32		2360
imp	'SetProcessDpiAwarenessContext'				SetProcessDpiAwarenessContext				user32		2361
imp	'SetProcessDpiAwarenessInternal'			SetProcessDpiAwarenessInternal				user32		2362
imp	'SetProcessGroupAffinity'				SetProcessGroupAffinity					KernelBase	1531
imp	'SetProcessInformation'					SetProcessInformation					kernel32	0		# KernelBase
imp	'SetProcessMitigationPolicy'				SetProcessMitigationPolicy				kernel32	0		# KernelBase
imp	'SetProcessPreferredUILanguages'			SetProcessPreferredUILanguages				kernel32	0		# KernelBase
imp	'SetProcessPriorityBoost'				SetProcessPriorityBoost					kernel32	0	2	# KernelBase
imp	'SetProcessRestrictionExemption'			SetProcessRestrictionExemption				user32		2363
imp	'SetProcessShutdownParameters'				SetProcessShutdownParameters				kernel32	0		# KernelBase
imp	'SetProcessValidCallTargets'				SetProcessValidCallTargets				KernelBase	1537
imp	'SetProcessWindowStation'				SetProcessWindowStation					user32		2364
imp	'SetProcessWorkingSetSize'				SetProcessWorkingSetSize				kernel32	1356	3
imp	'SetProcessWorkingSetSizeEx'				SetProcessWorkingSetSizeEx				kernel32	0	4	# KernelBase
imp	'SetProgmanWindow'					SetProgmanWindow					user32		2365
imp	'SetPropA'						SetPropA						user32		2366
imp	'SetProp'						SetPropW						user32		2367
imp	'SetProtectedPolicy'					SetProtectedPolicy					kernel32	0		# KernelBase
imp	'SetProtocolProperty'					SetProtocolProperty					KernelBase	1540
imp	'SetROP2'						SetROP2							gdi32		1905
imp	'SetRect'						SetRect							user32		2368
imp	'SetRectEmpty'						SetRectEmpty						user32		2369
imp	'SetRectRgn'						SetRectRgn						gdi32		1906
imp	'SetRelAbs'						SetRelAbs						gdi32		1907
imp	'SetRoamingLastObservedChangeTime'			SetRoamingLastObservedChangeTime			KernelBase	1541
imp	'SetScrollInfo'						SetScrollInfo						user32		2370
imp	'SetScrollPos'						SetScrollPos						user32		2371
imp	'SetScrollRange'					SetScrollRange						user32		2372
imp	'SetSearchPathMode'					SetSearchPathMode					kernel32	1359
imp	'SetSecurityAccessMask'					SetSecurityAccessMask					advapi32	0		# KernelBase
imp	'SetSecurityDescriptorControl'				SetSecurityDescriptorControl				advapi32	0		# KernelBase
imp	'SetSecurityDescriptorDacl'				SetSecurityDescriptorDacl				advapi32	0		# KernelBase
imp	'SetSecurityDescriptorGroup'				SetSecurityDescriptorGroup				advapi32	0		# KernelBase
imp	'SetSecurityDescriptorOwner'				SetSecurityDescriptorOwner				advapi32	0		# KernelBase
imp	'SetSecurityDescriptorRMControl'			SetSecurityDescriptorRMControl				advapi32	0		# KernelBase
imp	'SetSecurityDescriptorSacl'				SetSecurityDescriptorSacl				advapi32	0		# KernelBase
imp	'SetSecurityInfo'					SetSecurityInfo						advapi32	1750
imp	'SetSecurityInfoExA'					SetSecurityInfoExA					advapi32	1751
imp	'SetSecurityInfoEx'					SetSecurityInfoExW					advapi32	1752
imp	'SetServiceBits'					SetServiceBits						advapi32	1753
imp	'SetServiceObjectSecurity'				SetServiceObjectSecurity				advapi32	1754
imp	'SetServiceStatus'					SetServiceStatus					advapi32	1755
imp	'SetShellWindow'					SetShellWindow						user32		2373
imp	'SetShellWindowEx'					SetShellWindowEx					user32		2374
imp	'SetStateVersion'					SetStateVersion						KernelBase	1549
imp	'SetStdHandle'						SetStdHandle						kernel32	0	2	# KernelBase
imp	'SetStdHandleEx'					SetStdHandleEx						KernelBase	1551
imp	'SetStretchBltMode'					SetStretchBltMode					gdi32		1908
imp	'SetSysColors'						SetSysColors						user32		2375
imp	'SetSysColorsTemp'					SetSysColorsTemp					user32		2376
imp	'SetSystemCursor'					SetSystemCursor						user32		2377
imp	'SetSystemFileCacheSize'				SetSystemFileCacheSize					kernel32	0		# KernelBase
imp	'SetSystemMenu'						SetSystemMenu						user32		2378
imp	'SetSystemPaletteUse'					SetSystemPaletteUse					gdi32		1909
imp	'SetSystemPowerState'					SetSystemPowerState					kernel32	1363
imp	'SetSystemTime'						SetSystemTime						kernel32	0		# KernelBase
imp	'SetSystemTimeAdjustment'				SetSystemTimeAdjustment					kernel32	0		# KernelBase
imp	'SetSystemTimeAdjustmentPrecise'			SetSystemTimeAdjustmentPrecise				KernelBase	1555
imp	'SetTapeParameters'					SetTapeParameters					kernel32	1366
imp	'SetTapePosition'					SetTapePosition						kernel32	1367
imp	'SetTaskmanWindow'					SetTaskmanWindow					user32		2379
imp	'SetTermsrvAppInstallMode'				SetTermsrvAppInstallMode				kernel32	1368
imp	'SetTextAlign'						SetTextAlign						gdi32		1910	2
imp	'SetTextCharacterExtra'					SetTextCharacterExtra					gdi32		1911
imp	'SetTextColor'						SetTextColor						gdi32		1912	2
imp	'SetTextJustification'					SetTextJustification					gdi32		1913	3
imp	'SetThreadAffinityMask'					SetThreadAffinityMask					kernel32	1369	2
imp	'SetThreadContext'					SetThreadContext					kernel32	0		# KernelBase
imp	'SetThreadDescription'					SetThreadDescription					KernelBase	1557
imp	'SetThreadDesktop'					SetThreadDesktop					user32		2380
imp	'SetThreadDpiAwarenessContext'				SetThreadDpiAwarenessContext				user32		2381
imp	'SetThreadDpiHostingBehavior'				SetThreadDpiHostingBehavior				user32		2382
imp	'SetThreadErrorMode'					SetThreadErrorMode					kernel32	0		# KernelBase
imp	'SetThreadExecutionState'				SetThreadExecutionState					kernel32	1373
imp	'SetThreadGroupAffinity'				SetThreadGroupAffinity					kernel32	0		# KernelBase
imp	'SetThreadIdealProcessor'				SetThreadIdealProcessor					kernel32	0		# KernelBase
imp	'SetThreadIdealProcessorEx'				SetThreadIdealProcessorEx				kernel32	0		# KernelBase
imp	'SetThreadInformation'					SetThreadInformation					kernel32	0		# KernelBase
imp	'SetThreadInputBlocked'					SetThreadInputBlocked					user32		2383
imp	'SetThreadLocale'					SetThreadLocale						kernel32	0		# KernelBase
imp	'SetThreadPreferredUILanguages'				SetThreadPreferredUILanguages				kernel32	0		# KernelBase
imp	'SetThreadPriority'					SetThreadPriority					kernel32	0	2	# KernelBase
imp	'SetThreadPriorityBoost'				SetThreadPriorityBoost					kernel32	0	2	# KernelBase
imp	'SetThreadSelectedCpuSets'				SetThreadSelectedCpuSets				kernel32	0		# KernelBase
imp	'SetThreadStackGuarantee'				SetThreadStackGuarantee					kernel32	0		# KernelBase
imp	'SetThreadToken'					SetThreadToken						advapi32	0		# KernelBase
imp	'SetThreadUILanguage'					SetThreadUILanguage					kernel32	0		# KernelBase
imp	'SetThreadpoolStackInformation'				SetThreadpoolStackInformation				kernel32	0		# KernelBase
imp	'SetThreadpoolThreadMinimum'				SetThreadpoolThreadMinimum				kernel32	0		# KernelBase
imp	'SetTimeZoneInformation'				SetTimeZoneInformation					kernel32	0		# KernelBase
imp	'SetTimer'						SetTimer						user32		2384	4
imp	'SetTimerQueueTimer'					SetTimerQueueTimer					kernel32	1394
imp	'SetTokenInformation'					SetTokenInformation					advapi32	0		# KernelBase
imp	'SetUmsThreadInformation'				SetUmsThreadInformation					kernel32	1395
imp	'SetUnhandledExceptionFilter'				SetUnhandledExceptionFilter				kernel32	0	1	# KernelBase
imp	'SetUserFileEncryptionKey'				SetUserFileEncryptionKey				advapi32	1759
imp	'SetUserFileEncryptionKeyEx'				SetUserFileEncryptionKeyEx				advapi32	1760
imp	'SetUserGeoID'						SetUserGeoID						kernel32	0		# KernelBase
imp	'SetUserGeoName'					SetUserGeoName						KernelBase	1582
imp	'SetUserObjectInformationA'				SetUserObjectInformationA				user32		2385
imp	'SetUserObjectInformation'				SetUserObjectInformationW				user32		2386
imp	'SetUserObjectSecurity'					SetUserObjectSecurity					user32		2387
imp	'SetVDMCurrentDirectories'				SetVDMCurrentDirectories				kernel32	1399
imp	'SetViewportExtEx'					SetViewportExtEx					gdi32		1914
imp	'SetViewportOrgEx'					SetViewportOrgEx					gdi32		1915
imp	'SetVirtualResolution'					SetVirtualResolution					gdi32		1916
imp	'SetVolumeLabelA'					SetVolumeLabelA						kernel32	1400
imp	'SetVolumeLabel'					SetVolumeLabelW						kernel32	1401
imp	'SetVolumeMountPointA'					SetVolumeMountPointA					kernel32	1402
imp	'SetVolumeMountPoint'					SetVolumeMountPointW					kernel32	1403
imp	'SetVolumeMountPointWStub'				SetVolumeMountPointWStub				kernel32	1404
imp	'SetWaitableTimer'					SetWaitableTimer					kernel32	0	6	# KernelBase
imp	'SetWaitableTimerEx'					SetWaitableTimerEx					kernel32	0		# KernelBase
imp	'SetWinEventHook'					SetWinEventHook						user32		2388
imp	'SetWinMetaFileBits'					SetWinMetaFileBits					gdi32		1917
imp	'SetWindowBand'						SetWindowBand						user32		2389
imp	'SetWindowCompositionAttribute'				SetWindowCompositionAttribute				user32		2390
imp	'SetWindowCompositionTransition'			SetWindowCompositionTransition				user32		2391
imp	'SetWindowContextHelpId'				SetWindowContextHelpId					user32		2392
imp	'SetWindowDisplayAffinity'				SetWindowDisplayAffinity				user32		2393
imp	'SetWindowExtEx'					SetWindowExtEx						gdi32		1918
imp	'SetWindowFeedbackSetting'				SetWindowFeedbackSetting				user32		2394
imp	'SetWindowLongA'					SetWindowLongA						user32		2395
imp	'SetWindowLongPtrA'					SetWindowLongPtrA					user32		2396
imp	'SetWindowLongPtr'					SetWindowLongPtrW					user32		2397
imp	'SetWindowLong'						SetWindowLongW						user32		2398
imp	'SetWindowOrgEx'					SetWindowOrgEx						gdi32		1919
imp	'SetWindowPlacement'					SetWindowPlacement					user32		2399	2
imp	'SetWindowPos'						SetWindowPos						user32		2400	7
imp	'SetWindowRgn'						SetWindowRgn						user32		2401
imp	'SetWindowRgnEx'					SetWindowRgnEx						user32		2402
imp	'SetWindowStationUser'					SetWindowStationUser					user32		2403
imp	'SetWindowText'						SetWindowTextW						user32		2405	2
imp	'SetWindowTextA'					SetWindowTextA						user32		2404	2
imp	'SetWindowWord'						SetWindowWord						user32		2406
imp	'SetWindowsHook'					SetWindowsHookW						user32		2410	2
imp	'SetWindowsHookA'					SetWindowsHookA						user32		2407	2
imp	'SetWindowsHookEx'					SetWindowsHookExW					user32		2409	4
imp	'SetWindowsHookExA'					SetWindowsHookExA					user32		2408	4
imp	'SetWorldTransform'					SetWorldTransform					gdi32		1920
imp	'SetXStateFeaturesMask'					SetXStateFeaturesMask					kernel32	0		# KernelBase
imp	'SetupComm'						SetupComm						kernel32	0		# KernelBase
imp	'SharedLocalIsEnabled'					SharedLocalIsEnabled					KernelBase	1587
imp	'SheChangeDirA'						SheChangeDirA						shell32		570
imp	'SheChangeDirEx'					SheChangeDirExW						shell32		571
imp	'SheGetDirA'						SheGetDirA						shell32		572
imp	'SheSetCurDrive'					SheSetCurDrive						shell32		573
imp	'ShellAboutA'						ShellAboutA						shell32		574
imp	'ShellAbout'						ShellAboutW						shell32		575
imp	'ShellExec_RunDLL'					ShellExec_RunDLL					shell32		576
imp	'ShellExec_RunDLLA'					ShellExec_RunDLLA					shell32		577
imp	'ShellExec_RunDLLW'					ShellExec_RunDLLW					shell32		578
imp	'ShellExecuteA'						ShellExecuteA						shell32		579
imp	'ShellExecuteExA'					ShellExecuteExA						shell32		581
imp	'ShellExecuteEx'					ShellExecuteExW						shell32		582
imp	'ShellExecute'						ShellExecuteW						shell32		583
imp	'ShellHookProc'						ShellHookProc						shell32		584
imp	'Shell_GetCachedImageIndexA'				Shell_GetCachedImageIndexA				shell32		585
imp	'Shell_GetCachedImageIndex'				Shell_GetCachedImageIndexW				shell32		586
imp	'Shell_GetImageLists'					Shell_GetImageLists					shell32		71
imp	'Shell_MergeMenus'					Shell_MergeMenus					shell32		67
imp	'Shell_NotifyIconA'					Shell_NotifyIconA					shell32		588
imp	'Shell_NotifyIconGetRect'				Shell_NotifyIconGetRect					shell32		589
imp	'Shell_NotifyIcon'					Shell_NotifyIconW					shell32		590
imp	'ShipAssert'						ShipAssert						ntdll		1612
imp	'ShipAssertGetBufferInfo'				ShipAssertGetBufferInfo					ntdll		1613
imp	'ShipAssertMsgA'					ShipAssertMsgA						ntdll		1614
imp	'ShipAssertMsg'						ShipAssertMsgW						ntdll		1615
imp	'ShowCaret'						ShowCaret						user32		2411	1
imp	'ShowConsoleCursor'					ShowConsoleCursor					kernel32	1409
imp	'ShowCursor'						ShowCursor						user32		2412	1
imp	'ShowOwnedPopups'					ShowOwnedPopups						user32		2413
imp	'ShowScrollBar'						ShowScrollBar						user32		2414
imp	'ShowStartGlass'					ShowStartGlass						user32		2415
imp	'ShowSystemCursor'					ShowSystemCursor					user32		2416
imp	'ShowWindow'						ShowWindow						user32		2417	2
imp	'ShowWindowAsync'					ShowWindowAsync						user32		2418
imp	'ShutdownBlockReasonCreate'				ShutdownBlockReasonCreate				user32		2419
imp	'ShutdownBlockReasonDestroy'				ShutdownBlockReasonDestroy				user32		2420
imp	'ShutdownBlockReasonQuery'				ShutdownBlockReasonQuery				user32		2421
imp	'SignalFileOpen'					SignalFileOpen						shell32		103
imp	'SignalObjectAndWait'					SignalObjectAndWait					kernel32	0		# KernelBase
imp	'SignalRedirectionStartComplete'			SignalRedirectionStartComplete				user32		2422
imp	'SizeofResource'					SizeofResource						kernel32	0		# KernelBase
imp	'SkipPointerFrameMessages'				SkipPointerFrameMessages				user32		2423
imp	'Sleep'							Sleep							kernel32	0	1	# KernelBase
imp	'SleepConditionVariableCS'				SleepConditionVariableCS				kernel32	0		# KernelBase
imp	'SleepConditionVariableSRW'				SleepConditionVariableSRW				kernel32	0		# KernelBase
imp	'SleepEx'						SleepEx							kernel32	0	2	# KernelBase
imp	'SoftModalMessageBox'					SoftModalMessageBox					user32		2424
imp	'SortCloseHandle'					SortCloseHandle						kernel32	1416
imp	'SortGetHandle'						SortGetHandle						kernel32	1417
imp	'SoundSentry'						SoundSentry						user32		2425
imp	'SpecialMBToWC'						SpecialMBToWC						KernelBase	1594
imp	'Ssync_ANSI_UNICODE_Struct_For_WOW'			Ssync_ANSI_UNICODE_Struct_For_WOW			comdlg32	126
imp	'StartDocA'						StartDocA						gdi32		1921
imp	'StartDoc'						StartDocW						gdi32		1922
imp	'StartFormPage'						StartFormPage						gdi32		1923
imp	'StartPage'						StartPage						gdi32		1924
imp	'StartServiceA'						StartServiceA						advapi32	1761
imp	'StartServiceCtrlDispatcherA'				StartServiceCtrlDispatcherA				advapi32	1762
imp	'StartServiceCtrlDispatcher'				StartServiceCtrlDispatcherW				advapi32	1763
imp	'StartService'						StartServiceW						advapi32	1764
imp	'StartTraceA'						StartTraceA						advapi32	1765
imp	'StartTrace'						StartTraceW						advapi32	1766
imp	'StgMakeUniqueName'					StgMakeUniqueName					shell32		682
imp	'StmAlignSize'						StmAlignSize						KernelBase	1596
imp	'StmAllocateFlat'					StmAllocateFlat						KernelBase	1597
imp	'StmCoalesceChunks'					StmCoalesceChunks					KernelBase	1598
imp	'StmDeinitialize'					StmDeinitialize						KernelBase	1599
imp	'StmInitialize'						StmInitialize						KernelBase	1600
imp	'StmReduceSize'						StmReduceSize						KernelBase	1601
imp	'StmReserve'						StmReserve						KernelBase	1602
imp	'StmWrite'						StmWrite						KernelBase	1603
imp	'StopTraceA'						StopTraceA						advapi32	1767
imp	'StopTrace'						StopTraceW						advapi32	1768
imp	'StrCSpnA'						StrCSpnA						KernelBase	1604
imp	'StrCSpnIA'						StrCSpnIA						KernelBase	1605
imp	'StrCSpnIW'						StrCSpnIW						KernelBase	1606
imp	'StrCSpn'						StrCSpnW						KernelBase	1607
imp	'StrCatBuffA'						StrCatBuffA						KernelBase	1608
imp	'StrCatBuff'						StrCatBuffW						KernelBase	1609
imp	'StrCatChain'						StrCatChainW						KernelBase	1610
imp	'StrChrA'						StrChrA							KernelBase	1611
imp	'StrChrA_MB'						StrChrA_MB						KernelBase	1612
imp	'StrChrIA'						StrChrIA						KernelBase	1613
imp	'StrChrIW'						StrChrIW						KernelBase	1614
imp	'StrChrNIW'						StrChrNIW						KernelBase	1615
imp	'StrChrNW'						StrChrNW						KernelBase	1616
imp	'StrChr'						StrChrW							KernelBase	1617
imp	'StrCmpCA'						StrCmpCA						KernelBase	1618
imp	'StrCmpCW'						StrCmpCW						KernelBase	1619
imp	'StrCmpICA'						StrCmpICA						KernelBase	1620
imp	'StrCmpICW'						StrCmpICW						KernelBase	1621
imp	'StrCmpIW'						StrCmpIW						KernelBase	1622
imp	'StrCmpLogical'						StrCmpLogicalW						KernelBase	1623
imp	'StrCmpNA'						StrCmpNA						KernelBase	1624
imp	'StrCmpNCA'						StrCmpNCA						KernelBase	1625
imp	'StrCmpNCW'						StrCmpNCW						KernelBase	1626
imp	'StrCmpNIA'						StrCmpNIA						KernelBase	1627
imp	'StrCmpNICA'						StrCmpNICA						KernelBase	1628
imp	'StrCmpNICW'						StrCmpNICW						KernelBase	1629
imp	'StrCmpNIW'						StrCmpNIW						KernelBase	1630
imp	'StrCmpNW'						StrCmpNW						KernelBase	1631
imp	'StrCmp'						StrCmpW							KernelBase	1632
imp	'StrCpyNW'						StrCpyNW						KernelBase	1633
imp	'StrCpyNXA'						StrCpyNXA						KernelBase	1634
imp	'StrCpyNXW'						StrCpyNXW						KernelBase	1635
imp	'StrDupA'						StrDupA							KernelBase	1636
imp	'StrDup'						StrDupW							KernelBase	1637
imp	'StrIsIntlEqualA'					StrIsIntlEqualA						KernelBase	1638
imp	'StrIsIntlEqual'					StrIsIntlEqualW						KernelBase	1639
imp	'StrNCmpA'						StrNCmpA						shell32		599
imp	'StrNCmpIA'						StrNCmpIA						shell32		600
imp	'StrNCmpIW'						StrNCmpIW						shell32		601
imp	'StrNCmp'						StrNCmpW						shell32		602
imp	'StrPBrkA'						StrPBrkA						KernelBase	1640
imp	'StrPBrk'						StrPBrkW						KernelBase	1641
imp	'StrRChrA'						StrRChrA						KernelBase	1642
imp	'StrRChrIA'						StrRChrIA						KernelBase	1643
imp	'StrRChrIW'						StrRChrIW						KernelBase	1644
imp	'StrRChr'						StrRChrW						KernelBase	1645
imp	'StrRStrA'						StrRStrA						shell32		607
imp	'StrRStrIA'						StrRStrIA						KernelBase	1646
imp	'StrRStrIW'						StrRStrIW						KernelBase	1647
imp	'StrRStr'						StrRStrW						shell32		610
imp	'StrSpnA'						StrSpnA							KernelBase	1648
imp	'StrSpn'						StrSpnW							KernelBase	1649
imp	'StrStrA'						StrStrA							KernelBase	1650
imp	'StrStrIA'						StrStrIA						KernelBase	1651
imp	'StrStrIW'						StrStrIW						KernelBase	1652
imp	'StrStrNIW'						StrStrNIW						KernelBase	1653
imp	'StrStrNW'						StrStrNW						KernelBase	1654
imp	'StrStr'						StrStrW							KernelBase	1655
imp	'StrToInt64ExA'						StrToInt64ExA						KernelBase	1656
imp	'StrToInt64Ex'						StrToInt64ExW						KernelBase	1657
imp	'StrToIntA'						StrToIntA						KernelBase	1658
imp	'StrToIntExA'						StrToIntExA						KernelBase	1659
imp	'StrToIntEx'						StrToIntExW						KernelBase	1660
imp	'StrToInt'						StrToIntW						KernelBase	1661
imp	'StrTrimA'						StrTrimA						KernelBase	1662
imp	'StrTrim'						StrTrimW						KernelBase	1663
imp	'StretchBlt'						StretchBlt						gdi32		1925
imp	'StretchDIBits'						StretchDIBits						gdi32		1926
imp	'StrokeAndFillPath'					StrokeAndFillPath					gdi32		1927
imp	'StrokePath'						StrokePath						gdi32		1928
imp	'SubscribeEdpEnabledStateChange'			SubscribeEdpEnabledStateChange				KernelBase	1665
imp	'SubscribeStateChangeNotification'			SubscribeStateChangeNotification			KernelBase	1666
imp	'SubtractRect'						SubtractRect						user32		2426
imp	'SuspendThread'						SuspendThread						kernel32	0		# KernelBase
imp	'SwapBuffers'						SwapBuffers						gdi32		1929
imp	'SwapMouseButton'					SwapMouseButton						user32		2427
imp	'SwitchDesktop'						SwitchDesktop						user32		2428
imp	'SwitchDesktopWithFade'					SwitchDesktopWithFade					user32		2429
imp	'SwitchToFiber'						SwitchToFiber						kernel32	0		# KernelBase
imp	'SwitchToThisWindow'					SwitchToThisWindow					user32		2430
imp	'SwitchToThread'					SwitchToThread						kernel32	0		# KernelBase
imp	'SystemFunction017'					SystemFunction017					advapi32	1785
imp	'SystemFunction019'					SystemFunction019					advapi32	1787
imp	'SystemParametersInfoA'					SystemParametersInfoA					user32		2431
imp	'SystemParametersInfoForDpi'				SystemParametersInfoForDpi				user32		2432
imp	'SystemParametersInfo'					SystemParametersInfoW					user32		2433
imp	'SystemTimeToFileTime'					SystemTimeToFileTime					kernel32	0	2	# KernelBase
imp	'SystemTimeToTzSpecificLocalTime'			SystemTimeToTzSpecificLocalTime				kernel32	0		# KernelBase
imp	'SystemTimeToTzSpecificLocalTimeEx'			SystemTimeToTzSpecificLocalTimeEx			kernel32	0		# KernelBase
imp	'TabbedTextOutA'					TabbedTextOutA						user32		2434
imp	'TabbedTextOut'						TabbedTextOutW						user32		2435
imp	'TelnetProtocolHandler'					TelnetProtocolHandler					url		113
imp	'TelnetProtocolHandlerA'				TelnetProtocolHandlerA					url		114
imp	'TerminateEnclave'					TerminateEnclave					KernelBase	1673
imp	'TerminateJobObject'					TerminateJobObject					kernel32	1426
imp	'TerminateProcess'					TerminateProcess					kernel32	0	2	# KernelBase
imp	'TerminateProcessOnMemoryExhaustion'			TerminateProcessOnMemoryExhaustion			KernelBase	1675
imp	'TerminateThread'					TerminateThread						kernel32	0		# KernelBase
imp	'TermsrvAppInstallMode'					TermsrvAppInstallMode					kernel32	1429
imp	'TermsrvConvertSysRootToUserDir'			TermsrvConvertSysRootToUserDir				kernel32	1430
imp	'TermsrvCreateRegEntry'					TermsrvCreateRegEntry					kernel32	1431
imp	'TermsrvDeleteKey'					TermsrvDeleteKey					kernel32	1432
imp	'TermsrvDeleteValue'					TermsrvDeleteValue					kernel32	1433
imp	'TermsrvGetPreSetValue'					TermsrvGetPreSetValue					kernel32	1434
imp	'TermsrvGetWindowsDirectoryA'				TermsrvGetWindowsDirectoryA				kernel32	1435
imp	'TermsrvGetWindowsDirectory'				TermsrvGetWindowsDirectoryW				kernel32	1436
imp	'TermsrvOpenRegEntry'					TermsrvOpenRegEntry					kernel32	1437
imp	'TermsrvOpenUserClasses'				TermsrvOpenUserClasses					kernel32	1438
imp	'TermsrvRestoreKey'					TermsrvRestoreKey					kernel32	1439
imp	'TermsrvSetKeySecurity'					TermsrvSetKeySecurity					kernel32	1440
imp	'TermsrvSetValueKey'					TermsrvSetValueKey					kernel32	1441
imp	'TermsrvSyncUserIniFileExt'				TermsrvSyncUserIniFileExt				kernel32	1442
imp	'TextOutA'						TextOutA						gdi32		1930
imp	'TextOut'						TextOutW						gdi32		1931
imp	'Thread32First'						Thread32First						kernel32	1443
imp	'Thread32Next'						Thread32Next						kernel32	1444
imp	'TileChildWindows'					TileChildWindows					user32		2436
imp	'TileWindows'						TileWindows						user32		2437
imp	'TlsAlloc'						TlsAlloc						kernel32	0		# KernelBase
imp	'TlsFree'						TlsFree							kernel32	0		# KernelBase
imp	'TlsGetValue'						TlsGetValue						kernel32	0		# KernelBase
imp	'TlsSetValue'						TlsSetValue						kernel32	0		# KernelBase
imp	'ToAscii'						ToAscii							user32		2438
imp	'ToAsciiEx'						ToAsciiEx						user32		2439
imp	'ToUnicode'						ToUnicode						user32		2440
imp	'ToUnicodeEx'						ToUnicodeEx						user32		2441
imp	'Toolhelp32ReadProcessMemory'				Toolhelp32ReadProcessMemory				kernel32	1449
imp	'TpAllocAlpcCompletion'					TpAllocAlpcCompletion					ntdll		1616
imp	'TpAllocAlpcCompletionEx'				TpAllocAlpcCompletionEx					ntdll		1617
imp	'TpAllocCleanupGroup'					TpAllocCleanupGroup					ntdll		1618
imp	'TpAllocIoCompletion'					TpAllocIoCompletion					ntdll		1619
imp	'TpAllocJobNotification'				TpAllocJobNotification					ntdll		1620
imp	'TpAllocPool'						TpAllocPool						ntdll		1621
imp	'TpAllocTimer'						TpAllocTimer						ntdll		1622
imp	'TpAllocWait'						TpAllocWait						ntdll		1623
imp	'TpAllocWork'						TpAllocWork						ntdll		1624
imp	'TpAlpcRegisterCompletionList'				TpAlpcRegisterCompletionList				ntdll		1625
imp	'TpAlpcUnregisterCompletionList'			TpAlpcUnregisterCompletionList				ntdll		1626
imp	'TpCallbackDetectedUnrecoverableError'			TpCallbackDetectedUnrecoverableError			ntdll		1627
imp	'TpCallbackIndependent'					TpCallbackIndependent					ntdll		1628
imp	'TpCallbackLeaveCriticalSectionOnCompletion'		TpCallbackLeaveCriticalSectionOnCompletion		ntdll		1629
imp	'TpCallbackMayRunLong'					TpCallbackMayRunLong					ntdll		1630
imp	'TpCallbackReleaseMutexOnCompletion'			TpCallbackReleaseMutexOnCompletion			ntdll		1631
imp	'TpCallbackReleaseSemaphoreOnCompletion'		TpCallbackReleaseSemaphoreOnCompletion			ntdll		1632
imp	'TpCallbackSendAlpcMessageOnCompletion'			TpCallbackSendAlpcMessageOnCompletion			ntdll		1633
imp	'TpCallbackSendPendingAlpcMessage'			TpCallbackSendPendingAlpcMessage			ntdll		1634
imp	'TpCallbackSetEventOnCompletion'			TpCallbackSetEventOnCompletion				ntdll		1635
imp	'TpCallbackUnloadDllOnCompletion'			TpCallbackUnloadDllOnCompletion				ntdll		1636
imp	'TpCancelAsyncIoOperation'				TpCancelAsyncIoOperation				ntdll		1637
imp	'TpCaptureCaller'					TpCaptureCaller						ntdll		1638
imp	'TpCheckTerminateWorker'				TpCheckTerminateWorker					ntdll		1639
imp	'TpDbgDumpHeapUsage'					TpDbgDumpHeapUsage					ntdll		1640
imp	'TpDbgSetLogRoutine'					TpDbgSetLogRoutine					ntdll		1641
imp	'TpDisablePoolCallbackChecks'				TpDisablePoolCallbackChecks				ntdll		1642
imp	'TpDisassociateCallback'				TpDisassociateCallback					ntdll		1643
imp	'TpIsTimerSet'						TpIsTimerSet						ntdll		1644
imp	'TpPostWork'						TpPostWork						ntdll		1645
imp	'TpQueryPoolStackInformation'				TpQueryPoolStackInformation				ntdll		1646
imp	'TpReleaseAlpcCompletion'				TpReleaseAlpcCompletion					ntdll		1647
imp	'TpReleaseCleanupGroup'					TpReleaseCleanupGroup					ntdll		1648
imp	'TpReleaseCleanupGroupMembers'				TpReleaseCleanupGroupMembers				ntdll		1649
imp	'TpReleaseIoCompletion'					TpReleaseIoCompletion					ntdll		1650
imp	'TpReleaseJobNotification'				TpReleaseJobNotification				ntdll		1651
imp	'TpReleasePool'						TpReleasePool						ntdll		1652
imp	'TpReleaseTimer'					TpReleaseTimer						ntdll		1653
imp	'TpReleaseWait'						TpReleaseWait						ntdll		1654
imp	'TpReleaseWork'						TpReleaseWork						ntdll		1655
imp	'TpSetDefaultPoolMaxThreads'				TpSetDefaultPoolMaxThreads				ntdll		1656
imp	'TpSetDefaultPoolStackInformation'			TpSetDefaultPoolStackInformation			ntdll		1657
imp	'TpSetPoolMaxThreads'					TpSetPoolMaxThreads					ntdll		1658
imp	'TpSetPoolMaxThreadsSoftLimit'				TpSetPoolMaxThreadsSoftLimit				ntdll		1659
imp	'TpSetPoolMinThreads'					TpSetPoolMinThreads					ntdll		1660
imp	'TpSetPoolStackInformation'				TpSetPoolStackInformation				ntdll		1661
imp	'TpSetPoolThreadBasePriority'				TpSetPoolThreadBasePriority				ntdll		1662
imp	'TpSetPoolWorkerThreadIdleTimeout'			TpSetPoolWorkerThreadIdleTimeout			ntdll		1663
imp	'TpSetTimer'						TpSetTimer						ntdll		1664
imp	'TpSetTimerEx'						TpSetTimerEx						ntdll		1665
imp	'TpSetWait'						TpSetWait						ntdll		1666
imp	'TpSetWaitEx'						TpSetWaitEx						ntdll		1667
imp	'TpSimpleTryPost'					TpSimpleTryPost						ntdll		1668
imp	'TpStartAsyncIoOperation'				TpStartAsyncIoOperation					ntdll		1669
imp	'TpTimerOutstandingCallbackCount'			TpTimerOutstandingCallbackCount				ntdll		1670
imp	'TpTrimPools'						TpTrimPools						ntdll		1671
imp	'TpWaitForAlpcCompletion'				TpWaitForAlpcCompletion					ntdll		1672
imp	'TpWaitForIoCompletion'					TpWaitForIoCompletion					ntdll		1673
imp	'TpWaitForJobNotification'				TpWaitForJobNotification				ntdll		1674
imp	'TpWaitForTimer'					TpWaitForTimer						ntdll		1675
imp	'TpWaitForWait'						TpWaitForWait						ntdll		1676
imp	'TpWaitForWork'						TpWaitForWork						ntdll		1677
imp	'TraceSetInformation'					TraceSetInformation					advapi32	1812
imp	'TrackMouseEvent'					TrackMouseEvent						user32		2442
imp	'TrackPopupMenu'					TrackPopupMenu						user32		2443	7
imp	'TrackPopupMenuEx'					TrackPopupMenuEx					user32		2444
imp	'TransactNamedPipe'					TransactNamedPipe					kernel32	0	7	# KernelBase
imp	'TranslateAcceleratorA'					TranslateAcceleratorA					user32		2446
imp	'TranslateAccelerator'					TranslateAcceleratorW					user32		2447
imp	'TranslateCharsetInfo'					TranslateCharsetInfo					gdi32		1932
imp	'TranslateMDISysAccel'					TranslateMDISysAccel					user32		2448
imp	'TranslateMessage'					TranslateMessage					user32		2449	1
imp	'TranslateMessageEx'					TranslateMessageEx					user32		2450
imp	'TranslateURLA'						TranslateURLA						url		115
imp	'TranslateURLW'						TranslateURLW						url		116
imp	'TransmitFile'						TransmitFile						MsWSock		53	7
imp	'TransmitCommChar'					TransmitCommChar					kernel32	0		# KernelBase
imp	'TreeResetNamedSecurityInfoA'				TreeResetNamedSecurityInfoA				advapi32	1813
imp	'TreeResetNamedSecurityInfo'				TreeResetNamedSecurityInfoW				advapi32	1814
imp	'TreeSetNamedSecurityInfoA'				TreeSetNamedSecurityInfoA				advapi32	1815
imp	'TreeSetNamedSecurityInfo'				TreeSetNamedSecurityInfoW				advapi32	1816
imp	'TrusteeAccessToObjectA'				TrusteeAccessToObjectA					advapi32	1817
imp	'TrusteeAccessToObject'					TrusteeAccessToObjectW					advapi32	1818
imp	'TrySubmitThreadpoolCallback'				TrySubmitThreadpoolCallback				kernel32	0		# KernelBase
imp	'TzSpecificLocalTimeToSystemTime'			TzSpecificLocalTimeToSystemTime				kernel32	0		# KernelBase
imp	'TzSpecificLocalTimeToSystemTimeEx'			TzSpecificLocalTimeToSystemTimeEx			kernel32	0		# KernelBase
imp	'URLAssociationDialogA'					URLAssociationDialogA					url		117
imp	'URLAssociationDialog'					URLAssociationDialogW					url		118
imp	'UTRegister'						UTRegister						kernel32	1458
imp	'UTUnRegister'						UTUnRegister						kernel32	1459
imp	'UmsThreadYield'					UmsThreadYield						kernel32	1460
imp	'UndelegateInput'					UndelegateInput						user32		2504
imp	'UnhandledExceptionFilter'				UnhandledExceptionFilter				kernel32	0		# KernelBase
imp	'UnhookWinEvent'					UnhookWinEvent						user32		2451
imp	'UnhookWindowsHook'					UnhookWindowsHook					user32		2452	2
imp	'UnhookWindowsHookEx'					UnhookWindowsHookEx					user32		2453	1
imp	'UninstallApplication'					UninstallApplication					advapi32	1819
imp	'UnionRect'						UnionRect						user32		2454
imp	'UnloadKeyboardLayout'					UnloadKeyboardLayout					user32		2455
imp	'UnloadNetworkFonts'					UnloadNetworkFonts					gdi32		1933
imp	'UnlockFile'						UnlockFile						kernel32	0	5	# KernelBase
imp	'UnlockFileEx'						UnlockFileEx						kernel32	0	5	# KernelBase
imp	'UnlockServiceDatabase'					UnlockServiceDatabase					advapi32	1820
imp	'UnlockWindowStation'					UnlockWindowStation					user32		2456
imp	'UnmapViewOfFile'					UnmapViewOfFile						kernel32	0	1	# KernelBase
imp	'UnmapViewOfFile2'					UnmapViewOfFile2					KernelBase	1696
imp	'UnmapViewOfFileEx'					UnmapViewOfFileEx					KernelBase	1697
imp	'UnpackDDElParam'					UnpackDDElParam						user32		2457
imp	'UnrealizeObject'					UnrealizeObject						gdi32		1934
imp	'UnregisterApplicationRecoveryCallback'			UnregisterApplicationRecoveryCallback			kernel32	1466
imp	'UnregisterApplicationRestart'				UnregisterApplicationRestart				kernel32	1467
imp	'UnregisterBadMemoryNotification'			UnregisterBadMemoryNotification				kernel32	0		# KernelBase
imp	'UnregisterClassA'					UnregisterClassA					user32		2458
imp	'UnregisterClass'					UnregisterClassW					user32		2459
imp	'UnregisterConsoleIME'					UnregisterConsoleIME					kernel32	1469
imp	'UnregisterDeviceNotification'				UnregisterDeviceNotification				user32		2460
imp	'UnregisterGPNotificationInternal'			UnregisterGPNotificationInternal			KernelBase	1699
imp	'UnregisterHotKey'					UnregisterHotKey					user32		2461
imp	'UnregisterIdleTask'					UnregisterIdleTask					advapi32	1821
imp	'UnregisterMessagePumpHook'				UnregisterMessagePumpHook				user32		2462
imp	'UnregisterPointerInputTarget'				UnregisterPointerInputTarget				user32		2463
imp	'UnregisterPointerInputTargetEx'			UnregisterPointerInputTargetEx				user32		2464
imp	'UnregisterPowerSettingNotification'			UnregisterPowerSettingNotification			user32		2465
imp	'UnregisterSessionPort'					UnregisterSessionPort					user32		2466
imp	'UnregisterStateChangeNotification'			UnregisterStateChangeNotification			KernelBase	1700
imp	'UnregisterStateLock'					UnregisterStateLock					KernelBase	1701
imp	'UnregisterSuspendResumeNotification'			UnregisterSuspendResumeNotification			user32		2467
imp	'UnregisterTouchWindow'					UnregisterTouchWindow					user32		2468
imp	'UnregisterUserApiHook'					UnregisterUserApiHook					user32		2469
imp	'UnregisterWait'					UnregisterWait						kernel32	1470
imp	'UnregisterWaitEx'					UnregisterWaitEx					kernel32	0		# KernelBase
imp	'UnregisterWaitUntilOOBECompleted'			UnregisterWaitUntilOOBECompleted			kernel32	1472
imp	'UnsubscribeEdpEnabledStateChange'			UnsubscribeEdpEnabledStateChange			KernelBase	1704
imp	'UnsubscribeStateChangeNotification'			UnsubscribeStateChangeNotification			KernelBase	1705
imp	'UpdateCalendarDayOfWeek'				UpdateCalendarDayOfWeek					kernel32	1473
imp	'UpdateColors'						UpdateColors						gdi32		1935
imp	'UpdateDefaultDesktopThumbnail'				UpdateDefaultDesktopThumbnail				user32		2470
imp	'UpdateICMRegKeyA'					UpdateICMRegKeyA					gdi32		1936
imp	'UpdateICMRegKey'					UpdateICMRegKeyW					gdi32		1937
imp	'UpdateLayeredWindow'					UpdateLayeredWindow					user32		2471
imp	'UpdateLayeredWindowIndirect'				UpdateLayeredWindowIndirect				user32		2472
imp	'UpdatePackageStatus'					UpdatePackageStatus					KernelBase	1706
imp	'UpdatePackageStatusForUser'				UpdatePackageStatusForUser				KernelBase	1707
imp	'UpdatePackageStatusForUserSid'				UpdatePackageStatusForUserSid				KernelBase	1708
imp	'UpdatePerUserSystemParameters'				UpdatePerUserSystemParameters				user32		2473
imp	'UpdateProcThreadAttribute'				UpdateProcThreadAttribute				kernel32	0	7	# KernelBase
imp	'UpdateResourceA'					UpdateResourceA						kernel32	1475
imp	'UpdateResource'					UpdateResourceW						kernel32	1476
imp	'UpdateTraceA'						UpdateTraceA						advapi32	1823
imp	'UpdateTrace'						UpdateTraceW						advapi32	1824
imp	'UpdateWindow'						UpdateWindow						user32		2474
imp	'UpdateWindowInputSinkHints'				UpdateWindowInputSinkHints				user32		2475
imp	'UrlApplySchemeA'					UrlApplySchemeA						KernelBase	1710
imp	'UrlApplyScheme'					UrlApplySchemeW						KernelBase	1711
imp	'UrlCanonicalizeA'					UrlCanonicalizeA					KernelBase	1712
imp	'UrlCanonicalize'					UrlCanonicalizeW					KernelBase	1713
imp	'UrlCombineA'						UrlCombineA						KernelBase	1714
imp	'UrlCombine'						UrlCombineW						KernelBase	1715
imp	'UrlCompareA'						UrlCompareA						KernelBase	1716
imp	'UrlCompare'						UrlCompareW						KernelBase	1717
imp	'UrlCreateFromPathA'					UrlCreateFromPathA					KernelBase	1718
imp	'UrlCreateFromPath'					UrlCreateFromPathW					KernelBase	1719
imp	'UrlEscapeA'						UrlEscapeA						KernelBase	1720
imp	'UrlEscape'						UrlEscapeW						KernelBase	1721
imp	'UrlFixup'						UrlFixupW						KernelBase	1722
imp	'UrlGetLocationA'					UrlGetLocationA						KernelBase	1723
imp	'UrlGetLocation'					UrlGetLocationW						KernelBase	1724
imp	'UrlGetPartA'						UrlGetPartA						KernelBase	1725
imp	'UrlGetPart'						UrlGetPartW						KernelBase	1726
imp	'UrlHashA'						UrlHashA						KernelBase	1727
imp	'UrlHash'						UrlHashW						KernelBase	1728
imp	'UrlIsA'						UrlIsA							KernelBase	1729
imp	'UrlIsNoHistoryA'					UrlIsNoHistoryA						KernelBase	1730
imp	'UrlIsNoHistory'					UrlIsNoHistoryW						KernelBase	1731
imp	'UrlIsOpaqueA'						UrlIsOpaqueA						KernelBase	1732
imp	'UrlIsOpaque'						UrlIsOpaqueW						KernelBase	1733
imp	'UrlIs'							UrlIsW							KernelBase	1734
imp	'UrlUnescapeA'						UrlUnescapeA						KernelBase	1735
imp	'UrlUnescape'						UrlUnescapeW						KernelBase	1736
imp	'UsePinForEncryptedFilesA'				UsePinForEncryptedFilesA				advapi32	1825
imp	'UsePinForEncryptedFiles'				UsePinForEncryptedFilesW				advapi32	1826
imp	'User32InitializeImmEntryTable'				User32InitializeImmEntryTable				user32		2476
imp	'UserClientDllInitialize'				UserClientDllInitialize					user32		2477
imp	'UserHandleGrantAccess'					UserHandleGrantAccess					user32		2478
imp	'UserLpkPSMTextOut'					UserLpkPSMTextOut					user32		2479
imp	'UserLpkTabbedTextOut'					UserLpkTabbedTextOut					user32		2480
imp	'UserRealizePalette'					UserRealizePalette					user32		2481
imp	'UserRegisterWowHandlers'				UserRegisterWowHandlers					user32		2482
imp	'UsersLibrariesFolderUI_CreateInstance'			UsersLibrariesFolderUI_CreateInstance			shell32		615
imp	'VDMConsoleOperation'					VDMConsoleOperation					kernel32	1477
imp	'VDMOperationStarted'					VDMOperationStarted					kernel32	1478
imp	'VRipOutput'						VRipOutput						user32		2483
imp	'VTagOutput'						VTagOutput						user32		2484
imp	'ValidateRect'						ValidateRect						user32		2485
imp	'ValidateRgn'						ValidateRgn						user32		2486
imp	'VerFindFileA'						VerFindFileA						KernelBase	1737
imp	'VerFindFile'						VerFindFileW						KernelBase	1738
imp	'VerLanguageNameA'					VerLanguageNameA					kernel32	0		# KernelBase
imp	'VerLanguageName'					VerLanguageNameW					kernel32	0		# KernelBase
imp	'VerQueryValueA'					VerQueryValueA						KernelBase	1741
imp	'VerQueryValue'						VerQueryValueW						KernelBase	1742
imp	'VerSetConditionMask'					VerSetConditionMask					ntdll		1678
imp	'VerifyApplicationUserModelId'				VerifyApplicationUserModelId				KernelBase	1744
imp	'VerifyApplicationUserModelIdA'				VerifyApplicationUserModelIdA				KernelBase	1745
imp	'VerifyConsoleIoHandle'					VerifyConsoleIoHandle					kernel32	1482
imp	'VerifyPackageFamilyName'				VerifyPackageFamilyName					KernelBase	1746
imp	'VerifyPackageFamilyNameA'				VerifyPackageFamilyNameA				KernelBase	1747
imp	'VerifyPackageFullName'					VerifyPackageFullName					KernelBase	1748
imp	'VerifyPackageFullNameA'				VerifyPackageFullNameA					KernelBase	1749
imp	'VerifyPackageId'					VerifyPackageId						KernelBase	1750
imp	'VerifyPackageIdA'					VerifyPackageIdA					KernelBase	1751
imp	'VerifyPackageRelativeApplicationId'			VerifyPackageRelativeApplicationId			KernelBase	1752
imp	'VerifyPackageRelativeApplicationIdA'			VerifyPackageRelativeApplicationIdA			KernelBase	1753
imp	'VerifyScripts'						VerifyScripts						kernel32	0		# KernelBase
imp	'VerifyVersionInfoA'					VerifyVersionInfoA					kernel32	1484
imp	'VerifyVersionInfo'					VerifyVersionInfoW					kernel32	1485
imp	'VirtualAlloc'						VirtualAlloc						kernel32	0	4	# KernelBase
imp	'VirtualAlloc2'						VirtualAlloc2						KernelBase	1756
imp	'VirtualAlloc2FromApp'					VirtualAlloc2FromApp					KernelBase	1757
imp	'VirtualAllocEx'					VirtualAllocEx						kernel32	0	5	# KernelBase
imp	'VirtualAllocExNuma'					VirtualAllocExNuma					kernel32	0		# KernelBase
imp	'VirtualAllocFromApp'					VirtualAllocFromApp					KernelBase	1760
imp	'VirtualFree'						VirtualFree						kernel32	0	3	# KernelBase
imp	'VirtualFreeEx'						VirtualFreeEx						kernel32	0		# KernelBase
imp	'VirtualLock'						VirtualLock						kernel32	0		# KernelBase
imp	'VirtualProtect'					VirtualProtect						kernel32	0	4	# KernelBase
imp	'VirtualProtectEx'					VirtualProtectEx					kernel32	0		# KernelBase
imp	'VirtualProtectFromApp'					VirtualProtectFromApp					KernelBase	1766
imp	'VirtualQuery'						VirtualQuery						kernel32	0	3	# KernelBase
imp	'VirtualQueryEx'					VirtualQueryEx						kernel32	0		# KernelBase
imp	'VirtualUnlock'						VirtualUnlock						kernel32	0		# KernelBase
imp	'VirtualUnlockEx'					VirtualUnlockEx						KernelBase	1770
imp	'VkKeyScanA'						VkKeyScanA						user32		2487
imp	'VkKeyScanExA'						VkKeyScanExA						user32		2488
imp	'VkKeyScanEx'						VkKeyScanExW						user32		2489
imp	'VkKeyScan'						VkKeyScanW						user32		2490
imp	'WCSToMBEx'						WCSToMBEx						user32		2491
imp	'WEP'							WEP							ws2_32		500
imp	'WINNLSEnableIME'					WINNLSEnableIME						user32		2492
imp	'WINNLSGetEnableStatus'					WINNLSGetEnableStatus					user32		2493
imp	'WINNLSGetIMEHotkey'					WINNLSGetIMEHotkey					user32		2494
imp	'WOWShellExecute'					WOWShellExecute						shell32		616
imp	'WPUCompleteOverlappedRequest'				WPUCompleteOverlappedRequest				ws2_32		39
imp	'WPUGetProviderPathEx'					WPUGetProviderPathEx					ws2_32		40
imp	'WSAAccept'						WSAAccept						ws2_32		41	5
imp	'WSAAddressToString'					WSAAddressToStringW					ws2_32		43	5
imp	'WSAAddressToStringA'					WSAAddressToStringA					ws2_32		42	5
imp	'WSAAdvertiseProvider'					WSAAdvertiseProvider					ws2_32		44
imp	'WSAAsyncGetHostByAddr'					WSAAsyncGetHostByAddr					ws2_32		102
imp	'WSAAsyncGetHostByName'					WSAAsyncGetHostByName					ws2_32		103
imp	'WSAAsyncGetProtoByName'				WSAAsyncGetProtoByName					ws2_32		105
imp	'WSAAsyncGetProtoByNumber'				WSAAsyncGetProtoByNumber				ws2_32		104
imp	'WSAAsyncGetServByName'					WSAAsyncGetServByName					ws2_32		107
imp	'WSAAsyncGetServByPort'					WSAAsyncGetServByPort					ws2_32		106
imp	'WSAAsyncSelect'					WSAAsyncSelect						ws2_32		101
imp	'WSACancelAsyncRequest'					WSACancelAsyncRequest					ws2_32		108
imp	'WSACancelBlockingCall'					WSACancelBlockingCall					ws2_32		113
imp	'WSACleanup'						WSACleanup						ws2_32		116	0
imp	'WSACloseEvent'						WSACloseEvent						ws2_32		45	1
imp	'WSAConnect'						WSAConnect						ws2_32		46	7
imp	'WSAConnectByList'					WSAConnectByList					ws2_32		47	8
imp	'WSAConnectByName'					WSAConnectByNameW					ws2_32		49	9
imp	'WSAConnectByNameA'					WSAConnectByNameA					ws2_32		48	9
imp	'WSACreateEvent'					WSACreateEvent						ws2_32		50	0
imp	'WSADuplicateSocket'					WSADuplicateSocketW					ws2_32		59	3
imp	'WSADuplicateSocketA'					WSADuplicateSocketA					ws2_32		58	3
imp	'WSAEnumNameSpaceProvidersEx'				WSAEnumNameSpaceProvidersExW				ws2_32		62	2
imp	'WSAEnumNameSpaceProvidersExA'				WSAEnumNameSpaceProvidersExA				ws2_32		61	2
imp	'WSAEnumNameSpaceProviders'				WSAEnumNameSpaceProvidersW				ws2_32		63
imp	'WSAEnumNameSpaceProvidersA'				WSAEnumNameSpaceProvidersA				ws2_32		60
imp	'WSAEnumNetworkEvents'					WSAEnumNetworkEvents					ws2_32		64	3
imp	'WSAEnumProtocols'					WSAEnumProtocolsW					ws2_32		66	3
imp	'WSAEnumProtocolsA'					WSAEnumProtocolsA					ws2_32		65	3
imp	'WSAEventSelect'					WSAEventSelect						ws2_32		67	3
imp	'WSAGetLastError'					WSAGetLastError						ws2_32		111	0
imp	'WSAGetOverlappedResult'				WSAGetOverlappedResult					ws2_32		68	5
imp	'WSAGetQOSByName'					WSAGetQOSByName						ws2_32		69	3
imp	'WSAGetServiceClassInfo'				WSAGetServiceClassInfoW					ws2_32		71	4
imp	'WSAGetServiceClassInfoA'				WSAGetServiceClassInfoA					ws2_32		70	4
imp	'WSAGetServiceClassNameByClassId'			WSAGetServiceClassNameByClassIdW			ws2_32		73	3
imp	'WSAGetServiceClassNameByClassIdA'			WSAGetServiceClassNameByClassIdA			ws2_32		72	3
imp	'WSAHtonl'						WSAHtonl						ws2_32		74
imp	'WSAHtons'						WSAHtons						ws2_32		75
imp	'WSAInstallServiceClass'				WSAInstallServiceClassW					ws2_32		77	1
imp	'WSAInstallServiceClassA'				WSAInstallServiceClassA					ws2_32		76	1
imp	'WSAIoctl'						WSAIoctl						ws2_32		78	9
imp	'WSAIsBlocking'						WSAIsBlocking						ws2_32		114
imp	'WSAJoinLeaf'						WSAJoinLeaf						ws2_32		79	8
imp	'WSALookupServiceBegin'					WSALookupServiceBeginW					ws2_32		81	3
imp	'WSALookupServiceBeginA'				WSALookupServiceBeginA					ws2_32		80	3
imp	'WSALookupServiceEnd'					WSALookupServiceEnd					ws2_32		82	1
imp	'WSALookupServiceNext'					WSALookupServiceNextW					ws2_32		84	4
imp	'WSALookupServiceNextA'					WSALookupServiceNextA					ws2_32		83	4
imp	'WSANSPIoctl'						WSANSPIoctl						ws2_32		85	8
imp	'WSANtohl'						WSANtohl						ws2_32		86
imp	'WSANtohs'						WSANtohs						ws2_32		87
imp	'WSAPoll'						WSAPoll							ws2_32		88	3
imp	'WSAProviderCompleteAsyncCall'				WSAProviderCompleteAsyncCall				ws2_32		89
imp	'WSAProviderConfigChange'				WSAProviderConfigChange					ws2_32		90	3
imp	'WSARecv'						WSARecv							ws2_32		91	7
imp	'WSARecvDisconnect'					WSARecvDisconnect					ws2_32		92	2
imp	'WSARecvFrom'						WSARecvFrom						ws2_32		93	9
imp	'WSARecvEx'						WSARecvEx						MsWSock		54
imp	'WSARemoveServiceClass'					WSARemoveServiceClass					ws2_32		94	1
imp	'WSAResetEvent'						WSAResetEvent						ws2_32		95	1
imp	'WSASend'						WSASend							ws2_32		96	7
imp	'WSASendDisconnect'					WSASendDisconnect					ws2_32		97
imp	'WSASendMsg'						WSASendMsg						ws2_32		98	6
imp	'WSASendTo'						WSASendTo						ws2_32		99	9
imp	'WSASetBlockingHook'					WSASetBlockingHook					ws2_32		109
imp	'WSASetEvent'						WSASetEvent						ws2_32		100	1
imp	'WSASetLastError'					WSASetLastError						ws2_32		112	1
imp	'WSASetService'						WSASetServiceW						ws2_32		118	3
imp	'WSASetServiceA'					WSASetServiceA						ws2_32		117	3
imp	'WSASocket'						WSASocketW						ws2_32		120	6
imp	'WSASocketA'						WSASocketA						ws2_32		119	6
imp	'WSAStartup'						WSAStartup						ws2_32		115	2
imp	'WSAStringToAddressA'					WSAStringToAddressA					ws2_32		121	5
imp	'WSAStringToAddress'					WSAStringToAddressW					ws2_32		122
imp	'WSAUnadvertiseProvider'				WSAUnadvertiseProvider					ws2_32		123
imp	'WSAUnhookBlockingHook'					WSAUnhookBlockingHook					ws2_32		110
imp	'WSAWaitForMultipleEvents'				WSAWaitForMultipleEvents				ws2_32		124	5
imp	'WSApSetPostRoutine'					WSApSetPostRoutine					ws2_32		24
imp	'WSCDeinstallProvider'					WSCDeinstallProvider					ws2_32		125
imp	'WSCDeinstallProvider32'				WSCDeinstallProvider32					ws2_32		126
imp	'WSCDeinstallProviderEx'				WSCDeinstallProviderEx					ws2_32		127
imp	'WSCEnableNSProvider'					WSCEnableNSProvider					ws2_32		128
imp	'WSCEnableNSProvider32'					WSCEnableNSProvider32					ws2_32		129
imp	'WSCEnumNameSpaceProviders32'				WSCEnumNameSpaceProviders32				ws2_32		130
imp	'WSCEnumNameSpaceProvidersEx32'				WSCEnumNameSpaceProvidersEx32				ws2_32		131
imp	'WSCEnumProtocols'					WSCEnumProtocols					ws2_32		132
imp	'WSCEnumProtocols32'					WSCEnumProtocols32					ws2_32		133
imp	'WSCEnumProtocolsEx'					WSCEnumProtocolsEx					ws2_32		134
imp	'WSCGetApplicationCategory'				WSCGetApplicationCategory				ws2_32		135
imp	'WSCGetApplicationCategoryEx'				WSCGetApplicationCategoryEx				ws2_32		136
imp	'WSCGetProviderInfo'					WSCGetProviderInfo					ws2_32		137
imp	'WSCGetProviderInfo32'					WSCGetProviderInfo32					ws2_32		138
imp	'WSCGetProviderPath'					WSCGetProviderPath					ws2_32		139
imp	'WSCGetProviderPath32'					WSCGetProviderPath32					ws2_32		140
imp	'WSCInstallNameSpace'					WSCInstallNameSpace					ws2_32		141
imp	'WSCInstallNameSpace32'					WSCInstallNameSpace32					ws2_32		142
imp	'WSCInstallNameSpaceEx'					WSCInstallNameSpaceEx					ws2_32		143
imp	'WSCInstallNameSpaceEx2'				WSCInstallNameSpaceEx2					ws2_32		144
imp	'WSCInstallNameSpaceEx32'				WSCInstallNameSpaceEx32					ws2_32		145
imp	'WSCInstallProvider'					WSCInstallProvider					ws2_32		146
imp	'WSCInstallProvider64_32'				WSCInstallProvider64_32					ws2_32		147
imp	'WSCInstallProviderAndChains64_32'			WSCInstallProviderAndChains64_32			ws2_32		148
imp	'WSCInstallProviderEx'					WSCInstallProviderEx					ws2_32		149
imp	'WSCSetApplicationCategory'				WSCSetApplicationCategory				ws2_32		150
imp	'WSCSetApplicationCategoryEx'				WSCSetApplicationCategoryEx				ws2_32		152
imp	'WSCSetProviderInfo'					WSCSetProviderInfo					ws2_32		153
imp	'WSCSetProviderInfo32'					WSCSetProviderInfo32					ws2_32		154
imp	'WSCUnInstallNameSpace'					WSCUnInstallNameSpace					ws2_32		155
imp	'WSCUnInstallNameSpace32'				WSCUnInstallNameSpace32					ws2_32		156
imp	'WSCUnInstallNameSpaceEx2'				WSCUnInstallNameSpaceEx2				ws2_32		157
imp	'WSCUpdateProvider'					WSCUpdateProvider					ws2_32		158
imp	'WSCUpdateProvider32'					WSCUpdateProvider32					ws2_32		159
imp	'WSCUpdateProviderEx'					WSCUpdateProviderEx					ws2_32		160
imp	'WSCWriteNameSpaceOrder'				WSCWriteNameSpaceOrder					ws2_32		161
imp	'WSCWriteNameSpaceOrder32'				WSCWriteNameSpaceOrder32				ws2_32		162
imp	'WSCWriteProviderOrder'					WSCWriteProviderOrder					ws2_32		163
imp	'WSCWriteProviderOrder32'				WSCWriteProviderOrder32					ws2_32		164
imp	'WSCWriteProviderOrderEx'				WSCWriteProviderOrderEx					ws2_32		165
imp	'WTSGetActiveConsoleSessionId'				WTSGetActiveConsoleSessionId				kernel32	1497
imp	'WTSGetServiceSessionId'				WTSGetServiceSessionId					KernelBase	1771
imp	'WTSIsServerContainer'					WTSIsServerContainer					KernelBase	1772
imp	'WahCloseApcHelper'					WahCloseApcHelper					ws2_32		166
imp	'WahCloseHandleHelper'					WahCloseHandleHelper					ws2_32		167
imp	'WahCloseNotificationHandleHelper'			WahCloseNotificationHandleHelper			ws2_32		168
imp	'WahCloseSocketHandle'					WahCloseSocketHandle					ws2_32		169
imp	'WahCloseThread'					WahCloseThread						ws2_32		170
imp	'WahCompleteRequest'					WahCompleteRequest					ws2_32		171
imp	'WahCreateHandleContextTable'				WahCreateHandleContextTable				ws2_32		172
imp	'WahCreateNotificationHandle'				WahCreateNotificationHandle				ws2_32		173
imp	'WahCreateSocketHandle'					WahCreateSocketHandle					ws2_32		174
imp	'WahDestroyHandleContextTable'				WahDestroyHandleContextTable				ws2_32		175
imp	'WahDisableNonIFSHandleSupport'				WahDisableNonIFSHandleSupport				ws2_32		176
imp	'WahEnableNonIFSHandleSupport'				WahEnableNonIFSHandleSupport				ws2_32		177
imp	'WahEnumerateHandleContexts'				WahEnumerateHandleContexts				ws2_32		178
imp	'WahInsertHandleContext'				WahInsertHandleContext					ws2_32		179
imp	'WahNotifyAllProcesses'					WahNotifyAllProcesses					ws2_32		180
imp	'WahOpenApcHelper'					WahOpenApcHelper					ws2_32		181
imp	'WahOpenCurrentThread'					WahOpenCurrentThread					ws2_32		182
imp	'WahOpenHandleHelper'					WahOpenHandleHelper					ws2_32		183
imp	'WahOpenNotificationHandleHelper'			WahOpenNotificationHandleHelper				ws2_32		184
imp	'WahQueueUserApc'					WahQueueUserApc						ws2_32		185
imp	'WahReferenceContextByHandle'				WahReferenceContextByHandle				ws2_32		186
imp	'WahRemoveHandleContext'				WahRemoveHandleContext					ws2_32		187
imp	'WahWaitForNotification'				WahWaitForNotification					ws2_32		188
imp	'WahWriteLSPEvent'					WahWriteLSPEvent					ws2_32		189
imp	'WaitCommEvent'						WaitCommEvent						kernel32	0		# KernelBase
imp	'WaitForDebugEvent'					WaitForDebugEvent					kernel32	0		# KernelBase
imp	'WaitForDebugEventEx'					WaitForDebugEventEx					KernelBase	1775
imp	'WaitForExplorerRestart'				WaitForExplorerRestartW					shell32		617
imp	'WaitForInputIdle'					WaitForInputIdle					user32		2495	2
imp	'WaitForMachinePolicyForegroundProcessingInternal'	WaitForMachinePolicyForegroundProcessingInternal	KernelBase	1776
imp	'WaitForMultipleObjects'				WaitForMultipleObjects					kernel32	0	4	# KernelBase
imp	'WaitForMultipleObjectsEx'				WaitForMultipleObjectsEx				kernel32	0	5	# KernelBase
imp	'WaitForRedirectionStartComplete'			WaitForRedirectionStartComplete				user32		2496
imp	'WaitForSingleObject'					WaitForSingleObject					kernel32	0	2	# KernelBase
imp	'WaitForSingleObjectEx'					WaitForSingleObjectEx					kernel32	0	3	# KernelBase
imp	'WaitForUserPolicyForegroundProcessingInternal'		WaitForUserPolicyForegroundProcessingInternal		KernelBase	1785
imp	'WaitMessage'						WaitMessage						user32		2497
imp	'WaitNamedPipeA'					WaitNamedPipeA						kernel32	1509	2
imp	'WaitNamedPipe'						WaitNamedPipeW						kernel32	0		# KernelBase
imp	'WaitOnAddress'						WaitOnAddress						KernelBase	1787
imp	'WaitServiceState'					WaitServiceState					advapi32	1827
imp	'WantArrows'						WantArrows						comdlg32	127
imp	'WerGetFlags'						WerGetFlags						kernel32	0		# KernelBase
imp	'WerGetFlagsWorker'					WerGetFlagsWorker					kernel32	1514
imp	'WerRegisterAdditionalProcess'				WerRegisterAdditionalProcess				KernelBase	1793
imp	'WerRegisterAppLocalDump'				WerRegisterAppLocalDump					KernelBase	1794
imp	'WerRegisterCustomMetadata'				WerRegisterCustomMetadata				KernelBase	1795
imp	'WerRegisterExcludedMemoryBlock'			WerRegisterExcludedMemoryBlock				KernelBase	1796
imp	'WerRegisterFile'					WerRegisterFile						kernel32	0		# KernelBase
imp	'WerRegisterFileWorker'					WerRegisterFileWorker					kernel32	1520
imp	'WerRegisterMemoryBlock'				WerRegisterMemoryBlock					kernel32	0		# KernelBase
imp	'WerRegisterMemoryBlockWorker'				WerRegisterMemoryBlockWorker				kernel32	1522
imp	'WerRegisterRuntimeExceptionModule'			WerRegisterRuntimeExceptionModule			kernel32	0		# KernelBase
imp	'WerRegisterRuntimeExceptionModuleWorker'		WerRegisterRuntimeExceptionModuleWorker			kernel32	1524
imp	'WerReportExceptionWorker'				WerReportExceptionWorker				ntdll		1679
imp	'WerReportSQMEvent'					WerReportSQMEvent					ntdll		1680
imp	'WerSetFlags'						WerSetFlags						kernel32	0		# KernelBase
imp	'WerSetFlagsWorker'					WerSetFlagsWorker					kernel32	1526
imp	'WerUnregisterAdditionalProcess'			WerUnregisterAdditionalProcess				KernelBase	1801
imp	'WerUnregisterAppLocalDump'				WerUnregisterAppLocalDump				KernelBase	1802
imp	'WerUnregisterCustomMetadata'				WerUnregisterCustomMetadata				KernelBase	1803
imp	'WerUnregisterExcludedMemoryBlock'			WerUnregisterExcludedMemoryBlock			KernelBase	1804
imp	'WerUnregisterFile'					WerUnregisterFile					kernel32	0		# KernelBase
imp	'WerUnregisterFileWorker'				WerUnregisterFileWorker					kernel32	1532
imp	'WerUnregisterMemoryBlock'				WerUnregisterMemoryBlock				kernel32	0		# KernelBase
imp	'WerUnregisterMemoryBlockWorker'			WerUnregisterMemoryBlockWorker				kernel32	1534
imp	'WerUnregisterRuntimeExceptionModule'			WerUnregisterRuntimeExceptionModule			kernel32	0		# KernelBase
imp	'WerUnregisterRuntimeExceptionModuleWorker'		WerUnregisterRuntimeExceptionModuleWorker		kernel32	1536
imp	'WerpGetDebugger'					WerpGetDebugger						kernel32	1537
imp	'WerpInitiateRemoteRecovery'				WerpInitiateRemoteRecovery				kernel32	1538
imp	'WerpLaunchAeDebug'					WerpLaunchAeDebug					kernel32	1539
imp	'WerpNotifyLoadStringResource'				WerpNotifyLoadStringResource				KernelBase	1808
imp	'WerpNotifyLoadStringResourceWorker'			WerpNotifyLoadStringResourceWorker			kernel32	1540
imp	'WerpNotifyUseStringResource'				WerpNotifyUseStringResource				KernelBase	1809
imp	'WerpNotifyUseStringResourceWorker'			WerpNotifyUseStringResourceWorker			kernel32	1541
imp	'WideCharToMultiByte'					WideCharToMultiByte					kernel32	0		# KernelBase
imp	'WidenPath'						WidenPath						gdi32		1941
imp	'Win32DeleteFile'					Win32DeleteFile						shell32		164
imp	'WinExec'						WinExec							kernel32	1543
imp	'WinHelpA'						WinHelpA						user32		2498
imp	'WinHelp'						WinHelpW						user32		2499
imp	'WinSqmAddToAverageDWORD'				WinSqmAddToAverageDWORD					ntdll		1681
imp	'WinSqmAddToStream'					WinSqmAddToStream					ntdll		1682
imp	'WinSqmAddToStreamEx'					WinSqmAddToStreamEx					ntdll		1683
imp	'WinSqmCheckEscalationAddToStreamEx'			WinSqmCheckEscalationAddToStreamEx			ntdll		1684
imp	'WinSqmCheckEscalationSetDWORD'				WinSqmCheckEscalationSetDWORD				ntdll		1685
imp	'WinSqmCheckEscalationSetDWORD64'			WinSqmCheckEscalationSetDWORD64				ntdll		1686
imp	'WinSqmCheckEscalationSetString'			WinSqmCheckEscalationSetString				ntdll		1687
imp	'WinSqmCommonDatapointDelete'				WinSqmCommonDatapointDelete				ntdll		1688
imp	'WinSqmCommonDatapointSetDWORD'				WinSqmCommonDatapointSetDWORD				ntdll		1689
imp	'WinSqmCommonDatapointSetDWORD64'			WinSqmCommonDatapointSetDWORD64				ntdll		1690
imp	'WinSqmCommonDatapointSetStreamEx'			WinSqmCommonDatapointSetStreamEx			ntdll		1691
imp	'WinSqmCommonDatapointSetString'			WinSqmCommonDatapointSetString				ntdll		1692
imp	'WinSqmEndSession'					WinSqmEndSession					ntdll		1693
imp	'WinSqmEventEnabled'					WinSqmEventEnabled					ntdll		1694
imp	'WinSqmEventWrite'					WinSqmEventWrite					ntdll		1695
imp	'WinSqmGetEscalationRuleStatus'				WinSqmGetEscalationRuleStatus				ntdll		1696
imp	'WinSqmGetInstrumentationProperty'			WinSqmGetInstrumentationProperty			ntdll		1697
imp	'WinSqmIncrementDWORD'					WinSqmIncrementDWORD					ntdll		1698
imp	'WinSqmIsOptedIn'					WinSqmIsOptedIn						ntdll		1699
imp	'WinSqmIsOptedInEx'					WinSqmIsOptedInEx					ntdll		1700
imp	'WinSqmIsSessionDisabled'				WinSqmIsSessionDisabled					ntdll		1701
imp	'WinSqmSetDWORD'					WinSqmSetDWORD						ntdll		1702
imp	'WinSqmSetDWORD64'					WinSqmSetDWORD64					ntdll		1703
imp	'WinSqmSetEscalationInfo'				WinSqmSetEscalationInfo					ntdll		1704
imp	'WinSqmSetIfMaxDWORD'					WinSqmSetIfMaxDWORD					ntdll		1705
imp	'WinSqmSetIfMinDWORD'					WinSqmSetIfMinDWORD					ntdll		1706
imp	'WinSqmSetString'					WinSqmSetString						ntdll		1707
imp	'WinSqmStartSession'					WinSqmStartSession					ntdll		1708
imp	'WinSqmStartSessionForPartner'				WinSqmStartSessionForPartner				ntdll		1709
imp	'WinSqmStartSqmOptinListener'				WinSqmStartSqmOptinListener				ntdll		1710
imp	'WindowFromDC'						WindowFromDC						user32		2500
imp	'WindowFromPhysicalPoint'				WindowFromPhysicalPoint					user32		2501
imp	'WindowFromPoint'					WindowFromPoint						user32		2502
imp	'WmiCloseBlock'						WmiCloseBlock						advapi32	1828
imp	'WmiDevInstToInstanceNameA'				WmiDevInstToInstanceNameA				advapi32	1829
imp	'WmiDevInstToInstanceName'				WmiDevInstToInstanceNameW				advapi32	1830
imp	'WmiEnumerateGuids'					WmiEnumerateGuids					advapi32	1831
imp	'WmiExecuteMethodA'					WmiExecuteMethodA					advapi32	1832
imp	'WmiExecuteMethod'					WmiExecuteMethodW					advapi32	1833
imp	'WmiFileHandleToInstanceNameA'				WmiFileHandleToInstanceNameA				advapi32	1834
imp	'WmiFileHandleToInstanceName'				WmiFileHandleToInstanceNameW				advapi32	1835
imp	'WmiFreeBuffer'						WmiFreeBuffer						advapi32	1836
imp	'WmiMofEnumerateResourcesA'				WmiMofEnumerateResourcesA				advapi32	1837
imp	'WmiMofEnumerateResources'				WmiMofEnumerateResourcesW				advapi32	1838
imp	'WmiNotificationRegistrationA'				WmiNotificationRegistrationA				advapi32	1839
imp	'WmiNotificationRegistration'				WmiNotificationRegistrationW				advapi32	1840
imp	'WmiOpenBlock'						WmiOpenBlock						advapi32	1841
imp	'WmiQueryAllDataA'					WmiQueryAllDataA					advapi32	1842
imp	'WmiQueryAllDataMultipleA'				WmiQueryAllDataMultipleA				advapi32	1843
imp	'WmiQueryAllDataMultiple'				WmiQueryAllDataMultipleW				advapi32	1844
imp	'WmiQueryAllData'					WmiQueryAllDataW					advapi32	1845
imp	'WmiQueryGuidInformation'				WmiQueryGuidInformation					advapi32	1846
imp	'WmiQuerySingleInstanceA'				WmiQuerySingleInstanceA					advapi32	1847
imp	'WmiQuerySingleInstanceMultipleA'			WmiQuerySingleInstanceMultipleA				advapi32	1848
imp	'WmiQuerySingleInstanceMultiple'			WmiQuerySingleInstanceMultipleW				advapi32	1849
imp	'WmiQuerySingleInstance'				WmiQuerySingleInstanceW					advapi32	1850
imp	'WmiReceiveNotificationsA'				WmiReceiveNotificationsA				advapi32	1851
imp	'WmiReceiveNotifications'				WmiReceiveNotificationsW				advapi32	1852
imp	'WmiSetSingleInstanceA'					WmiSetSingleInstanceA					advapi32	1853
imp	'WmiSetSingleInstance'					WmiSetSingleInstanceW					advapi32	1854
imp	'WmiSetSingleItemA'					WmiSetSingleItemA					advapi32	1855
imp	'WmiSetSingleItem'					WmiSetSingleItemW					advapi32	1856
imp	'Wow64DisableWow64FsRedirection'			Wow64DisableWow64FsRedirection				kernel32	0		# KernelBase
imp	'Wow64EnableWow64FsRedirection'				Wow64EnableWow64FsRedirection				kernel32	1545
imp	'Wow64GetThreadContext'					Wow64GetThreadContext					kernel32	1546
imp	'Wow64GetThreadSelectorEntry'				Wow64GetThreadSelectorEntry				kernel32	1547
imp	'Wow64RevertWow64FsRedirection'				Wow64RevertWow64FsRedirection				kernel32	0		# KernelBase
imp	'Wow64SetThreadContext'					Wow64SetThreadContext					kernel32	1549
imp	'Wow64SetThreadDefaultGuestMachine'			Wow64SetThreadDefaultGuestMachine			KernelBase	1813
imp	'Wow64SuspendThread'					Wow64SuspendThread					kernel32	1550
imp	'WriteCabinetState'					WriteCabinetState					shell32		652
imp	'WriteConsole'						WriteConsoleW						kernel32	0	5	# KernelBase
imp	'WriteConsoleA'						WriteConsoleA						kernel32	0	5	# KernelBase
imp	'WriteConsoleInput'					WriteConsoleInputW					kernel32	0	4	# KernelBase
imp	'WriteConsoleInputA'					WriteConsoleInputA					kernel32	0	4	# KernelBase
imp	'WriteConsoleInputVDMA'					WriteConsoleInputVDMA					kernel32	1553
imp	'WriteConsoleInputVDMW'					WriteConsoleInputVDMW					kernel32	1554
imp	'WriteConsoleOutput'					WriteConsoleOutputW					kernel32	0		# KernelBase
imp	'WriteConsoleOutputA'					WriteConsoleOutputA					kernel32	0		# KernelBase
imp	'WriteConsoleOutputAttribute'				WriteConsoleOutputAttribute				kernel32	0	5	# KernelBase
imp	'WriteConsoleOutputCharacter'				WriteConsoleOutputCharacterW				kernel32	0	5	# KernelBase
imp	'WriteConsoleOutputCharacterA'				WriteConsoleOutputCharacterA				kernel32	0	5	# KernelBase
imp	'WriteEncryptedFileRaw'					WriteEncryptedFileRaw					advapi32	1857
imp	'WriteFile'						WriteFile						kernel32	0	5	# KernelBase
imp	'WriteFileEx'						WriteFileEx						kernel32	0	5	# KernelBase
imp	'WriteFileGather'					WriteFileGather						kernel32	0	5	# KernelBase
imp	'WritePrivateProfileSectionA'				WritePrivateProfileSectionA				kernel32	1565
imp	'WritePrivateProfileSection'				WritePrivateProfileSectionW				kernel32	1566
imp	'WritePrivateProfileStringA'				WritePrivateProfileStringA				kernel32	1567
imp	'WritePrivateProfileString'				WritePrivateProfileStringW				kernel32	1568
imp	'WritePrivateProfileStructA'				WritePrivateProfileStructA				kernel32	1569
imp	'WritePrivateProfileStruct'				WritePrivateProfileStructW				kernel32	1570
imp	'WriteProcessMemory'					WriteProcessMemory					kernel32	0		# KernelBase
imp	'WriteProfileSectionA'					WriteProfileSectionA					kernel32	1572
imp	'WriteProfileSection'					WriteProfileSectionW					kernel32	1573
imp	'WriteProfileStringA'					WriteProfileStringA					kernel32	1574
imp	'WriteProfileString'					WriteProfileStringW					kernel32	1575
imp	'WriteStateAtomValue'					WriteStateAtomValue					KernelBase	1827
imp	'WriteStateContainerValue'				WriteStateContainerValue				KernelBase	1828
imp	'WriteTapemark'						WriteTapemark						kernel32	1576
imp	'XFORMOBJ_bApplyXform'					XFORMOBJ_bApplyXform					gdi32		1942
imp	'XFORMOBJ_iGetXform'					XFORMOBJ_iGetXform					gdi32		1943
imp	'XLATEOBJ_cGetPalette'					XLATEOBJ_cGetPalette					gdi32		1944
imp	'XLATEOBJ_hGetColorTransform'				XLATEOBJ_hGetColorTransform				gdi32		1945
imp	'XLATEOBJ_iXlate'					XLATEOBJ_iXlate						gdi32		1946
imp	'XLATEOBJ_piVector'					XLATEOBJ_piVector					gdi32		1947
imp	'ZombifyActCtx'						ZombifyActCtx						kernel32	0		# KernelBase
imp	'ZombifyActCtxWorker'					ZombifyActCtxWorker					kernel32	1578
imp	'ZwAcceptConnectPort'					ZwAcceptConnectPort					ntdll		1711
imp	'ZwAccessCheck'						ZwAccessCheck						ntdll		1712
imp	'ZwAccessCheckAndAuditAlarm'				ZwAccessCheckAndAuditAlarm				ntdll		1713
imp	'ZwAccessCheckByType'					ZwAccessCheckByType					ntdll		1714
imp	'ZwAccessCheckByTypeAndAuditAlarm'			ZwAccessCheckByTypeAndAuditAlarm			ntdll		1715
imp	'ZwAccessCheckByTypeResultList'				ZwAccessCheckByTypeResultList				ntdll		1716
imp	'ZwAccessCheckByTypeResultListAndAuditAlarm'		ZwAccessCheckByTypeResultListAndAuditAlarm		ntdll		1717
imp	'ZwAccessCheckByTypeResultListAndAuditAlarmByHandle'	ZwAccessCheckByTypeResultListAndAuditAlarmByHandle	ntdll		1718
imp	'ZwAcquireProcessActivityReference'			ZwAcquireProcessActivityReference			ntdll		1719
imp	'ZwAddAtom'						ZwAddAtom						ntdll		1720
imp	'ZwAddAtomEx'						ZwAddAtomEx						ntdll		1721
imp	'ZwAddBootEntry'					ZwAddBootEntry						ntdll		1722
imp	'ZwAddDriverEntry'					ZwAddDriverEntry					ntdll		1723
imp	'ZwAdjustGroupsToken'					ZwAdjustGroupsToken					ntdll		1724
imp	'ZwAdjustPrivilegesToken'				ZwAdjustPrivilegesToken					ntdll		1725
imp	'ZwAdjustTokenClaimsAndDeviceGroups'			ZwAdjustTokenClaimsAndDeviceGroups			ntdll		1726
imp	'ZwAlertResumeThread'					ZwAlertResumeThread					ntdll		1727
imp	'ZwAlertThread'						ZwAlertThread						ntdll		1728
imp	'ZwAlertThreadByThreadId'				ZwAlertThreadByThreadId					ntdll		1729
imp	'ZwAllocateLocallyUniqueId'				ZwAllocateLocallyUniqueId				ntdll		1730
imp	'ZwAllocateReserveObject'				ZwAllocateReserveObject					ntdll		1731
imp	'ZwAllocateUserPhysicalPages'				ZwAllocateUserPhysicalPages				ntdll		1732
imp	'ZwAllocateUuids'					ZwAllocateUuids						ntdll		1733
imp	'ZwAllocateVirtualMemory'				ZwAllocateVirtualMemory					ntdll		1734
imp	'ZwAllocateVirtualMemoryEx'				ZwAllocateVirtualMemoryEx				ntdll		1735
imp	'ZwAlpcAcceptConnectPort'				ZwAlpcAcceptConnectPort					ntdll		1736
imp	'ZwAlpcCancelMessage'					ZwAlpcCancelMessage					ntdll		1737
imp	'ZwAlpcConnectPort'					ZwAlpcConnectPort					ntdll		1738
imp	'ZwAlpcConnectPortEx'					ZwAlpcConnectPortEx					ntdll		1739
imp	'ZwAlpcCreatePort'					ZwAlpcCreatePort					ntdll		1740
imp	'ZwAlpcCreatePortSection'				ZwAlpcCreatePortSection					ntdll		1741
imp	'ZwAlpcCreateResourceReserve'				ZwAlpcCreateResourceReserve				ntdll		1742
imp	'ZwAlpcCreateSectionView'				ZwAlpcCreateSectionView					ntdll		1743
imp	'ZwAlpcCreateSecurityContext'				ZwAlpcCreateSecurityContext				ntdll		1744
imp	'ZwAlpcDeletePortSection'				ZwAlpcDeletePortSection					ntdll		1745
imp	'ZwAlpcDeleteResourceReserve'				ZwAlpcDeleteResourceReserve				ntdll		1746
imp	'ZwAlpcDeleteSectionView'				ZwAlpcDeleteSectionView					ntdll		1747
imp	'ZwAlpcDeleteSecurityContext'				ZwAlpcDeleteSecurityContext				ntdll		1748
imp	'ZwAlpcDisconnectPort'					ZwAlpcDisconnectPort					ntdll		1749
imp	'ZwAlpcImpersonateClientContainerOfPort'		ZwAlpcImpersonateClientContainerOfPort			ntdll		1750
imp	'ZwAlpcImpersonateClientOfPort'				ZwAlpcImpersonateClientOfPort				ntdll		1751
imp	'ZwAlpcOpenSenderProcess'				ZwAlpcOpenSenderProcess					ntdll		1752
imp	'ZwAlpcOpenSenderThread'				ZwAlpcOpenSenderThread					ntdll		1753
imp	'ZwAlpcQueryInformation'				ZwAlpcQueryInformation					ntdll		1754
imp	'ZwAlpcQueryInformationMessage'				ZwAlpcQueryInformationMessage				ntdll		1755
imp	'ZwAlpcRevokeSecurityContext'				ZwAlpcRevokeSecurityContext				ntdll		1756
imp	'ZwAlpcSendWaitReceivePort'				ZwAlpcSendWaitReceivePort				ntdll		1757
imp	'ZwAlpcSetInformation'					ZwAlpcSetInformation					ntdll		1758
imp	'ZwApphelpCacheControl'					ZwApphelpCacheControl					ntdll		1759
imp	'ZwAreMappedFilesTheSame'				ZwAreMappedFilesTheSame					ntdll		1760	2
imp	'ZwAssignProcessToJobObject'				ZwAssignProcessToJobObject				ntdll		1761
imp	'ZwAssociateWaitCompletionPacket'			ZwAssociateWaitCompletionPacket				ntdll		1762
imp	'ZwCallEnclave'						ZwCallEnclave						ntdll		1763
imp	'ZwCallbackReturn'					ZwCallbackReturn					ntdll		1764
imp	'ZwCancelIoFile'					ZwCancelIoFile						ntdll		1765
imp	'ZwCancelIoFileEx'					ZwCancelIoFileEx					ntdll		1766
imp	'ZwCancelSynchronousIoFile'				ZwCancelSynchronousIoFile				ntdll		1767
imp	'ZwCancelTimer'						ZwCancelTimer						ntdll		1768
imp	'ZwCancelTimer2'					ZwCancelTimer2						ntdll		1769
imp	'ZwCancelWaitCompletionPacket'				ZwCancelWaitCompletionPacket				ntdll		1770
imp	'ZwClearEvent'						ZwClearEvent						ntdll		1771
imp	'ZwClose'						ZwClose							ntdll		1772
imp	'ZwCloseObjectAuditAlarm'				ZwCloseObjectAuditAlarm					ntdll		1773
imp	'ZwCommitComplete'					ZwCommitComplete					ntdll		1774
imp	'ZwCommitEnlistment'					ZwCommitEnlistment					ntdll		1775
imp	'ZwCommitRegistryTransaction'				ZwCommitRegistryTransaction				ntdll		1776
imp	'ZwCommitTransaction'					ZwCommitTransaction					ntdll		1777
imp	'ZwCompactKeys'						ZwCompactKeys						ntdll		1778
imp	'ZwCompareObjects'					ZwCompareObjects					ntdll		1779
imp	'ZwCompareSigningLevels'				ZwCompareSigningLevels					ntdll		1780
imp	'ZwCompareTokens'					ZwCompareTokens						ntdll		1781
imp	'ZwCompleteConnectPort'					ZwCompleteConnectPort					ntdll		1782
imp	'ZwCompressKey'						ZwCompressKey						ntdll		1783
imp	'ZwConnectPort'						ZwConnectPort						ntdll		1784
imp	'ZwContinue'						ZwContinue						ntdll		1785
imp	'ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter'	ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter	ntdll		1786
imp	'ZwCreateDebugObject'					ZwCreateDebugObject					ntdll		1787
imp	'ZwCreateDirectoryObject'				ZwCreateDirectoryObject					ntdll		1788
imp	'ZwCreateDirectoryObjectEx'				ZwCreateDirectoryObjectEx				ntdll		1789
imp	'ZwCreateEnclave'					ZwCreateEnclave						ntdll		1790
imp	'ZwCreateEnlistment'					ZwCreateEnlistment					ntdll		1791
imp	'ZwCreateEvent'						ZwCreateEvent						ntdll		1792
imp	'ZwCreateEventPair'					ZwCreateEventPair					ntdll		1793
imp	'ZwCreateFile'						ZwCreateFile						ntdll		1794
imp	'ZwCreateIRTimer'					ZwCreateIRTimer						ntdll		1795
imp	'ZwCreateIoCompletion'					ZwCreateIoCompletion					ntdll		1796
imp	'ZwCreateJobObject'					ZwCreateJobObject					ntdll		1797
imp	'ZwCreateJobSet'					ZwCreateJobSet						ntdll		1798
imp	'ZwCreateKey'						ZwCreateKey						ntdll		1799
imp	'ZwCreateKeyTransacted'					ZwCreateKeyTransacted					ntdll		1800
imp	'ZwCreateKeyedEvent'					ZwCreateKeyedEvent					ntdll		1801
imp	'ZwCreateLowBoxToken'					ZwCreateLowBoxToken					ntdll		1802
imp	'ZwCreateMailslotFile'					ZwCreateMailslotFile					ntdll		1803
imp	'ZwCreateMutant'					ZwCreateMutant						ntdll		1804
imp	'ZwCreateNamedPipeFile'					ZwCreateNamedPipeFile					ntdll		1805
imp	'ZwCreatePagingFile'					ZwCreatePagingFile					ntdll		1806
imp	'ZwCreatePartition'					ZwCreatePartition					ntdll		1807
imp	'ZwCreatePort'						ZwCreatePort						ntdll		1808
imp	'ZwCreatePrivateNamespace'				ZwCreatePrivateNamespace				ntdll		1809
imp	'ZwCreateProcess'					ZwCreateProcess						ntdll		1810
imp	'ZwCreateProcessEx'					ZwCreateProcessEx					ntdll		1811
imp	'ZwCreateProfile'					ZwCreateProfile						ntdll		1812
imp	'ZwCreateProfileEx'					ZwCreateProfileEx					ntdll		1813
imp	'ZwCreateRegistryTransaction'				ZwCreateRegistryTransaction				ntdll		1814
imp	'ZwCreateResourceManager'				ZwCreateResourceManager					ntdll		1815
imp	'ZwCreateSection'					ZwCreateSection						ntdll		1816
imp	'ZwCreateSemaphore'					ZwCreateSemaphore					ntdll		1817
imp	'ZwCreateSymbolicLinkObject'				ZwCreateSymbolicLinkObject				ntdll		1818
imp	'ZwCreateThread'					ZwCreateThread						ntdll		1819
imp	'ZwCreateThreadEx'					ZwCreateThreadEx					ntdll		1820
imp	'ZwCreateTimer'						ZwCreateTimer						ntdll		1821
imp	'ZwCreateTimer2'					ZwCreateTimer2						ntdll		1822
imp	'ZwCreateToken'						ZwCreateToken						ntdll		1823
imp	'ZwCreateTokenEx'					ZwCreateTokenEx						ntdll		1824
imp	'ZwCreateTransaction'					ZwCreateTransaction					ntdll		1825
imp	'ZwCreateTransactionManager'				ZwCreateTransactionManager				ntdll		1826
imp	'ZwCreateUserProcess'					ZwCreateUserProcess					ntdll		1827
imp	'ZwCreateWaitCompletionPacket'				ZwCreateWaitCompletionPacket				ntdll		1828
imp	'ZwCreateWaitablePort'					ZwCreateWaitablePort					ntdll		1829
imp	'ZwCreateWnfStateName'					ZwCreateWnfStateName					ntdll		1830
imp	'ZwCreateWorkerFactory'					ZwCreateWorkerFactory					ntdll		1831
imp	'ZwDebugActiveProcess'					ZwDebugActiveProcess					ntdll		1832
imp	'ZwDebugContinue'					ZwDebugContinue						ntdll		1833
imp	'ZwDelayExecution'					ZwDelayExecution					ntdll		1834
imp	'ZwDeleteAtom'						ZwDeleteAtom						ntdll		1835
imp	'ZwDeleteBootEntry'					ZwDeleteBootEntry					ntdll		1836
imp	'ZwDeleteDriverEntry'					ZwDeleteDriverEntry					ntdll		1837
imp	'ZwDeleteFile'						ZwDeleteFile						ntdll		1838
imp	'ZwDeleteKey'						ZwDeleteKey						ntdll		1839
imp	'ZwDeleteObjectAuditAlarm'				ZwDeleteObjectAuditAlarm				ntdll		1840
imp	'ZwDeletePrivateNamespace'				ZwDeletePrivateNamespace				ntdll		1841
imp	'ZwDeleteValueKey'					ZwDeleteValueKey					ntdll		1842
imp	'ZwDeleteWnfStateData'					ZwDeleteWnfStateData					ntdll		1843
imp	'ZwDeleteWnfStateName'					ZwDeleteWnfStateName					ntdll		1844
imp	'ZwDeviceIoControlFile'					ZwDeviceIoControlFile					ntdll		1845
imp	'ZwDisableLastKnownGood'				ZwDisableLastKnownGood					ntdll		1846
imp	'ZwDisplayString'					ZwDisplayString						ntdll		1847
imp	'ZwDrawText'						ZwDrawText						ntdll		1848
imp	'ZwDuplicateObject'					ZwDuplicateObject					ntdll		1849
imp	'ZwDuplicateToken'					ZwDuplicateToken					ntdll		1850
imp	'ZwEnableLastKnownGood'					ZwEnableLastKnownGood					ntdll		1851
imp	'ZwEnumerateBootEntries'				ZwEnumerateBootEntries					ntdll		1852
imp	'ZwEnumerateDriverEntries'				ZwEnumerateDriverEntries				ntdll		1853
imp	'ZwEnumerateKey'					ZwEnumerateKey						ntdll		1854
imp	'ZwEnumerateSystemEnvironmentValuesEx'			ZwEnumerateSystemEnvironmentValuesEx			ntdll		1855
imp	'ZwEnumerateTransactionObject'				ZwEnumerateTransactionObject				ntdll		1856
imp	'ZwEnumerateValueKey'					ZwEnumerateValueKey					ntdll		1857
imp	'ZwExtendSection'					ZwExtendSection						ntdll		1858
imp	'ZwFilterBootOption'					ZwFilterBootOption					ntdll		1859
imp	'ZwFilterToken'						ZwFilterToken						ntdll		1860
imp	'ZwFilterTokenEx'					ZwFilterTokenEx						ntdll		1861
imp	'ZwFindAtom'						ZwFindAtom						ntdll		1862
imp	'ZwFlushBuffersFile'					ZwFlushBuffersFile					ntdll		1863
imp	'ZwFlushBuffersFileEx'					ZwFlushBuffersFileEx					ntdll		1864
imp	'ZwFlushInstallUILanguage'				ZwFlushInstallUILanguage				ntdll		1865
imp	'ZwFlushInstructionCache'				ZwFlushInstructionCache					ntdll		1866
imp	'ZwFlushKey'						ZwFlushKey						ntdll		1867
imp	'ZwFlushProcessWriteBuffers'				ZwFlushProcessWriteBuffers				ntdll		1868
imp	'ZwFlushVirtualMemory'					ZwFlushVirtualMemory					ntdll		1869
imp	'ZwFlushWriteBuffer'					ZwFlushWriteBuffer					ntdll		1870
imp	'ZwFreeUserPhysicalPages'				ZwFreeUserPhysicalPages					ntdll		1871
imp	'ZwFreeVirtualMemory'					ZwFreeVirtualMemory					ntdll		1872
imp	'ZwFreezeRegistry'					ZwFreezeRegistry					ntdll		1873
imp	'ZwFreezeTransactions'					ZwFreezeTransactions					ntdll		1874
imp	'ZwFsControlFile'					ZwFsControlFile						ntdll		1875
imp	'ZwGetCachedSigningLevel'				ZwGetCachedSigningLevel					ntdll		1876
imp	'ZwGetCompleteWnfStateSubscription'			ZwGetCompleteWnfStateSubscription			ntdll		1877
imp	'ZwGetContextThread'					ZwGetContextThread					ntdll		1878
imp	'ZwGetCurrentProcessorNumber'				ZwGetCurrentProcessorNumber				ntdll		1879
imp	'ZwGetCurrentProcessorNumberEx'				ZwGetCurrentProcessorNumberEx				ntdll		1880
imp	'ZwGetDevicePowerState'					ZwGetDevicePowerState					ntdll		1881
imp	'ZwGetMUIRegistryInfo'					ZwGetMUIRegistryInfo					ntdll		1882
imp	'ZwGetNextProcess'					ZwGetNextProcess					ntdll		1883
imp	'ZwGetNextThread'					ZwGetNextThread						ntdll		1884
imp	'ZwGetNlsSectionPtr'					ZwGetNlsSectionPtr					ntdll		1885
imp	'ZwGetNotificationResourceManager'			ZwGetNotificationResourceManager			ntdll		1886
imp	'ZwGetWriteWatch'					ZwGetWriteWatch						ntdll		1887
imp	'ZwImpersonateAnonymousToken'				ZwImpersonateAnonymousToken				ntdll		1888
imp	'ZwImpersonateClientOfPort'				ZwImpersonateClientOfPort				ntdll		1889
imp	'ZwImpersonateThread'					ZwImpersonateThread					ntdll		1890
imp	'ZwInitializeEnclave'					ZwInitializeEnclave					ntdll		1891
imp	'ZwInitializeNlsFiles'					ZwInitializeNlsFiles					ntdll		1892
imp	'ZwInitializeRegistry'					ZwInitializeRegistry					ntdll		1893
imp	'ZwInitiatePowerAction'					ZwInitiatePowerAction					ntdll		1894
imp	'ZwIsProcessInJob'					ZwIsProcessInJob					ntdll		1895
imp	'ZwIsSystemResumeAutomatic'				ZwIsSystemResumeAutomatic				ntdll		1896
imp	'ZwIsUILanguageComitted'				ZwIsUILanguageComitted					ntdll		1897
imp	'ZwListenPort'						ZwListenPort						ntdll		1898
imp	'ZwLoadDriver'						ZwLoadDriver						ntdll		1899
imp	'ZwLoadEnclaveData'					ZwLoadEnclaveData					ntdll		1900
imp	'ZwLoadHotPatch'					ZwLoadHotPatch						ntdll		1901
imp	'ZwLoadKey'						ZwLoadKey						ntdll		1902
imp	'ZwLoadKey2'						ZwLoadKey2						ntdll		1903
imp	'ZwLoadKeyEx'						ZwLoadKeyEx						ntdll		1904
imp	'ZwLockFile'						ZwLockFile						ntdll		1905
imp	'ZwLockProductActivationKeys'				ZwLockProductActivationKeys				ntdll		1906
imp	'ZwLockRegistryKey'					ZwLockRegistryKey					ntdll		1907
imp	'ZwLockVirtualMemory'					ZwLockVirtualMemory					ntdll		1908
imp	'ZwMakePermanentObject'					ZwMakePermanentObject					ntdll		1909
imp	'ZwMakeTemporaryObject'					ZwMakeTemporaryObject					ntdll		1910
imp	'ZwManagePartition'					ZwManagePartition					ntdll		1911
imp	'ZwMapCMFModule'					ZwMapCMFModule						ntdll		1912
imp	'ZwMapUserPhysicalPages'				ZwMapUserPhysicalPages					ntdll		1913
imp	'ZwMapUserPhysicalPagesScatter'				ZwMapUserPhysicalPagesScatter				ntdll		1914
imp	'ZwMapViewOfSection'					ZwMapViewOfSection					ntdll		1915
imp	'ZwMapViewOfSectionEx'					ZwMapViewOfSectionEx					ntdll		1916
imp	'ZwModifyBootEntry'					ZwModifyBootEntry					ntdll		1917
imp	'ZwModifyDriverEntry'					ZwModifyDriverEntry					ntdll		1918
imp	'ZwNotifyChangeDirectoryFile'				ZwNotifyChangeDirectoryFile				ntdll		1919
imp	'ZwNotifyChangeDirectoryFileEx'				ZwNotifyChangeDirectoryFileEx				ntdll		1920
imp	'ZwNotifyChangeKey'					ZwNotifyChangeKey					ntdll		1921
imp	'ZwNotifyChangeMultipleKeys'				ZwNotifyChangeMultipleKeys				ntdll		1922
imp	'ZwNotifyChangeSession'					ZwNotifyChangeSession					ntdll		1923
imp	'ZwOpenDirectoryObject'					ZwOpenDirectoryObject					ntdll		1924
imp	'ZwOpenEnlistment'					ZwOpenEnlistment					ntdll		1925
imp	'ZwOpenEvent'						ZwOpenEvent						ntdll		1926
imp	'ZwOpenEventPair'					ZwOpenEventPair						ntdll		1927
imp	'ZwOpenFile'						ZwOpenFile						ntdll		1928
imp	'ZwOpenIoCompletion'					ZwOpenIoCompletion					ntdll		1929
imp	'ZwOpenJobObject'					ZwOpenJobObject						ntdll		1930
imp	'ZwOpenKey'						ZwOpenKey						ntdll		1931
imp	'ZwOpenKeyEx'						ZwOpenKeyEx						ntdll		1932
imp	'ZwOpenKeyTransacted'					ZwOpenKeyTransacted					ntdll		1933
imp	'ZwOpenKeyTransactedEx'					ZwOpenKeyTransactedEx					ntdll		1934
imp	'ZwOpenKeyedEvent'					ZwOpenKeyedEvent					ntdll		1935
imp	'ZwOpenMutant'						ZwOpenMutant						ntdll		1936
imp	'ZwOpenObjectAuditAlarm'				ZwOpenObjectAuditAlarm					ntdll		1937
imp	'ZwOpenPartition'					ZwOpenPartition						ntdll		1938
imp	'ZwOpenPrivateNamespace'				ZwOpenPrivateNamespace					ntdll		1939
imp	'ZwOpenProcess'						ZwOpenProcess						ntdll		1940
imp	'ZwOpenProcessToken'					ZwOpenProcessToken					ntdll		1941
imp	'ZwOpenProcessTokenEx'					ZwOpenProcessTokenEx					ntdll		1942
imp	'ZwOpenRegistryTransaction'				ZwOpenRegistryTransaction				ntdll		1943
imp	'ZwOpenResourceManager'					ZwOpenResourceManager					ntdll		1944
imp	'ZwOpenSection'						ZwOpenSection						ntdll		1945
imp	'ZwOpenSemaphore'					ZwOpenSemaphore						ntdll		1946
imp	'ZwOpenSession'						ZwOpenSession						ntdll		1947
imp	'ZwOpenSymbolicLinkObject'				ZwOpenSymbolicLinkObject				ntdll		1948
imp	'ZwOpenThread'						ZwOpenThread						ntdll		1949
imp	'ZwOpenThreadToken'					ZwOpenThreadToken					ntdll		1950
imp	'ZwOpenThreadTokenEx'					ZwOpenThreadTokenEx					ntdll		1951
imp	'ZwOpenTimer'						ZwOpenTimer						ntdll		1952
imp	'ZwOpenTransaction'					ZwOpenTransaction					ntdll		1953
imp	'ZwOpenTransactionManager'				ZwOpenTransactionManager				ntdll		1954
imp	'ZwPlugPlayControl'					ZwPlugPlayControl					ntdll		1955
imp	'ZwPowerInformation'					ZwPowerInformation					ntdll		1956
imp	'ZwPrePrepareComplete'					ZwPrePrepareComplete					ntdll		1957
imp	'ZwPrePrepareEnlistment'				ZwPrePrepareEnlistment					ntdll		1958
imp	'ZwPrepareComplete'					ZwPrepareComplete					ntdll		1959
imp	'ZwPrepareEnlistment'					ZwPrepareEnlistment					ntdll		1960
imp	'ZwPrivilegeCheck'					ZwPrivilegeCheck					ntdll		1961
imp	'ZwPrivilegeObjectAuditAlarm'				ZwPrivilegeObjectAuditAlarm				ntdll		1962
imp	'ZwPrivilegedServiceAuditAlarm'				ZwPrivilegedServiceAuditAlarm				ntdll		1963
imp	'ZwPropagationComplete'					ZwPropagationComplete					ntdll		1964
imp	'ZwPropagationFailed'					ZwPropagationFailed					ntdll		1965
imp	'ZwProtectVirtualMemory'				ZwProtectVirtualMemory					ntdll		1966
imp	'ZwPulseEvent'						ZwPulseEvent						ntdll		1967
imp	'ZwQueryAttributesFile'					ZwQueryAttributesFile					ntdll		1968
imp	'ZwQueryAuxiliaryCounterFrequency'			ZwQueryAuxiliaryCounterFrequency			ntdll		1969
imp	'ZwQueryBootEntryOrder'					ZwQueryBootEntryOrder					ntdll		1970
imp	'ZwQueryBootOptions'					ZwQueryBootOptions					ntdll		1971
imp	'ZwQueryDebugFilterState'				ZwQueryDebugFilterState					ntdll		1972
imp	'ZwQueryDefaultLocale'					ZwQueryDefaultLocale					ntdll		1973
imp	'ZwQueryDefaultUILanguage'				ZwQueryDefaultUILanguage				ntdll		1974
imp	'ZwQueryDirectoryFile'					ZwQueryDirectoryFile					ntdll		1975
imp	'ZwQueryDirectoryFileEx'				ZwQueryDirectoryFileEx					ntdll		1976
imp	'ZwQueryDirectoryObject'				ZwQueryDirectoryObject					ntdll		1977
imp	'ZwQueryDriverEntryOrder'				ZwQueryDriverEntryOrder					ntdll		1978
imp	'ZwQueryEaFile'						ZwQueryEaFile						ntdll		1979
imp	'ZwQueryEvent'						ZwQueryEvent						ntdll		1980
imp	'ZwQueryFullAttributesFile'				ZwQueryFullAttributesFile				ntdll		1981
imp	'ZwQueryInformationAtom'				ZwQueryInformationAtom					ntdll		1982
imp	'ZwQueryInformationByName'				ZwQueryInformationByName				ntdll		1983
imp	'ZwQueryInformationEnlistment'				ZwQueryInformationEnlistment				ntdll		1984
imp	'ZwQueryInformationFile'				ZwQueryInformationFile					ntdll		1985
imp	'ZwQueryInformationJobObject'				ZwQueryInformationJobObject				ntdll		1986
imp	'ZwQueryInformationPort'				ZwQueryInformationPort					ntdll		1987
imp	'ZwQueryInformationProcess'				ZwQueryInformationProcess				ntdll		1988
imp	'ZwQueryInformationResourceManager'			ZwQueryInformationResourceManager			ntdll		1989
imp	'ZwQueryInformationThread'				ZwQueryInformationThread				ntdll		1990
imp	'ZwQueryInformationToken'				ZwQueryInformationToken					ntdll		1991
imp	'ZwQueryInformationTransaction'				ZwQueryInformationTransaction				ntdll		1992
imp	'ZwQueryInformationTransactionManager'			ZwQueryInformationTransactionManager			ntdll		1993
imp	'ZwQueryInformationWorkerFactory'			ZwQueryInformationWorkerFactory				ntdll		1994
imp	'ZwQueryInstallUILanguage'				ZwQueryInstallUILanguage				ntdll		1995
imp	'ZwQueryIntervalProfile'				ZwQueryIntervalProfile					ntdll		1996
imp	'ZwQueryIoCompletion'					ZwQueryIoCompletion					ntdll		1997
imp	'ZwQueryKey'						ZwQueryKey						ntdll		1998
imp	'ZwQueryLicenseValue'					ZwQueryLicenseValue					ntdll		1999
imp	'ZwQueryMultipleValueKey'				ZwQueryMultipleValueKey					ntdll		2000
imp	'ZwQueryMutant'						ZwQueryMutant						ntdll		2001
imp	'ZwQueryObject'						ZwQueryObject						ntdll		2002
imp	'ZwQueryOpenSubKeys'					ZwQueryOpenSubKeys					ntdll		2003
imp	'ZwQueryOpenSubKeysEx'					ZwQueryOpenSubKeysEx					ntdll		2004
imp	'ZwQueryPerformanceCounter'				ZwQueryPerformanceCounter				ntdll		2005
imp	'ZwQueryPortInformationProcess'				ZwQueryPortInformationProcess				ntdll		2006
imp	'ZwQueryQuotaInformationFile'				ZwQueryQuotaInformationFile				ntdll		2007
imp	'ZwQuerySection'					ZwQuerySection						ntdll		2008
imp	'ZwQuerySecurityAttributesToken'			ZwQuerySecurityAttributesToken				ntdll		2009
imp	'ZwQuerySecurityObject'					ZwQuerySecurityObject					ntdll		2010
imp	'ZwQuerySecurityPolicy'					ZwQuerySecurityPolicy					ntdll		2011
imp	'ZwQuerySemaphore'					ZwQuerySemaphore					ntdll		2012
imp	'ZwQuerySymbolicLinkObject'				ZwQuerySymbolicLinkObject				ntdll		2013
imp	'ZwQuerySystemEnvironmentValue'				ZwQuerySystemEnvironmentValue				ntdll		2014
imp	'ZwQuerySystemEnvironmentValueEx'			ZwQuerySystemEnvironmentValueEx				ntdll		2015
imp	'ZwQuerySystemInformation'				ZwQuerySystemInformation				ntdll		2016
imp	'ZwQuerySystemInformationEx'				ZwQuerySystemInformationEx				ntdll		2017
imp	'ZwQuerySystemTime'					ZwQuerySystemTime					ntdll		2018
imp	'ZwQueryTimer'						ZwQueryTimer						ntdll		2019
imp	'ZwQueryTimerResolution'				ZwQueryTimerResolution					ntdll		2020
imp	'ZwQueryValueKey'					ZwQueryValueKey						ntdll		2021
imp	'ZwQueryVirtualMemory'					ZwQueryVirtualMemory					ntdll		2022
imp	'ZwQueryVolumeInformationFile'				ZwQueryVolumeInformationFile				ntdll		2023
imp	'ZwQueryWnfStateData'					ZwQueryWnfStateData					ntdll		2024
imp	'ZwQueryWnfStateNameInformation'			ZwQueryWnfStateNameInformation				ntdll		2025
imp	'ZwQueueApcThread'					ZwQueueApcThread					ntdll		2026
imp	'ZwQueueApcThreadEx'					ZwQueueApcThreadEx					ntdll		2027
imp	'ZwRaiseException'					ZwRaiseException					ntdll		2028
imp	'ZwRaiseHardError'					ZwRaiseHardError					ntdll		2029
imp	'ZwReadFile'						ZwReadFile						ntdll		2030
imp	'ZwReadFileScatter'					ZwReadFileScatter					ntdll		2031
imp	'ZwReadOnlyEnlistment'					ZwReadOnlyEnlistment					ntdll		2032
imp	'ZwReadRequestData'					ZwReadRequestData					ntdll		2033
imp	'ZwReadVirtualMemory'					ZwReadVirtualMemory					ntdll		2034
imp	'ZwRecoverEnlistment'					ZwRecoverEnlistment					ntdll		2035
imp	'ZwRecoverResourceManager'				ZwRecoverResourceManager				ntdll		2036
imp	'ZwRecoverTransactionManager'				ZwRecoverTransactionManager				ntdll		2037
imp	'ZwRegisterProtocolAddressInformation'			ZwRegisterProtocolAddressInformation			ntdll		2038
imp	'ZwRegisterThreadTerminatePort'				ZwRegisterThreadTerminatePort				ntdll		2039
imp	'ZwReleaseKeyedEvent'					ZwReleaseKeyedEvent					ntdll		2040
imp	'ZwReleaseMutant'					ZwReleaseMutant						ntdll		2041
imp	'ZwReleaseSemaphore'					ZwReleaseSemaphore					ntdll		2042
imp	'ZwReleaseWorkerFactoryWorker'				ZwReleaseWorkerFactoryWorker				ntdll		2043
imp	'ZwRemoveIoCompletion'					ZwRemoveIoCompletion					ntdll		2044
imp	'ZwRemoveIoCompletionEx'				ZwRemoveIoCompletionEx					ntdll		2045
imp	'ZwRemoveProcessDebug'					ZwRemoveProcessDebug					ntdll		2046
imp	'ZwRenameKey'						ZwRenameKey						ntdll		2047
imp	'ZwRenameTransactionManager'				ZwRenameTransactionManager				ntdll		2048
imp	'ZwReplaceKey'						ZwReplaceKey						ntdll		2049
imp	'ZwReplacePartitionUnit'				ZwReplacePartitionUnit					ntdll		2050
imp	'ZwReplyPort'						ZwReplyPort						ntdll		2051
imp	'ZwReplyWaitReceivePort'				ZwReplyWaitReceivePort					ntdll		2052
imp	'ZwReplyWaitReceivePortEx'				ZwReplyWaitReceivePortEx				ntdll		2053
imp	'ZwReplyWaitReplyPort'					ZwReplyWaitReplyPort					ntdll		2054
imp	'ZwRequestPort'						ZwRequestPort						ntdll		2055
imp	'ZwRequestWaitReplyPort'				ZwRequestWaitReplyPort					ntdll		2056
imp	'ZwResetEvent'						ZwResetEvent						ntdll		2057
imp	'ZwResetWriteWatch'					ZwResetWriteWatch					ntdll		2058
imp	'ZwRestoreKey'						ZwRestoreKey						ntdll		2059
imp	'ZwResumeProcess'					ZwResumeProcess						ntdll		2060
imp	'ZwResumeThread'					ZwResumeThread						ntdll		2061
imp	'ZwRevertContainerImpersonation'			ZwRevertContainerImpersonation				ntdll		2062
imp	'ZwRollbackComplete'					ZwRollbackComplete					ntdll		2063
imp	'ZwRollbackEnlistment'					ZwRollbackEnlistment					ntdll		2064
imp	'ZwRollbackRegistryTransaction'				ZwRollbackRegistryTransaction				ntdll		2065
imp	'ZwRollbackTransaction'					ZwRollbackTransaction					ntdll		2066
imp	'ZwRollforwardTransactionManager'			ZwRollforwardTransactionManager				ntdll		2067
imp	'ZwSaveKey'						ZwSaveKey						ntdll		2068
imp	'ZwSaveKeyEx'						ZwSaveKeyEx						ntdll		2069
imp	'ZwSaveMergedKeys'					ZwSaveMergedKeys					ntdll		2070
imp	'ZwSecureConnectPort'					ZwSecureConnectPort					ntdll		2071
imp	'ZwSerializeBoot'					ZwSerializeBoot						ntdll		2072
imp	'ZwSetBootEntryOrder'					ZwSetBootEntryOrder					ntdll		2073
imp	'ZwSetBootOptions'					ZwSetBootOptions					ntdll		2074
imp	'ZwSetCachedSigningLevel'				ZwSetCachedSigningLevel					ntdll		2075
imp	'ZwSetCachedSigningLevel2'				ZwSetCachedSigningLevel2				ntdll		2076
imp	'ZwSetContextThread'					ZwSetContextThread					ntdll		2077
imp	'ZwSetDebugFilterState'					ZwSetDebugFilterState					ntdll		2078
imp	'ZwSetDefaultHardErrorPort'				ZwSetDefaultHardErrorPort				ntdll		2079
imp	'ZwSetDefaultLocale'					ZwSetDefaultLocale					ntdll		2080
imp	'ZwSetDefaultUILanguage'				ZwSetDefaultUILanguage					ntdll		2081
imp	'ZwSetDriverEntryOrder'					ZwSetDriverEntryOrder					ntdll		2082
imp	'ZwSetEaFile'						ZwSetEaFile						ntdll		2083
imp	'ZwSetEvent'						ZwSetEvent						ntdll		2084
imp	'ZwSetEventBoostPriority'				ZwSetEventBoostPriority					ntdll		2085
imp	'ZwSetHighEventPair'					ZwSetHighEventPair					ntdll		2086
imp	'ZwSetHighWaitLowEventPair'				ZwSetHighWaitLowEventPair				ntdll		2087
imp	'ZwSetIRTimer'						ZwSetIRTimer						ntdll		2088
imp	'ZwSetInformationDebugObject'				ZwSetInformationDebugObject				ntdll		2089
imp	'ZwSetInformationEnlistment'				ZwSetInformationEnlistment				ntdll		2090
imp	'ZwSetInformationFile'					ZwSetInformationFile					ntdll		2091
imp	'ZwSetInformationJobObject'				ZwSetInformationJobObject				ntdll		2092
imp	'ZwSetInformationKey'					ZwSetInformationKey					ntdll		2093
imp	'ZwSetInformationObject'				ZwSetInformationObject					ntdll		2094
imp	'ZwSetInformationProcess'				ZwSetInformationProcess					ntdll		2095
imp	'ZwSetInformationResourceManager'			ZwSetInformationResourceManager				ntdll		2096
imp	'ZwSetInformationSymbolicLink'				ZwSetInformationSymbolicLink				ntdll		2097
imp	'ZwSetInformationThread'				ZwSetInformationThread					ntdll		2098
imp	'ZwSetInformationToken'					ZwSetInformationToken					ntdll		2099
imp	'ZwSetInformationTransaction'				ZwSetInformationTransaction				ntdll		2100
imp	'ZwSetInformationTransactionManager'			ZwSetInformationTransactionManager			ntdll		2101
imp	'ZwSetInformationVirtualMemory'				ZwSetInformationVirtualMemory				ntdll		2102
imp	'ZwSetInformationWorkerFactory'				ZwSetInformationWorkerFactory				ntdll		2103
imp	'ZwSetIntervalProfile'					ZwSetIntervalProfile					ntdll		2104
imp	'ZwSetIoCompletion'					ZwSetIoCompletion					ntdll		2105
imp	'ZwSetIoCompletionEx'					ZwSetIoCompletionEx					ntdll		2106
imp	'ZwSetLdtEntries'					ZwSetLdtEntries						ntdll		2107
imp	'ZwSetLowEventPair'					ZwSetLowEventPair					ntdll		2108
imp	'ZwSetLowWaitHighEventPair'				ZwSetLowWaitHighEventPair				ntdll		2109
imp	'ZwSetQuotaInformationFile'				ZwSetQuotaInformationFile				ntdll		2110
imp	'ZwSetSecurityObject'					ZwSetSecurityObject					ntdll		2111
imp	'ZwSetSystemEnvironmentValue'				ZwSetSystemEnvironmentValue				ntdll		2112
imp	'ZwSetSystemEnvironmentValueEx'				ZwSetSystemEnvironmentValueEx				ntdll		2113
imp	'ZwSetSystemInformation'				ZwSetSystemInformation					ntdll		2114
imp	'ZwSetSystemPowerState'					ZwSetSystemPowerState					ntdll		2115
imp	'ZwSetSystemTime'					ZwSetSystemTime						ntdll		2116
imp	'ZwSetThreadExecutionState'				ZwSetThreadExecutionState				ntdll		2117
imp	'ZwSetTimer'						ZwSetTimer						ntdll		2118
imp	'ZwSetTimer2'						ZwSetTimer2						ntdll		2119
imp	'ZwSetTimerEx'						ZwSetTimerEx						ntdll		2120
imp	'ZwSetTimerResolution'					ZwSetTimerResolution					ntdll		2121
imp	'ZwSetUuidSeed'						ZwSetUuidSeed						ntdll		2122
imp	'ZwSetValueKey'						ZwSetValueKey						ntdll		2123
imp	'ZwSetVolumeInformationFile'				ZwSetVolumeInformationFile				ntdll		2124
imp	'ZwSetWnfProcessNotificationEvent'			ZwSetWnfProcessNotificationEvent			ntdll		2125
imp	'ZwShutdownSystem'					ZwShutdownSystem					ntdll		2126
imp	'ZwShutdownWorkerFactory'				ZwShutdownWorkerFactory					ntdll		2127
imp	'ZwSignalAndWaitForSingleObject'			ZwSignalAndWaitForSingleObject				ntdll		2128
imp	'ZwSinglePhaseReject'					ZwSinglePhaseReject					ntdll		2129
imp	'ZwStartProfile'					ZwStartProfile						ntdll		2130
imp	'ZwStopProfile'						ZwStopProfile						ntdll		2131
imp	'ZwSubscribeWnfStateChange'				ZwSubscribeWnfStateChange				ntdll		2132
imp	'ZwSuspendProcess'					ZwSuspendProcess					ntdll		2133
imp	'ZwSuspendThread'					ZwSuspendThread						ntdll		2134
imp	'ZwSystemDebugControl'					ZwSystemDebugControl					ntdll		2135
imp	'ZwTerminateEnclave'					ZwTerminateEnclave					ntdll		2136
imp	'ZwTerminateJobObject'					ZwTerminateJobObject					ntdll		2137
imp	'ZwTerminateProcess'					ZwTerminateProcess					ntdll		2138
imp	'ZwTerminateThread'					ZwTerminateThread					ntdll		2139
imp	'ZwTestAlert'						ZwTestAlert						ntdll		2140
imp	'ZwThawRegistry'					ZwThawRegistry						ntdll		2141
imp	'ZwThawTransactions'					ZwThawTransactions					ntdll		2142
imp	'ZwTraceControl'					ZwTraceControl						ntdll		2143
imp	'ZwTraceEvent'						ZwTraceEvent						ntdll		2144
imp	'ZwTranslateFilePath'					ZwTranslateFilePath					ntdll		2145
imp	'ZwUmsThreadYield'					ZwUmsThreadYield					ntdll		2146
imp	'ZwUnloadDriver'					ZwUnloadDriver						ntdll		2147
imp	'ZwUnloadKey'						ZwUnloadKey						ntdll		2148
imp	'ZwUnloadKey2'						ZwUnloadKey2						ntdll		2149
imp	'ZwUnloadKeyEx'						ZwUnloadKeyEx						ntdll		2150
imp	'ZwUnlockFile'						ZwUnlockFile						ntdll		2151
imp	'ZwUnlockVirtualMemory'					ZwUnlockVirtualMemory					ntdll		2152
imp	'ZwUnmapViewOfSection'					ZwUnmapViewOfSection					ntdll		2153
imp	'ZwUnmapViewOfSectionEx'				ZwUnmapViewOfSectionEx					ntdll		2154
imp	'ZwUnsubscribeWnfStateChange'				ZwUnsubscribeWnfStateChange				ntdll		2155
imp	'ZwUpdateWnfStateData'					ZwUpdateWnfStateData					ntdll		2156
imp	'ZwVdmControl'						ZwVdmControl						ntdll		2157
imp	'ZwWaitForAlertByThreadId'				ZwWaitForAlertByThreadId				ntdll		2158
imp	'ZwWaitForDebugEvent'					ZwWaitForDebugEvent					ntdll		2159
imp	'ZwWaitForKeyedEvent'					ZwWaitForKeyedEvent					ntdll		2160
imp	'ZwWaitForMultipleObjects'				ZwWaitForMultipleObjects				ntdll		2161
imp	'ZwWaitForMultipleObjects32'				ZwWaitForMultipleObjects32				ntdll		2162
imp	'ZwWaitForSingleObject'					ZwWaitForSingleObject					ntdll		2163
imp	'ZwWaitForWorkViaWorkerFactory'				ZwWaitForWorkViaWorkerFactory				ntdll		2164
imp	'ZwWaitHighEventPair'					ZwWaitHighEventPair					ntdll		2165
imp	'ZwWaitLowEventPair'					ZwWaitLowEventPair					ntdll		2166
imp	'ZwWorkerFactoryWorkerReady'				ZwWorkerFactoryWorkerReady				ntdll		2167
imp	'ZwWriteFile'						ZwWriteFile						ntdll		2168
imp	'ZwWriteFileGather'					ZwWriteFileGather					ntdll		2169
imp	'ZwWriteRequestData'					ZwWriteRequestData					ntdll		2170
imp	'ZwWriteVirtualMemory'					ZwWriteVirtualMemory					ntdll		2171
imp	'ZwYieldExecution'					ZwYieldExecution					ntdll		2172
imp	'_AddMUIStringToCache'					_AddMUIStringToCache					KernelBase	1830
imp	'_GetMUIStringFromCache'				_GetMUIStringFromCache					KernelBase	1831
imp	'_OpenMuiStringCache'					_OpenMuiStringCache					KernelBase	1832
imp	'_UserTestTokenForInteractive'				_UserTestTokenForInteractive				user32		2543
imp	'_amsg_exit'						_amsg_exit						KernelBase	1838
imp	'_atoi64'						_atoi64							ntdll		2180
imp	'_c_exit'						_c_exit							KernelBase	1839
imp	'_cexit'						_cexit							KernelBase	1840
imp	'_errno'						_errno							ntdll		2181
imp	'_exit$nt'						_exit							KernelBase	1841
imp	'_fltused'						_fltused						ntdll		2182
imp	'_hread'						_hread							kernel32	1582
imp	'_hwrite'						_hwrite							kernel32	1583
imp	'_i64toa'						_i64toa							ntdll		2183
imp	'_i64toa_s'						_i64toa_s						ntdll		2184
imp	'_i64tow'						_i64tow							ntdll		2185
imp	'_i64tow_s'						_i64tow_s						ntdll		2186
imp	'_initterm'						_initterm						KernelBase	1842
imp	'_initterm_e'						_initterm_e						KernelBase	1843
imp	'_invalid_parameter'					_invalid_parameter					KernelBase	1844
imp	'_itoa'							_itoa							ntdll		2187
imp	'_itoa_s'						_itoa_s							ntdll		2188
imp	'_itow'							_itow							ntdll		2189
imp	'_itow_s'						_itow_s							ntdll		2190
imp	'_lclose'						_lclose							kernel32	1584
imp	'_lcreat'						_lcreat							kernel32	1585
imp	'_lfind'						_lfind							ntdll		2191
imp	'_llseek'						_llseek							kernel32	1586
imp	'_local_unwind'						_local_unwind						ntdll		2192
imp	'_lopen'						_lopen							kernel32	1588
imp	'_lread'						_lread							kernel32	1589
imp	'_ltoa'							_ltoa							ntdll		2193
imp	'_ltoa_s'						_ltoa_s							ntdll		2194
imp	'_ltow'							_ltow							ntdll		2195
imp	'_ltow_s'						_ltow_s							ntdll		2196
imp	'_lwrite'						_lwrite							kernel32	1590
imp	'_makepath_s'						_makepath_s						ntdll		2197
imp	'_memccpy'						_memccpy						ntdll		2198
imp	'_memicmp'						_memicmp						ntdll		2199
imp	'_onexit'						_onexit							KernelBase	1846
imp	'_purecall'						_purecall						KernelBase	1847
imp	'_setjmp$nt'						_setjmp							ntdll		2200
imp	'_setjmpex'						_setjmpex						ntdll		2201
imp	'_snprintf'						_snprintf						ntdll		2202
imp	'_snprintf_s'						_snprintf_s						ntdll		2203
imp	'_snscanf_s'						_snscanf_s						ntdll		2204
imp	'_snwprintf'						_snwprintf						ntdll		2205
imp	'_snwprintf_s'						_snwprintf_s						ntdll		2206
imp	'_snwscanf_s'						_snwscanf_s						ntdll		2207
imp	'_splitpath'						_splitpath						ntdll		2208
imp	'_splitpath_s'						_splitpath_s						ntdll		2209
imp	'_strcmpi'						_strcmpi						ntdll		2210
imp	'_stricmp'						_stricmp						ntdll		2211
imp	'_strlwr'						_strlwr							ntdll		2212
imp	'_strlwr_s'						_strlwr_s						ntdll		2213
imp	'_strnicmp'						_strnicmp						ntdll		2214
imp	'_strnset_s'						_strnset_s						ntdll		2215
imp	'_strset_s'						_strset_s						ntdll		2216
imp	'_strupr'						_strupr							ntdll		2217
imp	'_strupr_s'						_strupr_s						ntdll		2218
imp	'_swprintf'						_swprintf						ntdll		2219
imp	'_time64'						_time64							KernelBase	1848
imp	'_ui64toa'						_ui64toa						ntdll		2220
imp	'_ui64toa_s'						_ui64toa_s						ntdll		2221
imp	'_ui64tow'						_ui64tow						ntdll		2222
imp	'_ui64tow_s'						_ui64tow_s						ntdll		2223
imp	'_ultoa'						_ultoa							ntdll		2224
imp	'_ultoa_s'						_ultoa_s						ntdll		2225
imp	'_ultow'						_ultow							ntdll		2226
imp	'_ultow_s'						_ultow_s						ntdll		2227
imp	'_vscprintf'						_vscprintf						ntdll		2228
imp	'_vscwprintf'						_vscwprintf						ntdll		2229
imp	'_vsnprintf'						_vsnprintf						ntdll		2230
imp	'_vsnprintf_s'						_vsnprintf_s						ntdll		2231
imp	'_vsnwprintf'						_vsnwprintf						ntdll		2232
imp	'_vsnwprintf_s'						_vsnwprintf_s						ntdll		2233
imp	'_vswprintf'						_vswprintf						ntdll		2234
imp	'_wcsicmp'						_wcsicmp						ntdll		2235
imp	'_wcslwr'						_wcslwr							ntdll		2236
imp	'_wcslwr_s'						_wcslwr_s						ntdll		2237
imp	'_wcsnicmp'						_wcsnicmp						ntdll		2238
imp	'_wcsnset_s'						_wcsnset_s						ntdll		2239
imp	'_wcsset_s'						_wcsset_s						ntdll		2240
imp	'_wcstoi64'						_wcstoi64						ntdll		2241
imp	'_wcstoui64'						_wcstoui64						ntdll		2242
imp	'_wcsupr'						_wcsupr							ntdll		2243
imp	'_wcsupr_s'						_wcsupr_s						ntdll		2244
imp	'_wmakepath_s'						_wmakepath_s						ntdll		2245
imp	'_wsplitpath_s'						_wsplitpath_s						ntdll		2246
imp	'_wtoi'							_wtoi							ntdll		2247
imp	'_wtoi64'						_wtoi64							ntdll		2248
imp	'_wtol'							_wtol							ntdll		2249
imp	'abs$nt'						abs							ntdll		2250
imp	'__accept$nt'						accept							ws2_32		1
imp	'atan$nt'						atan							ntdll		2251
imp	'atan2$nt'						atan2							ntdll		2252
imp	'atexit$nt'						atexit							KernelBase	1849
imp	'atoi$nt'						atoi							ntdll		2253
imp	'atol$nt'						atol							ntdll		2254
imp	'bCreateDCW'						bCreateDCW						gdi32		1948
imp	'bDeleteLDC'						bDeleteLDC						gdi32		1949
imp	'bInitSystemAndFontsDirectories'			bInitSystemAndFontsDirectoriesW				gdi32		1950
imp	'bMakePathName'						bMakePathNameW						gdi32		1951
imp	'__bind$nt'						bind							ws2_32		2	3
imp	'bsearch$nt'						bsearch							ntdll		2255
imp	'bsearch_s'						bsearch_s						ntdll		2256
imp	'cGetTTFFromFOT'					cGetTTFFromFOT						gdi32		1952
imp	'ceil$nt'						ceil							ntdll		2257
imp	'__closesocket$nt'					closesocket						ws2_32		3	1
imp	'__connect$nt'						connect							ws2_32		4
imp	'cos$nt'						cos							ntdll		2258
imp	'dwLBSubclass'						dwLBSubclass						comdlg32	128
imp	'dwOKSubclass'						dwOKSubclass						comdlg32	129
imp	'exit$nt'						exit							KernelBase	1850
imp	'fabs$nt'						fabs							ntdll		2259
imp	'floor$nt'						floor							ntdll		2260
imp	'fpClosePrinter'					fpClosePrinter						gdi32		1953
imp	'freeaddrinfo$nt'					freeaddrinfo						ws2_32		190
imp	'gMaxGdiHandleCount'					gMaxGdiHandleCount					gdi32		1955
imp	'gSharedInfo'						gSharedInfo						user32		2547
imp	'gW32PID'						gW32PID							gdi32		1956
imp	'g_systemCallFilterId'					g_systemCallFilterId					gdi32		1957
imp	'gapfnScSendMessage'					gapfnScSendMessage					user32		2562
imp	'gdiPlaySpoolStream'					gdiPlaySpoolStream					gdi32		1958
imp	'getaddrinfo$nt'					getaddrinfo						ws2_32		191
imp	'gethostbyaddr$nt'					gethostbyaddr						ws2_32		51
imp	'gethostbyname$nt'					gethostbyname						ws2_32		52
imp	'gethostname$nt'					gethostname						ws2_32		57
imp	'getnameinfo$nt'					getnameinfo						ws2_32		192
imp	'__getpeername$nt'					getpeername						ws2_32		5	3
imp	'getprotobyname$nt'					getprotobyname						ws2_32		53
imp	'getprotobynumber$nt'					getprotobynumber					ws2_32		54
imp	'getservbyname$nt'					getservbyname						ws2_32		55
imp	'getservbyport$nt'					getservbyport						ws2_32		56
imp	'__getsockname$nt'					getsockname						ws2_32		6	3
imp	'__getsockopt$nt'					getsockopt						ws2_32		7	5
imp	'ghICM'							ghICM							gdi32		1959
imp	'hGetPEBHandle'						hGetPEBHandle						gdi32		1960
imp	'hgets'							hgets							KernelBase	1851
imp	'htonl$nt'						htonl							ws2_32		8
imp	'htons$nt'						htons							ws2_32		9
imp	'hwprintf'						hwprintf						KernelBase	1852
imp	'inet_addr$nt'						inet_addr						ws2_32		11
imp	'inet_ntoa$nt'						inet_ntoa						ws2_32		12
imp	'inet_ntop$nt'						inet_ntop						ws2_32		193
imp	'inet_pton$nt'						inet_pton						ws2_32		194
imp	'__ioctlsocket$nt'					ioctlsocket						ws2_32		10	3
imp	'isalnum$nt'						isalnum							ntdll		2261
imp	'isalpha$nt'						isalpha							ntdll		2262
imp	'iscntrl$nt'						iscntrl							ntdll		2263
imp	'isdigit$nt'						isdigit							ntdll		2264
imp	'isgraph$nt'						isgraph							ntdll		2265
imp	'islower$nt'						islower							ntdll		2266
imp	'isprint$nt'						isprint							ntdll		2267
imp	'ispunct$nt'						ispunct							ntdll		2268
imp	'isspace$nt'						isspace							ntdll		2269
imp	'isupper$nt'						isupper							ntdll		2270
imp	'iswalnum$nt'						iswalnum						ntdll		2271
imp	'iswalpha$nt'						iswalpha						ntdll		2272
imp	'iswascii'						iswascii						ntdll		2273
imp	'iswctype$nt'						iswctype						ntdll		2274
imp	'iswdigit$nt'						iswdigit						ntdll		2275
imp	'iswgraph$nt'						iswgraph						ntdll		2276
imp	'iswlower$nt'						iswlower						ntdll		2277
imp	'iswprint$nt'						iswprint						ntdll		2278
imp	'iswspace$nt'						iswspace						ntdll		2279
imp	'iswxdigit$nt'						iswxdigit						ntdll		2280
imp	'isxdigit$nt'						isxdigit						ntdll		2281
imp	'keybd_event'						keybd_event						user32		2580
imp	'labs$nt'						labs							ntdll		2282
imp	'__listen$nt'						listen							ws2_32		13	2
imp	'log$nt'						log							ntdll		2283
imp	'longjmp$nt'						longjmp							ntdll		2284
imp	'lstrcatA'						lstrcatA						kernel32	1592
imp	'lstrcat'						lstrcatW						kernel32	1593
imp	'lstrcmpA'						lstrcmpA						kernel32	0		# KernelBase
imp	'lstrcmp'						lstrcmpW						kernel32	0		# KernelBase
imp	'lstrcmpiA'						lstrcmpiA						kernel32	0		# KernelBase
imp	'lstrcmpi'						lstrcmpiW						kernel32	0		# KernelBase
imp	'lstrcpyA'						lstrcpyA						kernel32	1601
imp	'lstrcpy'						lstrcpyW						kernel32	1602
imp	'lstrcpynA'						lstrcpynA						kernel32	0		# KernelBase
imp	'lstrcpyn'						lstrcpynW						kernel32	0		# KernelBase
imp	'lstrlenA'						lstrlenA						kernel32	0		# KernelBase
imp	'lstrlen'						lstrlenW						kernel32	0		# KernelBase
imp	'mbstowcs$nt'						mbstowcs						ntdll		2285
imp	'memchr$nt'						memchr							ntdll		2286
imp	'memcmp$nt'						memcmp							ntdll		2287
imp	'memcpy$nt'						memcpy							ntdll		2288
imp	'memcpy_s'						memcpy_s						ntdll		2289
imp	'memmove$nt'						memmove							ntdll		2290
imp	'memmove_s'						memmove_s						ntdll		2291
imp	'memset$nt'						memset							ntdll		2292
imp	'mouse_event'						mouse_event						user32		2583
imp	'ntohl$nt'						ntohl							ws2_32		14
imp	'ntohs$nt'						ntohs							ws2_32		15
imp	'pGdiDevCaps'						pGdiDevCaps						gdi32		1961
imp	'pGdiSharedHandleTable'					pGdiSharedHandleTable					gdi32		1962
imp	'pGdiSharedMemory'					pGdiSharedMemory					gdi32		1963
imp	'pldcGet'						pldcGet							gdi32		1964
imp	'pow$nt'						pow							ntdll		2293
imp	'qsort$nt'						qsort							ntdll		2294
imp	'qsort_s$nt'						qsort_s							ntdll		2295
imp	'recv$nt'						recv							ws2_32		16
imp	'__recvfrom$nt'						recvfrom						ws2_32		17
imp	'__select$nt'						select							ws2_32		18	5
imp	'semDxTrimNotification'					semDxTrimNotification					gdi32		1965
imp	'send$nt'						send							ws2_32		19
imp	'__sendto$nt'						sendto							ws2_32		20
imp	'__setsockopt$nt'					setsockopt						ws2_32		21	5
imp	'__shutdown$nt'						shutdown						ws2_32		22	2
imp	'sin$nt'						sin							ntdll		2296
imp	'__socket$nt'						socket							ws2_32		23
imp	'sprintf$nt'						sprintf							ntdll		2297
imp	'sprintf_s$nt'						sprintf_s						ntdll		2298
imp	'sqrt$nt'						sqrt							ntdll		2299
imp	'sscanf$nt'						sscanf							ntdll		2300
imp	'sscanf_s$nt'						sscanf_s						ntdll		2301
imp	'strcat$nt'						strcat							ntdll		2302
imp	'strcat_s$nt'						strcat_s						ntdll		2303
imp	'strchr$nt'						strchr							ntdll		2304
imp	'strcmp$nt'						strcmp							ntdll		2305
imp	'strcpy$nt'						strcpy							ntdll		2306
imp	'strcpy_s$nt'						strcpy_s						ntdll		2307
imp	'strcspn$nt'						strcspn							ntdll		2308
imp	'strlen$nt'						strlen							ntdll		2309
imp	'strncat$nt'						strncat							ntdll		2310
imp	'strncat_s$nt'						strncat_s						ntdll		2311
imp	'strncmp$nt'						strncmp							ntdll		2312
imp	'strncpy$nt'						strncpy							ntdll		2313
imp	'strncpy_s$nt'						strncpy_s						ntdll		2314
imp	'strnlen$nt'						strnlen							ntdll		2315
imp	'strpbrk$nt'						strpbrk							ntdll		2316
imp	'strrchr$nt'						strrchr							ntdll		2317
imp	'strspn$nt'						strspn							ntdll		2318
imp	'strstr$nt'						strstr							ntdll		2319
imp	'strtok_s$nt'						strtok_s						ntdll		2320
imp	'strtol$nt'						strtol							ntdll		2321
imp	'strtoul$nt'						strtoul							ntdll		2322
imp	'swprintf$nt'						swprintf						ntdll		2323
imp	'swprintf_s'						swprintf_s						ntdll		2324
imp	'swscanf_s'						swscanf_s						ntdll		2325
imp	'tan$nt'						tan							ntdll		2326
imp	'time$nt'						time							KernelBase	1865
imp	'timeBeginPeriod'					timeBeginPeriod						kernel32	1609
imp	'timeEndPeriod'						timeEndPeriod						kernel32	1610
imp	'timeGetDevCaps'					timeGetDevCaps						kernel32	1611
imp	'timeGetSystemTime'					timeGetSystemTime					kernel32	1612
imp	'timeGetTime'						timeGetTime						kernel32	1613
imp	'tolower$nt'						tolower							ntdll		2327
imp	'toupper$nt'						toupper							ntdll		2328
imp	'towlower$nt'						towlower						ntdll		2329
imp	'towupper$nt'						towupper						ntdll		2330
imp	'uaw_lstrcmp'						uaw_lstrcmpW						kernel32	1614
imp	'uaw_lstrcmpi'						uaw_lstrcmpiW						kernel32	1615
imp	'uaw_lstrlen'						uaw_lstrlenW						kernel32	1616
imp	'uaw_wcschr'						uaw_wcschr						kernel32	1617
imp	'uaw_wcscpy'						uaw_wcscpy						kernel32	1618
imp	'uaw_wcsicmp'						uaw_wcsicmp						kernel32	1619
imp	'uaw_wcslen'						uaw_wcslen						kernel32	1620
imp	'uaw_wcsrchr'						uaw_wcsrchr						kernel32	1621
imp	'vDbgPrintEx'						vDbgPrintEx						ntdll		2331
imp	'vDbgPrintExWithPrefix'					vDbgPrintExWithPrefix					ntdll		2332
imp	'vSetPldc'						vSetPldc						gdi32		1966
imp	'vsprintf$nt'						vsprintf						ntdll		2333
imp	'vsprintf_s'						vsprintf_s						ntdll		2334
imp	'vswprintf_s'						vswprintf_s						ntdll		2335
imp	'wcscat$nt'						wcscat							ntdll		2336
imp	'wcscat_s'						wcscat_s						ntdll		2337
imp	'wcschr$nt'						wcschr							ntdll		2338
imp	'wcscmp$nt'						wcscmp							ntdll		2339
imp	'wcscpy$nt'						wcscpy							ntdll		2340
imp	'wcscpy_s'						wcscpy_s						ntdll		2341
imp	'wcscspn$nt'						wcscspn							ntdll		2342
imp	'wcslen$nt'						wcslen							ntdll		2343
imp	'wcsncat$nt'						wcsncat							ntdll		2344
imp	'wcsncat_s'						wcsncat_s						ntdll		2345
imp	'wcsncmp$nt'						wcsncmp							ntdll		2346
imp	'wcsncpy$nt'						wcsncpy							ntdll		2347
imp	'wcsncpy_s'						wcsncpy_s						ntdll		2348
imp	'wcsnlen$nt'						wcsnlen							ntdll		2349
imp	'wcspbrk$nt'						wcspbrk							ntdll		2350
imp	'wcsrchr$nt'						wcsrchr							ntdll		2351
imp	'wcsspn$nt'						wcsspn							ntdll		2352
imp	'wcsstr$nt'						wcsstr							ntdll		2353
imp	'wcstok_s'						wcstok_s						ntdll		2354
imp	'wcstol$nt'						wcstol							ntdll		2355
imp	'wcstombs$nt'						wcstombs						ntdll		2356
imp	'wcstoul$nt'						wcstoul							ntdll		2357
imp	'wprintf$nt'						wprintf							KernelBase	1866
imp	'wsprintfA'						wsprintfA						user32		2596
imp	'wsprintf'						wsprintfW						user32		2601
imp	'wvsprintfA'						wvsprintfA						user32		2602
imp	'wvsprintf'						wvsprintfW						user32		2603

imp	'InitializeCriticalSection'				InitializeCriticalSection				kernel32	0	1	# KernelBase
imp	'EnterCriticalSection'					EnterCriticalSection					kernel32	0	1	# KernelBase
imp	'LeaveCriticalSection'					LeaveCriticalSection					kernel32	0	1	# KernelBase
imp	'TryEnterCriticalSection'				TryEnterCriticalSection					kernel32	0	1	# KernelBase
imp	'DeleteCriticalSection'					DeleteCriticalSection					kernel32	0	1	# KernelBase
imp	'InitializeCriticalSectionAndSpinCount'			InitializeCriticalSectionAndSpinCount			kernel32	0	2	# KernelBase
imp	'SetCriticalSectionSpinCount'				SetCriticalSectionSpinCount				kernel32	0	2	# KernelBase

imp	'InitializeSRWLock'					InitializeSRWLock					kernel32	0	1	# KernelBase
imp	'AcquireSRWLockExclusive'				AcquireSRWLockExclusive					kernel32	0	1	# KernelBase
imp	'AcquireSRWLockShared'					AcquireSRWLockShared					kernel32	0	1	# KernelBase
imp	'ReleaseSRWLockExclusive'				ReleaseSRWLockExclusive					kernel32	0	1	# KernelBase
imp	'ReleaseSRWLockShared'					ReleaseSRWLockShared					kernel32	0	1	# KernelBase
imp	'TryAcquireSRWLockExclusive'				TryAcquireSRWLockExclusive				kernel32	0	1	# KernelBase
imp	'TryAcquireSRWLockShared'				TryAcquireSRWLockShared					kernel32	0	1	# KernelBase
