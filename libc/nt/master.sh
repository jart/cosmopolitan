/usr/bin/env echo ' -*-mode:sh;indent-tabs-mode:nil;tab-width:8;coding:utf-8-*-│
│vi: set net ft=sh ts=2 sts=2 sw=2 fenc=utf-8                               :vi│
╚────────────────────────────────────────────────────────────────'>/dev/null #*/
. libc/nt/codegen.sh

# The New Technology API
# » so many sections

# KERNEL32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'AcquireSRWLockExclusive'				AcquireSRWLockExclusive					kernel32	0	1
imp	'AcquireSRWLockShared'					AcquireSRWLockShared					kernel32	0	1
imp	'ActivateActCtx'					ActivateActCtx						kernel32	0
imp	'ActivateActCtxWorker'					ActivateActCtxWorker					kernel32	4
imp	'AddAtom'						AddAtomW						kernel32	6
imp	'AddConsoleAlias'					AddConsoleAliasW					kernel32	0
imp	'AddDllDirectory'					AddDllDirectory						kernel32	0
imp	'AddIntegrityLabelToBoundaryDescriptor'			AddIntegrityLabelToBoundaryDescriptor			kernel32	10
imp	'AddLocalAlternateComputerName'				AddLocalAlternateComputerNameW				kernel32	12
imp	'AddRefActCtx'						AddRefActCtx						kernel32	0
imp	'AddRefActCtxWorker'					AddRefActCtxWorker					kernel32	14
imp	'AddResourceAttributeAce'				AddResourceAttributeAce					kernel32	0
imp	'AddSIDToBoundaryDescriptor'				AddSIDToBoundaryDescriptor				kernel32	0
imp	'AddScopedPolicyIDAce'					AddScopedPolicyIDAce					kernel32	0
imp	'AddSecureMemoryCacheCallback'				AddSecureMemoryCacheCallback				kernel32	18
imp	'AddVectoredContinueHandler'				AddVectoredContinueHandler				kernel32	0	2
imp	'AddVectoredExceptionHandler'				AddVectoredExceptionHandler				kernel32	0	2
imp	'AdjustCalendarDate'					AdjustCalendarDate					kernel32	21
imp	'AllocConsole'						AllocConsole						kernel32	0	0
imp	'AllocateUserPhysicalPages'				AllocateUserPhysicalPages				kernel32	0
imp	'AllocateUserPhysicalPagesNuma'				AllocateUserPhysicalPagesNuma				kernel32	0
imp	'ApplicationRecoveryFinished'				ApplicationRecoveryFinished				kernel32	34
imp	'ApplicationRecoveryInProgress'				ApplicationRecoveryInProgress				kernel32	35
imp	'AreFileApisANSI'					AreFileApisANSI						kernel32	0
imp	'AssignProcessToJobObject'				AssignProcessToJobObject				kernel32	37
imp	'AttachConsole'						AttachConsole						kernel32	0	1
imp	'BackupRead'						BackupRead						kernel32	39
imp	'BackupSeek'						BackupSeek						kernel32	40
imp	'BackupWrite'						BackupWrite						kernel32	41
imp	'BaseCheckAppcompatCacheExWorker'			BaseCheckAppcompatCacheExWorker				kernel32	44
imp	'BaseCheckAppcompatCacheWorker'				BaseCheckAppcompatCacheWorker				kernel32	45
imp	'BaseCheckElevation'					BaseCheckElevation					kernel32	46
imp	'BaseCleanupAppcompatCacheSupportWorker'		BaseCleanupAppcompatCacheSupportWorker			kernel32	48
imp	'BaseDestroyVDMEnvironment'				BaseDestroyVDMEnvironment				kernel32	49
imp	'BaseDllReadWriteIniFile'				BaseDllReadWriteIniFile					kernel32	50
imp	'BaseDumpAppcompatCacheWorker'				BaseDumpAppcompatCacheWorker				kernel32	52
imp	'BaseElevationPostProcessing'				BaseElevationPostProcessing				kernel32	53
imp	'BaseFlushAppcompatCacheWorker'				BaseFlushAppcompatCacheWorker				kernel32	55
imp	'BaseFormatTimeOut'					BaseFormatTimeOut					kernel32	57
imp	'BaseFreeAppCompatDataForProcessWorker'			BaseFreeAppCompatDataForProcessWorker			kernel32	58
imp	'BaseGenerateAppCompatData'				BaseGenerateAppCompatData				kernel32	59
imp	'BaseInitAppcompatCacheSupportWorker'			BaseInitAppcompatCacheSupportWorker			kernel32	62
imp	'BaseIsAppcompatInfrastructureDisabledWorker'		BaseIsAppcompatInfrastructureDisabledWorker		kernel32	64
imp	'BaseIsDosApplication'					BaseIsDosApplication					kernel32	65
imp	'BaseQueryModuleData'					BaseQueryModuleData					kernel32	66
imp	'BaseReadAppCompatDataForProcessWorker'			BaseReadAppCompatDataForProcessWorker			kernel32	67
imp	'BaseSetLastNTError'					BaseSetLastNTError					kernel32	68
imp	'BaseThreadInitThunk'					BaseThreadInitThunk					kernel32	69
imp	'BaseUpdateAppcompatCacheWorker'			BaseUpdateAppcompatCacheWorker				kernel32	71
imp	'BaseUpdateVDMEntry'					BaseUpdateVDMEntry					kernel32	72
imp	'BaseVerifyUnicodeString'				BaseVerifyUnicodeString					kernel32	73
imp	'BaseWriteErrorElevationRequiredEvent'			BaseWriteErrorElevationRequiredEvent			kernel32	74
imp	'Basep8BitStringToDynamicUnicodeString'			Basep8BitStringToDynamicUnicodeString			kernel32	75
imp	'BasepAllocateActivationContextActivationBlock'		BasepAllocateActivationContextActivationBlock		kernel32	76
imp	'BasepAnsiStringToDynamicUnicodeString'			BasepAnsiStringToDynamicUnicodeString			kernel32	77
imp	'BasepAppContainerEnvironmentExtension'			BasepAppContainerEnvironmentExtension			kernel32	78
imp	'BasepAppXExtension'					BasepAppXExtension					kernel32	79
imp	'BasepCheckAppCompat'					BasepCheckAppCompat					kernel32	80
imp	'BasepCheckWebBladeHashes'				BasepCheckWebBladeHashes				kernel32	81
imp	'BasepCheckWinSaferRestrictions'			BasepCheckWinSaferRestrictions				kernel32	82
imp	'BasepConstructSxsCreateProcessMessage'			BasepConstructSxsCreateProcessMessage			kernel32	83
imp	'BasepCopyEncryption'					BasepCopyEncryption					kernel32	84
imp	'BasepFreeActivationContextActivationBlock'		BasepFreeActivationContextActivationBlock		kernel32	85
imp	'BasepFreeAppCompatData'				BasepFreeAppCompatData					kernel32	86
imp	'BasepGetAppCompatData'					BasepGetAppCompatData					kernel32	87
imp	'BasepGetComputerNameFromNtPath'			BasepGetComputerNameFromNtPath				kernel32	88
imp	'BasepGetExeArchType'					BasepGetExeArchType					kernel32	89
imp	'BasepInitAppCompatData'				BasepInitAppCompatData					kernel32	90
imp	'BasepIsProcessAllowed'					BasepIsProcessAllowed					kernel32	91
imp	'BasepMapModuleHandle'					BasepMapModuleHandle					kernel32	92
imp	'BasepNotifyLoadStringResource'				BasepNotifyLoadStringResource				kernel32	93
imp	'BasepPostSuccessAppXExtension'				BasepPostSuccessAppXExtension				kernel32	94
imp	'BasepProcessInvalidImage'				BasepProcessInvalidImage				kernel32	95
imp	'BasepQueryAppCompat'					BasepQueryAppCompat					kernel32	96
imp	'BasepQueryModuleChpeSettings'				BasepQueryModuleChpeSettings				kernel32	97
imp	'BasepReleaseAppXContext'				BasepReleaseAppXContext					kernel32	98
imp	'BasepReleaseSxsCreateProcessUtilityStruct'		BasepReleaseSxsCreateProcessUtilityStruct		kernel32	99
imp	'BasepReportFault'					BasepReportFault					kernel32	100
imp	'BasepSetFileEncryptionCompression'			BasepSetFileEncryptionCompression			kernel32	101
imp	'Beep'							Beep							kernel32	0
imp	'BeginUpdateResource'					BeginUpdateResourceW					kernel32	104
imp	'BindIoCompletionCallback'				BindIoCompletionCallback				kernel32	105
imp	'BuildCommDCBAndTimeouts'				BuildCommDCBAndTimeoutsW				kernel32	108
imp	'BuildCommDCBW'						BuildCommDCBW						kernel32	109
imp	'CallNamedPipe'						CallNamedPipeW						kernel32	0	7
imp	'CallNamedPipeA'					CallNamedPipeA						kernel32	110	7
imp	'CallbackMayRunLong'					CallbackMayRunLong					kernel32	0
imp	'CancelDeviceWakeupRequest'				CancelDeviceWakeupRequest				kernel32	113
imp	'CancelIo'						CancelIo						kernel32	0	1
imp	'CancelIoEx'						CancelIoEx						kernel32	0	2
imp	'CancelSynchronousIo'					CancelSynchronousIo					kernel32	0	1
imp	'CancelTimerQueueTimer'					CancelTimerQueueTimer					kernel32	118
imp	'CancelWaitableTimer'					CancelWaitableTimer					kernel32	0
imp	'CeipIsOptedIn'						CeipIsOptedIn						kernel32	0
imp	'ChangeTimerQueueTimer'					ChangeTimerQueueTimer					kernel32	0
imp	'CheckElevation'					CheckElevation						kernel32	123
imp	'CheckElevationEnabled'					CheckElevationEnabled					kernel32	124
imp	'CheckForReadOnlyResource'				CheckForReadOnlyResource				kernel32	125
imp	'CheckForReadOnlyResourceFilter'			CheckForReadOnlyResourceFilter				kernel32	126
imp	'CheckNameLegalDOS8Dot3'				CheckNameLegalDOS8Dot3W					kernel32	128
imp	'CheckRemoteDebuggerPresent'				CheckRemoteDebuggerPresent				kernel32	0	2
imp	'CheckTokenCapability'					CheckTokenCapability					kernel32	0
imp	'CheckTokenMembershipEx'				CheckTokenMembershipEx					kernel32	0
imp	'ClearCommBreak'					ClearCommBreak						kernel32	0	1
imp	'ClearCommError'					ClearCommError						kernel32	0
imp	'CloseConsoleHandle'					CloseConsoleHandle					kernel32	134
imp	'ClosePackageInfo'					ClosePackageInfo					kernel32	0
imp	'ClosePrivateNamespace'					ClosePrivateNamespace					kernel32	0
imp	'CloseProfileUserMapping'				CloseProfileUserMapping					kernel32	138
imp	'CmdBatNotification'					CmdBatNotification					kernel32	147
imp	'CommConfigDialog'					CommConfigDialogW					kernel32	149
imp	'CompareCalendarDates'					CompareCalendarDates					kernel32	150
imp	'CompareFileTime'					CompareFileTime						kernel32	0
imp	'CompareString'						CompareStringW						kernel32	0
imp	'CompareStringEx'					CompareStringEx						kernel32	0
imp	'CompareStringOrdinal'					CompareStringOrdinal					kernel32	0
imp	'ConnectNamedPipe'					ConnectNamedPipe					kernel32	0	2
imp	'ConsoleMenuControl'					ConsoleMenuControl					kernel32	157
imp	'ContinueDebugEvent'					ContinueDebugEvent					kernel32	0	3
imp	'ConvertCalDateTimeToSystemTime'			ConvertCalDateTimeToSystemTime				kernel32	159
imp	'ConvertDefaultLocale'					ConvertDefaultLocale					kernel32	0
imp	'ConvertFiberToThread'					ConvertFiberToThread					kernel32	0
imp	'ConvertNLSDayOfWeekToWin32DayOfWeek'			ConvertNLSDayOfWeekToWin32DayOfWeek			kernel32	162
imp	'ConvertSystemTimeToCalDateTime'			ConvertSystemTimeToCalDateTime				kernel32	163
imp	'ConvertThreadToFiber'					ConvertThreadToFiber					kernel32	0
imp	'ConvertThreadToFiberEx'				ConvertThreadToFiberEx					kernel32	0
imp	'CopyContext'						CopyContext						kernel32	0
imp	'CopyFile'						CopyFileW						kernel32	0	3
imp	'CopyFile2'						CopyFile2						kernel32	0
imp	'CopyFileEx'						CopyFileExW						kernel32	0
imp	'CopyFileTransacted'					CopyFileTransactedW					kernel32	172
imp	'CopyLZFile'						CopyLZFile						kernel32	174
imp	'CreateActCtx'						CreateActCtxW						kernel32	0
imp	'CreateActCtxWWorker'					CreateActCtxWWorker					kernel32	177
imp	'CreateBoundaryDescriptor'				CreateBoundaryDescriptorW				kernel32	0
imp	'CreateConsoleScreenBuffer'				CreateConsoleScreenBuffer				kernel32	0
imp	'CreateDirectoryEx'					CreateDirectoryExW					kernel32	0
imp	'CreateDirectoryTransacted'				CreateDirectoryTransactedW				kernel32	185
imp	'CreateEvent'						CreateEventW						kernel32	0	4
imp	'CreateEventEx'						CreateEventExW						kernel32	0	4
imp	'CreateFiber'						CreateFiber						kernel32	0
imp	'CreateFiberEx'						CreateFiberEx						kernel32	0
imp	'CreateFile2'						CreateFile2						kernel32	0
imp	'CreateFileMappingFromApp'				CreateFileMappingFromApp				kernel32	0
imp	'CreateFileTransacted'					CreateFileTransactedW					kernel32	202
imp	'CreateHardLink'					CreateHardLinkW						kernel32	0	3
imp	'CreateHardLinkTransacted'				CreateHardLinkTransactedW				kernel32	206
imp	'CreateIoCompletionPort'				CreateIoCompletionPort					kernel32	0	4
imp	'CreateJobObject'					CreateJobObjectW					kernel32	210
imp	'CreateJobSet'						CreateJobSet						kernel32	211
imp	'CreateMailslot'					CreateMailslotW						kernel32	213
imp	'CreateMemoryResourceNotification'			CreateMemoryResourceNotification			kernel32	0
imp	'CreateMutex'						CreateMutexW						kernel32	0
imp	'CreateMutexEx'						CreateMutexExW						kernel32	0
imp	'CreatePrivateNamespace'				CreatePrivateNamespaceW					kernel32	0
imp	'CreateRemoteThread'					CreateRemoteThread					kernel32	0
imp	'CreateRemoteThreadEx'					CreateRemoteThreadEx					kernel32	0
imp	'CreateSemaphore'					CreateSemaphoreW					kernel32	0	4
imp	'CreateSemaphoreEx'					CreateSemaphoreExW					kernel32	0
imp	'CreateSymbolicLinkTransacted'				CreateSymbolicLinkTransactedW				kernel32	238
imp	'CreateTapePartition'					CreateTapePartition					kernel32	240
imp	'CreateThreadpool'					CreateThreadpool					kernel32	0
imp	'CreateThreadpoolCleanupGroup'				CreateThreadpoolCleanupGroup				kernel32	0
imp	'CreateThreadpoolIo'					CreateThreadpoolIo					kernel32	0
imp	'CreateThreadpoolTimer'					CreateThreadpoolTimer					kernel32	0
imp	'CreateThreadpoolWait'					CreateThreadpoolWait					kernel32	0
imp	'CreateThreadpoolWork'					CreateThreadpoolWork					kernel32	0
imp	'CreateTimerQueue'					CreateTimerQueue					kernel32	0
imp	'CreateTimerQueueTimer'					CreateTimerQueueTimer					kernel32	0
imp	'CreateToolhelp32Snapshot'				CreateToolhelp32Snapshot				kernel32	0	2
imp	'CreateUmsCompletionList'				CreateUmsCompletionList					kernel32	251
imp	'CreateUmsThreadContext'				CreateUmsThreadContext					kernel32	252
imp	'CreateWaitableTimer'					CreateWaitableTimerW					kernel32	0	3
imp	'CreateWaitableTimerEx'					CreateWaitableTimerExW					kernel32	0	4
imp	'DeactivateActCtx'					DeactivateActCtx					kernel32	0
imp	'DeactivateActCtxWorker'				DeactivateActCtxWorker					kernel32	259
imp	'DebugActiveProcess'					DebugActiveProcess					kernel32	0	1
imp	'DebugActiveProcessStop'				DebugActiveProcessStop					kernel32	0	1
imp	'DebugBreakProcess'					DebugBreakProcess					kernel32	263	1
imp	'DebugSetProcessKillOnExit'				DebugSetProcessKillOnExit				kernel32	264
imp	'DefineDosDevice'					DefineDosDeviceW					kernel32	0
imp	'DeleteAtom'						DeleteAtom						kernel32	270
imp	'DeleteBoundaryDescriptor'				DeleteBoundaryDescriptor				kernel32	0
imp	'DeleteCriticalSection'					DeleteCriticalSection					kernel32	0	1
imp	'DeleteFiber'						DeleteFiber						kernel32	0
imp	'DeleteFileTransacted'					DeleteFileTransactedW					kernel32	276
imp	'DeleteProcThreadAttributeList'				DeleteProcThreadAttributeList				kernel32	0	1
imp	'DeleteSynchronizationBarrier'				DeleteSynchronizationBarrier				kernel32	279
imp	'DeleteTimerQueue'					DeleteTimerQueue					kernel32	280
imp	'DeleteTimerQueueEx'					DeleteTimerQueueEx					kernel32	0
imp	'DeleteTimerQueueTimer'					DeleteTimerQueueTimer					kernel32	0
imp	'DeleteUmsCompletionList'				DeleteUmsCompletionList					kernel32	283
imp	'DeleteUmsThreadContext'				DeleteUmsThreadContext					kernel32	284
imp	'DeleteVolumeMountPoint'				DeleteVolumeMountPointW					kernel32	0
imp	'DequeueUmsCompletionListItems'				DequeueUmsCompletionListItems				kernel32	287
imp	'DisableThreadLibraryCalls'				DisableThreadLibraryCalls				kernel32	0
imp	'DisableThreadProfiling'				DisableThreadProfiling					kernel32	290
imp	'DiscardVirtualMemory'					DiscardVirtualMemory					kernel32	0
imp	'DisconnectNamedPipe'					DisconnectNamedPipe					kernel32	0	1
imp	'DnsHostnameToComputerName'				DnsHostnameToComputerNameW				kernel32	296
imp	'DosDateTimeToFileTime'					DosDateTimeToFileTime					kernel32	297
imp	'DosPathToSessionPath'					DosPathToSessionPathW					kernel32	299
imp	'DuplicateConsoleHandle'				DuplicateConsoleHandle					kernel32	300
imp	'DuplicateEncryptionInfoFileExt'			DuplicateEncryptionInfoFileExt				kernel32	301
imp	'DuplicateHandle'					DuplicateHandle						kernel32	0	7
imp	'EnableThreadProfiling'					EnableThreadProfiling					kernel32	303
imp	'EndUpdateResource'					EndUpdateResourceW					kernel32	307
imp	'EnterCriticalSection'					EnterCriticalSection					kernel32	0	1
imp	'EnterSynchronizationBarrier'				EnterSynchronizationBarrier				kernel32	0
imp	'EnterUmsSchedulingMode'				EnterUmsSchedulingMode					kernel32	310
imp	'EnumCalendarInfo'					EnumCalendarInfoW					kernel32	0
imp	'EnumCalendarInfoEx'					EnumCalendarInfoExW					kernel32	0
imp	'EnumCalendarInfoExEx'					EnumCalendarInfoExEx					kernel32	0
imp	'EnumDateFormats'					EnumDateFormatsW					kernel32	0
imp	'EnumDateFormatsEx'					EnumDateFormatsExW					kernel32	0
imp	'EnumDateFormatsExEx'					EnumDateFormatsExEx					kernel32	0
imp	'EnumLanguageGroupLocales'				EnumLanguageGroupLocalesW				kernel32	0
imp	'EnumResourceLanguages'					EnumResourceLanguagesW					kernel32	326
imp	'EnumResourceLanguagesEx'				EnumResourceLanguagesExW				kernel32	0
imp	'EnumResourceNames'					EnumResourceNamesW					kernel32	0
imp	'EnumResourceNamesEx'					EnumResourceNamesExW					kernel32	0
imp	'EnumResourceTypes'					EnumResourceTypesW					kernel32	334
imp	'EnumResourceTypesEx'					EnumResourceTypesExW					kernel32	0
imp	'EnumSystemCodePages'					EnumSystemCodePagesW					kernel32	0
imp	'EnumSystemFirmwareTables'				EnumSystemFirmwareTables				kernel32	0
imp	'EnumSystemGeoID'					EnumSystemGeoID						kernel32	0
imp	'EnumSystemGeoNames'					EnumSystemGeoNames					kernel32	318
imp	'EnumSystemLanguageGroups'				EnumSystemLanguageGroupsW				kernel32	0
imp	'EnumSystemLocales'					EnumSystemLocalesW					kernel32	0
imp	'EnumSystemLocalesEx'					EnumSystemLocalesEx					kernel32	0
imp	'EnumTimeFormats'					EnumTimeFormatsW					kernel32	0
imp	'EnumTimeFormatsEx'					EnumTimeFormatsEx					kernel32	0
imp	'EnumUILanguages'					EnumUILanguagesW					kernel32	0
imp	'EnumerateLocalComputerNames'				EnumerateLocalComputerNamesW				kernel32	351
imp	'EraseTape'						EraseTape						kernel32	352
imp	'EscapeCommFunction'					EscapeCommFunction					kernel32	0
imp	'ExecuteUmsThread'					ExecuteUmsThread					kernel32	354
imp	'ExitProcess'						ExitProcess						kernel32	0	1	# a.k.a. RtlExitUserProcess
imp	'ExitThread'						ExitThread						kernel32	0	1
imp	'ExitVDM'						ExitVDM							kernel32	357
imp	'ExpandEnvironmentStrings'				ExpandEnvironmentStringsW				kernel32	0
imp	'FatalAppExit'						FatalAppExitW						kernel32	0
imp	'FatalExit'						FatalExit						kernel32	364	1
imp	'FileTimeToDosDateTime'					FileTimeToDosDateTime					kernel32	365
imp	'FileTimeToLocalFileTime'				FileTimeToLocalFileTime					kernel32	0
imp	'FileTimeToSystemTime'					FileTimeToSystemTime					kernel32	0
imp	'FillConsoleOutputAttribute'				FillConsoleOutputAttribute				kernel32	0	5
imp	'FillConsoleOutputCharacter'				FillConsoleOutputCharacterW				kernel32	0	5
imp	'FindActCtxSectionGuid'					FindActCtxSectionGuid					kernel32	0
imp	'FindActCtxSectionGuidWorker'				FindActCtxSectionGuidWorker				kernel32	372
imp	'FindActCtxSectionString'				FindActCtxSectionStringW				kernel32	0
imp	'FindActCtxSectionStringWWorker'			FindActCtxSectionStringWWorker				kernel32	375
imp	'FindAtom'						FindAtomW						kernel32	377
imp	'FindCloseChangeNotification'				FindCloseChangeNotification				kernel32	0
imp	'FindFirstChangeNotification'				FindFirstChangeNotificationW				kernel32	0
imp	'FindFirstFileEx'					FindFirstFileExW					kernel32	0	6
imp	'FindFirstFileName'					FindFirstFileNameW					kernel32	0
imp	'FindFirstFileNameTransacted'				FindFirstFileNameTransactedW				kernel32	385
imp	'FindFirstFileTransacted'				FindFirstFileTransactedW				kernel32	388
imp	'FindFirstStream'					FindFirstStreamW					kernel32	0
imp	'FindFirstStreamTransacted'				FindFirstStreamTransactedW				kernel32	390
imp	'FindFirstVolume'					FindFirstVolumeW					kernel32	0	2
imp	'FindFirstVolumeMountPoint'				FindFirstVolumeMountPointW				kernel32	394
imp	'FindNLSString'						FindNLSString						kernel32	0
imp	'FindNLSStringEx'					FindNLSStringEx						kernel32	0
imp	'FindNextChangeNotification'				FindNextChangeNotification				kernel32	0
imp	'FindNextFileName'					FindNextFileNameW					kernel32	0
imp	'FindNextStream'					FindNextStreamW						kernel32	0
imp	'FindNextVolume'					FindNextVolumeW						kernel32	0	3
imp	'FindNextVolumeMountPoint'				FindNextVolumeMountPointW				kernel32	405
imp	'FindPackagesByPackageFamily'				FindPackagesByPackageFamily				kernel32	0
imp	'FindResource'						FindResourceW						kernel32	0
imp	'FindResourceEx'					FindResourceExW						kernel32	0
imp	'FindStringOrdinal'					FindStringOrdinal					kernel32	0
imp	'FindVolumeClose'					FindVolumeClose						kernel32	0	1
imp	'FindVolumeMountPointClose'				FindVolumeMountPointClose				kernel32	414
imp	'FlsAlloc'						FlsAlloc						kernel32	0
imp	'FlsFree'						FlsFree							kernel32	0
imp	'FlsGetValue'						FlsGetValue						kernel32	0
imp	'FlsSetValue'						FlsSetValue						kernel32	0
imp	'FlushConsoleInputBuffer'				FlushConsoleInputBuffer					kernel32	0	1
imp	'FlushInstructionCache'					FlushInstructionCache					kernel32	0
imp	'FoldString'						FoldStringW						kernel32	0
imp	'FormatApplicationUserModelId'				FormatApplicationUserModelId				kernel32	0
imp	'FormatMessage'						FormatMessageW						kernel32	0	7
imp	'FreeConsole'						FreeConsole						kernel32	0	0
imp	'FreeEnvironmentStrings'				FreeEnvironmentStringsW					kernel32	0	1
imp	'FreeLibrary'						FreeLibrary						kernel32	0	1
imp	'FreeLibraryAndExitThread'				FreeLibraryAndExitThread				kernel32	0
imp	'FreeMemoryJobObject'					FreeMemoryJobObject					kernel32	435
imp	'FreeResource'						FreeResource						kernel32	0	1
imp	'FreeUserPhysicalPages'					FreeUserPhysicalPages					kernel32	0
imp	'GetACP'						GetACP							kernel32	0
imp	'GetActiveProcessorCount'				GetActiveProcessorCount					kernel32	440
imp	'GetActiveProcessorGroupCount'				GetActiveProcessorGroupCount				kernel32	441
imp	'GetAppContainerNamedObjectPath'			GetAppContainerNamedObjectPath				kernel32	0
imp	'GetApplicationRecoveryCallback'			GetApplicationRecoveryCallback				kernel32	0
imp	'GetApplicationRecoveryCallbackWorker'			GetApplicationRecoveryCallbackWorker			kernel32	445
imp	'GetApplicationRestartSettings'				GetApplicationRestartSettings				kernel32	0
imp	'GetApplicationRestartSettingsWorker'			GetApplicationRestartSettingsWorker			kernel32	447
imp	'GetApplicationUserModelId'				GetApplicationUserModelId				kernel32	0
imp	'GetAtomName'						GetAtomNameW						kernel32	450
imp	'GetBinaryType'						GetBinaryTypeW						kernel32	453
imp	'GetCPInfo'						GetCPInfo						kernel32	0
imp	'GetCPInfoEx'						GetCPInfoExW						kernel32	0
imp	'GetCalendarDateFormat'					GetCalendarDateFormat					kernel32	458
imp	'GetCalendarDateFormatEx'				GetCalendarDateFormatEx					kernel32	459
imp	'GetCalendarDaysInMonth'				GetCalendarDaysInMonth					kernel32	460
imp	'GetCalendarDifferenceInDays'				GetCalendarDifferenceInDays				kernel32	461
imp	'GetCalendarInfo'					GetCalendarInfoW					kernel32	0
imp	'GetCalendarInfoEx'					GetCalendarInfoEx					kernel32	0
imp	'GetCalendarMonthsInYear'				GetCalendarMonthsInYear					kernel32	465
imp	'GetCalendarSupportedDateRange'				GetCalendarSupportedDateRange				kernel32	466
imp	'GetCalendarWeekNumber'					GetCalendarWeekNumber					kernel32	467
imp	'GetComPlusPackageInstallStatus'			GetComPlusPackageInstallStatus				kernel32	468
imp	'GetCommConfig'						GetCommConfig						kernel32	0
imp	'GetCommMask'						GetCommMask						kernel32	0
imp	'GetCommModemStatus'					GetCommModemStatus					kernel32	0
imp	'GetCommProperties'					GetCommProperties					kernel32	0
imp	'GetCommState'						GetCommState						kernel32	0
imp	'GetCommTimeouts'					GetCommTimeouts						kernel32	0
imp	'GetCommandLine'					GetCommandLineW						kernel32	0	0
imp	'GetCompressedFileSize'					GetCompressedFileSizeW					kernel32	0	2
imp	'GetCompressedFileSizeTransacted'			GetCompressedFileSizeTransactedW			kernel32	479
imp	'GetComputerName'					GetComputerNameW					kernel32	484
imp	'GetComputerNameEx'					GetComputerNameExW					kernel32	0	3
imp	'GetConsoleAlias'					GetConsoleAliasW					kernel32	0
imp	'GetConsoleAliasExes'					GetConsoleAliasExesW					kernel32	0
imp	'GetConsoleAliasExesLength'				GetConsoleAliasExesLengthW				kernel32	0
imp	'GetConsoleAliases'					GetConsoleAliasesW					kernel32	0
imp	'GetConsoleAliasesLength'				GetConsoleAliasesLengthW				kernel32	0
imp	'GetConsoleCP'						GetConsoleCP						kernel32	0	0
imp	'GetConsoleCharType'					GetConsoleCharType					kernel32	496
imp	'GetConsoleCursorInfo'					GetConsoleCursorInfo					kernel32	0	2
imp	'GetConsoleCursorMode'					GetConsoleCursorMode					kernel32	502
imp	'GetConsoleDisplayMode'					GetConsoleDisplayMode					kernel32	0
imp	'GetConsoleFontInfo'					GetConsoleFontInfo					kernel32	504
imp	'GetConsoleFontSize'					GetConsoleFontSize					kernel32	0
imp	'GetConsoleHardwareState'				GetConsoleHardwareState					kernel32	506
imp	'GetConsoleHistoryInfo'					GetConsoleHistoryInfo					kernel32	0
imp	'GetConsoleInputWaitHandle'				GetConsoleInputWaitHandle				kernel32	510
imp	'GetConsoleKeyboardLayoutName'				GetConsoleKeyboardLayoutNameW				kernel32	512
imp	'GetConsoleMode'					GetConsoleMode						kernel32	0	2
imp	'GetConsoleNlsMode'					GetConsoleNlsMode					kernel32	514
imp	'GetConsoleOriginalTitle'				GetConsoleOriginalTitleW				kernel32	0
imp	'GetConsoleOutputCP'					GetConsoleOutputCP					kernel32	0	0
imp	'GetConsoleProcessList'					GetConsoleProcessList					kernel32	0
imp	'GetConsoleScreenBufferInfo'				GetConsoleScreenBufferInfo				kernel32	0	2
imp	'GetConsoleScreenBufferInfoEx'				GetConsoleScreenBufferInfoEx				kernel32	0	2
imp	'GetConsoleSelectionInfo'				GetConsoleSelectionInfo					kernel32	0	1
imp	'GetConsoleTitle'					GetConsoleTitleW					kernel32	0	2
imp	'GetConsoleWindow'					GetConsoleWindow					kernel32	0	0
imp	'GetCurrencyFormat'					GetCurrencyFormatW					kernel32	0
imp	'GetCurrencyFormatEx'					GetCurrencyFormatEx					kernel32	0
imp	'GetCurrentActCtx'					GetCurrentActCtx					kernel32	0
imp	'GetCurrentActCtxWorker'				GetCurrentActCtxWorker					kernel32	529
imp	'GetCurrentApplicationUserModelId'			GetCurrentApplicationUserModelId			kernel32	0
imp	'GetCurrentConsoleFont'					GetCurrentConsoleFont					kernel32	0
imp	'GetCurrentConsoleFontEx'				GetCurrentConsoleFontEx					kernel32	0
imp	'GetCurrentDirectory'					GetCurrentDirectoryW					kernel32	0	2
imp	'GetCurrentPackageFamilyName'				GetCurrentPackageFamilyName				kernel32	0
imp	'GetCurrentPackageFullName'				GetCurrentPackageFullName				kernel32	0
imp	'GetCurrentPackageId'					GetCurrentPackageId					kernel32	0
imp	'GetCurrentPackageInfo'					GetCurrentPackageInfo					kernel32	0
imp	'GetCurrentPackagePath'					GetCurrentPackagePath					kernel32	0
imp	'GetCurrentProcess'					GetCurrentProcess					kernel32	0	0
imp	'GetCurrentProcessId'					GetCurrentProcessId					kernel32	0	0
imp	'GetCurrentThread'					GetCurrentThread					kernel32	0	0
imp	'GetCurrentThreadId'					GetCurrentThreadId					kernel32	0	0
imp	'GetCurrentThreadStackLimits'				GetCurrentThreadStackLimits				kernel32	0
imp	'GetCurrentUmsThread'					GetCurrentUmsThread					kernel32	547
imp	'GetDateFormat'						GetDateFormatW						kernel32	0
imp	'GetDateFormatAWorker'					GetDateFormatAWorker					kernel32	549
imp	'GetDateFormatEx'					GetDateFormatEx						kernel32	0
imp	'GetDateFormatWWorker'					GetDateFormatWWorker					kernel32	552
imp	'GetDefaultCommConfig'					GetDefaultCommConfigW					kernel32	554
imp	'GetDevicePowerState'					GetDevicePowerState					kernel32	555
imp	'GetDiskFreeSpace'					GetDiskFreeSpaceW					kernel32	0
imp	'GetDiskFreeSpaceEx'					GetDiskFreeSpaceExW					kernel32	0
imp	'GetDllDirectory'					GetDllDirectoryW					kernel32	561
imp	'GetDriveType'						GetDriveTypeW						kernel32	0
imp	'GetDurationFormat'					GetDurationFormat					kernel32	564
imp	'GetDurationFormatEx'					GetDurationFormatEx					kernel32	0
imp	'GetDynamicTimeZoneInformation'				GetDynamicTimeZoneInformation				kernel32	0
imp	'GetEnabledXStateFeatures'				GetEnabledXStateFeatures				kernel32	0
imp	'GetEncryptedFileVersionExt'				GetEncryptedFileVersionExt				kernel32	568
imp	'GetEnvironmentStrings'					GetEnvironmentStringsW					kernel32	0	1
imp	'GetEnvironmentVariable'				GetEnvironmentVariableW					kernel32	0	3
imp	'GetErrorMode'						GetErrorMode						kernel32	0
imp	'GetExitCodeThread'					GetExitCodeThread					kernel32	0	2
imp	'GetExpandedName'					GetExpandedNameW					kernel32	579
imp	'GetFileAttributesEx'					GetFileAttributesExW					kernel32	0	3
imp	'GetFileAttributesTransacted'				GetFileAttributesTransactedW				kernel32	584
imp	'GetFileBandwidthReservation'				GetFileBandwidthReservation				kernel32	586
imp	'GetFileInformationByHandle'				GetFileInformationByHandle				kernel32	0	2
imp	'GetFileInformationByHandleEx'				GetFileInformationByHandleEx				kernel32	0	4
imp	'GetFileMUIInfo'					GetFileMUIInfo						kernel32	0
imp	'GetFileMUIPath'					GetFileMUIPath						kernel32	0
imp	'GetFileSize'						GetFileSize						kernel32	0	2
imp	'GetFileSizeEx'						GetFileSizeEx						kernel32	0	2
imp	'GetFileTime'						GetFileTime						kernel32	0	4
imp	'GetFileType'						GetFileType						kernel32	0	1
imp	'GetFinalPathNameByHandle'				GetFinalPathNameByHandleW				kernel32	0	4
imp	'GetFirmwareEnvironmentVariable'			GetFirmwareEnvironmentVariableW				kernel32	600
imp	'GetFirmwareEnvironmentVariableEx'			GetFirmwareEnvironmentVariableExW			kernel32	599
imp	'GetFirmwareType'					GetFirmwareType						kernel32	601
imp	'GetFullPathName'					GetFullPathNameW					kernel32	0	4
imp	'GetFullPathNameTransacted'				GetFullPathNameTransactedW				kernel32	604
imp	'GetGeoInfo'						GetGeoInfoW						kernel32	0
imp	'GetHandleInformation'					GetHandleInformation					kernel32	0	2
imp	'GetLargePageMinimum'					GetLargePageMinimum					kernel32	0
imp	'GetLargestConsoleWindowSize'				GetLargestConsoleWindowSize				kernel32	0	1
imp	'GetLastError'						GetLastError						kernel32	0	0
imp	'GetLocalTime'						GetLocalTime						kernel32	0
imp	'GetLocaleInfo'						GetLocaleInfoW						kernel32	0
imp	'GetLocaleInfoEx'					GetLocaleInfoEx						kernel32	0
imp	'GetLogicalDriveStrings'				GetLogicalDriveStringsW					kernel32	0
imp	'GetLogicalDrives'					GetLogicalDrives					kernel32	0	0
imp	'GetLogicalProcessorInformation'			GetLogicalProcessorInformation				kernel32	0
imp	'GetLogicalProcessorInformationEx'			GetLogicalProcessorInformationEx			kernel32	0
imp	'GetLongPathName'					GetLongPathNameW					kernel32	0
imp	'GetLongPathNameTransacted'				GetLongPathNameTransactedW				kernel32	624
imp	'GetMailslotInfo'					GetMailslotInfo						kernel32	626
imp	'GetMaximumProcessorCount'				GetMaximumProcessorCount				kernel32	627	1	# Windows 7+
imp	'GetMaximumProcessorGroupCount'				GetMaximumProcessorGroupCount				kernel32	628
imp	'GetMemoryErrorHandlingCapabilities'			GetMemoryErrorHandlingCapabilities			kernel32	0
imp	'GetModuleFileName'					GetModuleFileNameW					kernel32	0	3
imp	'GetModuleHandle'					GetModuleHandleA					kernel32	0	1
imp	'GetModuleHandleEx'					GetModuleHandleExW					kernel32	0	3
imp	'GetModuleHandleW'					GetModuleHandleW					kernel32	0	1
imp	'GetNLSVersion'						GetNLSVersion						kernel32	0
imp	'GetNLSVersionEx'					GetNLSVersionEx						kernel32	0
imp	'GetNamedPipeClientComputerName'			GetNamedPipeClientComputerNameW				kernel32	0
imp	'GetNamedPipeClientProcessId'				GetNamedPipeClientProcessId				kernel32	641
imp	'GetNamedPipeClientSessionId'				GetNamedPipeClientSessionId				kernel32	642
imp	'GetNamedPipeHandleState'				GetNamedPipeHandleStateW				kernel32	0
imp	'GetNamedPipeInfo'					GetNamedPipeInfo					kernel32	0
imp	'GetNamedPipeServerProcessId'				GetNamedPipeServerProcessId				kernel32	646
imp	'GetNamedPipeServerSessionId'				GetNamedPipeServerSessionId				kernel32	647
imp	'GetNativeSystemInfo'					GetNativeSystemInfo					kernel32	0
imp	'GetNextUmsListItem'					GetNextUmsListItem					kernel32	649
imp	'GetNextVDMCommand'					GetNextVDMCommand					kernel32	650
imp	'GetNumaAvailableMemoryNode'				GetNumaAvailableMemoryNode				kernel32	651
imp	'GetNumaAvailableMemoryNodeEx'				GetNumaAvailableMemoryNodeEx				kernel32	652
imp	'GetNumaHighestNodeNumber'				GetNumaHighestNodeNumber				kernel32	0
imp	'GetNumaNodeNumberFromHandle'				GetNumaNodeNumberFromHandle				kernel32	654
imp	'GetNumaNodeProcessorMask'				GetNumaNodeProcessorMask				kernel32	655
imp	'GetNumaNodeProcessorMaskEx'				GetNumaNodeProcessorMaskEx				kernel32	0
imp	'GetNumaProcessorNode'					GetNumaProcessorNode					kernel32	657
imp	'GetNumaProcessorNodeEx'				GetNumaProcessorNodeEx					kernel32	658
imp	'GetNumaProximityNode'					GetNumaProximityNode					kernel32	659
imp	'GetNumaProximityNodeEx'				GetNumaProximityNodeEx					kernel32	0
imp	'GetNumberFormat'					GetNumberFormatW					kernel32	0
imp	'GetNumberFormatEx'					GetNumberFormatEx					kernel32	0
imp	'GetNumberOfConsoleFonts'				GetNumberOfConsoleFonts					kernel32	664
imp	'GetNumberOfConsoleInputEvents'				GetNumberOfConsoleInputEvents				kernel32	0	2
imp	'GetNumberOfConsoleMouseButtons'			GetNumberOfConsoleMouseButtons				kernel32	0	1
imp	'GetOEMCP'						GetOEMCP						kernel32	0
imp	'GetOverlappedResult'					GetOverlappedResult					kernel32	0	4
imp	'GetOverlappedResultEx'					GetOverlappedResultEx					kernel32	0	5
imp	'GetPackageApplicationIds'				GetPackageApplicationIds				kernel32	0
imp	'GetPackageFamilyName'					GetPackageFamilyName					kernel32	0
imp	'GetPackageFullName'					GetPackageFullName					kernel32	0
imp	'GetPackageId'						GetPackageId						kernel32	0
imp	'GetPackageInfo'					GetPackageInfo						kernel32	0
imp	'GetPackagePath'					GetPackagePath						kernel32	0
imp	'GetPackagePathByFullName'				GetPackagePathByFullName				kernel32	0
imp	'GetPackagesByPackageFamily'				GetPackagesByPackageFamily				kernel32	0
imp	'GetPhysicallyInstalledSystemMemory'			GetPhysicallyInstalledSystemMemory			kernel32	0
imp	'GetPriorityClass'					GetPriorityClass					kernel32	0	1
imp	'GetPrivateProfileInt'					GetPrivateProfileIntW					kernel32	681
imp	'GetPrivateProfileSection'				GetPrivateProfileSectionW				kernel32	685
imp	'GetPrivateProfileSectionNames'				GetPrivateProfileSectionNamesW				kernel32	684
imp	'GetPrivateProfileString'				GetPrivateProfileStringW				kernel32	687
imp	'GetPrivateProfileStruct'				GetPrivateProfileStructW				kernel32	689
imp	'GetProcAddress'					GetProcAddress						kernel32	0	2
imp	'GetProcessAffinityMask'				GetProcessAffinityMask					kernel32	0	3
imp	'GetProcessDEPPolicy'					GetProcessDEPPolicy					kernel32	692
imp	'GetProcessDefaultCpuSets'				GetProcessDefaultCpuSets				kernel32	0
imp	'GetProcessGroupAffinity'				GetProcessGroupAffinity					kernel32	0
imp	'GetProcessHandleCount'					GetProcessHandleCount					kernel32	0	2
imp	'GetProcessHeap'					GetProcessHeap						kernel32	0	0
imp	'GetProcessHeaps'					GetProcessHeaps						kernel32	0	2
imp	'GetProcessId'						GetProcessId						kernel32	0	1
imp	'GetProcessIdOfThread'					GetProcessIdOfThread					kernel32	0	1
imp	'GetProcessInformation'					GetProcessInformation					kernel32	0	4
imp	'GetProcessIoCounters'					GetProcessIoCounters					kernel32	701	2
imp	'GetProcessMitigationPolicy'				GetProcessMitigationPolicy				kernel32	0
imp	'GetProcessPreferredUILanguages'			GetProcessPreferredUILanguages				kernel32	0
imp	'GetProcessPriorityBoost'				GetProcessPriorityBoost					kernel32	0	2
imp	'GetProcessShutdownParameters'				GetProcessShutdownParameters				kernel32	0
imp	'GetProcessTimes'					GetProcessTimes						kernel32	0	5
imp	'GetProcessVersion'					GetProcessVersion					kernel32	0
imp	'GetProcessWorkingSetSize'				GetProcessWorkingSetSize				kernel32	708	3
imp	'GetProcessWorkingSetSizeEx'				GetProcessWorkingSetSizeEx				kernel32	0	4
imp	'GetProcessorSystemCycleTime'				GetProcessorSystemCycleTime				kernel32	0
imp	'GetProductInfo'					GetProductInfo						kernel32	0
imp	'GetProfileInt'						GetProfileIntW						kernel32	713
imp	'GetProfileSection'					GetProfileSectionW					kernel32	715
imp	'GetProfileString'					GetProfileStringW					kernel32	717
imp	'GetQueuedCompletionStatus'				GetQueuedCompletionStatus				kernel32	0	5
imp	'GetQueuedCompletionStatusEx'				GetQueuedCompletionStatusEx				kernel32	0	6
imp	'GetShortPathName'					GetShortPathNameW					kernel32	0
imp	'GetStagedPackagePathByFullName'			GetStagedPackagePathByFullName				kernel32	0
imp	'GetStartupInfo'					GetStartupInfoW						kernel32	0	1
imp	'GetStdHandle'						GetStdHandle						kernel32	0	1
imp	'GetStringScripts'					GetStringScripts					kernel32	0
imp	'GetStringType'						GetStringTypeW						kernel32	0
imp	'GetStringTypeEx'					GetStringTypeExW					kernel32	0
imp	'GetSystemCpuSetInformation'				GetSystemCpuSetInformation				kernel32	0
imp	'GetSystemDEPPolicy'					GetSystemDEPPolicy					kernel32	734
imp	'GetSystemDefaultLCID'					GetSystemDefaultLCID					kernel32	0
imp	'GetSystemDefaultLangID'				GetSystemDefaultLangID					kernel32	0
imp	'GetSystemDefaultLocaleName'				GetSystemDefaultLocaleName				kernel32	0
imp	'GetSystemDefaultUILanguage'				GetSystemDefaultUILanguage				kernel32	0
imp	'GetSystemDirectory'					GetSystemDirectoryW					kernel32	0	2
imp	'GetSystemDirectoryA'					GetSystemDirectoryA					kernel32	0	2
imp	'GetSystemFileCacheSize'				GetSystemFileCacheSize					kernel32	0
imp	'GetSystemFirmwareTable'				GetSystemFirmwareTable					kernel32	0
imp	'GetSystemInfo'						GetSystemInfo						kernel32	0	1
imp	'GetSystemPowerStatus'					GetSystemPowerStatus					kernel32	744
imp	'GetSystemPreferredUILanguages'				GetSystemPreferredUILanguages				kernel32	0
imp	'GetSystemRegistryQuota'				GetSystemRegistryQuota					kernel32	746
imp	'GetSystemTime'						GetSystemTime						kernel32	0	1
imp	'GetSystemTimeAdjustment'				GetSystemTimeAdjustment					kernel32	0	3
imp	'GetSystemTimeAsFileTime'				GetSystemTimeAsFileTime					kernel32	0	1
imp	'GetSystemTimePreciseAsFileTime'			GetSystemTimePreciseAsFileTime				kernel32	0	1
imp	'GetSystemTimes'					GetSystemTimes						kernel32	0	3
imp	'GetSystemWindowsDirectory'				GetSystemWindowsDirectoryW				kernel32	0
imp	'GetTapeParameters'					GetTapeParameters					kernel32	756
imp	'GetTapePosition'					GetTapePosition						kernel32	757
imp	'GetTapeStatus'						GetTapeStatus						kernel32	758
imp	'GetTempFileName'					GetTempFileNameW					kernel32	0
imp	'GetTempPath'						GetTempPathW						kernel32	0	2
imp	'GetTempPathA'						GetTempPathA						kernel32	0	2
imp	'GetThreadContext'					GetThreadContext					kernel32	0
imp	'GetThreadErrorMode'					GetThreadErrorMode					kernel32	0
imp	'GetThreadGroupAffinity'				GetThreadGroupAffinity					kernel32	0
imp	'GetThreadIOPendingFlag'				GetThreadIOPendingFlag					kernel32	0	2
imp	'GetThreadId'						GetThreadId						kernel32	0	1
imp	'GetThreadIdealProcessorEx'				GetThreadIdealProcessorEx				kernel32	0
imp	'GetThreadInformation'					GetThreadInformation					kernel32	0
imp	'GetThreadLocale'					GetThreadLocale						kernel32	0
imp	'GetThreadPreferredUILanguages'				GetThreadPreferredUILanguages				kernel32	0
imp	'GetThreadPriority'					GetThreadPriority					kernel32	0	1
imp	'GetThreadPriorityBoost'				GetThreadPriorityBoost					kernel32	0	2
imp	'GetThreadSelectedCpuSets'				GetThreadSelectedCpuSets				kernel32	0
imp	'GetThreadSelectorEntry'				GetThreadSelectorEntry					kernel32	776
imp	'GetThreadTimes'					GetThreadTimes						kernel32	0	5
imp	'GetThreadUILanguage'					GetThreadUILanguage					kernel32	0
imp	'GetTickCount'						GetTickCount						kernel32	0
imp	'GetTickCount64'					GetTickCount64						kernel32	0	0
imp	'GetTimeFormat'						GetTimeFormatW						kernel32	0
imp	'GetTimeFormatAWorker'					GetTimeFormatAWorker					kernel32	782
imp	'GetTimeFormatEx'					GetTimeFormatEx						kernel32	0
imp	'GetTimeFormatWWorker'					GetTimeFormatWWorker					kernel32	785
imp	'GetTimeZoneInformation'				GetTimeZoneInformation					kernel32	0
imp	'GetTimeZoneInformationForYear'				GetTimeZoneInformationForYear				kernel32	0
imp	'GetUILanguageInfo'					GetUILanguageInfo					kernel32	0
imp	'GetUmsCompletionListEvent'				GetUmsCompletionListEvent				kernel32	789
imp	'GetUmsSystemThreadInformation'				GetUmsSystemThreadInformation				kernel32	790
imp	'GetUserDefaultLCID'					GetUserDefaultLCID					kernel32	0
imp	'GetUserDefaultLangID'					GetUserDefaultLangID					kernel32	0
imp	'GetUserDefaultLocaleName'				GetUserDefaultLocaleName				kernel32	0
imp	'GetUserDefaultUILanguage'				GetUserDefaultUILanguage				kernel32	0
imp	'GetUserGeoID'						GetUserGeoID						kernel32	0
imp	'GetUserPreferredUILanguages'				GetUserPreferredUILanguages				kernel32	0
imp	'GetVDMCurrentDirectories'				GetVDMCurrentDirectories				kernel32	798
imp	'GetVersion'						GetVersion						kernel32	0
imp	'GetVersionEx'						GetVersionExW						kernel32	0	1
imp	'GetVolumeInformation'					GetVolumeInformationW					kernel32	0
imp	'GetVolumeInformationByHandle'				GetVolumeInformationByHandleW				kernel32	0	8
imp	'GetVolumeNameForVolumeMountPoint'			GetVolumeNameForVolumeMountPointW			kernel32	0
imp	'GetVolumePathName'					GetVolumePathNameW					kernel32	0	3
imp	'GetVolumePathNamesForVolumeName'			GetVolumePathNamesForVolumeNameW			kernel32	0
imp	'GetWindowsDirectory'					GetWindowsDirectoryW					kernel32	0	2
imp	'GetWindowsDirectoryA'					GetWindowsDirectoryA					kernel32	0	2
imp	'GetWriteWatch'						GetWriteWatch						kernel32	0
imp	'GetXStateFeaturesMask'					GetXStateFeaturesMask					kernel32	0
imp	'GlobalAddAtom'						GlobalAddAtomW						kernel32	818
imp	'GlobalAddAtomEx'					GlobalAddAtomExW					kernel32	817
imp	'GlobalAlloc'						GlobalAlloc						kernel32	0	2
imp	'GlobalCompact'						GlobalCompact						kernel32	820
imp	'GlobalDeleteAtom'					GlobalDeleteAtom					kernel32	821
imp	'GlobalFindAtom'					GlobalFindAtomW						kernel32	823
imp	'GlobalFix'						GlobalFix						kernel32	824
imp	'GlobalFlags'						GlobalFlags						kernel32	825
imp	'GlobalFree'						GlobalFree						kernel32	0	1
imp	'GlobalGetAtomName'					GlobalGetAtomNameW					kernel32	828
imp	'GlobalHandle'						GlobalHandle						kernel32	829
imp	'GlobalLock'						GlobalLock						kernel32	830
imp	'GlobalMemoryStatus'					GlobalMemoryStatus					kernel32	831
imp	'GlobalMemoryStatusEx'					GlobalMemoryStatusEx					kernel32	0	1
imp	'GlobalReAlloc'						GlobalReAlloc						kernel32	833
imp	'GlobalSize'						GlobalSize						kernel32	834
imp	'GlobalUnWire'						GlobalUnWire						kernel32	835
imp	'GlobalUnfix'						GlobalUnfix						kernel32	836
imp	'GlobalUnlock'						GlobalUnlock						kernel32	837
imp	'GlobalWire'						GlobalWire						kernel32	838
imp	'Heap32First'						Heap32First						kernel32	839
imp	'Heap32ListFirst'					Heap32ListFirst						kernel32	840
imp	'Heap32ListNext'					Heap32ListNext						kernel32	841
imp	'Heap32Next'						Heap32Next						kernel32	842
imp	'HeapAlloc'						HeapAlloc						kernel32	0	3
imp	'HeapCompact'						HeapCompact						kernel32	0	2
imp	'HeapCreate'						HeapCreate						kernel32	0	3
imp	'HeapDestroy'						HeapDestroy						kernel32	0	1
imp	'HeapFree'						HeapFree						kernel32	847	3
imp	'HeapLock'						HeapLock						kernel32	0
imp	'HeapQueryInformation'					HeapQueryInformation					kernel32	0
imp	'HeapReAlloc'						HeapReAlloc						kernel32	0	4
imp	'HeapSetInformation'					HeapSetInformation					kernel32	0
imp	'HeapUnlock'						HeapUnlock						kernel32	0
imp	'HeapValidate'						HeapValidate						kernel32	0
imp	'HeapWalk'						HeapWalk						kernel32	0
imp	'IdnToAscii'						IdnToAscii						kernel32	0
imp	'IdnToNameprepUnicode'					IdnToNameprepUnicode					kernel32	0
imp	'IdnToUnicode'						IdnToUnicode						kernel32	0
imp	'InitAtomTable'						InitAtomTable						kernel32	860
imp	'InitOnceBeginInitialize'				InitOnceBeginInitialize					kernel32	0
imp	'InitOnceComplete'					InitOnceComplete					kernel32	0
imp	'InitOnceExecuteOnce'					InitOnceExecuteOnce					kernel32	0
imp	'InitializeContext'					InitializeContext					kernel32	0
imp	'InitializeCriticalSection'				InitializeCriticalSection				kernel32	0	1
imp	'InitializeCriticalSectionAndSpinCount'			InitializeCriticalSectionAndSpinCount			kernel32	0	2
imp	'InitializeCriticalSectionAndSpinCount'			InitializeCriticalSectionAndSpinCount			kernel32	0	2
imp	'InitializeCriticalSectionEx'				InitializeCriticalSectionEx				kernel32	0
imp	'InitializeProcThreadAttributeList'			InitializeProcThreadAttributeList			kernel32	0	4
imp	'InitializeSRWLock'					InitializeSRWLock					kernel32	0	1
imp	'InitializeSynchronizationBarrier'			InitializeSynchronizationBarrier			kernel32	0
imp	'InstallELAMCertificateInfo'				InstallELAMCertificateInfo				kernel32	0
imp	'InvalidateConsoleDIBits'				InvalidateConsoleDIBits					kernel32	881
imp	'IsBadCodePtr'						IsBadCodePtr						kernel32	882
imp	'IsBadHugeReadPtr'					IsBadHugeReadPtr					kernel32	883
imp	'IsBadHugeWritePtr'					IsBadHugeWritePtr					kernel32	884
imp	'IsBadReadPtr'						IsBadReadPtr						kernel32	885
imp	'IsBadStringPtr'					IsBadStringPtrW						kernel32	887
imp	'IsBadWritePtr'						IsBadWritePtr						kernel32	888
imp	'IsCalendarLeapDay'					IsCalendarLeapDay					kernel32	889
imp	'IsCalendarLeapMonth'					IsCalendarLeapMonth					kernel32	890
imp	'IsCalendarLeapYear'					IsCalendarLeapYear					kernel32	891
imp	'IsDBCSLeadByte'					IsDBCSLeadByte						kernel32	0
imp	'IsDBCSLeadByteEx'					IsDBCSLeadByteEx					kernel32	0
imp	'IsNLSDefinedString'					IsNLSDefinedString					kernel32	0
imp	'IsNativeVhdBoot'					IsNativeVhdBoot						kernel32	897
imp	'IsNormalizedString'					IsNormalizedString					kernel32	0
imp	'IsProcessCritical'					IsProcessCritical					kernel32	0
imp	'IsProcessInJob'					IsProcessInJob						kernel32	0
imp	'IsProcessorFeaturePresent'				IsProcessorFeaturePresent				kernel32	0
imp	'IsSystemResumeAutomatic'				IsSystemResumeAutomatic					kernel32	902
imp	'IsThreadAFiber'					IsThreadAFiber						kernel32	0
imp	'IsValidCalDateTime'					IsValidCalDateTime					kernel32	905
imp	'IsValidCodePage'					IsValidCodePage						kernel32	0
imp	'IsValidLanguageGroup'					IsValidLanguageGroup					kernel32	0
imp	'IsValidLocale'						IsValidLocale						kernel32	0
imp	'IsValidLocaleName'					IsValidLocaleName					kernel32	0
imp	'IsValidNLSVersion'					IsValidNLSVersion					kernel32	0
imp	'LCIDToLocaleName'					LCIDToLocaleName					kernel32	0
imp	'LCMapString'						LCMapStringW						kernel32	0
imp	'LCMapStringEx'						LCMapStringEx						kernel32	0
imp	'LZClose'						LZClose							kernel32	945
imp	'LZCloseFile'						LZCloseFile						kernel32	946
imp	'LZCopy'						LZCopy							kernel32	947
imp	'LZCreateFile'						LZCreateFileW						kernel32	948
imp	'LZDone'						LZDone							kernel32	949
imp	'LZInit'						LZInit							kernel32	950
imp	'LZOpenFile'						LZOpenFileW						kernel32	952
imp	'LZRead'						LZRead							kernel32	953
imp	'LZSeek'						LZSeek							kernel32	954
imp	'LZStart'						LZStart							kernel32	955
imp	'LeaveCriticalSection'					LeaveCriticalSection					kernel32	0	1
imp	'LoadLibrary'						LoadLibraryW						kernel32	0	1
imp	'LoadLibraryEx'						LoadLibraryExW						kernel32	0	3
imp	'LoadModule'						LoadModule						kernel32	964
imp	'LoadPackagedLibrary'					LoadPackagedLibrary					kernel32	0
imp	'LoadResource'						LoadResource						kernel32	0	2
imp	'LoadStringBase'					LoadStringBaseW						kernel32	968
imp	'LocalAlloc'						LocalAlloc						kernel32	0
imp	'LocalCompact'						LocalCompact						kernel32	970
imp	'LocalFileTimeToFileTime'				LocalFileTimeToFileTime					kernel32	0
imp	'LocalFlags'						LocalFlags						kernel32	972
imp	'LocalFree'						LocalFree						kernel32	0	1
imp	'LocalHandle'						LocalHandle						kernel32	974
imp	'LocalLock'						LocalLock						kernel32	0
imp	'LocalReAlloc'						LocalReAlloc						kernel32	0
imp	'LocalShrink'						LocalShrink						kernel32	977
imp	'LocalSize'						LocalSize						kernel32	978
imp	'LocalUnlock'						LocalUnlock						kernel32	0
imp	'LocaleNameToLCID'					LocaleNameToLCID					kernel32	0
imp	'LocateXStateFeature'					LocateXStateFeature					kernel32	0
imp	'LockFile'						LockFile						kernel32	0	5
imp	'LockResource'						LockResource						kernel32	0	1
imp	'MapUserPhysicalPages'					MapUserPhysicalPages					kernel32	0
imp	'MapUserPhysicalPagesScatter'				MapUserPhysicalPagesScatter				kernel32	986
imp	'MapViewOfFile'						MapViewOfFile						kernel32	0
imp	'MapViewOfFileFromApp'					MapViewOfFileFromApp					kernel32	0
imp	'Module32First'						Module32FirstW						kernel32	992
imp	'Module32Next'						Module32NextW						kernel32	994
imp	'MoveFile'						MoveFileW						kernel32	1000	2
imp	'MoveFileTransacted'					MoveFileTransactedW					kernel32	999
imp	'MoveFileWithProgress'					MoveFileWithProgressW					kernel32	0
imp	'MulDiv'						MulDiv							kernel32	0
imp	'MultiByteToWideChar'					MultiByteToWideChar					kernel32	0	6
imp	'NeedCurrentDirectoryForExePath'			NeedCurrentDirectoryForExePathW				kernel32	0
imp	'NormalizeString'					NormalizeString						kernel32	0
imp	'NotifyUILanguageChange'				NotifyUILanguageChange					kernel32	1015
imp	'OOBEComplete'						OOBEComplete						kernel32	1017
imp	'OfferVirtualMemory'					OfferVirtualMemory					kernel32	0	3
imp	'OpenConsole'						OpenConsoleW						kernel32	1019
imp	'OpenConsoleWStub'					OpenConsoleWStub					kernel32	1020
imp	'OpenEvent'						OpenEventW						kernel32	0
imp	'OpenFile'						OpenFile						kernel32	1023
imp	'OpenFileById'						OpenFileById						kernel32	0
imp	'OpenFileMapping'					OpenFileMappingW					kernel32	0
imp	'OpenJobObject'						OpenJobObjectW						kernel32	1028
imp	'OpenMutex'						OpenMutexW						kernel32	0
imp	'OpenPackageInfoByFullName'				OpenPackageInfoByFullName				kernel32	0
imp	'OpenPrivateNamespace'					OpenPrivateNamespaceW					kernel32	0
imp	'OpenProfileUserMapping'				OpenProfileUserMapping					kernel32	1036
imp	'OpenSemaphore'						OpenSemaphoreW						kernel32	0
imp	'OpenThread'						OpenThread						kernel32	0	3
imp	'OpenWaitableTimer'					OpenWaitableTimerW					kernel32	0
imp	'OutputDebugString'					OutputDebugStringW					kernel32	0
imp	'PackageFamilyNameFromFullName'				PackageFamilyNameFromFullName				kernel32	0
imp	'PackageFamilyNameFromId'				PackageFamilyNameFromId					kernel32	0
imp	'PackageFullNameFromId'					PackageFullNameFromId					kernel32	0
imp	'PackageIdFromFullName'					PackageIdFromFullName					kernel32	0
imp	'PackageNameAndPublisherIdFromFamilyName'		PackageNameAndPublisherIdFromFamilyName			kernel32	0
imp	'ParseApplicationUserModelId'				ParseApplicationUserModelId				kernel32	0
imp	'PeekConsoleInput'					PeekConsoleInputW					kernel32	0	4
imp	'PeekNamedPipe'						PeekNamedPipe						kernel32	0	6
imp	'PostQueuedCompletionStatus'				PostQueuedCompletionStatus				kernel32	0	4
imp	'PowerClearRequest'					PowerClearRequest					kernel32	1057
imp	'PowerCreateRequest'					PowerCreateRequest					kernel32	1058
imp	'PowerSetRequest'					PowerSetRequest						kernel32	1059
imp	'PrefetchVirtualMemory'					PrefetchVirtualMemory					kernel32	0	4
imp	'PrepareTape'						PrepareTape						kernel32	1061
imp	'PrivMoveFileIdentity'					PrivMoveFileIdentityW					kernel32	1063
imp	'Process32First'					Process32FirstW						kernel32	0	2
imp	'Process32Next'						Process32NextW						kernel32	0	2
imp	'ProcessIdToSessionId'					ProcessIdToSessionId					kernel32	0
imp	'PssCaptureSnapshot'					PssCaptureSnapshot					kernel32	0
imp	'PssDuplicateSnapshot'					PssDuplicateSnapshot					kernel32	0
imp	'PssFreeSnapshot'					PssFreeSnapshot						kernel32	0
imp	'PssQuerySnapshot'					PssQuerySnapshot					kernel32	0
imp	'PssWalkMarkerCreate'					PssWalkMarkerCreate					kernel32	0
imp	'PssWalkMarkerFree'					PssWalkMarkerFree					kernel32	0
imp	'PssWalkMarkerGetPosition'				PssWalkMarkerGetPosition				kernel32	0
imp	'PssWalkMarkerRewind'					PssWalkMarkerRewind					kernel32	1076
imp	'PssWalkMarkerSeek'					PssWalkMarkerSeek					kernel32	1077
imp	'PssWalkMarkerSeekToBeginning'				PssWalkMarkerSeekToBeginning				kernel32	0
imp	'PssWalkMarkerSetPosition'				PssWalkMarkerSetPosition				kernel32	0
imp	'PssWalkMarkerTell'					PssWalkMarkerTell					kernel32	1080
imp	'PssWalkSnapshot'					PssWalkSnapshot						kernel32	0
imp	'PulseEvent'						PulseEvent						kernel32	0	1
imp	'PurgeComm'						PurgeComm						kernel32	0	2
imp	'QueryActCtx'						QueryActCtxW						kernel32	0
imp	'QueryActCtxSettings'					QueryActCtxSettingsW					kernel32	0
imp	'QueryActCtxSettingsWWorker'				QueryActCtxSettingsWWorker				kernel32	1085
imp	'QueryActCtxWWorker'					QueryActCtxWWorker					kernel32	1087
imp	'QueryDosDevice'					QueryDosDeviceW						kernel32	0
imp	'QueryFullProcessImageName'				QueryFullProcessImageNameW				kernel32	0
imp	'QueryIdleProcessorCycleTime'				QueryIdleProcessorCycleTime				kernel32	0
imp	'QueryIdleProcessorCycleTimeEx'				QueryIdleProcessorCycleTimeEx				kernel32	0
imp	'QueryInformationJobObject'				QueryInformationJobObject				kernel32	1095
imp	'QueryIoRateControlInformationJobObject'		QueryIoRateControlInformationJobObject			kernel32	1096
imp	'QueryMemoryResourceNotification'			QueryMemoryResourceNotification				kernel32	0
imp	'QueryPerformanceCounter'				QueryPerformanceCounter					kernel32	1098	1
imp	'QueryPerformanceFrequency'				QueryPerformanceFrequency				kernel32	1099	1
imp	'QueryProcessAffinityUpdateMode'			QueryProcessAffinityUpdateMode				kernel32	0
imp	'QueryProcessCycleTime'					QueryProcessCycleTime					kernel32	0
imp	'QueryProtectedPolicy'					QueryProtectedPolicy					kernel32	0
imp	'QueryThreadCycleTime'					QueryThreadCycleTime					kernel32	0
imp	'QueryThreadProfiling'					QueryThreadProfiling					kernel32	1104
imp	'QueryThreadpoolStackInformation'			QueryThreadpoolStackInformation				kernel32	0
imp	'QueryUmsThreadInformation'				QueryUmsThreadInformation				kernel32	1106
imp	'QueryUnbiasedInterruptTime'				QueryUnbiasedInterruptTime				kernel32	1107
imp	'QueueUserAPC'						QueueUserAPC						kernel32	0
imp	'QueueUserWorkItem'					QueueUserWorkItem					kernel32	0
imp	'QuirkGetData2Worker'					QuirkGetData2Worker					kernel32	1110
imp	'QuirkGetDataWorker'					QuirkGetDataWorker					kernel32	1111
imp	'QuirkIsEnabled2Worker'					QuirkIsEnabled2Worker					kernel32	1112
imp	'QuirkIsEnabled3Worker'					QuirkIsEnabled3Worker					kernel32	1113
imp	'QuirkIsEnabledForPackage2Worker'			QuirkIsEnabledForPackage2Worker				kernel32	1114
imp	'QuirkIsEnabledForPackage3Worker'			QuirkIsEnabledForPackage3Worker				kernel32	1115
imp	'QuirkIsEnabledForPackage4Worker'			QuirkIsEnabledForPackage4Worker				kernel32	1116
imp	'QuirkIsEnabledForPackageWorker'			QuirkIsEnabledForPackageWorker				kernel32	1117
imp	'QuirkIsEnabledForProcessWorker'			QuirkIsEnabledForProcessWorker				kernel32	1118
imp	'QuirkIsEnabledWorker'					QuirkIsEnabledWorker					kernel32	1119
imp	'RaiseException'					RaiseException						kernel32	0
imp	'RaiseFailFastException'				RaiseFailFastException					kernel32	0
imp	'RaiseInvalid16BitExeError'				RaiseInvalid16BitExeError				kernel32	1122
imp	'ReadConsole'						ReadConsoleW						kernel32	0	5
imp	'ReadConsoleInput'					ReadConsoleInputW					kernel32	0	4
imp	'ReadConsoleOutput'					ReadConsoleOutputW					kernel32	0	5
imp	'ReadConsoleOutputAttribute'				ReadConsoleOutputAttribute				kernel32	0	5
imp	'ReadConsoleOutputCharacter'				ReadConsoleOutputCharacterW				kernel32	0	5
imp	'ReadDirectoryChanges'					ReadDirectoryChangesW					kernel32	0
imp	'ReadFile'						ReadFile						kernel32	0	5
imp	'ReadFileEx'						ReadFileEx						kernel32	0	5
imp	'ReadFileScatter'					ReadFileScatter						kernel32	0	5
imp	'ReadProcessMemory'					ReadProcessMemory					kernel32	0
imp	'ReadThreadProfilingData'				ReadThreadProfilingData					kernel32	1141
imp	'ReclaimVirtualMemory'					ReclaimVirtualMemory					kernel32	0
imp	'RegDisablePredefinedCacheEx'				RegDisablePredefinedCacheEx				kernel32	0
imp	'RegisterApplicationRecoveryCallback'			RegisterApplicationRecoveryCallback			kernel32	1184
imp	'RegisterApplicationRestart'				RegisterApplicationRestart				kernel32	1185
imp	'RegisterBadMemoryNotification'				RegisterBadMemoryNotification				kernel32	0
imp	'RegisterConsoleIME'					RegisterConsoleIME					kernel32	1187
imp	'RegisterConsoleOS2'					RegisterConsoleOS2					kernel32	1188
imp	'RegisterConsoleVDM'					RegisterConsoleVDM					kernel32	1189
imp	'RegisterWaitForInputIdle'				RegisterWaitForInputIdle				kernel32	1190
imp	'RegisterWaitForSingleObject'				RegisterWaitForSingleObject				kernel32	1191	6
imp	'RegisterWaitUntilOOBECompleted'			RegisterWaitUntilOOBECompleted				kernel32	1193
imp	'RegisterWowBaseHandlers'				RegisterWowBaseHandlers					kernel32	1194
imp	'RegisterWowExec'					RegisterWowExec						kernel32	1195
imp	'ReleaseActCtx'						ReleaseActCtx						kernel32	0
imp	'ReleaseActCtxWorker'					ReleaseActCtxWorker					kernel32	1197
imp	'ReleaseMutex'						ReleaseMutex						kernel32	0	1
imp	'ReleaseSRWLockExclusive'				ReleaseSRWLockExclusive					kernel32	0	1
imp	'ReleaseSRWLockShared'					ReleaseSRWLockShared					kernel32	0	1
imp	'ReleaseSemaphore'					ReleaseSemaphore					kernel32	0	3
imp	'RemoveDirectoryTransacted'				RemoveDirectoryTransactedW				kernel32	1206
imp	'RemoveDllDirectory'					RemoveDllDirectory					kernel32	0
imp	'RemoveLocalAlternateComputerName'			RemoveLocalAlternateComputerNameW			kernel32	1210
imp	'RemoveSecureMemoryCacheCallback'			RemoveSecureMemoryCacheCallback				kernel32	1211
imp	'RemoveVectoredContinueHandler'				RemoveVectoredContinueHandler				kernel32	0	1
imp	'RemoveVectoredExceptionHandler'			RemoveVectoredExceptionHandler				kernel32	0	1
imp	'ReplaceFile'						ReplaceFileW						kernel32	0
imp	'ReplacePartitionUnit'					ReplacePartitionUnit					kernel32	1217
imp	'RequestDeviceWakeup'					RequestDeviceWakeup					kernel32	1218
imp	'RequestWakeupLatency'					RequestWakeupLatency					kernel32	1219
imp	'ResetEvent'						ResetEvent						kernel32	0	1
imp	'ResetWriteWatch'					ResetWriteWatch						kernel32	0
imp	'ResolveDelayLoadedAPI'					ResolveDelayLoadedAPI					kernel32	0
imp	'ResolveDelayLoadsFromDll'				ResolveDelayLoadsFromDll				kernel32	0
imp	'ResolveLocaleName'					ResolveLocaleName					kernel32	0
imp	'ResumeThread'						ResumeThread						kernel32	0
imp	'ScrollConsoleScreenBuffer'				ScrollConsoleScreenBufferW				kernel32	0
imp	'SearchPath'						SearchPathW						kernel32	0
imp	'SetCalendarInfo'					SetCalendarInfoW					kernel32	0
imp	'SetComPlusPackageInstallStatus'			SetComPlusPackageInstallStatus				kernel32	1251
imp	'SetCommBreak'						SetCommBreak						kernel32	0
imp	'SetCommConfig'						SetCommConfig						kernel32	0
imp	'SetCommMask'						SetCommMask						kernel32	0
imp	'SetCommState'						SetCommState						kernel32	0
imp	'SetCommTimeouts'					SetCommTimeouts						kernel32	0
imp	'SetComputerName'					SetComputerNameW					kernel32	0
imp	'SetComputerNameEx'					SetComputerNameExW					kernel32	0
imp	'SetConsoleActiveScreenBuffer'				SetConsoleActiveScreenBuffer				kernel32	0	1	# TODO(jart): 6.2 and higher
imp	'SetConsoleCP'						SetConsoleCP						kernel32	0	1	# TODO(jart): 6.2 and higher
imp	'SetConsoleCtrlHandler'					SetConsoleCtrlHandler					kernel32	0	2
imp	'SetConsoleCursor'					SetConsoleCursor					kernel32	1265
imp	'SetConsoleCursorInfo'					SetConsoleCursorInfo					kernel32	0	2
imp	'SetConsoleCursorMode'					SetConsoleCursorMode					kernel32	1267
imp	'SetConsoleCursorPosition'				SetConsoleCursorPosition				kernel32	0	2
imp	'SetConsoleDisplayMode'					SetConsoleDisplayMode					kernel32	0
imp	'SetConsoleFont'					SetConsoleFont						kernel32	1270
imp	'SetConsoleHardwareState'				SetConsoleHardwareState					kernel32	1271
imp	'SetConsoleHistoryInfo'					SetConsoleHistoryInfo					kernel32	0
imp	'SetConsoleIcon'					SetConsoleIcon						kernel32	1273
imp	'SetConsoleKeyShortcuts'				SetConsoleKeyShortcuts					kernel32	1276
imp	'SetConsoleLocalEUDC'					SetConsoleLocalEUDC					kernel32	1277
imp	'SetConsoleMaximumWindowSize'				SetConsoleMaximumWindowSize				kernel32	1278
imp	'SetConsoleMenuClose'					SetConsoleMenuClose					kernel32	1279
imp	'SetConsoleMode'					SetConsoleMode						kernel32	0	2
imp	'SetConsoleNlsMode'					SetConsoleNlsMode					kernel32	1281
imp	'SetConsoleOS2OemFormat'				SetConsoleOS2OemFormat					kernel32	1284
imp	'SetConsoleOutputCP'					SetConsoleOutputCP					kernel32	0	1
imp	'SetConsolePalette'					SetConsolePalette					kernel32	1286
imp	'SetConsoleScreenBufferInfoEx'				SetConsoleScreenBufferInfoEx				kernel32	0	2
imp	'SetConsoleScreenBufferSize'				SetConsoleScreenBufferSize				kernel32	0	2
imp	'SetConsoleTextAttribute'				SetConsoleTextAttribute					kernel32	0
imp	'SetConsoleTitle'					SetConsoleTitleW					kernel32	0	1
imp	'SetConsoleWindowInfo'					SetConsoleWindowInfo					kernel32	0	3
imp	'SetCriticalSectionSpinCount'				SetCriticalSectionSpinCount				kernel32	0	2
imp	'SetCurrentConsoleFontEx'				SetCurrentConsoleFontEx					kernel32	0
imp	'SetDefaultCommConfig'					SetDefaultCommConfigW					kernel32	1298
imp	'SetDefaultDllDirectories'				SetDefaultDllDirectories				kernel32	0	1	# Windows 8+, KB2533623 on Windows 7
imp	'SetDllDirectory'					SetDllDirectoryW					kernel32	1301
imp	'SetDynamicTimeZoneInformation'				SetDynamicTimeZoneInformation				kernel32	0
imp	'SetEndOfFile'						SetEndOfFile						kernel32	0	1
imp	'SetEnvironmentVariable'				SetEnvironmentVariableW					kernel32	0	2
imp	'SetErrorMode'						SetErrorMode						kernel32	0	1
imp	'SetEvent'						SetEvent						kernel32	0	1
imp	'SetFileApisToANSI'					SetFileApisToANSI					kernel32	0
imp	'SetFileApisToOEM'					SetFileApisToOEM					kernel32	0
imp	'SetFileAttributes'					SetFileAttributesW					kernel32	0	2
imp	'SetFileAttributesTransacted'				SetFileAttributesTransactedW				kernel32	1315
imp	'SetFileBandwidthReservation'				SetFileBandwidthReservation				kernel32	1317
imp	'SetFileCompletionNotificationModes'			SetFileCompletionNotificationModes			kernel32	1318	2
imp	'SetFileInformationByHandle'				SetFileInformationByHandle				kernel32	0
imp	'SetFileIoOverlappedRange'				SetFileIoOverlappedRange				kernel32	0
imp	'SetFilePointer'					SetFilePointer						kernel32	0	4
imp	'SetFilePointerEx'					SetFilePointerEx					kernel32	0	4
imp	'SetFileShortName'					SetFileShortNameW					kernel32	1324
imp	'SetFileTime'						SetFileTime						kernel32	0	4
imp	'SetFileValidData'					SetFileValidData					kernel32	0	2
imp	'SetFirmwareEnvironmentVariable'			SetFirmwareEnvironmentVariableW				kernel32	1330
imp	'SetFirmwareEnvironmentVariableEx'			SetFirmwareEnvironmentVariableExW			kernel32	1329
imp	'SetHandleCount'					SetHandleCount						kernel32	0	1
imp	'SetHandleInformation'					SetHandleInformation					kernel32	0	3
imp	'SetInformationJobObject'				SetInformationJobObject					kernel32	1333
imp	'SetIoRateControlInformationJobObject'			SetIoRateControlInformationJobObject			kernel32	1334
imp	'SetLastError'						SetLastError						kernel32	0	1
imp	'SetLocalPrimaryComputerName'				SetLocalPrimaryComputerNameW				kernel32	1338
imp	'SetLocalTime'						SetLocalTime						kernel32	0
imp	'SetLocaleInfo'						SetLocaleInfoW						kernel32	0
imp	'SetMailslotInfo'					SetMailslotInfo						kernel32	1342
imp	'SetMessageWaitingIndicator'				SetMessageWaitingIndicator				kernel32	1343
imp	'SetNamedPipeAttribute'					SetNamedPipeAttribute					kernel32	1344
imp	'SetNamedPipeHandleState'				SetNamedPipeHandleState					kernel32	0	4
imp	'SetPriorityClass'					SetPriorityClass					kernel32	0	2
imp	'SetProcessAffinityMask'				SetProcessAffinityMask					kernel32	1347	2
imp	'SetProcessAffinityUpdateMode'				SetProcessAffinityUpdateMode				kernel32	0
imp	'SetProcessDEPPolicy'					SetProcessDEPPolicy					kernel32	1349
imp	'SetProcessDefaultCpuSets'				SetProcessDefaultCpuSets				kernel32	0
imp	'SetProcessInformation'					SetProcessInformation					kernel32	0
imp	'SetProcessMitigationPolicy'				SetProcessMitigationPolicy				kernel32	0
imp	'SetProcessPreferredUILanguages'			SetProcessPreferredUILanguages				kernel32	0
imp	'SetProcessPriorityBoost'				SetProcessPriorityBoost					kernel32	0	2
imp	'SetProcessShutdownParameters'				SetProcessShutdownParameters				kernel32	0
imp	'SetProcessWorkingSetSize'				SetProcessWorkingSetSize				kernel32	1356	3
imp	'SetProcessWorkingSetSizeEx'				SetProcessWorkingSetSizeEx				kernel32	0	4
imp	'SetProtectedPolicy'					SetProtectedPolicy					kernel32	0
imp	'SetSearchPathMode'					SetSearchPathMode					kernel32	1359
imp	'SetStdHandle'						SetStdHandle						kernel32	0	2
imp	'SetSystemFileCacheSize'				SetSystemFileCacheSize					kernel32	0
imp	'SetSystemPowerState'					SetSystemPowerState					kernel32	1363
imp	'SetSystemTime'						SetSystemTime						kernel32	0
imp	'SetSystemTimeAdjustment'				SetSystemTimeAdjustment					kernel32	0
imp	'SetTapeParameters'					SetTapeParameters					kernel32	1366
imp	'SetTapePosition'					SetTapePosition						kernel32	1367
imp	'SetTermsrvAppInstallMode'				SetTermsrvAppInstallMode				kernel32	1368
imp	'SetThreadAffinityMask'					SetThreadAffinityMask					kernel32	1369	2
imp	'SetThreadContext'					SetThreadContext					kernel32	0
imp	'SetThreadErrorMode'					SetThreadErrorMode					kernel32	0
imp	'SetThreadExecutionState'				SetThreadExecutionState					kernel32	1373
imp	'SetThreadGroupAffinity'				SetThreadGroupAffinity					kernel32	0
imp	'SetThreadIdealProcessor'				SetThreadIdealProcessor					kernel32	0
imp	'SetThreadIdealProcessorEx'				SetThreadIdealProcessorEx				kernel32	0
imp	'SetThreadInformation'					SetThreadInformation					kernel32	0
imp	'SetThreadLocale'					SetThreadLocale						kernel32	0
imp	'SetThreadPreferredUILanguages'				SetThreadPreferredUILanguages				kernel32	0
imp	'SetThreadPriority'					SetThreadPriority					kernel32	0	2
imp	'SetThreadPriorityBoost'				SetThreadPriorityBoost					kernel32	0	2
imp	'SetThreadSelectedCpuSets'				SetThreadSelectedCpuSets				kernel32	0
imp	'SetThreadStackGuarantee'				SetThreadStackGuarantee					kernel32	0
imp	'SetThreadUILanguage'					SetThreadUILanguage					kernel32	0
imp	'SetThreadpoolStackInformation'				SetThreadpoolStackInformation				kernel32	0
imp	'SetThreadpoolThreadMinimum'				SetThreadpoolThreadMinimum				kernel32	0
imp	'SetTimeZoneInformation'				SetTimeZoneInformation					kernel32	0
imp	'SetTimerQueueTimer'					SetTimerQueueTimer					kernel32	1394
imp	'SetUmsThreadInformation'				SetUmsThreadInformation					kernel32	1395
imp	'SetUnhandledExceptionFilter'				SetUnhandledExceptionFilter				kernel32	0	1
imp	'SetUserGeoID'						SetUserGeoID						kernel32	0
imp	'SetVDMCurrentDirectories'				SetVDMCurrentDirectories				kernel32	1399
imp	'SetVolumeLabel'					SetVolumeLabelW						kernel32	1401
imp	'SetVolumeMountPoint'					SetVolumeMountPointW					kernel32	1403
imp	'SetVolumeMountPointWStub'				SetVolumeMountPointWStub				kernel32	1404
imp	'SetWaitableTimer'					SetWaitableTimer					kernel32	0	6
imp	'SetWaitableTimerEx'					SetWaitableTimerEx					kernel32	0
imp	'SetXStateFeaturesMask'					SetXStateFeaturesMask					kernel32	0
imp	'SetupComm'						SetupComm						kernel32	0
imp	'ShowConsoleCursor'					ShowConsoleCursor					kernel32	1409
imp	'SignalObjectAndWait'					SignalObjectAndWait					kernel32	0
imp	'SizeofResource'					SizeofResource						kernel32	0
imp	'Sleep'							Sleep							kernel32	0	1
imp	'SleepConditionVariableCS'				SleepConditionVariableCS				kernel32	0
imp	'SleepConditionVariableSRW'				SleepConditionVariableSRW				kernel32	0
imp	'SleepEx'						SleepEx							kernel32	0	2
imp	'SortCloseHandle'					SortCloseHandle						kernel32	1416
imp	'SortGetHandle'						SortGetHandle						kernel32	1417
imp	'SuspendThread'						SuspendThread						kernel32	0	1
imp	'SwitchToFiber'						SwitchToFiber						kernel32	0
imp	'SwitchToThread'					SwitchToThread						kernel32	0
imp	'SystemTimeToFileTime'					SystemTimeToFileTime					kernel32	0	2
imp	'SystemTimeToTzSpecificLocalTime'			SystemTimeToTzSpecificLocalTime				kernel32	0
imp	'SystemTimeToTzSpecificLocalTimeEx'			SystemTimeToTzSpecificLocalTimeEx			kernel32	0
imp	'TerminateJobObject'					TerminateJobObject					kernel32	1426
imp	'TerminateThread'					TerminateThread						kernel32	0	2
imp	'TermsrvAppInstallMode'					TermsrvAppInstallMode					kernel32	1429
imp	'TermsrvConvertSysRootToUserDir'			TermsrvConvertSysRootToUserDir				kernel32	1430
imp	'TermsrvCreateRegEntry'					TermsrvCreateRegEntry					kernel32	1431
imp	'TermsrvDeleteKey'					TermsrvDeleteKey					kernel32	1432
imp	'TermsrvDeleteValue'					TermsrvDeleteValue					kernel32	1433
imp	'TermsrvGetPreSetValue'					TermsrvGetPreSetValue					kernel32	1434
imp	'TermsrvGetWindowsDirectory'				TermsrvGetWindowsDirectoryW				kernel32	1436
imp	'TermsrvOpenRegEntry'					TermsrvOpenRegEntry					kernel32	1437
imp	'TermsrvOpenUserClasses'				TermsrvOpenUserClasses					kernel32	1438
imp	'TermsrvRestoreKey'					TermsrvRestoreKey					kernel32	1439
imp	'TermsrvSetKeySecurity'					TermsrvSetKeySecurity					kernel32	1440
imp	'TermsrvSetValueKey'					TermsrvSetValueKey					kernel32	1441
imp	'TermsrvSyncUserIniFileExt'				TermsrvSyncUserIniFileExt				kernel32	1442
imp	'Thread32First'						Thread32First						kernel32	1443
imp	'Thread32Next'						Thread32Next						kernel32	1444
imp	'TlsAlloc'						TlsAlloc						kernel32	0	0
imp	'TlsFree'						TlsFree							kernel32	0	1
imp	'TlsGetValue'						TlsGetValue						kernel32	0	1
imp	'TlsSetValue'						TlsSetValue						kernel32	0	2
imp	'Toolhelp32ReadProcessMemory'				Toolhelp32ReadProcessMemory				kernel32	1449
imp	'TransactNamedPipe'					TransactNamedPipe					kernel32	0	7
imp	'TransmitCommChar'					TransmitCommChar					kernel32	0	2
imp	'TryAcquireSRWLockExclusive'				TryAcquireSRWLockExclusive				kernel32	0	1
imp	'TryAcquireSRWLockShared'				TryAcquireSRWLockShared					kernel32	0	1
imp	'TryEnterCriticalSection'				TryEnterCriticalSection					kernel32	0	1
imp	'TrySubmitThreadpoolCallback'				TrySubmitThreadpoolCallback				kernel32	0
imp	'TzSpecificLocalTimeToSystemTime'			TzSpecificLocalTimeToSystemTime				kernel32	0
imp	'TzSpecificLocalTimeToSystemTimeEx'			TzSpecificLocalTimeToSystemTimeEx			kernel32	0
imp	'UTRegister'						UTRegister						kernel32	1458
imp	'UTUnRegister'						UTUnRegister						kernel32	1459
imp	'UmsThreadYield'					UmsThreadYield						kernel32	1460
imp	'UnhandledExceptionFilter'				UnhandledExceptionFilter				kernel32	0
imp	'UnlockFile'						UnlockFile						kernel32	0	5
imp	'UnmapViewOfFile2'					UnmapViewOfFile2					kernel32	0	2
imp	'UnmapViewOfFileEx'					UnmapViewOfFileEx					kernel32	0	3
imp	'UnregisterApplicationRecoveryCallback'			UnregisterApplicationRecoveryCallback			kernel32	1466
imp	'UnregisterApplicationRestart'				UnregisterApplicationRestart				kernel32	1467
imp	'UnregisterBadMemoryNotification'			UnregisterBadMemoryNotification				kernel32	0
imp	'UnregisterConsoleIME'					UnregisterConsoleIME					kernel32	1469
imp	'UnregisterWait'					UnregisterWait						kernel32	1470
imp	'UnregisterWaitEx'					UnregisterWaitEx					kernel32	0
imp	'UnregisterWaitUntilOOBECompleted'			UnregisterWaitUntilOOBECompleted			kernel32	1472
imp	'UpdateCalendarDayOfWeek'				UpdateCalendarDayOfWeek					kernel32	1473
imp	'UpdateProcThreadAttribute'				UpdateProcThreadAttribute				kernel32	0	7
imp	'UpdateResource'					UpdateResourceW						kernel32	1476
imp	'VDMConsoleOperation'					VDMConsoleOperation					kernel32	1477
imp	'VDMOperationStarted'					VDMOperationStarted					kernel32	1478
imp	'VerLanguageName'					VerLanguageNameW					kernel32	0
imp	'VerifyConsoleIoHandle'					VerifyConsoleIoHandle					kernel32	1482
imp	'VerifyScripts'						VerifyScripts						kernel32	0
imp	'VerifyVersionInfo'					VerifyVersionInfoW					kernel32	1485
imp	'VirtualAlloc'						VirtualAlloc						kernel32	0	4
imp	'VirtualAllocEx'					VirtualAllocEx						kernel32	0	5
imp	'VirtualAllocExNuma'					VirtualAllocExNuma					kernel32	0
imp	'VirtualFree'						VirtualFree						kernel32	0	3
imp	'VirtualFreeEx'						VirtualFreeEx						kernel32	0
imp	'VirtualLock'						VirtualLock						kernel32	0
imp	'VirtualProtectEx'					VirtualProtectEx					kernel32	0
imp	'VirtualQuery'						VirtualQuery						kernel32	0	3
imp	'VirtualQueryEx'					VirtualQueryEx						kernel32	0
imp	'VirtualUnlock'						VirtualUnlock						kernel32	0
imp	'WTSGetActiveConsoleSessionId'				WTSGetActiveConsoleSessionId				kernel32	1497
imp	'WaitCommEvent'						WaitCommEvent						kernel32	0
imp	'WaitForDebugEvent'					WaitForDebugEvent					kernel32	0
imp	'WaitForMultipleObjectsEx'				WaitForMultipleObjectsEx				kernel32	0	5
imp	'WaitForSingleObjectEx'					WaitForSingleObjectEx					kernel32	0	3
imp	'WaitNamedPipe'						WaitNamedPipeW						kernel32	0
imp	'WerGetFlags'						WerGetFlags						kernel32	0
imp	'WerGetFlagsWorker'					WerGetFlagsWorker					kernel32	1514
imp	'WerRegisterFile'					WerRegisterFile						kernel32	0
imp	'WerRegisterFileWorker'					WerRegisterFileWorker					kernel32	1520
imp	'WerRegisterMemoryBlock'				WerRegisterMemoryBlock					kernel32	0
imp	'WerRegisterMemoryBlockWorker'				WerRegisterMemoryBlockWorker				kernel32	1522
imp	'WerRegisterRuntimeExceptionModule'			WerRegisterRuntimeExceptionModule			kernel32	0
imp	'WerRegisterRuntimeExceptionModuleWorker'		WerRegisterRuntimeExceptionModuleWorker			kernel32	1524
imp	'WerSetFlags'						WerSetFlags						kernel32	0
imp	'WerSetFlagsWorker'					WerSetFlagsWorker					kernel32	1526
imp	'WerUnregisterFile'					WerUnregisterFile					kernel32	0
imp	'WerUnregisterFileWorker'				WerUnregisterFileWorker					kernel32	1532
imp	'WerUnregisterMemoryBlock'				WerUnregisterMemoryBlock				kernel32	0
imp	'WerUnregisterMemoryBlockWorker'			WerUnregisterMemoryBlockWorker				kernel32	1534
imp	'WerUnregisterRuntimeExceptionModule'			WerUnregisterRuntimeExceptionModule			kernel32	0
imp	'WerUnregisterRuntimeExceptionModuleWorker'		WerUnregisterRuntimeExceptionModuleWorker		kernel32	1536
imp	'WerpGetDebugger'					WerpGetDebugger						kernel32	1537
imp	'WerpInitiateRemoteRecovery'				WerpInitiateRemoteRecovery				kernel32	1538
imp	'WerpLaunchAeDebug'					WerpLaunchAeDebug					kernel32	1539
imp	'WerpNotifyLoadStringResourceWorker'			WerpNotifyLoadStringResourceWorker			kernel32	1540
imp	'WerpNotifyUseStringResourceWorker'			WerpNotifyUseStringResourceWorker			kernel32	1541
imp	'WideCharToMultiByte'					WideCharToMultiByte					kernel32	1553    8
imp	'WinExec'						WinExec							kernel32	1543
imp	'WriteConsole'						WriteConsoleW						kernel32	0	5
imp	'WriteConsoleInput'					WriteConsoleInputW					kernel32	0	4
imp	'WriteConsoleInputVDMW'					WriteConsoleInputVDMW					kernel32	1554
imp	'WriteConsoleOutput'					WriteConsoleOutputW					kernel32	0
imp	'WriteConsoleOutputAttribute'				WriteConsoleOutputAttribute				kernel32	0	5
imp	'WriteConsoleOutputCharacter'				WriteConsoleOutputCharacterW				kernel32	0	5
imp	'WriteFile'						WriteFile						kernel32	0	5
imp	'WriteFileEx'						WriteFileEx						kernel32	0	5
imp	'WriteFileGather'					WriteFileGather						kernel32	0	5
imp	'WritePrivateProfileSection'				WritePrivateProfileSectionW				kernel32	1566
imp	'WritePrivateProfileString'				WritePrivateProfileStringW				kernel32	1568
imp	'WritePrivateProfileStruct'				WritePrivateProfileStructW				kernel32	1570
imp	'WriteProcessMemory'					WriteProcessMemory					kernel32	0
imp	'WriteProfileSection'					WriteProfileSectionW					kernel32	1573
imp	'WriteProfileString'					WriteProfileStringW					kernel32	1575
imp	'WriteTapemark'						WriteTapemark						kernel32	1576
imp	'ZombifyActCtx'						ZombifyActCtx						kernel32	0
imp	'ZombifyActCtxWorker'					ZombifyActCtxWorker					kernel32	1578
imp	'__CloseHandle'						CloseHandle						kernel32	0	1
imp	'__CreateDirectory'					CreateDirectoryW					kernel32	0	2
imp	'__CreateFile'						CreateFileW						kernel32	0	7
imp	'__CreateFileMapping'					CreateFileMappingW					kernel32	0	6
imp	'__CreateFileMappingNuma'				CreateFileMappingNumaW					kernel32	0	7
imp	'__CreateNamedPipe'					CreateNamedPipeW					kernel32	0	8
imp	'__CreatePipe'						CreatePipe						kernel32	0	4
imp	'__CreateProcess'					CreateProcessW						kernel32	0	10
imp	'__CreateSymbolicLink'					CreateSymbolicLinkW					kernel32	0	3
imp	'__CreateThread'					CreateThread						kernel32	0	6
imp	'__DeleteFile'						DeleteFileW						kernel32	0	1
imp	'__DeviceIoControl'					DeviceIoControl						kernel32	0	8
imp	'__FindClose'						FindClose						kernel32	0	1
imp	'__FindFirstFile'					FindFirstFileW						kernel32	0	2
imp	'__FindNextFile'					FindNextFileW						kernel32	0	2
imp	'__FlushFileBuffers'					FlushFileBuffers					kernel32	0	1
imp	'__FlushViewOfFile'					FlushViewOfFile						kernel32	0	2
imp	'__GenerateConsoleCtrlEvent'				GenerateConsoleCtrlEvent				kernel32	0	2
imp	'__GetExitCodeProcess'					GetExitCodeProcess					kernel32	0	2
imp	'__GetFileAttributes'					GetFileAttributesW					kernel32	0	1
imp	'__LockFileEx'						LockFileEx						kernel32	0	6
imp	'__MapViewOfFileEx'					MapViewOfFileEx						kernel32	0	6
imp	'__MapViewOfFileExNuma'					MapViewOfFileExNuma					kernel32	0	7
imp	'__MoveFileEx'						MoveFileExW						kernel32	0	3
imp	'__OpenProcess'						OpenProcess						kernel32	0	3
imp	'__ReOpenFile'						ReOpenFile						kernel32	0	4	# TODO(jart): 6.2 and higher
imp	'__RemoveDirectory'					RemoveDirectoryW					kernel32	0	1
imp	'__SetCurrentDirectory'					SetCurrentDirectoryW					kernel32	0	1
imp	'__TerminateProcess'					TerminateProcess					kernel32	0	2
imp	'__UnlockFileEx'					UnlockFileEx						kernel32	0	5
imp	'__UnmapViewOfFile'					UnmapViewOfFile						kernel32	0	1
imp	'__VirtualProtect'					VirtualProtect						kernel32	0	4
imp	'__WaitForMultipleObjects'				WaitForMultipleObjects					kernel32	0	4
imp	'__WaitForSingleObject'					WaitForSingleObject					kernel32	0	2

# ADVAPI32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'AbortSystemShutdown'					AbortSystemShutdownW					advapi32	1006
imp	'AccessCheck'						AccessCheck						advapi32	0	8
imp	'AccessCheckAndAuditAlarm'				AccessCheckAndAuditAlarmW				advapi32	0
imp	'AccessCheckByType'					AccessCheckByType					advapi32	0
imp	'AccessCheckByTypeAndAuditAlarm'			AccessCheckByTypeAndAuditAlarmW				advapi32	0
imp	'AccessCheckByTypeResultList'				AccessCheckByTypeResultList				advapi32	0
imp	'AccessCheckByTypeResultListAndAuditAlarm'		AccessCheckByTypeResultListAndAuditAlarmW		advapi32	0
imp	'AccessCheckByTypeResultListAndAuditAlarmByHandle'	AccessCheckByTypeResultListAndAuditAlarmByHandleW	advapi32	0
imp	'AddAccessAllowedAce'					AddAccessAllowedAce					advapi32	0
imp	'AddAccessAllowedAceEx'					AddAccessAllowedAceEx					advapi32	0
imp	'AddAccessAllowedObjectAce'				AddAccessAllowedObjectAce				advapi32	0
imp	'AddAccessDeniedAce'					AddAccessDeniedAce					advapi32	0
imp	'AddAccessDeniedAceEx'					AddAccessDeniedAceEx					advapi32	0
imp	'AddAccessDeniedObjectAce'				AddAccessDeniedObjectAce				advapi32	0
imp	'AddAce'						AddAce							advapi32	0
imp	'AddAuditAccessAce'					AddAuditAccessAce					advapi32	0
imp	'AddAuditAccessAceEx'					AddAuditAccessAceEx					advapi32	0
imp	'AddAuditAccessObjectAce'				AddAuditAccessObjectAce					advapi32	0
imp	'AddConditionalAce'					AddConditionalAce					advapi32	1028 # Windows 7+
imp	'AddMandatoryAce'					AddMandatoryAce						advapi32	0
imp	'AddUsersToEncryptedFile'				AddUsersToEncryptedFile					advapi32	1030
imp	'AdjustTokenGroups'					AdjustTokenGroups					advapi32	0
imp	'AdjustTokenPrivileges'					AdjustTokenPrivileges					advapi32	0	6
imp	'AllocateAndInitializeSid'				AllocateAndInitializeSid				advapi32	0
imp	'AllocateLocallyUniqueId'				AllocateLocallyUniqueId					advapi32	0
imp	'AreAllAccessesGranted'					AreAllAccessesGranted					advapi32	0
imp	'AreAnyAccessesGranted'					AreAnyAccessesGranted					advapi32	0
imp	'AuditComputeEffectivePolicyBySid'			AuditComputeEffectivePolicyBySid			advapi32	1038
imp	'AuditComputeEffectivePolicyByToken'			AuditComputeEffectivePolicyByToken			advapi32	1039
imp	'AuditEnumerateCategories'				AuditEnumerateCategories				advapi32	1040
imp	'AuditEnumeratePerUserPolicy'				AuditEnumeratePerUserPolicy				advapi32	1041
imp	'AuditEnumerateSubCategories'				AuditEnumerateSubCategories				advapi32	1042
imp	'AuditFree'						AuditFree						advapi32	1043
imp	'AuditLookupCategoryGuidFromCategoryId'			AuditLookupCategoryGuidFromCategoryId			advapi32	1044
imp	'AuditLookupCategoryIdFromCategoryGuid'			AuditLookupCategoryIdFromCategoryGuid			advapi32	1045
imp	'AuditLookupCategoryName'				AuditLookupCategoryNameW				advapi32	1047
imp	'AuditLookupSubCategoryName'				AuditLookupSubCategoryNameW				advapi32	1049
imp	'AuditQueryPerUserPolicy'				AuditQueryPerUserPolicy					advapi32	1052
imp	'AuditQuerySecurity'					AuditQuerySecurity					advapi32	1053
imp	'AuditQuerySystemPolicy'				AuditQuerySystemPolicy					advapi32	1054
imp	'AuditSetPerUserPolicy'					AuditSetPerUserPolicy					advapi32	1057
imp	'AuditSetSecurity'					AuditSetSecurity					advapi32	1058
imp	'AuditSetSystemPolicy'					AuditSetSystemPolicy					advapi32	1059
imp	'BackupEventLog'					BackupEventLogW						advapi32	1061
imp	'BuildExplicitAccessWithName'				BuildExplicitAccessWithNameW				advapi32	1076
imp	'BuildImpersonateExplicitAccessWithName'		BuildImpersonateExplicitAccessWithNameW			advapi32	1078
imp	'BuildImpersonateTrustee'				BuildImpersonateTrusteeW				advapi32	1080
imp	'BuildSecurityDescriptor'				BuildSecurityDescriptorW				advapi32	1082
imp	'BuildTrusteeWithName'					BuildTrusteeWithNameW					advapi32	1084
imp	'BuildTrusteeWithObjectsAndName'			BuildTrusteeWithObjectsAndNameW				advapi32	1086
imp	'BuildTrusteeWithObjectsAndSid'				BuildTrusteeWithObjectsAndSidW				advapi32	1088
imp	'BuildTrusteeWithSid'					BuildTrusteeWithSidW					advapi32	1090
imp	'ChangeServiceConfig'					ChangeServiceConfigW					advapi32	1095
imp	'ChangeServiceConfig2W'					ChangeServiceConfig2W					advapi32	1093
imp	'CheckTokenMembership'					CheckTokenMembership					advapi32	0
imp	'ClearEventLog'						ClearEventLogW						advapi32	1099
imp	'CloseEncryptedFileRaw'					CloseEncryptedFileRaw					advapi32	1101
imp	'CloseEventLog'						CloseEventLog						advapi32	1102
imp	'CloseServiceHandle'					CloseServiceHandle					advapi32	1103
imp	'CloseThreadWaitChainSession'				CloseThreadWaitChainSession				advapi32	1104
imp	'CloseTrace'						CloseTrace						advapi32	1105
imp	'ControlService'					ControlService						advapi32	1108
imp	'ControlServiceEx'					ControlServiceExW					advapi32	1110
imp	'ControlTrace'						ControlTraceW						advapi32	1112
imp	'ConvertSecurityDescriptorToStringSecurityDescriptor'	ConvertSecurityDescriptorToStringSecurityDescriptorW	advapi32	1123
imp	'ConvertSidToStringSid'					ConvertSidToStringSidW					advapi32	1125
imp	'ConvertStringSDToSDDomain'				ConvertStringSDToSDDomainW				advapi32	1127
imp	'ConvertStringSecurityDescriptorToSecurityDescriptor'	ConvertStringSecurityDescriptorToSecurityDescriptorW	advapi32	1131
imp	'ConvertStringSidToSid'					ConvertStringSidToSidW					advapi32	1133
imp	'ConvertToAutoInheritPrivateObjectSecurity'		ConvertToAutoInheritPrivateObjectSecurity		advapi32	0
imp	'CopySid'						CopySid							advapi32	0
imp	'CreatePrivateObjectSecurity'				CreatePrivateObjectSecurity				advapi32	0
imp	'CreatePrivateObjectSecurityEx'				CreatePrivateObjectSecurityEx				advapi32	0
imp	'CreatePrivateObjectSecurityWithMultipleInheritance'	CreatePrivateObjectSecurityWithMultipleInheritance	advapi32	0
imp	'CreateProcessAsUser'					CreateProcessAsUserW					advapi32	0	11
imp	'CreateProcessWithLogon'				CreateProcessWithLogonW					advapi32	1142
imp	'CreateProcessWithToken'				CreateProcessWithTokenW					advapi32	1143
imp	'CreateRestrictedToken'					CreateRestrictedToken					advapi32	0
imp	'CreateService'						CreateServiceW						advapi32	1147
imp	'CreateTraceInstanceId'					CreateTraceInstanceId					advapi32	0
imp	'CreateWellKnownSid'					CreateWellKnownSid					advapi32	0
imp	'CredDelete'						CredDeleteW						advapi32	1152
imp	'CredEnumerate'						CredEnumerateW						advapi32	1155
imp	'CredFindBestCredential'				CredFindBestCredentialW					advapi32	1157
imp	'CredFree'						CredFree						advapi32	1158
imp	'CredGetSessionTypes'					CredGetSessionTypes					advapi32	1159
imp	'CredGetTargetInfo'					CredGetTargetInfoW					advapi32	1161
imp	'CredIsMarshaledCredential'				CredIsMarshaledCredentialW				advapi32	1163
imp	'CredIsProtected'					CredIsProtectedW					advapi32	1165
imp	'CredMarshalCredential'					CredMarshalCredentialW					advapi32	1167
imp	'CredProtect'						CredProtectW						advapi32	1172
imp	'CredRead'						CredReadW						advapi32	1177
imp	'CredReadDomainCredentials'				CredReadDomainCredentialsW				advapi32	1176
imp	'CredRename'						CredRenameW						advapi32	1179
imp	'CredUnmarshalCredential'				CredUnmarshalCredentialW				advapi32	1182
imp	'CredUnprotect'						CredUnprotectW						advapi32	1184
imp	'CredWrite'						CredWriteW						advapi32	1188
imp	'CredWriteDomainCredentials'				CredWriteDomainCredentialsW				advapi32	1187
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
imp	'CryptEnumProviderTypes'				CryptEnumProviderTypesW					advapi32	1207
imp	'CryptEnumProviders'					CryptEnumProvidersW					advapi32	1209
imp	'CryptExportKey'					CryptExportKey						advapi32	1210
imp	'CryptGenKey'						CryptGenKey						advapi32	1211
imp	'CryptGenRandom'					CryptGenRandom						advapi32	1212
imp	'CryptGetDefaultProvider'				CryptGetDefaultProviderW				advapi32	1214
imp	'CryptGetHashParam'					CryptGetHashParam					advapi32	1215
imp	'CryptGetKeyParam'					CryptGetKeyParam					advapi32	1216
imp	'CryptGetLocalKeyLimits'				CryptGetLocalKeyLimits					advapi32	0
imp	'CryptGetProvParam'					CryptGetProvParam					advapi32	1217
imp	'CryptGetUserKey'					CryptGetUserKey						advapi32	1218
imp	'CryptHashData'						CryptHashData						advapi32	1219
imp	'CryptHashSessionKey'					CryptHashSessionKey					advapi32	1220
imp	'CryptImportKey'					CryptImportKey						advapi32	1221
imp	'CryptReleaseContext'					CryptReleaseContext					advapi32	1222
imp	'CryptSetHashParam'					CryptSetHashParam					advapi32	1223
imp	'CryptSetKeyParam'					CryptSetKeyParam					advapi32	1224
imp	'CryptSetProvParam'					CryptSetProvParam					advapi32	1225
imp	'CryptSetProvider'					CryptSetProviderW					advapi32	1229
imp	'CryptSetProviderEx'					CryptSetProviderExW					advapi32	1228
imp	'CryptSignHash'						CryptSignHashW						advapi32	1231
imp	'CryptVerifySignature'					CryptVerifySignatureW					advapi32	1233
imp	'DecryptFile'						DecryptFileW						advapi32	1236
imp	'DeleteAce'						DeleteAce						advapi32	0
imp	'DeleteService'						DeleteService						advapi32	1238
imp	'DeregisterEventSource'					DeregisterEventSource					advapi32	1239    1
imp	'DestroyPrivateObjectSecurity'				DestroyPrivateObjectSecurity				advapi32	0
imp	'DuplicateEncryptionInfoFile'				DuplicateEncryptionInfoFile				advapi32	1241
imp	'DuplicateToken'					DuplicateToken						advapi32	0	3
imp	'DuplicateTokenEx'					DuplicateTokenEx					advapi32	0	6
imp	'EnableTrace'						EnableTrace						advapi32	1265
imp	'EnableTraceEx'						EnableTraceEx						advapi32	1266
imp	'EnableTraceEx2'					EnableTraceEx2						advapi32	1267 # Windows 7+
imp	'EncryptFile'						EncryptFileW						advapi32	1269
imp	'EncryptionDisable'					EncryptionDisable					advapi32	1271
imp	'EnumDependentServices'					EnumDependentServicesW					advapi32	1273
imp	'EnumServicesStatus'					EnumServicesStatusW					advapi32	1279
imp	'EnumServicesStatusEx'					EnumServicesStatusExW					advapi32	1278
imp	'EnumerateTraceGuids'					EnumerateTraceGuids					advapi32	1280
imp	'EnumerateTraceGuidsEx'					EnumerateTraceGuidsEx					advapi32	1281
imp	'EqualDomainSid'					EqualDomainSid						advapi32	0
imp	'EqualPrefixSid'					EqualPrefixSid						advapi32	0
imp	'EqualSid'						EqualSid						advapi32	0
imp	'EventAccessControl'					EventAccessControl					advapi32	1285
imp	'EventAccessQuery'					EventAccessQuery					advapi32	1286
imp	'EventAccessRemove'					EventAccessRemove					advapi32	1287
imp	'EventActivityIdControl'				EventActivityIdControl					advapi32	0
imp	'EventEnabled'						EventEnabled						advapi32	0
imp	'EventProviderEnabled'					EventProviderEnabled					advapi32	0
imp	'EventRegister'						EventRegister						advapi32	0
imp	'EventUnregister'					EventUnregister						advapi32	0
imp	'EventWrite'						EventWrite						advapi32	0
imp	'EventWriteEx'						EventWriteEx						advapi32	0
imp	'EventWriteString'					EventWriteString					advapi32	0
imp	'EventWriteTransfer'					EventWriteTransfer					advapi32	0
imp	'FileEncryptionStatus'					FileEncryptionStatusW					advapi32	1301
imp	'FindFirstFreeAce'					FindFirstFreeAce					advapi32	0
imp	'FlushTrace'						FlushTraceW						advapi32	1305
imp	'FreeEncryptionCertificateHashList'			FreeEncryptionCertificateHashList			advapi32	1308
imp	'FreeInheritedFromArray'				FreeInheritedFromArray					advapi32	1309
imp	'FreeSid'						FreeSid							advapi32	0
imp	'GetAce'						GetAce							advapi32	0
imp	'GetAclInformation'					GetAclInformation					advapi32	0
imp	'GetAuditedPermissionsFromAcl'				GetAuditedPermissionsFromAclW				advapi32	1316
imp	'GetCurrentHwProfile'					GetCurrentHwProfileW					advapi32	1318
imp	'GetEffectiveRightsFromAcl'				GetEffectiveRightsFromAclW				advapi32	1321
imp	'GetEventLogInformation'				GetEventLogInformation					advapi32	1323
imp	'GetExplicitEntriesFromAcl'				GetExplicitEntriesFromAclW				advapi32	1325
imp	'GetFileSecurity'					GetFileSecurityW					advapi32	0	5
imp	'GetInheritanceSource'					GetInheritanceSourceW					advapi32	1331
imp	'GetKernelObjectSecurity'				GetKernelObjectSecurity					advapi32	0
imp	'GetLengthSid'						GetLengthSid						advapi32	0
imp	'GetLocalManagedApplications'				GetLocalManagedApplications				advapi32	1335
imp	'GetManagedApplicationCategories'			GetManagedApplicationCategories				advapi32	1336
imp	'GetManagedApplications'				GetManagedApplications					advapi32	1337
imp	'GetMultipleTrustee'					GetMultipleTrusteeW					advapi32	1341
imp	'GetMultipleTrusteeOperation'				GetMultipleTrusteeOperationW				advapi32	1340
imp	'GetNamedSecurityInfo'					GetNamedSecurityInfoW					advapi32	1345
imp	'GetNumberOfEventLogRecords'				GetNumberOfEventLogRecords				advapi32	1346
imp	'GetOldestEventLogRecord'				GetOldestEventLogRecord					advapi32	1347
imp	'GetPrivateObjectSecurity'				GetPrivateObjectSecurity				advapi32	0
imp	'GetSecurityDescriptorControl'				GetSecurityDescriptorControl				advapi32	0
imp	'GetSecurityDescriptorDacl'				GetSecurityDescriptorDacl				advapi32	0
imp	'GetSecurityDescriptorGroup'				GetSecurityDescriptorGroup				advapi32	0
imp	'GetSecurityDescriptorLength'				GetSecurityDescriptorLength				advapi32	0
imp	'GetSecurityDescriptorOwner'				GetSecurityDescriptorOwner				advapi32	0
imp	'GetSecurityDescriptorRMControl'			GetSecurityDescriptorRMControl				advapi32	0
imp	'GetSecurityDescriptorSacl'				GetSecurityDescriptorSacl				advapi32	0
imp	'GetSecurityInfo'					GetSecurityInfo						advapi32	1357
imp	'GetServiceDisplayName'					GetServiceDisplayNameW					advapi32	1361
imp	'GetServiceKeyName'					GetServiceKeyNameW					advapi32	1363
imp	'GetSidIdentifierAuthority'				GetSidIdentifierAuthority				advapi32	0
imp	'GetSidLengthRequired'					GetSidLengthRequired					advapi32	0
imp	'GetSidSubAuthority'					GetSidSubAuthority					advapi32	0
imp	'GetSidSubAuthorityCount'				GetSidSubAuthorityCount					advapi32	0
imp	'GetThreadWaitChain'					GetThreadWaitChain					advapi32	1369
imp	'GetTokenInformation'					GetTokenInformation					advapi32	0
imp	'GetTraceEnableFlags'					GetTraceEnableFlags					advapi32	0
imp	'GetTraceEnableLevel'					GetTraceEnableLevel					advapi32	0
imp	'GetTraceLoggerHandle'					GetTraceLoggerHandle					advapi32	0
imp	'GetTrusteeForm'					GetTrusteeFormW						advapi32	1375
imp	'GetTrusteeName'					GetTrusteeNameW						advapi32	1377
imp	'GetTrusteeType'					GetTrusteeTypeW						advapi32	1379
imp	'GetUserName'						GetUserNameW						advapi32	1381	2
imp	'GetWindowsAccountDomainSid'				GetWindowsAccountDomainSid				advapi32	0
imp	'ImpersonateAnonymousToken'				ImpersonateAnonymousToken				advapi32	0
imp	'ImpersonateLoggedOnUser'				ImpersonateLoggedOnUser					advapi32	0
imp	'ImpersonateNamedPipeClient'				ImpersonateNamedPipeClient				advapi32	0
imp	'ImpersonateSelf'					ImpersonateSelf						advapi32	0	1
imp	'InitializeAcl'						InitializeAcl						advapi32	0
imp	'InitializeSecurityDescriptor'				InitializeSecurityDescriptor				advapi32	0
imp	'InitializeSid'						InitializeSid						advapi32	0
imp	'InitiateShutdown'					InitiateShutdownW					advapi32	1403	5
imp	'InitiateSystemShutdown'				InitiateSystemShutdownW					advapi32	1407
imp	'InitiateSystemShutdownEx'				InitiateSystemShutdownExW				advapi32	1406
imp	'InstallApplication'					InstallApplication					advapi32	1408
imp	'IsTextUnicode'						IsTextUnicode						advapi32	1409
imp	'IsTokenRestricted'					IsTokenRestricted					advapi32	0
imp	'IsValidAcl'						IsValidAcl						advapi32	0
imp	'IsValidSecurityDescriptor'				IsValidSecurityDescriptor				advapi32	0
imp	'IsValidSid'						IsValidSid						advapi32	0
imp	'IsWellKnownSid'					IsWellKnownSid						advapi32	0
imp	'LockServiceDatabase'					LockServiceDatabase					advapi32	1417
imp	'LogonUser'						LogonUserW						advapi32	1422
imp	'LogonUserEx'						LogonUserExW						advapi32	1421
imp	'LogonUserExEx'						LogonUserExExW						advapi32	1420
imp	'LookupAccountName'					LookupAccountNameW					advapi32	1424
imp	'LookupAccountSid'					LookupAccountSidW					advapi32	1426
imp	'LookupPrivilegeDisplayName'				LookupPrivilegeDisplayNameW				advapi32	1428
imp	'LookupPrivilegeName'					LookupPrivilegeNameW					advapi32	1430
imp	'LookupPrivilegeValue'					LookupPrivilegeValueW					advapi32	1432	3
imp	'LookupSecurityDescriptorParts'				LookupSecurityDescriptorPartsW				advapi32	1434
imp	'LsaAddAccountRights'					LsaAddAccountRights					advapi32	1435
imp	'LsaClose'						LsaClose						advapi32	1438
imp	'LsaCreateTrustedDomain'				LsaCreateTrustedDomain					advapi32	1441
imp	'LsaCreateTrustedDomainEx'				LsaCreateTrustedDomainEx				advapi32	1442
imp	'LsaDeleteTrustedDomain'				LsaDeleteTrustedDomain					advapi32	1444
imp	'LsaEnumerateAccountRights'				LsaEnumerateAccountRights				advapi32	1445
imp	'LsaEnumerateAccountsWithUserRight'			LsaEnumerateAccountsWithUserRight			advapi32	1447
imp	'LsaEnumerateTrustedDomains'				LsaEnumerateTrustedDomains				advapi32	1450
imp	'LsaEnumerateTrustedDomainsEx'				LsaEnumerateTrustedDomainsEx				advapi32	1451
imp	'LsaFreeMemory'						LsaFreeMemory						advapi32	1452
imp	'LsaLookupNames'					LsaLookupNames						advapi32	1462
imp	'LsaLookupNames2'					LsaLookupNames2						advapi32	1463
imp	'LsaLookupSids'						LsaLookupSids						advapi32	1467
imp	'LsaNtStatusToWinError'					LsaNtStatusToWinError					advapi32	1470
imp	'LsaOpenPolicy'						LsaOpenPolicy						advapi32	1472
imp	'LsaOpenTrustedDomainByName'				LsaOpenTrustedDomainByName				advapi32	1476
imp	'LsaQueryDomainInformationPolicy'			LsaQueryDomainInformationPolicy				advapi32	1478
imp	'LsaQueryForestTrustInformation'			LsaQueryForestTrustInformation				advapi32	1479
imp	'LsaQueryInformationPolicy'				LsaQueryInformationPolicy				advapi32	1481
imp	'LsaQueryTrustedDomainInfo'				LsaQueryTrustedDomainInfo				advapi32	1484
imp	'LsaQueryTrustedDomainInfoByName'			LsaQueryTrustedDomainInfoByName				advapi32	1485
imp	'LsaRemoveAccountRights'				LsaRemoveAccountRights					advapi32	1486
imp	'LsaRetrievePrivateData'				LsaRetrievePrivateData					advapi32	1488
imp	'LsaSetDomainInformationPolicy'				LsaSetDomainInformationPolicy				advapi32	1490
imp	'LsaSetForestTrustInformation'				LsaSetForestTrustInformation				advapi32	1491
imp	'LsaSetInformationPolicy'				LsaSetInformationPolicy					advapi32	1492
imp	'LsaSetTrustedDomainInfoByName'				LsaSetTrustedDomainInfoByName				advapi32	1498
imp	'LsaSetTrustedDomainInformation'			LsaSetTrustedDomainInformation				advapi32	1499
imp	'LsaStorePrivateData'					LsaStorePrivateData					advapi32	1500
imp	'MSChapSrvChangePassword'				MSChapSrvChangePassword					advapi32	1508
imp	'MSChapSrvChangePassword2'				MSChapSrvChangePassword2				advapi32	1509
imp	'MakeAbsoluteSD'					MakeAbsoluteSD						advapi32	0
imp	'MakeSelfRelativeSD'					MakeSelfRelativeSD					advapi32	0
imp	'MapGenericMask'					MapGenericMask						advapi32	0	2
imp	'NotifyBootConfigStatus'				NotifyBootConfigStatus					advapi32	1514
imp	'NotifyChangeEventLog'					NotifyChangeEventLog					advapi32	1515
imp	'NotifyServiceStatusChange'				NotifyServiceStatusChange				advapi32	0
imp	'NotifyServiceStatusChange'				NotifyServiceStatusChangeW				advapi32	1518
imp	'ObjectCloseAuditAlarm'					ObjectCloseAuditAlarmW					advapi32	0
imp	'ObjectDeleteAuditAlarm'				ObjectDeleteAuditAlarmW					advapi32	0
imp	'ObjectOpenAuditAlarm'					ObjectOpenAuditAlarmW					advapi32	0
imp	'ObjectPrivilegeAuditAlarm'				ObjectPrivilegeAuditAlarmW				advapi32	0
imp	'OpenBackupEventLog'					OpenBackupEventLogW					advapi32	1529
imp	'OpenEncryptedFileRaw'					OpenEncryptedFileRawW					advapi32	1531
imp	'OpenEventLog'						OpenEventLogW						advapi32	1533
imp	'OpenProcessToken'					OpenProcessToken					advapi32	0	3
imp	'OpenSCManager'						OpenSCManagerW						advapi32	1536
imp	'OpenService'						OpenServiceW						advapi32	1538
imp	'OpenThreadToken'					OpenThreadToken						advapi32	0	4
imp	'OpenThreadWaitChainSession'				OpenThreadWaitChainSession				advapi32	1540
imp	'OpenTrace'						OpenTraceW						advapi32	1542
imp	'PerfCreateInstance'					PerfCreateInstance					advapi32	0
imp	'PerfDecrementULongCounterValue'			PerfDecrementULongCounterValue				advapi32	0
imp	'PerfDecrementULongLongCounterValue'			PerfDecrementULongLongCounterValue			advapi32	0
imp	'PerfDeleteInstance'					PerfDeleteInstance					advapi32	0
imp	'PerfIncrementULongCounterValue'			PerfIncrementULongCounterValue				advapi32	0
imp	'PerfIncrementULongLongCounterValue'			PerfIncrementULongLongCounterValue			advapi32	0
imp	'PerfQueryInstance'					PerfQueryInstance					advapi32	0
imp	'PerfSetCounterRefValue'				PerfSetCounterRefValue					advapi32	0
imp	'PerfSetCounterSetInfo'					PerfSetCounterSetInfo					advapi32	0
imp	'PerfSetULongCounterValue'				PerfSetULongCounterValue				advapi32	0
imp	'PerfSetULongLongCounterValue'				PerfSetULongLongCounterValue				advapi32	0
imp	'PerfStartProvider'					PerfStartProvider					advapi32	0
imp	'PerfStartProviderEx'					PerfStartProviderEx					advapi32	0
imp	'PerfStopProvider'					PerfStopProvider					advapi32	0
imp	'PrivilegeCheck'					PrivilegeCheck						advapi32	0
imp	'PrivilegedServiceAuditAlarm'				PrivilegedServiceAuditAlarmW				advapi32	0
imp	'ProcessTrace'						ProcessTrace						advapi32	1579
imp	'QueryAllTraces'					QueryAllTracesW						advapi32	1581
imp	'QueryRecoveryAgentsOnEncryptedFile'			QueryRecoveryAgentsOnEncryptedFile			advapi32	1583
imp	'QuerySecurityAccessMask'				QuerySecurityAccessMask					advapi32	0
imp	'QueryServiceConfig'					QueryServiceConfigW					advapi32	1588
imp	'QueryServiceConfig2W'					QueryServiceConfig2W					advapi32	1586
imp	'QueryServiceLockStatus'				QueryServiceLockStatusW					advapi32	1591
imp	'QueryServiceObjectSecurity'				QueryServiceObjectSecurity				advapi32	1592
imp	'QueryServiceStatus'					QueryServiceStatus					advapi32	1593
imp	'QueryServiceStatusEx'					QueryServiceStatusEx					advapi32	1594
imp	'QueryTrace'						QueryTraceW						advapi32	1597
imp	'QueryUsersOnEncryptedFile'				QueryUsersOnEncryptedFile				advapi32	1600
imp	'ReadEncryptedFileRaw'					ReadEncryptedFileRaw					advapi32	1601
imp	'ReadEventLog'						ReadEventLogW						advapi32	1603
imp	'RegCloseKey'						RegCloseKey						advapi32	0	1
imp	'RegConnectRegistry'					RegConnectRegistryW					advapi32	1608	3
imp	'RegCopyTree'						RegCopyTreeW						advapi32	0
imp	'RegCreateKey'						RegCreateKeyW						advapi32	1616	3
imp	'RegCreateKeyEx'					RegCreateKeyExW						advapi32	0	9
imp	'RegCreateKeyTransacted'				RegCreateKeyTransactedW					advapi32	1615
imp	'RegDeleteKey'						RegDeleteKeyW						advapi32	1624	2
imp	'RegDeleteKeyEx'					RegDeleteKeyExW						advapi32	0	4
imp	'RegDeleteKeyTransacted'				RegDeleteKeyTransactedW					advapi32	1621
imp	'RegDeleteKeyValue'					RegDeleteKeyValueW					advapi32	0
imp	'RegDeleteTree'						RegDeleteTreeW						advapi32	0	2
imp	'RegDeleteValue'					RegDeleteValueW						advapi32	0	2
imp	'RegDisablePredefinedCache'				RegDisablePredefinedCache				advapi32	1629	1
imp	'RegDisablePredefinedCacheEx'				RegDisablePredefinedCacheEx				advapi32	0
imp	'RegDisableReflectionKey'				RegDisableReflectionKey					advapi32	1631	1
imp	'RegEnableReflectionKey'				RegEnableReflectionKey					advapi32	1632	1
imp	'RegEnumKey'						RegEnumKeyW						advapi32	1636	4
imp	'RegEnumKeyEx'						RegEnumKeyExW						advapi32	0	8
imp	'RegEnumValue'						RegEnumValueW						advapi32	0	8
imp	'RegFlushKey'						RegFlushKey						advapi32	0	1
imp	'RegGetKeySecurity'					RegGetKeySecurity					advapi32	0	4
imp	'RegGetValue'						RegGetValueW						advapi32	0	7
imp	'RegLoadAppKey'						RegLoadAppKeyW						advapi32	0
imp	'RegLoadKey'						RegLoadKeyW						advapi32	0	3
imp	'RegLoadMUIString'					RegLoadMUIStringW					advapi32	0
imp	'RegNotifyChangeKeyValue'				RegNotifyChangeKeyValue					advapi32	0	5
imp	'RegOpenCurrentUser'					RegOpenCurrentUser					advapi32	0	2
imp	'RegOpenKey'						RegOpenKeyW						advapi32	1656
imp	'RegOpenKeyEx'						RegOpenKeyExW						advapi32	0	5
imp	'RegOpenKeyTransacted'					RegOpenKeyTransactedW					advapi32	1655
imp	'RegOpenUserClassesRoot'				RegOpenUserClassesRoot					advapi32	0	4
imp	'RegOverridePredefKey'					RegOverridePredefKey					advapi32	1658	2
imp	'RegQueryInfoKey'					RegQueryInfoKeyW					advapi32	0	12
imp	'RegQueryMultipleValues'				RegQueryMultipleValuesW					advapi32	0	5
imp	'RegQueryReflectionKey'					RegQueryReflectionKey					advapi32	1663	2
imp	'RegQueryValue'						RegQueryValueW						advapi32	1667	4
imp	'RegQueryValueEx'					RegQueryValueExW					advapi32	0	6
imp	'RegReplaceKey'						RegReplaceKeyW						advapi32	1670	4
imp	'RegRestoreKey'						RegRestoreKeyW						advapi32	0	3
imp	'RegSaveKey'						RegSaveKeyW						advapi32	1676	3
imp	'RegSaveKeyEx'						RegSaveKeyExW						advapi32	0
imp	'RegSetKeySecurity'					RegSetKeySecurity					advapi32	0	3
imp	'RegSetKeyValue'					RegSetKeyValueW						advapi32	0
imp	'RegSetValue'						RegSetValueW						advapi32	1683	5
imp	'RegSetValueEx'						RegSetValueExW						advapi32	0	6
imp	'RegUnLoadKey'						RegUnLoadKeyW						advapi32	0	2
imp	'RegisterEventSource'					RegisterEventSourceW					advapi32	1687    2
imp	'RegisterServiceCtrlHandler'				RegisterServiceCtrlHandlerW				advapi32	1692
imp	'RegisterServiceCtrlHandlerEx'				RegisterServiceCtrlHandlerExW				advapi32	1691
imp	'RegisterTraceGuids'					RegisterTraceGuidsW					advapi32	0
imp	'RegisterWaitChainCOMCallback'				RegisterWaitChainCOMCallback				advapi32	1695
imp	'RemoveTraceCallback'					RemoveTraceCallback					advapi32	0
imp	'RemoveUsersFromEncryptedFile'				RemoveUsersFromEncryptedFile				advapi32	1703
imp	'ReportEvent'						ReportEventW						advapi32	0	9
imp	'ReportEventA'						ReportEventA						advapi32	0	9
imp	'RevertToSelf'						RevertToSelf						advapi32	0	0
imp	'RtlGenRandom'						SystemFunction036					advapi32	0	2
imp	'SaferCloseLevel'					SaferCloseLevel						advapi32	1708
imp	'SaferComputeTokenFromLevel'				SaferComputeTokenFromLevel				advapi32	1709
imp	'SaferCreateLevel'					SaferCreateLevel					advapi32	1710
imp	'SaferGetLevelInformation'				SaferGetLevelInformation				advapi32	1711
imp	'SaferGetPolicyInformation'				SaferGetPolicyInformation				advapi32	1712
imp	'SaferIdentifyLevel'					SaferIdentifyLevel					advapi32	1713
imp	'SaferRecordEventLogEntry'				SaferRecordEventLogEntry				advapi32	1714
imp	'SaferiIsExecutableFileType'				SaferiIsExecutableFileType				advapi32	1720
imp	'SetAclInformation'					SetAclInformation					advapi32	0
imp	'SetEntriesInAcl'					SetEntriesInAclW					advapi32	1729
imp	'SetFileSecurity'					SetFileSecurityW					advapi32	0
imp	'SetKernelObjectSecurity'				SetKernelObjectSecurity					advapi32	0
imp	'SetNamedSecurityInfo'					SetNamedSecurityInfoW					advapi32	1740
imp	'SetPrivateObjectSecurity'				SetPrivateObjectSecurity				advapi32	0
imp	'SetPrivateObjectSecurityEx'				SetPrivateObjectSecurityEx				advapi32	0
imp	'SetSecurityAccessMask'					SetSecurityAccessMask					advapi32	0
imp	'SetSecurityDescriptorControl'				SetSecurityDescriptorControl				advapi32	0
imp	'SetSecurityDescriptorDacl'				SetSecurityDescriptorDacl				advapi32	0
imp	'SetSecurityDescriptorGroup'				SetSecurityDescriptorGroup				advapi32	0
imp	'SetSecurityDescriptorOwner'				SetSecurityDescriptorOwner				advapi32	0
imp	'SetSecurityDescriptorRMControl'			SetSecurityDescriptorRMControl				advapi32	0
imp	'SetSecurityDescriptorSacl'				SetSecurityDescriptorSacl				advapi32	0
imp	'SetSecurityInfo'					SetSecurityInfo						advapi32	1750
imp	'SetServiceBits'					SetServiceBits						advapi32	1753
imp	'SetServiceObjectSecurity'				SetServiceObjectSecurity				advapi32	1754
imp	'SetServiceStatus'					SetServiceStatus					advapi32	1755
imp	'SetThreadToken'					SetThreadToken						advapi32	0
imp	'SetTokenInformation'					SetTokenInformation					advapi32	0
imp	'SetTraceCallback'					SetTraceCallback					advapi32	0
imp	'SetUserFileEncryptionKey'				SetUserFileEncryptionKey				advapi32	1759
imp	'StartService'						StartServiceW						advapi32	1764
imp	'StartServiceCtrlDispatcher'				StartServiceCtrlDispatcherW				advapi32	1763
imp	'StartTrace'						StartTraceW						advapi32	1766
imp	'StopTrace'						StopTraceW						advapi32	1768
imp	'SystemFunction040'					SystemFunction040					advapi32	0
imp	'SystemFunction041'					SystemFunction041					advapi32	0
imp	'TraceEvent'						TraceEvent						advapi32	0
imp	'TraceEventInstance'					TraceEventInstance					advapi32	0
imp	'TraceMessage'						TraceMessage						advapi32	0
imp	'TraceMessageVa'					TraceMessageVa						advapi32	0
imp	'TraceSetInformation'					TraceSetInformation					advapi32	1812 # Windows 7+
imp	'TreeResetNamedSecurityInfo'				TreeResetNamedSecurityInfoW				advapi32	1814
imp	'TreeSetNamedSecurityInfo'				TreeSetNamedSecurityInfoW				advapi32	1816
imp	'UninstallApplication'					UninstallApplication					advapi32	1819
imp	'UnlockServiceDatabase'					UnlockServiceDatabase					advapi32	1820
imp	'UnregisterTraceGuids'					UnregisterTraceGuids					advapi32	0
imp	'UpdateTrace'						UpdateTraceW						advapi32	1824

# USER32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'ActivateKeyboardLayout'				ActivateKeyboardLayout					user32		1505
imp	'AddClipboardFormatListener'				AddClipboardFormatListener				user32		1506
imp	'AdjustWindowRect'					AdjustWindowRect					user32		1507	3
imp	'AdjustWindowRectEx'					AdjustWindowRectEx					user32		1508
imp	'AdjustWindowRectExForDpi'				AdjustWindowRectExForDpi				user32		1509
imp	'AlignRects'						AlignRects						user32		1510
imp	'AllowForegroundActivation'				AllowForegroundActivation				user32		1511
imp	'AllowSetForegroundWindow'				AllowSetForegroundWindow				user32		1512
imp	'AnimateWindow'						AnimateWindow						user32		1513	3
imp	'AnyPopup'						AnyPopup						user32		1514
imp	'AppendMenuA'						AppendMenuA						user32		1515	4
imp	'AppendMenu'						AppendMenuW						user32		1516	4
imp	'AreDpiAwarenessContextsEqual'				AreDpiAwarenessContextsEqual				user32		1517
imp	'ArrangeIconicWindows'					ArrangeIconicWindows					user32		1518
imp	'AttachThreadInput'					AttachThreadInput					user32		1519
imp	'BeginDeferWindowPos'					BeginDeferWindowPos					user32		1520
imp	'BeginPaint'						BeginPaint						user32		1521	2
imp	'BlockInput'						BlockInput						user32		1522
imp	'BringWindowToTop'					BringWindowToTop					user32		1523	1
imp	'BroadcastSystemMessage'				BroadcastSystemMessageW					user32		1528
imp	'BroadcastSystemMessageEx'				BroadcastSystemMessageExW				user32		1527
imp	'BuildReasonArray'					BuildReasonArray					user32		1529
imp	'CalcMenuBar'						CalcMenuBar						user32		1530
imp	'CalculatePopupWindowPosition'				CalculatePopupWindowPosition				user32		1531
imp	'CallMsgFilter'						CallMsgFilterW						user32		1534
imp	'CallNextHookEx'					CallNextHookEx						user32		1535	4
imp	'CallWindowProc'					CallWindowProcW						user32		1537
imp	'CancelShutdown'					CancelShutdown						user32		1538
imp	'CascadeChildWindows'					CascadeChildWindows					user32		1539
imp	'CascadeWindows'					CascadeWindows						user32		1540
imp	'ChangeClipboardChain'					ChangeClipboardChain					user32		1541
imp	'ChangeDisplaySettings'					ChangeDisplaySettingsW					user32		1545
imp	'ChangeDisplaySettingsEx'				ChangeDisplaySettingsExW				user32		1544
imp	'ChangeMenu'						ChangeMenuW						user32		1547
imp	'ChangeWindowMessageFilter'				ChangeWindowMessageFilter				user32		1548
imp	'ChangeWindowMessageFilterEx'				ChangeWindowMessageFilterEx				user32		1549
imp	'CharToOem'						CharToOemW						user32		1568
imp	'CharToOemBuff'						CharToOemBuffW						user32		1567
imp	'CheckDBCSEnabledExt'					CheckDBCSEnabledExt					user32		1573
imp	'CheckDlgButton'					CheckDlgButton						user32		1574
imp	'CheckMenuItem'						CheckMenuItem						user32		1575
imp	'CheckMenuRadioItem'					CheckMenuRadioItem					user32		1576
imp	'CheckProcessForClipboardAccess'			CheckProcessForClipboardAccess				user32		1577
imp	'CheckProcessSession'					CheckProcessSession					user32		1578
imp	'CheckRadioButton'					CheckRadioButton					user32		1579
imp	'CheckWindowThreadDesktop'				CheckWindowThreadDesktop				user32		1580
imp	'ChildWindowFromPoint'					ChildWindowFromPoint					user32		1581
imp	'ChildWindowFromPointEx'				ChildWindowFromPointEx					user32		1582
imp	'CliImmSetHotKey'					CliImmSetHotKey						user32		1583
imp	'ClientThreadSetup'					ClientThreadSetup					user32		1584
imp	'ClientToScreen'					ClientToScreen						user32		1585
imp	'ClipCursor'						ClipCursor						user32		1586
imp	'CloseClipboard'					CloseClipboard						user32		1587
imp	'CloseDesktop'						CloseDesktop						user32		1588
imp	'CloseGestureInfoHandle'				CloseGestureInfoHandle					user32		1589
imp	'CloseTouchInputHandle'					CloseTouchInputHandle					user32		1590
imp	'CloseWindow'						CloseWindow						user32		1591	1
imp	'CloseWindowStation'					CloseWindowStation					user32		1592
imp	'ConsoleControl'					ConsoleControl						user32		1593
imp	'ControlMagnification'					ControlMagnification					user32		1594
imp	'CopyAcceleratorTable'					CopyAcceleratorTableW					user32		1596
imp	'CopyIcon'						CopyIcon						user32		1597
imp	'CopyImage'						CopyImage						user32		1598
imp	'CopyRect'						CopyRect						user32		1599
imp	'CountClipboardFormats'					CountClipboardFormats					user32		1600
imp	'CreateAcceleratorTable'				CreateAcceleratorTableW					user32		1602
imp	'CreateCaret'						CreateCaret						user32		1603
imp	'CreateCursor'						CreateCursor						user32		1604
imp	'CreateDCompositionHwndTarget'				CreateDCompositionHwndTarget				user32		1605
imp	'CreateDesktop'						CreateDesktopW						user32		1609
imp	'CreateDesktopEx'					CreateDesktopExW					user32		1608
imp	'CreateDialogIndirectParam'				CreateDialogIndirectParamW				user32		1612
imp	'CreateDialogIndirectParamAor'				CreateDialogIndirectParamAorW				user32		1611
imp	'CreateDialogParam'					CreateDialogParamW					user32		1614
imp	'CreateIcon'						CreateIcon						user32		1615
imp	'CreateIconFromResource'				CreateIconFromResource					user32		1616
imp	'CreateIconFromResourceEx'				CreateIconFromResourceEx				user32		1617
imp	'CreateIconIndirect'					CreateIconIndirect					user32		1618	1
imp	'CreateMDIWindow'					CreateMDIWindowW					user32		1620
imp	'CreateMenu'						CreateMenu						user32		1621	0
imp	'CreatePalmRejectionDelayZone'				CreatePalmRejectionDelayZone				user32		1503
imp	'CreatePopupMenu'					CreatePopupMenu						user32		1622	0
imp	'CreateSystemThreads'					CreateSystemThreads					user32		1623
imp	'CreateWindowEx'					CreateWindowExW						user32		1625	12
imp	'CreateWindowInBand'					CreateWindowInBand					user32		1626
imp	'CreateWindowInBandEx'					CreateWindowInBandEx					user32		1627
imp	'CreateWindowIndirect'					CreateWindowIndirect					user32		1628
imp	'CreateWindowStation'					CreateWindowStationW					user32		1630
imp	'CsrBroadcastSystemMessageEx'				CsrBroadcastSystemMessageExW				user32		1631
imp	'CtxInitUser32'						CtxInitUser32						user32		1632
imp	'DWMBindCursorToOutputConfig'				DWMBindCursorToOutputConfig				user32		1633
imp	'DWMCommitInputSystemOutputConfig'			DWMCommitInputSystemOutputConfig			user32		1634
imp	'DWMSetCursorOrientation'				DWMSetCursorOrientation					user32		1635
imp	'DWMSetInputSystemOutputConfig'				DWMSetInputSystemOutputConfig				user32		1636
imp	'DdeAbandonTransaction'					DdeAbandonTransaction					user32		1637
imp	'DdeAccessData'						DdeAccessData						user32		1638
imp	'DdeAddData'						DdeAddData						user32		1639
imp	'DdeClientTransaction'					DdeClientTransaction					user32		1640
imp	'DdeCmpStringHandles'					DdeCmpStringHandles					user32		1641
imp	'DdeConnect'						DdeConnect						user32		1642
imp	'DdeConnectList'					DdeConnectList						user32		1643
imp	'DdeCreateDataHandle'					DdeCreateDataHandle					user32		1644
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
imp	'DdeInitialize'						DdeInitializeW						user32		1657
imp	'DdeKeepStringHandle'					DdeKeepStringHandle					user32		1658
imp	'DdeNameService'					DdeNameService						user32		1659
imp	'DdePostAdvise'						DdePostAdvise						user32		1660
imp	'DdeQueryConvInfo'					DdeQueryConvInfo					user32		1661
imp	'DdeQueryNextServer'					DdeQueryNextServer					user32		1662
imp	'DdeQueryString'					DdeQueryStringW						user32		1664
imp	'DdeReconnect'						DdeReconnect						user32		1665
imp	'DdeSetQualityOfService'				DdeSetQualityOfService					user32		1666
imp	'DdeSetUserHandle'					DdeSetUserHandle					user32		1667
imp	'DdeUnaccessData'					DdeUnaccessData						user32		1668
imp	'DdeUninitialize'					DdeUninitialize						user32		1669
imp	'DefFrameProc'						DefFrameProcW						user32		1673
imp	'DefMDIChildProc'					DefMDIChildProcW					user32		1675
imp	'DefRawInputProc'					DefRawInputProc						user32		1676
imp	'DefWindowProc'						DefWindowProcW						user32		174	4
imp	'DeferWindowPos'					DeferWindowPos						user32		1679
imp	'DeferWindowPosAndBand'					DeferWindowPosAndBand					user32		1680
imp	'DelegateInput'						DelegateInput						user32		2503
imp	'DeleteMenu'						DeleteMenu						user32		1681	3
imp	'DeregisterShellHookWindow'				DeregisterShellHookWindow				user32		1682
imp	'DestroyAcceleratorTable'				DestroyAcceleratorTable					user32		1683
imp	'DestroyCaret'						DestroyCaret						user32		1684
imp	'DestroyCursor'						DestroyCursor						user32		1685
imp	'DestroyDCompositionHwndTarget'				DestroyDCompositionHwndTarget				user32		1686
imp	'DestroyIcon'						DestroyIcon						user32		1687	1
imp	'DestroyMenu'						DestroyMenu						user32		1688	1
imp	'DestroyPalmRejectionDelayZone'				DestroyPalmRejectionDelayZone				user32		1504
imp	'DestroyReasons'					DestroyReasons						user32		1689
imp	'DestroyWindow'						DestroyWindow						user32		1690	1
imp	'DialogBoxIndirectParam'				DialogBoxIndirectParamW					user32		1693
imp	'DialogBoxIndirectParamAor'				DialogBoxIndirectParamAorW				user32		1692
imp	'DialogBoxParam'					DialogBoxParamW						user32		1695
imp	'DisableProcessWindowsGhosting'				DisableProcessWindowsGhosting				user32		1696
imp	'DispatchMessage'					DispatchMessageW					user32		1698	1
imp	'DisplayConfigGetDeviceInfo'				DisplayConfigGetDeviceInfo				user32		1699
imp	'DisplayConfigSetDeviceInfo'				DisplayConfigSetDeviceInfo				user32		1700
imp	'DisplayExitWindowsWarnings'				DisplayExitWindowsWarnings				user32		1701
imp	'DlgDirList'						DlgDirListW						user32		1705
imp	'DlgDirListComboBox'					DlgDirListComboBoxW					user32		1704
imp	'DlgDirSelectComboBoxEx'				DlgDirSelectComboBoxExW					user32		1707
imp	'DlgDirSelectEx'					DlgDirSelectExW						user32		1709
imp	'DoSoundConnect'					DoSoundConnect						user32		1710
imp	'DoSoundDisconnect'					DoSoundDisconnect					user32		1711
imp	'DragDetect'						DragDetect						user32		1712
imp	'DragObject'						DragObject						user32		1713
imp	'DrawAnimatedRects'					DrawAnimatedRects					user32		1714
imp	'DrawCaption'						DrawCaption						user32		1715
imp	'DrawCaptionTemp'					DrawCaptionTempW					user32		1717
imp	'DrawEdge'						DrawEdge						user32		1718
imp	'DrawFocusRect'						DrawFocusRect						user32		1719
imp	'DrawFrame'						DrawFrame						user32		1720
imp	'DrawFrameControl'					DrawFrameControl					user32		1721
imp	'DrawIcon'						DrawIcon						user32		1722
imp	'DrawIconEx'						DrawIconEx						user32		1723
imp	'DrawMenuBar'						DrawMenuBar						user32		1724
imp	'DrawMenuBarTemp'					DrawMenuBarTemp						user32		1725
imp	'DrawState'						DrawStateW						user32		1727
imp	'DrawText'						DrawTextW						user32		1731	5
imp	'DrawTextEx'						DrawTextExW						user32		1730	6
imp	'DwmGetDxRgn'						DwmGetDxRgn						user32		1553
imp	'DwmGetDxSharedSurface'					DwmGetDxSharedSurface					user32		1732
imp	'DwmGetRemoteSessionOcclusionEvent'			DwmGetRemoteSessionOcclusionEvent			user32		1733
imp	'DwmGetRemoteSessionOcclusionState'			DwmGetRemoteSessionOcclusionState			user32		1734
imp	'DwmKernelShutdown'					DwmKernelShutdown					user32		1735
imp	'DwmKernelStartup'					DwmKernelStartup					user32		1736
imp	'DwmLockScreenUpdates'					DwmLockScreenUpdates					user32		1737
imp	'DwmValidateWindow'					DwmValidateWindow					user32		1738
imp	'EditWndProc'						EditWndProc						user32		1739
imp	'EmptyClipboard'					EmptyClipboard						user32		1704
imp	'EnableMenuItem'					EnableMenuItem						user32		1741
imp	'EnableMouseInPointer'					EnableMouseInPointer					user32		1742
imp	'EnableNonClientDpiScaling'				EnableNonClientDpiScaling				user32		1743
imp	'EnableOneCoreTransformMode'				EnableOneCoreTransformMode				user32		1744
imp	'EnableScrollBar'					EnableScrollBar						user32		1745
imp	'EnableSessionForMMCSS'					EnableSessionForMMCSS					user32		1746
imp	'EnableWindow'						EnableWindow						user32		1747
imp	'EndDeferWindowPos'					EndDeferWindowPos					user32		1748
imp	'EndDeferWindowPosEx'					EndDeferWindowPosEx					user32		1749
imp	'EndDialog'						EndDialog						user32		1750
imp	'EndMenu'						EndMenu							user32		1751
imp	'EndPaint'						EndPaint						user32		1752	2
imp	'EndTask'						EndTask							user32		1753
imp	'EnterReaderModeHelper'					EnterReaderModeHelper					user32		1754
imp	'EnumChildWindows'					EnumChildWindows					user32		1755	3
imp	'EnumClipboardFormats'					EnumClipboardFormats					user32		1756
imp	'EnumDesktopWindows'					EnumDesktopWindows					user32		1757
imp	'EnumDesktops'						EnumDesktopsW						user32		1759
imp	'EnumDisplayDevices'					EnumDisplayDevicesW					user32		1761
imp	'EnumDisplayMonitors'					EnumDisplayMonitors					user32		1762
imp	'EnumDisplaySettings'					EnumDisplaySettingsW					user32		1766
imp	'EnumDisplaySettingsEx'					EnumDisplaySettingsExW					user32		1765
imp	'EnumProps'						EnumPropsW						user32		1770
imp	'EnumPropsEx'						EnumPropsExW						user32		1769
imp	'EnumThreadWindows'					EnumThreadWindows					user32		1771
imp	'EnumWindowStations'					EnumWindowStationsW					user32		1773
imp	'EnumWindows'						EnumWindows						user32		1774
imp	'EqualRect'						EqualRect						user32		1775
imp	'EvaluateProximityToPolygon'				EvaluateProximityToPolygon				user32		1776
imp	'EvaluateProximityToRect'				EvaluateProximityToRect					user32		1777
imp	'ExcludeUpdateRgn'					ExcludeUpdateRgn					user32		1778
imp	'ExitWindowsEx'						ExitWindowsEx						user32		1779
imp	'FillRect'						FillRect						user32		1780	3
imp	'FindWindow'						FindWindowW						user32		1784	2
imp	'FindWindowEx'						FindWindowExW						user32		1783	4
imp	'FlashWindow'						FlashWindow						user32		1785
imp	'FlashWindowEx'						FlashWindowEx						user32		1786
imp	'FrameRect'						FrameRect						user32		1787
imp	'FreeDDElParam'						FreeDDElParam						user32		1788
imp	'FrostCrashedWindow'					FrostCrashedWindow					user32		1789
imp	'GetActiveWindow'					GetActiveWindow						user32		1790
imp	'GetAltTabInfo'						GetAltTabInfoW						user32		1793
imp	'GetAncestor'						GetAncestor						user32		1794
imp	'GetAppCompatFlags'					GetAppCompatFlags					user32		1795
imp	'GetAppCompatFlags2'					GetAppCompatFlags2					user32		1796
imp	'GetAsyncKeyState'					GetAsyncKeyState					user32		1797
imp	'GetAutoRotationState'					GetAutoRotationState					user32		1798
imp	'GetAwarenessFromDpiAwarenessContext'			GetAwarenessFromDpiAwarenessContext			user32		1799
imp	'GetCIMSSM'						GetCIMSSM						user32		1800
imp	'GetCapture'						GetCapture						user32		1801
imp	'GetCaretBlinkTime'					GetCaretBlinkTime					user32		1802
imp	'GetCaretPos'						GetCaretPos						user32		1803
imp	'GetClassInfo'						GetClassInfoW						user32		1807
imp	'GetClassInfoEx'					GetClassInfoExW						user32		1806
imp	'GetClassLong'						GetClassLongW						user32		1811
imp	'GetClassLongPtr'					GetClassLongPtrW					user32		1810
imp	'GetClassName'						GetClassNameW						user32		1813
imp	'GetClassWord'						GetClassWord						user32		1814
imp	'GetClientRect'						GetClientRect						user32		1815	2
imp	'GetClipCursor'						GetClipCursor						user32		1816
imp	'GetClipboardAccessToken'				GetClipboardAccessToken					user32		1817
imp	'GetClipboardData'					GetClipboardData					user32		1818
imp	'GetClipboardFormatName'				GetClipboardFormatNameW					user32		1820
imp	'GetClipboardOwner'					GetClipboardOwner					user32		1821
imp	'GetClipboardSequenceNumber'				GetClipboardSequenceNumber				user32		1822
imp	'GetClipboardViewer'					GetClipboardViewer					user32		1823
imp	'GetComboBoxInfo'					GetComboBoxInfo						user32		1824
imp	'GetCurrentInputMessageSource'				GetCurrentInputMessageSource				user32		1825
imp	'GetCursor'						GetCursor						user32		1826	0
imp	'GetCursorFrameInfo'					GetCursorFrameInfo					user32		1827
imp	'GetCursorInfo'						GetCursorInfo						user32		1828
imp	'GetCursorPos'						GetCursorPos						user32		1829	1
imp	'GetDC'							GetDC							user32		1830	1
imp	'GetDCEx'						GetDCEx							user32		1831
imp	'GetDesktopID'						GetDesktopID						user32		1832
imp	'GetDesktopWindow'					GetDesktopWindow					user32		1833	0
imp	'GetDialogBaseUnits'					GetDialogBaseUnits					user32		1834
imp	'GetDialogControlDpiChangeBehavior'			GetDialogControlDpiChangeBehavior			user32		1835
imp	'GetDialogDpiChangeBehavior'				GetDialogDpiChangeBehavior				user32		1836
imp	'GetDisplayAutoRotationPreferences'			GetDisplayAutoRotationPreferences			user32		1837
imp	'GetDisplayConfigBufferSizes'				GetDisplayConfigBufferSizes				user32		1838
imp	'GetDlgCtrlID'						GetDlgCtrlID						user32		1839
imp	'GetDlgItem'						GetDlgItem						user32		1840
imp	'GetDlgItemInt'						GetDlgItemInt						user32		1841
imp	'GetDlgItemText'					GetDlgItemTextW						user32		1843
imp	'GetDoubleClickTime'					GetDoubleClickTime					user32		1844
imp	'GetDpiForMonitorInternal'				GetDpiForMonitorInternal				user32		1845
imp	'GetDpiForSystem'					GetDpiForSystem						user32		1846
imp	'GetDpiForWindow'					GetDpiForWindow						user32		1847
imp	'GetDpiFromDpiAwarenessContext'				GetDpiFromDpiAwarenessContext				user32		1848
imp	'GetFocus'						GetFocus						user32		1849
imp	'GetForegroundWindow'					GetForegroundWindow					user32		1850
imp	'GetGUIThreadInfo'					GetGUIThreadInfo					user32		1851
imp	'GetGestureConfig'					GetGestureConfig					user32		1852
imp	'GetGestureExtraArgs'					GetGestureExtraArgs					user32		1853
imp	'GetGestureInfo'					GetGestureInfo						user32		1854
imp	'GetGuiResources'					GetGuiResources						user32		1855
imp	'GetIconInfo'						GetIconInfo						user32		1856
imp	'GetIconInfoEx'						GetIconInfoExW						user32		1858
imp	'GetInputDesktop'					GetInputDesktop						user32		1859
imp	'GetInputLocaleInfo'					GetInputLocaleInfo					user32		1860
imp	'GetInputState'						GetInputState						user32		1861
imp	'GetInternalWindowPos'					GetInternalWindowPos					user32		1862
imp	'GetKBCodePage'						GetKBCodePage						user32		1863
imp	'GetKeyNameText'					GetKeyNameTextW						user32		1865
imp	'GetKeyState'						GetKeyState						user32		1866	1
imp	'GetKeyboardLayout'					GetKeyboardLayout					user32		1867	1
imp	'GetKeyboardLayoutList'					GetKeyboardLayoutList					user32		1868
imp	'GetKeyboardLayoutName'					GetKeyboardLayoutNameW					user32		1870
imp	'GetKeyboardState'					GetKeyboardState					user32		1871
imp	'GetKeyboardType'					GetKeyboardType						user32		1872
imp	'GetLastActivePopup'					GetLastActivePopup					user32		1873
imp	'GetLastInputInfo'					GetLastInputInfo					user32		1874
imp	'GetLayeredWindowAttributes'				GetLayeredWindowAttributes				user32		1875
imp	'GetListBoxInfo'					GetListBoxInfo						user32		1876
imp	'GetMagnificationDesktopColorEffect'			GetMagnificationDesktopColorEffect			user32		1877
imp	'GetMagnificationDesktopMagnification'			GetMagnificationDesktopMagnification			user32		1878
imp	'GetMagnificationDesktopSamplingMode'			GetMagnificationDesktopSamplingMode			user32		1879
imp	'GetMagnificationLensCtxInformation'			GetMagnificationLensCtxInformation			user32		1880
imp	'GetMenu'						GetMenu							user32		1881	1
imp	'GetMenuBarInfo'					GetMenuBarInfo						user32		1882
imp	'GetMenuCheckMarkDimensions'				GetMenuCheckMarkDimensions				user32		1883
imp	'GetMenuContextHelpId'					GetMenuContextHelpId					user32		1884
imp	'GetMenuDefaultItem'					GetMenuDefaultItem					user32		1885
imp	'GetMenuInfo'						GetMenuInfo						user32		1886
imp	'GetMenuItemCount'					GetMenuItemCount					user32		1887
imp	'GetMenuItemID'						GetMenuItemID						user32		1888
imp	'GetMenuItemInfo'					GetMenuItemInfoW					user32		1890
imp	'GetMenuItemRect'					GetMenuItemRect						user32		1891
imp	'GetMenuState'						GetMenuState						user32		1892
imp	'GetMenuString'						GetMenuStringW						user32		1894
imp	'GetMessage'						GetMessageW						user32		1899	4
imp	'GetMessageExtraInfo'					GetMessageExtraInfo					user32		1896
imp	'GetMessagePos'						GetMessagePos						user32		1897
imp	'GetMessageTime'					GetMessageTime						user32		1898
imp	'GetMonitorInfo'					GetMonitorInfoW						user32		1901
imp	'GetMouseMovePointsEx'					GetMouseMovePointsEx					user32		1902
imp	'GetNextDlgGroupItem'					GetNextDlgGroupItem					user32		1903
imp	'GetNextDlgTabItem'					GetNextDlgTabItem					user32		1904
imp	'GetOpenClipboardWindow'				GetOpenClipboardWindow					user32		1905
imp	'GetParent'						GetParent						user32		1906	1
imp	'GetPhysicalCursorPos'					GetPhysicalCursorPos					user32		1907
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
imp	'GetPriorityClipboardFormat'				GetPriorityClipboardFormat				user32		1929
imp	'GetProcessDefaultLayout'				GetProcessDefaultLayout					user32		1930
imp	'GetProcessDpiAwarenessInternal'			GetProcessDpiAwarenessInternal				user32		1931
imp	'GetProcessUIContextInformation'			GetProcessUIContextInformation				user32		2521
imp	'GetProcessWindowStation'				GetProcessWindowStation					user32		1932
imp	'GetProgmanWindow'					GetProgmanWindow					user32		1933
imp	'GetProp'						GetPropW						user32		1935
imp	'GetQueueStatus'					GetQueueStatus						user32		1936
imp	'GetRawInputBuffer'					GetRawInputBuffer					user32		1937
imp	'GetRawInputData'					GetRawInputData						user32		1938
imp	'GetRawInputDeviceInfo'					GetRawInputDeviceInfoW					user32		1940
imp	'GetRawInputDeviceList'					GetRawInputDeviceList					user32		1941
imp	'GetRawPointerDeviceData'				GetRawPointerDeviceData					user32		1942
imp	'GetReasonTitleFromReasonCode'				GetReasonTitleFromReasonCode				user32		1943
imp	'GetRegisteredRawInputDevices'				GetRegisteredRawInputDevices				user32		1944
imp	'GetScrollBarInfo'					GetScrollBarInfo					user32		1945
imp	'GetScrollInfo'						GetScrollInfo						user32		1946
imp	'GetScrollPos'						GetScrollPos						user32		1947
imp	'GetScrollRange'					GetScrollRange						user32		1948
imp	'GetSendMessageReceiver'				GetSendMessageReceiver					user32		1949
imp	'GetShellWindow'					GetShellWindow						user32		1950	0
imp	'GetSubMenu'						GetSubMenu						user32		1951
imp	'GetSysColor'						GetSysColor						user32		1952
imp	'GetSysColorBrush'					GetSysColorBrush					user32		1953
imp	'GetSystemDpiForProcess'				GetSystemDpiForProcess					user32		1954
imp	'GetSystemMenu'						GetSystemMenu						user32		1955	2
imp	'GetSystemMetrics'					GetSystemMetrics					user32		1956
imp	'GetSystemMetricsForDpi'				GetSystemMetricsForDpi					user32		1957
imp	'GetTabbedTextExtent'					GetTabbedTextExtentW					user32		1959
imp	'GetTaskmanWindow'					GetTaskmanWindow					user32		1960
imp	'GetThreadDesktop'					GetThreadDesktop					user32		1961
imp	'GetThreadDpiAwarenessContext'				GetThreadDpiAwarenessContext				user32		1962
imp	'GetThreadDpiHostingBehavior'				GetThreadDpiHostingBehavior				user32		1963
imp	'GetTitleBarInfo'					GetTitleBarInfo						user32		1964
imp	'GetTopLevelWindow'					GetTopLevelWindow					user32		1965
imp	'GetTopWindow'						GetTopWindow						user32		1966
imp	'GetTouchInputInfo'					GetTouchInputInfo					user32		1967
imp	'GetUnpredictedMessagePos'				GetUnpredictedMessagePos				user32		1968
imp	'GetUpdateRect'						GetUpdateRect						user32		1969
imp	'GetUpdateRgn'						GetUpdateRgn						user32		1970
imp	'GetUpdatedClipboardFormats'				GetUpdatedClipboardFormats				user32		1971
imp	'GetUserObjectInformation'				GetUserObjectInformationW				user32		1973
imp	'GetUserObjectSecurity'					GetUserObjectSecurity					user32		1974
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
imp	'GetWindowFeedbackSetting'				GetWindowFeedbackSetting				user32		1985
imp	'GetWindowInfo'						GetWindowInfo						user32		1986
imp	'GetWindowLong'						GetWindowLongW						user32		1990
imp	'GetWindowLongPtr'					GetWindowLongPtrW					user32		1989
imp	'GetWindowMinimizeRect'					GetWindowMinimizeRect					user32		1991
imp	'GetWindowModuleFileName'				GetWindowModuleFileNameW				user32		1994
imp	'GetWindowPlacement'					GetWindowPlacement					user32		1995	2
imp	'GetWindowProcessHandle'				GetWindowProcessHandle					user32		1996
imp	'GetWindowRect'						GetWindowRect						user32		1997	2
imp	'GetWindowRgn'						GetWindowRgn						user32		1998
imp	'GetWindowRgnBox'					GetWindowRgnBox						user32		1999
imp	'GetWindowRgnEx'					GetWindowRgnEx						user32		2000
imp	'GetWindowText'						GetWindowTextW						user32		2007	3
imp	'GetWindowTextLength'					GetWindowTextLengthW					user32		2006
imp	'GetWindowThreadProcessId'				GetWindowThreadProcessId				user32		2008
imp	'GetWindowWord'						GetWindowWord						user32		2009
imp	'GhostWindowFromHungWindow'				GhostWindowFromHungWindow				user32		2011
imp	'GrayString'						GrayStringW						user32		2013
imp	'HandleDelegatedInput'					HandleDelegatedInput					user32		2505
imp	'HideCaret'						HideCaret						user32		2014
imp	'HiliteMenuItem'					HiliteMenuItem						user32		2015
imp	'HungWindowFromGhostWindow'				HungWindowFromGhostWindow				user32		2016
imp	'IMPGetIMEW'						IMPGetIMEW						user32		2018
imp	'IMPQueryIMEW'						IMPQueryIMEW						user32		2020
imp	'IMPSetIMEW'						IMPSetIMEW						user32		2022
imp	'ImpersonateDdeClientWindow'				ImpersonateDdeClientWindow				user32		2023
imp	'InSendMessage'						InSendMessage						user32		2024
imp	'InSendMessageEx'					InSendMessageEx						user32		2025
imp	'InflateRect'						InflateRect						user32		2026
imp	'InheritWindowMonitor'					InheritWindowMonitor					user32		2027
imp	'InitDManipHook'					InitDManipHook						user32		2028
imp	'InitializeGenericHidInjection'				InitializeGenericHidInjection				user32		2029
imp	'InitializeInputDeviceInjection'			InitializeInputDeviceInjection				user32		2030
imp	'InitializeLpkHooks'					InitializeLpkHooks					user32		2031
imp	'InitializePointerDeviceInjection'			InitializePointerDeviceInjection			user32		2032
imp	'InitializePointerDeviceInjectionEx'			InitializePointerDeviceInjectionEx			user32		2033
imp	'InitializeTouchInjection'				InitializeTouchInjection				user32		2034
imp	'InjectDeviceInput'					InjectDeviceInput					user32		2035
imp	'InjectGenericHidInput'					InjectGenericHidInput					user32		2036
imp	'InjectKeyboardInput'					InjectKeyboardInput					user32		2037
imp	'InjectMouseInput'					InjectMouseInput					user32		2038
imp	'InjectPointerInput'					InjectPointerInput					user32		2039
imp	'InjectTouchInput'					InjectTouchInput					user32		2040
imp	'InsertMenu'						InsertMenuW						user32		2044	5
imp	'InsertMenuItem'					InsertMenuItemW						user32		2043
imp	'InternalGetWindowIcon'					InternalGetWindowIcon					user32		2045
imp	'InternalGetWindowText'					InternalGetWindowText					user32		2046
imp	'IntersectRect'						IntersectRect						user32		2047
imp	'InvalidateRect'					InvalidateRect						user32		2048	3
imp	'InvalidateRgn'						InvalidateRgn						user32		2049
imp	'InvertRect'						InvertRect						user32		2050
imp	'IsChild'						IsChild							user32		2059	2
imp	'IsClipboardFormatAvailable'				IsClipboardFormatAvailable				user32		2060
imp	'IsDialogMessage'					IsDialogMessageW					user32		2063
imp	'IsDlgButtonChecked'					IsDlgButtonChecked					user32		2064
imp	'IsGUIThread'						IsGUIThread						user32		2065
imp	'IsHungAppWindow'					IsHungAppWindow						user32		2066
imp	'IsIconic'						IsIconic						user32		2067	1
imp	'IsImmersiveProcess'					IsImmersiveProcess					user32		2068
imp	'IsInDesktopWindowBand'					IsInDesktopWindowBand					user32		2069
imp	'IsMenu'						IsMenu							user32		2070	1
imp	'IsMouseInPointerEnabled'				IsMouseInPointerEnabled					user32		2071
imp	'IsOneCoreTransformMode'				IsOneCoreTransformMode					user32		2072
imp	'IsProcessDPIAware'					IsProcessDPIAware					user32		2073
imp	'IsQueueAttached'					IsQueueAttached						user32		2074
imp	'IsRectEmpty'						IsRectEmpty						user32		2075
imp	'IsSETEnabled'						IsSETEnabled						user32		2076
imp	'IsServerSideWindow'					IsServerSideWindow					user32		2077
imp	'IsThreadDesktopComposited'				IsThreadDesktopComposited				user32		2078
imp	'IsThreadMessageQueueAttached'				IsThreadMessageQueueAttached				user32		2528
imp	'IsThreadTSFEventAware'					IsThreadTSFEventAware					user32		2079
imp	'IsTopLevelWindow'					IsTopLevelWindow					user32		2080
imp	'IsTouchWindow'						IsTouchWindow						user32		2081
imp	'IsValidDpiAwarenessContext'				IsValidDpiAwarenessContext				user32		2082
imp	'IsWinEventHookInstalled'				IsWinEventHookInstalled					user32		2083
imp	'IsWindow'						IsWindow						user32		2084	1
imp	'IsWindowArranged'					IsWindowArranged					user32		2085
imp	'IsWindowEnabled'					IsWindowEnabled						user32		2086
imp	'IsWindowInDestroy'					IsWindowInDestroy					user32		2087
imp	'IsWindowRedirectedForPrint'				IsWindowRedirectedForPrint				user32		2088
imp	'IsWindowUnicode'					IsWindowUnicode						user32		2089
imp	'IsWindowVisible'					IsWindowVisible						user32		2090	1
imp	'IsZoomed'						IsZoomed						user32		2092	1
imp	'KillTimer'						KillTimer						user32		2093	2
imp	'LoadAccelerators'					LoadAcceleratorsW					user32		2095
imp	'LoadBitmap'						LoadBitmapW						user32		2097
imp	'LoadCursor'						LoadCursorW						user32		2101	2
imp	'LoadCursorFromFile'					LoadCursorFromFileW					user32		2100
imp	'LoadIcon'						LoadIconW						user32		2103	2
imp	'LoadImage'						LoadImageW						user32		2105	6
imp	'LoadKeyboardLayout'					LoadKeyboardLayoutW					user32		2108
imp	'LoadKeyboardLayoutEx'					LoadKeyboardLayoutEx					user32		2107
imp	'LoadLocalFonts'					LoadLocalFonts						user32		2109
imp	'LoadMenu'						LoadMenuW						user32		2113
imp	'LoadMenuIndirect'					LoadMenuIndirectW					user32		2112
imp	'LoadRemoteFonts'					LoadRemoteFonts						user32		2114
imp	'LockSetForegroundWindow'				LockSetForegroundWindow					user32		2117
imp	'LockWindowStation'					LockWindowStation					user32		2118
imp	'LockWindowUpdate'					LockWindowUpdate					user32		2119
imp	'LockWorkStation'					LockWorkStation						user32		2120
imp	'LogicalToPhysicalPoint'				LogicalToPhysicalPoint					user32		2121
imp	'LogicalToPhysicalPointForPerMonitorDPI'		LogicalToPhysicalPointForPerMonitorDPI			user32		2122
imp	'LookupIconIdFromDirectory'				LookupIconIdFromDirectory				user32		2123
imp	'LookupIconIdFromDirectoryEx'				LookupIconIdFromDirectoryEx				user32		2124
imp	'MBToWCSEx'						MBToWCSEx						user32		2125
imp	'MBToWCSExt'						MBToWCSExt						user32		2126
imp	'MB_GetString'						MB_GetString						user32		2127
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
imp	'MakeThreadTSFEventAware'				MakeThreadTSFEventAware					user32		2151
imp	'MapDialogRect'						MapDialogRect						user32		2152
imp	'MapVirtualKey'						MapVirtualKeyW						user32		2156
imp	'MapVirtualKeyEx'					MapVirtualKeyExW					user32		2155	3
imp	'MapVisualRelativePoints'				MapVisualRelativePoints					user32		2157
imp	'MapWindowPoints'					MapWindowPoints						user32		2158
imp	'MenuItemFromPoint'					MenuItemFromPoint					user32		2159
imp	'MenuWindowProc'					MenuWindowProcW						user32		2161
imp	'MessageBeep'						MessageBeep						user32		2162
imp	'MessageBox'						MessageBoxW						user32		2170	4
imp	'MessageBoxEx'						MessageBoxExW						user32		2165	5
imp	'MessageBoxIndirect'					MessageBoxIndirectW					user32		2167
imp	'MessageBoxTimeout'					MessageBoxTimeoutW					user32		2169
imp	'ModifyMenu'						ModifyMenuW						user32		2172
imp	'MonitorFromPoint'					MonitorFromPoint					user32		2173
imp	'MonitorFromRect'					MonitorFromRect						user32		2174
imp	'MonitorFromWindow'					MonitorFromWindow					user32		2175
imp	'MoveWindow'						MoveWindow						user32		2176	6
imp	'MsgWaitForMultipleObjects'				MsgWaitForMultipleObjects				user32		2177
imp	'MsgWaitForMultipleObjectsEx'				MsgWaitForMultipleObjectsEx				user32		2178
imp	'NotifyOverlayWindow'					NotifyOverlayWindow					user32		2179
imp	'NotifyWinEvent'					NotifyWinEvent						user32		2180
imp	'OemKeyScan'						OemKeyScan						user32		2181
imp	'OemToChar'						OemToCharW						user32		2185
imp	'OemToCharBuff'						OemToCharBuffW						user32		2184
imp	'OffsetRect'						OffsetRect						user32		2186
imp	'OpenClipboard'						OpenClipboard						user32		2187
imp	'OpenDesktop'						OpenDesktopW						user32		2189
imp	'OpenIcon'						OpenIcon						user32		2190
imp	'OpenInputDesktop'					OpenInputDesktop					user32		2191
imp	'OpenThreadDesktop'					OpenThreadDesktop					user32		2192
imp	'OpenWindowStation'					OpenWindowStationW					user32		2194
imp	'PackDDElParam'						PackDDElParam						user32		2195
imp	'PackTouchHitTestingProximityEvaluation'		PackTouchHitTestingProximityEvaluation			user32		2196
imp	'PaintDesktop'						PaintDesktop						user32		2197
imp	'PaintMenuBar'						PaintMenuBar						user32		2198
imp	'PaintMonitor'						PaintMonitor						user32		2199
imp	'PeekMessage'						PeekMessageW						user32		2201	5
imp	'PhysicalToLogicalPoint'				PhysicalToLogicalPoint					user32		2202
imp	'PhysicalToLogicalPointForPerMonitorDPI'		PhysicalToLogicalPointForPerMonitorDPI			user32		2203
imp	'PostMessage'						PostMessageW						user32		2205
imp	'PostQuitMessage'					PostQuitMessage						user32		2206	1
imp	'PostThreadMessage'					PostThreadMessageW					user32		2208
imp	'PrintWindow'						PrintWindow						user32		2209
imp	'PrivateExtractIconEx'					PrivateExtractIconExW					user32		2211
imp	'PrivateExtractIcons'					PrivateExtractIconsW					user32		2213
imp	'PrivateRegisterICSProc'				PrivateRegisterICSProc					user32		2214
imp	'PtInRect'						PtInRect						user32		2215
imp	'QueryBSDRWindow'					QueryBSDRWindow						user32		2216
imp	'QueryDisplayConfig'					QueryDisplayConfig					user32		2217
imp	'QuerySendMessage'					QuerySendMessage					user32		2218
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
imp	'RealChildWindowFromPoint'				RealChildWindowFromPoint				user32		2239
imp	'RealGetWindowClass'					RealGetWindowClassW					user32		2242
imp	'ReasonCodeNeedsBugID'					ReasonCodeNeedsBugID					user32		2243
imp	'ReasonCodeNeedsComment'				ReasonCodeNeedsComment					user32		2244
imp	'RecordShutdownReason'					RecordShutdownReason					user32		2245
imp	'RedrawWindow'						RedrawWindow						user32		2246	4
imp	'RegisterBSDRWindow'					RegisterBSDRWindow					user32		2247
imp	'RegisterClass'						RegisterClassW						user32		2251	1
imp	'RegisterClassEx'					RegisterClassExW					user32		2250	1
imp	'RegisterClipboardFormat'				RegisterClipboardFormatW				user32		2253
imp	'RegisterDManipHook'					RegisterDManipHook					user32		2254
imp	'RegisterDeviceNotification'				RegisterDeviceNotificationW				user32		2256
imp	'RegisterErrorReportingDialog'				RegisterErrorReportingDialog				user32		2257
imp	'RegisterFrostWindow'					RegisterFrostWindow					user32		2258
imp	'RegisterGhostWindow'					RegisterGhostWindow					user32		2259
imp	'RegisterHotKey'					RegisterHotKey						user32		2260
imp	'RegisterLogonProcess'					RegisterLogonProcess					user32		2261
imp	'RegisterMessagePumpHook'				RegisterMessagePumpHook					user32		2262
imp	'RegisterPointerDeviceNotifications'			RegisterPointerDeviceNotifications			user32		2263
imp	'RegisterPointerInputTarget'				RegisterPointerInputTarget				user32		2264
imp	'RegisterPointerInputTargetEx'				RegisterPointerInputTargetEx				user32		2265
imp	'RegisterPowerSettingNotification'			RegisterPowerSettingNotification			user32		2266
imp	'RegisterRawInputDevices'				RegisterRawInputDevices					user32		2267
imp	'RegisterServicesProcess'				RegisterServicesProcess					user32		2268
imp	'RegisterSessionPort'					RegisterSessionPort					user32		2269
imp	'RegisterShellHookWindow'				RegisterShellHookWindow					user32		2270
imp	'RegisterSuspendResumeNotification'			RegisterSuspendResumeNotification			user32		2271
imp	'RegisterSystemThread'					RegisterSystemThread					user32		2272
imp	'RegisterTasklist'					RegisterTasklist					user32		2273
imp	'RegisterTouchHitTestingWindow'				RegisterTouchHitTestingWindow				user32		2274
imp	'RegisterTouchWindow'					RegisterTouchWindow					user32		2275
imp	'RegisterUserApiHook'					RegisterUserApiHook					user32		2276
imp	'RegisterWindowMessage'					RegisterWindowMessageW					user32		2278
imp	'ReleaseCapture'					ReleaseCapture						user32		2279	0
imp	'ReleaseDC'						ReleaseDC						user32		2280	2
imp	'ReleaseDwmHitTestWaiters'				ReleaseDwmHitTestWaiters				user32		2281
imp	'RemoveClipboardFormatListener'				RemoveClipboardFormatListener				user32		2282
imp	'RemoveInjectionDevice'					RemoveInjectionDevice					user32		2283
imp	'RemoveMenu'						RemoveMenu						user32		2284
imp	'RemoveProp'						RemovePropW						user32		2286
imp	'RemoveThreadTSFEventAwareness'				RemoveThreadTSFEventAwareness				user32		2287
imp	'ReplyMessage'						ReplyMessage						user32		2288
imp	'ReportInertia'						ReportInertia						user32		2551
imp	'ResolveDesktopForWOW'					ResolveDesktopForWOW					user32		2289
imp	'ReuseDDElParam'					ReuseDDElParam						user32		2290
imp	'ScreenToClient'					ScreenToClient						user32		2291
imp	'ScrollChildren'					ScrollChildren						user32		2292
imp	'ScrollDC'						ScrollDC						user32		2293
imp	'ScrollWindow'						ScrollWindow						user32		2294
imp	'ScrollWindowEx'					ScrollWindowEx						user32		2295
imp	'SendDlgItemMessage'					SendDlgItemMessageW					user32		2297
imp	'SendIMEMessageEx'					SendIMEMessageExW					user32		2299
imp	'SendInput'						SendInput						user32		2300
imp	'SendMessage'						SendMessageW						user32		2306	4
imp	'SendMessageCallback'					SendMessageCallbackW					user32		2303
imp	'SendMessageTimeout'					SendMessageTimeoutW					user32		2305
imp	'SendNotifyMessage'					SendNotifyMessageW					user32		2308
imp	'SetActiveWindow'					SetActiveWindow						user32		2309
imp	'SetCapture'						SetCapture						user32		2310	1
imp	'SetCaretBlinkTime'					SetCaretBlinkTime					user32		2311
imp	'SetCaretPos'						SetCaretPos						user32		2312
imp	'SetClassLong'						SetClassLongW						user32		2316	3
imp	'SetClassLongPtr'					SetClassLongPtrW					user32		2315
imp	'SetClassWord'						SetClassWord						user32		2317
imp	'SetClipboardData'					SetClipboardData					user32		2318
imp	'SetClipboardViewer'					SetClipboardViewer					user32		2319
imp	'SetCoalescableTimer'					SetCoalescableTimer					user32		2320
imp	'SetCoreWindow'						SetCoreWindow						user32		2571
imp	'SetCursor'						SetCursor						user32		2321	1
imp	'SetCursorContents'					SetCursorContents					user32		2322
imp	'SetCursorPos'						SetCursorPos						user32		2323
imp	'SetDebugErrorLevel'					SetDebugErrorLevel					user32		2324
imp	'SetDeskWallpaper'					SetDeskWallpaper					user32		2325
imp	'SetDesktopColorTransform'				SetDesktopColorTransform				user32		2326
imp	'SetDialogControlDpiChangeBehavior'			SetDialogControlDpiChangeBehavior			user32		2327
imp	'SetDialogDpiChangeBehavior'				SetDialogDpiChangeBehavior				user32		2328
imp	'SetDisplayAutoRotationPreferences'			SetDisplayAutoRotationPreferences			user32		2329
imp	'SetDisplayConfig'					SetDisplayConfig					user32		2330
imp	'SetDlgItemInt'						SetDlgItemInt						user32		2331
imp	'SetDlgItemText'					SetDlgItemTextW						user32		2333
imp	'SetDoubleClickTime'					SetDoubleClickTime					user32		2334
imp	'SetFeatureReportResponse'				SetFeatureReportResponse				user32		2335
imp	'SetFocus'						SetFocus						user32		2336
imp	'SetForegroundWindow'					SetForegroundWindow					user32		2337
imp	'SetGestureConfig'					SetGestureConfig					user32		2338
imp	'SetInternalWindowPos'					SetInternalWindowPos					user32		2339
imp	'SetKeyboardState'					SetKeyboardState					user32		2340
imp	'SetLastErrorEx'					SetLastErrorEx						user32		2341
imp	'SetLayeredWindowAttributes'				SetLayeredWindowAttributes				user32		2342
imp	'SetMagnificationDesktopColorEffect'			SetMagnificationDesktopColorEffect			user32		2343
imp	'SetMagnificationDesktopMagnification'			SetMagnificationDesktopMagnification			user32		2344
imp	'SetMagnificationDesktopSamplingMode'			SetMagnificationDesktopSamplingMode			user32		2345
imp	'SetMagnificationLensCtxInformation'			SetMagnificationLensCtxInformation			user32		2346
imp	'SetMenu'						SetMenu							user32		2347
imp	'SetMenuContextHelpId'					SetMenuContextHelpId					user32		2348
imp	'SetMenuDefaultItem'					SetMenuDefaultItem					user32		2349
imp	'SetMenuInfo'						SetMenuInfo						user32		2350
imp	'SetMenuItemBitmaps'					SetMenuItemBitmaps					user32		2351
imp	'SetMenuItemInfo'					SetMenuItemInfoW					user32		2353
imp	'SetMessageExtraInfo'					SetMessageExtraInfo					user32		2354
imp	'SetMessageQueue'					SetMessageQueue						user32		2355
imp	'SetMirrorRendering'					SetMirrorRendering					user32		2356
imp	'SetParent'						SetParent						user32		2357	2
imp	'SetPhysicalCursorPos'					SetPhysicalCursorPos					user32		2358
imp	'SetProcessDPIAware'					SetProcessDPIAware					user32		2359
imp	'SetProcessDefaultLayout'				SetProcessDefaultLayout					user32		2360
imp	'SetProcessDpiAwarenessContext'				SetProcessDpiAwarenessContext				user32		2361
imp	'SetProcessDpiAwarenessInternal'			SetProcessDpiAwarenessInternal				user32		2362
imp	'SetProcessRestrictionExemption'			SetProcessRestrictionExemption				user32		2363
imp	'SetProcessWindowStation'				SetProcessWindowStation					user32		2364
imp	'SetProgmanWindow'					SetProgmanWindow					user32		2365
imp	'SetProp'						SetPropW						user32		2367
imp	'SetRect'						SetRect							user32		2368
imp	'SetRectEmpty'						SetRectEmpty						user32		2369
imp	'SetScrollInfo'						SetScrollInfo						user32		2370
imp	'SetScrollPos'						SetScrollPos						user32		2371
imp	'SetScrollRange'					SetScrollRange						user32		2372
imp	'SetShellWindow'					SetShellWindow						user32		2373
imp	'SetShellWindowEx'					SetShellWindowEx					user32		2374
imp	'SetSysColors'						SetSysColors						user32		2375
imp	'SetSysColorsTemp'					SetSysColorsTemp					user32		2376
imp	'SetSystemCursor'					SetSystemCursor						user32		2377
imp	'SetSystemMenu'						SetSystemMenu						user32		2378
imp	'SetTaskmanWindow'					SetTaskmanWindow					user32		2379
imp	'SetThreadDesktop'					SetThreadDesktop					user32		2380
imp	'SetThreadDpiAwarenessContext'				SetThreadDpiAwarenessContext				user32		2381
imp	'SetThreadDpiHostingBehavior'				SetThreadDpiHostingBehavior				user32		2382
imp	'SetThreadInputBlocked'					SetThreadInputBlocked					user32		2383
imp	'SetTimer'						SetTimer						user32		2384	4
imp	'SetUserObjectInformation'				SetUserObjectInformationW				user32		2386
imp	'SetUserObjectSecurity'					SetUserObjectSecurity					user32		2387
imp	'SetWinEventHook'					SetWinEventHook						user32		2388
imp	'SetWindowBand'						SetWindowBand						user32		2389
imp	'SetWindowCompositionAttribute'				SetWindowCompositionAttribute				user32		2390
imp	'SetWindowCompositionTransition'			SetWindowCompositionTransition				user32		2391
imp	'SetWindowContextHelpId'				SetWindowContextHelpId					user32		2392
imp	'SetWindowDisplayAffinity'				SetWindowDisplayAffinity				user32		2393
imp	'SetWindowFeedbackSetting'				SetWindowFeedbackSetting				user32		2394
imp	'SetWindowLong'						SetWindowLongW						user32		2398	3
imp	'SetWindowLongPtr'					SetWindowLongPtrW					user32		2397
imp	'SetWindowPlacement'					SetWindowPlacement					user32		2399	2
imp	'SetWindowPos'						SetWindowPos						user32		2400	7
imp	'SetWindowRgn'						SetWindowRgn						user32		2401
imp	'SetWindowRgnEx'					SetWindowRgnEx						user32		2402
imp	'SetWindowStationUser'					SetWindowStationUser					user32		2403
imp	'SetWindowText'						SetWindowTextW						user32		2405	2
imp	'SetWindowWord'						SetWindowWord						user32		2406
imp	'SetWindowsHook'					SetWindowsHookW						user32		2410	2
imp	'SetWindowsHookEx'					SetWindowsHookExW					user32		2409	4
imp	'ShowCaret'						ShowCaret						user32		2411	1
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
imp	'SignalRedirectionStartComplete'			SignalRedirectionStartComplete				user32		2422
imp	'SkipPointerFrameMessages'				SkipPointerFrameMessages				user32		2423
imp	'SoftModalMessageBox'					SoftModalMessageBox					user32		2424
imp	'SoundSentry'						SoundSentry						user32		2425
imp	'SubtractRect'						SubtractRect						user32		2426
imp	'SwapMouseButton'					SwapMouseButton						user32		2427
imp	'SwitchDesktop'						SwitchDesktop						user32		2428
imp	'SwitchDesktopWithFade'					SwitchDesktopWithFade					user32		2429
imp	'SwitchToThisWindow'					SwitchToThisWindow					user32		2430
imp	'SystemParametersInfo'					SystemParametersInfoW					user32		2433
imp	'SystemParametersInfoForDpi'				SystemParametersInfoForDpi				user32		2432
imp	'TabbedTextOut'						TabbedTextOutW						user32		2435
imp	'TileChildWindows'					TileChildWindows					user32		2436
imp	'TileWindows'						TileWindows						user32		2437
imp	'ToAscii'						ToAscii							user32		2438
imp	'ToAsciiEx'						ToAsciiEx						user32		2439
imp	'ToUnicode'						ToUnicode						user32		2440
imp	'ToUnicodeEx'						ToUnicodeEx						user32		2441
imp	'TrackMouseEvent'					TrackMouseEvent						user32		2442
imp	'TrackPopupMenu'					TrackPopupMenu						user32		2443	7
imp	'TrackPopupMenuEx'					TrackPopupMenuEx					user32		2444
imp	'TranslateAccelerator'					TranslateAcceleratorW					user32		2447
imp	'TranslateMDISysAccel'					TranslateMDISysAccel					user32		2448
imp	'TranslateMessage'					TranslateMessage					user32		2449	1
imp	'TranslateMessageEx'					TranslateMessageEx					user32		2450
imp	'UndelegateInput'					UndelegateInput						user32		2504
imp	'UnhookWinEvent'					UnhookWinEvent						user32		2451
imp	'UnhookWindowsHook'					UnhookWindowsHook					user32		2452	2
imp	'UnhookWindowsHookEx'					UnhookWindowsHookEx					user32		2453	1
imp	'UnionRect'						UnionRect						user32		2454
imp	'UnloadKeyboardLayout'					UnloadKeyboardLayout					user32		2455
imp	'UnlockWindowStation'					UnlockWindowStation					user32		2456
imp	'UnpackDDElParam'					UnpackDDElParam						user32		2457
imp	'UnregisterClass'					UnregisterClassW					user32		2459
imp	'UnregisterDeviceNotification'				UnregisterDeviceNotification				user32		2460
imp	'UnregisterHotKey'					UnregisterHotKey					user32		2461
imp	'UnregisterMessagePumpHook'				UnregisterMessagePumpHook				user32		2462
imp	'UnregisterPointerInputTarget'				UnregisterPointerInputTarget				user32		2463
imp	'UnregisterPointerInputTargetEx'			UnregisterPointerInputTargetEx				user32		2464
imp	'UnregisterPowerSettingNotification'			UnregisterPowerSettingNotification			user32		2465
imp	'UnregisterSessionPort'					UnregisterSessionPort					user32		2466
imp	'UnregisterSuspendResumeNotification'			UnregisterSuspendResumeNotification			user32		2467
imp	'UnregisterTouchWindow'					UnregisterTouchWindow					user32		2468
imp	'UnregisterUserApiHook'					UnregisterUserApiHook					user32		2469
imp	'UpdateDefaultDesktopThumbnail'				UpdateDefaultDesktopThumbnail				user32		2470
imp	'UpdateLayeredWindow'					UpdateLayeredWindow					user32		2471
imp	'UpdateLayeredWindowIndirect'				UpdateLayeredWindowIndirect				user32		2472
imp	'UpdatePerUserSystemParameters'				UpdatePerUserSystemParameters				user32		2473
imp	'UpdateWindow'						UpdateWindow						user32		2474	1
imp	'UpdateWindowInputSinkHints'				UpdateWindowInputSinkHints				user32		2475
imp	'User32InitializeImmEntryTable'				User32InitializeImmEntryTable				user32		2476
imp	'UserClientDllInitialize'				UserClientDllInitialize					user32		2477
imp	'UserHandleGrantAccess'					UserHandleGrantAccess					user32		2478
imp	'UserLpkPSMTextOut'					UserLpkPSMTextOut					user32		2479
imp	'UserLpkTabbedTextOut'					UserLpkTabbedTextOut					user32		2480
imp	'UserRealizePalette'					UserRealizePalette					user32		2481
imp	'UserRegisterWowHandlers'				UserRegisterWowHandlers					user32		2482
imp	'VRipOutput'						VRipOutput						user32		2483
imp	'VTagOutput'						VTagOutput						user32		2484
imp	'ValidateRect'						ValidateRect						user32		2485
imp	'ValidateRgn'						ValidateRgn						user32		2486
imp	'VkKeyScan'						VkKeyScanW						user32		2490
imp	'VkKeyScanEx'						VkKeyScanExW						user32		2489
imp	'WCSToMBEx'						WCSToMBEx						user32		2491
imp	'WINNLSEnableIME'					WINNLSEnableIME						user32		2492
imp	'WINNLSGetEnableStatus'					WINNLSGetEnableStatus					user32		2493
imp	'WINNLSGetIMEHotkey'					WINNLSGetIMEHotkey					user32		2494
imp	'WaitForInputIdle'					WaitForInputIdle					user32		2495	2
imp	'WaitForRedirectionStartComplete'			WaitForRedirectionStartComplete				user32		2496
imp	'WaitMessage'						WaitMessage						user32		2497
imp	'WinHelp'						WinHelpW						user32		2499
imp	'WindowFromDC'						WindowFromDC						user32		2500
imp	'WindowFromPhysicalPoint'				WindowFromPhysicalPoint					user32		2501
imp	'WindowFromPoint'					WindowFromPoint						user32		2502

# GDI32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'AbortDoc'						AbortDoc						gdi32		1011
imp	'AbortPath'						AbortPath						gdi32		1012
imp	'AddFontMemResourceEx'					AddFontMemResourceEx					gdi32		1017
imp	'AddFontResource'					AddFontResourceW					gdi32		1022
imp	'AddFontResourceEx'					AddFontResourceExW					gdi32		1020
imp	'AddFontResourceTracking'				AddFontResourceTracking					gdi32		1021
imp	'AngleArc'						AngleArc						gdi32		1023
imp	'AnimatePalette'					AnimatePalette						gdi32		1024
imp	'AnyLinkedFonts'					AnyLinkedFonts						gdi32		1025
imp	'Arc'							Arc							gdi32		1026
imp	'ArcTo'							ArcTo							gdi32		1027
imp	'BRUSHOBJ_hGetColorTransform'				BRUSHOBJ_hGetColorTransform				gdi32		1028
imp	'BRUSHOBJ_pvAllocRbrush'				BRUSHOBJ_pvAllocRbrush					gdi32		1029
imp	'BRUSHOBJ_pvGetRbrush'					BRUSHOBJ_pvGetRbrush					gdi32		1030
imp	'BRUSHOBJ_ulGetBrushColor'				BRUSHOBJ_ulGetBrushColor				gdi32		1031
imp	'BeginGdiRendering'					BeginGdiRendering					gdi32		1032
imp	'BeginPath'						BeginPath						gdi32		1033
imp	'BitBlt'						BitBlt							gdi32		1034	9
imp	'CLIPOBJ_bEnum'						CLIPOBJ_bEnum						gdi32		1035
imp	'CLIPOBJ_cEnumStart'					CLIPOBJ_cEnumStart					gdi32		1036
imp	'CLIPOBJ_ppoGetPath'					CLIPOBJ_ppoGetPath					gdi32		1037
imp	'CancelDC'						CancelDC						gdi32		1038
imp	'CheckColorsInGamut'					CheckColorsInGamut					gdi32		1039
imp	'ChoosePixelFormat'					ChoosePixelFormat					gdi32		1040	2
imp	'Chord'							Chord							gdi32		1041
imp	'ClearBitmapAttributes'					ClearBitmapAttributes					gdi32		1042
imp	'ClearBrushAttributes'					ClearBrushAttributes					gdi32		1043
imp	'CloseEnhMetaFile'					CloseEnhMetaFile					gdi32		1044
imp	'CloseFigure'						CloseFigure						gdi32		1045
imp	'CloseMetaFile'						CloseMetaFile						gdi32		1046
imp	'ColorCorrectPalette'					ColorCorrectPalette					gdi32		1047
imp	'ColorMatchToTarget'					ColorMatchToTarget					gdi32		1048
imp	'CombineRgn'						CombineRgn						gdi32		1049
imp	'CombineTransform'					CombineTransform					gdi32		1050
imp	'ConfigureOPMProtectedOutput'				ConfigureOPMProtectedOutput				gdi32		1051
imp	'CopyEnhMetaFile'					CopyEnhMetaFileW					gdi32		1053
imp	'CopyMetaFile'						CopyMetaFileW						gdi32		1055
imp	'CreateBitmap'						CreateBitmap						gdi32		1056	5
imp	'CreateBitmapFromDxSurface'				CreateBitmapFromDxSurface				gdi32		1057
imp	'CreateBitmapFromDxSurface2'				CreateBitmapFromDxSurface2				gdi32		1058
imp	'CreateBitmapIndirect'					CreateBitmapIndirect					gdi32		1059
imp	'CreateBrushIndirect'					CreateBrushIndirect					gdi32		1060
imp	'CreateColorSpace'					CreateColorSpaceW					gdi32		1062
imp	'CreateCompatibleBitmap'				CreateCompatibleBitmap					gdi32		1063	3
imp	'CreateCompatibleDC'					CreateCompatibleDC					gdi32		1064	1
imp	'CreateDCEx'						CreateDCExW						gdi32		2000
imp	'CreateDCW'						CreateDCW						gdi32		1066
imp	'CreateDIBPatternBrush'					CreateDIBPatternBrush					gdi32		1067
imp	'CreateDIBPatternBrushPt'				CreateDIBPatternBrushPt					gdi32		1068
imp	'CreateDIBSection'					CreateDIBSection					gdi32		1069	6
imp	'CreateDIBitmap'					CreateDIBitmap						gdi32		1070
imp	'CreateDPIScaledDIBSection'				CreateDPIScaledDIBSection				gdi32		1071
imp	'CreateDiscardableBitmap'				CreateDiscardableBitmap					gdi32		1072
imp	'CreateEllipticRgn'					CreateEllipticRgn					gdi32		1073
imp	'CreateEllipticRgnIndirect'				CreateEllipticRgnIndirect				gdi32		1074
imp	'CreateEnhMetaFile'					CreateEnhMetaFileW					gdi32		1076
imp	'CreateFont'						CreateFontW						gdi32		1082
imp	'CreateFontIndirect'					CreateFontIndirectW					gdi32		1081
imp	'CreateFontIndirectEx'					CreateFontIndirectExW					gdi32		1080
imp	'CreateHalftonePalette'					CreateHalftonePalette					gdi32		1083
imp	'CreateHatchBrush'					CreateHatchBrush					gdi32		1084
imp	'CreateICW'						CreateICW						gdi32		1086
imp	'CreateMetaFile'					CreateMetaFileW						gdi32		1088
imp	'CreateOPMProtectedOutput'				CreateOPMProtectedOutput				gdi32		1089
imp	'CreateOPMProtectedOutputs'				CreateOPMProtectedOutputs				gdi32		1090
imp	'CreatePalette'						CreatePalette						gdi32		1091
imp	'CreatePatternBrush'					CreatePatternBrush					gdi32		1092
imp	'CreatePen'						CreatePen						gdi32		1093
imp	'CreatePenIndirect'					CreatePenIndirect					gdi32		1094
imp	'CreatePolyPolygonRgn'					CreatePolyPolygonRgn					gdi32		1095
imp	'CreatePolygonRgn'					CreatePolygonRgn					gdi32		1096
imp	'CreateRectRgn'						CreateRectRgn						gdi32		1097	4
imp	'CreateRectRgnIndirect'					CreateRectRgnIndirect					gdi32		1098
imp	'CreateRoundRectRgn'					CreateRoundRectRgn					gdi32		1099
imp	'CreateScalableFontResource'				CreateScalableFontResourceW				gdi32		1101
imp	'CreateSessionMappedDIBSection'				CreateSessionMappedDIBSection				gdi32		1102
imp	'CreateSolidBrush'					CreateSolidBrush					gdi32		1103
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
imp	'DDCCIGetCapabilitiesString'				DDCCIGetCapabilitiesString				gdi32		1315
imp	'DDCCIGetCapabilitiesStringLength'			DDCCIGetCapabilitiesStringLength			gdi32		1316
imp	'DDCCIGetTimingReport'					DDCCIGetTimingReport					gdi32		1317
imp	'DDCCIGetVCPFeature'					DDCCIGetVCPFeature					gdi32		1318
imp	'DDCCISaveCurrentSettings'				DDCCISaveCurrentSettings				gdi32		1319
imp	'DDCCISetVCPFeature'					DDCCISetVCPFeature					gdi32		1320
imp	'DPtoLP'						DPtoLP							gdi32		1321
imp	'DdCreateFullscreenSprite'				DdCreateFullscreenSprite				gdi32		1322
imp	'DdDestroyFullscreenSprite'				DdDestroyFullscreenSprite				gdi32		1323
imp	'DdNotifyFullscreenSpriteUpdate'			DdNotifyFullscreenSpriteUpdate				gdi32		1381
imp	'DdQueryVisRgnUniqueness'				DdQueryVisRgnUniqueness					gdi32		1382
imp	'DeleteColorSpace'					DeleteColorSpace					gdi32		1383
imp	'DeleteDC'						DeleteDC						gdi32		1384	1
imp	'DeleteEnhMetaFile'					DeleteEnhMetaFile					gdi32		1385
imp	'DeleteMetaFile'					DeleteMetaFile						gdi32		1386
imp	'DeleteObject'						DeleteObject						gdi32		1387	1
imp	'DescribePixelFormat'					DescribePixelFormat					gdi32		1388
imp	'DestroyOPMProtectedOutput'				DestroyOPMProtectedOutput				gdi32		1389
imp	'DestroyPhysicalMonitorInternal'			DestroyPhysicalMonitorInternal				gdi32		1390
imp	'DrawEscape'						DrawEscape						gdi32		1393
imp	'DwmCreatedBitmapRemotingOutput'			DwmCreatedBitmapRemotingOutput				gdi32		1014
imp	'DxTrimNotificationListHead'				DxTrimNotificationListHead				gdi32		1394
imp	'Ellipse'						Ellipse							gdi32		1395
imp	'EnableEUDC'						EnableEUDC						gdi32		1396
imp	'EndDoc'						EndDoc							gdi32		1397
imp	'EndFormPage'						EndFormPage						gdi32		1398
imp	'EndGdiRendering'					EndGdiRendering						gdi32		1399
imp	'EndPage'						EndPage							gdi32		1400
imp	'EndPath'						EndPath							gdi32		1401
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
imp	'EnumEnhMetaFile'					EnumEnhMetaFile						gdi32		1448
imp	'EnumFontFamilies'					EnumFontFamiliesW					gdi32		1452
imp	'EnumFontFamiliesEx'					EnumFontFamiliesExW					gdi32		1451
imp	'EnumFonts'						EnumFontsW						gdi32		1454
imp	'EnumICMProfiles'					EnumICMProfilesW					gdi32		1456
imp	'EnumMetaFile'						EnumMetaFile						gdi32		1457
imp	'EnumObjects'						EnumObjects						gdi32		1458
imp	'EqualRgn'						EqualRgn						gdi32		1459
imp	'Escape'						Escape							gdi32		1460
imp	'EudcLoadLink'						EudcLoadLinkW						gdi32		1461
imp	'EudcUnloadLink'					EudcUnloadLinkW						gdi32		1462
imp	'ExcludeClipRect'					ExcludeClipRect						gdi32		1463
imp	'ExtCreatePen'						ExtCreatePen						gdi32		1464
imp	'ExtCreateRegion'					ExtCreateRegion						gdi32		1465
imp	'ExtEscape'						ExtEscape						gdi32		1466
imp	'ExtFloodFill'						ExtFloodFill						gdi32		1467
imp	'ExtSelectClipRgn'					ExtSelectClipRgn					gdi32		1468
imp	'ExtTextOut'						ExtTextOutW						gdi32		1470
imp	'FONTOBJ_cGetAllGlyphHandles'				FONTOBJ_cGetAllGlyphHandles				gdi32		1471
imp	'FONTOBJ_cGetGlyphs'					FONTOBJ_cGetGlyphs					gdi32		1472
imp	'FONTOBJ_pQueryGlyphAttrs'				FONTOBJ_pQueryGlyphAttrs				gdi32		1473
imp	'FONTOBJ_pfdg'						FONTOBJ_pfdg						gdi32		1474
imp	'FONTOBJ_pifi'						FONTOBJ_pifi						gdi32		1475
imp	'FONTOBJ_pvTrueTypeFontFile'				FONTOBJ_pvTrueTypeFontFile				gdi32		1476
imp	'FONTOBJ_pxoGetXform'					FONTOBJ_pxoGetXform					gdi32		1477
imp	'FONTOBJ_vGetInfo'					FONTOBJ_vGetInfo					gdi32		1478
imp	'FillPath'						FillPath						gdi32		1479
imp	'FillRgn'						FillRgn							gdi32		1480
imp	'FixBrushOrgEx'						FixBrushOrgEx						gdi32		1481
imp	'FlattenPath'						FlattenPath						gdi32		1482
imp	'FloodFill'						FloodFill						gdi32		1483
imp	'FontIsLinked'						FontIsLinked						gdi32		1484
imp	'FrameRgn'						FrameRgn						gdi32		1485
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
imp	'GdiDeleteLocalDC'					GdiDeleteLocalDC					gdi32		1511
imp	'GdiDeleteSpoolFileHandle'				GdiDeleteSpoolFileHandle				gdi32		1512
imp	'GdiDescribePixelFormat'				GdiDescribePixelFormat					gdi32		1513
imp	'GdiDllInitialize'					GdiDllInitialize					gdi32		1514
imp	'GdiDrawStream'						GdiDrawStream						gdi32		1515
imp	'GdiEndDocEMF'						GdiEndDocEMF						gdi32		1516
imp	'GdiEndPageEMF'						GdiEndPageEMF						gdi32		1517
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
imp	'GetArcDirection'					GetArcDirection						gdi32		1589
imp	'GetAspectRatioFilterEx'				GetAspectRatioFilterEx					gdi32		1590
imp	'GetBitmapAttributes'					GetBitmapAttributes					gdi32		1591
imp	'GetBitmapBits'						GetBitmapBits						gdi32		1592
imp	'GetBitmapDimensionEx'					GetBitmapDimensionEx					gdi32		1593
imp	'GetBitmapDpiScaleValue'				GetBitmapDpiScaleValue					gdi32		1594
imp	'GetBkColor'						GetBkColor						gdi32		1595
imp	'GetBkMode'						GetBkMode						gdi32		1596
imp	'GetBoundsRect'						GetBoundsRect						gdi32		1597
imp	'GetBrushAttributes'					GetBrushAttributes					gdi32		1598
imp	'GetBrushOrgEx'						GetBrushOrgEx						gdi32		1599
imp	'GetCOPPCompatibleOPMInformation'			GetCOPPCompatibleOPMInformation				gdi32		1600
imp	'GetCertificate'					GetCertificate						gdi32		1601
imp	'GetCertificateByHandle'				GetCertificateByHandle					gdi32		1602
imp	'GetCertificateSize'					GetCertificateSize					gdi32		1603
imp	'GetCertificateSizeByHandle'				GetCertificateSizeByHandle				gdi32		1604
imp	'GetCharABCWidths'					GetCharABCWidthsW					gdi32		1610
imp	'GetCharABCWidthsFloat'					GetCharABCWidthsFloatW					gdi32		1608
imp	'GetCharABCWidthsFloatI'				GetCharABCWidthsFloatI					gdi32		1607
imp	'GetCharABCWidthsI'					GetCharABCWidthsI					gdi32		1609
imp	'GetCharWidth'						GetCharWidthW						gdi32		1618
imp	'GetCharWidth32W'					GetCharWidth32W						gdi32		1612
imp	'GetCharWidthFloat'					GetCharWidthFloatW					gdi32		1615
imp	'GetCharWidthI'						GetCharWidthI						gdi32		1616
imp	'GetCharWidthInfo'					GetCharWidthInfo					gdi32		1617
imp	'GetCharacterPlacement'					GetCharacterPlacementW					gdi32		1620
imp	'GetClipBox'						GetClipBox						gdi32		1621
imp	'GetClipRgn'						GetClipRgn						gdi32		1622
imp	'GetColorAdjustment'					GetColorAdjustment					gdi32		1623
imp	'GetColorSpace'						GetColorSpace						gdi32		1624
imp	'GetCurrentDpiInfo'					GetCurrentDpiInfo					gdi32		1625
imp	'GetCurrentObject'					GetCurrentObject					gdi32		1626
imp	'GetCurrentPositionEx'					GetCurrentPositionEx					gdi32		1627
imp	'GetDCBrushColor'					GetDCBrushColor						gdi32		1628
imp	'GetDCDpiScaleValue'					GetDCDpiScaleValue					gdi32		1629
imp	'GetDCOrgEx'						GetDCOrgEx						gdi32		1630
imp	'GetDCPenColor'						GetDCPenColor						gdi32		1631
imp	'GetDIBColorTable'					GetDIBColorTable					gdi32		1632
imp	'GetDIBits'						GetDIBits						gdi32		1633
imp	'GetDeviceCaps'						GetDeviceCaps						gdi32		1634
imp	'GetDeviceGammaRamp'					GetDeviceGammaRamp					gdi32		1635
imp	'GetETM'						GetETM							gdi32		1636
imp	'GetEUDCTimeStamp'					GetEUDCTimeStamp					gdi32		1637
imp	'GetEUDCTimeStampEx'					GetEUDCTimeStampExW					gdi32		1638
imp	'GetEnhMetaFile'					GetEnhMetaFileW						gdi32		1646
imp	'GetEnhMetaFileBits'					GetEnhMetaFileBits					gdi32		1640
imp	'GetEnhMetaFileDescription'				GetEnhMetaFileDescriptionW				gdi32		1642
imp	'GetEnhMetaFileHeader'					GetEnhMetaFileHeader					gdi32		1643
imp	'GetEnhMetaFilePaletteEntries'				GetEnhMetaFilePaletteEntries				gdi32		1644
imp	'GetEnhMetaFilePixelFormat'				GetEnhMetaFilePixelFormat				gdi32		1645
imp	'GetFontAssocStatus'					GetFontAssocStatus					gdi32		1647
imp	'GetFontData'						GetFontData						gdi32		1648
imp	'GetFontFileData'					GetFontFileData						gdi32		1649
imp	'GetFontFileInfo'					GetFontFileInfo						gdi32		1650
imp	'GetFontLanguageInfo'					GetFontLanguageInfo					gdi32		1651
imp	'GetFontRealizationInfo'				GetFontRealizationInfo					gdi32		1652
imp	'GetFontResourceInfo'					GetFontResourceInfoW					gdi32		1653
imp	'GetFontUnicodeRanges'					GetFontUnicodeRanges					gdi32		1654
imp	'GetGlyphIndices'					GetGlyphIndicesW					gdi32		1656
imp	'GetGlyphOutline'					GetGlyphOutlineW					gdi32		1659
imp	'GetGlyphOutlineWow'					GetGlyphOutlineWow					gdi32		1660
imp	'GetGraphicsMode'					GetGraphicsMode						gdi32		1661
imp	'GetHFONT'						GetHFONT						gdi32		1662
imp	'GetICMProfile'						GetICMProfileW						gdi32		1664
imp	'GetKerningPairs'					GetKerningPairsW					gdi32		1667
imp	'GetLayout'						GetLayout						gdi32		1668
imp	'GetLogColorSpace'					GetLogColorSpaceW					gdi32		1670
imp	'GetMapMode'						GetMapMode						gdi32		1671
imp	'GetMetaFile'						GetMetaFileW						gdi32		1674
imp	'GetMetaFileBitsEx'					GetMetaFileBitsEx					gdi32		1673
imp	'GetMetaRgn'						GetMetaRgn						gdi32		1675
imp	'GetMiterLimit'						GetMiterLimit						gdi32		1676
imp	'GetNearestColor'					GetNearestColor						gdi32		1677
imp	'GetNearestPaletteIndex'				GetNearestPaletteIndex					gdi32		1678
imp	'GetNumberOfPhysicalMonitors'				GetNumberOfPhysicalMonitors				gdi32		1679
imp	'GetOPMInformation'					GetOPMInformation					gdi32		1680
imp	'GetOPMRandomNumber'					GetOPMRandomNumber					gdi32		1681
imp	'GetObject'						GetObjectW						gdi32		1684
imp	'GetObjectType'						GetObjectType						gdi32		1683
imp	'GetOutlineTextMetrics'					GetOutlineTextMetricsW					gdi32		1686
imp	'GetPaletteEntries'					GetPaletteEntries					gdi32		1687
imp	'GetPath'						GetPath							gdi32		1688
imp	'GetPhysicalMonitorDescription'				GetPhysicalMonitorDescription				gdi32		1689
imp	'GetPhysicalMonitors'					GetPhysicalMonitors					gdi32		1690
imp	'GetPixel'						GetPixel						gdi32		1691	3
imp	'GetPixelFormat'					GetPixelFormat						gdi32		1692
imp	'GetPolyFillMode'					GetPolyFillMode						gdi32		1693
imp	'GetProcessSessionFonts'				GetProcessSessionFonts					gdi32		1694
imp	'GetROP2'						GetROP2							gdi32		1695
imp	'GetRandomRgn'						GetRandomRgn						gdi32		1696
imp	'GetRasterizerCaps'					GetRasterizerCaps					gdi32		1697
imp	'GetRegionData'						GetRegionData						gdi32		1698
imp	'GetRelAbs'						GetRelAbs						gdi32		1699
imp	'GetRgnBox'						GetRgnBox						gdi32		1700
imp	'GetStockObject'					GetStockObject						gdi32		1701
imp	'GetStretchBltMode'					GetStretchBltMode					gdi32		1702
imp	'GetStringBitmap'					GetStringBitmapW					gdi32		1704
imp	'GetSuggestedOPMProtectedOutputArraySize'		GetSuggestedOPMProtectedOutputArraySize			gdi32		1705
imp	'GetSystemPaletteEntries'				GetSystemPaletteEntries					gdi32		1706
imp	'GetSystemPaletteUse'					GetSystemPaletteUse					gdi32		1707
imp	'GetTextAlign'						GetTextAlign						gdi32		1708
imp	'GetTextCharacterExtra'					GetTextCharacterExtra					gdi32		1709
imp	'GetTextCharset'					GetTextCharset						gdi32		1710
imp	'GetTextCharsetInfo'					GetTextCharsetInfo					gdi32		1711
imp	'GetTextColor'						GetTextColor						gdi32		1712
imp	'GetTextExtentExPoint'					GetTextExtentExPointW					gdi32		1715
imp	'GetTextExtentExPointI'					GetTextExtentExPointI					gdi32		1714
imp	'GetTextExtentExPointWPri'				GetTextExtentExPointWPri				gdi32		1716
imp	'GetTextExtentPoint'					GetTextExtentPointW					gdi32		1721
imp	'GetTextExtentPoint32W'					GetTextExtentPoint32W					gdi32		1718
imp	'GetTextExtentPointI'					GetTextExtentPointI					gdi32		1720
imp	'GetTextFace'						GetTextFaceW						gdi32		1724
imp	'GetTextFaceAlias'					GetTextFaceAliasW					gdi32		1723
imp	'GetTextMetrics'					GetTextMetricsW						gdi32		1726
imp	'GetTransform'						GetTransform						gdi32		1727
imp	'GetViewportExtEx'					GetViewportExtEx					gdi32		1728
imp	'GetViewportOrgEx'					GetViewportOrgEx					gdi32		1729
imp	'GetWinMetaFileBits'					GetWinMetaFileBits					gdi32		1730
imp	'GetWindowExtEx'					GetWindowExtEx						gdi32		1731
imp	'GetWindowOrgEx'					GetWindowOrgEx						gdi32		1732
imp	'GetWorldTransform'					GetWorldTransform					gdi32		1733
imp	'HT_Get8BPPFormatPalette'				HT_Get8BPPFormatPalette					gdi32		1734
imp	'HT_Get8BPPMaskPalette'					HT_Get8BPPMaskPalette					gdi32		1735
imp	'InternalDeleteDC'					InternalDeleteDC					gdi32		1736
imp	'IntersectClipRect'					IntersectClipRect					gdi32		1737
imp	'InvertRgn'						InvertRgn						gdi32		1738
imp	'IsValidEnhMetaRecord'					IsValidEnhMetaRecord					gdi32		1739
imp	'IsValidEnhMetaRecordOffExt'				IsValidEnhMetaRecordOffExt				gdi32		1740
imp	'LPtoDP'						LPtoDP							gdi32		1741
imp	'LineTo'						LineTo							gdi32		1743
imp	'LpkEditControl'					LpkEditControl						gdi32		1745
imp	'LpkGetEditControl'					LpkGetEditControl					gdi32		1748
imp	'LpkpEditControlSize'					LpkpEditControlSize					gdi32		1755
imp	'LpkpInitializeEditControl'				LpkpInitializeEditControl				gdi32		1756
imp	'MaskBlt'						MaskBlt							gdi32		1757
imp	'MirrorRgn'						MirrorRgn						gdi32		1758
imp	'ModerncoreGdiInit'					ModerncoreGdiInit					gdi32		1759
imp	'ModifyWorldTransform'					ModifyWorldTransform					gdi32		1760
imp	'MoveToEx'						MoveToEx						gdi32		1761
imp	'NamedEscape'						NamedEscape						gdi32		1762
imp	'OffsetClipRgn'						OffsetClipRgn						gdi32		1763
imp	'OffsetRgn'						OffsetRgn						gdi32		1764
imp	'OffsetViewportOrgEx'					OffsetViewportOrgEx					gdi32		1765
imp	'OffsetWindowOrgEx'					OffsetWindowOrgEx					gdi32		1766
imp	'PATHOBJ_bEnum'						PATHOBJ_bEnum						gdi32		1767
imp	'PATHOBJ_bEnumClipLines'				PATHOBJ_bEnumClipLines					gdi32		1768
imp	'PATHOBJ_vEnumStart'					PATHOBJ_vEnumStart					gdi32		1769
imp	'PATHOBJ_vEnumStartClipLines'				PATHOBJ_vEnumStartClipLines				gdi32		1770
imp	'PATHOBJ_vGetBounds'					PATHOBJ_vGetBounds					gdi32		1771
imp	'PaintRgn'						PaintRgn						gdi32		1772
imp	'PatBlt'						PatBlt							gdi32		1773
imp	'PathToRegion'						PathToRegion						gdi32		1774
imp	'Pie'							Pie							gdi32		1775
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
imp	'PolyTextOut'						PolyTextOutW						gdi32		1788
imp	'Polygon'						Polygon							gdi32		1789
imp	'Polyline'						Polyline						gdi32		1790
imp	'PolylineTo'						PolylineTo						gdi32		1791
imp	'PtInRegion'						PtInRegion						gdi32		1792
imp	'PtVisible'						PtVisible						gdi32		1793
imp	'QueryFontAssocStatus'					QueryFontAssocStatus					gdi32		1794
imp	'RealizePalette'					RealizePalette						gdi32		1795
imp	'RectInRegion'						RectInRegion						gdi32		1796
imp	'RectVisible'						RectVisible						gdi32		1797
imp	'Rectangle'						Rectangle						gdi32		1798
imp	'RemoveFontMemResourceEx'				RemoveFontMemResourceEx					gdi32		1799
imp	'RemoveFontResource'					RemoveFontResourceW					gdi32		1804
imp	'RemoveFontResourceEx'					RemoveFontResourceExW					gdi32		1802
imp	'RemoveFontResourceTracking'				RemoveFontResourceTracking				gdi32		1803
imp	'ResetDCW'						ResetDCW						gdi32		1806
imp	'ResizePalette'						ResizePalette						gdi32		1807
imp	'RestoreDC'						RestoreDC						gdi32		1808	2
imp	'RoundRect'						RoundRect						gdi32		1809
imp	'STROBJ_bEnum'						STROBJ_bEnum						gdi32		1810
imp	'STROBJ_bEnumPositionsOnly'				STROBJ_bEnumPositionsOnly				gdi32		1811
imp	'STROBJ_bGetAdvanceWidths'				STROBJ_bGetAdvanceWidths				gdi32		1812
imp	'STROBJ_dwGetCodePage'					STROBJ_dwGetCodePage					gdi32		1813
imp	'STROBJ_vEnumStart'					STROBJ_vEnumStart					gdi32		1814
imp	'SaveDC'						SaveDC							gdi32		1815	1
imp	'ScaleRgn'						ScaleRgn						gdi32		1816
imp	'ScaleValues'						ScaleValues						gdi32		1817
imp	'ScaleViewportExtEx'					ScaleViewportExtEx					gdi32		1818
imp	'ScaleWindowExtEx'					ScaleWindowExtEx					gdi32		1819
imp	'SelectBrushLocal'					SelectBrushLocal					gdi32		1860
imp	'SelectClipPath'					SelectClipPath						gdi32		1861
imp	'SelectClipRgn'						SelectClipRgn						gdi32		1862
imp	'SelectFontLocal'					SelectFontLocal						gdi32		1863
imp	'SelectObject'						SelectObject						gdi32		1864	2
imp	'SelectPalette'						SelectPalette						gdi32		1865
imp	'SetAbortProc'						SetAbortProc						gdi32		1866
imp	'SetArcDirection'					SetArcDirection						gdi32		1867
imp	'SetBitmapAttributes'					SetBitmapAttributes					gdi32		1868
imp	'SetBitmapBits'						SetBitmapBits						gdi32		1869
imp	'SetBitmapDimensionEx'					SetBitmapDimensionEx					gdi32		1870
imp	'SetBkColor'						SetBkColor						gdi32		1871
imp	'SetBkMode'						SetBkMode						gdi32		1872	2
imp	'SetBoundsRect'						SetBoundsRect						gdi32		1873
imp	'SetBrushAttributes'					SetBrushAttributes					gdi32		1874
imp	'SetBrushOrgEx'						SetBrushOrgEx						gdi32		1875
imp	'SetColorAdjustment'					SetColorAdjustment					gdi32		1876
imp	'SetColorSpace'						SetColorSpace						gdi32		1877
imp	'SetDCBrushColor'					SetDCBrushColor						gdi32		1878
imp	'SetDCDpiScaleValue'					SetDCDpiScaleValue					gdi32		1879
imp	'SetDCPenColor'						SetDCPenColor						gdi32		1880
imp	'SetDIBColorTable'					SetDIBColorTable					gdi32		1881
imp	'SetDIBits'						SetDIBits						gdi32		1882
imp	'SetDIBitsToDevice'					SetDIBitsToDevice					gdi32		1883
imp	'SetDeviceGammaRamp'					SetDeviceGammaRamp					gdi32		1884
imp	'SetEnhMetaFileBits'					SetEnhMetaFileBits					gdi32		1885
imp	'SetFontEnumeration'					SetFontEnumeration					gdi32		1886
imp	'SetGraphicsMode'					SetGraphicsMode						gdi32		1887
imp	'SetICMMode'						SetICMMode						gdi32		1888
imp	'SetICMProfile'						SetICMProfileW						gdi32		1890
imp	'SetLayout'						SetLayout						gdi32		1891
imp	'SetLayoutWidth'					SetLayoutWidth						gdi32		1892
imp	'SetMagicColors'					SetMagicColors						gdi32		1893
imp	'SetMapMode'						SetMapMode						gdi32		1894
imp	'SetMapperFlags'					SetMapperFlags						gdi32		1895
imp	'SetMetaFileBitsEx'					SetMetaFileBitsEx					gdi32		1896
imp	'SetMetaRgn'						SetMetaRgn						gdi32		1897
imp	'SetMiterLimit'						SetMiterLimit						gdi32		1898
imp	'SetOPMSigningKeyAndSequenceNumbers'			SetOPMSigningKeyAndSequenceNumbers			gdi32		1899
imp	'SetPaletteEntries'					SetPaletteEntries					gdi32		1900
imp	'SetPixel'						SetPixel						gdi32		1901	4
imp	'SetPixelFormat'					SetPixelFormat						gdi32		1902	3
imp	'SetPixelV'						SetPixelV						gdi32		1903
imp	'SetPolyFillMode'					SetPolyFillMode						gdi32		1904
imp	'SetROP2'						SetROP2							gdi32		1905
imp	'SetRectRgn'						SetRectRgn						gdi32		1906
imp	'SetRelAbs'						SetRelAbs						gdi32		1907
imp	'SetStretchBltMode'					SetStretchBltMode					gdi32		1908
imp	'SetSystemPaletteUse'					SetSystemPaletteUse					gdi32		1909
imp	'SetTextAlign'						SetTextAlign						gdi32		1910	2
imp	'SetTextCharacterExtra'					SetTextCharacterExtra					gdi32		1911
imp	'SetTextColor'						SetTextColor						gdi32		1912	2
imp	'SetTextJustification'					SetTextJustification					gdi32		1913	3
imp	'SetViewportExtEx'					SetViewportExtEx					gdi32		1914
imp	'SetViewportOrgEx'					SetViewportOrgEx					gdi32		1915
imp	'SetVirtualResolution'					SetVirtualResolution					gdi32		1916
imp	'SetWinMetaFileBits'					SetWinMetaFileBits					gdi32		1917
imp	'SetWindowExtEx'					SetWindowExtEx						gdi32		1918
imp	'SetWindowOrgEx'					SetWindowOrgEx						gdi32		1919
imp	'SetWorldTransform'					SetWorldTransform					gdi32		1920
imp	'StartDoc'						StartDocW						gdi32		1922
imp	'StartFormPage'						StartFormPage						gdi32		1923
imp	'StartPage'						StartPage						gdi32		1924
imp	'StretchBlt'						StretchBlt						gdi32		1925
imp	'StretchDIBits'						StretchDIBits						gdi32		1926
imp	'StrokeAndFillPath'					StrokeAndFillPath					gdi32		1927
imp	'StrokePath'						StrokePath						gdi32		1928
imp	'SwapBuffers'						SwapBuffers						gdi32		1929	1
imp	'TextOut'						TextOutW						gdi32		1931
imp	'TranslateCharsetInfo'					TranslateCharsetInfo					gdi32		1932
imp	'UnloadNetworkFonts'					UnloadNetworkFonts					gdi32		1933
imp	'UnrealizeObject'					UnrealizeObject						gdi32		1934
imp	'UpdateColors'						UpdateColors						gdi32		1935
imp	'UpdateICMRegKey'					UpdateICMRegKeyW					gdi32		1937
imp	'WidenPath'						WidenPath						gdi32		1941
imp	'XFORMOBJ_bApplyXform'					XFORMOBJ_bApplyXform					gdi32		1942
imp	'XFORMOBJ_iGetXform'					XFORMOBJ_iGetXform					gdi32		1943
imp	'XLATEOBJ_cGetPalette'					XLATEOBJ_cGetPalette					gdi32		1944
imp	'XLATEOBJ_hGetColorTransform'				XLATEOBJ_hGetColorTransform				gdi32		1945
imp	'XLATEOBJ_iXlate'					XLATEOBJ_iXlate						gdi32		1946
imp	'XLATEOBJ_piVector'					XLATEOBJ_piVector					gdi32		1947
imp	'cGetTTFFromFOT'					cGetTTFFromFOT						gdi32		1952
imp	'fpClosePrinter'					fpClosePrinter						gdi32		1953
imp	'gMaxGdiHandleCount'					gMaxGdiHandleCount					gdi32		1955
imp	'gW32PID'						gW32PID							gdi32		1956
imp	'g_systemCallFilterId'					g_systemCallFilterId					gdi32		1957
imp	'gdiPlaySpoolStream'					gdiPlaySpoolStream					gdi32		1958

# COMDLG32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'ChooseColor'						ChooseColorW						comdlg32	103	1
imp	'ChooseFont'						ChooseFontW						comdlg32	105	1
imp	'CommDlgExtendedError'					CommDlgExtendedError					comdlg32	106
imp	'DllCanUnloadNow'					DllCanUnloadNow						comdlg32	107
imp	'DllGetClassObject'					DllGetClassObject					comdlg32	108
imp	'FindText'						FindTextW						comdlg32	110
imp	'GetFileTitle'						GetFileTitleW						comdlg32	112	3
imp	'GetOpenFileName'					GetOpenFileNameW					comdlg32	114	1
imp	'GetSaveFileName'					GetSaveFileNameW					comdlg32	116	1
imp	'LoadAlterBitmap'					LoadAlterBitmap						comdlg32	117
imp	'PageSetupDlg'						PageSetupDlgW						comdlg32	119
imp	'PrintDlg'						PrintDlgW						comdlg32	123	1
imp	'PrintDlgEx'						PrintDlgExW						comdlg32	122
imp	'ReplaceText'						ReplaceTextW						comdlg32	125	1
imp	'Ssync_ANSI_UNICODE_Struct_For_WOW'			Ssync_ANSI_UNICODE_Struct_For_WOW			comdlg32	126
imp	'WantArrows'						WantArrows						comdlg32	127

# MSWSOCK.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'AcceptEx'						AcceptEx						MsWSock		0	8
imp	'DisconnectEx'						DisconnectEx						MsWSock		0	4
imp	'GetAcceptExSockaddrs'					GetAcceptExSockaddrs					MsWSock		0	8
imp	'TransmitFile'						TransmitFile						MsWSock		0	7
imp	'WSARecvEx'						WSARecvEx						MsWSock		0	4

# WS2_32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'FreeAddrInfo'						FreeAddrInfoW						ws2_32		27	1
imp	'FreeAddrInfoEx'					FreeAddrInfoExW						ws2_32		26	1
imp	'GetAddrInfo'						GetAddrInfoW						ws2_32		32	4
imp	'GetAddrInfoEx'						GetAddrInfoExW						ws2_32		31	10
imp	'GetAddrInfoExCancel'					GetAddrInfoExCancel					ws2_32		29	1
imp	'GetAddrInfoExOverlappedResult'				GetAddrInfoExOverlappedResult				ws2_32		30	1
imp	'GetHostName'						GetHostNameW						ws2_32		33	2
imp	'GetNameInfo'						GetNameInfoW						ws2_32		34	7
imp	'SetAddrInfoEx'						SetAddrInfoExW						ws2_32		38	12
imp	'WSAAccept'						WSAAccept						ws2_32		41	5
imp	'WSAAddressToString'					WSAAddressToStringW					ws2_32		43	5
imp	'WSAAsyncGetHostByAddr'					WSAAsyncGetHostByAddr					ws2_32		102	7
imp	'WSAAsyncGetHostByName'					WSAAsyncGetHostByName					ws2_32		103	5
imp	'WSAAsyncGetProtoByName'				WSAAsyncGetProtoByName					ws2_32		105	5
imp	'WSAAsyncGetProtoByNumber'				WSAAsyncGetProtoByNumber				ws2_32		104	5
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
imp	'WSACreateEvent'					WSACreateEvent						ws2_32		50	0
imp	'WSADuplicateSocket'					WSADuplicateSocketW					ws2_32		59	3
imp	'WSAEnumNameSpaceProviders'				WSAEnumNameSpaceProvidersW				ws2_32		63	2
imp	'WSAEnumNameSpaceProvidersEx'				WSAEnumNameSpaceProvidersExW				ws2_32		62	2
imp	'WSAEnumNetworkEvents'					WSAEnumNetworkEvents					ws2_32		64	3
imp	'WSAEnumProtocols'					WSAEnumProtocolsW					ws2_32		66	3
imp	'WSAEventSelect'					WSAEventSelect						ws2_32		67	3
imp	'WSAGetLastError'					WSAGetLastError						ws2_32		111	0
imp	'WSAGetOverlappedResult'				WSAGetOverlappedResult					ws2_32		68	5
imp	'WSAGetQOSByName'					WSAGetQOSByName						ws2_32		69	3
imp	'WSAGetServiceClassInfo'				WSAGetServiceClassInfoW					ws2_32		71	4
imp	'WSAGetServiceClassNameByClassId'			WSAGetServiceClassNameByClassIdW			ws2_32		73	3
imp	'WSAInstallServiceClass'				WSAInstallServiceClassW					ws2_32		77	1
imp	'WSAIoctl'						WSAIoctl						ws2_32		78	9
imp	'WSAJoinLeaf'						WSAJoinLeaf						ws2_32		79	8
imp	'WSALookupServiceBegin'					WSALookupServiceBeginW					ws2_32		81	3
imp	'WSALookupServiceEnd'					WSALookupServiceEnd					ws2_32		82	1
imp	'WSALookupServiceNext'					WSALookupServiceNextW					ws2_32		84	4
imp	'WSANSPIoctl'						WSANSPIoctl						ws2_32		85	8
imp	'WSAPoll'						WSAPoll							ws2_32		88	3
imp	'WSAProviderCompleteAsyncCall'				WSAProviderCompleteAsyncCall				ws2_32		89
imp	'WSAProviderConfigChange'				WSAProviderConfigChange					ws2_32		90	3
imp	'WSARecvDisconnect'					WSARecvDisconnect					ws2_32		92	2
imp	'WSARemoveServiceClass'					WSARemoveServiceClass					ws2_32		94	1
imp	'WSAResetEvent'						WSAResetEvent						ws2_32		95	1
imp	'WSASend'						WSASend							ws2_32		96	7
imp	'WSASendDisconnect'					WSASendDisconnect					ws2_32		97	2
imp	'WSASendMsg'						WSASendMsg						ws2_32		98	6
imp	'WSASendTo'						WSASendTo						ws2_32		99	9
imp	'WSASetBlockingHook'					WSASetBlockingHook					ws2_32		109
imp	'WSASetEvent'						WSASetEvent						ws2_32		100	1
imp	'WSASetLastError'					WSASetLastError						ws2_32		112	1
imp	'WSASetService'						WSASetServiceW						ws2_32		118	3
imp	'WSASocket'						WSASocketW						ws2_32		120	6
imp	'WSAStartup'						WSAStartup						ws2_32		115	2
imp	'WSAStringToAddress'					WSAStringToAddressW					ws2_32		122
imp	'WSAUnadvertiseProvider'				WSAUnadvertiseProvider					ws2_32		123
imp	'WSAUnhookBlockingHook'					WSAUnhookBlockingHook					ws2_32		110
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
imp	'__sys_accept_nt'					accept							ws2_32		1	3	# we're using WSAAccept()
imp	'__sys_bind_nt'						bind							ws2_32		2	3
imp	'__sys_closesocket_nt'					closesocket						ws2_32		3	1
imp	'__sys_connect_nt'					connect							ws2_32		4
imp	'__sys_getpeername_nt'					getpeername						ws2_32		5	3
imp	'__sys_getsockname_nt'					getsockname						ws2_32		6	3
imp	'__sys_getsockopt_nt'					getsockopt						ws2_32		7	5
imp	'__sys_ioctlsocket_nt'					ioctlsocket						ws2_32		10	3
imp	'__sys_listen_nt'					listen							ws2_32		13	2
imp	'__sys_recvfrom_nt'					recvfrom						ws2_32		17	6	# we're using WSARecvFrom()
imp	'__sys_select_nt'					select							ws2_32		18	5
imp	'__sys_sendto_nt'					sendto							ws2_32		20	6	# we're using WSASendTo()
imp	'__sys_setsockopt_nt'					setsockopt						ws2_32		21	5
imp	'__sys_shutdown_nt'					shutdown						ws2_32		22	2
imp	'__sys_socket_nt'					socket							ws2_32		23	3
imp	'sys_freeaddrinfo_nt'					freeaddrinfo						ws2_32		190
imp	'sys_getaddrinfo_nt'					getaddrinfo						ws2_32		191
imp	'sys_gethostbyaddr_nt'					gethostbyaddr						ws2_32		51
imp	'sys_gethostbyname_nt'					gethostbyname						ws2_32		52
imp	'sys_gethostname_nt'					gethostname						ws2_32		57
imp	'sys_getnameinfo_nt'					getnameinfo						ws2_32		192
imp	'sys_getprotobyname_nt'					getprotobyname						ws2_32		53
imp	'sys_getprotobynumber_nt'				getprotobynumber					ws2_32		54
imp	'sys_getservbyname_nt'					getservbyname						ws2_32		55
imp	'sys_getservbyport_nt'					getservbyport						ws2_32		56
imp	'__WSARecv'						WSARecv							ws2_32		91	7
imp	'__WSARecvFrom'						WSARecvFrom						ws2_32		93	9
imp	'__WSAWaitForMultipleEvents'				WSAWaitForMultipleEvents				ws2_32		124	5
imp	'__sys_recv_nt'						recv							ws2_32		16	4	# we're using WSARecvFrom()
imp	'__sys_send_nt'						send							ws2_32		19	4	# we're using WSASendTo()

# IPHLPAPI.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'AddIPAddress'						AddIPAddress						iphlpapi	0	5
imp	'AllocateAndGetTcpExTableFromStack'			AllocateAndGetTcpExTableFromStack			iphlpapi	0	5
imp	'AllocateAndGetUdpExTableFromStack'			AllocateAndGetUdpExTableFromStack			iphlpapi	0	5
imp	'CancelIPChangeNotify'					CancelIPChangeNotify					iphlpapi	0	1
imp	'CaptureInterfaceHardwareCrossTimestamp'		CaptureInterfaceHardwareCrossTimestamp			iphlpapi	0	2
imp	'CreateIpForwardEntry'					CreateIpForwardEntry					iphlpapi	0	1
imp	'CreateIpNetEntry'					CreateIpNetEntry					iphlpapi	0
imp	'CreatePersistentTcpPortReservation'			CreatePersistentTcpPortReservation			iphlpapi	0
imp	'CreatePersistentUdpPortReservation'			CreatePersistentUdpPortReservation			iphlpapi	0
imp	'CreateProxyArpEntry'					CreateProxyArpEntry					iphlpapi	0
imp	'DeleteIPAddress'					DeleteIPAddress						iphlpapi	0
imp	'DeleteIpForwardEntry'					DeleteIpForwardEntry					iphlpapi	0
imp	'DeleteIpNetEntry'					DeleteIpNetEntry					iphlpapi	0
imp	'DeletePersistentTcpPortReservation'			DeletePersistentTcpPortReservation			iphlpapi	0
imp	'DeletePersistentUdpPortReservation'			DeletePersistentUdpPortReservation			iphlpapi	0
imp	'DeleteProxyArpEntry'					DeleteProxyArpEntry					iphlpapi	0
imp	'DisableMediaSense'					DisableMediaSense					iphlpapi	0
imp	'EnableRouter'						EnableRouter						iphlpapi	0
imp	'FlushIpNetTable'					FlushIpNetTable						iphlpapi	0	1
imp	'GetAdapterIndex'					GetAdapterIndex						iphlpapi	0	2
imp	'GetAdapterOrderMap'					GetAdapterOrderMap					iphlpapi	0	0
imp     'GetAdaptersAddresses'                                  GetAdaptersAddresses                                    iphlpapi        67      5
imp	'GetAdaptersInfo'					GetAdaptersInfo						iphlpapi	0	2
imp	'GetBestInterface'					GetBestInterface					iphlpapi	0	2
imp	'GetBestInterfaceEx'					GetBestInterfaceEx					iphlpapi	0	2
imp	'GetBestRoute'						GetBestRoute						iphlpapi	0	3
imp	'GetExtendedTcpTable'					GetExtendedTcpTable					iphlpapi	0
imp	'GetExtendedUdpTable'					GetExtendedUdpTable					iphlpapi	0
imp	'GetFriendlyIfIndex'					GetFriendlyIfIndex					iphlpapi	0
imp	'GetIcmpStatistics'					GetIcmpStatistics					iphlpapi	0
imp	'GetIcmpStatisticsEx'					GetIcmpStatisticsEx					iphlpapi	0
imp	'GetIfEntry'						GetIfEntry						iphlpapi	0
imp	'GetIfTable'						GetIfTable						iphlpapi	0
imp	'GetInterfaceActiveTimestampCapabilities'		GetInterfaceActiveTimestampCapabilities			iphlpapi	0
imp	'GetInterfaceInfo'					GetInterfaceInfo					iphlpapi	0
imp	'GetInterfaceSupportedTimestampCapabilities'		GetInterfaceSupportedTimestampCapabilities		iphlpapi	0
imp	'GetIpAddrTable'					GetIpAddrTable						iphlpapi	0
imp	'GetIpErrorString'					GetIpErrorString					iphlpapi	0
imp	'GetIpForwardTable'					GetIpForwardTable					iphlpapi	0
imp	'GetIpNetTable'						GetIpNetTable						iphlpapi	0
imp	'GetIpStatistics'					GetIpStatistics						iphlpapi	0
imp	'GetIpStatisticsEx'					GetIpStatisticsEx					iphlpapi	0
imp	'GetNetworkParams'					GetNetworkParams					iphlpapi	0
imp	'GetNumberOfInterfaces'					GetNumberOfInterfaces					iphlpapi	0	1
imp	'GetOwnerModuleFromTcp6Entry'				GetOwnerModuleFromTcp6Entry				iphlpapi	0
imp	'GetOwnerModuleFromTcpEntry'				GetOwnerModuleFromTcpEntry				iphlpapi	0
imp	'GetOwnerModuleFromUdp6Entry'				GetOwnerModuleFromUdp6Entry				iphlpapi	0
imp	'GetOwnerModuleFromUdpEntry'				GetOwnerModuleFromUdpEntry				iphlpapi	0
imp	'GetPerAdapterInfo'					GetPerAdapterInfo					iphlpapi	0
imp	'GetPerTcp6ConnectionEStats'				GetPerTcp6ConnectionEStats				iphlpapi	0
imp	'GetPerTcpConnectionEStats'				GetPerTcpConnectionEStats				iphlpapi	0
imp	'GetRTTAndHopCount'					GetRTTAndHopCount					iphlpapi	0
imp	'GetTcp6Table'						GetTcp6Table						iphlpapi	0
imp	'GetTcp6Table2'						GetTcp6Table2						iphlpapi	0
imp	'GetTcpStatistics'					GetTcpStatistics					iphlpapi	0
imp	'GetTcpStatisticsEx'					GetTcpStatisticsEx					iphlpapi	0
imp	'GetTcpStatisticsEx2'					GetTcpStatisticsEx2					iphlpapi	0
imp	'GetTcpTable'						GetTcpTable						iphlpapi	0	3
imp	'GetTcpTable2'						GetTcpTable2						iphlpapi	0	3
imp	'GetUdp6Table'						GetUdp6Table						iphlpapi	0
imp	'GetUdpStatistics'					GetUdpStatistics					iphlpapi	0
imp	'GetUdpStatisticsEx'					GetUdpStatisticsEx					iphlpapi	0
imp	'GetUdpStatisticsEx2'					GetUdpStatisticsEx2					iphlpapi	0
imp	'GetUdpTable'						GetUdpTable						iphlpapi	0
imp	'GetUniDirectionalAdapterInfo'				GetUniDirectionalAdapterInfo				iphlpapi	0
imp	'IpReleaseAddress'					IpReleaseAddress					iphlpapi	0
imp	'IpRenewAddress'					IpRenewAddress						iphlpapi	0
imp	'LookupPersistentTcpPortReservation'			LookupPersistentTcpPortReservation			iphlpapi	0
imp	'LookupPersistentUdpPortReservation'			LookupPersistentUdpPortReservation			iphlpapi	0
imp	'NhpAllocateAndGetInterfaceInfoFromStack'		NhpAllocateAndGetInterfaceInfoFromStack			iphlpapi	0
imp	'NotifyAddrChange'					NotifyAddrChange					iphlpapi	0
imp	'NotifyRouteChange'					NotifyRouteChange					iphlpapi	0
imp	'ParseNetworkString'					ParseNetworkString					iphlpapi	0
imp	'RegisterInterfaceTimestampConfigChange'		RegisterInterfaceTimestampConfigChange			iphlpapi	0
imp	'ResolveNeighbor'					ResolveNeighbor						iphlpapi	0
imp	'RestoreMediaSense'					RestoreMediaSense					iphlpapi	0
imp	'SendARP'						SendARP							iphlpapi	0
imp	'SetIfEntry'						SetIfEntry						iphlpapi	0
imp	'SetIpForwardEntry'					SetIpForwardEntry					iphlpapi	0
imp	'SetIpNetEntry'						SetIpNetEntry						iphlpapi	0
imp	'SetIpStatistics'					SetIpStatistics						iphlpapi	0
imp	'SetIpStatisticsEx'					SetIpStatisticsEx					iphlpapi	0
imp	'SetIpTTL'						SetIpTTL						iphlpapi	0
imp	'SetPerTcp6ConnectionEStats'				SetPerTcp6ConnectionEStats				iphlpapi	0
imp	'SetPerTcpConnectionEStats'				SetPerTcpConnectionEStats				iphlpapi	0
imp	'SetTcpEntry'						SetTcpEntry						iphlpapi	0
imp	'UnenableRouter'					UnenableRouter						iphlpapi	0
imp	'UnregisterInterfaceTimestampConfigChange'		UnregisterInterfaceTimestampConfigChange		iphlpapi	0

# POWRPROF.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'SetSuspendState'					SetSuspendState						PowrProf	0	3

# PDH.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'CounterPathCallBack'					CounterPathCallBack					pdh		0		# Applications implement the CounterPathCallBack function to process the counter path strings returned by the Browse dialog box.
imp	'LoadPerfCounterTextStrings'				LoadPerfCounterTextStringsW				pdh		0		# Loads onto the computer the performance objects and counters defined in the specified initialization file.
imp	'PdhAddCounter'						PdhAddCounterW						pdh		0		# Adds the specified counter to the query.
imp	'PdhAddEnglishCounter'					PdhAddEnglishCounterW					pdh		0	4	# Adds the specified language-neutral counter to the query.
imp	'PdhBindInputDataSource'				PdhBindInputDataSourceW					pdh		0		# Binds one or more binary log files together for reading log data.
imp	'PdhBrowseCounters'					PdhBrowseCountersW					pdh		0		# Displays a Browse Counters dialog box that the user can use to select one or more counters that they want to add to the query. To use handles to data sources, use the PdhBrowseCountersH function.
imp	'PdhBrowseCountersH'					PdhBrowseCountersHW					pdh		0		# Displays a Browse Counters dialog box that the user can use to select one or more counters that they want to add to the query. This function is identical to the PdhBrowseCounters function, except that it supports the use of handles to data sources.
imp	'PdhCalculateCounterFromRawValue'			PdhCalculateCounterFromRawValue				pdh		0		# Calculates the displayable value of two raw counter values.
imp	'PdhCloseLog'						PdhCloseLog						pdh		0		# Closes the specified log file.
imp	'PdhCloseQuery'						PdhCloseQuery						pdh		0		# Closes all counters contained in the specified query, closes all handles related to the query, and frees all memory associated with the query.
imp	'PdhCollectQueryData'					PdhCollectQueryData					pdh		0		# Collects the current raw data value for all counters in the specified query and updates the status code of each counter.
imp	'PdhCollectQueryDataEx'					PdhCollectQueryDataEx					pdh		0	3	# Uses a separate thread to collect the current raw data value for all counters in the specified query. The function then signals the application-defined event and waits the specified time interval before returning.
imp	'PdhCollectQueryDataWithTime'				PdhCollectQueryDataWithTime				pdh		0		# Collects the current raw data value for all counters in the specified query and updates the status code of each counter.
imp	'PdhComputeCounterStatistics'				PdhComputeCounterStatistics				pdh		0		# Computes statistics for a counter from an array of raw values.
imp	'PdhConnectMachine'					PdhConnectMachineW					pdh		0		# Connects to the specified computer.
imp	'PdhEnumLogSetNames'					PdhEnumLogSetNamesW					pdh		0		# Enumerates the names of the log sets within the DSN.
imp	'PdhEnumMachines'					PdhEnumMachinesW					pdh		0		# Returns a list of the computer names associated with counters in a log file.
imp	'PdhEnumMachinesH'					PdhEnumMachinesHW					pdh		0		# Returns a list of the computer names associated with counters in a log file.
imp	'PdhEnumObjectItems'					PdhEnumObjectItemsW					pdh		0		# Returns the specified object's counter and instance names that exist on the specified computer or in the specified log file. To use handles to data sources, use the PdhEnumObjectItemsH function.
imp	'PdhEnumObjectItemsH'					PdhEnumObjectItemsHW					pdh		0		# Returns the specified object's counter and instance names that exist on the specified computer or in the specified log file. This function is identical to the PdhEnumObjectItems function, except that it supports the use of handles to data sources.
imp	'PdhEnumObjects'					PdhEnumObjectsW				       		pdh		0		# Returns a list of objects available on the specified computer or in the specified log file. To use handles to data sources, use the PdhEnumObjectsH function.
imp	'PdhEnumObjectsH'					PdhEnumObjectsHW					pdh		0		# Returns a list of objects available on the specified computer or in the specified log file.This function is identical to PdhEnumObjects, except that it supports the use of handles to data sources.
imp	'PdhExpandCounterPath'					PdhExpandCounterPathW					pdh		0		# Examines the specified computer (or local computer if none is specified) for counters and instances of counters that match the wildcard strings in the counter path.
imp	'PdhExpandWildCardPath'					PdhExpandWildCardPathW					pdh		0		# Examines the specified computer or log file and returns those counter paths that match the given counter path which contains wildcard characters. To use handles to data sources, use the PdhExpandWildCardPathH function.
imp	'PdhExpandWildCardPathH'				PdhExpandWildCardPathHW					pdh		0		# Examines the specified computer or log file and returns those counter paths that match the given counter path which contains wildcard characters.This function is identical to the PdhExpandWildCardPath function, except that it supports the use of handles to data sources.
imp	'PdhFormatFromRawValue'					PdhFormatFromRawValue					pdh		0		# Computes a displayable value for the given raw counter values.
imp	'PdhGetCounterInfo'					PdhGetCounterInfoW					pdh		0		# Retrieves information about a counter, such as data size, counter type, path, and user-supplied data values.
imp	'PdhGetCounterTimeBase'					PdhGetCounterTimeBase					pdh		0		# Returns the time base of the specified counter.
imp	'PdhGetDataSourceTimeRange'				PdhGetDataSourceTimeRangeW				pdh		0		# Determines the time range, number of entries and, if applicable, the size of the buffer containing the performance data from the specified input source. To use handles to data sources, use the PdhGetDataSourceTimeRangeH function.
imp	'PdhGetDataSourceTimeRangeH'				PdhGetDataSourceTimeRangeH				pdh		0		# Determines the time range, number of entries and, if applicable, the size of the buffer containing the performance data from the specified input source.This function is identical to the PdhGetDataSourceTimeRange function, except that it supports the use of handles to data sources.
imp	'PdhGetDefaultPerfCounter'				PdhGetDefaultPerfCounterW				pdh		0		# Retrieves the name of the default counter for the specified object. This name can be used to set the initial counter selection in the Browse Counter dialog box. To use handles to data sources, use the PdhGetDefaultPerfCounterH function.
imp	'PdhGetDefaultPerfCounterH'				PdhGetDefaultPerfCounterHW				pdh		0		# Retrieves the name of the default counter for the specified object.
imp	'PdhGetDefaultPerfObject'				PdhGetDefaultPerfObjectW				pdh		0		# Retrieves the name of the default object. This name can be used to set the initial object selection in the Browse Counter dialog box. To use handles to data sources, use the PdhGetDefaultPerfObjectH function.
imp	'PdhGetDefaultPerfObjectH'				PdhGetDefaultPerfObjectHW				pdh		0		# Retrieves the name of the default object.
imp	'PdhGetDllVersion'					PdhGetDllVersion					pdh		0		# Returns the version of the currently installed Pdh.dll file.
imp	'PdhGetFormattedCounterArray'				PdhGetFormattedCounterArrayW				pdh		0		# Returns an array of formatted counter values. Use this function when you want to format the counter values of a counter that contains a wildcard character for the instance name.
imp	'PdhGetFormattedCounterValue'				PdhGetFormattedCounterValue				pdh		0	4	# Computes a displayable value for the specified counter.
imp	'PdhGetLogFileSize'					PdhGetLogFileSize					pdh		0		# Returns the size of the specified log file.
imp	'PdhGetRawCounterArray'					PdhGetRawCounterArrayW					pdh		0		# Returns an array of raw values from the specified counter. Use this function when you want to retrieve the raw counter values of a counter that contains a wildcard character for the instance name.
imp	'PdhGetRawCounterValue'					PdhGetRawCounterValue					pdh		0		# Returns the current raw value of the counter.
imp	'PdhIsRealTimeQuery'					PdhIsRealTimeQuery					pdh		0		# Determines if the specified query is a real-time query.
imp	'PdhLookupPerfIndexByName'				PdhLookupPerfIndexByNameW				pdh		0		# Returns the counter index corresponding to the specified counter name.
imp	'PdhLookupPerfNameByIndex'				PdhLookupPerfNameByIndexW				pdh		0		# Returns the performance object name or counter name corresponding to the specified index.
imp	'PdhMakeCounterPath'					PdhMakeCounterPathW					pdh		0		# Creates a full counter path using the members specified in the PDH_COUNTER_PATH_ELEMENTS structure.
imp	'PdhOpenLog'						PdhOpenLogW						pdh		0		# Opens the specified log file for reading or writing.
imp	'PdhOpenQuery'						PdhOpenQueryW						pdh		0	3	# Creates a new query that is used to manage the collection of performance data. To use handles to data sources, use the PdhOpenQueryH function.
imp	'PdhOpenQueryH'						PdhOpenQueryH						pdh		0		# Creates a new query that is used to manage the collection of performance data. This function is identical to the PdhOpenQuery function, except that it supports the use of handles to data sources.
imp	'PdhParseCounterPath'					PdhParseCounterPathW					pdh		0		# Parses the elements of the counter path and stores the results in the PDH_COUNTER_PATH_ELEMENTS structure.
imp	'PdhParseInstanceName'					PdhParseInstanceNameW					pdh		0		# Parses the elements of an instance string.
imp	'PdhReadRawLogRecord'					PdhReadRawLogRecord					pdh		0		# Reads the information in the specified binary trace log file.
imp	'PdhRemoveCounter'					PdhRemoveCounter					pdh		0		# Removes a counter from a query.
imp	'PdhSelectDataSource'					PdhSelectDataSourceW					pdh		0		# Displays a dialog window that prompts the user to specify the source of the performance data.
imp	'PdhSetCounterScaleFactor'				PdhSetCounterScaleFactor				pdh		0		# Sets the scale factor that is applied to the calculated value of the specified counter when you request the formatted counter value. If the PDH_FMT_NOSCALE flag is set, then this scale factor is ignored.
imp	'PdhSetDefaultRealTimeDataSource'			PdhSetDefaultRealTimeDataSource				pdh		0		# Specifies the source of the real-time data.
imp	'PdhSetQueryTimeRange'					PdhSetQueryTimeRange					pdh		0		# Limits the samples that you can read from a log file to those within the specified time range, inclusively.
imp	'PdhUpdateLog'						PdhUpdateLogW						pdh		0		# Collects counter data for the current query and writes the data to the log file.
imp	'PdhUpdateLogFileCatalog'				PdhUpdateLogFileCatalog					pdh		0		# Synchronizes the information in the log file catalog with the performance data in the log file.
imp	'PdhValidatePath'					PdhValidatePathW					pdh		0		# Validates that the counter is present on the computer specified in the counter path.
imp	'PdhValidatePathEx'					PdhValidatePathExW					pdh		0		# Validates that the specified counter is present on the computer or in the log file.
imp	'PerfAddCounters'					PerfAddCounters						pdh		0		# Adds performance counter specifications to the specified query.
imp	'PerfCloseQueryHandle'					PerfCloseQueryHandle					pdh		0		# Closes a query handle that you opened by calling PerfOpenQueryHandle.
imp	'PerfCreateInstance'					PerfCreateInstance					pdh		0		# Creates an instance of the specified counter set.
imp	'PerfDecrementULongCounterValue'			PerfDecrementULongCounterValue				pdh		0		# Decrements the value of a counter whose value is a 4-byte unsigned integer. Providers use this function.
imp	'PerfDecrementULongLongCounterValue'			PerfDecrementULongLongCounterValue			pdh		0		# Decrements the value of a counter whose value is an 8-byte unsigned integer. Providers use this function.
imp	'PerfDeleteCounters'					PerfDeleteCounters					pdh		0		# Removes the specified performance counter specifications from the specified query.
imp	'PerfDeleteInstance'					PerfDeleteInstance					pdh		0		# Deletes an instance of the counter set created by the PerfCreateInstance function.
imp	'PerfEnumerateCounterSet'				PerfEnumerateCounterSet					pdh		0		# Gets the counter set identifiers of the counter sets that are registered on the specified system. Counter set identifiers are globally unique identifiers (GUIDs).
imp	'PerfEnumerateCounterSetInstances'			PerfEnumerateCounterSetInstances			pdh		0		# Gets the names and identifiers of the active instances of a counter set on the specified system.
imp	'PerfIncrementULongCounterValue'			PerfIncrementULongCounterValue				pdh		0		# Increments the value of a counter whose value is a 4-byte unsigned integer. Providers use this function.
imp	'PerfIncrementULongLongCounterValue'			PerfIncrementULongLongCounterValue			pdh		0		# Increments the value of a counter whose value is an 8-byte unsigned integer. Providers use this function.
imp	'PerfOpenQueryHandle'					PerfOpenQueryHandle					pdh		0		# Creates a handle that references a query on the specified system. A query is a list of counter specifications.
imp	'PerfQueryCounterData'					PerfQueryCounterData					pdh		0		# Gets the values of the performance counters that match the counter specifications in the specified query.
imp	'PerfQueryCounterInfo'					PerfQueryCounterInfo					pdh		0		# Gets the counter specifications in the specified query.
imp	'PerfQueryCounterSetRegistrationInfo'			PerfQueryCounterSetRegistrationInfo			pdh		0		# Gets information about a counter set on the specified system.
imp	'PerfQueryInstance'					PerfQueryInstance					pdh		0		# Retrieves a pointer to the specified counter set instance. Providers use this function.
imp	'PerfSetCounterRefValue'				PerfSetCounterRefValue					pdh		0		# Updates the value of a counter whose value is a pointer to the actual data. Providers use this function.
imp	'PerfSetCounterSetInfo'					PerfSetCounterSetInfo					pdh		0		# Specifies the layout of a particular counter set.
imp	'PerfSetULongCounterValue'				PerfSetULongCounterValue				pdh		0		# Updates the value of a counter whose value is a 4-byte unsigned integer. Providers use this function.
imp	'PerfSetULongLongCounterValue'				PerfSetULongLongCounterValue				pdh		0		# Updates the value of a counter whose value is an 8-byte unsigned integer. Providers use this function.
imp	'PerfStartProvider'					PerfStartProvider					pdh		0		# Registers the provider.
imp	'PerfStartProviderEx'					PerfStartProviderEx					pdh		0		# Registers the provider.
imp	'PerfStopProvider'					PerfStopProvider					pdh		0		# Removes the provider's registration from the list of registered providers and frees all resources associated with the provider.
imp	'UnloadPerfCounterTextStrings'				UnloadPerfCounterTextStringsW				pdh		0		# Unloads performance objects and counters from the computer for the specified application.

# PSAPI.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'EmptyWorkingSet'					EmptyWorkingSet						psapi		0
imp	'EnumDeviceDrivers'					EnumDeviceDrivers					psapi		0
imp	'EnumPageFiles'						EnumPageFilesW						psapi		0
imp	'EnumProcessModules'					EnumProcessModules					psapi		0
imp	'EnumProcessModulesEx'					EnumProcessModulesEx					psapi		0
imp	'EnumProcesses'						EnumProcesses						psapi		0
imp	'GetDeviceDriverBaseName'				GetDeviceDriverBaseNameW				psapi		0
imp	'GetDeviceDriverFileName'				GetDeviceDriverFileNameW				psapi		0
imp	'GetMappedFileName'					GetMappedFileNameW					psapi		0
imp	'GetModuleBaseName'					GetModuleBaseNameW					psapi		0
imp	'GetModuleFileNameEx'					GetModuleFileNameExW					psapi		0
imp	'GetModuleInformation'					GetModuleInformation					psapi		0
imp	'GetPerformanceInfo'					GetPerformanceInfo					psapi		0
imp	'GetProcessImageFileName'				GetProcessImageFileNameW				psapi		0	3
imp	'GetProcessMemoryInfo'					GetProcessMemoryInfo					psapi		0	3
imp	'GetWsChanges'						GetWsChanges						psapi		0
imp	'GetWsChangesEx'					GetWsChangesEx						psapi		0
imp	'InitializeProcessForWsWatch'				InitializeProcessForWsWatch				psapi		0
imp	'QueryWorkingSet'					QueryWorkingSet						psapi		0
imp	'QueryWorkingSetEx'					QueryWorkingSetEx					psapi		0

# URL.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'OpenURL'						OpenURL							url		111
imp	'TelnetProtocolHandler'					TelnetProtocolHandler					url		113
imp	'TranslateURLW'						TranslateURLW						url		116
imp	'URLAssociationDialog'					URLAssociationDialogW					url		118
imp	'AddMIMEFileTypesPS'					AddMIMEFileTypesPS					url		102
imp	'AutodialHookCallback'					AutodialHookCallback					url		103
imp	'FileProtocolHandler'					FileProtocolHandler					url		104
imp	'InetIsOffline'						InetIsOffline						url		106
imp	'MIMEAssociationDialog'					MIMEAssociationDialogW					url		108
imp	'MailToProtocolHandler'					MailToProtocolHandler					url		109

# API-MS-Win-Core-Synch-l1-2-0.dll (Windows 8+)
#
#	Name							Actual							DLL					Hint	Arity
imp	'WaitOnAddress'						WaitOnAddress						API-MS-Win-Core-Synch-l1-2-0		111	4
imp	'WakeByAddressAll'					WakeByAddressAll					API-MS-Win-Core-Synch-l1-2-0		113	1
imp	'WakeByAddressSingle'					WakeByAddressSingle					API-MS-Win-Core-Synch-l1-2-0		116	1

# NTDLL.DLL
# BEYOND THE PALE
#
#  “The functions and structures in [for these APIs] are internal to
#   the operating system and subject to change from one release of
#   Windows to the next, and possibly even between service packs for
#   each release.” ──Quoth MSDN
#
#	Name							Actual							DLL		Hint	Arity
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
imp	'ApiSetQueryApiSetPresence'				ApiSetQueryApiSetPresence				ntdll		27
imp	'CsrAllocateCaptureBuffer'				CsrAllocateCaptureBuffer				ntdll		28
imp	'CsrAllocateMessagePointer'				CsrAllocateMessagePointer				ntdll		29
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
imp	'EvtIntReportAuthzEventAndSourceAsync'			EvtIntReportAuthzEventAndSourceAsync			ntdll		98
imp	'EvtIntReportEventAndSourceAsync'			EvtIntReportEventAndSourceAsync				ntdll		99
imp	'ExpInterlockedPopEntrySListEnd'			ExpInterlockedPopEntrySListEnd				ntdll		100
imp	'ExpInterlockedPopEntrySListFault'			ExpInterlockedPopEntrySListFault			ntdll		101
imp	'ExpInterlockedPopEntrySListResume'			ExpInterlockedPopEntrySListResume			ntdll		102
imp	'KiRaiseUserExceptionDispatcher'			KiRaiseUserExceptionDispatcher				ntdll		103
imp	'KiUserApcDispatcher'					KiUserApcDispatcher					ntdll		104
imp	'KiUserCallbackDispatcher'				KiUserCallbackDispatcher				ntdll		105
imp	'KiUserExceptionDispatcher'				KiUserExceptionDispatcher				ntdll		106
imp	'KiUserInvertedFunctionTable'				KiUserInvertedFunctionTable				ntdll		107
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
imp	'MD4Final'						MD4Final						ntdll		188
imp	'MD4Init'						MD4Init							ntdll		189
imp	'MD4Update'						MD4Update						ntdll		190
imp	'NlsAnsiCodePage'					NlsAnsiCodePage						ntdll		194
imp	'NlsMbCodePageTag'					NlsMbCodePageTag					ntdll		195
imp	'NlsMbOemCodePageTag'					NlsMbOemCodePageTag					ntdll		196
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
imp	'NtdllDefWindowProc_W'					NtdllDefWindowProc_W					ntdll		661
imp	'NtdllDialogWndProc_W'					NtdllDialogWndProc_W					ntdll		663
imp	'PfxFindPrefix'						PfxFindPrefix						ntdll		664
imp	'PfxInitialize'						PfxInitialize						ntdll		665
imp	'PfxInsertPrefix'					PfxInsertPrefix						ntdll		666
imp	'PfxRemovePrefix'					PfxRemovePrefix						ntdll		667
imp	'PssNtCaptureSnapshot'					PssNtCaptureSnapshot					ntdll		668
imp	'PssNtDuplicateSnapshot'				PssNtDuplicateSnapshot					ntdll		669
imp	'PssNtFreeRemoteSnapshot'				PssNtFreeRemoteSnapshot					ntdll		670
imp	'PssNtFreeSnapshot'					PssNtFreeSnapshot					ntdll		671
imp	'PssNtFreeWalkMarker'					PssNtFreeWalkMarker					ntdll		672
imp	'PssNtQuerySnapshot'					PssNtQuerySnapshot					ntdll		673
imp	'PssNtValidateDescriptor'				PssNtValidateDescriptor					ntdll		674
imp	'PssNtWalkSnapshot'					PssNtWalkSnapshot					ntdll		675
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
imp	'RtlEthernetAddressToString'				RtlEthernetAddressToStringW				ntdll		947
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
imp	'RtlIpv4AddressToString'				RtlIpv4AddressToStringW					ntdll		1152
imp	'RtlIpv4AddressToStringEx'				RtlIpv4AddressToStringExW				ntdll		1151
imp	'RtlIpv4StringToAddress'				RtlIpv4StringToAddressW					ntdll		1156
imp	'RtlIpv4StringToAddressEx'				RtlIpv4StringToAddressExW				ntdll		1155
imp	'RtlIpv6AddressToString'				RtlIpv6AddressToStringW					ntdll		1160
imp	'RtlIpv6AddressToStringEx'				RtlIpv6AddressToStringExW				ntdll		1159
imp	'RtlIpv6StringToAddress'				RtlIpv6StringToAddressW					ntdll		1164
imp	'RtlIpv6StringToAddressEx'				RtlIpv6StringToAddressExW				ntdll		1163
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
imp	'SbExecuteProcedure'					SbExecuteProcedure					ntdll		1610
imp	'SbSelectProcedure'					SbSelectProcedure					ntdll		1611
imp	'ShipAssert'						ShipAssert						ntdll		1612
imp	'ShipAssertGetBufferInfo'				ShipAssertGetBufferInfo					ntdll		1613
imp	'ShipAssertMsg'						ShipAssertMsgW						ntdll		1615
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
imp	'VerSetConditionMask'					VerSetConditionMask					ntdll		1678
imp	'WerReportExceptionWorker'				WerReportExceptionWorker				ntdll		1679
imp	'WerReportSQMEvent'					WerReportSQMEvent					ntdll		1680
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
