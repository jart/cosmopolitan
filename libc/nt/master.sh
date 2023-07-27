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
imp	'ActivateActCtxWorker'					ActivateActCtxWorker					kernel32	0
imp	'AddAtom'						AddAtomW						kernel32	0
imp	'AddConsoleAlias'					AddConsoleAliasW					kernel32	0
imp	'AddDllDirectory'					AddDllDirectory						kernel32	0
imp	'AddIntegrityLabelToBoundaryDescriptor'			AddIntegrityLabelToBoundaryDescriptor			kernel32	0
imp	'AddLocalAlternateComputerName'				AddLocalAlternateComputerNameW				kernel32	0
imp	'AddRefActCtx'						AddRefActCtx						kernel32	0
imp	'AddRefActCtxWorker'					AddRefActCtxWorker					kernel32	0
imp	'AddResourceAttributeAce'				AddResourceAttributeAce					kernel32	0
imp	'AddSIDToBoundaryDescriptor'				AddSIDToBoundaryDescriptor				kernel32	0
imp	'AddScopedPolicyIDAce'					AddScopedPolicyIDAce					kernel32	0
imp	'AddSecureMemoryCacheCallback'				AddSecureMemoryCacheCallback				kernel32	0
imp	'AddVectoredContinueHandler'				AddVectoredContinueHandler				kernel32	0	2
imp	'AddVectoredExceptionHandler'				AddVectoredExceptionHandler				kernel32	0	2
imp	'AdjustCalendarDate'					AdjustCalendarDate					kernel32	0
imp	'AllocConsole'						AllocConsole						kernel32	0	0
imp	'AllocateUserPhysicalPages'				AllocateUserPhysicalPages				kernel32	0
imp	'AllocateUserPhysicalPagesNuma'				AllocateUserPhysicalPagesNuma				kernel32	0
imp	'ApplicationRecoveryFinished'				ApplicationRecoveryFinished				kernel32	0
imp	'ApplicationRecoveryInProgress'				ApplicationRecoveryInProgress				kernel32	0
imp	'AreFileApisANSI'					AreFileApisANSI						kernel32	0
imp	'AssignProcessToJobObject'				AssignProcessToJobObject				kernel32	0
imp	'AttachConsole'						AttachConsole						kernel32	0	1
imp	'BackupRead'						BackupRead						kernel32	0
imp	'BackupSeek'						BackupSeek						kernel32	0
imp	'BackupWrite'						BackupWrite						kernel32	0
imp	'BaseCheckAppcompatCacheExWorker'			BaseCheckAppcompatCacheExWorker				kernel32	0
imp	'BaseCheckAppcompatCacheWorker'				BaseCheckAppcompatCacheWorker				kernel32	0
imp	'BaseCheckElevation'					BaseCheckElevation					kernel32	0
imp	'BaseCleanupAppcompatCacheSupportWorker'		BaseCleanupAppcompatCacheSupportWorker			kernel32	0
imp	'BaseDestroyVDMEnvironment'				BaseDestroyVDMEnvironment				kernel32	0
imp	'BaseDllReadWriteIniFile'				BaseDllReadWriteIniFile					kernel32	0
imp	'BaseDumpAppcompatCacheWorker'				BaseDumpAppcompatCacheWorker				kernel32	0
imp	'BaseElevationPostProcessing'				BaseElevationPostProcessing				kernel32	0
imp	'BaseFlushAppcompatCacheWorker'				BaseFlushAppcompatCacheWorker				kernel32	0
imp	'BaseFormatTimeOut'					BaseFormatTimeOut					kernel32	0
imp	'BaseFreeAppCompatDataForProcessWorker'			BaseFreeAppCompatDataForProcessWorker			kernel32	0
imp	'BaseGenerateAppCompatData'				BaseGenerateAppCompatData				kernel32	0
imp	'BaseInitAppcompatCacheSupportWorker'			BaseInitAppcompatCacheSupportWorker			kernel32	0
imp	'BaseIsAppcompatInfrastructureDisabledWorker'		BaseIsAppcompatInfrastructureDisabledWorker		kernel32	0
imp	'BaseIsDosApplication'					BaseIsDosApplication					kernel32	0
imp	'BaseQueryModuleData'					BaseQueryModuleData					kernel32	0
imp	'BaseReadAppCompatDataForProcessWorker'			BaseReadAppCompatDataForProcessWorker			kernel32	0
imp	'BaseSetLastNTError'					BaseSetLastNTError					kernel32	0
imp	'BaseThreadInitThunk'					BaseThreadInitThunk					kernel32	0
imp	'BaseUpdateAppcompatCacheWorker'			BaseUpdateAppcompatCacheWorker				kernel32	0
imp	'BaseUpdateVDMEntry'					BaseUpdateVDMEntry					kernel32	0
imp	'BaseVerifyUnicodeString'				BaseVerifyUnicodeString					kernel32	0
imp	'BaseWriteErrorElevationRequiredEvent'			BaseWriteErrorElevationRequiredEvent			kernel32	0
imp	'Basep8BitStringToDynamicUnicodeString'			Basep8BitStringToDynamicUnicodeString			kernel32	0
imp	'BasepAllocateActivationContextActivationBlock'		BasepAllocateActivationContextActivationBlock		kernel32	0
imp	'BasepAnsiStringToDynamicUnicodeString'			BasepAnsiStringToDynamicUnicodeString			kernel32	0
imp	'BasepAppContainerEnvironmentExtension'			BasepAppContainerEnvironmentExtension			kernel32	0
imp	'BasepAppXExtension'					BasepAppXExtension					kernel32	0
imp	'BasepCheckAppCompat'					BasepCheckAppCompat					kernel32	0
imp	'BasepCheckWebBladeHashes'				BasepCheckWebBladeHashes				kernel32	0
imp	'BasepCheckWinSaferRestrictions'			BasepCheckWinSaferRestrictions				kernel32	0
imp	'BasepConstructSxsCreateProcessMessage'			BasepConstructSxsCreateProcessMessage			kernel32	0
imp	'BasepCopyEncryption'					BasepCopyEncryption					kernel32	0
imp	'BasepFreeActivationContextActivationBlock'		BasepFreeActivationContextActivationBlock		kernel32	0
imp	'BasepFreeAppCompatData'				BasepFreeAppCompatData					kernel32	0
imp	'BasepGetAppCompatData'					BasepGetAppCompatData					kernel32	0
imp	'BasepGetComputerNameFromNtPath'			BasepGetComputerNameFromNtPath				kernel32	0
imp	'BasepGetExeArchType'					BasepGetExeArchType					kernel32	0
imp	'BasepInitAppCompatData'				BasepInitAppCompatData					kernel32	0
imp	'BasepIsProcessAllowed'					BasepIsProcessAllowed					kernel32	0
imp	'BasepMapModuleHandle'					BasepMapModuleHandle					kernel32	0
imp	'BasepNotifyLoadStringResource'				BasepNotifyLoadStringResource				kernel32	0
imp	'BasepPostSuccessAppXExtension'				BasepPostSuccessAppXExtension				kernel32	0
imp	'BasepProcessInvalidImage'				BasepProcessInvalidImage				kernel32	0
imp	'BasepQueryAppCompat'					BasepQueryAppCompat					kernel32	0
imp	'BasepQueryModuleChpeSettings'				BasepQueryModuleChpeSettings				kernel32	0
imp	'BasepReleaseAppXContext'				BasepReleaseAppXContext					kernel32	0
imp	'BasepReleaseSxsCreateProcessUtilityStruct'		BasepReleaseSxsCreateProcessUtilityStruct		kernel32	0
imp	'BasepReportFault'					BasepReportFault					kernel32	0
imp	'BasepSetFileEncryptionCompression'			BasepSetFileEncryptionCompression			kernel32	0
imp	'Beep'							Beep							kernel32	0
imp	'BeginUpdateResource'					BeginUpdateResourceW					kernel32	0
imp	'BindIoCompletionCallback'				BindIoCompletionCallback				kernel32	0
imp	'BuildCommDCBAndTimeouts'				BuildCommDCBAndTimeoutsW				kernel32	0
imp	'BuildCommDCBW'						BuildCommDCBW						kernel32	0
imp	'CallNamedPipe'						CallNamedPipeW						kernel32	0	7
imp	'CallNamedPipeA'					CallNamedPipeA						kernel32	0	7
imp	'CallbackMayRunLong'					CallbackMayRunLong					kernel32	0
imp	'CancelDeviceWakeupRequest'				CancelDeviceWakeupRequest				kernel32	0
imp	'CancelIo'						CancelIo						kernel32	0	1
imp	'CancelSynchronousIo'					CancelSynchronousIo					kernel32	0	1
imp	'CancelTimerQueueTimer'					CancelTimerQueueTimer					kernel32	0
imp	'CancelWaitableTimer'					CancelWaitableTimer					kernel32	0
imp	'CeipIsOptedIn'						CeipIsOptedIn						kernel32	0
imp	'ChangeTimerQueueTimer'					ChangeTimerQueueTimer					kernel32	0
imp	'CheckElevation'					CheckElevation						kernel32	0
imp	'CheckElevationEnabled'					CheckElevationEnabled					kernel32	0
imp	'CheckForReadOnlyResource'				CheckForReadOnlyResource				kernel32	0
imp	'CheckForReadOnlyResourceFilter'			CheckForReadOnlyResourceFilter				kernel32	0
imp	'CheckNameLegalDOS8Dot3'				CheckNameLegalDOS8Dot3W					kernel32	0
imp	'CheckRemoteDebuggerPresent'				CheckRemoteDebuggerPresent				kernel32	0	2
imp	'CheckTokenCapability'					CheckTokenCapability					kernel32	0
imp	'CheckTokenMembershipEx'				CheckTokenMembershipEx					kernel32	0
imp	'ClearCommBreak'					ClearCommBreak						kernel32	0	1
imp	'ClearCommError'					ClearCommError						kernel32	0
imp	'CloseConsoleHandle'					CloseConsoleHandle					kernel32	0
imp	'ClosePackageInfo'					ClosePackageInfo					kernel32	0
imp	'ClosePrivateNamespace'					ClosePrivateNamespace					kernel32	0
imp	'CloseProfileUserMapping'				CloseProfileUserMapping					kernel32	0
imp	'CmdBatNotification'					CmdBatNotification					kernel32	0
imp	'CommConfigDialog'					CommConfigDialogW					kernel32	0
imp	'CompareCalendarDates'					CompareCalendarDates					kernel32	0
imp	'CompareFileTime'					CompareFileTime						kernel32	0
imp	'CompareString'						CompareStringW						kernel32	0
imp	'CompareStringEx'					CompareStringEx						kernel32	0
imp	'CompareStringOrdinal'					CompareStringOrdinal					kernel32	0
imp	'ConnectNamedPipe'					ConnectNamedPipe					kernel32	0	2
imp	'ConsoleMenuControl'					ConsoleMenuControl					kernel32	0
imp	'ContinueDebugEvent'					ContinueDebugEvent					kernel32	0	3
imp	'ConvertCalDateTimeToSystemTime'			ConvertCalDateTimeToSystemTime				kernel32	0
imp	'ConvertDefaultLocale'					ConvertDefaultLocale					kernel32	0
imp	'ConvertFiberToThread'					ConvertFiberToThread					kernel32	0
imp	'ConvertNLSDayOfWeekToWin32DayOfWeek'			ConvertNLSDayOfWeekToWin32DayOfWeek			kernel32	0
imp	'ConvertSystemTimeToCalDateTime'			ConvertSystemTimeToCalDateTime				kernel32	0
imp	'ConvertThreadToFiber'					ConvertThreadToFiber					kernel32	0
imp	'ConvertThreadToFiberEx'				ConvertThreadToFiberEx					kernel32	0
imp	'CopyContext'						CopyContext						kernel32	0
imp	'CopyFile'						CopyFileW						kernel32	0	3
imp	'CopyFile2'						CopyFile2						kernel32	0
imp	'CopyFileEx'						CopyFileExW						kernel32	0
imp	'CopyFileTransacted'					CopyFileTransactedW					kernel32	0
imp	'CopyLZFile'						CopyLZFile						kernel32	0
imp	'CreateActCtx'						CreateActCtxW						kernel32	0
imp	'CreateActCtxWWorker'					CreateActCtxWWorker					kernel32	0
imp	'CreateBoundaryDescriptor'				CreateBoundaryDescriptorW				kernel32	0
imp	'CreateConsoleScreenBuffer'				CreateConsoleScreenBuffer				kernel32	0
imp	'CreateDirectoryEx'					CreateDirectoryExW					kernel32	0
imp	'CreateDirectoryTransacted'				CreateDirectoryTransactedW				kernel32	0
imp	'CreateEvent'						CreateEventW						kernel32	0	4
imp	'CreateEventEx'						CreateEventExW						kernel32	0	4
imp	'CreateFiber'						CreateFiber						kernel32	0
imp	'CreateFiberEx'						CreateFiberEx						kernel32	0
imp	'CreateFile2'						CreateFile2						kernel32	0
imp	'CreateFileMappingFromApp'				CreateFileMappingFromApp				kernel32	0
imp	'CreateFileTransacted'					CreateFileTransactedW					kernel32	0
imp	'CreateHardLink'					CreateHardLinkW						kernel32	0	3
imp	'CreateHardLinkTransacted'				CreateHardLinkTransactedW				kernel32	0
imp	'CreateIoCompletionPort'				CreateIoCompletionPort					kernel32	0	4
imp	'CreateJobObject'					CreateJobObjectW					kernel32	0
imp	'CreateJobSet'						CreateJobSet						kernel32	0
imp	'CreateMailslot'					CreateMailslotW						kernel32	0
imp	'CreateMemoryResourceNotification'			CreateMemoryResourceNotification			kernel32	0
imp	'CreateMutex'						CreateMutexW						kernel32	0
imp	'CreateMutexEx'						CreateMutexExW						kernel32	0
imp	'CreatePrivateNamespace'				CreatePrivateNamespaceW					kernel32	0
imp	'CreateRemoteThread'					CreateRemoteThread					kernel32	0
imp	'CreateRemoteThreadEx'					CreateRemoteThreadEx					kernel32	0
imp	'CreateSemaphore'					CreateSemaphoreW					kernel32	0	4
imp	'CreateSemaphoreEx'					CreateSemaphoreExW					kernel32	0
imp	'CreateSymbolicLinkTransacted'				CreateSymbolicLinkTransactedW				kernel32	0
imp	'CreateTapePartition'					CreateTapePartition					kernel32	0
imp	'CreateThreadpool'					CreateThreadpool					kernel32	0
imp	'CreateThreadpoolCleanupGroup'				CreateThreadpoolCleanupGroup				kernel32	0
imp	'CreateThreadpoolIo'					CreateThreadpoolIo					kernel32	0
imp	'CreateThreadpoolTimer'					CreateThreadpoolTimer					kernel32	0
imp	'CreateThreadpoolWait'					CreateThreadpoolWait					kernel32	0
imp	'CreateThreadpoolWork'					CreateThreadpoolWork					kernel32	0
imp	'CreateTimerQueue'					CreateTimerQueue					kernel32	0
imp	'CreateTimerQueueTimer'					CreateTimerQueueTimer					kernel32	0
imp	'CreateToolhelp32Snapshot'				CreateToolhelp32Snapshot				kernel32	0	2
imp	'CreateUmsCompletionList'				CreateUmsCompletionList					kernel32	0
imp	'CreateUmsThreadContext'				CreateUmsThreadContext					kernel32	0
imp	'CreateWaitableTimer'					CreateWaitableTimerW					kernel32	0	3
imp	'CreateWaitableTimerEx'					CreateWaitableTimerExW					kernel32	0	4
imp	'DeactivateActCtx'					DeactivateActCtx					kernel32	0
imp	'DeactivateActCtxWorker'				DeactivateActCtxWorker					kernel32	0
imp	'DebugActiveProcess'					DebugActiveProcess					kernel32	0	1
imp	'DebugActiveProcessStop'				DebugActiveProcessStop					kernel32	0	1
imp	'DebugBreakProcess'					DebugBreakProcess					kernel32	0	1
imp	'DebugSetProcessKillOnExit'				DebugSetProcessKillOnExit				kernel32	0
imp	'DefineDosDevice'					DefineDosDeviceW					kernel32	0
imp	'DeleteAtom'						DeleteAtom						kernel32	0
imp	'DeleteBoundaryDescriptor'				DeleteBoundaryDescriptor				kernel32	0
imp	'DeleteCriticalSection'					DeleteCriticalSection					kernel32	0	1
imp	'DeleteFiber'						DeleteFiber						kernel32	0
imp	'DeleteFileTransacted'					DeleteFileTransactedW					kernel32	0
imp	'DeleteProcThreadAttributeList'				DeleteProcThreadAttributeList				kernel32	0	1
imp	'DeleteSynchronizationBarrier'				DeleteSynchronizationBarrier				kernel32	0
imp	'DeleteTimerQueue'					DeleteTimerQueue					kernel32	0
imp	'DeleteTimerQueueEx'					DeleteTimerQueueEx					kernel32	0
imp	'DeleteTimerQueueTimer'					DeleteTimerQueueTimer					kernel32	0
imp	'DeleteUmsCompletionList'				DeleteUmsCompletionList					kernel32	0
imp	'DeleteUmsThreadContext'				DeleteUmsThreadContext					kernel32	0
imp	'DeleteVolumeMountPoint'				DeleteVolumeMountPointW					kernel32	0
imp	'DequeueUmsCompletionListItems'				DequeueUmsCompletionListItems				kernel32	0
imp	'DisableThreadLibraryCalls'				DisableThreadLibraryCalls				kernel32	0
imp	'DisableThreadProfiling'				DisableThreadProfiling					kernel32	0
imp	'DiscardVirtualMemory'					DiscardVirtualMemory					kernel32	0
imp	'DisconnectNamedPipe'					DisconnectNamedPipe					kernel32	0	1
imp	'DnsHostnameToComputerName'				DnsHostnameToComputerNameW				kernel32	0
imp	'DosDateTimeToFileTime'					DosDateTimeToFileTime					kernel32	0
imp	'DosPathToSessionPath'					DosPathToSessionPathW					kernel32	0
imp	'DuplicateConsoleHandle'				DuplicateConsoleHandle					kernel32	0
imp	'DuplicateEncryptionInfoFileExt'			DuplicateEncryptionInfoFileExt				kernel32	0
imp	'DuplicateHandle'					DuplicateHandle						kernel32	0	7
imp	'EnableThreadProfiling'					EnableThreadProfiling					kernel32	0
imp	'EndUpdateResource'					EndUpdateResourceW					kernel32	0
imp	'EnterCriticalSection'					EnterCriticalSection					kernel32	0	1
imp	'EnterSynchronizationBarrier'				EnterSynchronizationBarrier				kernel32	0
imp	'EnterUmsSchedulingMode'				EnterUmsSchedulingMode					kernel32	0
imp	'EnumCalendarInfo'					EnumCalendarInfoW					kernel32	0
imp	'EnumCalendarInfoEx'					EnumCalendarInfoExW					kernel32	0
imp	'EnumCalendarInfoExEx'					EnumCalendarInfoExEx					kernel32	0
imp	'EnumDateFormats'					EnumDateFormatsW					kernel32	0
imp	'EnumDateFormatsEx'					EnumDateFormatsExW					kernel32	0
imp	'EnumDateFormatsExEx'					EnumDateFormatsExEx					kernel32	0
imp	'EnumLanguageGroupLocales'				EnumLanguageGroupLocalesW				kernel32	0
imp	'EnumResourceLanguages'					EnumResourceLanguagesW					kernel32	0
imp	'EnumResourceLanguagesEx'				EnumResourceLanguagesExW				kernel32	0
imp	'EnumResourceNames'					EnumResourceNamesW					kernel32	0
imp	'EnumResourceNamesEx'					EnumResourceNamesExW					kernel32	0
imp	'EnumResourceTypes'					EnumResourceTypesW					kernel32	0
imp	'EnumResourceTypesEx'					EnumResourceTypesExW					kernel32	0
imp	'EnumSystemCodePages'					EnumSystemCodePagesW					kernel32	0
imp	'EnumSystemFirmwareTables'				EnumSystemFirmwareTables				kernel32	0
imp	'EnumSystemGeoID'					EnumSystemGeoID						kernel32	0
imp	'EnumSystemGeoNames'					EnumSystemGeoNames					kernel32	0
imp	'EnumSystemLanguageGroups'				EnumSystemLanguageGroupsW				kernel32	0
imp	'EnumSystemLocales'					EnumSystemLocalesW					kernel32	0
imp	'EnumSystemLocalesEx'					EnumSystemLocalesEx					kernel32	0
imp	'EnumTimeFormats'					EnumTimeFormatsW					kernel32	0
imp	'EnumTimeFormatsEx'					EnumTimeFormatsEx					kernel32	0
imp	'EnumUILanguages'					EnumUILanguagesW					kernel32	0
imp	'EnumerateLocalComputerNames'				EnumerateLocalComputerNamesW				kernel32	0
imp	'EraseTape'						EraseTape						kernel32	0
imp	'EscapeCommFunction'					EscapeCommFunction					kernel32	0
imp	'ExecuteUmsThread'					ExecuteUmsThread					kernel32	0
imp	'ExitProcess'						ExitProcess						kernel32	0	1	# a.k.a. RtlExitUserProcess
imp	'ExitThread'						ExitThread						kernel32	0	1
imp	'ExitVDM'						ExitVDM							kernel32	0
imp	'ExpandEnvironmentStrings'				ExpandEnvironmentStringsW				kernel32	0
imp	'FatalAppExit'						FatalAppExitW						kernel32	0
imp	'FatalExit'						FatalExit						kernel32	0	1
imp	'FileTimeToDosDateTime'					FileTimeToDosDateTime					kernel32	0
imp	'FileTimeToLocalFileTime'				FileTimeToLocalFileTime					kernel32	0
imp	'FileTimeToSystemTime'					FileTimeToSystemTime					kernel32	0
imp	'FillConsoleOutputAttribute'				FillConsoleOutputAttribute				kernel32	0	5
imp	'FillConsoleOutputCharacter'				FillConsoleOutputCharacterW				kernel32	0	5
imp	'FindActCtxSectionGuid'					FindActCtxSectionGuid					kernel32	0
imp	'FindActCtxSectionGuidWorker'				FindActCtxSectionGuidWorker				kernel32	0
imp	'FindActCtxSectionString'				FindActCtxSectionStringW				kernel32	0
imp	'FindActCtxSectionStringWWorker'			FindActCtxSectionStringWWorker				kernel32	0
imp	'FindAtom'						FindAtomW						kernel32	0
imp	'FindCloseChangeNotification'				FindCloseChangeNotification				kernel32	0
imp	'FindFirstChangeNotification'				FindFirstChangeNotificationW				kernel32	0
imp	'FindFirstFileEx'					FindFirstFileExW					kernel32	0	6
imp	'FindFirstFileName'					FindFirstFileNameW					kernel32	0
imp	'FindFirstFileNameTransacted'				FindFirstFileNameTransactedW				kernel32	0
imp	'FindFirstFileTransacted'				FindFirstFileTransactedW				kernel32	0
imp	'FindFirstStream'					FindFirstStreamW					kernel32	0
imp	'FindFirstStreamTransacted'				FindFirstStreamTransactedW				kernel32	0
imp	'FindFirstVolume'					FindFirstVolumeW					kernel32	0	2
imp	'FindFirstVolumeMountPoint'				FindFirstVolumeMountPointW				kernel32	0
imp	'FindNLSString'						FindNLSString						kernel32	0
imp	'FindNLSStringEx'					FindNLSStringEx						kernel32	0
imp	'FindNextChangeNotification'				FindNextChangeNotification				kernel32	0
imp	'FindNextFileName'					FindNextFileNameW					kernel32	0
imp	'FindNextStream'					FindNextStreamW						kernel32	0
imp	'FindNextVolume'					FindNextVolumeW						kernel32	0	3
imp	'FindNextVolumeMountPoint'				FindNextVolumeMountPointW				kernel32	0
imp	'FindPackagesByPackageFamily'				FindPackagesByPackageFamily				kernel32	0
imp	'FindResource'						FindResourceW						kernel32	0
imp	'FindResourceEx'					FindResourceExW						kernel32	0
imp	'FindStringOrdinal'					FindStringOrdinal					kernel32	0
imp	'FindVolumeClose'					FindVolumeClose						kernel32	0	1
imp	'FindVolumeMountPointClose'				FindVolumeMountPointClose				kernel32	0
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
imp	'FreeMemoryJobObject'					FreeMemoryJobObject					kernel32	0
imp	'FreeResource'						FreeResource						kernel32	0	1
imp	'FreeUserPhysicalPages'					FreeUserPhysicalPages					kernel32	0
imp	'GetACP'						GetACP							kernel32	0
imp	'GetActiveProcessorCount'				GetActiveProcessorCount					kernel32	0
imp	'GetActiveProcessorGroupCount'				GetActiveProcessorGroupCount				kernel32	0
imp	'GetAppContainerNamedObjectPath'			GetAppContainerNamedObjectPath				kernel32	0
imp	'GetApplicationRecoveryCallback'			GetApplicationRecoveryCallback				kernel32	0
imp	'GetApplicationRecoveryCallbackWorker'			GetApplicationRecoveryCallbackWorker			kernel32	0
imp	'GetApplicationRestartSettings'				GetApplicationRestartSettings				kernel32	0
imp	'GetApplicationRestartSettingsWorker'			GetApplicationRestartSettingsWorker			kernel32	0
imp	'GetApplicationUserModelId'				GetApplicationUserModelId				kernel32	0
imp	'GetAtomName'						GetAtomNameW						kernel32	0
imp	'GetBinaryType'						GetBinaryTypeW						kernel32	0
imp	'GetCPInfo'						GetCPInfo						kernel32	0
imp	'GetCPInfoEx'						GetCPInfoExW						kernel32	0
imp	'GetCalendarDateFormat'					GetCalendarDateFormat					kernel32	0
imp	'GetCalendarDateFormatEx'				GetCalendarDateFormatEx					kernel32	0
imp	'GetCalendarDaysInMonth'				GetCalendarDaysInMonth					kernel32	0
imp	'GetCalendarDifferenceInDays'				GetCalendarDifferenceInDays				kernel32	0
imp	'GetCalendarInfo'					GetCalendarInfoW					kernel32	0
imp	'GetCalendarInfoEx'					GetCalendarInfoEx					kernel32	0
imp	'GetCalendarMonthsInYear'				GetCalendarMonthsInYear					kernel32	0
imp	'GetCalendarSupportedDateRange'				GetCalendarSupportedDateRange				kernel32	0
imp	'GetCalendarWeekNumber'					GetCalendarWeekNumber					kernel32	0
imp	'GetComPlusPackageInstallStatus'			GetComPlusPackageInstallStatus				kernel32	0
imp	'GetCommConfig'						GetCommConfig						kernel32	0
imp	'GetCommMask'						GetCommMask						kernel32	0
imp	'GetCommModemStatus'					GetCommModemStatus					kernel32	0
imp	'GetCommProperties'					GetCommProperties					kernel32	0
imp	'GetCommState'						GetCommState						kernel32	0
imp	'GetCommTimeouts'					GetCommTimeouts						kernel32	0
imp	'GetCommandLine'					GetCommandLineW						kernel32	0	0
imp	'GetCompressedFileSize'					GetCompressedFileSizeW					kernel32	0	2
imp	'GetCompressedFileSizeTransacted'			GetCompressedFileSizeTransactedW			kernel32	0
imp	'GetComputerName'					GetComputerNameW					kernel32	0
imp	'GetComputerNameEx'					GetComputerNameExW					kernel32	0	3
imp	'GetConsoleAlias'					GetConsoleAliasW					kernel32	0
imp	'GetConsoleAliasExes'					GetConsoleAliasExesW					kernel32	0
imp	'GetConsoleAliasExesLength'				GetConsoleAliasExesLengthW				kernel32	0
imp	'GetConsoleAliases'					GetConsoleAliasesW					kernel32	0
imp	'GetConsoleAliasesLength'				GetConsoleAliasesLengthW				kernel32	0
imp	'GetConsoleCP'						GetConsoleCP						kernel32	0	0
imp	'GetConsoleCharType'					GetConsoleCharType					kernel32	0
imp	'GetConsoleCursorInfo'					GetConsoleCursorInfo					kernel32	0	2
imp	'GetConsoleCursorMode'					GetConsoleCursorMode					kernel32	0
imp	'GetConsoleDisplayMode'					GetConsoleDisplayMode					kernel32	0
imp	'GetConsoleFontInfo'					GetConsoleFontInfo					kernel32	0
imp	'GetConsoleFontSize'					GetConsoleFontSize					kernel32	0
imp	'GetConsoleHardwareState'				GetConsoleHardwareState					kernel32	0
imp	'GetConsoleHistoryInfo'					GetConsoleHistoryInfo					kernel32	0
imp	'GetConsoleInputWaitHandle'				GetConsoleInputWaitHandle				kernel32	0
imp	'GetConsoleKeyboardLayoutName'				GetConsoleKeyboardLayoutNameW				kernel32	0
imp	'GetConsoleMode'					GetConsoleMode						kernel32	0	2
imp	'GetConsoleNlsMode'					GetConsoleNlsMode					kernel32	0
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
imp	'GetCurrentActCtxWorker'				GetCurrentActCtxWorker					kernel32	0
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
imp	'GetCurrentUmsThread'					GetCurrentUmsThread					kernel32	0
imp	'GetDateFormat'						GetDateFormatW						kernel32	0
imp	'GetDateFormatAWorker'					GetDateFormatAWorker					kernel32	0
imp	'GetDateFormatEx'					GetDateFormatEx						kernel32	0
imp	'GetDateFormatWWorker'					GetDateFormatWWorker					kernel32	0
imp	'GetDefaultCommConfig'					GetDefaultCommConfigW					kernel32	0
imp	'GetDevicePowerState'					GetDevicePowerState					kernel32	0
imp	'GetDiskFreeSpace'					GetDiskFreeSpaceW					kernel32	0
imp	'GetDiskFreeSpaceEx'					GetDiskFreeSpaceExW					kernel32	0
imp	'GetDllDirectory'					GetDllDirectoryW					kernel32	0
imp	'GetDriveType'						GetDriveTypeW						kernel32	0
imp	'GetDurationFormat'					GetDurationFormat					kernel32	0
imp	'GetDurationFormatEx'					GetDurationFormatEx					kernel32	0
imp	'GetDynamicTimeZoneInformation'				GetDynamicTimeZoneInformation				kernel32	0
imp	'GetEnabledXStateFeatures'				GetEnabledXStateFeatures				kernel32	0
imp	'GetEncryptedFileVersionExt'				GetEncryptedFileVersionExt				kernel32	0
imp	'GetEnvironmentStrings'					GetEnvironmentStringsW					kernel32	0	1
imp	'GetEnvironmentVariable'				GetEnvironmentVariableW					kernel32	0	3
imp	'GetErrorMode'						GetErrorMode						kernel32	0
imp	'GetExitCodeThread'					GetExitCodeThread					kernel32	0	2
imp	'GetExpandedName'					GetExpandedNameW					kernel32	0
imp	'GetFileAttributesEx'					GetFileAttributesExW					kernel32	0	3
imp	'GetFileAttributesTransacted'				GetFileAttributesTransactedW				kernel32	0
imp	'GetFileBandwidthReservation'				GetFileBandwidthReservation				kernel32	0
imp	'GetFileInformationByHandle'				GetFileInformationByHandle				kernel32	0	2
imp	'GetFileInformationByHandleEx'				GetFileInformationByHandleEx				kernel32	0	4
imp	'GetFileMUIInfo'					GetFileMUIInfo						kernel32	0
imp	'GetFileMUIPath'					GetFileMUIPath						kernel32	0
imp	'GetFileSize'						GetFileSize						kernel32	0	2
imp	'GetFileSizeEx'						GetFileSizeEx						kernel32	0	2
imp	'GetFileTime'						GetFileTime						kernel32	0	4
imp	'GetFileType'						GetFileType						kernel32	0	1
imp	'GetFinalPathNameByHandle'				GetFinalPathNameByHandleW				kernel32	0	4
imp	'GetFirmwareEnvironmentVariable'			GetFirmwareEnvironmentVariableW				kernel32	0
imp	'GetFirmwareEnvironmentVariableEx'			GetFirmwareEnvironmentVariableExW			kernel32	0
imp	'GetFirmwareType'					GetFirmwareType						kernel32	0
imp	'GetFullPathName'					GetFullPathNameW					kernel32	0	4
imp	'GetFullPathNameTransacted'				GetFullPathNameTransactedW				kernel32	0
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
imp	'GetLongPathNameTransacted'				GetLongPathNameTransactedW				kernel32	0
imp	'GetMailslotInfo'					GetMailslotInfo						kernel32	0
imp	'GetMaximumProcessorCount'				GetMaximumProcessorCount				kernel32	0	1	# Windows 7+
imp	'GetMaximumProcessorGroupCount'				GetMaximumProcessorGroupCount				kernel32	0
imp	'GetMemoryErrorHandlingCapabilities'			GetMemoryErrorHandlingCapabilities			kernel32	0
imp	'GetModuleFileName'					GetModuleFileNameW					kernel32	0	3
imp	'GetModuleHandle'					GetModuleHandleA					kernel32	0	1
imp	'GetModuleHandleEx'					GetModuleHandleExW					kernel32	0	3
imp	'GetModuleHandleW'					GetModuleHandleW					kernel32	0	1
imp	'GetNLSVersion'						GetNLSVersion						kernel32	0
imp	'GetNLSVersionEx'					GetNLSVersionEx						kernel32	0
imp	'GetNamedPipeClientComputerName'			GetNamedPipeClientComputerNameW				kernel32	0
imp	'GetNamedPipeClientProcessId'				GetNamedPipeClientProcessId				kernel32	0
imp	'GetNamedPipeClientSessionId'				GetNamedPipeClientSessionId				kernel32	0
imp	'GetNamedPipeHandleState'				GetNamedPipeHandleStateW				kernel32	0
imp	'GetNamedPipeInfo'					GetNamedPipeInfo					kernel32	0
imp	'GetNamedPipeServerProcessId'				GetNamedPipeServerProcessId				kernel32	0
imp	'GetNamedPipeServerSessionId'				GetNamedPipeServerSessionId				kernel32	0
imp	'GetNativeSystemInfo'					GetNativeSystemInfo					kernel32	0
imp	'GetNextUmsListItem'					GetNextUmsListItem					kernel32	0
imp	'GetNextVDMCommand'					GetNextVDMCommand					kernel32	0
imp	'GetNumaAvailableMemoryNode'				GetNumaAvailableMemoryNode				kernel32	0
imp	'GetNumaAvailableMemoryNodeEx'				GetNumaAvailableMemoryNodeEx				kernel32	0
imp	'GetNumaHighestNodeNumber'				GetNumaHighestNodeNumber				kernel32	0
imp	'GetNumaNodeNumberFromHandle'				GetNumaNodeNumberFromHandle				kernel32	0
imp	'GetNumaNodeProcessorMask'				GetNumaNodeProcessorMask				kernel32	0
imp	'GetNumaNodeProcessorMaskEx'				GetNumaNodeProcessorMaskEx				kernel32	0
imp	'GetNumaProcessorNode'					GetNumaProcessorNode					kernel32	0
imp	'GetNumaProcessorNodeEx'				GetNumaProcessorNodeEx					kernel32	0
imp	'GetNumaProximityNode'					GetNumaProximityNode					kernel32	0
imp	'GetNumaProximityNodeEx'				GetNumaProximityNodeEx					kernel32	0
imp	'GetNumberFormat'					GetNumberFormatW					kernel32	0
imp	'GetNumberFormatEx'					GetNumberFormatEx					kernel32	0
imp	'GetNumberOfConsoleFonts'				GetNumberOfConsoleFonts					kernel32	0
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
imp	'GetPrivateProfileInt'					GetPrivateProfileIntW					kernel32	0
imp	'GetPrivateProfileSection'				GetPrivateProfileSectionW				kernel32	0
imp	'GetPrivateProfileSectionNames'				GetPrivateProfileSectionNamesW				kernel32	0
imp	'GetPrivateProfileString'				GetPrivateProfileStringW				kernel32	0
imp	'GetPrivateProfileStruct'				GetPrivateProfileStructW				kernel32	0
imp	'GetProcAddress'					GetProcAddress						kernel32	0	2
imp	'GetProcessAffinityMask'				GetProcessAffinityMask					kernel32	0	3
imp	'GetProcessDEPPolicy'					GetProcessDEPPolicy					kernel32	0
imp	'GetProcessDefaultCpuSets'				GetProcessDefaultCpuSets				kernel32	0
imp	'GetProcessGroupAffinity'				GetProcessGroupAffinity					kernel32	0
imp	'GetProcessHandleCount'					GetProcessHandleCount					kernel32	0	2
imp	'GetProcessHeap'					GetProcessHeap						kernel32	0	0
imp	'GetProcessHeaps'					GetProcessHeaps						kernel32	0	2
imp	'GetProcessId'						GetProcessId						kernel32	0	1
imp	'GetProcessIdOfThread'					GetProcessIdOfThread					kernel32	0	1
imp	'GetProcessInformation'					GetProcessInformation					kernel32	0	4
imp	'GetProcessIoCounters'					GetProcessIoCounters					kernel32	0	2
imp	'GetProcessMitigationPolicy'				GetProcessMitigationPolicy				kernel32	0
imp	'GetProcessPreferredUILanguages'			GetProcessPreferredUILanguages				kernel32	0
imp	'GetProcessPriorityBoost'				GetProcessPriorityBoost					kernel32	0	2
imp	'GetProcessShutdownParameters'				GetProcessShutdownParameters				kernel32	0
imp	'GetProcessTimes'					GetProcessTimes						kernel32	0	5
imp	'GetProcessVersion'					GetProcessVersion					kernel32	0
imp	'GetProcessWorkingSetSize'				GetProcessWorkingSetSize				kernel32	0	3
imp	'GetProcessWorkingSetSizeEx'				GetProcessWorkingSetSizeEx				kernel32	0	4
imp	'GetProcessorSystemCycleTime'				GetProcessorSystemCycleTime				kernel32	0
imp	'GetProductInfo'					GetProductInfo						kernel32	0
imp	'GetProfileInt'						GetProfileIntW						kernel32	0
imp	'GetProfileSection'					GetProfileSectionW					kernel32	0
imp	'GetProfileString'					GetProfileStringW					kernel32	0
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
imp	'GetSystemDEPPolicy'					GetSystemDEPPolicy					kernel32	0
imp	'GetSystemDefaultLCID'					GetSystemDefaultLCID					kernel32	0
imp	'GetSystemDefaultLangID'				GetSystemDefaultLangID					kernel32	0
imp	'GetSystemDefaultLocaleName'				GetSystemDefaultLocaleName				kernel32	0
imp	'GetSystemDefaultUILanguage'				GetSystemDefaultUILanguage				kernel32	0
imp	'GetSystemDirectory'					GetSystemDirectoryW					kernel32	0	2
imp	'GetSystemDirectoryA'					GetSystemDirectoryA					kernel32	0	2
imp	'GetSystemFileCacheSize'				GetSystemFileCacheSize					kernel32	0
imp	'GetSystemFirmwareTable'				GetSystemFirmwareTable					kernel32	0
imp	'GetSystemInfo'						GetSystemInfo						kernel32	0	1
imp	'GetSystemPowerStatus'					GetSystemPowerStatus					kernel32	0
imp	'GetSystemPreferredUILanguages'				GetSystemPreferredUILanguages				kernel32	0
imp	'GetSystemRegistryQuota'				GetSystemRegistryQuota					kernel32	0
imp	'GetSystemTime'						GetSystemTime						kernel32	0	1
imp	'GetSystemTimeAdjustment'				GetSystemTimeAdjustment					kernel32	0	3
imp	'GetSystemTimeAsFileTime'				GetSystemTimeAsFileTime					kernel32	0	1
imp	'GetSystemTimePreciseAsFileTime'			GetSystemTimePreciseAsFileTime				kernel32	0	1
imp	'GetSystemTimes'					GetSystemTimes						kernel32	0	3
imp	'GetSystemWindowsDirectory'				GetSystemWindowsDirectoryW				kernel32	0
imp	'GetTapeParameters'					GetTapeParameters					kernel32	0
imp	'GetTapePosition'					GetTapePosition						kernel32	0
imp	'GetTapeStatus'						GetTapeStatus						kernel32	0
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
imp	'GetThreadSelectorEntry'				GetThreadSelectorEntry					kernel32	0
imp	'GetThreadTimes'					GetThreadTimes						kernel32	0	5
imp	'GetThreadUILanguage'					GetThreadUILanguage					kernel32	0
imp	'GetTickCount'						GetTickCount						kernel32	0
imp	'GetTickCount64'					GetTickCount64						kernel32	0	0
imp	'GetTimeFormat'						GetTimeFormatW						kernel32	0
imp	'GetTimeFormatAWorker'					GetTimeFormatAWorker					kernel32	0
imp	'GetTimeFormatEx'					GetTimeFormatEx						kernel32	0
imp	'GetTimeFormatWWorker'					GetTimeFormatWWorker					kernel32	0
imp	'GetTimeZoneInformation'				GetTimeZoneInformation					kernel32	0
imp	'GetTimeZoneInformationForYear'				GetTimeZoneInformationForYear				kernel32	0
imp	'GetUILanguageInfo'					GetUILanguageInfo					kernel32	0
imp	'GetUmsCompletionListEvent'				GetUmsCompletionListEvent				kernel32	0
imp	'GetUmsSystemThreadInformation'				GetUmsSystemThreadInformation				kernel32	0
imp	'GetUserDefaultLCID'					GetUserDefaultLCID					kernel32	0
imp	'GetUserDefaultLangID'					GetUserDefaultLangID					kernel32	0
imp	'GetUserDefaultLocaleName'				GetUserDefaultLocaleName				kernel32	0
imp	'GetUserDefaultUILanguage'				GetUserDefaultUILanguage				kernel32	0
imp	'GetUserGeoID'						GetUserGeoID						kernel32	0
imp	'GetUserPreferredUILanguages'				GetUserPreferredUILanguages				kernel32	0
imp	'GetVDMCurrentDirectories'				GetVDMCurrentDirectories				kernel32	0
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
imp	'GlobalAddAtom'						GlobalAddAtomW						kernel32	0
imp	'GlobalAddAtomEx'					GlobalAddAtomExW					kernel32	0
imp	'GlobalAlloc'						GlobalAlloc						kernel32	0	2
imp	'GlobalCompact'						GlobalCompact						kernel32	0
imp	'GlobalDeleteAtom'					GlobalDeleteAtom					kernel32	0
imp	'GlobalFindAtom'					GlobalFindAtomW						kernel32	0
imp	'GlobalFix'						GlobalFix						kernel32	0
imp	'GlobalFlags'						GlobalFlags						kernel32	0
imp	'GlobalFree'						GlobalFree						kernel32	0	1
imp	'GlobalGetAtomName'					GlobalGetAtomNameW					kernel32	0
imp	'GlobalHandle'						GlobalHandle						kernel32	0
imp	'GlobalLock'						GlobalLock						kernel32	0
imp	'GlobalMemoryStatus'					GlobalMemoryStatus					kernel32	0
imp	'GlobalMemoryStatusEx'					GlobalMemoryStatusEx					kernel32	0	1
imp	'GlobalReAlloc'						GlobalReAlloc						kernel32	0
imp	'GlobalSize'						GlobalSize						kernel32	0
imp	'GlobalUnWire'						GlobalUnWire						kernel32	0
imp	'GlobalUnfix'						GlobalUnfix						kernel32	0
imp	'GlobalUnlock'						GlobalUnlock						kernel32	0
imp	'GlobalWire'						GlobalWire						kernel32	0
imp	'Heap32First'						Heap32First						kernel32	0
imp	'Heap32ListFirst'					Heap32ListFirst						kernel32	0
imp	'Heap32ListNext'					Heap32ListNext						kernel32	0
imp	'Heap32Next'						Heap32Next						kernel32	0
imp	'HeapAlloc'						HeapAlloc						kernel32	0	3
imp	'HeapCompact'						HeapCompact						kernel32	0	2
imp	'HeapCreate'						HeapCreate						kernel32	0	3
imp	'HeapDestroy'						HeapDestroy						kernel32	0	1
imp	'HeapFree'						HeapFree						kernel32	0	3
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
imp	'InitAtomTable'						InitAtomTable						kernel32	0
imp	'InitOnceBeginInitialize'				InitOnceBeginInitialize					kernel32	0
imp	'InitOnceComplete'					InitOnceComplete					kernel32	0
imp	'InitOnceExecuteOnce'					InitOnceExecuteOnce					kernel32	0
imp	'InitializeContext'					InitializeContext					kernel32	0
imp	'InitializeCriticalSection'				InitializeCriticalSection				kernel32	0	1
imp	'InitializeCriticalSectionAndSpinCount'			InitializeCriticalSectionAndSpinCount			kernel32	0	2
imp	'InitializeCriticalSectionEx'				InitializeCriticalSectionEx				kernel32	0
imp	'InitializeProcThreadAttributeList'			InitializeProcThreadAttributeList			kernel32	0	4
imp	'InitializeSRWLock'					InitializeSRWLock					kernel32	0	1
imp	'InitializeSynchronizationBarrier'			InitializeSynchronizationBarrier			kernel32	0
imp	'InstallELAMCertificateInfo'				InstallELAMCertificateInfo				kernel32	0
imp	'InvalidateConsoleDIBits'				InvalidateConsoleDIBits					kernel32	0
imp	'IsBadCodePtr'						IsBadCodePtr						kernel32	0
imp	'IsBadHugeReadPtr'					IsBadHugeReadPtr					kernel32	0
imp	'IsBadHugeWritePtr'					IsBadHugeWritePtr					kernel32	0
imp	'IsBadReadPtr'						IsBadReadPtr						kernel32	0
imp	'IsBadStringPtr'					IsBadStringPtrW						kernel32	0
imp	'IsBadWritePtr'						IsBadWritePtr						kernel32	0
imp	'IsCalendarLeapDay'					IsCalendarLeapDay					kernel32	0
imp	'IsCalendarLeapMonth'					IsCalendarLeapMonth					kernel32	0
imp	'IsCalendarLeapYear'					IsCalendarLeapYear					kernel32	0
imp	'IsDBCSLeadByte'					IsDBCSLeadByte						kernel32	0
imp	'IsDBCSLeadByteEx'					IsDBCSLeadByteEx					kernel32	0
imp	'IsNLSDefinedString'					IsNLSDefinedString					kernel32	0
imp	'IsNativeVhdBoot'					IsNativeVhdBoot						kernel32	0
imp	'IsNormalizedString'					IsNormalizedString					kernel32	0
imp	'IsProcessCritical'					IsProcessCritical					kernel32	0
imp	'IsProcessInJob'					IsProcessInJob						kernel32	0
imp	'IsProcessorFeaturePresent'				IsProcessorFeaturePresent				kernel32	0
imp	'IsSystemResumeAutomatic'				IsSystemResumeAutomatic					kernel32	0
imp	'IsThreadAFiber'					IsThreadAFiber						kernel32	0
imp	'IsValidCalDateTime'					IsValidCalDateTime					kernel32	0
imp	'IsValidCodePage'					IsValidCodePage						kernel32	0
imp	'IsValidLanguageGroup'					IsValidLanguageGroup					kernel32	0
imp	'IsValidLocale'						IsValidLocale						kernel32	0
imp	'IsValidLocaleName'					IsValidLocaleName					kernel32	0
imp	'IsValidNLSVersion'					IsValidNLSVersion					kernel32	0
imp	'LCIDToLocaleName'					LCIDToLocaleName					kernel32	0
imp	'LCMapString'						LCMapStringW						kernel32	0
imp	'LCMapStringEx'						LCMapStringEx						kernel32	0
imp	'LZClose'						LZClose							kernel32	0
imp	'LZCloseFile'						LZCloseFile						kernel32	0
imp	'LZCopy'						LZCopy							kernel32	0
imp	'LZCreateFile'						LZCreateFileW						kernel32	0
imp	'LZDone'						LZDone							kernel32	0
imp	'LZInit'						LZInit							kernel32	0
imp	'LZOpenFile'						LZOpenFileW						kernel32	0
imp	'LZRead'						LZRead							kernel32	0
imp	'LZSeek'						LZSeek							kernel32	0
imp	'LZStart'						LZStart							kernel32	0
imp	'LeaveCriticalSection'					LeaveCriticalSection					kernel32	0	1
imp	'LoadLibrary'						LoadLibraryW						kernel32	0	1
imp	'LoadLibraryEx'						LoadLibraryExW						kernel32	0	3
imp	'LoadModule'						LoadModule						kernel32	0
imp	'LoadPackagedLibrary'					LoadPackagedLibrary					kernel32	0
imp	'LoadResource'						LoadResource						kernel32	0	2
imp	'LoadStringBase'					LoadStringBaseW						kernel32	0
imp	'LocalAlloc'						LocalAlloc						kernel32	0
imp	'LocalCompact'						LocalCompact						kernel32	0
imp	'LocalFileTimeToFileTime'				LocalFileTimeToFileTime					kernel32	0
imp	'LocalFlags'						LocalFlags						kernel32	0
imp	'LocalFree'						LocalFree						kernel32	0	1
imp	'LocalHandle'						LocalHandle						kernel32	0
imp	'LocalLock'						LocalLock						kernel32	0
imp	'LocalReAlloc'						LocalReAlloc						kernel32	0
imp	'LocalShrink'						LocalShrink						kernel32	0
imp	'LocalSize'						LocalSize						kernel32	0
imp	'LocalUnlock'						LocalUnlock						kernel32	0
imp	'LocaleNameToLCID'					LocaleNameToLCID					kernel32	0
imp	'LocateXStateFeature'					LocateXStateFeature					kernel32	0
imp	'LockFile'						LockFile						kernel32	0	5
imp	'LockResource'						LockResource						kernel32	0	1
imp	'MapUserPhysicalPages'					MapUserPhysicalPages					kernel32	0
imp	'MapUserPhysicalPagesScatter'				MapUserPhysicalPagesScatter				kernel32	0
imp	'MapViewOfFile'						MapViewOfFile						kernel32	0
imp	'MapViewOfFileFromApp'					MapViewOfFileFromApp					kernel32	0
imp	'Module32First'						Module32FirstW						kernel32	0
imp	'Module32Next'						Module32NextW						kernel32	0
imp	'MoveFile'						MoveFileW						kernel32	0	2
imp	'MoveFileTransacted'					MoveFileTransactedW					kernel32	0
imp	'MoveFileWithProgress'					MoveFileWithProgressW					kernel32	0
imp	'MulDiv'						MulDiv							kernel32	0
imp	'MultiByteToWideChar'					MultiByteToWideChar					kernel32	0	6
imp	'NeedCurrentDirectoryForExePath'			NeedCurrentDirectoryForExePathW				kernel32	0
imp	'NormalizeString'					NormalizeString						kernel32	0
imp	'NotifyUILanguageChange'				NotifyUILanguageChange					kernel32	0
imp	'OOBEComplete'						OOBEComplete						kernel32	0
imp	'OfferVirtualMemory'					OfferVirtualMemory					kernel32	0	3
imp	'OpenConsole'						OpenConsoleW						kernel32	0
imp	'OpenConsoleWStub'					OpenConsoleWStub					kernel32	0
imp	'OpenEvent'						OpenEventW						kernel32	0
imp	'OpenFile'						OpenFile						kernel32	0
imp	'OpenFileById'						OpenFileById						kernel32	0
imp	'OpenFileMapping'					OpenFileMappingW					kernel32	0
imp	'OpenJobObject'						OpenJobObjectW						kernel32	0
imp	'OpenMutex'						OpenMutexW						kernel32	0
imp	'OpenPackageInfoByFullName'				OpenPackageInfoByFullName				kernel32	0
imp	'OpenPrivateNamespace'					OpenPrivateNamespaceW					kernel32	0
imp	'OpenProfileUserMapping'				OpenProfileUserMapping					kernel32	0
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
imp	'PowerClearRequest'					PowerClearRequest					kernel32	0
imp	'PowerCreateRequest'					PowerCreateRequest					kernel32	0
imp	'PowerSetRequest'					PowerSetRequest						kernel32	0
imp	'PrefetchVirtualMemory'					PrefetchVirtualMemory					kernel32	0	4
imp	'PrepareTape'						PrepareTape						kernel32	0
imp	'PrivMoveFileIdentity'					PrivMoveFileIdentityW					kernel32	0
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
imp	'PssWalkMarkerRewind'					PssWalkMarkerRewind					kernel32	0
imp	'PssWalkMarkerSeek'					PssWalkMarkerSeek					kernel32	0
imp	'PssWalkMarkerSeekToBeginning'				PssWalkMarkerSeekToBeginning				kernel32	0
imp	'PssWalkMarkerSetPosition'				PssWalkMarkerSetPosition				kernel32	0
imp	'PssWalkMarkerTell'					PssWalkMarkerTell					kernel32	0
imp	'PssWalkSnapshot'					PssWalkSnapshot						kernel32	0
imp	'PulseEvent'						PulseEvent						kernel32	0	1
imp	'PurgeComm'						PurgeComm						kernel32	0	2
imp	'QueryActCtx'						QueryActCtxW						kernel32	0
imp	'QueryActCtxSettings'					QueryActCtxSettingsW					kernel32	0
imp	'QueryActCtxSettingsWWorker'				QueryActCtxSettingsWWorker				kernel32	0
imp	'QueryActCtxWWorker'					QueryActCtxWWorker					kernel32	0
imp	'QueryDosDevice'					QueryDosDeviceW						kernel32	0
imp	'QueryFullProcessImageName'				QueryFullProcessImageNameW				kernel32	0
imp	'QueryIdleProcessorCycleTime'				QueryIdleProcessorCycleTime				kernel32	0
imp	'QueryIdleProcessorCycleTimeEx'				QueryIdleProcessorCycleTimeEx				kernel32	0
imp	'QueryInformationJobObject'				QueryInformationJobObject				kernel32	0
imp	'QueryIoRateControlInformationJobObject'		QueryIoRateControlInformationJobObject			kernel32	0
imp	'QueryMemoryResourceNotification'			QueryMemoryResourceNotification				kernel32	0
imp	'QueryPerformanceCounter'				QueryPerformanceCounter					kernel32	0	1
imp	'QueryPerformanceFrequency'				QueryPerformanceFrequency				kernel32	0	1
imp	'QueryProcessAffinityUpdateMode'			QueryProcessAffinityUpdateMode				kernel32	0
imp	'QueryProcessCycleTime'					QueryProcessCycleTime					kernel32	0
imp	'QueryProtectedPolicy'					QueryProtectedPolicy					kernel32	0
imp	'QueryThreadCycleTime'					QueryThreadCycleTime					kernel32	0
imp	'QueryThreadProfiling'					QueryThreadProfiling					kernel32	0
imp	'QueryThreadpoolStackInformation'			QueryThreadpoolStackInformation				kernel32	0
imp	'QueryUmsThreadInformation'				QueryUmsThreadInformation				kernel32	0
imp	'QueryUnbiasedInterruptTime'				QueryUnbiasedInterruptTime				kernel32	0
imp	'QueueUserAPC'						QueueUserAPC						kernel32	0
imp	'QueueUserWorkItem'					QueueUserWorkItem					kernel32	0
imp	'QuirkGetData2Worker'					QuirkGetData2Worker					kernel32	0
imp	'QuirkGetDataWorker'					QuirkGetDataWorker					kernel32	0
imp	'QuirkIsEnabled2Worker'					QuirkIsEnabled2Worker					kernel32	0
imp	'QuirkIsEnabled3Worker'					QuirkIsEnabled3Worker					kernel32	0
imp	'QuirkIsEnabledForPackage2Worker'			QuirkIsEnabledForPackage2Worker				kernel32	0
imp	'QuirkIsEnabledForPackage3Worker'			QuirkIsEnabledForPackage3Worker				kernel32	0
imp	'QuirkIsEnabledForPackage4Worker'			QuirkIsEnabledForPackage4Worker				kernel32	0
imp	'QuirkIsEnabledForPackageWorker'			QuirkIsEnabledForPackageWorker				kernel32	0
imp	'QuirkIsEnabledForProcessWorker'			QuirkIsEnabledForProcessWorker				kernel32	0
imp	'QuirkIsEnabledWorker'					QuirkIsEnabledWorker					kernel32	0
imp	'RaiseException'					RaiseException						kernel32	0
imp	'RaiseFailFastException'				RaiseFailFastException					kernel32	0
imp	'RaiseInvalid16BitExeError'				RaiseInvalid16BitExeError				kernel32	0
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
imp	'ReadThreadProfilingData'				ReadThreadProfilingData					kernel32	0
imp	'ReclaimVirtualMemory'					ReclaimVirtualMemory					kernel32	0
imp	'RegDisablePredefinedCacheEx'				RegDisablePredefinedCacheEx				kernel32	0
imp	'RegisterApplicationRecoveryCallback'			RegisterApplicationRecoveryCallback			kernel32	0
imp	'RegisterApplicationRestart'				RegisterApplicationRestart				kernel32	0
imp	'RegisterBadMemoryNotification'				RegisterBadMemoryNotification				kernel32	0
imp	'RegisterConsoleIME'					RegisterConsoleIME					kernel32	0
imp	'RegisterConsoleOS2'					RegisterConsoleOS2					kernel32	0
imp	'RegisterConsoleVDM'					RegisterConsoleVDM					kernel32	0
imp	'RegisterWaitForInputIdle'				RegisterWaitForInputIdle				kernel32	0
imp	'RegisterWaitForSingleObject'				RegisterWaitForSingleObject				kernel32	0	6
imp	'RegisterWaitUntilOOBECompleted'			RegisterWaitUntilOOBECompleted				kernel32	0
imp	'RegisterWowBaseHandlers'				RegisterWowBaseHandlers					kernel32	0
imp	'RegisterWowExec'					RegisterWowExec						kernel32	0
imp	'ReleaseActCtx'						ReleaseActCtx						kernel32	0
imp	'ReleaseActCtxWorker'					ReleaseActCtxWorker					kernel32	0
imp	'ReleaseMutex'						ReleaseMutex						kernel32	0	1
imp	'ReleaseSRWLockExclusive'				ReleaseSRWLockExclusive					kernel32	0	1
imp	'ReleaseSRWLockShared'					ReleaseSRWLockShared					kernel32	0	1
imp	'ReleaseSemaphore'					ReleaseSemaphore					kernel32	0	3
imp	'RemoveDirectoryTransacted'				RemoveDirectoryTransactedW				kernel32	0
imp	'RemoveDllDirectory'					RemoveDllDirectory					kernel32	0
imp	'RemoveLocalAlternateComputerName'			RemoveLocalAlternateComputerNameW			kernel32	0
imp	'RemoveSecureMemoryCacheCallback'			RemoveSecureMemoryCacheCallback				kernel32	0
imp	'RemoveVectoredContinueHandler'				RemoveVectoredContinueHandler				kernel32	0	1
imp	'RemoveVectoredExceptionHandler'			RemoveVectoredExceptionHandler				kernel32	0	1
imp	'ReplaceFile'						ReplaceFileW						kernel32	0
imp	'ReplacePartitionUnit'					ReplacePartitionUnit					kernel32	0
imp	'RequestDeviceWakeup'					RequestDeviceWakeup					kernel32	0
imp	'RequestWakeupLatency'					RequestWakeupLatency					kernel32	0
imp	'ResetEvent'						ResetEvent						kernel32	0	1
imp	'ResetWriteWatch'					ResetWriteWatch						kernel32	0
imp	'ResolveDelayLoadedAPI'					ResolveDelayLoadedAPI					kernel32	0
imp	'ResolveDelayLoadsFromDll'				ResolveDelayLoadsFromDll				kernel32	0
imp	'ResolveLocaleName'					ResolveLocaleName					kernel32	0
imp	'ResumeThread'						ResumeThread						kernel32	0
imp	'ScrollConsoleScreenBuffer'				ScrollConsoleScreenBufferW				kernel32	0
imp	'SearchPath'						SearchPathW						kernel32	0
imp	'SetCalendarInfo'					SetCalendarInfoW					kernel32	0
imp	'SetComPlusPackageInstallStatus'			SetComPlusPackageInstallStatus				kernel32	0
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
imp	'SetConsoleCursor'					SetConsoleCursor					kernel32	0
imp	'SetConsoleCursorInfo'					SetConsoleCursorInfo					kernel32	0	2
imp	'SetConsoleCursorMode'					SetConsoleCursorMode					kernel32	0
imp	'SetConsoleCursorPosition'				SetConsoleCursorPosition				kernel32	0	2
imp	'SetConsoleDisplayMode'					SetConsoleDisplayMode					kernel32	0
imp	'SetConsoleFont'					SetConsoleFont						kernel32	0
imp	'SetConsoleHardwareState'				SetConsoleHardwareState					kernel32	0
imp	'SetConsoleHistoryInfo'					SetConsoleHistoryInfo					kernel32	0
imp	'SetConsoleIcon'					SetConsoleIcon						kernel32	0
imp	'SetConsoleKeyShortcuts'				SetConsoleKeyShortcuts					kernel32	0
imp	'SetConsoleLocalEUDC'					SetConsoleLocalEUDC					kernel32	0
imp	'SetConsoleMaximumWindowSize'				SetConsoleMaximumWindowSize				kernel32	0
imp	'SetConsoleMenuClose'					SetConsoleMenuClose					kernel32	0
imp	'SetConsoleMode'					SetConsoleMode						kernel32	0	2
imp	'SetConsoleNlsMode'					SetConsoleNlsMode					kernel32	0
imp	'SetConsoleOS2OemFormat'				SetConsoleOS2OemFormat					kernel32	0
imp	'SetConsoleOutputCP'					SetConsoleOutputCP					kernel32	0	1
imp	'SetConsolePalette'					SetConsolePalette					kernel32	0
imp	'SetConsoleScreenBufferInfoEx'				SetConsoleScreenBufferInfoEx				kernel32	0	2
imp	'SetConsoleScreenBufferSize'				SetConsoleScreenBufferSize				kernel32	0	2
imp	'SetConsoleTextAttribute'				SetConsoleTextAttribute					kernel32	0
imp	'SetConsoleTitle'					SetConsoleTitleW					kernel32	0	1
imp	'SetConsoleWindowInfo'					SetConsoleWindowInfo					kernel32	0	3
imp	'SetCriticalSectionSpinCount'				SetCriticalSectionSpinCount				kernel32	0	2
imp	'SetCurrentConsoleFontEx'				SetCurrentConsoleFontEx					kernel32	0
imp	'SetDefaultCommConfig'					SetDefaultCommConfigW					kernel32	0
imp	'SetDefaultDllDirectories'				SetDefaultDllDirectories				kernel32	0	1	# Windows 8+, KB2533623 on Windows 7
imp	'SetDllDirectory'					SetDllDirectoryW					kernel32	0
imp	'SetDynamicTimeZoneInformation'				SetDynamicTimeZoneInformation				kernel32	0
imp	'SetEndOfFile'						SetEndOfFile						kernel32	0	1
imp	'SetEnvironmentVariable'				SetEnvironmentVariableW					kernel32	0	2
imp	'SetErrorMode'						SetErrorMode						kernel32	0	1
imp	'SetEvent'						SetEvent						kernel32	0	1
imp	'SetFileApisToANSI'					SetFileApisToANSI					kernel32	0
imp	'SetFileApisToOEM'					SetFileApisToOEM					kernel32	0
imp	'SetFileAttributes'					SetFileAttributesW					kernel32	0	2
imp	'SetFileAttributesTransacted'				SetFileAttributesTransactedW				kernel32	0
imp	'SetFileBandwidthReservation'				SetFileBandwidthReservation				kernel32	0
imp	'SetFileCompletionNotificationModes'			SetFileCompletionNotificationModes			kernel32	0	2
imp	'SetFileInformationByHandle'				SetFileInformationByHandle				kernel32	0
imp	'SetFileIoOverlappedRange'				SetFileIoOverlappedRange				kernel32	0
imp	'SetFilePointer'					SetFilePointer						kernel32	0	4
imp	'SetFilePointerEx'					SetFilePointerEx					kernel32	0	4
imp	'SetFileShortName'					SetFileShortNameW					kernel32	0
imp	'SetFileTime'						SetFileTime						kernel32	0	4
imp	'SetFileValidData'					SetFileValidData					kernel32	0	2
imp	'SetFirmwareEnvironmentVariable'			SetFirmwareEnvironmentVariableW				kernel32	0
imp	'SetFirmwareEnvironmentVariableEx'			SetFirmwareEnvironmentVariableExW			kernel32	0
imp	'SetHandleCount'					SetHandleCount						kernel32	0	1
imp	'SetHandleInformation'					SetHandleInformation					kernel32	0	3
imp	'SetInformationJobObject'				SetInformationJobObject					kernel32	0
imp	'SetIoRateControlInformationJobObject'			SetIoRateControlInformationJobObject			kernel32	0
imp	'SetLastError'						SetLastError						kernel32	0	1
imp	'SetLocalPrimaryComputerName'				SetLocalPrimaryComputerNameW				kernel32	0
imp	'SetLocalTime'						SetLocalTime						kernel32	0
imp	'SetLocaleInfo'						SetLocaleInfoW						kernel32	0
imp	'SetMailslotInfo'					SetMailslotInfo						kernel32	0
imp	'SetMessageWaitingIndicator'				SetMessageWaitingIndicator				kernel32	0
imp	'SetNamedPipeAttribute'					SetNamedPipeAttribute					kernel32	0
imp	'SetNamedPipeHandleState'				SetNamedPipeHandleState					kernel32	0	4
imp	'SetPriorityClass'					SetPriorityClass					kernel32	0	2
imp	'SetProcessAffinityMask'				SetProcessAffinityMask					kernel32	0	2
imp	'SetProcessAffinityUpdateMode'				SetProcessAffinityUpdateMode				kernel32	0
imp	'SetProcessDEPPolicy'					SetProcessDEPPolicy					kernel32	0
imp	'SetProcessDefaultCpuSets'				SetProcessDefaultCpuSets				kernel32	0
imp	'SetProcessInformation'					SetProcessInformation					kernel32	0
imp	'SetProcessMitigationPolicy'				SetProcessMitigationPolicy				kernel32	0
imp	'SetProcessPreferredUILanguages'			SetProcessPreferredUILanguages				kernel32	0
imp	'SetProcessPriorityBoost'				SetProcessPriorityBoost					kernel32	0	2
imp	'SetProcessShutdownParameters'				SetProcessShutdownParameters				kernel32	0
imp	'SetProcessWorkingSetSize'				SetProcessWorkingSetSize				kernel32	0	3
imp	'SetProcessWorkingSetSizeEx'				SetProcessWorkingSetSizeEx				kernel32	0	4
imp	'SetProtectedPolicy'					SetProtectedPolicy					kernel32	0
imp	'SetSearchPathMode'					SetSearchPathMode					kernel32	0
imp	'SetStdHandle'						SetStdHandle						kernel32	0	2
imp	'SetSystemFileCacheSize'				SetSystemFileCacheSize					kernel32	0
imp	'SetSystemPowerState'					SetSystemPowerState					kernel32	0
imp	'SetSystemTime'						SetSystemTime						kernel32	0
imp	'SetSystemTimeAdjustment'				SetSystemTimeAdjustment					kernel32	0
imp	'SetTapeParameters'					SetTapeParameters					kernel32	0
imp	'SetTapePosition'					SetTapePosition						kernel32	0
imp	'SetTermsrvAppInstallMode'				SetTermsrvAppInstallMode				kernel32	0
imp	'SetThreadAffinityMask'					SetThreadAffinityMask					kernel32	0	2
imp	'SetThreadContext'					SetThreadContext					kernel32	0
imp	'SetThreadErrorMode'					SetThreadErrorMode					kernel32	0
imp	'SetThreadExecutionState'				SetThreadExecutionState					kernel32	0
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
imp	'SetTimerQueueTimer'					SetTimerQueueTimer					kernel32	0
imp	'SetUmsThreadInformation'				SetUmsThreadInformation					kernel32	0
imp	'SetUnhandledExceptionFilter'				SetUnhandledExceptionFilter				kernel32	0	1
imp	'SetUserGeoID'						SetUserGeoID						kernel32	0
imp	'SetVDMCurrentDirectories'				SetVDMCurrentDirectories				kernel32	0
imp	'SetVolumeLabel'					SetVolumeLabelW						kernel32	0
imp	'SetVolumeMountPoint'					SetVolumeMountPointW					kernel32	0
imp	'SetVolumeMountPointWStub'				SetVolumeMountPointWStub				kernel32	0
imp	'SetWaitableTimer'					SetWaitableTimer					kernel32	0	6
imp	'SetWaitableTimerEx'					SetWaitableTimerEx					kernel32	0
imp	'SetXStateFeaturesMask'					SetXStateFeaturesMask					kernel32	0
imp	'SetupComm'						SetupComm						kernel32	0
imp	'ShowConsoleCursor'					ShowConsoleCursor					kernel32	0
imp	'SignalObjectAndWait'					SignalObjectAndWait					kernel32	0
imp	'SizeofResource'					SizeofResource						kernel32	0
imp	'Sleep'							Sleep							kernel32	0	1
imp	'SleepConditionVariableCS'				SleepConditionVariableCS				kernel32	0
imp	'SleepConditionVariableSRW'				SleepConditionVariableSRW				kernel32	0
imp	'SleepEx'						SleepEx							kernel32	0	2
imp	'SortCloseHandle'					SortCloseHandle						kernel32	0
imp	'SortGetHandle'						SortGetHandle						kernel32	0
imp	'SuspendThread'						SuspendThread						kernel32	0	1
imp	'SwitchToFiber'						SwitchToFiber						kernel32	0
imp	'SwitchToThread'					SwitchToThread						kernel32	0
imp	'SystemTimeToFileTime'					SystemTimeToFileTime					kernel32	0	2
imp	'SystemTimeToTzSpecificLocalTime'			SystemTimeToTzSpecificLocalTime				kernel32	0
imp	'SystemTimeToTzSpecificLocalTimeEx'			SystemTimeToTzSpecificLocalTimeEx			kernel32	0
imp	'TerminateJobObject'					TerminateJobObject					kernel32	0
imp	'TerminateThread'					TerminateThread						kernel32	0	2
imp	'TermsrvAppInstallMode'					TermsrvAppInstallMode					kernel32	0
imp	'TermsrvConvertSysRootToUserDir'			TermsrvConvertSysRootToUserDir				kernel32	0
imp	'TermsrvCreateRegEntry'					TermsrvCreateRegEntry					kernel32	0
imp	'TermsrvDeleteKey'					TermsrvDeleteKey					kernel32	0
imp	'TermsrvDeleteValue'					TermsrvDeleteValue					kernel32	0
imp	'TermsrvGetPreSetValue'					TermsrvGetPreSetValue					kernel32	0
imp	'TermsrvGetWindowsDirectory'				TermsrvGetWindowsDirectoryW				kernel32	0
imp	'TermsrvOpenRegEntry'					TermsrvOpenRegEntry					kernel32	0
imp	'TermsrvOpenUserClasses'				TermsrvOpenUserClasses					kernel32	0
imp	'TermsrvRestoreKey'					TermsrvRestoreKey					kernel32	0
imp	'TermsrvSetKeySecurity'					TermsrvSetKeySecurity					kernel32	0
imp	'TermsrvSetValueKey'					TermsrvSetValueKey					kernel32	0
imp	'TermsrvSyncUserIniFileExt'				TermsrvSyncUserIniFileExt				kernel32	0
imp	'Thread32First'						Thread32First						kernel32	0
imp	'Thread32Next'						Thread32Next						kernel32	0
imp	'TlsAlloc'						TlsAlloc						kernel32	0	0
imp	'TlsFree'						TlsFree							kernel32	0	1
imp	'TlsGetValue'						TlsGetValue						kernel32	0	1
imp	'TlsSetValue'						TlsSetValue						kernel32	0	2
imp	'Toolhelp32ReadProcessMemory'				Toolhelp32ReadProcessMemory				kernel32	0
imp	'TransactNamedPipe'					TransactNamedPipe					kernel32	0	7
imp	'TransmitCommChar'					TransmitCommChar					kernel32	0	2
imp	'TryAcquireSRWLockExclusive'				TryAcquireSRWLockExclusive				kernel32	0	1
imp	'TryAcquireSRWLockShared'				TryAcquireSRWLockShared					kernel32	0	1
imp	'TryEnterCriticalSection'				TryEnterCriticalSection					kernel32	0	1
imp	'TrySubmitThreadpoolCallback'				TrySubmitThreadpoolCallback				kernel32	0
imp	'TzSpecificLocalTimeToSystemTime'			TzSpecificLocalTimeToSystemTime				kernel32	0
imp	'TzSpecificLocalTimeToSystemTimeEx'			TzSpecificLocalTimeToSystemTimeEx			kernel32	0
imp	'UTRegister'						UTRegister						kernel32	0
imp	'UTUnRegister'						UTUnRegister						kernel32	0
imp	'UmsThreadYield'					UmsThreadYield						kernel32	0
imp	'UnhandledExceptionFilter'				UnhandledExceptionFilter				kernel32	0
imp	'UnlockFile'						UnlockFile						kernel32	0	5
imp	'UnmapViewOfFile2'					UnmapViewOfFile2					kernel32	0	2
imp	'UnmapViewOfFileEx'					UnmapViewOfFileEx					kernel32	0	3
imp	'UnregisterApplicationRecoveryCallback'			UnregisterApplicationRecoveryCallback			kernel32	0
imp	'UnregisterApplicationRestart'				UnregisterApplicationRestart				kernel32	0
imp	'UnregisterBadMemoryNotification'			UnregisterBadMemoryNotification				kernel32	0
imp	'UnregisterConsoleIME'					UnregisterConsoleIME					kernel32	0
imp	'UnregisterWait'					UnregisterWait						kernel32	0
imp	'UnregisterWaitEx'					UnregisterWaitEx					kernel32	0
imp	'UnregisterWaitUntilOOBECompleted'			UnregisterWaitUntilOOBECompleted			kernel32	0
imp	'UpdateCalendarDayOfWeek'				UpdateCalendarDayOfWeek					kernel32	0
imp	'UpdateProcThreadAttribute'				UpdateProcThreadAttribute				kernel32	0	7
imp	'UpdateResource'					UpdateResourceW						kernel32	0
imp	'VDMConsoleOperation'					VDMConsoleOperation					kernel32	0
imp	'VDMOperationStarted'					VDMOperationStarted					kernel32	0
imp	'VerLanguageName'					VerLanguageNameW					kernel32	0
imp	'VerifyConsoleIoHandle'					VerifyConsoleIoHandle					kernel32	0
imp	'VerifyScripts'						VerifyScripts						kernel32	0
imp	'VerifyVersionInfo'					VerifyVersionInfoW					kernel32	0
imp	'VirtualAlloc'						VirtualAlloc						kernel32	0	4
imp	'VirtualAllocEx'					VirtualAllocEx						kernel32	0	5
imp	'VirtualAllocExNuma'					VirtualAllocExNuma					kernel32	0
imp	'VirtualFree'						VirtualFree						kernel32	0	3
imp	'VirtualFreeEx'						VirtualFreeEx						kernel32	0
imp	'VirtualLock'						VirtualLock						kernel32	0	2
imp	'VirtualUnlock'						VirtualUnlock						kernel32	0	2
imp	'VirtualProtectEx'					VirtualProtectEx					kernel32	0
imp	'VirtualQuery'						VirtualQuery						kernel32	0	3
imp	'VirtualQueryEx'					VirtualQueryEx						kernel32	0
imp	'WTSGetActiveConsoleSessionId'				WTSGetActiveConsoleSessionId				kernel32	0
imp	'WaitCommEvent'						WaitCommEvent						kernel32	0
imp	'WaitForDebugEvent'					WaitForDebugEvent					kernel32	0
imp	'WaitForMultipleObjectsEx'				WaitForMultipleObjectsEx				kernel32	0	5
imp	'WaitForSingleObjectEx'					WaitForSingleObjectEx					kernel32	0	3
imp	'WaitNamedPipe'						WaitNamedPipeW						kernel32	0
imp	'WerGetFlags'						WerGetFlags						kernel32	0
imp	'WerGetFlagsWorker'					WerGetFlagsWorker					kernel32	0
imp	'WerRegisterFile'					WerRegisterFile						kernel32	0
imp	'WerRegisterFileWorker'					WerRegisterFileWorker					kernel32	0
imp	'WerRegisterMemoryBlock'				WerRegisterMemoryBlock					kernel32	0
imp	'WerRegisterMemoryBlockWorker'				WerRegisterMemoryBlockWorker				kernel32	0
imp	'WerRegisterRuntimeExceptionModule'			WerRegisterRuntimeExceptionModule			kernel32	0
imp	'WerRegisterRuntimeExceptionModuleWorker'		WerRegisterRuntimeExceptionModuleWorker			kernel32	0
imp	'WerSetFlags'						WerSetFlags						kernel32	0
imp	'WerSetFlagsWorker'					WerSetFlagsWorker					kernel32	0
imp	'WerUnregisterFile'					WerUnregisterFile					kernel32	0
imp	'WerUnregisterFileWorker'				WerUnregisterFileWorker					kernel32	0
imp	'WerUnregisterMemoryBlock'				WerUnregisterMemoryBlock				kernel32	0
imp	'WerUnregisterMemoryBlockWorker'			WerUnregisterMemoryBlockWorker				kernel32	0
imp	'WerUnregisterRuntimeExceptionModule'			WerUnregisterRuntimeExceptionModule			kernel32	0
imp	'WerUnregisterRuntimeExceptionModuleWorker'		WerUnregisterRuntimeExceptionModuleWorker		kernel32	0
imp	'WerpGetDebugger'					WerpGetDebugger						kernel32	0
imp	'WerpInitiateRemoteRecovery'				WerpInitiateRemoteRecovery				kernel32	0
imp	'WerpLaunchAeDebug'					WerpLaunchAeDebug					kernel32	0
imp	'WerpNotifyLoadStringResourceWorker'			WerpNotifyLoadStringResourceWorker			kernel32	0
imp	'WerpNotifyUseStringResourceWorker'			WerpNotifyUseStringResourceWorker			kernel32	0
imp	'WideCharToMultiByte'					WideCharToMultiByte					kernel32	0    8
imp	'WinExec'						WinExec							kernel32	0
imp	'WriteConsole'						WriteConsoleW						kernel32	0	5
imp	'WriteConsoleInput'					WriteConsoleInputW					kernel32	0	4
imp	'WriteConsoleInputVDMW'					WriteConsoleInputVDMW					kernel32	0
imp	'WriteConsoleOutput'					WriteConsoleOutputW					kernel32	0
imp	'WriteConsoleOutputAttribute'				WriteConsoleOutputAttribute				kernel32	0	5
imp	'WriteConsoleOutputCharacter'				WriteConsoleOutputCharacterW				kernel32	0	5
imp	'WriteFile'						WriteFile						kernel32	0	5
imp	'WriteFileEx'						WriteFileEx						kernel32	0	5
imp	'WriteFileGather'					WriteFileGather						kernel32	0	5
imp	'WritePrivateProfileSection'				WritePrivateProfileSectionW				kernel32	0
imp	'WritePrivateProfileString'				WritePrivateProfileStringW				kernel32	0
imp	'WritePrivateProfileStruct'				WritePrivateProfileStructW				kernel32	0
imp	'WriteProcessMemory'					WriteProcessMemory					kernel32	0
imp	'WriteProfileSection'					WriteProfileSectionW					kernel32	0
imp	'WriteProfileString'					WriteProfileStringW					kernel32	0
imp	'WriteTapemark'						WriteTapemark						kernel32	0
imp	'ZombifyActCtx'						ZombifyActCtx						kernel32	0
imp	'ZombifyActCtxWorker'					ZombifyActCtxWorker					kernel32	0
imp	'__CancelIoEx'						CancelIoEx						kernel32	0	2
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
imp	'AbortSystemShutdown'					AbortSystemShutdownW					advapi32	0
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
imp	'AddConditionalAce'					AddConditionalAce					advapi32	0 # Windows 7+
imp	'AddMandatoryAce'					AddMandatoryAce						advapi32	0
imp	'AddUsersToEncryptedFile'				AddUsersToEncryptedFile					advapi32	0
imp	'AdjustTokenGroups'					AdjustTokenGroups					advapi32	0
imp	'AdjustTokenPrivileges'					AdjustTokenPrivileges					advapi32	0	6
imp	'AllocateAndInitializeSid'				AllocateAndInitializeSid				advapi32	0
imp	'AllocateLocallyUniqueId'				AllocateLocallyUniqueId					advapi32	0
imp	'AreAllAccessesGranted'					AreAllAccessesGranted					advapi32	0
imp	'AreAnyAccessesGranted'					AreAnyAccessesGranted					advapi32	0
imp	'AuditComputeEffectivePolicyBySid'			AuditComputeEffectivePolicyBySid			advapi32	0
imp	'AuditComputeEffectivePolicyByToken'			AuditComputeEffectivePolicyByToken			advapi32	0
imp	'AuditEnumerateCategories'				AuditEnumerateCategories				advapi32	0
imp	'AuditEnumeratePerUserPolicy'				AuditEnumeratePerUserPolicy				advapi32	0
imp	'AuditEnumerateSubCategories'				AuditEnumerateSubCategories				advapi32	0
imp	'AuditFree'						AuditFree						advapi32	0
imp	'AuditLookupCategoryGuidFromCategoryId'			AuditLookupCategoryGuidFromCategoryId			advapi32	0
imp	'AuditLookupCategoryIdFromCategoryGuid'			AuditLookupCategoryIdFromCategoryGuid			advapi32	0
imp	'AuditLookupCategoryName'				AuditLookupCategoryNameW				advapi32	0
imp	'AuditLookupSubCategoryName'				AuditLookupSubCategoryNameW				advapi32	0
imp	'AuditQueryPerUserPolicy'				AuditQueryPerUserPolicy					advapi32	0
imp	'AuditQuerySecurity'					AuditQuerySecurity					advapi32	0
imp	'AuditQuerySystemPolicy'				AuditQuerySystemPolicy					advapi32	0
imp	'AuditSetPerUserPolicy'					AuditSetPerUserPolicy					advapi32	0
imp	'AuditSetSecurity'					AuditSetSecurity					advapi32	0
imp	'AuditSetSystemPolicy'					AuditSetSystemPolicy					advapi32	0
imp	'BackupEventLog'					BackupEventLogW						advapi32	0
imp	'BuildExplicitAccessWithName'				BuildExplicitAccessWithNameW				advapi32	0
imp	'BuildImpersonateExplicitAccessWithName'		BuildImpersonateExplicitAccessWithNameW			advapi32	0
imp	'BuildImpersonateTrustee'				BuildImpersonateTrusteeW				advapi32	0
imp	'BuildSecurityDescriptor'				BuildSecurityDescriptorW				advapi32	0
imp	'BuildTrusteeWithName'					BuildTrusteeWithNameW					advapi32	0
imp	'BuildTrusteeWithObjectsAndName'			BuildTrusteeWithObjectsAndNameW				advapi32	0
imp	'BuildTrusteeWithObjectsAndSid'				BuildTrusteeWithObjectsAndSidW				advapi32	0
imp	'BuildTrusteeWithSid'					BuildTrusteeWithSidW					advapi32	0
imp	'ChangeServiceConfig'					ChangeServiceConfigW					advapi32	0
imp	'ChangeServiceConfig2W'					ChangeServiceConfig2W					advapi32	0
imp	'CheckTokenMembership'					CheckTokenMembership					advapi32	0
imp	'ClearEventLog'						ClearEventLogW						advapi32	0
imp	'CloseEncryptedFileRaw'					CloseEncryptedFileRaw					advapi32	0
imp	'CloseEventLog'						CloseEventLog						advapi32	0
imp	'CloseServiceHandle'					CloseServiceHandle					advapi32	0
imp	'CloseThreadWaitChainSession'				CloseThreadWaitChainSession				advapi32	0
imp	'CloseTrace'						CloseTrace						advapi32	0
imp	'ControlService'					ControlService						advapi32	0
imp	'ControlServiceEx'					ControlServiceExW					advapi32	0
imp	'ControlTrace'						ControlTraceW						advapi32	0
imp	'ConvertSecurityDescriptorToStringSecurityDescriptor'	ConvertSecurityDescriptorToStringSecurityDescriptorW	advapi32	0
imp	'ConvertSidToStringSid'					ConvertSidToStringSidW					advapi32	0
imp	'ConvertStringSDToSDDomain'				ConvertStringSDToSDDomainW				advapi32	0
imp	'ConvertStringSecurityDescriptorToSecurityDescriptor'	ConvertStringSecurityDescriptorToSecurityDescriptorW	advapi32	0
imp	'ConvertStringSidToSid'					ConvertStringSidToSidW					advapi32	0
imp	'ConvertToAutoInheritPrivateObjectSecurity'		ConvertToAutoInheritPrivateObjectSecurity		advapi32	0
imp	'CopySid'						CopySid							advapi32	0
imp	'CreatePrivateObjectSecurity'				CreatePrivateObjectSecurity				advapi32	0
imp	'CreatePrivateObjectSecurityEx'				CreatePrivateObjectSecurityEx				advapi32	0
imp	'CreatePrivateObjectSecurityWithMultipleInheritance'	CreatePrivateObjectSecurityWithMultipleInheritance	advapi32	0
imp	'CreateProcessAsUser'					CreateProcessAsUserW					advapi32	0	11
imp	'CreateProcessWithLogon'				CreateProcessWithLogonW					advapi32	0
imp	'CreateProcessWithToken'				CreateProcessWithTokenW					advapi32	0
imp	'CreateRestrictedToken'					CreateRestrictedToken					advapi32	0
imp	'CreateService'						CreateServiceW						advapi32	0
imp	'CreateTraceInstanceId'					CreateTraceInstanceId					advapi32	0
imp	'CreateWellKnownSid'					CreateWellKnownSid					advapi32	0
imp	'CredDelete'						CredDeleteW						advapi32	0
imp	'CredEnumerate'						CredEnumerateW						advapi32	0
imp	'CredFindBestCredential'				CredFindBestCredentialW					advapi32	0
imp	'CredFree'						CredFree						advapi32	0
imp	'CredGetSessionTypes'					CredGetSessionTypes					advapi32	0
imp	'CredGetTargetInfo'					CredGetTargetInfoW					advapi32	0
imp	'CredIsMarshaledCredential'				CredIsMarshaledCredentialW				advapi32	0
imp	'CredIsProtected'					CredIsProtectedW					advapi32	0
imp	'CredMarshalCredential'					CredMarshalCredentialW					advapi32	0
imp	'CredProtect'						CredProtectW						advapi32	0
imp	'CredRead'						CredReadW						advapi32	0
imp	'CredReadDomainCredentials'				CredReadDomainCredentialsW				advapi32	0
imp	'CredRename'						CredRenameW						advapi32	0
imp	'CredUnmarshalCredential'				CredUnmarshalCredentialW				advapi32	0
imp	'CredUnprotect'						CredUnprotectW						advapi32	0
imp	'CredWrite'						CredWriteW						advapi32	0
imp	'CredWriteDomainCredentials'				CredWriteDomainCredentialsW				advapi32	0
imp	'CryptAcquireContext'					CryptAcquireContextW					advapi32	0
imp	'CryptContextAddRef'					CryptContextAddRef					advapi32	0
imp	'CryptCreateHash'					CryptCreateHash						advapi32	0
imp	'CryptDecrypt'						CryptDecrypt						advapi32	0
imp	'CryptDeriveKey'					CryptDeriveKey						advapi32	0
imp	'CryptDestroyHash'					CryptDestroyHash					advapi32	0
imp	'CryptDestroyKey'					CryptDestroyKey						advapi32	0
imp	'CryptDuplicateHash'					CryptDuplicateHash					advapi32	0
imp	'CryptDuplicateKey'					CryptDuplicateKey					advapi32	0
imp	'CryptEncrypt'						CryptEncrypt						advapi32	0
imp	'CryptEnumProviderTypes'				CryptEnumProviderTypesW					advapi32	0
imp	'CryptEnumProviders'					CryptEnumProvidersW					advapi32	0
imp	'CryptExportKey'					CryptExportKey						advapi32	0
imp	'CryptGenKey'						CryptGenKey						advapi32	0
imp	'CryptGenRandom'					CryptGenRandom						advapi32	0
imp	'CryptGetDefaultProvider'				CryptGetDefaultProviderW				advapi32	0
imp	'CryptGetHashParam'					CryptGetHashParam					advapi32	0
imp	'CryptGetKeyParam'					CryptGetKeyParam					advapi32	0
imp	'CryptGetLocalKeyLimits'				CryptGetLocalKeyLimits					advapi32	0
imp	'CryptGetProvParam'					CryptGetProvParam					advapi32	0
imp	'CryptGetUserKey'					CryptGetUserKey						advapi32	0
imp	'CryptHashData'						CryptHashData						advapi32	0
imp	'CryptHashSessionKey'					CryptHashSessionKey					advapi32	0
imp	'CryptImportKey'					CryptImportKey						advapi32	0
imp	'CryptReleaseContext'					CryptReleaseContext					advapi32	0
imp	'CryptSetHashParam'					CryptSetHashParam					advapi32	0
imp	'CryptSetKeyParam'					CryptSetKeyParam					advapi32	0
imp	'CryptSetProvParam'					CryptSetProvParam					advapi32	0
imp	'CryptSetProvider'					CryptSetProviderW					advapi32	0
imp	'CryptSetProviderEx'					CryptSetProviderExW					advapi32	0
imp	'CryptSignHash'						CryptSignHashW						advapi32	0
imp	'CryptVerifySignature'					CryptVerifySignatureW					advapi32	0
imp	'DecryptFile'						DecryptFileW						advapi32	0
imp	'DeleteAce'						DeleteAce						advapi32	0
imp	'DeleteService'						DeleteService						advapi32	0
imp	'DeregisterEventSource'					DeregisterEventSource					advapi32	0    1
imp	'DestroyPrivateObjectSecurity'				DestroyPrivateObjectSecurity				advapi32	0
imp	'DuplicateEncryptionInfoFile'				DuplicateEncryptionInfoFile				advapi32	0
imp	'DuplicateToken'					DuplicateToken						advapi32	0	3
imp	'DuplicateTokenEx'					DuplicateTokenEx					advapi32	0	6
imp	'EnableTrace'						EnableTrace						advapi32	0
imp	'EnableTraceEx'						EnableTraceEx						advapi32	0
imp	'EnableTraceEx2'					EnableTraceEx2						advapi32	0 # Windows 7+
imp	'EncryptFile'						EncryptFileW						advapi32	0
imp	'EncryptionDisable'					EncryptionDisable					advapi32	0
imp	'EnumDependentServices'					EnumDependentServicesW					advapi32	0
imp	'EnumServicesStatus'					EnumServicesStatusW					advapi32	0
imp	'EnumServicesStatusEx'					EnumServicesStatusExW					advapi32	0
imp	'EnumerateTraceGuids'					EnumerateTraceGuids					advapi32	0
imp	'EnumerateTraceGuidsEx'					EnumerateTraceGuidsEx					advapi32	0
imp	'EqualDomainSid'					EqualDomainSid						advapi32	0
imp	'EqualPrefixSid'					EqualPrefixSid						advapi32	0
imp	'EqualSid'						EqualSid						advapi32	0
imp	'EventAccessControl'					EventAccessControl					advapi32	0
imp	'EventAccessQuery'					EventAccessQuery					advapi32	0
imp	'EventAccessRemove'					EventAccessRemove					advapi32	0
imp	'EventActivityIdControl'				EventActivityIdControl					advapi32	0
imp	'EventEnabled'						EventEnabled						advapi32	0
imp	'EventProviderEnabled'					EventProviderEnabled					advapi32	0
imp	'EventRegister'						EventRegister						advapi32	0
imp	'EventUnregister'					EventUnregister						advapi32	0
imp	'EventWrite'						EventWrite						advapi32	0
imp	'EventWriteEx'						EventWriteEx						advapi32	0
imp	'EventWriteString'					EventWriteString					advapi32	0
imp	'EventWriteTransfer'					EventWriteTransfer					advapi32	0
imp	'FileEncryptionStatus'					FileEncryptionStatusW					advapi32	0
imp	'FindFirstFreeAce'					FindFirstFreeAce					advapi32	0
imp	'FlushTrace'						FlushTraceW						advapi32	0
imp	'FreeEncryptionCertificateHashList'			FreeEncryptionCertificateHashList			advapi32	0
imp	'FreeInheritedFromArray'				FreeInheritedFromArray					advapi32	0
imp	'FreeSid'						FreeSid							advapi32	0
imp	'GetAce'						GetAce							advapi32	0
imp	'GetAclInformation'					GetAclInformation					advapi32	0
imp	'GetAuditedPermissionsFromAcl'				GetAuditedPermissionsFromAclW				advapi32	0
imp	'GetCurrentHwProfile'					GetCurrentHwProfileW					advapi32	0
imp	'GetEffectiveRightsFromAcl'				GetEffectiveRightsFromAclW				advapi32	0
imp	'GetEventLogInformation'				GetEventLogInformation					advapi32	0
imp	'GetExplicitEntriesFromAcl'				GetExplicitEntriesFromAclW				advapi32	0
imp	'GetFileSecurity'					GetFileSecurityW					advapi32	0	5
imp	'GetInheritanceSource'					GetInheritanceSourceW					advapi32	0
imp	'GetKernelObjectSecurity'				GetKernelObjectSecurity					advapi32	0
imp	'GetLengthSid'						GetLengthSid						advapi32	0
imp	'GetLocalManagedApplications'				GetLocalManagedApplications				advapi32	0
imp	'GetManagedApplicationCategories'			GetManagedApplicationCategories				advapi32	0
imp	'GetManagedApplications'				GetManagedApplications					advapi32	0
imp	'GetMultipleTrustee'					GetMultipleTrusteeW					advapi32	0
imp	'GetMultipleTrusteeOperation'				GetMultipleTrusteeOperationW				advapi32	0
imp	'GetNamedSecurityInfo'					GetNamedSecurityInfoW					advapi32	0
imp	'GetNumberOfEventLogRecords'				GetNumberOfEventLogRecords				advapi32	0
imp	'GetOldestEventLogRecord'				GetOldestEventLogRecord					advapi32	0
imp	'GetPrivateObjectSecurity'				GetPrivateObjectSecurity				advapi32	0
imp	'GetSecurityDescriptorControl'				GetSecurityDescriptorControl				advapi32	0
imp	'GetSecurityDescriptorDacl'				GetSecurityDescriptorDacl				advapi32	0
imp	'GetSecurityDescriptorGroup'				GetSecurityDescriptorGroup				advapi32	0
imp	'GetSecurityDescriptorLength'				GetSecurityDescriptorLength				advapi32	0
imp	'GetSecurityDescriptorOwner'				GetSecurityDescriptorOwner				advapi32	0
imp	'GetSecurityDescriptorRMControl'			GetSecurityDescriptorRMControl				advapi32	0
imp	'GetSecurityDescriptorSacl'				GetSecurityDescriptorSacl				advapi32	0
imp	'GetSecurityInfo'					GetSecurityInfo						advapi32	0
imp	'GetServiceDisplayName'					GetServiceDisplayNameW					advapi32	0
imp	'GetServiceKeyName'					GetServiceKeyNameW					advapi32	0
imp	'GetSidIdentifierAuthority'				GetSidIdentifierAuthority				advapi32	0
imp	'GetSidLengthRequired'					GetSidLengthRequired					advapi32	0
imp	'GetSidSubAuthority'					GetSidSubAuthority					advapi32	0
imp	'GetSidSubAuthorityCount'				GetSidSubAuthorityCount					advapi32	0
imp	'GetThreadWaitChain'					GetThreadWaitChain					advapi32	0
imp	'GetTokenInformation'					GetTokenInformation					advapi32	0
imp	'GetTraceEnableFlags'					GetTraceEnableFlags					advapi32	0
imp	'GetTraceEnableLevel'					GetTraceEnableLevel					advapi32	0
imp	'GetTraceLoggerHandle'					GetTraceLoggerHandle					advapi32	0
imp	'GetTrusteeForm'					GetTrusteeFormW						advapi32	0
imp	'GetTrusteeName'					GetTrusteeNameW						advapi32	0
imp	'GetTrusteeType'					GetTrusteeTypeW						advapi32	0
imp	'GetUserName'						GetUserNameW						advapi32	0	2
imp	'GetWindowsAccountDomainSid'				GetWindowsAccountDomainSid				advapi32	0
imp	'ImpersonateAnonymousToken'				ImpersonateAnonymousToken				advapi32	0
imp	'ImpersonateLoggedOnUser'				ImpersonateLoggedOnUser					advapi32	0
imp	'ImpersonateNamedPipeClient'				ImpersonateNamedPipeClient				advapi32	0
imp	'ImpersonateSelf'					ImpersonateSelf						advapi32	0	1
imp	'InitializeAcl'						InitializeAcl						advapi32	0
imp	'InitializeSecurityDescriptor'				InitializeSecurityDescriptor				advapi32	0
imp	'InitializeSid'						InitializeSid						advapi32	0
imp	'InitiateShutdown'					InitiateShutdownW					advapi32	0	5
imp	'InitiateSystemShutdown'				InitiateSystemShutdownW					advapi32	0
imp	'InitiateSystemShutdownEx'				InitiateSystemShutdownExW				advapi32	0
imp	'InstallApplication'					InstallApplication					advapi32	0
imp	'IsTextUnicode'						IsTextUnicode						advapi32	0
imp	'IsTokenRestricted'					IsTokenRestricted					advapi32	0
imp	'IsValidAcl'						IsValidAcl						advapi32	0
imp	'IsValidSecurityDescriptor'				IsValidSecurityDescriptor				advapi32	0
imp	'IsValidSid'						IsValidSid						advapi32	0
imp	'IsWellKnownSid'					IsWellKnownSid						advapi32	0
imp	'LockServiceDatabase'					LockServiceDatabase					advapi32	0
imp	'LogonUser'						LogonUserW						advapi32	0
imp	'LogonUserEx'						LogonUserExW						advapi32	0
imp	'LogonUserExEx'						LogonUserExExW						advapi32	0
imp	'LookupAccountName'					LookupAccountNameW					advapi32	0
imp	'LookupAccountSid'					LookupAccountSidW					advapi32	0
imp	'LookupPrivilegeDisplayName'				LookupPrivilegeDisplayNameW				advapi32	0
imp	'LookupPrivilegeName'					LookupPrivilegeNameW					advapi32	0
imp	'LookupPrivilegeValue'					LookupPrivilegeValueW					advapi32	0	3
imp	'LookupSecurityDescriptorParts'				LookupSecurityDescriptorPartsW				advapi32	0
imp	'LsaAddAccountRights'					LsaAddAccountRights					advapi32	0
imp	'LsaClose'						LsaClose						advapi32	0
imp	'LsaCreateTrustedDomain'				LsaCreateTrustedDomain					advapi32	0
imp	'LsaCreateTrustedDomainEx'				LsaCreateTrustedDomainEx				advapi32	0
imp	'LsaDeleteTrustedDomain'				LsaDeleteTrustedDomain					advapi32	0
imp	'LsaEnumerateAccountRights'				LsaEnumerateAccountRights				advapi32	0
imp	'LsaEnumerateAccountsWithUserRight'			LsaEnumerateAccountsWithUserRight			advapi32	0
imp	'LsaEnumerateTrustedDomains'				LsaEnumerateTrustedDomains				advapi32	0
imp	'LsaEnumerateTrustedDomainsEx'				LsaEnumerateTrustedDomainsEx				advapi32	0
imp	'LsaFreeMemory'						LsaFreeMemory						advapi32	0
imp	'LsaLookupNames'					LsaLookupNames						advapi32	0
imp	'LsaLookupNames2'					LsaLookupNames2						advapi32	0
imp	'LsaLookupSids'						LsaLookupSids						advapi32	0
imp	'LsaNtStatusToWinError'					LsaNtStatusToWinError					advapi32	0
imp	'LsaOpenPolicy'						LsaOpenPolicy						advapi32	0
imp	'LsaOpenTrustedDomainByName'				LsaOpenTrustedDomainByName				advapi32	0
imp	'LsaQueryDomainInformationPolicy'			LsaQueryDomainInformationPolicy				advapi32	0
imp	'LsaQueryForestTrustInformation'			LsaQueryForestTrustInformation				advapi32	0
imp	'LsaQueryInformationPolicy'				LsaQueryInformationPolicy				advapi32	0
imp	'LsaQueryTrustedDomainInfo'				LsaQueryTrustedDomainInfo				advapi32	0
imp	'LsaQueryTrustedDomainInfoByName'			LsaQueryTrustedDomainInfoByName				advapi32	0
imp	'LsaRemoveAccountRights'				LsaRemoveAccountRights					advapi32	0
imp	'LsaRetrievePrivateData'				LsaRetrievePrivateData					advapi32	0
imp	'LsaSetDomainInformationPolicy'				LsaSetDomainInformationPolicy				advapi32	0
imp	'LsaSetForestTrustInformation'				LsaSetForestTrustInformation				advapi32	0
imp	'LsaSetInformationPolicy'				LsaSetInformationPolicy					advapi32	0
imp	'LsaSetTrustedDomainInfoByName'				LsaSetTrustedDomainInfoByName				advapi32	0
imp	'LsaSetTrustedDomainInformation'			LsaSetTrustedDomainInformation				advapi32	0
imp	'LsaStorePrivateData'					LsaStorePrivateData					advapi32	0
imp	'MSChapSrvChangePassword'				MSChapSrvChangePassword					advapi32	0
imp	'MSChapSrvChangePassword2'				MSChapSrvChangePassword2				advapi32	0
imp	'MakeAbsoluteSD'					MakeAbsoluteSD						advapi32	0
imp	'MakeSelfRelativeSD'					MakeSelfRelativeSD					advapi32	0
imp	'MapGenericMask'					MapGenericMask						advapi32	0	2
imp	'NotifyBootConfigStatus'				NotifyBootConfigStatus					advapi32	0
imp	'NotifyChangeEventLog'					NotifyChangeEventLog					advapi32	0
imp	'NotifyServiceStatusChange'				NotifyServiceStatusChangeW				advapi32	0
imp	'ObjectCloseAuditAlarm'					ObjectCloseAuditAlarmW					advapi32	0
imp	'ObjectDeleteAuditAlarm'				ObjectDeleteAuditAlarmW					advapi32	0
imp	'ObjectOpenAuditAlarm'					ObjectOpenAuditAlarmW					advapi32	0
imp	'ObjectPrivilegeAuditAlarm'				ObjectPrivilegeAuditAlarmW				advapi32	0
imp	'OpenBackupEventLog'					OpenBackupEventLogW					advapi32	0
imp	'OpenEncryptedFileRaw'					OpenEncryptedFileRawW					advapi32	0
imp	'OpenEventLog'						OpenEventLogW						advapi32	0
imp	'OpenProcessToken'					OpenProcessToken					advapi32	0	3
imp	'OpenSCManager'						OpenSCManagerW						advapi32	0
imp	'OpenService'						OpenServiceW						advapi32	0
imp	'OpenThreadToken'					OpenThreadToken						advapi32	0	4
imp	'OpenThreadWaitChainSession'				OpenThreadWaitChainSession				advapi32	0
imp	'OpenTrace'						OpenTraceW						advapi32	0
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
imp	'ProcessTrace'						ProcessTrace						advapi32	0
imp	'QueryAllTraces'					QueryAllTracesW						advapi32	0
imp	'QueryRecoveryAgentsOnEncryptedFile'			QueryRecoveryAgentsOnEncryptedFile			advapi32	0
imp	'QuerySecurityAccessMask'				QuerySecurityAccessMask					advapi32	0
imp	'QueryServiceConfig'					QueryServiceConfigW					advapi32	0
imp	'QueryServiceConfig2W'					QueryServiceConfig2W					advapi32	0
imp	'QueryServiceLockStatus'				QueryServiceLockStatusW					advapi32	0
imp	'QueryServiceObjectSecurity'				QueryServiceObjectSecurity				advapi32	0
imp	'QueryServiceStatus'					QueryServiceStatus					advapi32	0
imp	'QueryServiceStatusEx'					QueryServiceStatusEx					advapi32	0
imp	'QueryTrace'						QueryTraceW						advapi32	0
imp	'QueryUsersOnEncryptedFile'				QueryUsersOnEncryptedFile				advapi32	0
imp	'ReadEncryptedFileRaw'					ReadEncryptedFileRaw					advapi32	0
imp	'ReadEventLog'						ReadEventLogW						advapi32	0
imp	'RegCloseKey'						RegCloseKey						advapi32	0	1
imp	'RegConnectRegistry'					RegConnectRegistryW					advapi32	0	3
imp	'RegCopyTree'						RegCopyTreeW						advapi32	0
imp	'RegCreateKey'						RegCreateKeyW						advapi32	0	3
imp	'RegCreateKeyEx'					RegCreateKeyExW						advapi32	0	9
imp	'RegCreateKeyTransacted'				RegCreateKeyTransactedW					advapi32	0
imp	'RegDeleteKey'						RegDeleteKeyW						advapi32	0	2
imp	'RegDeleteKeyEx'					RegDeleteKeyExW						advapi32	0	4
imp	'RegDeleteKeyTransacted'				RegDeleteKeyTransactedW					advapi32	0
imp	'RegDeleteKeyValue'					RegDeleteKeyValueW					advapi32	0
imp	'RegDeleteTree'						RegDeleteTreeW						advapi32	0	2
imp	'RegDeleteValue'					RegDeleteValueW						advapi32	0	2
imp	'RegDisablePredefinedCache'				RegDisablePredefinedCache				advapi32	0	1
imp	'RegDisablePredefinedCacheEx'				RegDisablePredefinedCacheEx				advapi32	0
imp	'RegDisableReflectionKey'				RegDisableReflectionKey					advapi32	0	1
imp	'RegEnableReflectionKey'				RegEnableReflectionKey					advapi32	0	1
imp	'RegEnumKey'						RegEnumKeyW						advapi32	0	4
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
imp	'RegOpenKey'						RegOpenKeyW						advapi32	0
imp	'RegOpenKeyEx'						RegOpenKeyExW						advapi32	0	5
imp	'RegOpenKeyTransacted'					RegOpenKeyTransactedW					advapi32	0
imp	'RegOpenUserClassesRoot'				RegOpenUserClassesRoot					advapi32	0	4
imp	'RegOverridePredefKey'					RegOverridePredefKey					advapi32	0	2
imp	'RegQueryInfoKey'					RegQueryInfoKeyW					advapi32	0	12
imp	'RegQueryMultipleValues'				RegQueryMultipleValuesW					advapi32	0	5
imp	'RegQueryReflectionKey'					RegQueryReflectionKey					advapi32	0	2
imp	'RegQueryValue'						RegQueryValueW						advapi32	0	4
imp	'RegQueryValueEx'					RegQueryValueExW					advapi32	0	6
imp	'RegReplaceKey'						RegReplaceKeyW						advapi32	0	4
imp	'RegRestoreKey'						RegRestoreKeyW						advapi32	0	3
imp	'RegSaveKey'						RegSaveKeyW						advapi32	0	3
imp	'RegSaveKeyEx'						RegSaveKeyExW						advapi32	0
imp	'RegSetKeySecurity'					RegSetKeySecurity					advapi32	0	3
imp	'RegSetKeyValue'					RegSetKeyValueW						advapi32	0
imp	'RegSetValue'						RegSetValueW						advapi32	0	5
imp	'RegSetValueEx'						RegSetValueExW						advapi32	0	6
imp	'RegUnLoadKey'						RegUnLoadKeyW						advapi32	0	2
imp	'RegisterEventSource'					RegisterEventSourceW					advapi32	0    2
imp	'RegisterServiceCtrlHandler'				RegisterServiceCtrlHandlerW				advapi32	0
imp	'RegisterServiceCtrlHandlerEx'				RegisterServiceCtrlHandlerExW				advapi32	0
imp	'RegisterTraceGuids'					RegisterTraceGuidsW					advapi32	0
imp	'RegisterWaitChainCOMCallback'				RegisterWaitChainCOMCallback				advapi32	0
imp	'RemoveTraceCallback'					RemoveTraceCallback					advapi32	0
imp	'RemoveUsersFromEncryptedFile'				RemoveUsersFromEncryptedFile				advapi32	0
imp	'ReportEvent'						ReportEventW						advapi32	0	9
imp	'ReportEventA'						ReportEventA						advapi32	0	9
imp	'RevertToSelf'						RevertToSelf						advapi32	0	0
imp	'RtlGenRandom'						SystemFunction036					advapi32	0	2
imp	'SaferCloseLevel'					SaferCloseLevel						advapi32	0
imp	'SaferComputeTokenFromLevel'				SaferComputeTokenFromLevel				advapi32	0
imp	'SaferCreateLevel'					SaferCreateLevel					advapi32	0
imp	'SaferGetLevelInformation'				SaferGetLevelInformation				advapi32	0
imp	'SaferGetPolicyInformation'				SaferGetPolicyInformation				advapi32	0
imp	'SaferIdentifyLevel'					SaferIdentifyLevel					advapi32	0
imp	'SaferRecordEventLogEntry'				SaferRecordEventLogEntry				advapi32	0
imp	'SaferiIsExecutableFileType'				SaferiIsExecutableFileType				advapi32	0
imp	'SetAclInformation'					SetAclInformation					advapi32	0
imp	'SetEntriesInAcl'					SetEntriesInAclW					advapi32	0
imp	'SetFileSecurity'					SetFileSecurityW					advapi32	0
imp	'SetKernelObjectSecurity'				SetKernelObjectSecurity					advapi32	0
imp	'SetNamedSecurityInfo'					SetNamedSecurityInfoW					advapi32	0
imp	'SetPrivateObjectSecurity'				SetPrivateObjectSecurity				advapi32	0
imp	'SetPrivateObjectSecurityEx'				SetPrivateObjectSecurityEx				advapi32	0
imp	'SetSecurityAccessMask'					SetSecurityAccessMask					advapi32	0
imp	'SetSecurityDescriptorControl'				SetSecurityDescriptorControl				advapi32	0
imp	'SetSecurityDescriptorDacl'				SetSecurityDescriptorDacl				advapi32	0
imp	'SetSecurityDescriptorGroup'				SetSecurityDescriptorGroup				advapi32	0
imp	'SetSecurityDescriptorOwner'				SetSecurityDescriptorOwner				advapi32	0
imp	'SetSecurityDescriptorRMControl'			SetSecurityDescriptorRMControl				advapi32	0
imp	'SetSecurityDescriptorSacl'				SetSecurityDescriptorSacl				advapi32	0
imp	'SetSecurityInfo'					SetSecurityInfo						advapi32	0
imp	'SetServiceBits'					SetServiceBits						advapi32	0
imp	'SetServiceObjectSecurity'				SetServiceObjectSecurity				advapi32	0
imp	'SetServiceStatus'					SetServiceStatus					advapi32	0
imp	'SetThreadToken'					SetThreadToken						advapi32	0
imp	'SetTokenInformation'					SetTokenInformation					advapi32	0
imp	'SetTraceCallback'					SetTraceCallback					advapi32	0
imp	'SetUserFileEncryptionKey'				SetUserFileEncryptionKey				advapi32	0
imp	'StartService'						StartServiceW						advapi32	0
imp	'StartServiceCtrlDispatcher'				StartServiceCtrlDispatcherW				advapi32	0
imp	'StartTrace'						StartTraceW						advapi32	0
imp	'StopTrace'						StopTraceW						advapi32	0
imp	'SystemFunction040'					SystemFunction040					advapi32	0
imp	'SystemFunction041'					SystemFunction041					advapi32	0
imp	'TraceEvent'						TraceEvent						advapi32	0
imp	'TraceEventInstance'					TraceEventInstance					advapi32	0
imp	'TraceMessage'						TraceMessage						advapi32	0
imp	'TraceMessageVa'					TraceMessageVa						advapi32	0
imp	'TraceSetInformation'					TraceSetInformation					advapi32	0 # Windows 7+
imp	'TreeResetNamedSecurityInfo'				TreeResetNamedSecurityInfoW				advapi32	0
imp	'TreeSetNamedSecurityInfo'				TreeSetNamedSecurityInfoW				advapi32	0
imp	'UninstallApplication'					UninstallApplication					advapi32	0
imp	'UnlockServiceDatabase'					UnlockServiceDatabase					advapi32	0
imp	'UnregisterTraceGuids'					UnregisterTraceGuids					advapi32	0
imp	'UpdateTrace'						UpdateTraceW						advapi32	0

# USER32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'ActivateKeyboardLayout'				ActivateKeyboardLayout					user32		0
imp	'AddClipboardFormatListener'				AddClipboardFormatListener				user32		0
imp	'AdjustWindowRect'					AdjustWindowRect					user32		0	3
imp	'AdjustWindowRectEx'					AdjustWindowRectEx					user32		0
imp	'AdjustWindowRectExForDpi'				AdjustWindowRectExForDpi				user32		0
imp	'AlignRects'						AlignRects						user32		0
imp	'AllowForegroundActivation'				AllowForegroundActivation				user32		0
imp	'AllowSetForegroundWindow'				AllowSetForegroundWindow				user32		0
imp	'AnimateWindow'						AnimateWindow						user32		0	3
imp	'AnyPopup'						AnyPopup						user32		0
imp	'AppendMenuA'						AppendMenuA						user32		0	4
imp	'AppendMenu'						AppendMenuW						user32		0	4
imp	'AreDpiAwarenessContextsEqual'				AreDpiAwarenessContextsEqual				user32		0
imp	'ArrangeIconicWindows'					ArrangeIconicWindows					user32		0
imp	'AttachThreadInput'					AttachThreadInput					user32		0
imp	'BeginDeferWindowPos'					BeginDeferWindowPos					user32		0
imp	'BeginPaint'						BeginPaint						user32		0	2
imp	'BlockInput'						BlockInput						user32		0
imp	'BringWindowToTop'					BringWindowToTop					user32		0	1
imp	'BroadcastSystemMessage'				BroadcastSystemMessageW					user32		0
imp	'BroadcastSystemMessageEx'				BroadcastSystemMessageExW				user32		0
imp	'BuildReasonArray'					BuildReasonArray					user32		0
imp	'CalcMenuBar'						CalcMenuBar						user32		0
imp	'CalculatePopupWindowPosition'				CalculatePopupWindowPosition				user32		0
imp	'CallMsgFilter'						CallMsgFilterW						user32		0
imp	'CallNextHookEx'					CallNextHookEx						user32		0	4
imp	'CallWindowProc'					CallWindowProcW						user32		0
imp	'CancelShutdown'					CancelShutdown						user32		0
imp	'CascadeChildWindows'					CascadeChildWindows					user32		0
imp	'CascadeWindows'					CascadeWindows						user32		0
imp	'ChangeClipboardChain'					ChangeClipboardChain					user32		0
imp	'ChangeDisplaySettings'					ChangeDisplaySettingsW					user32		0
imp	'ChangeDisplaySettingsEx'				ChangeDisplaySettingsExW				user32		0
imp	'ChangeMenu'						ChangeMenuW						user32		0
imp	'ChangeWindowMessageFilter'				ChangeWindowMessageFilter				user32		0
imp	'ChangeWindowMessageFilterEx'				ChangeWindowMessageFilterEx				user32		0
imp	'CharToOem'						CharToOemW						user32		0
imp	'CharToOemBuff'						CharToOemBuffW						user32		0
imp	'CheckDBCSEnabledExt'					CheckDBCSEnabledExt					user32		0
imp	'CheckDlgButton'					CheckDlgButton						user32		0
imp	'CheckMenuItem'						CheckMenuItem						user32		0
imp	'CheckMenuRadioItem'					CheckMenuRadioItem					user32		0
imp	'CheckProcessForClipboardAccess'			CheckProcessForClipboardAccess				user32		0
imp	'CheckProcessSession'					CheckProcessSession					user32		0
imp	'CheckRadioButton'					CheckRadioButton					user32		0
imp	'CheckWindowThreadDesktop'				CheckWindowThreadDesktop				user32		0
imp	'ChildWindowFromPoint'					ChildWindowFromPoint					user32		0
imp	'ChildWindowFromPointEx'				ChildWindowFromPointEx					user32		0
imp	'CliImmSetHotKey'					CliImmSetHotKey						user32		0
imp	'ClientThreadSetup'					ClientThreadSetup					user32		0
imp	'ClientToScreen'					ClientToScreen						user32		0
imp	'ClipCursor'						ClipCursor						user32		0
imp	'CloseClipboard'					CloseClipboard						user32		0
imp	'CloseDesktop'						CloseDesktop						user32		0
imp	'CloseGestureInfoHandle'				CloseGestureInfoHandle					user32		0
imp	'CloseTouchInputHandle'					CloseTouchInputHandle					user32		0
imp	'CloseWindow'						CloseWindow						user32		0	1
imp	'CloseWindowStation'					CloseWindowStation					user32		0
imp	'ConsoleControl'					ConsoleControl						user32		0
imp	'ControlMagnification'					ControlMagnification					user32		0
imp	'CopyAcceleratorTable'					CopyAcceleratorTableW					user32		0
imp	'CopyIcon'						CopyIcon						user32		0
imp	'CopyImage'						CopyImage						user32		0
imp	'CopyRect'						CopyRect						user32		0
imp	'CountClipboardFormats'					CountClipboardFormats					user32		0
imp	'CreateAcceleratorTable'				CreateAcceleratorTableW					user32		0
imp	'CreateCaret'						CreateCaret						user32		0
imp	'CreateCursor'						CreateCursor						user32		0
imp	'CreateDCompositionHwndTarget'				CreateDCompositionHwndTarget				user32		0
imp	'CreateDesktop'						CreateDesktopW						user32		0
imp	'CreateDesktopEx'					CreateDesktopExW					user32		0
imp	'CreateDialogIndirectParam'				CreateDialogIndirectParamW				user32		0
imp	'CreateDialogIndirectParamAor'				CreateDialogIndirectParamAorW				user32		0
imp	'CreateDialogParam'					CreateDialogParamW					user32		0
imp	'CreateIcon'						CreateIcon						user32		0
imp	'CreateIconFromResource'				CreateIconFromResource					user32		0
imp	'CreateIconFromResourceEx'				CreateIconFromResourceEx				user32		0
imp	'CreateIconIndirect'					CreateIconIndirect					user32		0	1
imp	'CreateMDIWindow'					CreateMDIWindowW					user32		0
imp	'CreateMenu'						CreateMenu						user32		0	0
imp	'CreatePalmRejectionDelayZone'				CreatePalmRejectionDelayZone				user32		0
imp	'CreatePopupMenu'					CreatePopupMenu						user32		0	0
imp	'CreateSystemThreads'					CreateSystemThreads					user32		0
imp	'CreateWindowEx'					CreateWindowExW						user32		0	12
imp	'CreateWindowInBand'					CreateWindowInBand					user32		0
imp	'CreateWindowInBandEx'					CreateWindowInBandEx					user32		0
imp	'CreateWindowIndirect'					CreateWindowIndirect					user32		0
imp	'CreateWindowStation'					CreateWindowStationW					user32		0
imp	'CsrBroadcastSystemMessageEx'				CsrBroadcastSystemMessageExW				user32		0
imp	'CtxInitUser32'						CtxInitUser32						user32		0
imp	'DWMBindCursorToOutputConfig'				DWMBindCursorToOutputConfig				user32		0
imp	'DWMCommitInputSystemOutputConfig'			DWMCommitInputSystemOutputConfig			user32		0
imp	'DWMSetCursorOrientation'				DWMSetCursorOrientation					user32		0
imp	'DWMSetInputSystemOutputConfig'				DWMSetInputSystemOutputConfig				user32		0
imp	'DdeAbandonTransaction'					DdeAbandonTransaction					user32		0
imp	'DdeAccessData'						DdeAccessData						user32		0
imp	'DdeAddData'						DdeAddData						user32		0
imp	'DdeClientTransaction'					DdeClientTransaction					user32		0
imp	'DdeCmpStringHandles'					DdeCmpStringHandles					user32		0
imp	'DdeConnect'						DdeConnect						user32		0
imp	'DdeConnectList'					DdeConnectList						user32		0
imp	'DdeCreateDataHandle'					DdeCreateDataHandle					user32		0
imp	'DdeCreateStringHandle'					DdeCreateStringHandleW					user32		0
imp	'DdeDisconnect'						DdeDisconnect						user32		0
imp	'DdeDisconnectList'					DdeDisconnectList					user32		0
imp	'DdeEnableCallback'					DdeEnableCallback					user32		0
imp	'DdeFreeDataHandle'					DdeFreeDataHandle					user32		0
imp	'DdeFreeStringHandle'					DdeFreeStringHandle					user32		0
imp	'DdeGetData'						DdeGetData						user32		0
imp	'DdeGetLastError'					DdeGetLastError						user32		0
imp	'DdeGetQualityOfService'				DdeGetQualityOfService					user32		0
imp	'DdeImpersonateClient'					DdeImpersonateClient					user32		0
imp	'DdeInitialize'						DdeInitializeW						user32		0
imp	'DdeKeepStringHandle'					DdeKeepStringHandle					user32		0
imp	'DdeNameService'					DdeNameService						user32		0
imp	'DdePostAdvise'						DdePostAdvise						user32		0
imp	'DdeQueryConvInfo'					DdeQueryConvInfo					user32		0
imp	'DdeQueryNextServer'					DdeQueryNextServer					user32		0
imp	'DdeQueryString'					DdeQueryStringW						user32		0
imp	'DdeReconnect'						DdeReconnect						user32		0
imp	'DdeSetQualityOfService'				DdeSetQualityOfService					user32		0
imp	'DdeSetUserHandle'					DdeSetUserHandle					user32		0
imp	'DdeUnaccessData'					DdeUnaccessData						user32		0
imp	'DdeUninitialize'					DdeUninitialize						user32		0
imp	'DefFrameProc'						DefFrameProcW						user32		0
imp	'DefMDIChildProc'					DefMDIChildProcW					user32		0
imp	'DefRawInputProc'					DefRawInputProc						user32		0
imp	'DefWindowProc'						DefWindowProcW						user32		0	4
imp	'DeferWindowPos'					DeferWindowPos						user32		0
imp	'DeferWindowPosAndBand'					DeferWindowPosAndBand					user32		0
imp	'DelegateInput'						DelegateInput						user32		0
imp	'DeleteMenu'						DeleteMenu						user32		0	3
imp	'DeregisterShellHookWindow'				DeregisterShellHookWindow				user32		0
imp	'DestroyAcceleratorTable'				DestroyAcceleratorTable					user32		0
imp	'DestroyCaret'						DestroyCaret						user32		0
imp	'DestroyCursor'						DestroyCursor						user32		0
imp	'DestroyDCompositionHwndTarget'				DestroyDCompositionHwndTarget				user32		0
imp	'DestroyIcon'						DestroyIcon						user32		0	1
imp	'DestroyMenu'						DestroyMenu						user32		0	1
imp	'DestroyPalmRejectionDelayZone'				DestroyPalmRejectionDelayZone				user32		0
imp	'DestroyReasons'					DestroyReasons						user32		0
imp	'DestroyWindow'						DestroyWindow						user32		0	1
imp	'DialogBoxIndirectParam'				DialogBoxIndirectParamW					user32		0
imp	'DialogBoxIndirectParamAor'				DialogBoxIndirectParamAorW				user32		0
imp	'DialogBoxParam'					DialogBoxParamW						user32		0
imp	'DisableProcessWindowsGhosting'				DisableProcessWindowsGhosting				user32		0
imp	'DispatchMessage'					DispatchMessageW					user32		0	1
imp	'DisplayConfigGetDeviceInfo'				DisplayConfigGetDeviceInfo				user32		0
imp	'DisplayConfigSetDeviceInfo'				DisplayConfigSetDeviceInfo				user32		0
imp	'DisplayExitWindowsWarnings'				DisplayExitWindowsWarnings				user32		0
imp	'DlgDirList'						DlgDirListW						user32		0
imp	'DlgDirListComboBox'					DlgDirListComboBoxW					user32		0
imp	'DlgDirSelectComboBoxEx'				DlgDirSelectComboBoxExW					user32		0
imp	'DlgDirSelectEx'					DlgDirSelectExW						user32		0
imp	'DoSoundConnect'					DoSoundConnect						user32		0
imp	'DoSoundDisconnect'					DoSoundDisconnect					user32		0
imp	'DragDetect'						DragDetect						user32		0
imp	'DragObject'						DragObject						user32		0
imp	'DrawAnimatedRects'					DrawAnimatedRects					user32		0
imp	'DrawCaption'						DrawCaption						user32		0
imp	'DrawCaptionTemp'					DrawCaptionTempW					user32		0
imp	'DrawEdge'						DrawEdge						user32		0
imp	'DrawFocusRect'						DrawFocusRect						user32		0
imp	'DrawFrame'						DrawFrame						user32		0
imp	'DrawFrameControl'					DrawFrameControl					user32		0
imp	'DrawIcon'						DrawIcon						user32		0
imp	'DrawIconEx'						DrawIconEx						user32		0
imp	'DrawMenuBar'						DrawMenuBar						user32		0
imp	'DrawMenuBarTemp'					DrawMenuBarTemp						user32		0
imp	'DrawState'						DrawStateW						user32		0
imp	'DrawText'						DrawTextW						user32		0	5
imp	'DrawTextEx'						DrawTextExW						user32		0	6
imp	'DwmGetDxRgn'						DwmGetDxRgn						user32		0
imp	'DwmGetDxSharedSurface'					DwmGetDxSharedSurface					user32		0
imp	'DwmGetRemoteSessionOcclusionEvent'			DwmGetRemoteSessionOcclusionEvent			user32		0
imp	'DwmGetRemoteSessionOcclusionState'			DwmGetRemoteSessionOcclusionState			user32		0
imp	'DwmKernelShutdown'					DwmKernelShutdown					user32		0
imp	'DwmKernelStartup'					DwmKernelStartup					user32		0
imp	'DwmLockScreenUpdates'					DwmLockScreenUpdates					user32		0
imp	'DwmValidateWindow'					DwmValidateWindow					user32		0
imp	'EditWndProc'						EditWndProc						user32		0
imp	'EmptyClipboard'					EmptyClipboard						user32		0
imp	'EnableMenuItem'					EnableMenuItem						user32		0
imp	'EnableMouseInPointer'					EnableMouseInPointer					user32		0
imp	'EnableNonClientDpiScaling'				EnableNonClientDpiScaling				user32		0
imp	'EnableOneCoreTransformMode'				EnableOneCoreTransformMode				user32		0
imp	'EnableScrollBar'					EnableScrollBar						user32		0
imp	'EnableSessionForMMCSS'					EnableSessionForMMCSS					user32		0
imp	'EnableWindow'						EnableWindow						user32		0
imp	'EndDeferWindowPos'					EndDeferWindowPos					user32		0
imp	'EndDeferWindowPosEx'					EndDeferWindowPosEx					user32		0
imp	'EndDialog'						EndDialog						user32		0
imp	'EndMenu'						EndMenu							user32		0
imp	'EndPaint'						EndPaint						user32		0	2
imp	'EndTask'						EndTask							user32		0
imp	'EnterReaderModeHelper'					EnterReaderModeHelper					user32		0
imp	'EnumChildWindows'					EnumChildWindows					user32		0	3
imp	'EnumClipboardFormats'					EnumClipboardFormats					user32		0
imp	'EnumDesktopWindows'					EnumDesktopWindows					user32		0
imp	'EnumDesktops'						EnumDesktopsW						user32		0
imp	'EnumDisplayDevices'					EnumDisplayDevicesW					user32		0
imp	'EnumDisplayMonitors'					EnumDisplayMonitors					user32		0
imp	'EnumDisplaySettings'					EnumDisplaySettingsW					user32		0
imp	'EnumDisplaySettingsEx'					EnumDisplaySettingsExW					user32		0
imp	'EnumProps'						EnumPropsW						user32		0
imp	'EnumPropsEx'						EnumPropsExW						user32		0
imp	'EnumThreadWindows'					EnumThreadWindows					user32		0
imp	'EnumWindowStations'					EnumWindowStationsW					user32		0
imp	'EnumWindows'						EnumWindows						user32		0
imp	'EqualRect'						EqualRect						user32		0
imp	'EvaluateProximityToPolygon'				EvaluateProximityToPolygon				user32		0
imp	'EvaluateProximityToRect'				EvaluateProximityToRect					user32		0
imp	'ExcludeUpdateRgn'					ExcludeUpdateRgn					user32		0
imp	'ExitWindowsEx'						ExitWindowsEx						user32		0
imp	'FillRect'						FillRect						user32		0	3
imp	'FindWindow'						FindWindowW						user32		0	2
imp	'FindWindowEx'						FindWindowExW						user32		0	4
imp	'FlashWindow'						FlashWindow						user32		0
imp	'FlashWindowEx'						FlashWindowEx						user32		0
imp	'FrameRect'						FrameRect						user32		0
imp	'FreeDDElParam'						FreeDDElParam						user32		0
imp	'FrostCrashedWindow'					FrostCrashedWindow					user32		0
imp	'GetActiveWindow'					GetActiveWindow						user32		0
imp	'GetAltTabInfo'						GetAltTabInfoW						user32		0
imp	'GetAncestor'						GetAncestor						user32		0
imp	'GetAppCompatFlags'					GetAppCompatFlags					user32		0
imp	'GetAppCompatFlags2'					GetAppCompatFlags2					user32		0
imp	'GetAsyncKeyState'					GetAsyncKeyState					user32		0
imp	'GetAutoRotationState'					GetAutoRotationState					user32		0
imp	'GetAwarenessFromDpiAwarenessContext'			GetAwarenessFromDpiAwarenessContext			user32		0
imp	'GetCIMSSM'						GetCIMSSM						user32		0
imp	'GetCapture'						GetCapture						user32		0
imp	'GetCaretBlinkTime'					GetCaretBlinkTime					user32		0
imp	'GetCaretPos'						GetCaretPos						user32		0
imp	'GetClassInfo'						GetClassInfoW						user32		0
imp	'GetClassInfoEx'					GetClassInfoExW						user32		0
imp	'GetClassLong'						GetClassLongW						user32		0
imp	'GetClassLongPtr'					GetClassLongPtrW					user32		0
imp	'GetClassName'						GetClassNameW						user32		0
imp	'GetClassWord'						GetClassWord						user32		0
imp	'GetClientRect'						GetClientRect						user32		0	2
imp	'GetClipCursor'						GetClipCursor						user32		0
imp	'GetClipboardAccessToken'				GetClipboardAccessToken					user32		0
imp	'GetClipboardData'					GetClipboardData					user32		0
imp	'GetClipboardFormatName'				GetClipboardFormatNameW					user32		0
imp	'GetClipboardOwner'					GetClipboardOwner					user32		0
imp	'GetClipboardSequenceNumber'				GetClipboardSequenceNumber				user32		0
imp	'GetClipboardViewer'					GetClipboardViewer					user32		0
imp	'GetComboBoxInfo'					GetComboBoxInfo						user32		0
imp	'GetCurrentInputMessageSource'				GetCurrentInputMessageSource				user32		0
imp	'GetCursor'						GetCursor						user32		0	0
imp	'GetCursorFrameInfo'					GetCursorFrameInfo					user32		0
imp	'GetCursorInfo'						GetCursorInfo						user32		0
imp	'GetCursorPos'						GetCursorPos						user32		0	1
imp	'GetDC'							GetDC							user32		0	1
imp	'GetDCEx'						GetDCEx							user32		0
imp	'GetDesktopID'						GetDesktopID						user32		0
imp	'GetDesktopWindow'					GetDesktopWindow					user32		0	0
imp	'GetDialogBaseUnits'					GetDialogBaseUnits					user32		0
imp	'GetDialogControlDpiChangeBehavior'			GetDialogControlDpiChangeBehavior			user32		0
imp	'GetDialogDpiChangeBehavior'				GetDialogDpiChangeBehavior				user32		0
imp	'GetDisplayAutoRotationPreferences'			GetDisplayAutoRotationPreferences			user32		0
imp	'GetDisplayConfigBufferSizes'				GetDisplayConfigBufferSizes				user32		0
imp	'GetDlgCtrlID'						GetDlgCtrlID						user32		0
imp	'GetDlgItem'						GetDlgItem						user32		0
imp	'GetDlgItemInt'						GetDlgItemInt						user32		0
imp	'GetDlgItemText'					GetDlgItemTextW						user32		0
imp	'GetDoubleClickTime'					GetDoubleClickTime					user32		0
imp	'GetDpiForMonitorInternal'				GetDpiForMonitorInternal				user32		0
imp	'GetDpiForSystem'					GetDpiForSystem						user32		0
imp	'GetDpiForWindow'					GetDpiForWindow						user32		0
imp	'GetDpiFromDpiAwarenessContext'				GetDpiFromDpiAwarenessContext				user32		0
imp	'GetFocus'						GetFocus						user32		0
imp	'GetForegroundWindow'					GetForegroundWindow					user32		0
imp	'GetGUIThreadInfo'					GetGUIThreadInfo					user32		0
imp	'GetGestureConfig'					GetGestureConfig					user32		0
imp	'GetGestureExtraArgs'					GetGestureExtraArgs					user32		0
imp	'GetGestureInfo'					GetGestureInfo						user32		0
imp	'GetGuiResources'					GetGuiResources						user32		0
imp	'GetIconInfo'						GetIconInfo						user32		0
imp	'GetIconInfoEx'						GetIconInfoExW						user32		0
imp	'GetInputDesktop'					GetInputDesktop						user32		0
imp	'GetInputLocaleInfo'					GetInputLocaleInfo					user32		0
imp	'GetInputState'						GetInputState						user32		0
imp	'GetInternalWindowPos'					GetInternalWindowPos					user32		0
imp	'GetKBCodePage'						GetKBCodePage						user32		0
imp	'GetKeyNameText'					GetKeyNameTextW						user32		0
imp	'GetKeyState'						GetKeyState						user32		0	1
imp	'GetKeyboardLayout'					GetKeyboardLayout					user32		0	1
imp	'GetKeyboardLayoutList'					GetKeyboardLayoutList					user32		0
imp	'GetKeyboardLayoutName'					GetKeyboardLayoutNameW					user32		0
imp	'GetKeyboardState'					GetKeyboardState					user32		0
imp	'GetKeyboardType'					GetKeyboardType						user32		0
imp	'GetLastActivePopup'					GetLastActivePopup					user32		0
imp	'GetLastInputInfo'					GetLastInputInfo					user32		0
imp	'GetLayeredWindowAttributes'				GetLayeredWindowAttributes				user32		0
imp	'GetListBoxInfo'					GetListBoxInfo						user32		0
imp	'GetMagnificationDesktopColorEffect'			GetMagnificationDesktopColorEffect			user32		0
imp	'GetMagnificationDesktopMagnification'			GetMagnificationDesktopMagnification			user32		0
imp	'GetMagnificationDesktopSamplingMode'			GetMagnificationDesktopSamplingMode			user32		0
imp	'GetMagnificationLensCtxInformation'			GetMagnificationLensCtxInformation			user32		0
imp	'GetMenu'						GetMenu							user32		0	1
imp	'GetMenuBarInfo'					GetMenuBarInfo						user32		0
imp	'GetMenuCheckMarkDimensions'				GetMenuCheckMarkDimensions				user32		0
imp	'GetMenuContextHelpId'					GetMenuContextHelpId					user32		0
imp	'GetMenuDefaultItem'					GetMenuDefaultItem					user32		0
imp	'GetMenuInfo'						GetMenuInfo						user32		0
imp	'GetMenuItemCount'					GetMenuItemCount					user32		0
imp	'GetMenuItemID'						GetMenuItemID						user32		0
imp	'GetMenuItemInfo'					GetMenuItemInfoW					user32		0
imp	'GetMenuItemRect'					GetMenuItemRect						user32		0
imp	'GetMenuState'						GetMenuState						user32		0
imp	'GetMenuString'						GetMenuStringW						user32		0
imp	'GetMessage'						GetMessageW						user32		0	4
imp	'GetMessageExtraInfo'					GetMessageExtraInfo					user32		0
imp	'GetMessagePos'						GetMessagePos						user32		0
imp	'GetMessageTime'					GetMessageTime						user32		0
imp	'GetMonitorInfo'					GetMonitorInfoW						user32		0
imp	'GetMouseMovePointsEx'					GetMouseMovePointsEx					user32		0
imp	'GetNextDlgGroupItem'					GetNextDlgGroupItem					user32		0
imp	'GetNextDlgTabItem'					GetNextDlgTabItem					user32		0
imp	'GetOpenClipboardWindow'				GetOpenClipboardWindow					user32		0
imp	'GetParent'						GetParent						user32		0	1
imp	'GetPhysicalCursorPos'					GetPhysicalCursorPos					user32		0
imp	'GetPointerCursorId'					GetPointerCursorId					user32		0
imp	'GetPointerDevice'					GetPointerDevice					user32		0
imp	'GetPointerDeviceCursors'				GetPointerDeviceCursors					user32		0
imp	'GetPointerDeviceProperties'				GetPointerDeviceProperties				user32		0
imp	'GetPointerDeviceRects'					GetPointerDeviceRects					user32		0
imp	'GetPointerDevices'					GetPointerDevices					user32		0
imp	'GetPointerFrameArrivalTimes'				GetPointerFrameArrivalTimes				user32		0
imp	'GetPointerFrameInfo'					GetPointerFrameInfo					user32		0
imp	'GetPointerFrameInfoHistory'				GetPointerFrameInfoHistory				user32		0
imp	'GetPointerFramePenInfo'				GetPointerFramePenInfo					user32		0
imp	'GetPointerFramePenInfoHistory'				GetPointerFramePenInfoHistory				user32		0
imp	'GetPointerFrameTouchInfo'				GetPointerFrameTouchInfo				user32		0
imp	'GetPointerFrameTouchInfoHistory'			GetPointerFrameTouchInfoHistory				user32		0
imp	'GetPointerInfo'					GetPointerInfo						user32		0
imp	'GetPointerInfoHistory'					GetPointerInfoHistory					user32		0
imp	'GetPointerInputTransform'				GetPointerInputTransform				user32		0
imp	'GetPointerPenInfo'					GetPointerPenInfo					user32		0
imp	'GetPointerPenInfoHistory'				GetPointerPenInfoHistory				user32		0
imp	'GetPointerTouchInfo'					GetPointerTouchInfo					user32		0
imp	'GetPointerTouchInfoHistory'				GetPointerTouchInfoHistory				user32		0
imp	'GetPointerType'					GetPointerType						user32		0
imp	'GetPriorityClipboardFormat'				GetPriorityClipboardFormat				user32		0
imp	'GetProcessDefaultLayout'				GetProcessDefaultLayout					user32		0
imp	'GetProcessDpiAwarenessInternal'			GetProcessDpiAwarenessInternal				user32		0
imp	'GetProcessUIContextInformation'			GetProcessUIContextInformation				user32		0
imp	'GetProcessWindowStation'				GetProcessWindowStation					user32		0
imp	'GetProgmanWindow'					GetProgmanWindow					user32		0
imp	'GetProp'						GetPropW						user32		0
imp	'GetQueueStatus'					GetQueueStatus						user32		0
imp	'GetRawInputBuffer'					GetRawInputBuffer					user32		0
imp	'GetRawInputData'					GetRawInputData						user32		0
imp	'GetRawInputDeviceInfo'					GetRawInputDeviceInfoW					user32		0
imp	'GetRawInputDeviceList'					GetRawInputDeviceList					user32		0
imp	'GetRawPointerDeviceData'				GetRawPointerDeviceData					user32		0
imp	'GetReasonTitleFromReasonCode'				GetReasonTitleFromReasonCode				user32		0
imp	'GetRegisteredRawInputDevices'				GetRegisteredRawInputDevices				user32		0
imp	'GetScrollBarInfo'					GetScrollBarInfo					user32		0
imp	'GetScrollInfo'						GetScrollInfo						user32		0
imp	'GetScrollPos'						GetScrollPos						user32		0
imp	'GetScrollRange'					GetScrollRange						user32		0
imp	'GetSendMessageReceiver'				GetSendMessageReceiver					user32		0
imp	'GetShellWindow'					GetShellWindow						user32		0	0
imp	'GetSubMenu'						GetSubMenu						user32		0
imp	'GetSysColor'						GetSysColor						user32		0
imp	'GetSysColorBrush'					GetSysColorBrush					user32		0
imp	'GetSystemDpiForProcess'				GetSystemDpiForProcess					user32		0
imp	'GetSystemMenu'						GetSystemMenu						user32		0	2
imp	'GetSystemMetrics'					GetSystemMetrics					user32		0
imp	'GetSystemMetricsForDpi'				GetSystemMetricsForDpi					user32		0
imp	'GetTabbedTextExtent'					GetTabbedTextExtentW					user32		0
imp	'GetTaskmanWindow'					GetTaskmanWindow					user32		0
imp	'GetThreadDesktop'					GetThreadDesktop					user32		0
imp	'GetThreadDpiAwarenessContext'				GetThreadDpiAwarenessContext				user32		0
imp	'GetThreadDpiHostingBehavior'				GetThreadDpiHostingBehavior				user32		0
imp	'GetTitleBarInfo'					GetTitleBarInfo						user32		0
imp	'GetTopLevelWindow'					GetTopLevelWindow					user32		0
imp	'GetTopWindow'						GetTopWindow						user32		0
imp	'GetTouchInputInfo'					GetTouchInputInfo					user32		0
imp	'GetUnpredictedMessagePos'				GetUnpredictedMessagePos				user32		0
imp	'GetUpdateRect'						GetUpdateRect						user32		0
imp	'GetUpdateRgn'						GetUpdateRgn						user32		0
imp	'GetUpdatedClipboardFormats'				GetUpdatedClipboardFormats				user32		0
imp	'GetUserObjectInformation'				GetUserObjectInformationW				user32		0
imp	'GetUserObjectSecurity'					GetUserObjectSecurity					user32		0
imp	'GetWinStationInfo'					GetWinStationInfo					user32		0
imp	'GetWindow'						GetWindow						user32		0	2
imp	'GetWindowBand'						GetWindowBand						user32		0
imp	'GetWindowCompositionAttribute'				GetWindowCompositionAttribute				user32		0
imp	'GetWindowCompositionInfo'				GetWindowCompositionInfo				user32		0
imp	'GetWindowContextHelpId'				GetWindowContextHelpId					user32		0
imp	'GetWindowDC'						GetWindowDC						user32		0
imp	'GetWindowDisplayAffinity'				GetWindowDisplayAffinity				user32		0
imp	'GetWindowDpiAwarenessContext'				GetWindowDpiAwarenessContext				user32		0
imp	'GetWindowDpiHostingBehavior'				GetWindowDpiHostingBehavior				user32		0
imp	'GetWindowFeedbackSetting'				GetWindowFeedbackSetting				user32		0
imp	'GetWindowInfo'						GetWindowInfo						user32		0
imp	'GetWindowLong'						GetWindowLongW						user32		0
imp	'GetWindowLongPtr'					GetWindowLongPtrW					user32		0
imp	'GetWindowMinimizeRect'					GetWindowMinimizeRect					user32		0
imp	'GetWindowModuleFileName'				GetWindowModuleFileNameW				user32		0
imp	'GetWindowPlacement'					GetWindowPlacement					user32		0	2
imp	'GetWindowProcessHandle'				GetWindowProcessHandle					user32		0
imp	'GetWindowRect'						GetWindowRect						user32		0	2
imp	'GetWindowRgn'						GetWindowRgn						user32		0
imp	'GetWindowRgnBox'					GetWindowRgnBox						user32		0
imp	'GetWindowRgnEx'					GetWindowRgnEx						user32		0
imp	'GetWindowText'						GetWindowTextW						user32		0	3
imp	'GetWindowTextLength'					GetWindowTextLengthW					user32		0
imp	'GetWindowThreadProcessId'				GetWindowThreadProcessId				user32		0
imp	'GetWindowWord'						GetWindowWord						user32		0
imp	'GhostWindowFromHungWindow'				GhostWindowFromHungWindow				user32		0
imp	'GrayString'						GrayStringW						user32		0
imp	'HandleDelegatedInput'					HandleDelegatedInput					user32		0
imp	'HideCaret'						HideCaret						user32		0
imp	'HiliteMenuItem'					HiliteMenuItem						user32		0
imp	'HungWindowFromGhostWindow'				HungWindowFromGhostWindow				user32		0
imp	'IMPGetIMEW'						IMPGetIMEW						user32		0
imp	'IMPQueryIMEW'						IMPQueryIMEW						user32		0
imp	'IMPSetIMEW'						IMPSetIMEW						user32		0
imp	'ImpersonateDdeClientWindow'				ImpersonateDdeClientWindow				user32		0
imp	'InSendMessage'						InSendMessage						user32		0
imp	'InSendMessageEx'					InSendMessageEx						user32		0
imp	'InflateRect'						InflateRect						user32		0
imp	'InheritWindowMonitor'					InheritWindowMonitor					user32		0
imp	'InitDManipHook'					InitDManipHook						user32		0
imp	'InitializeGenericHidInjection'				InitializeGenericHidInjection				user32		0
imp	'InitializeInputDeviceInjection'			InitializeInputDeviceInjection				user32		0
imp	'InitializeLpkHooks'					InitializeLpkHooks					user32		0
imp	'InitializePointerDeviceInjection'			InitializePointerDeviceInjection			user32		0
imp	'InitializePointerDeviceInjectionEx'			InitializePointerDeviceInjectionEx			user32		0
imp	'InitializeTouchInjection'				InitializeTouchInjection				user32		0
imp	'InjectDeviceInput'					InjectDeviceInput					user32		0
imp	'InjectGenericHidInput'					InjectGenericHidInput					user32		0
imp	'InjectKeyboardInput'					InjectKeyboardInput					user32		0
imp	'InjectMouseInput'					InjectMouseInput					user32		0
imp	'InjectPointerInput'					InjectPointerInput					user32		0
imp	'InjectTouchInput'					InjectTouchInput					user32		0
imp	'InsertMenu'						InsertMenuW						user32		0	5
imp	'InsertMenuItem'					InsertMenuItemW						user32		0
imp	'InternalGetWindowIcon'					InternalGetWindowIcon					user32		0
imp	'InternalGetWindowText'					InternalGetWindowText					user32		0
imp	'IntersectRect'						IntersectRect						user32		0
imp	'InvalidateRect'					InvalidateRect						user32		0	3
imp	'InvalidateRgn'						InvalidateRgn						user32		0
imp	'InvertRect'						InvertRect						user32		0
imp	'IsChild'						IsChild							user32		0	2
imp	'IsClipboardFormatAvailable'				IsClipboardFormatAvailable				user32		0
imp	'IsDialogMessage'					IsDialogMessageW					user32		0
imp	'IsDlgButtonChecked'					IsDlgButtonChecked					user32		0
imp	'IsGUIThread'						IsGUIThread						user32		0
imp	'IsHungAppWindow'					IsHungAppWindow						user32		0
imp	'IsIconic'						IsIconic						user32		0	1
imp	'IsImmersiveProcess'					IsImmersiveProcess					user32		0
imp	'IsInDesktopWindowBand'					IsInDesktopWindowBand					user32		0
imp	'IsMenu'						IsMenu							user32		0	1
imp	'IsMouseInPointerEnabled'				IsMouseInPointerEnabled					user32		0
imp	'IsOneCoreTransformMode'				IsOneCoreTransformMode					user32		0
imp	'IsProcessDPIAware'					IsProcessDPIAware					user32		0
imp	'IsQueueAttached'					IsQueueAttached						user32		0
imp	'IsRectEmpty'						IsRectEmpty						user32		0
imp	'IsSETEnabled'						IsSETEnabled						user32		0
imp	'IsServerSideWindow'					IsServerSideWindow					user32		0
imp	'IsThreadDesktopComposited'				IsThreadDesktopComposited				user32		0
imp	'IsThreadMessageQueueAttached'				IsThreadMessageQueueAttached				user32		0
imp	'IsThreadTSFEventAware'					IsThreadTSFEventAware					user32		0
imp	'IsTopLevelWindow'					IsTopLevelWindow					user32		0
imp	'IsTouchWindow'						IsTouchWindow						user32		0
imp	'IsValidDpiAwarenessContext'				IsValidDpiAwarenessContext				user32		0
imp	'IsWinEventHookInstalled'				IsWinEventHookInstalled					user32		0
imp	'IsWindow'						IsWindow						user32		0	1
imp	'IsWindowArranged'					IsWindowArranged					user32		0
imp	'IsWindowEnabled'					IsWindowEnabled						user32		0
imp	'IsWindowInDestroy'					IsWindowInDestroy					user32		0
imp	'IsWindowRedirectedForPrint'				IsWindowRedirectedForPrint				user32		0
imp	'IsWindowUnicode'					IsWindowUnicode						user32		0
imp	'IsWindowVisible'					IsWindowVisible						user32		0	1
imp	'IsZoomed'						IsZoomed						user32		0	1
imp	'KillTimer'						KillTimer						user32		0	2
imp	'LoadAccelerators'					LoadAcceleratorsW					user32		0
imp	'LoadBitmap'						LoadBitmapW						user32		0
imp	'LoadCursor'						LoadCursorW						user32		0	2
imp	'LoadCursorFromFile'					LoadCursorFromFileW					user32		0
imp	'LoadIcon'						LoadIconW						user32		0	2
imp	'LoadImage'						LoadImageW						user32		0	6
imp	'LoadKeyboardLayout'					LoadKeyboardLayoutW					user32		0
imp	'LoadKeyboardLayoutEx'					LoadKeyboardLayoutEx					user32		0
imp	'LoadLocalFonts'					LoadLocalFonts						user32		0
imp	'LoadMenu'						LoadMenuW						user32		0
imp	'LoadMenuIndirect'					LoadMenuIndirectW					user32		0
imp	'LoadRemoteFonts'					LoadRemoteFonts						user32		0
imp	'LockSetForegroundWindow'				LockSetForegroundWindow					user32		0
imp	'LockWindowStation'					LockWindowStation					user32		0
imp	'LockWindowUpdate'					LockWindowUpdate					user32		0
imp	'LockWorkStation'					LockWorkStation						user32		0
imp	'LogicalToPhysicalPoint'				LogicalToPhysicalPoint					user32		0
imp	'LogicalToPhysicalPointForPerMonitorDPI'		LogicalToPhysicalPointForPerMonitorDPI			user32		0
imp	'LookupIconIdFromDirectory'				LookupIconIdFromDirectory				user32		0
imp	'LookupIconIdFromDirectoryEx'				LookupIconIdFromDirectoryEx				user32		0
imp	'MBToWCSEx'						MBToWCSEx						user32		0
imp	'MBToWCSExt'						MBToWCSExt						user32		0
imp	'MB_GetString'						MB_GetString						user32		0
imp	'MITActivateInputProcessing'				MITActivateInputProcessing				user32		0
imp	'MITBindInputTypeToMonitors'				MITBindInputTypeToMonitors				user32		0
imp	'MITCoreMsgKGetConnectionHandle'			MITCoreMsgKGetConnectionHandle				user32		0
imp	'MITCoreMsgKOpenConnectionTo'				MITCoreMsgKOpenConnectionTo				user32		0
imp	'MITCoreMsgKSend'					MITCoreMsgKSend						user32		0
imp	'MITDeactivateInputProcessing'				MITDeactivateInputProcessing				user32		0
imp	'MITDisableMouseIntercept'				MITDisableMouseIntercept				user32		0
imp	'MITDispatchCompletion'					MITDispatchCompletion					user32		0
imp	'MITEnableMouseIntercept'				MITEnableMouseIntercept					user32		0
imp	'MITGetCursorUpdateHandle'				MITGetCursorUpdateHandle				user32		0
imp	'MITInjectLegacyISMTouchFrame'				MITInjectLegacyISMTouchFrame				user32		0
imp	'MITRegisterManipulationThread'				MITRegisterManipulationThread				user32		0
imp	'MITSetForegroundRoutingInfo'				MITSetForegroundRoutingInfo				user32		0
imp	'MITSetInputCallbacks'					MITSetInputCallbacks					user32		0
imp	'MITSetInputDelegationMode'				MITSetInputDelegationMode				user32		0
imp	'MITSetLastInputRecipient'				MITSetLastInputRecipient				user32		0
imp	'MITSetManipulationInputTarget'				MITSetManipulationInputTarget				user32		0
imp	'MITStopAndEndInertia'					MITStopAndEndInertia					user32		0
imp	'MITSynthesizeMouseInput'				MITSynthesizeMouseInput					user32		0
imp	'MITSynthesizeMouseWheel'				MITSynthesizeMouseWheel					user32		0
imp	'MITSynthesizeTouchInput'				MITSynthesizeTouchInput					user32		0
imp	'MITUpdateInputGlobals'					MITUpdateInputGlobals					user32		0
imp	'MITWaitForMultipleObjectsEx'				MITWaitForMultipleObjectsEx				user32		0
imp	'MakeThreadTSFEventAware'				MakeThreadTSFEventAware					user32		0
imp	'MapDialogRect'						MapDialogRect						user32		0
imp	'MapVirtualKey'						MapVirtualKeyW						user32		0
imp	'MapVirtualKeyEx'					MapVirtualKeyExW					user32		0	3
imp	'MapVisualRelativePoints'				MapVisualRelativePoints					user32		0
imp	'MapWindowPoints'					MapWindowPoints						user32		0
imp	'MenuItemFromPoint'					MenuItemFromPoint					user32		0
imp	'MenuWindowProc'					MenuWindowProcW						user32		0
imp	'MessageBeep'						MessageBeep						user32		0
imp	'MessageBox'						MessageBoxW						user32		0	4
imp	'MessageBoxEx'						MessageBoxExW						user32		0	5
imp	'MessageBoxIndirect'					MessageBoxIndirectW					user32		0
imp	'MessageBoxTimeout'					MessageBoxTimeoutW					user32		0
imp	'ModifyMenu'						ModifyMenuW						user32		0
imp	'MonitorFromPoint'					MonitorFromPoint					user32		0
imp	'MonitorFromRect'					MonitorFromRect						user32		0
imp	'MonitorFromWindow'					MonitorFromWindow					user32		0
imp	'MoveWindow'						MoveWindow						user32		0	6
imp	'MsgWaitForMultipleObjects'				MsgWaitForMultipleObjects				user32		0
imp	'MsgWaitForMultipleObjectsEx'				MsgWaitForMultipleObjectsEx				user32		0
imp	'NotifyOverlayWindow'					NotifyOverlayWindow					user32		0
imp	'NotifyWinEvent'					NotifyWinEvent						user32		0
imp	'OemKeyScan'						OemKeyScan						user32		0
imp	'OemToChar'						OemToCharW						user32		0
imp	'OemToCharBuff'						OemToCharBuffW						user32		0
imp	'OffsetRect'						OffsetRect						user32		0
imp	'OpenClipboard'						OpenClipboard						user32		0
imp	'OpenDesktop'						OpenDesktopW						user32		0
imp	'OpenIcon'						OpenIcon						user32		0
imp	'OpenInputDesktop'					OpenInputDesktop					user32		0
imp	'OpenThreadDesktop'					OpenThreadDesktop					user32		0
imp	'OpenWindowStation'					OpenWindowStationW					user32		0
imp	'PackDDElParam'						PackDDElParam						user32		0
imp	'PackTouchHitTestingProximityEvaluation'		PackTouchHitTestingProximityEvaluation			user32		0
imp	'PaintDesktop'						PaintDesktop						user32		0
imp	'PaintMenuBar'						PaintMenuBar						user32		0
imp	'PaintMonitor'						PaintMonitor						user32		0
imp	'PeekMessage'						PeekMessageW						user32		0	5
imp	'PhysicalToLogicalPoint'				PhysicalToLogicalPoint					user32		0
imp	'PhysicalToLogicalPointForPerMonitorDPI'		PhysicalToLogicalPointForPerMonitorDPI			user32		0
imp	'PostMessage'						PostMessageW						user32		0
imp	'PostQuitMessage'					PostQuitMessage						user32		0	1
imp	'PostThreadMessage'					PostThreadMessageW					user32		0
imp	'PrintWindow'						PrintWindow						user32		0
imp	'PrivateExtractIconEx'					PrivateExtractIconExW					user32		0
imp	'PrivateExtractIcons'					PrivateExtractIconsW					user32		0
imp	'PrivateRegisterICSProc'				PrivateRegisterICSProc					user32		0
imp	'PtInRect'						PtInRect						user32		0
imp	'QueryBSDRWindow'					QueryBSDRWindow						user32		0
imp	'QueryDisplayConfig'					QueryDisplayConfig					user32		0
imp	'QuerySendMessage'					QuerySendMessage					user32		0
imp	'RIMAddInputObserver'					RIMAddInputObserver					user32		0
imp	'RIMAreSiblingDevices'					RIMAreSiblingDevices					user32		0
imp	'RIMDeviceIoControl'					RIMDeviceIoControl					user32		0
imp	'RIMEnableMonitorMappingForDevice'			RIMEnableMonitorMappingForDevice			user32		0
imp	'RIMFreeInputBuffer'					RIMFreeInputBuffer					user32		0
imp	'RIMGetDevicePreparsedData'				RIMGetDevicePreparsedData				user32		0
imp	'RIMGetDevicePreparsedDataLockfree'			RIMGetDevicePreparsedDataLockfree			user32		0
imp	'RIMGetDeviceProperties'				RIMGetDeviceProperties					user32		0
imp	'RIMGetDevicePropertiesLockfree'			RIMGetDevicePropertiesLockfree				user32		0
imp	'RIMGetPhysicalDeviceRect'				RIMGetPhysicalDeviceRect				user32		0
imp	'RIMGetSourceProcessId'					RIMGetSourceProcessId					user32		0
imp	'RIMObserveNextInput'					RIMObserveNextInput					user32		0
imp	'RIMOnPnpNotification'					RIMOnPnpNotification					user32		0
imp	'RIMOnTimerNotification'				RIMOnTimerNotification					user32		0
imp	'RIMReadInput'						RIMReadInput						user32		0
imp	'RIMRegisterForInput'					RIMRegisterForInput					user32		0
imp	'RIMRemoveInputObserver'				RIMRemoveInputObserver					user32		0
imp	'RIMSetTestModeStatus'					RIMSetTestModeStatus					user32		0
imp	'RIMUnregisterForInput'					RIMUnregisterForInput					user32		0
imp	'RIMUpdateInputObserverRegistration'			RIMUpdateInputObserverRegistration			user32		0
imp	'RealChildWindowFromPoint'				RealChildWindowFromPoint				user32		0
imp	'RealGetWindowClass'					RealGetWindowClassW					user32		0
imp	'ReasonCodeNeedsBugID'					ReasonCodeNeedsBugID					user32		0
imp	'ReasonCodeNeedsComment'				ReasonCodeNeedsComment					user32		0
imp	'RecordShutdownReason'					RecordShutdownReason					user32		0
imp	'RedrawWindow'						RedrawWindow						user32		0	4
imp	'RegisterBSDRWindow'					RegisterBSDRWindow					user32		0
imp	'RegisterClass'						RegisterClassW						user32		0	1
imp	'RegisterClassEx'					RegisterClassExW					user32		0	1
imp	'RegisterClipboardFormat'				RegisterClipboardFormatW				user32		0
imp	'RegisterDManipHook'					RegisterDManipHook					user32		0
imp	'RegisterDeviceNotification'				RegisterDeviceNotificationW				user32		0
imp	'RegisterErrorReportingDialog'				RegisterErrorReportingDialog				user32		0
imp	'RegisterFrostWindow'					RegisterFrostWindow					user32		0
imp	'RegisterGhostWindow'					RegisterGhostWindow					user32		0
imp	'RegisterHotKey'					RegisterHotKey						user32		0
imp	'RegisterLogonProcess'					RegisterLogonProcess					user32		0
imp	'RegisterMessagePumpHook'				RegisterMessagePumpHook					user32		0
imp	'RegisterPointerDeviceNotifications'			RegisterPointerDeviceNotifications			user32		0
imp	'RegisterPointerInputTarget'				RegisterPointerInputTarget				user32		0
imp	'RegisterPointerInputTargetEx'				RegisterPointerInputTargetEx				user32		0
imp	'RegisterPowerSettingNotification'			RegisterPowerSettingNotification			user32		0
imp	'RegisterRawInputDevices'				RegisterRawInputDevices					user32		0
imp	'RegisterServicesProcess'				RegisterServicesProcess					user32		0
imp	'RegisterSessionPort'					RegisterSessionPort					user32		0
imp	'RegisterShellHookWindow'				RegisterShellHookWindow					user32		0
imp	'RegisterSuspendResumeNotification'			RegisterSuspendResumeNotification			user32		0
imp	'RegisterSystemThread'					RegisterSystemThread					user32		0
imp	'RegisterTasklist'					RegisterTasklist					user32		0
imp	'RegisterTouchHitTestingWindow'				RegisterTouchHitTestingWindow				user32		0
imp	'RegisterTouchWindow'					RegisterTouchWindow					user32		0
imp	'RegisterUserApiHook'					RegisterUserApiHook					user32		0
imp	'RegisterWindowMessage'					RegisterWindowMessageW					user32		0
imp	'ReleaseCapture'					ReleaseCapture						user32		0	0
imp	'ReleaseDC'						ReleaseDC						user32		0	2
imp	'ReleaseDwmHitTestWaiters'				ReleaseDwmHitTestWaiters				user32		0
imp	'RemoveClipboardFormatListener'				RemoveClipboardFormatListener				user32		0
imp	'RemoveInjectionDevice'					RemoveInjectionDevice					user32		0
imp	'RemoveMenu'						RemoveMenu						user32		0
imp	'RemoveProp'						RemovePropW						user32		0
imp	'RemoveThreadTSFEventAwareness'				RemoveThreadTSFEventAwareness				user32		0
imp	'ReplyMessage'						ReplyMessage						user32		0
imp	'ReportInertia'						ReportInertia						user32		0
imp	'ResolveDesktopForWOW'					ResolveDesktopForWOW					user32		0
imp	'ReuseDDElParam'					ReuseDDElParam						user32		0
imp	'ScreenToClient'					ScreenToClient						user32		0
imp	'ScrollChildren'					ScrollChildren						user32		0
imp	'ScrollDC'						ScrollDC						user32		0
imp	'ScrollWindow'						ScrollWindow						user32		0
imp	'ScrollWindowEx'					ScrollWindowEx						user32		0
imp	'SendDlgItemMessage'					SendDlgItemMessageW					user32		0
imp	'SendIMEMessageEx'					SendIMEMessageExW					user32		0
imp	'SendInput'						SendInput						user32		0
imp	'SendMessage'						SendMessageW						user32		0	4
imp	'SendMessageCallback'					SendMessageCallbackW					user32		0
imp	'SendMessageTimeout'					SendMessageTimeoutW					user32		0
imp	'SendNotifyMessage'					SendNotifyMessageW					user32		0
imp	'SetActiveWindow'					SetActiveWindow						user32		0
imp	'SetCapture'						SetCapture						user32		0	1
imp	'SetCaretBlinkTime'					SetCaretBlinkTime					user32		0
imp	'SetCaretPos'						SetCaretPos						user32		0
imp	'SetClassLong'						SetClassLongW						user32		0	3
imp	'SetClassLongPtr'					SetClassLongPtrW					user32		0
imp	'SetClassWord'						SetClassWord						user32		0
imp	'SetClipboardData'					SetClipboardData					user32		0
imp	'SetClipboardViewer'					SetClipboardViewer					user32		0
imp	'SetCoalescableTimer'					SetCoalescableTimer					user32		0
imp	'SetCoreWindow'						SetCoreWindow						user32		0
imp	'SetCursor'						SetCursor						user32		0	1
imp	'SetCursorContents'					SetCursorContents					user32		0
imp	'SetCursorPos'						SetCursorPos						user32		0
imp	'SetDebugErrorLevel'					SetDebugErrorLevel					user32		0
imp	'SetDeskWallpaper'					SetDeskWallpaper					user32		0
imp	'SetDesktopColorTransform'				SetDesktopColorTransform				user32		0
imp	'SetDialogControlDpiChangeBehavior'			SetDialogControlDpiChangeBehavior			user32		0
imp	'SetDialogDpiChangeBehavior'				SetDialogDpiChangeBehavior				user32		0
imp	'SetDisplayAutoRotationPreferences'			SetDisplayAutoRotationPreferences			user32		0
imp	'SetDisplayConfig'					SetDisplayConfig					user32		0
imp	'SetDlgItemInt'						SetDlgItemInt						user32		0
imp	'SetDlgItemText'					SetDlgItemTextW						user32		0
imp	'SetDoubleClickTime'					SetDoubleClickTime					user32		0
imp	'SetFeatureReportResponse'				SetFeatureReportResponse				user32		0
imp	'SetFocus'						SetFocus						user32		0
imp	'SetForegroundWindow'					SetForegroundWindow					user32		0
imp	'SetGestureConfig'					SetGestureConfig					user32		0
imp	'SetInternalWindowPos'					SetInternalWindowPos					user32		0
imp	'SetKeyboardState'					SetKeyboardState					user32		0
imp	'SetLastErrorEx'					SetLastErrorEx						user32		0
imp	'SetLayeredWindowAttributes'				SetLayeredWindowAttributes				user32		0
imp	'SetMagnificationDesktopColorEffect'			SetMagnificationDesktopColorEffect			user32		0
imp	'SetMagnificationDesktopMagnification'			SetMagnificationDesktopMagnification			user32		0
imp	'SetMagnificationDesktopSamplingMode'			SetMagnificationDesktopSamplingMode			user32		0
imp	'SetMagnificationLensCtxInformation'			SetMagnificationLensCtxInformation			user32		0
imp	'SetMenu'						SetMenu							user32		0
imp	'SetMenuContextHelpId'					SetMenuContextHelpId					user32		0
imp	'SetMenuDefaultItem'					SetMenuDefaultItem					user32		0
imp	'SetMenuInfo'						SetMenuInfo						user32		0
imp	'SetMenuItemBitmaps'					SetMenuItemBitmaps					user32		0
imp	'SetMenuItemInfo'					SetMenuItemInfoW					user32		0
imp	'SetMessageExtraInfo'					SetMessageExtraInfo					user32		0
imp	'SetMessageQueue'					SetMessageQueue						user32		0
imp	'SetMirrorRendering'					SetMirrorRendering					user32		0
imp	'SetParent'						SetParent						user32		0	2
imp	'SetPhysicalCursorPos'					SetPhysicalCursorPos					user32		0
imp	'SetProcessDPIAware'					SetProcessDPIAware					user32		0
imp	'SetProcessDefaultLayout'				SetProcessDefaultLayout					user32		0
imp	'SetProcessDpiAwarenessContext'				SetProcessDpiAwarenessContext				user32		0
imp	'SetProcessDpiAwarenessInternal'			SetProcessDpiAwarenessInternal				user32		0
imp	'SetProcessRestrictionExemption'			SetProcessRestrictionExemption				user32		0
imp	'SetProcessWindowStation'				SetProcessWindowStation					user32		0
imp	'SetProgmanWindow'					SetProgmanWindow					user32		0
imp	'SetProp'						SetPropW						user32		0
imp	'SetRect'						SetRect							user32		0
imp	'SetRectEmpty'						SetRectEmpty						user32		0
imp	'SetScrollInfo'						SetScrollInfo						user32		0
imp	'SetScrollPos'						SetScrollPos						user32		0
imp	'SetScrollRange'					SetScrollRange						user32		0
imp	'SetShellWindow'					SetShellWindow						user32		0
imp	'SetShellWindowEx'					SetShellWindowEx					user32		0
imp	'SetSysColors'						SetSysColors						user32		0
imp	'SetSysColorsTemp'					SetSysColorsTemp					user32		0
imp	'SetSystemCursor'					SetSystemCursor						user32		0
imp	'SetSystemMenu'						SetSystemMenu						user32		0
imp	'SetTaskmanWindow'					SetTaskmanWindow					user32		0
imp	'SetThreadDesktop'					SetThreadDesktop					user32		0
imp	'SetThreadDpiAwarenessContext'				SetThreadDpiAwarenessContext				user32		0
imp	'SetThreadDpiHostingBehavior'				SetThreadDpiHostingBehavior				user32		0
imp	'SetThreadInputBlocked'					SetThreadInputBlocked					user32		0
imp	'SetTimer'						SetTimer						user32		0	4
imp	'SetUserObjectInformation'				SetUserObjectInformationW				user32		0
imp	'SetUserObjectSecurity'					SetUserObjectSecurity					user32		0
imp	'SetWinEventHook'					SetWinEventHook						user32		0
imp	'SetWindowBand'						SetWindowBand						user32		0
imp	'SetWindowCompositionAttribute'				SetWindowCompositionAttribute				user32		0
imp	'SetWindowCompositionTransition'			SetWindowCompositionTransition				user32		0
imp	'SetWindowContextHelpId'				SetWindowContextHelpId					user32		0
imp	'SetWindowDisplayAffinity'				SetWindowDisplayAffinity				user32		0
imp	'SetWindowFeedbackSetting'				SetWindowFeedbackSetting				user32		0
imp	'SetWindowLong'						SetWindowLongW						user32		0	3
imp	'SetWindowLongPtr'					SetWindowLongPtrW					user32		0
imp	'SetWindowPlacement'					SetWindowPlacement					user32		0	2
imp	'SetWindowPos'						SetWindowPos						user32		0	7
imp	'SetWindowRgn'						SetWindowRgn						user32		0
imp	'SetWindowRgnEx'					SetWindowRgnEx						user32		0
imp	'SetWindowStationUser'					SetWindowStationUser					user32		0
imp	'SetWindowText'						SetWindowTextW						user32		0	2
imp	'SetWindowWord'						SetWindowWord						user32		0
imp	'SetWindowsHook'					SetWindowsHookW						user32		0	2
imp	'SetWindowsHookEx'					SetWindowsHookExW					user32		0	4
imp	'ShowCaret'						ShowCaret						user32		0	1
imp	'ShowCursor'						ShowCursor						user32		0	1
imp	'ShowOwnedPopups'					ShowOwnedPopups						user32		0
imp	'ShowScrollBar'						ShowScrollBar						user32		0
imp	'ShowStartGlass'					ShowStartGlass						user32		0
imp	'ShowSystemCursor'					ShowSystemCursor					user32		0
imp	'ShowWindow'						ShowWindow						user32		0	2
imp	'ShowWindowAsync'					ShowWindowAsync						user32		0
imp	'ShutdownBlockReasonCreate'				ShutdownBlockReasonCreate				user32		0
imp	'ShutdownBlockReasonDestroy'				ShutdownBlockReasonDestroy				user32		0
imp	'ShutdownBlockReasonQuery'				ShutdownBlockReasonQuery				user32		0
imp	'SignalRedirectionStartComplete'			SignalRedirectionStartComplete				user32		0
imp	'SkipPointerFrameMessages'				SkipPointerFrameMessages				user32		0
imp	'SoftModalMessageBox'					SoftModalMessageBox					user32		0
imp	'SoundSentry'						SoundSentry						user32		0
imp	'SubtractRect'						SubtractRect						user32		0
imp	'SwapMouseButton'					SwapMouseButton						user32		0
imp	'SwitchDesktop'						SwitchDesktop						user32		0
imp	'SwitchDesktopWithFade'					SwitchDesktopWithFade					user32		0
imp	'SwitchToThisWindow'					SwitchToThisWindow					user32		0
imp	'SystemParametersInfo'					SystemParametersInfoW					user32		0
imp	'SystemParametersInfoForDpi'				SystemParametersInfoForDpi				user32		0
imp	'TabbedTextOut'						TabbedTextOutW						user32		0
imp	'TileChildWindows'					TileChildWindows					user32		0
imp	'TileWindows'						TileWindows						user32		0
imp	'ToAscii'						ToAscii							user32		0
imp	'ToAsciiEx'						ToAsciiEx						user32		0
imp	'ToUnicode'						ToUnicode						user32		0
imp	'ToUnicodeEx'						ToUnicodeEx						user32		0
imp	'TrackMouseEvent'					TrackMouseEvent						user32		0
imp	'TrackPopupMenu'					TrackPopupMenu						user32		0	7
imp	'TrackPopupMenuEx'					TrackPopupMenuEx					user32		0
imp	'TranslateAccelerator'					TranslateAcceleratorW					user32		0
imp	'TranslateMDISysAccel'					TranslateMDISysAccel					user32		0
imp	'TranslateMessage'					TranslateMessage					user32		0	1
imp	'TranslateMessageEx'					TranslateMessageEx					user32		0
imp	'UndelegateInput'					UndelegateInput						user32		0
imp	'UnhookWinEvent'					UnhookWinEvent						user32		0
imp	'UnhookWindowsHook'					UnhookWindowsHook					user32		0	2
imp	'UnhookWindowsHookEx'					UnhookWindowsHookEx					user32		0	1
imp	'UnionRect'						UnionRect						user32		0
imp	'UnloadKeyboardLayout'					UnloadKeyboardLayout					user32		0
imp	'UnlockWindowStation'					UnlockWindowStation					user32		0
imp	'UnpackDDElParam'					UnpackDDElParam						user32		0
imp	'UnregisterClass'					UnregisterClassW					user32		0
imp	'UnregisterDeviceNotification'				UnregisterDeviceNotification				user32		0
imp	'UnregisterHotKey'					UnregisterHotKey					user32		0
imp	'UnregisterMessagePumpHook'				UnregisterMessagePumpHook				user32		0
imp	'UnregisterPointerInputTarget'				UnregisterPointerInputTarget				user32		0
imp	'UnregisterPointerInputTargetEx'			UnregisterPointerInputTargetEx				user32		0
imp	'UnregisterPowerSettingNotification'			UnregisterPowerSettingNotification			user32		0
imp	'UnregisterSessionPort'					UnregisterSessionPort					user32		0
imp	'UnregisterSuspendResumeNotification'			UnregisterSuspendResumeNotification			user32		0
imp	'UnregisterTouchWindow'					UnregisterTouchWindow					user32		0
imp	'UnregisterUserApiHook'					UnregisterUserApiHook					user32		0
imp	'UpdateDefaultDesktopThumbnail'				UpdateDefaultDesktopThumbnail				user32		0
imp	'UpdateLayeredWindow'					UpdateLayeredWindow					user32		0
imp	'UpdateLayeredWindowIndirect'				UpdateLayeredWindowIndirect				user32		0
imp	'UpdatePerUserSystemParameters'				UpdatePerUserSystemParameters				user32		0
imp	'UpdateWindow'						UpdateWindow						user32		0	1
imp	'UpdateWindowInputSinkHints'				UpdateWindowInputSinkHints				user32		0
imp	'User32InitializeImmEntryTable'				User32InitializeImmEntryTable				user32		0
imp	'UserClientDllInitialize'				UserClientDllInitialize					user32		0
imp	'UserHandleGrantAccess'					UserHandleGrantAccess					user32		0
imp	'UserLpkPSMTextOut'					UserLpkPSMTextOut					user32		0
imp	'UserLpkTabbedTextOut'					UserLpkTabbedTextOut					user32		0
imp	'UserRealizePalette'					UserRealizePalette					user32		0
imp	'UserRegisterWowHandlers'				UserRegisterWowHandlers					user32		0
imp	'VRipOutput'						VRipOutput						user32		0
imp	'VTagOutput'						VTagOutput						user32		0
imp	'ValidateRect'						ValidateRect						user32		0
imp	'ValidateRgn'						ValidateRgn						user32		0
imp	'VkKeyScan'						VkKeyScanW						user32		0
imp	'VkKeyScanEx'						VkKeyScanExW						user32		0
imp	'WCSToMBEx'						WCSToMBEx						user32		0
imp	'WINNLSEnableIME'					WINNLSEnableIME						user32		0
imp	'WINNLSGetEnableStatus'					WINNLSGetEnableStatus					user32		0
imp	'WINNLSGetIMEHotkey'					WINNLSGetIMEHotkey					user32		0
imp	'WaitForInputIdle'					WaitForInputIdle					user32		0	2
imp	'WaitForRedirectionStartComplete'			WaitForRedirectionStartComplete				user32		0
imp	'WaitMessage'						WaitMessage						user32		0
imp	'WinHelp'						WinHelpW						user32		0
imp	'WindowFromDC'						WindowFromDC						user32		0
imp	'WindowFromPhysicalPoint'				WindowFromPhysicalPoint					user32		0
imp	'WindowFromPoint'					WindowFromPoint						user32		0

# GDI32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'AbortDoc'						AbortDoc						gdi32		0
imp	'AbortPath'						AbortPath						gdi32		0
imp	'AddFontMemResourceEx'					AddFontMemResourceEx					gdi32		0
imp	'AddFontResource'					AddFontResourceW					gdi32		0
imp	'AddFontResourceEx'					AddFontResourceExW					gdi32		0
imp	'AddFontResourceTracking'				AddFontResourceTracking					gdi32		0
imp	'AngleArc'						AngleArc						gdi32		0
imp	'AnimatePalette'					AnimatePalette						gdi32		0
imp	'AnyLinkedFonts'					AnyLinkedFonts						gdi32		0
imp	'Arc'							Arc							gdi32		0
imp	'ArcTo'							ArcTo							gdi32		0
imp	'BRUSHOBJ_hGetColorTransform'				BRUSHOBJ_hGetColorTransform				gdi32		0
imp	'BRUSHOBJ_pvAllocRbrush'				BRUSHOBJ_pvAllocRbrush					gdi32		0
imp	'BRUSHOBJ_pvGetRbrush'					BRUSHOBJ_pvGetRbrush					gdi32		0
imp	'BRUSHOBJ_ulGetBrushColor'				BRUSHOBJ_ulGetBrushColor				gdi32		0
imp	'BeginGdiRendering'					BeginGdiRendering					gdi32		0
imp	'BeginPath'						BeginPath						gdi32		0
imp	'BitBlt'						BitBlt							gdi32		0	9
imp	'CLIPOBJ_bEnum'						CLIPOBJ_bEnum						gdi32		0
imp	'CLIPOBJ_cEnumStart'					CLIPOBJ_cEnumStart					gdi32		0
imp	'CLIPOBJ_ppoGetPath'					CLIPOBJ_ppoGetPath					gdi32		0
imp	'CancelDC'						CancelDC						gdi32		0
imp	'CheckColorsInGamut'					CheckColorsInGamut					gdi32		0
imp	'ChoosePixelFormat'					ChoosePixelFormat					gdi32		0	2
imp	'Chord'							Chord							gdi32		0
imp	'ClearBitmapAttributes'					ClearBitmapAttributes					gdi32		0
imp	'ClearBrushAttributes'					ClearBrushAttributes					gdi32		0
imp	'CloseEnhMetaFile'					CloseEnhMetaFile					gdi32		0
imp	'CloseFigure'						CloseFigure						gdi32		0
imp	'CloseMetaFile'						CloseMetaFile						gdi32		0
imp	'ColorCorrectPalette'					ColorCorrectPalette					gdi32		0
imp	'ColorMatchToTarget'					ColorMatchToTarget					gdi32		0
imp	'CombineRgn'						CombineRgn						gdi32		0
imp	'CombineTransform'					CombineTransform					gdi32		0
imp	'ConfigureOPMProtectedOutput'				ConfigureOPMProtectedOutput				gdi32		0
imp	'CopyEnhMetaFile'					CopyEnhMetaFileW					gdi32		0
imp	'CopyMetaFile'						CopyMetaFileW						gdi32		0
imp	'CreateBitmap'						CreateBitmap						gdi32		0	5
imp	'CreateBitmapFromDxSurface'				CreateBitmapFromDxSurface				gdi32		0
imp	'CreateBitmapFromDxSurface2'				CreateBitmapFromDxSurface2				gdi32		0
imp	'CreateBitmapIndirect'					CreateBitmapIndirect					gdi32		0
imp	'CreateBrushIndirect'					CreateBrushIndirect					gdi32		0
imp	'CreateColorSpace'					CreateColorSpaceW					gdi32		0
imp	'CreateCompatibleBitmap'				CreateCompatibleBitmap					gdi32		0	3
imp	'CreateCompatibleDC'					CreateCompatibleDC					gdi32		0	1
imp	'CreateDCEx'						CreateDCExW						gdi32		0
imp	'CreateDCW'						CreateDCW						gdi32		0
imp	'CreateDIBPatternBrush'					CreateDIBPatternBrush					gdi32		0
imp	'CreateDIBPatternBrushPt'				CreateDIBPatternBrushPt					gdi32		0
imp	'CreateDIBSection'					CreateDIBSection					gdi32		0	6
imp	'CreateDIBitmap'					CreateDIBitmap						gdi32		0
imp	'CreateDPIScaledDIBSection'				CreateDPIScaledDIBSection				gdi32		0
imp	'CreateDiscardableBitmap'				CreateDiscardableBitmap					gdi32		0
imp	'CreateEllipticRgn'					CreateEllipticRgn					gdi32		0
imp	'CreateEllipticRgnIndirect'				CreateEllipticRgnIndirect				gdi32		0
imp	'CreateEnhMetaFile'					CreateEnhMetaFileW					gdi32		0
imp	'CreateFont'						CreateFontW						gdi32		0
imp	'CreateFontIndirect'					CreateFontIndirectW					gdi32		0
imp	'CreateFontIndirectEx'					CreateFontIndirectExW					gdi32		0
imp	'CreateHalftonePalette'					CreateHalftonePalette					gdi32		0
imp	'CreateHatchBrush'					CreateHatchBrush					gdi32		0
imp	'CreateICW'						CreateICW						gdi32		0
imp	'CreateMetaFile'					CreateMetaFileW						gdi32		0
imp	'CreateOPMProtectedOutput'				CreateOPMProtectedOutput				gdi32		0
imp	'CreateOPMProtectedOutputs'				CreateOPMProtectedOutputs				gdi32		0
imp	'CreatePalette'						CreatePalette						gdi32		0
imp	'CreatePatternBrush'					CreatePatternBrush					gdi32		0
imp	'CreatePen'						CreatePen						gdi32		0
imp	'CreatePenIndirect'					CreatePenIndirect					gdi32		0
imp	'CreatePolyPolygonRgn'					CreatePolyPolygonRgn					gdi32		0
imp	'CreatePolygonRgn'					CreatePolygonRgn					gdi32		0
imp	'CreateRectRgn'						CreateRectRgn						gdi32		0	4
imp	'CreateRectRgnIndirect'					CreateRectRgnIndirect					gdi32		0
imp	'CreateRoundRectRgn'					CreateRoundRectRgn					gdi32		0
imp	'CreateScalableFontResource'				CreateScalableFontResourceW				gdi32		0
imp	'CreateSessionMappedDIBSection'				CreateSessionMappedDIBSection				gdi32		0
imp	'CreateSolidBrush'					CreateSolidBrush					gdi32		0
imp	'D3DKMTAbandonSwapChain'				D3DKMTAbandonSwapChain					gdi32		0
imp	'D3DKMTAcquireKeyedMutex'				D3DKMTAcquireKeyedMutex					gdi32		0
imp	'D3DKMTAcquireKeyedMutex2'				D3DKMTAcquireKeyedMutex2				gdi32		0
imp	'D3DKMTAcquireSwapChain'				D3DKMTAcquireSwapChain					gdi32		0
imp	'D3DKMTAddSurfaceToSwapChain'				D3DKMTAddSurfaceToSwapChain				gdi32		0
imp	'D3DKMTAdjustFullscreenGamma'				D3DKMTAdjustFullscreenGamma				gdi32		0
imp	'D3DKMTCacheHybridQueryValue'				D3DKMTCacheHybridQueryValue				gdi32		0
imp	'D3DKMTChangeVideoMemoryReservation'			D3DKMTChangeVideoMemoryReservation			gdi32		0
imp	'D3DKMTCheckExclusiveOwnership'				D3DKMTCheckExclusiveOwnership				gdi32		0
imp	'D3DKMTCheckMonitorPowerState'				D3DKMTCheckMonitorPowerState				gdi32		0
imp	'D3DKMTCheckMultiPlaneOverlaySupport'			D3DKMTCheckMultiPlaneOverlaySupport			gdi32		0
imp	'D3DKMTCheckMultiPlaneOverlaySupport2'			D3DKMTCheckMultiPlaneOverlaySupport2			gdi32		0
imp	'D3DKMTCheckMultiPlaneOverlaySupport3'			D3DKMTCheckMultiPlaneOverlaySupport3			gdi32		0
imp	'D3DKMTCheckOcclusion'					D3DKMTCheckOcclusion					gdi32		0
imp	'D3DKMTCheckSharedResourceAccess'			D3DKMTCheckSharedResourceAccess				gdi32		0
imp	'D3DKMTCheckVidPnExclusiveOwnership'			D3DKMTCheckVidPnExclusiveOwnership			gdi32		0
imp	'D3DKMTCloseAdapter'					D3DKMTCloseAdapter					gdi32		0
imp	'D3DKMTConfigureSharedResource'				D3DKMTConfigureSharedResource				gdi32		0
imp	'D3DKMTCreateAllocation'				D3DKMTCreateAllocation					gdi32		0
imp	'D3DKMTCreateAllocation2'				D3DKMTCreateAllocation2					gdi32		0
imp	'D3DKMTCreateBundleObject'				D3DKMTCreateBundleObject				gdi32		0
imp	'D3DKMTCreateContext'					D3DKMTCreateContext					gdi32		0
imp	'D3DKMTCreateContextVirtual'				D3DKMTCreateContextVirtual				gdi32		0
imp	'D3DKMTCreateDCFromMemory'				D3DKMTCreateDCFromMemory				gdi32		0
imp	'D3DKMTCreateDevice'					D3DKMTCreateDevice					gdi32		0
imp	'D3DKMTCreateHwContext'					D3DKMTCreateHwContext					gdi32		0
imp	'D3DKMTCreateHwQueue'					D3DKMTCreateHwQueue					gdi32		0
imp	'D3DKMTCreateKeyedMutex'				D3DKMTCreateKeyedMutex					gdi32		0
imp	'D3DKMTCreateKeyedMutex2'				D3DKMTCreateKeyedMutex2					gdi32		0
imp	'D3DKMTCreateOutputDupl'				D3DKMTCreateOutputDupl					gdi32		0
imp	'D3DKMTCreateOverlay'					D3DKMTCreateOverlay					gdi32		0
imp	'D3DKMTCreatePagingQueue'				D3DKMTCreatePagingQueue					gdi32		0
imp	'D3DKMTCreateProtectedSession'				D3DKMTCreateProtectedSession				gdi32		0
imp	'D3DKMTCreateSwapChain'					D3DKMTCreateSwapChain					gdi32		0
imp	'D3DKMTCreateSynchronizationObject'			D3DKMTCreateSynchronizationObject			gdi32		0
imp	'D3DKMTCreateSynchronizationObject2'			D3DKMTCreateSynchronizationObject2			gdi32		0
imp	'D3DKMTDDisplayEnum'					D3DKMTDDisplayEnum					gdi32		0
imp	'D3DKMTDestroyAllocation'				D3DKMTDestroyAllocation					gdi32		0
imp	'D3DKMTDestroyAllocation2'				D3DKMTDestroyAllocation2				gdi32		0
imp	'D3DKMTDestroyContext'					D3DKMTDestroyContext					gdi32		0
imp	'D3DKMTDestroyDCFromMemory'				D3DKMTDestroyDCFromMemory				gdi32		0
imp	'D3DKMTDestroyDevice'					D3DKMTDestroyDevice					gdi32		0
imp	'D3DKMTDestroyHwContext'				D3DKMTDestroyHwContext					gdi32		0
imp	'D3DKMTDestroyHwQueue'					D3DKMTDestroyHwQueue					gdi32		0
imp	'D3DKMTDestroyKeyedMutex'				D3DKMTDestroyKeyedMutex					gdi32		0
imp	'D3DKMTDestroyOutputDupl'				D3DKMTDestroyOutputDupl					gdi32		0
imp	'D3DKMTDestroyOverlay'					D3DKMTDestroyOverlay					gdi32		0
imp	'D3DKMTDestroyPagingQueue'				D3DKMTDestroyPagingQueue				gdi32		0
imp	'D3DKMTDestroyProtectedSession'				D3DKMTDestroyProtectedSession				gdi32		0
imp	'D3DKMTDestroySynchronizationObject'			D3DKMTDestroySynchronizationObject			gdi32		0
imp	'D3DKMTDispMgrCreate'					D3DKMTDispMgrCreate					gdi32		0
imp	'D3DKMTDispMgrSourceOperation'				D3DKMTDispMgrSourceOperation				gdi32		0
imp	'D3DKMTDispMgrTargetOperation'				D3DKMTDispMgrTargetOperation				gdi32		0
imp	'D3DKMTEnumAdapters'					D3DKMTEnumAdapters					gdi32		0
imp	'D3DKMTEnumAdapters2'					D3DKMTEnumAdapters2					gdi32		0
imp	'D3DKMTEscape'						D3DKMTEscape						gdi32		0
imp	'D3DKMTEvict'						D3DKMTEvict						gdi32		0
imp	'D3DKMTExtractBundleObject'				D3DKMTExtractBundleObject				gdi32		0
imp	'D3DKMTFlipOverlay'					D3DKMTFlipOverlay					gdi32		0
imp	'D3DKMTFlushHeapTransitions'				D3DKMTFlushHeapTransitions				gdi32		0
imp	'D3DKMTFreeGpuVirtualAddress'				D3DKMTFreeGpuVirtualAddress				gdi32		0
imp	'D3DKMTGetAllocationPriority'				D3DKMTGetAllocationPriority				gdi32		0
imp	'D3DKMTGetCachedHybridQueryValue'			D3DKMTGetCachedHybridQueryValue				gdi32		0
imp	'D3DKMTGetContextInProcessSchedulingPriority'		D3DKMTGetContextInProcessSchedulingPriority		gdi32		0
imp	'D3DKMTGetContextSchedulingPriority'			D3DKMTGetContextSchedulingPriority			gdi32		0
imp	'D3DKMTGetDWMVerticalBlankEvent'			D3DKMTGetDWMVerticalBlankEvent				gdi32		0
imp	'D3DKMTGetDeviceState'					D3DKMTGetDeviceState					gdi32		0
imp	'D3DKMTGetDisplayModeList'				D3DKMTGetDisplayModeList				gdi32		0
imp	'D3DKMTGetMemoryBudgetTarget'				D3DKMTGetMemoryBudgetTarget				gdi32		0
imp	'D3DKMTGetMultiPlaneOverlayCaps'			D3DKMTGetMultiPlaneOverlayCaps				gdi32		0
imp	'D3DKMTGetMultisampleMethodList'			D3DKMTGetMultisampleMethodList				gdi32		0
imp	'D3DKMTGetOverlayState'					D3DKMTGetOverlayState					gdi32		0
imp	'D3DKMTGetPostCompositionCaps'				D3DKMTGetPostCompositionCaps				gdi32		0
imp	'D3DKMTGetPresentHistory'				D3DKMTGetPresentHistory					gdi32		0
imp	'D3DKMTGetPresentQueueEvent'				D3DKMTGetPresentQueueEvent				gdi32		0
imp	'D3DKMTGetProcessDeviceRemovalSupport'			D3DKMTGetProcessDeviceRemovalSupport			gdi32		0
imp	'D3DKMTGetProcessList'					D3DKMTGetProcessList					gdi32		0
imp	'D3DKMTGetProcessSchedulingPriorityBand'		D3DKMTGetProcessSchedulingPriorityBand			gdi32		0
imp	'D3DKMTGetProcessSchedulingPriorityClass'		D3DKMTGetProcessSchedulingPriorityClass			gdi32		0
imp	'D3DKMTGetResourcePresentPrivateDriverData'		D3DKMTGetResourcePresentPrivateDriverData		gdi32		0
imp	'D3DKMTGetRuntimeData'					D3DKMTGetRuntimeData					gdi32		0
imp	'D3DKMTGetScanLine'					D3DKMTGetScanLine					gdi32		0
imp	'D3DKMTGetSetSwapChainMetadata'				D3DKMTGetSetSwapChainMetadata				gdi32		0
imp	'D3DKMTGetSharedPrimaryHandle'				D3DKMTGetSharedPrimaryHandle				gdi32		0
imp	'D3DKMTGetSharedResourceAdapterLuid'			D3DKMTGetSharedResourceAdapterLuid			gdi32		0
imp	'D3DKMTGetYieldPercentage'				D3DKMTGetYieldPercentage				gdi32		0
imp	'D3DKMTInvalidateActiveVidPn'				D3DKMTInvalidateActiveVidPn				gdi32		0
imp	'D3DKMTInvalidateCache'					D3DKMTInvalidateCache					gdi32		0
imp	'D3DKMTLock'						D3DKMTLock						gdi32		0
imp	'D3DKMTLock2'						D3DKMTLock2						gdi32		0
imp	'D3DKMTMakeResident'					D3DKMTMakeResident					gdi32		0
imp	'D3DKMTMapGpuVirtualAddress'				D3DKMTMapGpuVirtualAddress				gdi32		0
imp	'D3DKMTMarkDeviceAsError'				D3DKMTMarkDeviceAsError					gdi32		0
imp	'D3DKMTNetDispGetNextChunkInfo'				D3DKMTNetDispGetNextChunkInfo				gdi32		0
imp	'D3DKMTNetDispQueryMiracastDisplayDeviceStatus'		D3DKMTNetDispQueryMiracastDisplayDeviceStatus		gdi32		0
imp	'D3DKMTNetDispQueryMiracastDisplayDeviceSupport'	D3DKMTNetDispQueryMiracastDisplayDeviceSupport		gdi32		0
imp	'D3DKMTNetDispStartMiracastDisplayDevice'		D3DKMTNetDispStartMiracastDisplayDevice			gdi32		0
imp	'D3DKMTNetDispStartMiracastDisplayDevice2'		D3DKMTNetDispStartMiracastDisplayDevice2		gdi32		0
imp	'D3DKMTNetDispStartMiracastDisplayDeviceEx'		D3DKMTNetDispStartMiracastDisplayDeviceEx		gdi32		0
imp	'D3DKMTNetDispStopMiracastDisplayDevice'		D3DKMTNetDispStopMiracastDisplayDevice			gdi32		0
imp	'D3DKMTNetDispStopSessions'				D3DKMTNetDispStopSessions				gdi32		0
imp	'D3DKMTOfferAllocations'				D3DKMTOfferAllocations					gdi32		0
imp	'D3DKMTOpenAdapterFromDeviceName'			D3DKMTOpenAdapterFromDeviceName				gdi32		0
imp	'D3DKMTOpenAdapterFromGdiDisplayName'			D3DKMTOpenAdapterFromGdiDisplayName			gdi32		0
imp	'D3DKMTOpenAdapterFromHdc'				D3DKMTOpenAdapterFromHdc				gdi32		0
imp	'D3DKMTOpenAdapterFromLuid'				D3DKMTOpenAdapterFromLuid				gdi32		0
imp	'D3DKMTOpenBundleObjectNtHandleFromName'		D3DKMTOpenBundleObjectNtHandleFromName			gdi32		0
imp	'D3DKMTOpenKeyedMutex'					D3DKMTOpenKeyedMutex					gdi32		0
imp	'D3DKMTOpenKeyedMutex2'					D3DKMTOpenKeyedMutex2					gdi32		0
imp	'D3DKMTOpenKeyedMutexFromNtHandle'			D3DKMTOpenKeyedMutexFromNtHandle			gdi32		0
imp	'D3DKMTOpenNtHandleFromName'				D3DKMTOpenNtHandleFromName				gdi32		0
imp	'D3DKMTOpenProtectedSessionFromNtHandle'		D3DKMTOpenProtectedSessionFromNtHandle			gdi32		0
imp	'D3DKMTOpenResource'					D3DKMTOpenResource					gdi32		0
imp	'D3DKMTOpenResource2'					D3DKMTOpenResource2					gdi32		0
imp	'D3DKMTOpenResourceFromNtHandle'			D3DKMTOpenResourceFromNtHandle				gdi32		0
imp	'D3DKMTOpenSwapChain'					D3DKMTOpenSwapChain					gdi32		0
imp	'D3DKMTOpenSyncObjectFromNtHandle'			D3DKMTOpenSyncObjectFromNtHandle			gdi32		0
imp	'D3DKMTOpenSyncObjectFromNtHandle2'			D3DKMTOpenSyncObjectFromNtHandle2			gdi32		0
imp	'D3DKMTOpenSyncObjectNtHandleFromName'			D3DKMTOpenSyncObjectNtHandleFromName			gdi32		0
imp	'D3DKMTOpenSynchronizationObject'			D3DKMTOpenSynchronizationObject				gdi32		0
imp	'D3DKMTOutputDuplGetFrameInfo'				D3DKMTOutputDuplGetFrameInfo				gdi32		0
imp	'D3DKMTOutputDuplGetMetaData'				D3DKMTOutputDuplGetMetaData				gdi32		0
imp	'D3DKMTOutputDuplGetPointerShapeData'			D3DKMTOutputDuplGetPointerShapeData			gdi32		0
imp	'D3DKMTOutputDuplPresent'				D3DKMTOutputDuplPresent					gdi32		0
imp	'D3DKMTOutputDuplReleaseFrame'				D3DKMTOutputDuplReleaseFrame				gdi32		0
imp	'D3DKMTPinDirectFlipResources'				D3DKMTPinDirectFlipResources				gdi32		0
imp	'D3DKMTPollDisplayChildren'				D3DKMTPollDisplayChildren				gdi32		0
imp	'D3DKMTPresent'						D3DKMTPresent						gdi32		0
imp	'D3DKMTPresentMultiPlaneOverlay'			D3DKMTPresentMultiPlaneOverlay				gdi32		0
imp	'D3DKMTPresentMultiPlaneOverlay2'			D3DKMTPresentMultiPlaneOverlay2				gdi32		0
imp	'D3DKMTPresentMultiPlaneOverlay3'			D3DKMTPresentMultiPlaneOverlay3				gdi32		0
imp	'D3DKMTPresentRedirected'				D3DKMTPresentRedirected					gdi32		0
imp	'D3DKMTQueryAdapterInfo'				D3DKMTQueryAdapterInfo					gdi32		0
imp	'D3DKMTQueryAllocationResidency'			D3DKMTQueryAllocationResidency				gdi32		0
imp	'D3DKMTQueryClockCalibration'				D3DKMTQueryClockCalibration				gdi32		0
imp	'D3DKMTQueryFSEBlock'					D3DKMTQueryFSEBlock					gdi32		0
imp	'D3DKMTQueryProcessOfferInfo'				D3DKMTQueryProcessOfferInfo				gdi32		0
imp	'D3DKMTQueryProtectedSessionInfoFromNtHandle'		D3DKMTQueryProtectedSessionInfoFromNtHandle		gdi32		0
imp	'D3DKMTQueryProtectedSessionStatus'			D3DKMTQueryProtectedSessionStatus			gdi32		0
imp	'D3DKMTQueryRemoteVidPnSourceFromGdiDisplayName'	D3DKMTQueryRemoteVidPnSourceFromGdiDisplayName		gdi32		0
imp	'D3DKMTQueryResourceInfo'				D3DKMTQueryResourceInfo					gdi32		0
imp	'D3DKMTQueryResourceInfoFromNtHandle'			D3DKMTQueryResourceInfoFromNtHandle			gdi32		0
imp	'D3DKMTQueryStatistics'					D3DKMTQueryStatistics					gdi32		0
imp	'D3DKMTQueryVidPnExclusiveOwnership'			D3DKMTQueryVidPnExclusiveOwnership			gdi32		0
imp	'D3DKMTQueryVideoMemoryInfo'				D3DKMTQueryVideoMemoryInfo				gdi32		0
imp	'D3DKMTReclaimAllocations'				D3DKMTReclaimAllocations				gdi32		0
imp	'D3DKMTReclaimAllocations2'				D3DKMTReclaimAllocations2				gdi32		0
imp	'D3DKMTRegisterTrimNotification'			D3DKMTRegisterTrimNotification				gdi32		0
imp	'D3DKMTRegisterVailProcess'				D3DKMTRegisterVailProcess				gdi32		0
imp	'D3DKMTReleaseKeyedMutex'				D3DKMTReleaseKeyedMutex					gdi32		0
imp	'D3DKMTReleaseKeyedMutex2'				D3DKMTReleaseKeyedMutex2				gdi32		0
imp	'D3DKMTReleaseProcessVidPnSourceOwners'			D3DKMTReleaseProcessVidPnSourceOwners			gdi32		0
imp	'D3DKMTReleaseSwapChain'				D3DKMTReleaseSwapChain					gdi32		0
imp	'D3DKMTRemoveSurfaceFromSwapChain'			D3DKMTRemoveSurfaceFromSwapChain			gdi32		0
imp	'D3DKMTRender'						D3DKMTRender						gdi32		0
imp	'D3DKMTReserveGpuVirtualAddress'			D3DKMTReserveGpuVirtualAddress				gdi32		0
imp	'D3DKMTSetAllocationPriority'				D3DKMTSetAllocationPriority				gdi32		0
imp	'D3DKMTSetContextInProcessSchedulingPriority'		D3DKMTSetContextInProcessSchedulingPriority		gdi32		0
imp	'D3DKMTSetContextSchedulingPriority'			D3DKMTSetContextSchedulingPriority			gdi32		0
imp	'D3DKMTSetDisplayMode'					D3DKMTSetDisplayMode					gdi32		0
imp	'D3DKMTSetDisplayPrivateDriverFormat'			D3DKMTSetDisplayPrivateDriverFormat			gdi32		0
imp	'D3DKMTSetDodIndirectSwapchain'				D3DKMTSetDodIndirectSwapchain				gdi32		0
imp	'D3DKMTSetFSEBlock'					D3DKMTSetFSEBlock					gdi32		0
imp	'D3DKMTSetGammaRamp'					D3DKMTSetGammaRamp					gdi32		0
imp	'D3DKMTSetHwProtectionTeardownRecovery'			D3DKMTSetHwProtectionTeardownRecovery			gdi32		0
imp	'D3DKMTSetMemoryBudgetTarget'				D3DKMTSetMemoryBudgetTarget				gdi32		0
imp	'D3DKMTSetMonitorColorSpaceTransform'			D3DKMTSetMonitorColorSpaceTransform			gdi32		0
imp	'D3DKMTSetProcessDeviceRemovalSupport'			D3DKMTSetProcessDeviceRemovalSupport			gdi32		0
imp	'D3DKMTSetProcessSchedulingPriorityBand'		D3DKMTSetProcessSchedulingPriorityBand			gdi32		0
imp	'D3DKMTSetProcessSchedulingPriorityClass'		D3DKMTSetProcessSchedulingPriorityClass			gdi32		0
imp	'D3DKMTSetQueuedLimit'					D3DKMTSetQueuedLimit					gdi32		0
imp	'D3DKMTSetStablePowerState'				D3DKMTSetStablePowerState				gdi32		0
imp	'D3DKMTSetStereoEnabled'				D3DKMTSetStereoEnabled					gdi32		0
imp	'D3DKMTSetSyncRefreshCountWaitTarget'			D3DKMTSetSyncRefreshCountWaitTarget			gdi32		0
imp	'D3DKMTSetVidPnSourceHwProtection'			D3DKMTSetVidPnSourceHwProtection			gdi32		0
imp	'D3DKMTSetVidPnSourceOwner'				D3DKMTSetVidPnSourceOwner				gdi32		0
imp	'D3DKMTSetVidPnSourceOwner1'				D3DKMTSetVidPnSourceOwner1				gdi32		0
imp	'D3DKMTSetVidPnSourceOwner2'				D3DKMTSetVidPnSourceOwner2				gdi32		0
imp	'D3DKMTSetYieldPercentage'				D3DKMTSetYieldPercentage				gdi32		0
imp	'D3DKMTShareObjects'					D3DKMTShareObjects					gdi32		0
imp	'D3DKMTSharedPrimaryLockNotification'			D3DKMTSharedPrimaryLockNotification			gdi32		0
imp	'D3DKMTSharedPrimaryUnLockNotification'			D3DKMTSharedPrimaryUnLockNotification			gdi32		0
imp	'D3DKMTSignalSynchronizationObject'			D3DKMTSignalSynchronizationObject			gdi32		0
imp	'D3DKMTSignalSynchronizationObject2'			D3DKMTSignalSynchronizationObject2			gdi32		0
imp	'D3DKMTSignalSynchronizationObjectFromCpu'		D3DKMTSignalSynchronizationObjectFromCpu		gdi32		0
imp	'D3DKMTSignalSynchronizationObjectFromGpu'		D3DKMTSignalSynchronizationObjectFromGpu		gdi32		0
imp	'D3DKMTSignalSynchronizationObjectFromGpu2'		D3DKMTSignalSynchronizationObjectFromGpu2		gdi32		0
imp	'D3DKMTSubmitCommand'					D3DKMTSubmitCommand					gdi32		0
imp	'D3DKMTSubmitCommandToHwQueue'				D3DKMTSubmitCommandToHwQueue				gdi32		0
imp	'D3DKMTSubmitPresentBltToHwQueue'			D3DKMTSubmitPresentBltToHwQueue				gdi32		0
imp	'D3DKMTSubmitSignalSyncObjectsToHwQueue'		D3DKMTSubmitSignalSyncObjectsToHwQueue			gdi32		0
imp	'D3DKMTSubmitWaitForSyncObjectsToHwQueue'		D3DKMTSubmitWaitForSyncObjectsToHwQueue			gdi32		0
imp	'D3DKMTTrimProcessCommitment'				D3DKMTTrimProcessCommitment				gdi32		0
imp	'D3DKMTUnOrderedPresentSwapChain'			D3DKMTUnOrderedPresentSwapChain				gdi32		0
imp	'D3DKMTUnlock'						D3DKMTUnlock						gdi32		0
imp	'D3DKMTUnlock2'						D3DKMTUnlock2						gdi32		0
imp	'D3DKMTUnpinDirectFlipResources'			D3DKMTUnpinDirectFlipResources				gdi32		0
imp	'D3DKMTUnregisterTrimNotification'			D3DKMTUnregisterTrimNotification			gdi32		0
imp	'D3DKMTUpdateAllocationProperty'			D3DKMTUpdateAllocationProperty				gdi32		0
imp	'D3DKMTUpdateGpuVirtualAddress'				D3DKMTUpdateGpuVirtualAddress				gdi32		0
imp	'D3DKMTUpdateOverlay'					D3DKMTUpdateOverlay					gdi32		0
imp	'D3DKMTVailConnect'					D3DKMTVailConnect					gdi32		0
imp	'D3DKMTVailDisconnect'					D3DKMTVailDisconnect					gdi32		0
imp	'D3DKMTVailPromoteCompositionSurface'			D3DKMTVailPromoteCompositionSurface			gdi32		0
imp	'D3DKMTWaitForIdle'					D3DKMTWaitForIdle					gdi32		0
imp	'D3DKMTWaitForSynchronizationObject'			D3DKMTWaitForSynchronizationObject			gdi32		0
imp	'D3DKMTWaitForSynchronizationObject2'			D3DKMTWaitForSynchronizationObject2			gdi32		0
imp	'D3DKMTWaitForSynchronizationObjectFromCpu'		D3DKMTWaitForSynchronizationObjectFromCpu		gdi32		0
imp	'D3DKMTWaitForSynchronizationObjectFromGpu'		D3DKMTWaitForSynchronizationObjectFromGpu		gdi32		0
imp	'D3DKMTWaitForVerticalBlankEvent'			D3DKMTWaitForVerticalBlankEvent				gdi32		0
imp	'D3DKMTWaitForVerticalBlankEvent2'			D3DKMTWaitForVerticalBlankEvent2			gdi32		0
imp	'DDCCIGetCapabilitiesString'				DDCCIGetCapabilitiesString				gdi32		0
imp	'DDCCIGetCapabilitiesStringLength'			DDCCIGetCapabilitiesStringLength			gdi32		0
imp	'DDCCIGetTimingReport'					DDCCIGetTimingReport					gdi32		0
imp	'DDCCIGetVCPFeature'					DDCCIGetVCPFeature					gdi32		0
imp	'DDCCISaveCurrentSettings'				DDCCISaveCurrentSettings				gdi32		0
imp	'DDCCISetVCPFeature'					DDCCISetVCPFeature					gdi32		0
imp	'DPtoLP'						DPtoLP							gdi32		0
imp	'DdCreateFullscreenSprite'				DdCreateFullscreenSprite				gdi32		0
imp	'DdDestroyFullscreenSprite'				DdDestroyFullscreenSprite				gdi32		0
imp	'DdNotifyFullscreenSpriteUpdate'			DdNotifyFullscreenSpriteUpdate				gdi32		0
imp	'DdQueryVisRgnUniqueness'				DdQueryVisRgnUniqueness					gdi32		0
imp	'DeleteColorSpace'					DeleteColorSpace					gdi32		0
imp	'DeleteDC'						DeleteDC						gdi32		0	1
imp	'DeleteEnhMetaFile'					DeleteEnhMetaFile					gdi32		0
imp	'DeleteMetaFile'					DeleteMetaFile						gdi32		0
imp	'DeleteObject'						DeleteObject						gdi32		0	1
imp	'DescribePixelFormat'					DescribePixelFormat					gdi32		0
imp	'DestroyOPMProtectedOutput'				DestroyOPMProtectedOutput				gdi32		0
imp	'DestroyPhysicalMonitorInternal'			DestroyPhysicalMonitorInternal				gdi32		0
imp	'DrawEscape'						DrawEscape						gdi32		0
imp	'DwmCreatedBitmapRemotingOutput'			DwmCreatedBitmapRemotingOutput				gdi32		0
imp	'DxTrimNotificationListHead'				DxTrimNotificationListHead				gdi32		0
imp	'Ellipse'						Ellipse							gdi32		0
imp	'EnableEUDC'						EnableEUDC						gdi32		0
imp	'EndDoc'						EndDoc							gdi32		0
imp	'EndFormPage'						EndFormPage						gdi32		0
imp	'EndGdiRendering'					EndGdiRendering						gdi32		0
imp	'EndPage'						EndPage							gdi32		0
imp	'EndPath'						EndPath							gdi32		0
imp	'EngAcquireSemaphore'					EngAcquireSemaphore					gdi32		0
imp	'EngAlphaBlend'						EngAlphaBlend						gdi32		0
imp	'EngAssociateSurface'					EngAssociateSurface					gdi32		0
imp	'EngBitBlt'						EngBitBlt						gdi32		0
imp	'EngCheckAbort'						EngCheckAbort						gdi32		0
imp	'EngComputeGlyphSet'					EngComputeGlyphSet					gdi32		0
imp	'EngCopyBits'						EngCopyBits						gdi32		0
imp	'EngCreateBitmap'					EngCreateBitmap						gdi32		0
imp	'EngCreateClip'						EngCreateClip						gdi32		0
imp	'EngCreateDeviceBitmap'					EngCreateDeviceBitmap					gdi32		0
imp	'EngCreateDeviceSurface'				EngCreateDeviceSurface					gdi32		0
imp	'EngCreatePalette'					EngCreatePalette					gdi32		0
imp	'EngCreateSemaphore'					EngCreateSemaphore					gdi32		0
imp	'EngDeleteClip'						EngDeleteClip						gdi32		0
imp	'EngDeletePalette'					EngDeletePalette					gdi32		0
imp	'EngDeletePath'						EngDeletePath						gdi32		0
imp	'EngDeleteSemaphore'					EngDeleteSemaphore					gdi32		0
imp	'EngDeleteSurface'					EngDeleteSurface					gdi32		0
imp	'EngEraseSurface'					EngEraseSurface						gdi32		0
imp	'EngFillPath'						EngFillPath						gdi32		0
imp	'EngFindResource'					EngFindResource						gdi32		0
imp	'EngFreeModule'						EngFreeModule						gdi32		0
imp	'EngGetCurrentCodePage'					EngGetCurrentCodePage					gdi32		0
imp	'EngGetDriverName'					EngGetDriverName					gdi32		0
imp	'EngGetPrinterDataFileName'				EngGetPrinterDataFileName				gdi32		0
imp	'EngGradientFill'					EngGradientFill						gdi32		0
imp	'EngLineTo'						EngLineTo						gdi32		0
imp	'EngLoadModule'						EngLoadModule						gdi32		0
imp	'EngLockSurface'					EngLockSurface						gdi32		0
imp	'EngMarkBandingSurface'					EngMarkBandingSurface					gdi32		0
imp	'EngMultiByteToUnicodeN'				EngMultiByteToUnicodeN					gdi32		0
imp	'EngMultiByteToWideChar'				EngMultiByteToWideChar					gdi32		0
imp	'EngPaint'						EngPaint						gdi32		0
imp	'EngPlgBlt'						EngPlgBlt						gdi32		0
imp	'EngQueryEMFInfo'					EngQueryEMFInfo						gdi32		0
imp	'EngQueryLocalTime'					EngQueryLocalTime					gdi32		0
imp	'EngReleaseSemaphore'					EngReleaseSemaphore					gdi32		0
imp	'EngStretchBlt'						EngStretchBlt						gdi32		0
imp	'EngStretchBltROP'					EngStretchBltROP					gdi32		0
imp	'EngStrokeAndFillPath'					EngStrokeAndFillPath					gdi32		0
imp	'EngStrokePath'						EngStrokePath						gdi32		0
imp	'EngTextOut'						EngTextOut						gdi32		0
imp	'EngTransparentBlt'					EngTransparentBlt					gdi32		0
imp	'EngUnicodeToMultiByteN'				EngUnicodeToMultiByteN					gdi32		0
imp	'EngUnlockSurface'					EngUnlockSurface					gdi32		0
imp	'EngWideCharToMultiByte'				EngWideCharToMultiByte					gdi32		0
imp	'EnumEnhMetaFile'					EnumEnhMetaFile						gdi32		0
imp	'EnumFontFamilies'					EnumFontFamiliesW					gdi32		0
imp	'EnumFontFamiliesEx'					EnumFontFamiliesExW					gdi32		0
imp	'EnumFonts'						EnumFontsW						gdi32		0
imp	'EnumICMProfiles'					EnumICMProfilesW					gdi32		0
imp	'EnumMetaFile'						EnumMetaFile						gdi32		0
imp	'EnumObjects'						EnumObjects						gdi32		0
imp	'EqualRgn'						EqualRgn						gdi32		0
imp	'Escape'						Escape							gdi32		0
imp	'EudcLoadLink'						EudcLoadLinkW						gdi32		0
imp	'EudcUnloadLink'					EudcUnloadLinkW						gdi32		0
imp	'ExcludeClipRect'					ExcludeClipRect						gdi32		0
imp	'ExtCreatePen'						ExtCreatePen						gdi32		0
imp	'ExtCreateRegion'					ExtCreateRegion						gdi32		0
imp	'ExtEscape'						ExtEscape						gdi32		0
imp	'ExtFloodFill'						ExtFloodFill						gdi32		0
imp	'ExtSelectClipRgn'					ExtSelectClipRgn					gdi32		0
imp	'ExtTextOut'						ExtTextOutW						gdi32		0
imp	'FONTOBJ_cGetAllGlyphHandles'				FONTOBJ_cGetAllGlyphHandles				gdi32		0
imp	'FONTOBJ_cGetGlyphs'					FONTOBJ_cGetGlyphs					gdi32		0
imp	'FONTOBJ_pQueryGlyphAttrs'				FONTOBJ_pQueryGlyphAttrs				gdi32		0
imp	'FONTOBJ_pfdg'						FONTOBJ_pfdg						gdi32		0
imp	'FONTOBJ_pifi'						FONTOBJ_pifi						gdi32		0
imp	'FONTOBJ_pvTrueTypeFontFile'				FONTOBJ_pvTrueTypeFontFile				gdi32		0
imp	'FONTOBJ_pxoGetXform'					FONTOBJ_pxoGetXform					gdi32		0
imp	'FONTOBJ_vGetInfo'					FONTOBJ_vGetInfo					gdi32		0
imp	'FillPath'						FillPath						gdi32		0
imp	'FillRgn'						FillRgn							gdi32		0
imp	'FixBrushOrgEx'						FixBrushOrgEx						gdi32		0
imp	'FlattenPath'						FlattenPath						gdi32		0
imp	'FloodFill'						FloodFill						gdi32		0
imp	'FontIsLinked'						FontIsLinked						gdi32		0
imp	'FrameRgn'						FrameRgn						gdi32		0
imp	'Gdi32DllInitialize'					Gdi32DllInitialize					gdi32		0
imp	'GdiAddFontResource'					GdiAddFontResourceW					gdi32		0
imp	'GdiAddGlsBounds'					GdiAddGlsBounds						gdi32		0
imp	'GdiAddGlsRecord'					GdiAddGlsRecord						gdi32		0
imp	'GdiAddInitialFonts'					GdiAddInitialFonts					gdi32		0
imp	'GdiAlphaBlend'						GdiAlphaBlend						gdi32		0
imp	'GdiArtificialDecrementDriver'				GdiArtificialDecrementDriver				gdi32		0
imp	'GdiBatchLimit'						GdiBatchLimit						gdi32		0
imp	'GdiCleanCacheDC'					GdiCleanCacheDC						gdi32		0
imp	'GdiComment'						GdiComment						gdi32		0
imp	'GdiConsoleTextOut'					GdiConsoleTextOut					gdi32		0
imp	'GdiConvertAndCheckDC'					GdiConvertAndCheckDC					gdi32		0
imp	'GdiConvertBitmap'					GdiConvertBitmap					gdi32		0
imp	'GdiConvertBitmapV5'					GdiConvertBitmapV5					gdi32		0
imp	'GdiConvertBrush'					GdiConvertBrush						gdi32		0
imp	'GdiConvertDC'						GdiConvertDC						gdi32		0
imp	'GdiConvertEnhMetaFile'					GdiConvertEnhMetaFile					gdi32		0
imp	'GdiConvertFont'					GdiConvertFont						gdi32		0
imp	'GdiConvertMetaFilePict'				GdiConvertMetaFilePict					gdi32		0
imp	'GdiConvertPalette'					GdiConvertPalette					gdi32		0
imp	'GdiConvertRegion'					GdiConvertRegion					gdi32		0
imp	'GdiConvertToDevmode'					GdiConvertToDevmodeW					gdi32		0
imp	'GdiCreateLocalEnhMetaFile'				GdiCreateLocalEnhMetaFile				gdi32		0
imp	'GdiCreateLocalMetaFilePict'				GdiCreateLocalMetaFilePict				gdi32		0
imp	'GdiDeleteLocalDC'					GdiDeleteLocalDC					gdi32		0
imp	'GdiDeleteSpoolFileHandle'				GdiDeleteSpoolFileHandle				gdi32		0
imp	'GdiDescribePixelFormat'				GdiDescribePixelFormat					gdi32		0
imp	'GdiDllInitialize'					GdiDllInitialize					gdi32		0
imp	'GdiDrawStream'						GdiDrawStream						gdi32		0
imp	'GdiEndDocEMF'						GdiEndDocEMF						gdi32		0
imp	'GdiEndPageEMF'						GdiEndPageEMF						gdi32		0
imp	'GdiFixUpHandle'					GdiFixUpHandle						gdi32		0
imp	'GdiFlush'						GdiFlush						gdi32		0
imp	'GdiFullscreenControl'					GdiFullscreenControl					gdi32		0
imp	'GdiGetBatchLimit'					GdiGetBatchLimit					gdi32		0
imp	'GdiGetBitmapBitsSize'					GdiGetBitmapBitsSize					gdi32		0
imp	'GdiGetCharDimensions'					GdiGetCharDimensions					gdi32		0
imp	'GdiGetCodePage'					GdiGetCodePage						gdi32		0
imp	'GdiGetDC'						GdiGetDC						gdi32		0
imp	'GdiGetDevmodeForPage'					GdiGetDevmodeForPage					gdi32		0
imp	'GdiGetEntry'						GdiGetEntry						gdi32		0
imp	'GdiGetLocalBrush'					GdiGetLocalBrush					gdi32		0
imp	'GdiGetLocalDC'						GdiGetLocalDC						gdi32		0
imp	'GdiGetLocalFont'					GdiGetLocalFont						gdi32		0
imp	'GdiGetPageCount'					GdiGetPageCount						gdi32		0
imp	'GdiGetPageHandle'					GdiGetPageHandle					gdi32		0
imp	'GdiGetSpoolFileHandle'					GdiGetSpoolFileHandle					gdi32		0
imp	'GdiGetSpoolMessage'					GdiGetSpoolMessage					gdi32		0
imp	'GdiGetVariationStoreDelta'				GdiGetVariationStoreDelta				gdi32		0
imp	'GdiGradientFill'					GdiGradientFill						gdi32		0
imp	'GdiInitSpool'						GdiInitSpool						gdi32		0
imp	'GdiInitializeLanguagePack'				GdiInitializeLanguagePack				gdi32		0
imp	'GdiIsMetaFileDC'					GdiIsMetaFileDC						gdi32		0
imp	'GdiIsMetaPrintDC'					GdiIsMetaPrintDC					gdi32		0
imp	'GdiIsPlayMetafileDC'					GdiIsPlayMetafileDC					gdi32		0
imp	'GdiIsScreenDC'						GdiIsScreenDC						gdi32		0
imp	'GdiIsTrackingEnabled'					GdiIsTrackingEnabled					gdi32		0
imp	'GdiIsUMPDSandboxingEnabled'				GdiIsUMPDSandboxingEnabled				gdi32		0
imp	'GdiLoadType1Fonts'					GdiLoadType1Fonts					gdi32		0
imp	'GdiPlayDCScript'					GdiPlayDCScript						gdi32		0
imp	'GdiPlayEMF'						GdiPlayEMF						gdi32		0
imp	'GdiPlayJournal'					GdiPlayJournal						gdi32		0
imp	'GdiPlayPageEMF'					GdiPlayPageEMF						gdi32		0
imp	'GdiPlayPrivatePageEMF'					GdiPlayPrivatePageEMF					gdi32		0
imp	'GdiPlayScript'						GdiPlayScript						gdi32		0
imp	'GdiPrinterThunk'					GdiPrinterThunk						gdi32		0
imp	'GdiProcessSetup'					GdiProcessSetup						gdi32		0
imp	'GdiQueryFonts'						GdiQueryFonts						gdi32		0
imp	'GdiQueryTable'						GdiQueryTable						gdi32		0
imp	'GdiRealizationInfo'					GdiRealizationInfo					gdi32		0
imp	'GdiReleaseDC'						GdiReleaseDC						gdi32		0
imp	'GdiReleaseLocalDC'					GdiReleaseLocalDC					gdi32		0
imp	'GdiResetDCEMF'						GdiResetDCEMF						gdi32		0
imp	'GdiSetAttrs'						GdiSetAttrs						gdi32		0
imp	'GdiSetBatchLimit'					GdiSetBatchLimit					gdi32		0
imp	'GdiSetLastError'					GdiSetLastError						gdi32		0
imp	'GdiSetPixelFormat'					GdiSetPixelFormat					gdi32		0
imp	'GdiSetServerAttr'					GdiSetServerAttr					gdi32		0
imp	'GdiStartDocEMF'					GdiStartDocEMF						gdi32		0
imp	'GdiStartPageEMF'					GdiStartPageEMF						gdi32		0
imp	'GdiSupportsFontChangeEvent'				GdiSupportsFontChangeEvent				gdi32		0
imp	'GdiSwapBuffers'					GdiSwapBuffers						gdi32		0
imp	'GdiTrackHCreate'					GdiTrackHCreate						gdi32		0
imp	'GdiTrackHDelete'					GdiTrackHDelete						gdi32		0
imp	'GdiTransparentBlt'					GdiTransparentBlt					gdi32		0
imp	'GdiValidateHandle'					GdiValidateHandle					gdi32		0
imp	'GetArcDirection'					GetArcDirection						gdi32		0
imp	'GetAspectRatioFilterEx'				GetAspectRatioFilterEx					gdi32		0
imp	'GetBitmapAttributes'					GetBitmapAttributes					gdi32		0
imp	'GetBitmapBits'						GetBitmapBits						gdi32		0
imp	'GetBitmapDimensionEx'					GetBitmapDimensionEx					gdi32		0
imp	'GetBitmapDpiScaleValue'				GetBitmapDpiScaleValue					gdi32		0
imp	'GetBkColor'						GetBkColor						gdi32		0
imp	'GetBkMode'						GetBkMode						gdi32		0
imp	'GetBoundsRect'						GetBoundsRect						gdi32		0
imp	'GetBrushAttributes'					GetBrushAttributes					gdi32		0
imp	'GetBrushOrgEx'						GetBrushOrgEx						gdi32		0
imp	'GetCOPPCompatibleOPMInformation'			GetCOPPCompatibleOPMInformation				gdi32		0
imp	'GetCertificate'					GetCertificate						gdi32		0
imp	'GetCertificateByHandle'				GetCertificateByHandle					gdi32		0
imp	'GetCertificateSize'					GetCertificateSize					gdi32		0
imp	'GetCertificateSizeByHandle'				GetCertificateSizeByHandle				gdi32		0
imp	'GetCharABCWidths'					GetCharABCWidthsW					gdi32		0
imp	'GetCharABCWidthsFloat'					GetCharABCWidthsFloatW					gdi32		0
imp	'GetCharABCWidthsFloatI'				GetCharABCWidthsFloatI					gdi32		0
imp	'GetCharABCWidthsI'					GetCharABCWidthsI					gdi32		0
imp	'GetCharWidth'						GetCharWidthW						gdi32		0
imp	'GetCharWidth32W'					GetCharWidth32W						gdi32		0
imp	'GetCharWidthFloat'					GetCharWidthFloatW					gdi32		0
imp	'GetCharWidthI'						GetCharWidthI						gdi32		0
imp	'GetCharWidthInfo'					GetCharWidthInfo					gdi32		0
imp	'GetCharacterPlacement'					GetCharacterPlacementW					gdi32		0
imp	'GetClipBox'						GetClipBox						gdi32		0
imp	'GetClipRgn'						GetClipRgn						gdi32		0
imp	'GetColorAdjustment'					GetColorAdjustment					gdi32		0
imp	'GetColorSpace'						GetColorSpace						gdi32		0
imp	'GetCurrentDpiInfo'					GetCurrentDpiInfo					gdi32		0
imp	'GetCurrentObject'					GetCurrentObject					gdi32		0
imp	'GetCurrentPositionEx'					GetCurrentPositionEx					gdi32		0
imp	'GetDCBrushColor'					GetDCBrushColor						gdi32		0
imp	'GetDCDpiScaleValue'					GetDCDpiScaleValue					gdi32		0
imp	'GetDCOrgEx'						GetDCOrgEx						gdi32		0
imp	'GetDCPenColor'						GetDCPenColor						gdi32		0
imp	'GetDIBColorTable'					GetDIBColorTable					gdi32		0
imp	'GetDIBits'						GetDIBits						gdi32		0
imp	'GetDeviceCaps'						GetDeviceCaps						gdi32		0
imp	'GetDeviceGammaRamp'					GetDeviceGammaRamp					gdi32		0
imp	'GetETM'						GetETM							gdi32		0
imp	'GetEUDCTimeStamp'					GetEUDCTimeStamp					gdi32		0
imp	'GetEUDCTimeStampEx'					GetEUDCTimeStampExW					gdi32		0
imp	'GetEnhMetaFile'					GetEnhMetaFileW						gdi32		0
imp	'GetEnhMetaFileBits'					GetEnhMetaFileBits					gdi32		0
imp	'GetEnhMetaFileDescription'				GetEnhMetaFileDescriptionW				gdi32		0
imp	'GetEnhMetaFileHeader'					GetEnhMetaFileHeader					gdi32		0
imp	'GetEnhMetaFilePaletteEntries'				GetEnhMetaFilePaletteEntries				gdi32		0
imp	'GetEnhMetaFilePixelFormat'				GetEnhMetaFilePixelFormat				gdi32		0
imp	'GetFontAssocStatus'					GetFontAssocStatus					gdi32		0
imp	'GetFontData'						GetFontData						gdi32		0
imp	'GetFontFileData'					GetFontFileData						gdi32		0
imp	'GetFontFileInfo'					GetFontFileInfo						gdi32		0
imp	'GetFontLanguageInfo'					GetFontLanguageInfo					gdi32		0
imp	'GetFontRealizationInfo'				GetFontRealizationInfo					gdi32		0
imp	'GetFontResourceInfo'					GetFontResourceInfoW					gdi32		0
imp	'GetFontUnicodeRanges'					GetFontUnicodeRanges					gdi32		0
imp	'GetGlyphIndices'					GetGlyphIndicesW					gdi32		0
imp	'GetGlyphOutline'					GetGlyphOutlineW					gdi32		0
imp	'GetGlyphOutlineWow'					GetGlyphOutlineWow					gdi32		0
imp	'GetGraphicsMode'					GetGraphicsMode						gdi32		0
imp	'GetHFONT'						GetHFONT						gdi32		0
imp	'GetICMProfile'						GetICMProfileW						gdi32		0
imp	'GetKerningPairs'					GetKerningPairsW					gdi32		0
imp	'GetLayout'						GetLayout						gdi32		0
imp	'GetLogColorSpace'					GetLogColorSpaceW					gdi32		0
imp	'GetMapMode'						GetMapMode						gdi32		0
imp	'GetMetaFile'						GetMetaFileW						gdi32		0
imp	'GetMetaFileBitsEx'					GetMetaFileBitsEx					gdi32		0
imp	'GetMetaRgn'						GetMetaRgn						gdi32		0
imp	'GetMiterLimit'						GetMiterLimit						gdi32		0
imp	'GetNearestColor'					GetNearestColor						gdi32		0
imp	'GetNearestPaletteIndex'				GetNearestPaletteIndex					gdi32		0
imp	'GetNumberOfPhysicalMonitors'				GetNumberOfPhysicalMonitors				gdi32		0
imp	'GetOPMInformation'					GetOPMInformation					gdi32		0
imp	'GetOPMRandomNumber'					GetOPMRandomNumber					gdi32		0
imp	'GetObject'						GetObjectW						gdi32		0
imp	'GetObjectType'						GetObjectType						gdi32		0
imp	'GetOutlineTextMetrics'					GetOutlineTextMetricsW					gdi32		0
imp	'GetPaletteEntries'					GetPaletteEntries					gdi32		0
imp	'GetPath'						GetPath							gdi32		0
imp	'GetPhysicalMonitorDescription'				GetPhysicalMonitorDescription				gdi32		0
imp	'GetPhysicalMonitors'					GetPhysicalMonitors					gdi32		0
imp	'GetPixel'						GetPixel						gdi32		0	3
imp	'GetPixelFormat'					GetPixelFormat						gdi32		0
imp	'GetPolyFillMode'					GetPolyFillMode						gdi32		0
imp	'GetProcessSessionFonts'				GetProcessSessionFonts					gdi32		0
imp	'GetROP2'						GetROP2							gdi32		0
imp	'GetRandomRgn'						GetRandomRgn						gdi32		0
imp	'GetRasterizerCaps'					GetRasterizerCaps					gdi32		0
imp	'GetRegionData'						GetRegionData						gdi32		0
imp	'GetRelAbs'						GetRelAbs						gdi32		0
imp	'GetRgnBox'						GetRgnBox						gdi32		0
imp	'GetStockObject'					GetStockObject						gdi32		0
imp	'GetStretchBltMode'					GetStretchBltMode					gdi32		0
imp	'GetStringBitmap'					GetStringBitmapW					gdi32		0
imp	'GetSuggestedOPMProtectedOutputArraySize'		GetSuggestedOPMProtectedOutputArraySize			gdi32		0
imp	'GetSystemPaletteEntries'				GetSystemPaletteEntries					gdi32		0
imp	'GetSystemPaletteUse'					GetSystemPaletteUse					gdi32		0
imp	'GetTextAlign'						GetTextAlign						gdi32		0
imp	'GetTextCharacterExtra'					GetTextCharacterExtra					gdi32		0
imp	'GetTextCharset'					GetTextCharset						gdi32		0
imp	'GetTextCharsetInfo'					GetTextCharsetInfo					gdi32		0
imp	'GetTextColor'						GetTextColor						gdi32		0
imp	'GetTextExtentExPoint'					GetTextExtentExPointW					gdi32		0
imp	'GetTextExtentExPointI'					GetTextExtentExPointI					gdi32		0
imp	'GetTextExtentExPointWPri'				GetTextExtentExPointWPri				gdi32		0
imp	'GetTextExtentPoint'					GetTextExtentPointW					gdi32		0
imp	'GetTextExtentPoint32W'					GetTextExtentPoint32W					gdi32		0
imp	'GetTextExtentPointI'					GetTextExtentPointI					gdi32		0
imp	'GetTextFace'						GetTextFaceW						gdi32		0
imp	'GetTextFaceAlias'					GetTextFaceAliasW					gdi32		0
imp	'GetTextMetrics'					GetTextMetricsW						gdi32		0
imp	'GetTransform'						GetTransform						gdi32		0
imp	'GetViewportExtEx'					GetViewportExtEx					gdi32		0
imp	'GetViewportOrgEx'					GetViewportOrgEx					gdi32		0
imp	'GetWinMetaFileBits'					GetWinMetaFileBits					gdi32		0
imp	'GetWindowExtEx'					GetWindowExtEx						gdi32		0
imp	'GetWindowOrgEx'					GetWindowOrgEx						gdi32		0
imp	'GetWorldTransform'					GetWorldTransform					gdi32		0
imp	'HT_Get8BPPFormatPalette'				HT_Get8BPPFormatPalette					gdi32		0
imp	'HT_Get8BPPMaskPalette'					HT_Get8BPPMaskPalette					gdi32		0
imp	'InternalDeleteDC'					InternalDeleteDC					gdi32		0
imp	'IntersectClipRect'					IntersectClipRect					gdi32		0
imp	'InvertRgn'						InvertRgn						gdi32		0
imp	'IsValidEnhMetaRecord'					IsValidEnhMetaRecord					gdi32		0
imp	'IsValidEnhMetaRecordOffExt'				IsValidEnhMetaRecordOffExt				gdi32		0
imp	'LPtoDP'						LPtoDP							gdi32		0
imp	'LineTo'						LineTo							gdi32		0
imp	'LpkEditControl'					LpkEditControl						gdi32		0
imp	'LpkGetEditControl'					LpkGetEditControl					gdi32		0
imp	'LpkpEditControlSize'					LpkpEditControlSize					gdi32		0
imp	'LpkpInitializeEditControl'				LpkpInitializeEditControl				gdi32		0
imp	'MaskBlt'						MaskBlt							gdi32		0
imp	'MirrorRgn'						MirrorRgn						gdi32		0
imp	'ModerncoreGdiInit'					ModerncoreGdiInit					gdi32		0
imp	'ModifyWorldTransform'					ModifyWorldTransform					gdi32		0
imp	'MoveToEx'						MoveToEx						gdi32		0
imp	'NamedEscape'						NamedEscape						gdi32		0
imp	'OffsetClipRgn'						OffsetClipRgn						gdi32		0
imp	'OffsetRgn'						OffsetRgn						gdi32		0
imp	'OffsetViewportOrgEx'					OffsetViewportOrgEx					gdi32		0
imp	'OffsetWindowOrgEx'					OffsetWindowOrgEx					gdi32		0
imp	'PATHOBJ_bEnum'						PATHOBJ_bEnum						gdi32		0
imp	'PATHOBJ_bEnumClipLines'				PATHOBJ_bEnumClipLines					gdi32		0
imp	'PATHOBJ_vEnumStart'					PATHOBJ_vEnumStart					gdi32		0
imp	'PATHOBJ_vEnumStartClipLines'				PATHOBJ_vEnumStartClipLines				gdi32		0
imp	'PATHOBJ_vGetBounds'					PATHOBJ_vGetBounds					gdi32		0
imp	'PaintRgn'						PaintRgn						gdi32		0
imp	'PatBlt'						PatBlt							gdi32		0
imp	'PathToRegion'						PathToRegion						gdi32		0
imp	'Pie'							Pie							gdi32		0
imp	'PlayEnhMetaFile'					PlayEnhMetaFile						gdi32		0
imp	'PlayEnhMetaFileRecord'					PlayEnhMetaFileRecord					gdi32		0
imp	'PlayMetaFile'						PlayMetaFile						gdi32		0
imp	'PlayMetaFileRecord'					PlayMetaFileRecord					gdi32		0
imp	'PlgBlt'						PlgBlt							gdi32		0
imp	'PolyBezier'						PolyBezier						gdi32		0
imp	'PolyBezierTo'						PolyBezierTo						gdi32		0
imp	'PolyDraw'						PolyDraw						gdi32		0
imp	'PolyPatBlt'						PolyPatBlt						gdi32		0
imp	'PolyPolygon'						PolyPolygon						gdi32		0
imp	'PolyPolyline'						PolyPolyline						gdi32		0
imp	'PolyTextOut'						PolyTextOutW						gdi32		0
imp	'Polygon'						Polygon							gdi32		0
imp	'Polyline'						Polyline						gdi32		0
imp	'PolylineTo'						PolylineTo						gdi32		0
imp	'PtInRegion'						PtInRegion						gdi32		0
imp	'PtVisible'						PtVisible						gdi32		0
imp	'QueryFontAssocStatus'					QueryFontAssocStatus					gdi32		0
imp	'RealizePalette'					RealizePalette						gdi32		0
imp	'RectInRegion'						RectInRegion						gdi32		0
imp	'RectVisible'						RectVisible						gdi32		0
imp	'Rectangle'						Rectangle						gdi32		0
imp	'RemoveFontMemResourceEx'				RemoveFontMemResourceEx					gdi32		0
imp	'RemoveFontResource'					RemoveFontResourceW					gdi32		0
imp	'RemoveFontResourceEx'					RemoveFontResourceExW					gdi32		0
imp	'RemoveFontResourceTracking'				RemoveFontResourceTracking				gdi32		0
imp	'ResetDCW'						ResetDCW						gdi32		0
imp	'ResizePalette'						ResizePalette						gdi32		0
imp	'RestoreDC'						RestoreDC						gdi32		0	2
imp	'RoundRect'						RoundRect						gdi32		0
imp	'STROBJ_bEnum'						STROBJ_bEnum						gdi32		0
imp	'STROBJ_bEnumPositionsOnly'				STROBJ_bEnumPositionsOnly				gdi32		0
imp	'STROBJ_bGetAdvanceWidths'				STROBJ_bGetAdvanceWidths				gdi32		0
imp	'STROBJ_dwGetCodePage'					STROBJ_dwGetCodePage					gdi32		0
imp	'STROBJ_vEnumStart'					STROBJ_vEnumStart					gdi32		0
imp	'SaveDC'						SaveDC							gdi32		0	1
imp	'ScaleRgn'						ScaleRgn						gdi32		0
imp	'ScaleValues'						ScaleValues						gdi32		0
imp	'ScaleViewportExtEx'					ScaleViewportExtEx					gdi32		0
imp	'ScaleWindowExtEx'					ScaleWindowExtEx					gdi32		0
imp	'SelectBrushLocal'					SelectBrushLocal					gdi32		0
imp	'SelectClipPath'					SelectClipPath						gdi32		0
imp	'SelectClipRgn'						SelectClipRgn						gdi32		0
imp	'SelectFontLocal'					SelectFontLocal						gdi32		0
imp	'SelectObject'						SelectObject						gdi32		0	2
imp	'SelectPalette'						SelectPalette						gdi32		0
imp	'SetAbortProc'						SetAbortProc						gdi32		0
imp	'SetArcDirection'					SetArcDirection						gdi32		0
imp	'SetBitmapAttributes'					SetBitmapAttributes					gdi32		0
imp	'SetBitmapBits'						SetBitmapBits						gdi32		0
imp	'SetBitmapDimensionEx'					SetBitmapDimensionEx					gdi32		0
imp	'SetBkColor'						SetBkColor						gdi32		0
imp	'SetBkMode'						SetBkMode						gdi32		0	2
imp	'SetBoundsRect'						SetBoundsRect						gdi32		0
imp	'SetBrushAttributes'					SetBrushAttributes					gdi32		0
imp	'SetBrushOrgEx'						SetBrushOrgEx						gdi32		0
imp	'SetColorAdjustment'					SetColorAdjustment					gdi32		0
imp	'SetColorSpace'						SetColorSpace						gdi32		0
imp	'SetDCBrushColor'					SetDCBrushColor						gdi32		0
imp	'SetDCDpiScaleValue'					SetDCDpiScaleValue					gdi32		0
imp	'SetDCPenColor'						SetDCPenColor						gdi32		0
imp	'SetDIBColorTable'					SetDIBColorTable					gdi32		0
imp	'SetDIBits'						SetDIBits						gdi32		0
imp	'SetDIBitsToDevice'					SetDIBitsToDevice					gdi32		0
imp	'SetDeviceGammaRamp'					SetDeviceGammaRamp					gdi32		0
imp	'SetEnhMetaFileBits'					SetEnhMetaFileBits					gdi32		0
imp	'SetFontEnumeration'					SetFontEnumeration					gdi32		0
imp	'SetGraphicsMode'					SetGraphicsMode						gdi32		0
imp	'SetICMMode'						SetICMMode						gdi32		0
imp	'SetICMProfile'						SetICMProfileW						gdi32		0
imp	'SetLayout'						SetLayout						gdi32		0
imp	'SetLayoutWidth'					SetLayoutWidth						gdi32		0
imp	'SetMagicColors'					SetMagicColors						gdi32		0
imp	'SetMapMode'						SetMapMode						gdi32		0
imp	'SetMapperFlags'					SetMapperFlags						gdi32		0
imp	'SetMetaFileBitsEx'					SetMetaFileBitsEx					gdi32		0
imp	'SetMetaRgn'						SetMetaRgn						gdi32		0
imp	'SetMiterLimit'						SetMiterLimit						gdi32		0
imp	'SetOPMSigningKeyAndSequenceNumbers'			SetOPMSigningKeyAndSequenceNumbers			gdi32		0
imp	'SetPaletteEntries'					SetPaletteEntries					gdi32		0
imp	'SetPixel'						SetPixel						gdi32		0	4
imp	'SetPixelFormat'					SetPixelFormat						gdi32		0	3
imp	'SetPixelV'						SetPixelV						gdi32		0
imp	'SetPolyFillMode'					SetPolyFillMode						gdi32		0
imp	'SetROP2'						SetROP2							gdi32		0
imp	'SetRectRgn'						SetRectRgn						gdi32		0
imp	'SetRelAbs'						SetRelAbs						gdi32		0
imp	'SetStretchBltMode'					SetStretchBltMode					gdi32		0
imp	'SetSystemPaletteUse'					SetSystemPaletteUse					gdi32		0
imp	'SetTextAlign'						SetTextAlign						gdi32		0	2
imp	'SetTextCharacterExtra'					SetTextCharacterExtra					gdi32		0
imp	'SetTextColor'						SetTextColor						gdi32		0	2
imp	'SetTextJustification'					SetTextJustification					gdi32		0	3
imp	'SetViewportExtEx'					SetViewportExtEx					gdi32		0
imp	'SetViewportOrgEx'					SetViewportOrgEx					gdi32		0
imp	'SetVirtualResolution'					SetVirtualResolution					gdi32		0
imp	'SetWinMetaFileBits'					SetWinMetaFileBits					gdi32		0
imp	'SetWindowExtEx'					SetWindowExtEx						gdi32		0
imp	'SetWindowOrgEx'					SetWindowOrgEx						gdi32		0
imp	'SetWorldTransform'					SetWorldTransform					gdi32		0
imp	'StartDoc'						StartDocW						gdi32		0
imp	'StartFormPage'						StartFormPage						gdi32		0
imp	'StartPage'						StartPage						gdi32		0
imp	'StretchBlt'						StretchBlt						gdi32		0
imp	'StretchDIBits'						StretchDIBits						gdi32		0
imp	'StrokeAndFillPath'					StrokeAndFillPath					gdi32		0
imp	'StrokePath'						StrokePath						gdi32		0
imp	'SwapBuffers'						SwapBuffers						gdi32		0	1
imp	'TextOut'						TextOutW						gdi32		0
imp	'TranslateCharsetInfo'					TranslateCharsetInfo					gdi32		0
imp	'UnloadNetworkFonts'					UnloadNetworkFonts					gdi32		0
imp	'UnrealizeObject'					UnrealizeObject						gdi32		0
imp	'UpdateColors'						UpdateColors						gdi32		0
imp	'UpdateICMRegKey'					UpdateICMRegKeyW					gdi32		0
imp	'WidenPath'						WidenPath						gdi32		0
imp	'XFORMOBJ_bApplyXform'					XFORMOBJ_bApplyXform					gdi32		0
imp	'XFORMOBJ_iGetXform'					XFORMOBJ_iGetXform					gdi32		0
imp	'XLATEOBJ_cGetPalette'					XLATEOBJ_cGetPalette					gdi32		0
imp	'XLATEOBJ_hGetColorTransform'				XLATEOBJ_hGetColorTransform				gdi32		0
imp	'XLATEOBJ_iXlate'					XLATEOBJ_iXlate						gdi32		0
imp	'XLATEOBJ_piVector'					XLATEOBJ_piVector					gdi32		0
imp	'cGetTTFFromFOT'					cGetTTFFromFOT						gdi32		0
imp	'fpClosePrinter'					fpClosePrinter						gdi32		0
imp	'gMaxGdiHandleCount'					gMaxGdiHandleCount					gdi32		0
imp	'gW32PID'						gW32PID							gdi32		0
imp	'g_systemCallFilterId'					g_systemCallFilterId					gdi32		0
imp	'gdiPlaySpoolStream'					gdiPlaySpoolStream					gdi32		0

# COMDLG32.DLL
#
#	Name							Actual							DLL		Hint	Arity
imp	'ChooseColor'						ChooseColorW						comdlg32	0	1
imp	'ChooseFont'						ChooseFontW						comdlg32	0	1
imp	'CommDlgExtendedError'					CommDlgExtendedError					comdlg32	0
imp	'DllCanUnloadNow'					DllCanUnloadNow						comdlg32	0
imp	'DllGetClassObject'					DllGetClassObject					comdlg32	0
imp	'FindText'						FindTextW						comdlg32	0
imp	'GetFileTitle'						GetFileTitleW						comdlg32	0	3
imp	'GetOpenFileName'					GetOpenFileNameW					comdlg32	0	1
imp	'GetSaveFileName'					GetSaveFileNameW					comdlg32	0	1
imp	'LoadAlterBitmap'					LoadAlterBitmap						comdlg32	0
imp	'PageSetupDlg'						PageSetupDlgW						comdlg32	0
imp	'PrintDlg'						PrintDlgW						comdlg32	0	1
imp	'PrintDlgEx'						PrintDlgExW						comdlg32	0
imp	'ReplaceText'						ReplaceTextW						comdlg32	0	1
imp	'Ssync_ANSI_UNICODE_Struct_For_WOW'			Ssync_ANSI_UNICODE_Struct_For_WOW			comdlg32	0
imp	'WantArrows'						WantArrows						comdlg32	0

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
imp	'FreeAddrInfo'						FreeAddrInfoW						ws2_32		0	1
imp	'FreeAddrInfoEx'					FreeAddrInfoExW						ws2_32		0	1
imp	'GetAddrInfo'						GetAddrInfoW						ws2_32		0	4
imp	'GetAddrInfoEx'						GetAddrInfoExW						ws2_32		0	10
imp	'GetAddrInfoExCancel'					GetAddrInfoExCancel					ws2_32		0	1
imp	'GetAddrInfoExOverlappedResult'				GetAddrInfoExOverlappedResult				ws2_32		0	1
imp	'GetHostName'						GetHostNameW						ws2_32		0	2
imp	'GetNameInfo'						GetNameInfoW						ws2_32		0	7
imp	'SetAddrInfoEx'						SetAddrInfoExW						ws2_32		0	12
imp	'WSAAccept'						WSAAccept						ws2_32		0	5
imp	'WSAAddressToString'					WSAAddressToStringW					ws2_32		0	5
imp	'WSAAsyncGetHostByAddr'					WSAAsyncGetHostByAddr					ws2_32		0	7
imp	'WSAAsyncGetHostByName'					WSAAsyncGetHostByName					ws2_32		0	5
imp	'WSAAsyncGetProtoByName'				WSAAsyncGetProtoByName					ws2_32		0	5
imp	'WSAAsyncGetProtoByNumber'				WSAAsyncGetProtoByNumber				ws2_32		0	5
imp	'WSAAsyncGetServByName'					WSAAsyncGetServByName					ws2_32		0
imp	'WSAAsyncGetServByPort'					WSAAsyncGetServByPort					ws2_32		0
imp	'WSAAsyncSelect'					WSAAsyncSelect						ws2_32		0
imp	'WSACancelAsyncRequest'					WSACancelAsyncRequest					ws2_32		0
imp	'WSACancelBlockingCall'					WSACancelBlockingCall					ws2_32		0
imp	'WSACleanup'						WSACleanup						ws2_32		0	0
imp	'WSACloseEvent'						WSACloseEvent						ws2_32		0	1
imp	'WSAConnect'						WSAConnect						ws2_32		0	7
imp	'WSAConnectByList'					WSAConnectByList					ws2_32		0	8
imp	'WSAConnectByName'					WSAConnectByNameW					ws2_32		0	9
imp	'WSACreateEvent'					WSACreateEvent						ws2_32		0	0
imp	'WSADuplicateSocket'					WSADuplicateSocketW					ws2_32		0	3
imp	'WSAEnumNameSpaceProviders'				WSAEnumNameSpaceProvidersW				ws2_32		0	2
imp	'WSAEnumNameSpaceProvidersEx'				WSAEnumNameSpaceProvidersExW				ws2_32		0	2
imp	'WSAEnumNetworkEvents'					WSAEnumNetworkEvents					ws2_32		0	3
imp	'WSAEnumProtocols'					WSAEnumProtocolsW					ws2_32		0	3
imp	'WSAEventSelect'					WSAEventSelect						ws2_32		0	3
imp	'WSAGetLastError'					WSAGetLastError						ws2_32		0	0
imp	'WSAGetQOSByName'					WSAGetQOSByName						ws2_32		0	3
imp	'WSAGetServiceClassInfo'				WSAGetServiceClassInfoW					ws2_32		0	4
imp	'WSAGetServiceClassNameByClassId'			WSAGetServiceClassNameByClassIdW			ws2_32		0	3
imp	'WSAInstallServiceClass'				WSAInstallServiceClassW					ws2_32		0	1
imp	'WSAIoctl'						WSAIoctl						ws2_32		0	9
imp	'WSAJoinLeaf'						WSAJoinLeaf						ws2_32		0	8
imp	'WSALookupServiceBegin'					WSALookupServiceBeginW					ws2_32		0	3
imp	'WSALookupServiceEnd'					WSALookupServiceEnd					ws2_32		0	1
imp	'WSALookupServiceNext'					WSALookupServiceNextW					ws2_32		0	4
imp	'WSANSPIoctl'						WSANSPIoctl						ws2_32		0	8
imp	'WSAPoll'						WSAPoll							ws2_32		0	3
imp	'WSAProviderCompleteAsyncCall'				WSAProviderCompleteAsyncCall				ws2_32		0
imp	'WSAProviderConfigChange'				WSAProviderConfigChange					ws2_32		0	3
imp	'WSARecvDisconnect'					WSARecvDisconnect					ws2_32		0	2
imp	'WSARemoveServiceClass'					WSARemoveServiceClass					ws2_32		0	1
imp	'WSAResetEvent'						WSAResetEvent						ws2_32		0	1
imp	'WSASend'						WSASend							ws2_32		0	7
imp	'WSASendDisconnect'					WSASendDisconnect					ws2_32		0	2
imp	'WSASendMsg'						WSASendMsg						ws2_32		0	6
imp	'WSASendTo'						WSASendTo						ws2_32		0	9
imp	'WSASetBlockingHook'					WSASetBlockingHook					ws2_32		0
imp	'WSASetEvent'						WSASetEvent						ws2_32		0	1
imp	'WSASetLastError'					WSASetLastError						ws2_32		0	1
imp	'WSASetService'						WSASetServiceW						ws2_32		0	3
imp	'WSASocket'						WSASocketW						ws2_32		0	6
imp	'WSAStartup'						WSAStartup						ws2_32		0	2
imp	'WSAStringToAddress'					WSAStringToAddressW					ws2_32		0
imp	'WSAUnadvertiseProvider'				WSAUnadvertiseProvider					ws2_32		0
imp	'WSAUnhookBlockingHook'					WSAUnhookBlockingHook					ws2_32		0
imp	'WSApSetPostRoutine'					WSApSetPostRoutine					ws2_32		0
imp	'WSCDeinstallProvider'					WSCDeinstallProvider					ws2_32		0
imp	'WSCDeinstallProvider32'				WSCDeinstallProvider32					ws2_32		0
imp	'WSCDeinstallProviderEx'				WSCDeinstallProviderEx					ws2_32		0
imp	'WSCEnableNSProvider'					WSCEnableNSProvider					ws2_32		0
imp	'WSCEnableNSProvider32'					WSCEnableNSProvider32					ws2_32		0
imp	'WSCEnumNameSpaceProviders32'				WSCEnumNameSpaceProviders32				ws2_32		0
imp	'WSCEnumNameSpaceProvidersEx32'				WSCEnumNameSpaceProvidersEx32				ws2_32		0
imp	'WSCEnumProtocols'					WSCEnumProtocols					ws2_32		0
imp	'WSCEnumProtocols32'					WSCEnumProtocols32					ws2_32		0
imp	'WSCEnumProtocolsEx'					WSCEnumProtocolsEx					ws2_32		0
imp	'WSCGetApplicationCategory'				WSCGetApplicationCategory				ws2_32		0
imp	'WSCGetApplicationCategoryEx'				WSCGetApplicationCategoryEx				ws2_32		0
imp	'WSCGetProviderInfo'					WSCGetProviderInfo					ws2_32		0
imp	'WSCGetProviderInfo32'					WSCGetProviderInfo32					ws2_32		0
imp	'WSCGetProviderPath'					WSCGetProviderPath					ws2_32		0
imp	'WSCGetProviderPath32'					WSCGetProviderPath32					ws2_32		0
imp	'WSCInstallNameSpace'					WSCInstallNameSpace					ws2_32		0
imp	'WSCInstallNameSpace32'					WSCInstallNameSpace32					ws2_32		0
imp	'WSCInstallNameSpaceEx'					WSCInstallNameSpaceEx					ws2_32		0
imp	'WSCInstallNameSpaceEx2'				WSCInstallNameSpaceEx2					ws2_32		0
imp	'WSCInstallNameSpaceEx32'				WSCInstallNameSpaceEx32					ws2_32		0
imp	'WSCInstallProvider'					WSCInstallProvider					ws2_32		0
imp	'WSCInstallProvider64_32'				WSCInstallProvider64_32					ws2_32		0
imp	'WSCInstallProviderAndChains64_32'			WSCInstallProviderAndChains64_32			ws2_32		0
imp	'WSCInstallProviderEx'					WSCInstallProviderEx					ws2_32		0
imp	'WSCSetApplicationCategory'				WSCSetApplicationCategory				ws2_32		0
imp	'WSCSetApplicationCategoryEx'				WSCSetApplicationCategoryEx				ws2_32		0
imp	'WSCSetProviderInfo'					WSCSetProviderInfo					ws2_32		0
imp	'WSCSetProviderInfo32'					WSCSetProviderInfo32					ws2_32		0
imp	'WSCUnInstallNameSpace'					WSCUnInstallNameSpace					ws2_32		0
imp	'WSCUnInstallNameSpace32'				WSCUnInstallNameSpace32					ws2_32		0
imp	'WSCUnInstallNameSpaceEx2'				WSCUnInstallNameSpaceEx2				ws2_32		0
imp	'WSCUpdateProvider'					WSCUpdateProvider					ws2_32		0
imp	'WSCUpdateProvider32'					WSCUpdateProvider32					ws2_32		0
imp	'WSCUpdateProviderEx'					WSCUpdateProviderEx					ws2_32		0
imp	'WSCWriteNameSpaceOrder'				WSCWriteNameSpaceOrder					ws2_32		0
imp	'WSCWriteNameSpaceOrder32'				WSCWriteNameSpaceOrder32				ws2_32		0
imp	'WSCWriteProviderOrder'					WSCWriteProviderOrder					ws2_32		0
imp	'WSCWriteProviderOrder32'				WSCWriteProviderOrder32					ws2_32		0
imp	'WSCWriteProviderOrderEx'				WSCWriteProviderOrderEx					ws2_32		0
imp	'__sys_socket_nt'					socket							ws2_32		0	3
imp	'__sys_accept_nt'					accept							ws2_32		0	3	# we're using WSAAccept()
imp	'__sys_bind_nt'						bind							ws2_32		0	3
imp	'__sys_closesocket_nt'					closesocket						ws2_32		0	1
imp	'__sys_connect_nt'					connect							ws2_32		0
imp	'__sys_getpeername_nt'					getpeername						ws2_32		0	3
imp	'__sys_getsockname_nt'					getsockname						ws2_32		0	3
imp	'__sys_getsockopt_nt'					getsockopt						ws2_32		0	5
imp	'__sys_ioctlsocket_nt'					ioctlsocket						ws2_32		0	3
imp	'__sys_listen_nt'					listen							ws2_32		0	2
imp	'__sys_recvfrom_nt'					recvfrom						ws2_32		0	6	# we're using WSARecvFrom()
imp	'__sys_select_nt'					select							ws2_32		0	5
imp	'__sys_sendto_nt'					sendto							ws2_32		0	6	# we're using WSASendTo()
imp	'__sys_setsockopt_nt'					setsockopt						ws2_32		0	5
imp	'__sys_shutdown_nt'					shutdown						ws2_32		0	2
imp	'__sys_socket_nt'					socket							ws2_32		0	3
imp	'sys_freeaddrinfo_nt'					freeaddrinfo						ws2_32		0
imp	'sys_getaddrinfo_nt'					getaddrinfo						ws2_32		0
imp	'sys_gethostbyaddr_nt'					gethostbyaddr						ws2_32		0
imp	'sys_gethostbyname_nt'					gethostbyname						ws2_32		0
imp	'sys_gethostname_nt'					gethostname						ws2_32		0
imp	'sys_getnameinfo_nt'					getnameinfo						ws2_32		0
imp	'sys_getprotobyname_nt'					getprotobyname						ws2_32		0
imp	'sys_getprotobynumber_nt'				getprotobynumber					ws2_32		0
imp	'sys_getservbyname_nt'					getservbyname						ws2_32		0
imp	'sys_getservbyport_nt'					getservbyport						ws2_32		0
imp	'__WSARecv'						WSARecv							ws2_32		0	7
imp	'__WSARecvFrom'						WSARecvFrom						ws2_32		0	9
imp	'__WSAWaitForMultipleEvents'				WSAWaitForMultipleEvents				ws2_32		0	5
imp	'__sys_recv_nt'						recv							ws2_32		0	4	# we're using WSARecvFrom()
imp	'__sys_send_nt'						send							ws2_32		0	4	# we're using WSASendTo()
imp	'__WSAGetOverlappedResult'				WSAGetOverlappedResult					ws2_32		0	5

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
imp     'GetAdaptersAddresses'                                  GetAdaptersAddresses                                    iphlpapi        0	5
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
imp	'OpenURL'						OpenURL							url		0
imp	'TelnetProtocolHandler'					TelnetProtocolHandler					url		0
imp	'TranslateURLW'						TranslateURLW						url		0
imp	'URLAssociationDialog'					URLAssociationDialogW					url		0
imp	'AddMIMEFileTypesPS'					AddMIMEFileTypesPS					url		0
imp	'AutodialHookCallback'					AutodialHookCallback					url		0
imp	'FileProtocolHandler'					FileProtocolHandler					url		0
imp	'InetIsOffline'						InetIsOffline						url		0
imp	'MIMEAssociationDialog'					MIMEAssociationDialogW					url		0
imp	'MailToProtocolHandler'					MailToProtocolHandler					url		0

# API-MS-Win-Core-Synch-l1-2-0.dll (Windows 8+)
#
#	Name							Actual							DLL					Hint	Arity
imp	'WaitOnAddress'						WaitOnAddress						API-MS-Win-Core-Synch-l1-2-0		0	4
imp	'WakeByAddressAll'					WakeByAddressAll					API-MS-Win-Core-Synch-l1-2-0		0	1
imp	'WakeByAddressSingle'					WakeByAddressSingle					API-MS-Win-Core-Synch-l1-2-0		0	1

# NTDLL.DLL
# BEYOND THE PALE
#
#  “The functions and structures in [for these APIs] are internal to
#   the operating system and subject to change from one release of
#   Windows to the next, and possibly even between service packs for
#   each release.” ──Quoth MSDN
#
#	Name							Actual							DLL		Hint	Arity
imp	'AlpcAdjustCompletionListConcurrencyCount'		AlpcAdjustCompletionListConcurrencyCount		ntdll		0
imp	'AlpcFreeCompletionListMessage'				AlpcFreeCompletionListMessage				ntdll		0
imp	'AlpcGetCompletionListLastMessageInformation'		AlpcGetCompletionListLastMessageInformation		ntdll		0
imp	'AlpcGetCompletionListMessageAttributes'		AlpcGetCompletionListMessageAttributes			ntdll		0
imp	'AlpcGetHeaderSize'					AlpcGetHeaderSize					ntdll		0
imp	'AlpcGetMessageAttribute'				AlpcGetMessageAttribute					ntdll		0
imp	'AlpcGetMessageFromCompletionList'			AlpcGetMessageFromCompletionList			ntdll		0
imp	'AlpcGetOutstandingCompletionListMessageCount'		AlpcGetOutstandingCompletionListMessageCount		ntdll		0
imp	'AlpcInitializeMessageAttribute'			AlpcInitializeMessageAttribute				ntdll		0
imp	'AlpcMaxAllowedMessageLength'				AlpcMaxAllowedMessageLength				ntdll		0
imp	'AlpcRegisterCompletionList'				AlpcRegisterCompletionList				ntdll		0
imp	'AlpcRegisterCompletionListWorkerThread'		AlpcRegisterCompletionListWorkerThread			ntdll		0
imp	'AlpcRundownCompletionList'				AlpcRundownCompletionList				ntdll		0
imp	'AlpcUnregisterCompletionList'				AlpcUnregisterCompletionList				ntdll		0
imp	'AlpcUnregisterCompletionListWorkerThread'		AlpcUnregisterCompletionListWorkerThread		ntdll		0
imp	'ApiSetQueryApiSetPresence'				ApiSetQueryApiSetPresence				ntdll		0
imp	'CsrAllocateCaptureBuffer'				CsrAllocateCaptureBuffer				ntdll		0
imp	'CsrAllocateMessagePointer'				CsrAllocateMessagePointer				ntdll		0
imp	'CsrCaptureMessageBuffer'				CsrCaptureMessageBuffer					ntdll		0
imp	'CsrCaptureMessageMultiUnicodeStringsInPlace'		CsrCaptureMessageMultiUnicodeStringsInPlace		ntdll		0
imp	'CsrCaptureMessageString'				CsrCaptureMessageString					ntdll		0
imp	'CsrCaptureTimeout'					CsrCaptureTimeout					ntdll		0
imp	'CsrClientCallServer'					CsrClientCallServer					ntdll		0	4
imp	'CsrClientConnectToServer'				CsrClientConnectToServer				ntdll		0
imp	'CsrFreeCaptureBuffer'					CsrFreeCaptureBuffer					ntdll		0
imp	'CsrGetProcessId'					CsrGetProcessId						ntdll		0
imp	'CsrIdentifyAlertableThread'				CsrIdentifyAlertableThread				ntdll		0
imp	'CsrSetPriorityClass'					CsrSetPriorityClass					ntdll		0
imp	'CsrVerifyRegion'					CsrVerifyRegion						ntdll		0
imp	'DbgBreakPoint'						DbgBreakPoint						ntdll		0
imp	'DbgPrint'						DbgPrint						ntdll		0
imp	'DbgPrintEx'						DbgPrintEx						ntdll		0
imp	'DbgPrintReturnControlC'				DbgPrintReturnControlC					ntdll		0
imp	'DbgPrompt'						DbgPrompt						ntdll		0
imp	'DbgQueryDebugFilterState'				DbgQueryDebugFilterState				ntdll		0
imp	'DbgSetDebugFilterState'				DbgSetDebugFilterState					ntdll		0
imp	'DbgUiConnectToDbg'					DbgUiConnectToDbg					ntdll		0
imp	'DbgUiContinue'						DbgUiContinue						ntdll		0
imp	'DbgUiConvertStateChangeStructure'			DbgUiConvertStateChangeStructure			ntdll		0
imp	'DbgUiConvertStateChangeStructureEx'			DbgUiConvertStateChangeStructureEx			ntdll		0
imp	'DbgUiDebugActiveProcess'				DbgUiDebugActiveProcess					ntdll		0
imp	'DbgUiGetThreadDebugObject'				DbgUiGetThreadDebugObject				ntdll		0
imp	'DbgUiIssueRemoteBreakin'				DbgUiIssueRemoteBreakin					ntdll		0
imp	'DbgUiRemoteBreakin'					DbgUiRemoteBreakin					ntdll		0
imp	'DbgUiSetThreadDebugObject'				DbgUiSetThreadDebugObject				ntdll		0
imp	'DbgUiStopDebugging'					DbgUiStopDebugging					ntdll		0
imp	'DbgUiWaitStateChange'					DbgUiWaitStateChange					ntdll		0
imp	'DbgUserBreakPoint'					DbgUserBreakPoint					ntdll		0
imp	'EtwCheckCoverage'					EtwCheckCoverage					ntdll		0
imp	'EtwCreateTraceInstanceId'				EtwCreateTraceInstanceId				ntdll		0
imp	'EtwDeliverDataBlock'					EtwDeliverDataBlock					ntdll		0
imp	'EtwEnumerateProcessRegGuids'				EtwEnumerateProcessRegGuids				ntdll		0
imp	'EtwEventActivityIdControl'				EtwEventActivityIdControl				ntdll		0
imp	'EtwEventEnabled'					EtwEventEnabled						ntdll		0
imp	'EtwEventProviderEnabled'				EtwEventProviderEnabled					ntdll		0
imp	'EtwEventRegister'					EtwEventRegister					ntdll		0
imp	'EtwEventSetInformation'				EtwEventSetInformation					ntdll		0
imp	'EtwEventUnregister'					EtwEventUnregister					ntdll		0
imp	'EtwEventWrite'						EtwEventWrite						ntdll		0
imp	'EtwEventWriteEndScenario'				EtwEventWriteEndScenario				ntdll		0
imp	'EtwEventWriteEx'					EtwEventWriteEx						ntdll		0
imp	'EtwEventWriteFull'					EtwEventWriteFull					ntdll		0
imp	'EtwEventWriteNoRegistration'				EtwEventWriteNoRegistration				ntdll		0
imp	'EtwEventWriteStartScenario'				EtwEventWriteStartScenario				ntdll		0
imp	'EtwEventWriteString'					EtwEventWriteString					ntdll		0
imp	'EtwEventWriteTransfer'					EtwEventWriteTransfer					ntdll		0
imp	'EtwGetTraceEnableFlags'				EtwGetTraceEnableFlags					ntdll		0
imp	'EtwGetTraceEnableLevel'				EtwGetTraceEnableLevel					ntdll		0
imp	'EtwGetTraceLoggerHandle'				EtwGetTraceLoggerHandle					ntdll		0
imp	'EtwLogTraceEvent'					EtwLogTraceEvent					ntdll		0
imp	'EtwNotificationRegister'				EtwNotificationRegister					ntdll		0
imp	'EtwNotificationUnregister'				EtwNotificationUnregister				ntdll		0
imp	'EtwProcessPrivateLoggerRequest'			EtwProcessPrivateLoggerRequest				ntdll		0
imp	'EtwRegisterSecurityProvider'				EtwRegisterSecurityProvider				ntdll		0
imp	'EtwRegisterTraceGuids'					EtwRegisterTraceGuidsW					ntdll		0
imp	'EtwReplyNotification'					EtwReplyNotification					ntdll		0
imp	'EtwSendNotification'					EtwSendNotification					ntdll		0
imp	'EtwSetMark'						EtwSetMark						ntdll		0
imp	'EtwTraceEventInstance'					EtwTraceEventInstance					ntdll		0
imp	'EtwTraceMessage'					EtwTraceMessage						ntdll		0
imp	'EtwTraceMessageVa'					EtwTraceMessageVa					ntdll		0
imp	'EtwUnregisterTraceGuids'				EtwUnregisterTraceGuids					ntdll		0
imp	'EtwWriteUMSecurityEvent'				EtwWriteUMSecurityEvent					ntdll		0
imp	'EtwpCreateEtwThread'					EtwpCreateEtwThread					ntdll		0
imp	'EtwpGetCpuSpeed'					EtwpGetCpuSpeed						ntdll		0
imp	'EvtIntReportAuthzEventAndSourceAsync'			EvtIntReportAuthzEventAndSourceAsync			ntdll		0
imp	'EvtIntReportEventAndSourceAsync'			EvtIntReportEventAndSourceAsync				ntdll		0
imp	'ExpInterlockedPopEntrySListEnd'			ExpInterlockedPopEntrySListEnd				ntdll		0
imp	'ExpInterlockedPopEntrySListFault'			ExpInterlockedPopEntrySListFault			ntdll		0
imp	'ExpInterlockedPopEntrySListResume'			ExpInterlockedPopEntrySListResume			ntdll		0
imp	'KiRaiseUserExceptionDispatcher'			KiRaiseUserExceptionDispatcher				ntdll		0
imp	'KiUserApcDispatcher'					KiUserApcDispatcher					ntdll		0
imp	'KiUserCallbackDispatcher'				KiUserCallbackDispatcher				ntdll		0
imp	'KiUserExceptionDispatcher'				KiUserExceptionDispatcher				ntdll		0
imp	'KiUserInvertedFunctionTable'				KiUserInvertedFunctionTable				ntdll		0
imp	'LdrAccessResource'					LdrAccessResource					ntdll		0
imp	'LdrAddDllDirectory'					LdrAddDllDirectory					ntdll		0
imp	'LdrAddLoadAsDataTable'					LdrAddLoadAsDataTable					ntdll		0
imp	'LdrAddRefDll'						LdrAddRefDll						ntdll		0
imp	'LdrAppxHandleIntegrityFailure'				LdrAppxHandleIntegrityFailure				ntdll		0
imp	'LdrCallEnclave'					LdrCallEnclave						ntdll		0
imp	'LdrControlFlowGuardEnforced'				LdrControlFlowGuardEnforced				ntdll		0
imp	'LdrCreateEnclave'					LdrCreateEnclave					ntdll		0
imp	'LdrDeleteEnclave'					LdrDeleteEnclave					ntdll		0
imp	'LdrDisableThreadCalloutsForDll'			LdrDisableThreadCalloutsForDll				ntdll		0
imp	'LdrEnumResources'					LdrEnumResources					ntdll		0
imp	'LdrEnumerateLoadedModules'				LdrEnumerateLoadedModules				ntdll		0
imp	'LdrFastFailInLoaderCallout'				LdrFastFailInLoaderCallout				ntdll		0
imp	'LdrFindEntryForAddress'				LdrFindEntryForAddress					ntdll		0
imp	'LdrFindResourceDirectory_U'				LdrFindResourceDirectory_U				ntdll		0
imp	'LdrFindResourceEx_U'					LdrFindResourceEx_U					ntdll		0
imp	'LdrFindResource_U'					LdrFindResource_U					ntdll		0
imp	'LdrFlushAlternateResourceModules'			LdrFlushAlternateResourceModules			ntdll		0
imp	'LdrGetDllDirectory'					LdrGetDllDirectory					ntdll		0
imp	'LdrGetDllFullName'					LdrGetDllFullName					ntdll		0
imp	'LdrGetDllHandle'					LdrGetDllHandle						ntdll		0	4
imp	'LdrGetDllHandleByMapping'				LdrGetDllHandleByMapping				ntdll		0
imp	'LdrGetDllHandleByName'					LdrGetDllHandleByName					ntdll		0
imp	'LdrGetDllHandleEx'					LdrGetDllHandleEx					ntdll		0
imp	'LdrGetDllPath'						LdrGetDllPath						ntdll		0
imp	'LdrGetFailureData'					LdrGetFailureData					ntdll		0
imp	'LdrGetFileNameFromLoadAsDataTable'			LdrGetFileNameFromLoadAsDataTable			ntdll		0
imp	'LdrGetKnownDllSectionHandle'				LdrGetKnownDllSectionHandle				ntdll		0
imp	'LdrGetProcedureAddress'				LdrGetProcedureAddress					ntdll		0	4
imp	'LdrGetProcedureAddressEx'				LdrGetProcedureAddressEx				ntdll		0
imp	'LdrGetProcedureAddressForCaller'			LdrGetProcedureAddressForCaller				ntdll		0
imp	'LdrInitShimEngineDynamic'				LdrInitShimEngineDynamic				ntdll		0
imp	'LdrInitializeEnclave'					LdrInitializeEnclave					ntdll		0
imp	'LdrInitializeThunk'					LdrInitializeThunk					ntdll		0
imp	'LdrLoadAlternateResourceModule'			LdrLoadAlternateResourceModule				ntdll		0
imp	'LdrLoadAlternateResourceModuleEx'			LdrLoadAlternateResourceModuleEx			ntdll		0
imp	'LdrLoadDll'						LdrLoadDll						ntdll		0	4
imp	'LdrLoadEnclaveModule'					LdrLoadEnclaveModule					ntdll		0
imp	'LdrLockLoaderLock'					LdrLockLoaderLock					ntdll		0
imp	'LdrOpenImageFileOptionsKey'				LdrOpenImageFileOptionsKey				ntdll		0
imp	'LdrProcessInitializationComplete'			LdrProcessInitializationComplete			ntdll		0
imp	'LdrProcessRelocationBlock'				LdrProcessRelocationBlock				ntdll		0
imp	'LdrProcessRelocationBlockEx'				LdrProcessRelocationBlockEx				ntdll		0
imp	'LdrQueryImageFileExecutionOptions'			LdrQueryImageFileExecutionOptions			ntdll		0
imp	'LdrQueryImageFileExecutionOptionsEx'			LdrQueryImageFileExecutionOptionsEx			ntdll		0
imp	'LdrQueryImageFileKeyOption'				LdrQueryImageFileKeyOption				ntdll		0
imp	'LdrQueryModuleServiceTags'				LdrQueryModuleServiceTags				ntdll		0
imp	'LdrQueryOptionalDelayLoadedAPI'			LdrQueryOptionalDelayLoadedAPI				ntdll		0
imp	'LdrQueryProcessModuleInformation'			LdrQueryProcessModuleInformation			ntdll		0
imp	'LdrRegisterDllNotification'				LdrRegisterDllNotification				ntdll		0
imp	'LdrRemoveDllDirectory'					LdrRemoveDllDirectory					ntdll		0
imp	'LdrRemoveLoadAsDataTable'				LdrRemoveLoadAsDataTable				ntdll		0
imp	'LdrResFindResource'					LdrResFindResource					ntdll		0
imp	'LdrResFindResourceDirectory'				LdrResFindResourceDirectory				ntdll		0
imp	'LdrResGetRCConfig'					LdrResGetRCConfig					ntdll		0
imp	'LdrResRelease'						LdrResRelease						ntdll		0
imp	'LdrResSearchResource'					LdrResSearchResource					ntdll		0
imp	'LdrResolveDelayLoadedAPI'				LdrResolveDelayLoadedAPI				ntdll		0
imp	'LdrResolveDelayLoadsFromDll'				LdrResolveDelayLoadsFromDll				ntdll		0
imp	'LdrRscIsTypeExist'					LdrRscIsTypeExist					ntdll		0
imp	'LdrSetAppCompatDllRedirectionCallback'			LdrSetAppCompatDllRedirectionCallback			ntdll		0
imp	'LdrSetDefaultDllDirectories'				LdrSetDefaultDllDirectories				ntdll		0
imp	'LdrSetDllDirectory'					LdrSetDllDirectory					ntdll		0
imp	'LdrSetDllManifestProber'				LdrSetDllManifestProber					ntdll		0
imp	'LdrSetImplicitPathOptions'				LdrSetImplicitPathOptions				ntdll		0
imp	'LdrSetMUICacheType'					LdrSetMUICacheType					ntdll		0
imp	'LdrShutdownProcess'					LdrShutdownProcess					ntdll		0
imp	'LdrShutdownThread'					LdrShutdownThread					ntdll		0
imp	'LdrStandardizeSystemPath'				LdrStandardizeSystemPath				ntdll		0
imp	'LdrSystemDllInitBlock'					LdrSystemDllInitBlock					ntdll		0
imp	'LdrUnloadAlternateResourceModule'			LdrUnloadAlternateResourceModule			ntdll		0
imp	'LdrUnloadAlternateResourceModuleEx'			LdrUnloadAlternateResourceModuleEx			ntdll		0
imp	'LdrUnloadDll'						LdrUnloadDll						ntdll		0	1
imp	'LdrUnlockLoaderLock'					LdrUnlockLoaderLock					ntdll		0
imp	'LdrUnregisterDllNotification'				LdrUnregisterDllNotification				ntdll		0
imp	'LdrUpdatePackageSearchPath'				LdrUpdatePackageSearchPath				ntdll		0
imp	'LdrVerifyImageMatchesChecksum'				LdrVerifyImageMatchesChecksum				ntdll		0
imp	'LdrVerifyImageMatchesChecksumEx'			LdrVerifyImageMatchesChecksumEx				ntdll		0
imp	'LdrpResGetMappingSize'					LdrpResGetMappingSize					ntdll		0
imp	'LdrpResGetResourceDirectory'				LdrpResGetResourceDirectory				ntdll		0
imp	'MD4Final'						MD4Final						ntdll		0
imp	'MD4Init'						MD4Init							ntdll		0
imp	'MD4Update'						MD4Update						ntdll		0
imp	'NlsAnsiCodePage'					NlsAnsiCodePage						ntdll		0
imp	'NlsMbCodePageTag'					NlsMbCodePageTag					ntdll		0
imp	'NlsMbOemCodePageTag'					NlsMbOemCodePageTag					ntdll		0
imp	'NtAcceptConnectPort'					NtAcceptConnectPort					ntdll		0
imp	'NtAccessCheck'						NtAccessCheck						ntdll		0
imp	'NtAccessCheckAndAuditAlarm'				NtAccessCheckAndAuditAlarm				ntdll		0
imp	'NtAccessCheckByType'					NtAccessCheckByType					ntdll		0
imp	'NtAccessCheckByTypeAndAuditAlarm'			NtAccessCheckByTypeAndAuditAlarm			ntdll		0
imp	'NtAccessCheckByTypeResultList'				NtAccessCheckByTypeResultList				ntdll		0
imp	'NtAccessCheckByTypeResultListAndAuditAlarm'		NtAccessCheckByTypeResultListAndAuditAlarm		ntdll		0
imp	'NtAccessCheckByTypeResultListAndAuditAlarmByHandle'	NtAccessCheckByTypeResultListAndAuditAlarmByHandle	ntdll		0
imp	'NtAcquireProcessActivityReference'			NtAcquireProcessActivityReference			ntdll		0
imp	'NtAddAtom'						NtAddAtom						ntdll		0
imp	'NtAddAtomEx'						NtAddAtomEx						ntdll		0
imp	'NtAddBootEntry'					NtAddBootEntry						ntdll		0
imp	'NtAddDriverEntry'					NtAddDriverEntry					ntdll		0
imp	'NtAdjustGroupsToken'					NtAdjustGroupsToken					ntdll		0
imp	'NtAdjustPrivilegesToken'				NtAdjustPrivilegesToken					ntdll		0
imp	'NtAdjustTokenClaimsAndDeviceGroups'			NtAdjustTokenClaimsAndDeviceGroups			ntdll		0
imp	'NtAlertResumeThread'					NtAlertResumeThread					ntdll		0
imp	'NtAlertThread'						NtAlertThread						ntdll		0
imp	'NtAlertThreadByThreadId'				NtAlertThreadByThreadId					ntdll		0
imp	'NtAllocateLocallyUniqueId'				NtAllocateLocallyUniqueId				ntdll		0
imp	'NtAllocateReserveObject'				NtAllocateReserveObject					ntdll		0
imp	'NtAllocateUserPhysicalPages'				NtAllocateUserPhysicalPages				ntdll		0
imp	'NtAllocateUuids'					NtAllocateUuids						ntdll		0
imp	'NtAllocateVirtualMemory'				NtAllocateVirtualMemory					ntdll		0	6
imp	'NtAllocateVirtualMemoryEx'				NtAllocateVirtualMemoryEx				ntdll		0
imp	'NtAlpcAcceptConnectPort'				NtAlpcAcceptConnectPort					ntdll		0
imp	'NtAlpcCancelMessage'					NtAlpcCancelMessage					ntdll		0
imp	'NtAlpcConnectPort'					NtAlpcConnectPort					ntdll		0
imp	'NtAlpcConnectPortEx'					NtAlpcConnectPortEx					ntdll		0
imp	'NtAlpcCreatePort'					NtAlpcCreatePort					ntdll		0
imp	'NtAlpcCreatePortSection'				NtAlpcCreatePortSection					ntdll		0
imp	'NtAlpcCreateResourceReserve'				NtAlpcCreateResourceReserve				ntdll		0
imp	'NtAlpcCreateSectionView'				NtAlpcCreateSectionView					ntdll		0
imp	'NtAlpcCreateSecurityContext'				NtAlpcCreateSecurityContext				ntdll		0
imp	'NtAlpcDeletePortSection'				NtAlpcDeletePortSection					ntdll		0
imp	'NtAlpcDeleteResourceReserve'				NtAlpcDeleteResourceReserve				ntdll		0
imp	'NtAlpcDeleteSectionView'				NtAlpcDeleteSectionView					ntdll		0
imp	'NtAlpcDeleteSecurityContext'				NtAlpcDeleteSecurityContext				ntdll		0
imp	'NtAlpcDisconnectPort'					NtAlpcDisconnectPort					ntdll		0
imp	'NtAlpcImpersonateClientContainerOfPort'		NtAlpcImpersonateClientContainerOfPort			ntdll		0
imp	'NtAlpcImpersonateClientOfPort'				NtAlpcImpersonateClientOfPort				ntdll		0
imp	'NtAlpcOpenSenderProcess'				NtAlpcOpenSenderProcess					ntdll		0
imp	'NtAlpcOpenSenderThread'				NtAlpcOpenSenderThread					ntdll		0
imp	'NtAlpcQueryInformation'				NtAlpcQueryInformation					ntdll		0
imp	'NtAlpcQueryInformationMessage'				NtAlpcQueryInformationMessage				ntdll		0
imp	'NtAlpcRevokeSecurityContext'				NtAlpcRevokeSecurityContext				ntdll		0
imp	'NtAlpcSendWaitReceivePort'				NtAlpcSendWaitReceivePort				ntdll		0
imp	'NtAlpcSetInformation'					NtAlpcSetInformation					ntdll		0
imp	'NtApphelpCacheControl'					NtApphelpCacheControl					ntdll		0
imp	'NtAreMappedFilesTheSame'				NtAreMappedFilesTheSame					ntdll		0
imp	'NtAssignProcessToJobObject'				NtAssignProcessToJobObject				ntdll		0
imp	'NtAssociateWaitCompletionPacket'			NtAssociateWaitCompletionPacket				ntdll		0
imp	'NtCallEnclave'						NtCallEnclave						ntdll		0
imp	'NtCallbackReturn'					NtCallbackReturn					ntdll		0	3
imp	'NtCancelIoFile'					NtCancelIoFile						ntdll		0	2
imp	'NtCancelIoFileEx'					NtCancelIoFileEx					ntdll		0	3
imp	'NtCancelSynchronousIoFile'				NtCancelSynchronousIoFile				ntdll		0
imp	'NtCancelTimer'						NtCancelTimer						ntdll		0
imp	'NtCancelTimer2'					NtCancelTimer2						ntdll		0
imp	'NtCancelWaitCompletionPacket'				NtCancelWaitCompletionPacket				ntdll		0
imp	'NtClearEvent'						NtClearEvent						ntdll		0	1
imp	'NtClose'						NtClose							ntdll		0	1
imp	'NtCloseObjectAuditAlarm'				NtCloseObjectAuditAlarm					ntdll		0
imp	'NtCommitComplete'					NtCommitComplete					ntdll		0
imp	'NtCommitEnlistment'					NtCommitEnlistment					ntdll		0
imp	'NtCommitRegistryTransaction'				NtCommitRegistryTransaction				ntdll		0
imp	'NtCommitTransaction'					NtCommitTransaction					ntdll		0
imp	'NtCompactKeys'						NtCompactKeys						ntdll		0
imp	'NtCompareObjects'					NtCompareObjects					ntdll		0
imp	'NtCompareSigningLevels'				NtCompareSigningLevels					ntdll		0
imp	'NtCompareTokens'					NtCompareTokens						ntdll		0
imp	'NtCompleteConnectPort'					NtCompleteConnectPort					ntdll		0
imp	'NtCompressKey'						NtCompressKey						ntdll		0
imp	'NtConnectPort'						NtConnectPort						ntdll		0
imp	'NtContinue'						NtContinue						ntdll		0	2
imp	'NtConvertBetweenAuxiliaryCounterAndPerformanceCounter'	NtConvertBetweenAuxiliaryCounterAndPerformanceCounter	ntdll		0
imp	'NtCreateDebugObject'					NtCreateDebugObject					ntdll		0
imp	'NtCreateDirectoryObject'				NtCreateDirectoryObject					ntdll		0	3
imp	'NtCreateDirectoryObjectEx'				NtCreateDirectoryObjectEx				ntdll		0
imp	'NtCreateEnclave'					NtCreateEnclave						ntdll		0
imp	'NtCreateEnlistment'					NtCreateEnlistment					ntdll		0
imp	'NtCreateEvent'						NtCreateEvent						ntdll		0	5
imp	'NtCreateEventPair'					NtCreateEventPair					ntdll		0
imp	'NtCreateFile'						NtCreateFile						ntdll		0	11
imp	'NtCreateIRTimer'					NtCreateIRTimer						ntdll		0
imp	'NtCreateIoCompletion'					NtCreateIoCompletion					ntdll		0	4
imp	'NtCreateJobObject'					NtCreateJobObject					ntdll		0
imp	'NtCreateJobSet'					NtCreateJobSet						ntdll		0
imp	'NtCreateKey'						NtCreateKey						ntdll		0	7
imp	'NtCreateKeyTransacted'					NtCreateKeyTransacted					ntdll		0
imp	'NtCreateKeyedEvent'					NtCreateKeyedEvent					ntdll		0	4
imp	'NtCreateLowBoxToken'					NtCreateLowBoxToken					ntdll		0
imp	'NtCreateMailslotFile'					NtCreateMailslotFile					ntdll		0
imp	'NtCreateMutant'					NtCreateMutant						ntdll		0
imp	'NtCreateNamedPipeFile'					NtCreateNamedPipeFile					ntdll		0	14
imp	'NtCreatePagingFile'					NtCreatePagingFile					ntdll		0
imp	'NtCreatePartition'					NtCreatePartition					ntdll		0
imp	'NtCreatePort'						NtCreatePort						ntdll		0
imp	'NtCreatePrivateNamespace'				NtCreatePrivateNamespace				ntdll		0
imp	'NtCreateProcess'					NtCreateProcess						ntdll		0	8
imp	'NtCreateProcessEx'					NtCreateProcessEx					ntdll		0
imp	'NtCreateProfile'					NtCreateProfile						ntdll		0	9
imp	'NtCreateProfileEx'					NtCreateProfileEx					ntdll		0
imp	'NtCreateRegistryTransaction'				NtCreateRegistryTransaction				ntdll		0
imp	'NtCreateResourceManager'				NtCreateResourceManager					ntdll		0
imp	'NtCreateSection'					NtCreateSection						ntdll		0	7
imp	'NtCreateSemaphore'					NtCreateSemaphore					ntdll		0
imp	'NtCreateSymbolicLinkObject'				NtCreateSymbolicLinkObject				ntdll		0
imp	'NtCreateThread'					NtCreateThread						ntdll		0	8
imp	'NtCreateThreadEx'					NtCreateThreadEx					ntdll		0
imp	'NtCreateTimer'						NtCreateTimer						ntdll		0	4
imp	'NtCreateTimer2'					NtCreateTimer2						ntdll		0
imp	'NtCreateToken'						NtCreateToken						ntdll		0
imp	'NtCreateTokenEx'					NtCreateTokenEx						ntdll		0
imp	'NtCreateTransaction'					NtCreateTransaction					ntdll		0
imp	'NtCreateTransactionManager'				NtCreateTransactionManager				ntdll		0
imp	'NtCreateUserProcess'					NtCreateUserProcess					ntdll		0
imp	'NtCreateWaitCompletionPacket'				NtCreateWaitCompletionPacket				ntdll		0
imp	'NtCreateWaitablePort'					NtCreateWaitablePort					ntdll		0
imp	'NtCreateWnfStateName'					NtCreateWnfStateName					ntdll		0
imp	'NtCreateWorkerFactory'					NtCreateWorkerFactory					ntdll		0
imp	'NtDebugActiveProcess'					NtDebugActiveProcess					ntdll		0
imp	'NtDebugContinue'					NtDebugContinue						ntdll		0
imp	'NtDelayExecution'					NtDelayExecution					ntdll		0	2
imp	'NtDeleteAtom'						NtDeleteAtom						ntdll		0
imp	'NtDeleteBootEntry'					NtDeleteBootEntry					ntdll		0
imp	'NtDeleteDriverEntry'					NtDeleteDriverEntry					ntdll		0
imp	'NtDeleteFile'						NtDeleteFile						ntdll		0	1
imp	'NtDeleteKey'						NtDeleteKey						ntdll		0	1
imp	'NtDeleteObjectAuditAlarm'				NtDeleteObjectAuditAlarm				ntdll		0
imp	'NtDeletePrivateNamespace'				NtDeletePrivateNamespace				ntdll		0
imp	'NtDeleteValueKey'					NtDeleteValueKey					ntdll		0
imp	'NtDeleteWnfStateData'					NtDeleteWnfStateData					ntdll		0
imp	'NtDeleteWnfStateName'					NtDeleteWnfStateName					ntdll		0
imp	'NtDeviceIoControlFile'					NtDeviceIoControlFile					ntdll		0	10
imp	'NtDisableLastKnownGood'				NtDisableLastKnownGood					ntdll		0
imp	'NtDisplayString'					NtDisplayString						ntdll		0
imp	'NtDrawText'						NtDrawText						ntdll		0
imp	'NtDuplicateObject'					NtDuplicateObject					ntdll		0	7
imp	'NtDuplicateToken'					NtDuplicateToken					ntdll		0
imp	'NtEnableLastKnownGood'					NtEnableLastKnownGood					ntdll		0
imp	'NtEnumerateBootEntries'				NtEnumerateBootEntries					ntdll		0
imp	'NtEnumerateDriverEntries'				NtEnumerateDriverEntries				ntdll		0
imp	'NtEnumerateKey'					NtEnumerateKey						ntdll		0	6
imp	'NtEnumerateSystemEnvironmentValuesEx'			NtEnumerateSystemEnvironmentValuesEx			ntdll		0
imp	'NtEnumerateTransactionObject'				NtEnumerateTransactionObject				ntdll		0
imp	'NtEnumerateValueKey'					NtEnumerateValueKey					ntdll		0	6
imp	'NtExtendSection'					NtExtendSection						ntdll		0
imp	'NtFilterBootOption'					NtFilterBootOption					ntdll		0
imp	'NtFilterToken'						NtFilterToken						ntdll		0
imp	'NtFilterTokenEx'					NtFilterTokenEx						ntdll		0
imp	'NtFindAtom'						NtFindAtom						ntdll		0
imp	'NtFlushBuffersFile'					NtFlushBuffersFile					ntdll		0	2
imp	'NtFlushBuffersFileEx'					NtFlushBuffersFileEx					ntdll		0
imp	'NtFlushInstallUILanguage'				NtFlushInstallUILanguage				ntdll		0
imp	'NtFlushInstructionCache'				NtFlushInstructionCache					ntdll		0	3
imp	'NtFlushKey'						NtFlushKey						ntdll		0	1
imp	'NtFlushProcessWriteBuffers'				NtFlushProcessWriteBuffers				ntdll		0
imp	'NtFlushVirtualMemory'					NtFlushVirtualMemory					ntdll		0	4
imp	'NtFlushWriteBuffer'					NtFlushWriteBuffer					ntdll		0
imp	'NtFreeUserPhysicalPages'				NtFreeUserPhysicalPages					ntdll		0
imp	'NtFreeVirtualMemory'					NtFreeVirtualMemory					ntdll		0	4
imp	'NtFreezeRegistry'					NtFreezeRegistry					ntdll		0
imp	'NtFreezeTransactions'					NtFreezeTransactions					ntdll		0
imp	'NtFsControlFile'					NtFsControlFile						ntdll		0	10
imp	'NtGetCachedSigningLevel'				NtGetCachedSigningLevel					ntdll		0
imp	'NtGetCompleteWnfStateSubscription'			NtGetCompleteWnfStateSubscription			ntdll		0
imp	'NtGetContextThread'					NtGetContextThread					ntdll		0	2
imp	'NtGetCurrentProcessorNumber'				NtGetCurrentProcessorNumber				ntdll		0
imp	'NtGetCurrentProcessorNumberEx'				NtGetCurrentProcessorNumberEx				ntdll		0
imp	'NtGetDevicePowerState'					NtGetDevicePowerState					ntdll		0
imp	'NtGetMUIRegistryInfo'					NtGetMUIRegistryInfo					ntdll		0
imp	'NtGetNextProcess'					NtGetNextProcess					ntdll		0
imp	'NtGetNextThread'					NtGetNextThread						ntdll		0
imp	'NtGetNlsSectionPtr'					NtGetNlsSectionPtr					ntdll		0
imp	'NtGetNotificationResourceManager'			NtGetNotificationResourceManager			ntdll		0
imp	'NtGetTickCount'					NtGetTickCount						ntdll		0
imp	'NtGetWriteWatch'					NtGetWriteWatch						ntdll		0
imp	'NtImpersonateAnonymousToken'				NtImpersonateAnonymousToken				ntdll		0
imp	'NtImpersonateClientOfPort'				NtImpersonateClientOfPort				ntdll		0
imp	'NtImpersonateThread'					NtImpersonateThread					ntdll		0
imp	'NtInitializeEnclave'					NtInitializeEnclave					ntdll		0
imp	'NtInitializeNlsFiles'					NtInitializeNlsFiles					ntdll		0
imp	'NtInitializeRegistry'					NtInitializeRegistry					ntdll		0
imp	'NtInitiatePowerAction'					NtInitiatePowerAction					ntdll		0
imp	'NtIsProcessInJob'					NtIsProcessInJob					ntdll		0
imp	'NtIsSystemResumeAutomatic'				NtIsSystemResumeAutomatic				ntdll		0
imp	'NtIsUILanguageComitted'				NtIsUILanguageComitted					ntdll		0
imp	'NtListenPort'						NtListenPort						ntdll		0
imp	'NtLoadDriver'						NtLoadDriver						ntdll		0
imp	'NtLoadEnclaveData'					NtLoadEnclaveData					ntdll		0
imp	'NtLoadHotPatch'					NtLoadHotPatch						ntdll		0
imp	'NtLoadKey'						NtLoadKey						ntdll		0
imp	'NtLoadKey2'						NtLoadKey2						ntdll		0
imp	'NtLoadKeyEx'						NtLoadKeyEx						ntdll		0
imp	'NtLockFile'						NtLockFile						ntdll		0
imp	'NtLockProductActivationKeys'				NtLockProductActivationKeys				ntdll		0
imp	'NtLockRegistryKey'					NtLockRegistryKey					ntdll		0
imp	'NtLockVirtualMemory'					NtLockVirtualMemory					ntdll		0
imp	'NtMakePermanentObject'					NtMakePermanentObject					ntdll		0
imp	'NtMakeTemporaryObject'					NtMakeTemporaryObject					ntdll		0
imp	'NtManagePartition'					NtManagePartition					ntdll		0
imp	'NtMapCMFModule'					NtMapCMFModule						ntdll		0
imp	'NtMapUserPhysicalPages'				NtMapUserPhysicalPages					ntdll		0
imp	'NtMapUserPhysicalPagesScatter'				NtMapUserPhysicalPagesScatter				ntdll		0
imp	'NtMapViewOfSection'					NtMapViewOfSection					ntdll		0	10
imp	'NtMapViewOfSectionEx'					NtMapViewOfSectionEx					ntdll		0
imp	'NtModifyBootEntry'					NtModifyBootEntry					ntdll		0
imp	'NtModifyDriverEntry'					NtModifyDriverEntry					ntdll		0
imp	'NtNotifyChangeDirectoryFile'				NtNotifyChangeDirectoryFile				ntdll		0
imp	'NtNotifyChangeDirectoryFileEx'				NtNotifyChangeDirectoryFileEx				ntdll		0
imp	'NtNotifyChangeKey'					NtNotifyChangeKey					ntdll		0
imp	'NtNotifyChangeMultipleKeys'				NtNotifyChangeMultipleKeys				ntdll		0
imp	'NtNotifyChangeSession'					NtNotifyChangeSession					ntdll		0
imp	'NtOpenDirectoryObject'					NtOpenDirectoryObject					ntdll		0	3
imp	'NtOpenEnlistment'					NtOpenEnlistment					ntdll		0
imp	'NtOpenEvent'						NtOpenEvent						ntdll		0
imp	'NtOpenEventPair'					NtOpenEventPair						ntdll		0
imp	'NtOpenFile'						NtOpenFile						ntdll		0	6
imp	'NtOpenIoCompletion'					NtOpenIoCompletion					ntdll		0
imp	'NtOpenJobObject'					NtOpenJobObject						ntdll		0
imp	'NtOpenKey'						NtOpenKey						ntdll		0	3
imp	'NtOpenKeyEx'						NtOpenKeyEx						ntdll		0
imp	'NtOpenKeyTransacted'					NtOpenKeyTransacted					ntdll		0
imp	'NtOpenKeyTransactedEx'					NtOpenKeyTransactedEx					ntdll		0
imp	'NtOpenKeyedEvent'					NtOpenKeyedEvent					ntdll		0
imp	'NtOpenMutant'						NtOpenMutant						ntdll		0
imp	'NtOpenObjectAuditAlarm'				NtOpenObjectAuditAlarm					ntdll		0
imp	'NtOpenPartition'					NtOpenPartition						ntdll		0
imp	'NtOpenPrivateNamespace'				NtOpenPrivateNamespace					ntdll		0
imp	'NtOpenProcess'						NtOpenProcess						ntdll		0	4
imp	'NtOpenProcessToken'					NtOpenProcessToken					ntdll		0	3
imp	'NtOpenProcessTokenEx'					NtOpenProcessTokenEx					ntdll		0
imp	'NtOpenRegistryTransaction'				NtOpenRegistryTransaction				ntdll		0
imp	'NtOpenResourceManager'					NtOpenResourceManager					ntdll		0
imp	'NtOpenSection'						NtOpenSection						ntdll		0	3
imp	'NtOpenSemaphore'					NtOpenSemaphore						ntdll		0
imp	'NtOpenSession'						NtOpenSession						ntdll		0
imp	'NtOpenSymbolicLinkObject'				NtOpenSymbolicLinkObject				ntdll		0	3
imp	'NtOpenThread'						NtOpenThread						ntdll		0	4
imp	'NtOpenThreadToken'					NtOpenThreadToken					ntdll		0	4
imp	'NtOpenThreadTokenEx'					NtOpenThreadTokenEx					ntdll		0
imp	'NtOpenTimer'						NtOpenTimer						ntdll		0
imp	'NtOpenTransaction'					NtOpenTransaction					ntdll		0
imp	'NtOpenTransactionManager'				NtOpenTransactionManager				ntdll		0
imp	'NtPlugPlayControl'					NtPlugPlayControl					ntdll		0
imp	'NtPowerInformation'					NtPowerInformation					ntdll		0
imp	'NtPrePrepareComplete'					NtPrePrepareComplete					ntdll		0
imp	'NtPrePrepareEnlistment'				NtPrePrepareEnlistment					ntdll		0
imp	'NtPrepareComplete'					NtPrepareComplete					ntdll		0
imp	'NtPrepareEnlistment'					NtPrepareEnlistment					ntdll		0
imp	'NtPrivilegeCheck'					NtPrivilegeCheck					ntdll		0
imp	'NtPrivilegeObjectAuditAlarm'				NtPrivilegeObjectAuditAlarm				ntdll		0
imp	'NtPrivilegedServiceAuditAlarm'				NtPrivilegedServiceAuditAlarm				ntdll		0
imp	'NtPropagationComplete'					NtPropagationComplete					ntdll		0
imp	'NtPropagationFailed'					NtPropagationFailed					ntdll		0
imp	'NtProtectVirtualMemory'				NtProtectVirtualMemory					ntdll		0	5
imp	'NtPulseEvent'						NtPulseEvent						ntdll		0
imp	'NtQueryAttributesFile'					NtQueryAttributesFile					ntdll		0	2
imp	'NtQueryAuxiliaryCounterFrequency'			NtQueryAuxiliaryCounterFrequency			ntdll		0
imp	'NtQueryBootEntryOrder'					NtQueryBootEntryOrder					ntdll		0
imp	'NtQueryBootOptions'					NtQueryBootOptions					ntdll		0
imp	'NtQueryDebugFilterState'				NtQueryDebugFilterState					ntdll		0
imp	'NtQueryDefaultLocale'					NtQueryDefaultLocale					ntdll		0
imp	'NtQueryDefaultUILanguage'				NtQueryDefaultUILanguage				ntdll		0
imp	'NtQueryDirectoryFile'					NtQueryDirectoryFile					ntdll		0	11
imp	'NtQueryDirectoryFileEx'				NtQueryDirectoryFileEx					ntdll		0
imp	'NtQueryDirectoryObject'				NtQueryDirectoryObject					ntdll		0
imp	'NtQueryDriverEntryOrder'				NtQueryDriverEntryOrder					ntdll		0
imp	'NtQueryEaFile'						NtQueryEaFile						ntdll		0
imp	'NtQueryEvent'						NtQueryEvent						ntdll		0
imp	'NtQueryFullAttributesFile'				NtQueryFullAttributesFile				ntdll		0	2
imp	'NtQueryInformationAtom'				NtQueryInformationAtom					ntdll		0
imp	'NtQueryInformationByName'				NtQueryInformationByName				ntdll		0
imp	'NtQueryInformationEnlistment'				NtQueryInformationEnlistment				ntdll		0
imp	'NtQueryInformationFile'				NtQueryInformationFile					ntdll		0	5
imp	'NtQueryInformationJobObject'				NtQueryInformationJobObject				ntdll		0	5
imp	'NtQueryInformationPort'				NtQueryInformationPort					ntdll		0
imp	'NtQueryInformationProcess'				NtQueryInformationProcess				ntdll		0	5
imp	'NtQueryInformationResourceManager'			NtQueryInformationResourceManager			ntdll		0
imp	'NtQueryInformationThread'				NtQueryInformationThread				ntdll		0	5
imp	'NtQueryInformationToken'				NtQueryInformationToken					ntdll		0	5
imp	'NtQueryInformationTransaction'				NtQueryInformationTransaction				ntdll		0
imp	'NtQueryInformationTransactionManager'			NtQueryInformationTransactionManager			ntdll		0
imp	'NtQueryInformationWorkerFactory'			NtQueryInformationWorkerFactory				ntdll		0
imp	'NtQueryInstallUILanguage'				NtQueryInstallUILanguage				ntdll		0
imp	'NtQueryIntervalProfile'				NtQueryIntervalProfile					ntdll		0	2
imp	'NtQueryIoCompletion'					NtQueryIoCompletion					ntdll		0
imp	'NtQueryKey'						NtQueryKey						ntdll		0
imp	'NtQueryLicenseValue'					NtQueryLicenseValue					ntdll		0
imp	'NtQueryMultipleValueKey'				NtQueryMultipleValueKey					ntdll		0
imp	'NtQueryMutant'						NtQueryMutant						ntdll		0
imp	'NtQueryObject'						NtQueryObject						ntdll		0	5
imp	'NtQueryOpenSubKeys'					NtQueryOpenSubKeys					ntdll		0
imp	'NtQueryOpenSubKeysEx'					NtQueryOpenSubKeysEx					ntdll		0
imp	'NtQueryPerformanceCounter'				NtQueryPerformanceCounter				ntdll		0	2
imp	'NtQueryPortInformationProcess'				NtQueryPortInformationProcess				ntdll		0
imp	'NtQueryQuotaInformationFile'				NtQueryQuotaInformationFile				ntdll		0
imp	'NtQuerySection'					NtQuerySection						ntdll		0	5
imp	'NtQuerySecurityAttributesToken'			NtQuerySecurityAttributesToken				ntdll		0
imp	'NtQuerySecurityObject'					NtQuerySecurityObject					ntdll		0	5
imp	'NtQuerySecurityPolicy'					NtQuerySecurityPolicy					ntdll		0
imp	'NtQuerySemaphore'					NtQuerySemaphore					ntdll		0
imp	'NtQuerySymbolicLinkObject'				NtQuerySymbolicLinkObject				ntdll		0	3
imp	'NtQuerySystemEnvironmentValue'				NtQuerySystemEnvironmentValue				ntdll		0
imp	'NtQuerySystemEnvironmentValueEx'			NtQuerySystemEnvironmentValueEx				ntdll		0
imp	'NtQuerySystemInformation'				NtQuerySystemInformation				ntdll		0	4
imp	'NtQuerySystemInformationEx'				NtQuerySystemInformationEx				ntdll		0
imp	'NtQuerySystemTime'					NtQuerySystemTime					ntdll		0	1
imp	'NtQueryTimer'						NtQueryTimer						ntdll		0
imp	'NtQueryTimerResolution'				NtQueryTimerResolution					ntdll		0
imp	'NtQueryValueKey'					NtQueryValueKey						ntdll		0	6
imp	'NtQueryVirtualMemory'					NtQueryVirtualMemory					ntdll		0	6
imp	'NtQueryVolumeInformationFile'				NtQueryVolumeInformationFile				ntdll		0	5
imp	'NtQueryWnfStateData'					NtQueryWnfStateData					ntdll		0
imp	'NtQueryWnfStateNameInformation'			NtQueryWnfStateNameInformation				ntdll		0
imp	'NtQueueApcThread'					NtQueueApcThread					ntdll		0	5
imp	'NtQueueApcThreadEx'					NtQueueApcThreadEx					ntdll		0
imp	'NtRaiseException'					NtRaiseException					ntdll		0	3
imp	'NtRaiseHardError'					NtRaiseHardError					ntdll		0	6
imp	'NtReadFile'						NtReadFile						ntdll		0	9
imp	'NtReadFileScatter'					NtReadFileScatter					ntdll		0
imp	'NtReadOnlyEnlistment'					NtReadOnlyEnlistment					ntdll		0
imp	'NtReadRequestData'					NtReadRequestData					ntdll		0
imp	'NtReadVirtualMemory'					NtReadVirtualMemory					ntdll		0	5
imp	'NtRecoverEnlistment'					NtRecoverEnlistment					ntdll		0
imp	'NtRecoverResourceManager'				NtRecoverResourceManager				ntdll		0
imp	'NtRecoverTransactionManager'				NtRecoverTransactionManager				ntdll		0
imp	'NtRegisterProtocolAddressInformation'			NtRegisterProtocolAddressInformation			ntdll		0
imp	'NtRegisterThreadTerminatePort'				NtRegisterThreadTerminatePort				ntdll		0
imp	'NtReleaseKeyedEvent'					NtReleaseKeyedEvent					ntdll		0	4
imp	'NtReleaseMutant'					NtReleaseMutant						ntdll		0
imp	'NtReleaseSemaphore'					NtReleaseSemaphore					ntdll		0
imp	'NtReleaseWorkerFactoryWorker'				NtReleaseWorkerFactoryWorker				ntdll		0
imp	'NtRemoveIoCompletion'					NtRemoveIoCompletion					ntdll		0
imp	'NtRemoveIoCompletionEx'				NtRemoveIoCompletionEx					ntdll		0
imp	'NtRemoveProcessDebug'					NtRemoveProcessDebug					ntdll		0
imp	'NtRenameKey'						NtRenameKey						ntdll		0
imp	'NtRenameTransactionManager'				NtRenameTransactionManager				ntdll		0
imp	'NtReplaceKey'						NtReplaceKey						ntdll		0
imp	'NtReplacePartitionUnit'				NtReplacePartitionUnit					ntdll		0
imp	'NtReplyPort'						NtReplyPort						ntdll		0
imp	'NtReplyWaitReceivePort'				NtReplyWaitReceivePort					ntdll		0
imp	'NtReplyWaitReceivePortEx'				NtReplyWaitReceivePortEx				ntdll		0
imp	'NtReplyWaitReplyPort'					NtReplyWaitReplyPort					ntdll		0
imp	'NtRequestPort'						NtRequestPort						ntdll		0
imp	'NtRequestWaitReplyPort'				NtRequestWaitReplyPort					ntdll		0
imp	'NtResetEvent'						NtResetEvent						ntdll		0
imp	'NtResetWriteWatch'					NtResetWriteWatch					ntdll		0
imp	'NtRestoreKey'						NtRestoreKey						ntdll		0
imp	'NtResumeProcess'					NtResumeProcess						ntdll		0
imp	'NtResumeThread'					NtResumeThread						ntdll		0	2
imp	'NtRevertContainerImpersonation'			NtRevertContainerImpersonation				ntdll		0
imp	'NtRollbackComplete'					NtRollbackComplete					ntdll		0
imp	'NtRollbackEnlistment'					NtRollbackEnlistment					ntdll		0
imp	'NtRollbackRegistryTransaction'				NtRollbackRegistryTransaction				ntdll		0
imp	'NtRollbackTransaction'					NtRollbackTransaction					ntdll		0
imp	'NtRollforwardTransactionManager'			NtRollforwardTransactionManager				ntdll		0
imp	'NtSaveKey'						NtSaveKey						ntdll		0
imp	'NtSaveKeyEx'						NtSaveKeyEx						ntdll		0
imp	'NtSaveMergedKeys'					NtSaveMergedKeys					ntdll		0
imp	'NtSecureConnectPort'					NtSecureConnectPort					ntdll		0
imp	'NtSerializeBoot'					NtSerializeBoot						ntdll		0
imp	'NtSetBootEntryOrder'					NtSetBootEntryOrder					ntdll		0
imp	'NtSetBootOptions'					NtSetBootOptions					ntdll		0
imp	'NtSetCachedSigningLevel'				NtSetCachedSigningLevel					ntdll		0
imp	'NtSetCachedSigningLevel2'				NtSetCachedSigningLevel2				ntdll		0
imp	'NtSetContextThread'					NtSetContextThread					ntdll		0	2
imp	'NtSetDebugFilterState'					NtSetDebugFilterState					ntdll		0
imp	'NtSetDefaultHardErrorPort'				NtSetDefaultHardErrorPort				ntdll		0
imp	'NtSetDefaultLocale'					NtSetDefaultLocale					ntdll		0
imp	'NtSetDefaultUILanguage'				NtSetDefaultUILanguage					ntdll		0
imp	'NtSetDriverEntryOrder'					NtSetDriverEntryOrder					ntdll		0
imp	'NtSetEaFile'						NtSetEaFile						ntdll		0
imp	'NtSetEvent'						NtSetEvent						ntdll		0	2
imp	'NtSetEventBoostPriority'				NtSetEventBoostPriority					ntdll		0
imp	'NtSetHighEventPair'					NtSetHighEventPair					ntdll		0
imp	'NtSetHighWaitLowEventPair'				NtSetHighWaitLowEventPair				ntdll		0
imp	'NtSetIRTimer'						NtSetIRTimer						ntdll		0
imp	'NtSetInformationDebugObject'				NtSetInformationDebugObject				ntdll		0
imp	'NtSetInformationEnlistment'				NtSetInformationEnlistment				ntdll		0
imp	'NtSetInformationFile'					NtSetInformationFile					ntdll		0	5
imp	'NtSetInformationJobObject'				NtSetInformationJobObject				ntdll		0
imp	'NtSetInformationKey'					NtSetInformationKey					ntdll		0
imp	'NtSetInformationObject'				NtSetInformationObject					ntdll		0
imp	'NtSetInformationProcess'				NtSetInformationProcess					ntdll		0
imp	'NtSetInformationResourceManager'			NtSetInformationResourceManager				ntdll		0
imp	'NtSetInformationSymbolicLink'				NtSetInformationSymbolicLink				ntdll		0
imp	'NtSetInformationThread'				NtSetInformationThread					ntdll		0	4
imp	'NtSetInformationToken'					NtSetInformationToken					ntdll		0
imp	'NtSetInformationTransaction'				NtSetInformationTransaction				ntdll		0
imp	'NtSetInformationTransactionManager'			NtSetInformationTransactionManager			ntdll		0
imp	'NtSetInformationVirtualMemory'				NtSetInformationVirtualMemory				ntdll		0
imp	'NtSetInformationWorkerFactory'				NtSetInformationWorkerFactory				ntdll		0
imp	'NtSetIntervalProfile'					NtSetIntervalProfile					ntdll		0	2
imp	'NtSetIoCompletion'					NtSetIoCompletion					ntdll		0
imp	'NtSetIoCompletionEx'					NtSetIoCompletionEx					ntdll		0
imp	'NtSetLdtEntries'					NtSetLdtEntries						ntdll		0
imp	'NtSetLowEventPair'					NtSetLowEventPair					ntdll		0
imp	'NtSetLowWaitHighEventPair'				NtSetLowWaitHighEventPair				ntdll		0
imp	'NtSetQuotaInformationFile'				NtSetQuotaInformationFile				ntdll		0
imp	'NtSetSecurityObject'					NtSetSecurityObject					ntdll		0
imp	'NtSetSystemEnvironmentValue'				NtSetSystemEnvironmentValue				ntdll		0
imp	'NtSetSystemEnvironmentValueEx'				NtSetSystemEnvironmentValueEx				ntdll		0
imp	'NtSetSystemInformation'				NtSetSystemInformation					ntdll		0
imp	'NtSetSystemPowerState'					NtSetSystemPowerState					ntdll		0
imp	'NtSetSystemTime'					NtSetSystemTime						ntdll		0
imp	'NtSetThreadExecutionState'				NtSetThreadExecutionState				ntdll		0
imp	'NtSetTimer'						NtSetTimer						ntdll		0	7
imp	'NtSetTimer2'						NtSetTimer2						ntdll		0
imp	'NtSetTimerEx'						NtSetTimerEx						ntdll		0
imp	'NtSetTimerResolution'					NtSetTimerResolution					ntdll		0
imp	'NtSetUuidSeed'						NtSetUuidSeed						ntdll		0
imp	'NtSetValueKey'						NtSetValueKey						ntdll		0	6
imp	'NtSetVolumeInformationFile'				NtSetVolumeInformationFile				ntdll		0
imp	'NtSetWnfProcessNotificationEvent'			NtSetWnfProcessNotificationEvent			ntdll		0
imp	'NtShutdownSystem'					NtShutdownSystem					ntdll		0
imp	'NtShutdownWorkerFactory'				NtShutdownWorkerFactory					ntdll		0
imp	'NtSignalAndWaitForSingleObject'			NtSignalAndWaitForSingleObject				ntdll		0	4
imp	'NtSinglePhaseReject'					NtSinglePhaseReject					ntdll		0
imp	'NtStartProfile'					NtStartProfile						ntdll		0	1
imp	'NtStopProfile'						NtStopProfile						ntdll		0	1
imp	'NtSubscribeWnfStateChange'				NtSubscribeWnfStateChange				ntdll		0
imp	'NtSuspendProcess'					NtSuspendProcess					ntdll		0
imp	'NtSuspendThread'					NtSuspendThread						ntdll		0	2
imp	'NtSystemDebugControl'					NtSystemDebugControl					ntdll		0
imp	'NtTerminateEnclave'					NtTerminateEnclave					ntdll		0
imp	'NtTerminateJobObject'					NtTerminateJobObject					ntdll		0
imp	'NtTerminateProcess'					NtTerminateProcess					ntdll		0	2
imp	'NtTerminateThread'					NtTerminateThread					ntdll		0	2
imp	'NtTestAlert'						NtTestAlert						ntdll		0	0
imp	'NtThawRegistry'					NtThawRegistry						ntdll		0
imp	'NtThawTransactions'					NtThawTransactions					ntdll		0
imp	'NtTraceControl'					NtTraceControl						ntdll		0
imp	'NtTraceEvent'						NtTraceEvent						ntdll		0
imp	'NtTranslateFilePath'					NtTranslateFilePath					ntdll		0
imp	'NtUmsThreadYield'					NtUmsThreadYield					ntdll		0
imp	'NtUnloadDriver'					NtUnloadDriver						ntdll		0
imp	'NtUnloadKey'						NtUnloadKey						ntdll		0
imp	'NtUnloadKey2'						NtUnloadKey2						ntdll		0
imp	'NtUnloadKeyEx'						NtUnloadKeyEx						ntdll		0
imp	'NtUnlockFile'						NtUnlockFile						ntdll		0
imp	'NtUnlockVirtualMemory'					NtUnlockVirtualMemory					ntdll		0
imp	'NtUnmapViewOfSection'					NtUnmapViewOfSection					ntdll		0	2
imp	'NtUnmapViewOfSectionEx'				NtUnmapViewOfSectionEx					ntdll		0
imp	'NtUnsubscribeWnfStateChange'				NtUnsubscribeWnfStateChange				ntdll		0
imp	'NtUpdateWnfStateData'					NtUpdateWnfStateData					ntdll		0
imp	'NtVdm64CreateProcessInternal'				NtVdm64CreateProcessInternalW				kernel32	0
imp	'NtVdmControl'						NtVdmControl						ntdll		0
imp	'NtWaitForAlertByThreadId'				NtWaitForAlertByThreadId				ntdll		0
imp	'NtWaitForDebugEvent'					NtWaitForDebugEvent					ntdll		0
imp	'NtWaitForKeyedEvent'					NtWaitForKeyedEvent					ntdll		0	4
imp	'NtWaitForMultipleObjects'				NtWaitForMultipleObjects				ntdll		0
imp	'NtWaitForMultipleObjects32'				NtWaitForMultipleObjects32				ntdll		0
imp	'NtWaitForSingleObject'					NtWaitForSingleObject					ntdll		0	3
imp	'NtWaitForWorkViaWorkerFactory'				NtWaitForWorkViaWorkerFactory				ntdll		0
imp	'NtWaitHighEventPair'					NtWaitHighEventPair					ntdll		0
imp	'NtWaitLowEventPair'					NtWaitLowEventPair					ntdll		0
imp	'NtWorkerFactoryWorkerReady'				NtWorkerFactoryWorkerReady				ntdll		0
imp	'NtWriteFile'						NtWriteFile						ntdll		0	9
imp	'NtWriteFileGather'					NtWriteFileGather					ntdll		0
imp	'NtWriteRequestData'					NtWriteRequestData					ntdll		0
imp	'NtWriteVirtualMemory'					NtWriteVirtualMemory					ntdll		0	5
imp	'NtYieldExecution'					NtYieldExecution					ntdll		0	0
imp	'NtdllDefWindowProc_W'					NtdllDefWindowProc_W					ntdll		0
imp	'NtdllDialogWndProc_W'					NtdllDialogWndProc_W					ntdll		0
imp	'PfxFindPrefix'						PfxFindPrefix						ntdll		0
imp	'PfxInitialize'						PfxInitialize						ntdll		0
imp	'PfxInsertPrefix'					PfxInsertPrefix						ntdll		0
imp	'PfxRemovePrefix'					PfxRemovePrefix						ntdll		0
imp	'PssNtCaptureSnapshot'					PssNtCaptureSnapshot					ntdll		0
imp	'PssNtDuplicateSnapshot'				PssNtDuplicateSnapshot					ntdll		0
imp	'PssNtFreeRemoteSnapshot'				PssNtFreeRemoteSnapshot					ntdll		0
imp	'PssNtFreeSnapshot'					PssNtFreeSnapshot					ntdll		0
imp	'PssNtFreeWalkMarker'					PssNtFreeWalkMarker					ntdll		0
imp	'PssNtQuerySnapshot'					PssNtQuerySnapshot					ntdll		0
imp	'PssNtValidateDescriptor'				PssNtValidateDescriptor					ntdll		0
imp	'PssNtWalkSnapshot'					PssNtWalkSnapshot					ntdll		0
imp	'RtlAbortRXact'						RtlAbortRXact						ntdll		0
imp	'RtlAbsoluteToSelfRelativeSD'				RtlAbsoluteToSelfRelativeSD				ntdll		0
imp	'RtlAcquirePebLock'					RtlAcquirePebLock					ntdll		0
imp	'RtlAcquirePrivilege'					RtlAcquirePrivilege					ntdll		0
imp	'RtlAcquireReleaseSRWLockExclusive'			RtlAcquireReleaseSRWLockExclusive			ntdll		0
imp	'RtlAcquireResourceExclusive'				RtlAcquireResourceExclusive				ntdll		0
imp	'RtlAcquireResourceShared'				RtlAcquireResourceShared				ntdll		0
imp	'RtlAcquireSRWLockExclusive'				RtlAcquireSRWLockExclusive				ntdll		0
imp	'RtlAcquireSRWLockShared'				RtlAcquireSRWLockShared					ntdll		0
imp	'RtlActivateActivationContext'				RtlActivateActivationContext				ntdll		0
imp	'RtlActivateActivationContextEx'			RtlActivateActivationContextEx				ntdll		0
imp	'RtlActivateActivationContextUnsafeFast'		RtlActivateActivationContextUnsafeFast			ntdll		0
imp	'RtlAddAccessAllowedAce'				RtlAddAccessAllowedAce					ntdll		0
imp	'RtlAddAccessAllowedAceEx'				RtlAddAccessAllowedAceEx				ntdll		0
imp	'RtlAddAccessAllowedObjectAce'				RtlAddAccessAllowedObjectAce				ntdll		0
imp	'RtlAddAccessDeniedAce'					RtlAddAccessDeniedAce					ntdll		0
imp	'RtlAddAccessDeniedAceEx'				RtlAddAccessDeniedAceEx					ntdll		0
imp	'RtlAddAccessDeniedObjectAce'				RtlAddAccessDeniedObjectAce				ntdll		0
imp	'RtlAddAccessFilterAce'					RtlAddAccessFilterAce					ntdll		0
imp	'RtlAddAce'						RtlAddAce						ntdll		0
imp	'RtlAddActionToRXact'					RtlAddActionToRXact					ntdll		0
imp	'RtlAddAtomToAtomTable'					RtlAddAtomToAtomTable					ntdll		0
imp	'RtlAddAttributeActionToRXact'				RtlAddAttributeActionToRXact				ntdll		0
imp	'RtlAddAuditAccessAce'					RtlAddAuditAccessAce					ntdll		0
imp	'RtlAddAuditAccessAceEx'				RtlAddAuditAccessAceEx					ntdll		0
imp	'RtlAddAuditAccessObjectAce'				RtlAddAuditAccessObjectAce				ntdll		0
imp	'RtlAddCompoundAce'					RtlAddCompoundAce					ntdll		0
imp	'RtlAddFunctionTable'					RtlAddFunctionTable					ntdll		0
imp	'RtlAddGrowableFunctionTable'				RtlAddGrowableFunctionTable				ntdll		0
imp	'RtlAddIntegrityLabelToBoundaryDescriptor'		RtlAddIntegrityLabelToBoundaryDescriptor		ntdll		0
imp	'RtlAddMandatoryAce'					RtlAddMandatoryAce					ntdll		0
imp	'RtlAddProcessTrustLabelAce'				RtlAddProcessTrustLabelAce				ntdll		0
imp	'RtlAddRefActivationContext'				RtlAddRefActivationContext				ntdll		0
imp	'RtlAddRefMemoryStream'					RtlAddRefMemoryStream					ntdll		0
imp	'RtlAddResourceAttributeAce'				RtlAddResourceAttributeAce				ntdll		0
imp	'RtlAddSIDToBoundaryDescriptor'				RtlAddSIDToBoundaryDescriptor				ntdll		0
imp	'RtlAddScopedPolicyIDAce'				RtlAddScopedPolicyIDAce					ntdll		0
imp	'RtlAddVectoredContinueHandler'				RtlAddVectoredContinueHandler				ntdll		0
imp	'RtlAddVectoredExceptionHandler'			RtlAddVectoredExceptionHandler				ntdll		0
imp	'RtlAddressInSectionTable'				RtlAddressInSectionTable				ntdll		0
imp	'RtlAdjustPrivilege'					RtlAdjustPrivilege					ntdll		0
imp	'RtlAllocateActivationContextStack'			RtlAllocateActivationContextStack			ntdll		0
imp	'RtlAllocateAndInitializeSid'				RtlAllocateAndInitializeSid				ntdll		0
imp	'RtlAllocateAndInitializeSidEx'				RtlAllocateAndInitializeSidEx				ntdll		0
imp	'RtlAllocateHandle'					RtlAllocateHandle					ntdll		0
imp	'RtlAllocateHeap'					RtlAllocateHeap						ntdll		0	3
imp	'RtlAllocateMemoryBlockLookaside'			RtlAllocateMemoryBlockLookaside				ntdll		0
imp	'RtlAllocateMemoryZone'					RtlAllocateMemoryZone					ntdll		0
imp	'RtlAllocateWnfSerializationGroup'			RtlAllocateWnfSerializationGroup			ntdll		0
imp	'RtlAnsiCharToUnicodeChar'				RtlAnsiCharToUnicodeChar				ntdll		0
imp	'RtlAnsiStringToUnicodeSize'				RtlAnsiStringToUnicodeSize				ntdll		0
imp	'RtlAnsiStringToUnicodeString'				RtlAnsiStringToUnicodeString				ntdll		0
imp	'RtlAppendAsciizToString'				RtlAppendAsciizToString					ntdll		0
imp	'RtlAppendPathElement'					RtlAppendPathElement					ntdll		0
imp	'RtlAppendStringToString'				RtlAppendStringToString					ntdll		0
imp	'RtlAppendUnicodeStringToString'			RtlAppendUnicodeStringToString				ntdll		0
imp	'RtlAppendUnicodeToString'				RtlAppendUnicodeToString				ntdll		0
imp	'RtlApplicationVerifierStop'				RtlApplicationVerifierStop				ntdll		0
imp	'RtlApplyRXact'						RtlApplyRXact						ntdll		0
imp	'RtlApplyRXactNoFlush'					RtlApplyRXactNoFlush					ntdll		0
imp	'RtlAppxIsFileOwnedByTrustedInstaller'			RtlAppxIsFileOwnedByTrustedInstaller			ntdll		0
imp	'RtlAreAllAccessesGranted'				RtlAreAllAccessesGranted				ntdll		0
imp	'RtlAreAnyAccessesGranted'				RtlAreAnyAccessesGranted				ntdll		0
imp	'RtlAreBitsClear'					RtlAreBitsClear						ntdll		0
imp	'RtlAreBitsSet'						RtlAreBitsSet						ntdll		0
imp	'RtlAreLongPathsEnabled'				RtlAreLongPathsEnabled					ntdll		0
imp	'RtlAssert'						RtlAssert						ntdll		0
imp	'RtlAvlInsertNodeEx'					RtlAvlInsertNodeEx					ntdll		0
imp	'RtlAvlRemoveNode'					RtlAvlRemoveNode					ntdll		0
imp	'RtlBarrier'						RtlBarrier						ntdll		0
imp	'RtlBarrierForDelete'					RtlBarrierForDelete					ntdll		0
imp	'RtlCallEnclaveReturn'					RtlCallEnclaveReturn					ntdll		0
imp	'RtlCancelTimer'					RtlCancelTimer						ntdll		0
imp	'RtlCanonicalizeDomainName'				RtlCanonicalizeDomainName				ntdll		0
imp	'RtlCapabilityCheck'					RtlCapabilityCheck					ntdll		0
imp	'RtlCapabilityCheckForSingleSessionSku'			RtlCapabilityCheckForSingleSessionSku			ntdll		0
imp	'RtlCaptureContext'					RtlCaptureContext					ntdll		0
imp	'RtlCaptureStackBackTrace'				RtlCaptureStackBackTrace				ntdll		0
imp	'RtlCharToInteger'					RtlCharToInteger					ntdll		0
imp	'RtlCheckBootStatusIntegrity'				RtlCheckBootStatusIntegrity				ntdll		0
imp	'RtlCheckForOrphanedCriticalSections'			RtlCheckForOrphanedCriticalSections			ntdll		0
imp	'RtlCheckPortableOperatingSystem'			RtlCheckPortableOperatingSystem				ntdll		0
imp	'RtlCheckRegistryKey'					RtlCheckRegistryKey					ntdll		0
imp	'RtlCheckSandboxedToken'				RtlCheckSandboxedToken					ntdll		0
imp	'RtlCheckSystemBootStatusIntegrity'			RtlCheckSystemBootStatusIntegrity			ntdll		0
imp	'RtlCheckTokenCapability'				RtlCheckTokenCapability					ntdll		0
imp	'RtlCheckTokenMembership'				RtlCheckTokenMembership					ntdll		0
imp	'RtlCheckTokenMembershipEx'				RtlCheckTokenMembershipEx				ntdll		0
imp	'RtlCleanUpTEBLangLists'				RtlCleanUpTEBLangLists					ntdll		0
imp	'RtlClearAllBits'					RtlClearAllBits						ntdll		0
imp	'RtlClearBit'						RtlClearBit						ntdll		0
imp	'RtlClearBits'						RtlClearBits						ntdll		0
imp	'RtlClearThreadWorkOnBehalfTicket'			RtlClearThreadWorkOnBehalfTicket			ntdll		0
imp	'RtlCloneMemoryStream'					RtlCloneMemoryStream					ntdll		0
imp	'RtlCloneUserProcess'					RtlCloneUserProcess					ntdll		0	5
imp	'RtlCmDecodeMemIoResource'				RtlCmDecodeMemIoResource				ntdll		0
imp	'RtlCmEncodeMemIoResource'				RtlCmEncodeMemIoResource				ntdll		0
imp	'RtlCommitDebugInfo'					RtlCommitDebugInfo					ntdll		0
imp	'RtlCommitMemoryStream'					RtlCommitMemoryStream					ntdll		0
imp	'RtlCompactHeap'					RtlCompactHeap						ntdll		0
imp	'RtlCompareAltitudes'					RtlCompareAltitudes					ntdll		0
imp	'RtlCompareMemory'					RtlCompareMemory					ntdll		0
imp	'RtlCompareMemoryUlong'					RtlCompareMemoryUlong					ntdll		0
imp	'RtlCompareString'					RtlCompareString					ntdll		0
imp	'RtlCompareUnicodeString'				RtlCompareUnicodeString					ntdll		0
imp	'RtlCompareUnicodeStrings'				RtlCompareUnicodeStrings				ntdll		0
imp	'RtlCompleteProcessCloning'				RtlCompleteProcessCloning				ntdll		0
imp	'RtlCompressBuffer'					RtlCompressBuffer					ntdll		0
imp	'RtlComputeCrc32'					RtlComputeCrc32						ntdll		0
imp	'RtlComputeImportTableHash'				RtlComputeImportTableHash				ntdll		0
imp	'RtlComputePrivatizedDllName_U'				RtlComputePrivatizedDllName_U				ntdll		0
imp	'RtlConnectToSm'					RtlConnectToSm						ntdll		0
imp	'RtlConsoleMultiByteToUnicodeN'				RtlConsoleMultiByteToUnicodeN				ntdll		0
imp	'RtlContractHashTable'					RtlContractHashTable					ntdll		0
imp	'RtlConvertDeviceFamilyInfoToString'			RtlConvertDeviceFamilyInfoToString			ntdll		0
imp	'RtlConvertExclusiveToShared'				RtlConvertExclusiveToShared				ntdll		0
imp	'RtlConvertLCIDToString'				RtlConvertLCIDToString					ntdll		0
imp	'RtlConvertSRWLockExclusiveToShared'			RtlConvertSRWLockExclusiveToShared			ntdll		0
imp	'RtlConvertSharedToExclusive'				RtlConvertSharedToExclusive				ntdll		0
imp	'RtlConvertSidToUnicodeString'				RtlConvertSidToUnicodeString				ntdll		0	3
imp	'RtlConvertToAutoInheritSecurityObject'			RtlConvertToAutoInheritSecurityObject			ntdll		0
imp	'RtlCopyBitMap'						RtlCopyBitMap						ntdll		0
imp	'RtlCopyContext'					RtlCopyContext						ntdll		0
imp	'RtlCopyExtendedContext'				RtlCopyExtendedContext					ntdll		0
imp	'RtlCopyLuid'						RtlCopyLuid						ntdll		0
imp	'RtlCopyLuidAndAttributesArray'				RtlCopyLuidAndAttributesArray				ntdll		0
imp	'RtlCopyMappedMemory'					RtlCopyMappedMemory					ntdll		0
imp	'RtlCopyMemory'						RtlCopyMemory						ntdll		0
imp	'RtlCopyMemoryNonTemporal'				RtlCopyMemoryNonTemporal				ntdll		0
imp	'RtlCopyMemoryStreamTo'					RtlCopyMemoryStreamTo					ntdll		0
imp	'RtlCopyOutOfProcessMemoryStreamTo'			RtlCopyOutOfProcessMemoryStreamTo			ntdll		0
imp	'RtlCopySecurityDescriptor'				RtlCopySecurityDescriptor				ntdll		0
imp	'RtlCopySid'						RtlCopySid						ntdll		0
imp	'RtlCopySidAndAttributesArray'				RtlCopySidAndAttributesArray				ntdll		0
imp	'RtlCopyString'						RtlCopyString						ntdll		0
imp	'RtlCopyUnicodeString'					RtlCopyUnicodeString					ntdll		0
imp	'RtlCrc32'						RtlCrc32						ntdll		0
imp	'RtlCrc64'						RtlCrc64						ntdll		0
imp	'RtlCreateAcl'						RtlCreateAcl						ntdll		0
imp	'RtlCreateActivationContext'				RtlCreateActivationContext				ntdll		0
imp	'RtlCreateAndSetSD'					RtlCreateAndSetSD					ntdll		0
imp	'RtlCreateAtomTable'					RtlCreateAtomTable					ntdll		0
imp	'RtlCreateBootStatusDataFile'				RtlCreateBootStatusDataFile				ntdll		0
imp	'RtlCreateBoundaryDescriptor'				RtlCreateBoundaryDescriptor				ntdll		0
imp	'RtlCreateEnvironment'					RtlCreateEnvironment					ntdll		0
imp	'RtlCreateEnvironmentEx'				RtlCreateEnvironmentEx					ntdll		0
imp	'RtlCreateHashTable'					RtlCreateHashTable					ntdll		0
imp	'RtlCreateHashTableEx'					RtlCreateHashTableEx					ntdll		0
imp	'RtlCreateHeap'						RtlCreateHeap						ntdll		0	6
imp	'RtlCreateMemoryBlockLookaside'				RtlCreateMemoryBlockLookaside				ntdll		0
imp	'RtlCreateMemoryZone'					RtlCreateMemoryZone					ntdll		0
imp	'RtlCreateProcessParameters'				RtlCreateProcessParameters				ntdll		0	10
imp	'RtlCreateProcessParametersEx'				RtlCreateProcessParametersEx				ntdll		0
imp	'RtlCreateProcessReflection'				RtlCreateProcessReflection				ntdll		0
imp	'RtlCreateQueryDebugBuffer'				RtlCreateQueryDebugBuffer				ntdll		0
imp	'RtlCreateRegistryKey'					RtlCreateRegistryKey					ntdll		0
imp	'RtlCreateSecurityDescriptor'				RtlCreateSecurityDescriptor				ntdll		0
imp	'RtlCreateServiceSid'					RtlCreateServiceSid					ntdll		0
imp	'RtlCreateSystemVolumeInformationFolder'		RtlCreateSystemVolumeInformationFolder			ntdll		0
imp	'RtlCreateTagHeap'					RtlCreateTagHeap					ntdll		0
imp	'RtlCreateTimer'					RtlCreateTimer						ntdll		0
imp	'RtlCreateTimerQueue'					RtlCreateTimerQueue					ntdll		0
imp	'RtlCreateUmsCompletionList'				RtlCreateUmsCompletionList				ntdll		0
imp	'RtlCreateUmsThreadContext'				RtlCreateUmsThreadContext				ntdll		0
imp	'RtlCreateUnicodeString'				RtlCreateUnicodeString					ntdll		0
imp	'RtlCreateUnicodeStringFromAsciiz'			RtlCreateUnicodeStringFromAsciiz			ntdll		0
imp	'RtlCreateUserProcess'					RtlCreateUserProcess					ntdll		0
imp	'RtlCreateUserProcessEx'				RtlCreateUserProcessEx					ntdll		0
imp	'RtlCreateUserSecurityObject'				RtlCreateUserSecurityObject				ntdll		0
imp	'RtlCreateUserStack'					RtlCreateUserStack					ntdll		0
imp	'RtlCreateUserThread'					RtlCreateUserThread					ntdll		0
imp	'RtlCreateVirtualAccountSid'				RtlCreateVirtualAccountSid				ntdll		0
imp	'RtlCultureNameToLCID'					RtlCultureNameToLCID					ntdll		0
imp	'RtlCustomCPToUnicodeN'					RtlCustomCPToUnicodeN					ntdll		0
imp	'RtlCutoverTimeToSystemTime'				RtlCutoverTimeToSystemTime				ntdll		0
imp	'RtlDeCommitDebugInfo'					RtlDeCommitDebugInfo					ntdll		0
imp	'RtlDeNormalizeProcessParams'				RtlDeNormalizeProcessParams				ntdll		0
imp	'RtlDeactivateActivationContext'			RtlDeactivateActivationContext				ntdll		0
imp	'RtlDeactivateActivationContextUnsafeFast'		RtlDeactivateActivationContextUnsafeFast		ntdll		0
imp	'RtlDebugPrintTimes'					RtlDebugPrintTimes					ntdll		0
imp	'RtlDecodePointer'					RtlDecodePointer					ntdll		0
imp	'RtlDecodeRemotePointer'				RtlDecodeRemotePointer					ntdll		0
imp	'RtlDecodeSystemPointer'				RtlDecodeSystemPointer					ntdll		0
imp	'RtlDecompressBuffer'					RtlDecompressBuffer					ntdll		0
imp	'RtlDecompressBufferEx'					RtlDecompressBufferEx					ntdll		0
imp	'RtlDecompressFragment'					RtlDecompressFragment					ntdll		0
imp	'RtlDefaultNpAcl'					RtlDefaultNpAcl						ntdll		0
imp	'RtlDelete'						RtlDelete						ntdll		0
imp	'RtlDeleteAce'						RtlDeleteAce						ntdll		0
imp	'RtlDeleteAtomFromAtomTable'				RtlDeleteAtomFromAtomTable				ntdll		0
imp	'RtlDeleteBarrier'					RtlDeleteBarrier					ntdll		0
imp	'RtlDeleteBoundaryDescriptor'				RtlDeleteBoundaryDescriptor				ntdll		0
imp	'RtlDeleteCriticalSection'				RtlDeleteCriticalSection				ntdll		0	1
imp	'RtlDeleteElementGenericTable'				RtlDeleteElementGenericTable				ntdll		0
imp	'RtlDeleteElementGenericTableAvl'			RtlDeleteElementGenericTableAvl				ntdll		0
imp	'RtlDeleteElementGenericTableAvlEx'			RtlDeleteElementGenericTableAvlEx			ntdll		0
imp	'RtlDeleteFunctionTable'				RtlDeleteFunctionTable					ntdll		0
imp	'RtlDeleteGrowableFunctionTable'			RtlDeleteGrowableFunctionTable				ntdll		0
imp	'RtlDeleteHashTable'					RtlDeleteHashTable					ntdll		0
imp	'RtlDeleteNoSplay'					RtlDeleteNoSplay					ntdll		0
imp	'RtlDeleteRegistryValue'				RtlDeleteRegistryValue					ntdll		0
imp	'RtlDeleteResource'					RtlDeleteResource					ntdll		0
imp	'RtlDeleteSecurityObject'				RtlDeleteSecurityObject					ntdll		0
imp	'RtlDeleteTimer'					RtlDeleteTimer						ntdll		0
imp	'RtlDeleteTimerQueue'					RtlDeleteTimerQueue					ntdll		0
imp	'RtlDeleteTimerQueueEx'					RtlDeleteTimerQueueEx					ntdll		0
imp	'RtlDeleteUmsCompletionList'				RtlDeleteUmsCompletionList				ntdll		0
imp	'RtlDeleteUmsThreadContext'				RtlDeleteUmsThreadContext				ntdll		0
imp	'RtlDequeueUmsCompletionListItems'			RtlDequeueUmsCompletionListItems			ntdll		0
imp	'RtlDeregisterSecureMemoryCacheCallback'		RtlDeregisterSecureMemoryCacheCallback			ntdll		0
imp	'RtlDeregisterWait'					RtlDeregisterWait					ntdll		0
imp	'RtlDeregisterWaitEx'					RtlDeregisterWaitEx					ntdll		0
imp	'RtlDeriveCapabilitySidsFromName'			RtlDeriveCapabilitySidsFromName				ntdll		0
imp	'RtlDestroyAtomTable'					RtlDestroyAtomTable					ntdll		0
imp	'RtlDestroyEnvironment'					RtlDestroyEnvironment					ntdll		0
imp	'RtlDestroyHandleTable'					RtlDestroyHandleTable					ntdll		0
imp	'RtlDestroyHeap'					RtlDestroyHeap						ntdll		0	1
imp	'RtlDestroyMemoryBlockLookaside'			RtlDestroyMemoryBlockLookaside				ntdll		0
imp	'RtlDestroyMemoryZone'					RtlDestroyMemoryZone					ntdll		0
imp	'RtlDestroyProcessParameters'				RtlDestroyProcessParameters				ntdll		0	1
imp	'RtlDestroyQueryDebugBuffer'				RtlDestroyQueryDebugBuffer				ntdll		0
imp	'RtlDetectHeapLeaks'					RtlDetectHeapLeaks					ntdll		0
imp	'RtlDetermineDosPathNameType_U'				RtlDetermineDosPathNameType_U				ntdll		0
imp	'RtlDisableThreadProfiling'				RtlDisableThreadProfiling				ntdll		0
imp	'RtlDllShutdownInProgress'				RtlDllShutdownInProgress				ntdll		0
imp	'RtlDnsHostNameToComputerName'				RtlDnsHostNameToComputerName				ntdll		0
imp	'RtlDoesFileExists_U'					RtlDoesFileExists_U					ntdll		0
imp	'RtlDosApplyFileIsolationRedirection_Ustr'		RtlDosApplyFileIsolationRedirection_Ustr		ntdll		0
imp	'RtlDosLongPathNameToNtPathName_U_WithStatus'		RtlDosLongPathNameToNtPathName_U_WithStatus		ntdll		0
imp	'RtlDosLongPathNameToRelativeNtPathName_U_WithStatus'	RtlDosLongPathNameToRelativeNtPathName_U_WithStatus	ntdll		0
imp	'RtlDosPathNameToNtPathName_U'				RtlDosPathNameToNtPathName_U				ntdll		0
imp	'RtlDosPathNameToNtPathName_U_WithStatus'		RtlDosPathNameToNtPathName_U_WithStatus			ntdll		0
imp	'RtlDosPathNameToRelativeNtPathName_U'			RtlDosPathNameToRelativeNtPathName_U			ntdll		0
imp	'RtlDosPathNameToRelativeNtPathName_U_WithStatus'	RtlDosPathNameToRelativeNtPathName_U_WithStatus		ntdll		0
imp	'RtlDosSearchPath_U'					RtlDosSearchPath_U					ntdll		0
imp	'RtlDosSearchPath_Ustr'					RtlDosSearchPath_Ustr					ntdll		0
imp	'RtlDowncaseUnicodeChar'				RtlDowncaseUnicodeChar					ntdll		0
imp	'RtlDowncaseUnicodeString'				RtlDowncaseUnicodeString				ntdll		0
imp	'RtlDrainNonVolatileFlush'				RtlDrainNonVolatileFlush				ntdll		0
imp	'RtlDumpResource'					RtlDumpResource						ntdll		0
imp	'RtlDuplicateUnicodeString'				RtlDuplicateUnicodeString				ntdll		0
imp	'RtlEmptyAtomTable'					RtlEmptyAtomTable					ntdll		0
imp	'RtlEnableEarlyCriticalSectionEventCreation'		RtlEnableEarlyCriticalSectionEventCreation		ntdll		0
imp	'RtlEnableThreadProfiling'				RtlEnableThreadProfiling				ntdll		0
imp	'RtlEnclaveCallDispatch'				RtlEnclaveCallDispatch					ntdll		0
imp	'RtlEnclaveCallDispatchReturn'				RtlEnclaveCallDispatchReturn				ntdll		0
imp	'RtlEncodePointer'					RtlEncodePointer					ntdll		0
imp	'RtlEncodeRemotePointer'				RtlEncodeRemotePointer					ntdll		0
imp	'RtlEncodeSystemPointer'				RtlEncodeSystemPointer					ntdll		0
imp	'RtlEndEnumerationHashTable'				RtlEndEnumerationHashTable				ntdll		0
imp	'RtlEndStrongEnumerationHashTable'			RtlEndStrongEnumerationHashTable			ntdll		0
imp	'RtlEndWeakEnumerationHashTable'			RtlEndWeakEnumerationHashTable				ntdll		0
imp	'RtlEnterCriticalSection'				RtlEnterCriticalSection					ntdll		0	1
imp	'RtlEnterUmsSchedulingMode'				RtlEnterUmsSchedulingMode				ntdll		0
imp	'RtlEnumProcessHeaps'					RtlEnumProcessHeaps					ntdll		0
imp	'RtlEnumerateEntryHashTable'				RtlEnumerateEntryHashTable				ntdll		0
imp	'RtlEnumerateGenericTable'				RtlEnumerateGenericTable				ntdll		0
imp	'RtlEnumerateGenericTableAvl'				RtlEnumerateGenericTableAvl				ntdll		0
imp	'RtlEnumerateGenericTableLikeADirectory'		RtlEnumerateGenericTableLikeADirectory			ntdll		0
imp	'RtlEnumerateGenericTableWithoutSplaying'		RtlEnumerateGenericTableWithoutSplaying			ntdll		0
imp	'RtlEnumerateGenericTableWithoutSplayingAvl'		RtlEnumerateGenericTableWithoutSplayingAvl		ntdll		0
imp	'RtlEqualComputerName'					RtlEqualComputerName					ntdll		0
imp	'RtlEqualDomainName'					RtlEqualDomainName					ntdll		0
imp	'RtlEqualLuid'						RtlEqualLuid						ntdll		0
imp	'RtlEqualPrefixSid'					RtlEqualPrefixSid					ntdll		0
imp	'RtlEqualSid'						RtlEqualSid						ntdll		0
imp	'RtlEqualString'					RtlEqualString						ntdll		0
imp	'RtlEqualUnicodeString'					RtlEqualUnicodeString					ntdll		0
imp	'RtlEqualWnfChangeStamps'				RtlEqualWnfChangeStamps					ntdll		0
imp	'RtlEraseUnicodeString'					RtlEraseUnicodeString					ntdll		0
imp	'RtlEthernetAddressToString'				RtlEthernetAddressToStringW				ntdll		0
imp	'RtlEthernetStringToAddress'				RtlEthernetStringToAddressW				ntdll		0
imp	'RtlExecuteUmsThread'					RtlExecuteUmsThread					ntdll		0
imp	'RtlExitUserProcess'					RtlExitUserProcess					ntdll		0
imp	'RtlExitUserThread'					RtlExitUserThread					ntdll		0
imp	'RtlExpandEnvironmentStrings'				RtlExpandEnvironmentStrings				ntdll		0
imp	'RtlExpandEnvironmentStrings_U'				RtlExpandEnvironmentStrings_U				ntdll		0
imp	'RtlExpandHashTable'					RtlExpandHashTable					ntdll		0
imp	'RtlExtendCorrelationVector'				RtlExtendCorrelationVector				ntdll		0
imp	'RtlExtendMemoryBlockLookaside'				RtlExtendMemoryBlockLookaside				ntdll		0
imp	'RtlExtendMemoryZone'					RtlExtendMemoryZone					ntdll		0
imp	'RtlExtractBitMap'					RtlExtractBitMap					ntdll		0
imp	'RtlFillMemory'						RtlFillMemory						ntdll		0
imp	'RtlFinalReleaseOutOfProcessMemoryStream'		RtlFinalReleaseOutOfProcessMemoryStream			ntdll		0
imp	'RtlFindAceByType'					RtlFindAceByType					ntdll		0
imp	'RtlFindActivationContextSectionGuid'			RtlFindActivationContextSectionGuid			ntdll		0
imp	'RtlFindActivationContextSectionString'			RtlFindActivationContextSectionString			ntdll		0
imp	'RtlFindCharInUnicodeString'				RtlFindCharInUnicodeString				ntdll		0
imp	'RtlFindClearBits'					RtlFindClearBits					ntdll		0
imp	'RtlFindClearBitsAndSet'				RtlFindClearBitsAndSet					ntdll		0
imp	'RtlFindClearRuns'					RtlFindClearRuns					ntdll		0
imp	'RtlFindClosestEncodableLength'				RtlFindClosestEncodableLength				ntdll		0
imp	'RtlFindExportedRoutineByName'				RtlFindExportedRoutineByName				ntdll		0
imp	'RtlFindLastBackwardRunClear'				RtlFindLastBackwardRunClear				ntdll		0
imp	'RtlFindLeastSignificantBit'				RtlFindLeastSignificantBit				ntdll		0
imp	'RtlFindLongestRunClear'				RtlFindLongestRunClear					ntdll		0
imp	'RtlFindMessage'					RtlFindMessage						ntdll		0
imp	'RtlFindMostSignificantBit'				RtlFindMostSignificantBit				ntdll		0
imp	'RtlFindNextForwardRunClear'				RtlFindNextForwardRunClear				ntdll		0
imp	'RtlFindSetBits'					RtlFindSetBits						ntdll		0
imp	'RtlFindSetBitsAndClear'				RtlFindSetBitsAndClear					ntdll		0
imp	'RtlFindUnicodeSubstring'				RtlFindUnicodeSubstring					ntdll		0
imp	'RtlFirstEntrySList'					RtlFirstEntrySList					ntdll		0
imp	'RtlFirstFreeAce'					RtlFirstFreeAce						ntdll		0
imp	'RtlFlsAlloc'						RtlFlsAlloc						ntdll		0
imp	'RtlFlsFree'						RtlFlsFree						ntdll		0
imp	'RtlFlushHeaps'						RtlFlushHeaps						ntdll		0
imp	'RtlFlushNonVolatileMemory'				RtlFlushNonVolatileMemory				ntdll		0
imp	'RtlFlushNonVolatileMemoryRanges'			RtlFlushNonVolatileMemoryRanges				ntdll		0
imp	'RtlFlushSecureMemoryCache'				RtlFlushSecureMemoryCache				ntdll		0
imp	'RtlFormatCurrentUserKeyPath'				RtlFormatCurrentUserKeyPath				ntdll		0
imp	'RtlFormatMessage'					RtlFormatMessage					ntdll		0
imp	'RtlFormatMessageEx'					RtlFormatMessageEx					ntdll		0
imp	'RtlFreeActivationContextStack'				RtlFreeActivationContextStack				ntdll		0
imp	'RtlFreeAnsiString'					RtlFreeAnsiString					ntdll		0
imp	'RtlFreeHandle'						RtlFreeHandle						ntdll		0
imp	'RtlFreeHeap'						RtlFreeHeap						ntdll		0	3
imp	'RtlFreeMemoryBlockLookaside'				RtlFreeMemoryBlockLookaside				ntdll		0
imp	'RtlFreeNonVolatileToken'				RtlFreeNonVolatileToken					ntdll		0
imp	'RtlFreeOemString'					RtlFreeOemString					ntdll		0
imp	'RtlFreeSid'						RtlFreeSid						ntdll		0
imp	'RtlFreeThreadActivationContextStack'			RtlFreeThreadActivationContextStack			ntdll		0
imp	'RtlFreeUnicodeString'					RtlFreeUnicodeString					ntdll		0	1
imp	'RtlFreeUserStack'					RtlFreeUserStack					ntdll		0
imp	'RtlGUIDFromString'					RtlGUIDFromString					ntdll		0
imp	'RtlGenerate8dot3Name'					RtlGenerate8dot3Name					ntdll		0
imp	'RtlGetAce'						RtlGetAce						ntdll		0
imp	'RtlGetActiveActivationContext'				RtlGetActiveActivationContext				ntdll		0
imp	'RtlGetActiveConsoleId'					RtlGetActiveConsoleId					ntdll		0
imp	'RtlGetAppContainerNamedObjectPath'			RtlGetAppContainerNamedObjectPath			ntdll		0
imp	'RtlGetAppContainerParent'				RtlGetAppContainerParent				ntdll		0
imp	'RtlGetAppContainerSidType'				RtlGetAppContainerSidType				ntdll		0
imp	'RtlGetCallersAddress'					RtlGetCallersAddress					ntdll		0
imp	'RtlGetCompressionWorkSpaceSize'			RtlGetCompressionWorkSpaceSize				ntdll		0
imp	'RtlGetConsoleSessionForegroundProcessId'		RtlGetConsoleSessionForegroundProcessId			ntdll		0
imp	'RtlGetControlSecurityDescriptor'			RtlGetControlSecurityDescriptor				ntdll		0
imp	'RtlGetCriticalSectionRecursionCount'			RtlGetCriticalSectionRecursionCount			ntdll		0
imp	'RtlGetCurrentDirectory_U'				RtlGetCurrentDirectory_U				ntdll		0
imp	'RtlGetCurrentPeb'					RtlGetCurrentPeb					ntdll		0
imp	'RtlGetCurrentProcessorNumber'				RtlGetCurrentProcessorNumber				ntdll		0
imp	'RtlGetCurrentProcessorNumberEx'			RtlGetCurrentProcessorNumberEx				ntdll		0
imp	'RtlGetCurrentServiceSessionId'				RtlGetCurrentServiceSessionId				ntdll		0
imp	'RtlGetCurrentTransaction'				RtlGetCurrentTransaction				ntdll		0
imp	'RtlGetCurrentUmsThread'				RtlGetCurrentUmsThread					ntdll		0
imp	'RtlGetDaclSecurityDescriptor'				RtlGetDaclSecurityDescriptor				ntdll		0
imp	'RtlGetDeviceFamilyInfoEnum'				RtlGetDeviceFamilyInfoEnum				ntdll		0
imp	'RtlGetElementGenericTable'				RtlGetElementGenericTable				ntdll		0
imp	'RtlGetElementGenericTableAvl'				RtlGetElementGenericTableAvl				ntdll		0
imp	'RtlGetEnabledExtendedFeatures'				RtlGetEnabledExtendedFeatures				ntdll		0
imp	'RtlGetExePath'						RtlGetExePath						ntdll		0
imp	'RtlGetExtendedContextLength'				RtlGetExtendedContextLength				ntdll		0
imp	'RtlGetExtendedFeaturesMask'				RtlGetExtendedFeaturesMask				ntdll		0
imp	'RtlGetFileMUIPath'					RtlGetFileMUIPath					ntdll		0
imp	'RtlGetFrame'						RtlGetFrame						ntdll		0
imp	'RtlGetFullPathName_U'					RtlGetFullPathName_U					ntdll		0
imp	'RtlGetFullPathName_UEx'				RtlGetFullPathName_UEx					ntdll		0
imp	'RtlGetFullPathName_UstrEx'				RtlGetFullPathName_UstrEx				ntdll		0
imp	'RtlGetFunctionTableListHead'				RtlGetFunctionTableListHead				ntdll		0
imp	'RtlGetGroupSecurityDescriptor'				RtlGetGroupSecurityDescriptor				ntdll		0
imp	'RtlGetIntegerAtom'					RtlGetIntegerAtom					ntdll		0
imp	'RtlGetInterruptTimePrecise'				RtlGetInterruptTimePrecise				ntdll		0
imp	'RtlGetLastNtStatus'					RtlGetLastNtStatus					ntdll		0
imp	'RtlGetLastWin32Error'					RtlGetLastWin32Error					ntdll		0
imp	'RtlGetLengthWithoutLastFullDosOrNtPathElement'		RtlGetLengthWithoutLastFullDosOrNtPathElement		ntdll		0
imp	'RtlGetLengthWithoutTrailingPathSeperators'		RtlGetLengthWithoutTrailingPathSeperators		ntdll		0
imp	'RtlGetLocaleFileMappingAddress'			RtlGetLocaleFileMappingAddress				ntdll		0
imp	'RtlGetLongestNtPathLength'				RtlGetLongestNtPathLength				ntdll		0
imp	'RtlGetNativeSystemInformation'				RtlGetNativeSystemInformation				ntdll		0
imp	'RtlGetNextEntryHashTable'				RtlGetNextEntryHashTable				ntdll		0
imp	'RtlGetNextUmsListItem'					RtlGetNextUmsListItem					ntdll		0
imp	'RtlGetNonVolatileToken'				RtlGetNonVolatileToken					ntdll		0
imp	'RtlGetNtGlobalFlags'					RtlGetNtGlobalFlags					ntdll		0
imp	'RtlGetNtProductType'					RtlGetNtProductType					ntdll		0
imp	'RtlGetNtSystemRoot'					RtlGetNtSystemRoot					ntdll		0
imp	'RtlGetNtVersionNumbers'				RtlGetNtVersionNumbers					ntdll		0
imp	'RtlGetOwnerSecurityDescriptor'				RtlGetOwnerSecurityDescriptor				ntdll		0
imp	'RtlGetParentLocaleName'				RtlGetParentLocaleName					ntdll		0
imp	'RtlGetPersistedStateLocation'				RtlGetPersistedStateLocation				ntdll		0
imp	'RtlGetProcessHeaps'					RtlGetProcessHeaps					ntdll		0	2
imp	'RtlGetProcessPreferredUILanguages'			RtlGetProcessPreferredUILanguages			ntdll		0
imp	'RtlGetProductInfo'					RtlGetProductInfo					ntdll		0
imp	'RtlGetSaclSecurityDescriptor'				RtlGetSaclSecurityDescriptor				ntdll		0
imp	'RtlGetSearchPath'					RtlGetSearchPath					ntdll		0
imp	'RtlGetSecurityDescriptorRMControl'			RtlGetSecurityDescriptorRMControl			ntdll		0
imp	'RtlGetSessionProperties'				RtlGetSessionProperties					ntdll		0
imp	'RtlGetSetBootStatusData'				RtlGetSetBootStatusData					ntdll		0
imp	'RtlGetSuiteMask'					RtlGetSuiteMask						ntdll		0
imp	'RtlGetSystemBootStatus'				RtlGetSystemBootStatus					ntdll		0
imp	'RtlGetSystemBootStatusEx'				RtlGetSystemBootStatusEx				ntdll		0
imp	'RtlGetSystemPreferredUILanguages'			RtlGetSystemPreferredUILanguages			ntdll		0
imp	'RtlGetSystemTimePrecise'				RtlGetSystemTimePrecise					ntdll		0
imp	'RtlGetThreadErrorMode'					RtlGetThreadErrorMode					ntdll		0
imp	'RtlGetThreadLangIdByIndex'				RtlGetThreadLangIdByIndex				ntdll		0
imp	'RtlGetThreadPreferredUILanguages'			RtlGetThreadPreferredUILanguages			ntdll		0
imp	'RtlGetThreadWorkOnBehalfTicket'			RtlGetThreadWorkOnBehalfTicket				ntdll		0
imp	'RtlGetTokenNamedObjectPath'				RtlGetTokenNamedObjectPath				ntdll		0
imp	'RtlGetUILanguageInfo'					RtlGetUILanguageInfo					ntdll		0
imp	'RtlGetUmsCompletionListEvent'				RtlGetUmsCompletionListEvent				ntdll		0
imp	'RtlGetUnloadEventTrace'				RtlGetUnloadEventTrace					ntdll		0
imp	'RtlGetUnloadEventTraceEx'				RtlGetUnloadEventTraceEx				ntdll		0
imp	'RtlGetUserInfoHeap'					RtlGetUserInfoHeap					ntdll		0
imp	'RtlGetUserPreferredUILanguages'			RtlGetUserPreferredUILanguages				ntdll		0
imp	'RtlGetVersion'						RtlGetVersion						ntdll		0
imp	'RtlGrowFunctionTable'					RtlGrowFunctionTable					ntdll		0
imp	'RtlGuardCheckLongJumpTarget'				RtlGuardCheckLongJumpTarget				ntdll		0
imp	'RtlHashUnicodeString'					RtlHashUnicodeString					ntdll		0
imp	'RtlHeapTrkInitialize'					RtlHeapTrkInitialize					ntdll		0
imp	'RtlIdentifierAuthoritySid'				RtlIdentifierAuthoritySid				ntdll		0
imp	'RtlIdnToAscii'						RtlIdnToAscii						ntdll		0
imp	'RtlIdnToNameprepUnicode'				RtlIdnToNameprepUnicode					ntdll		0
imp	'RtlIdnToUnicode'					RtlIdnToUnicode						ntdll		0
imp	'RtlImageDirectoryEntryToData'				RtlImageDirectoryEntryToData				ntdll		0
imp	'RtlImageNtHeader'					RtlImageNtHeader					ntdll		0
imp	'RtlImageNtHeaderEx'					RtlImageNtHeaderEx					ntdll		0
imp	'RtlImageRvaToSection'					RtlImageRvaToSection					ntdll		0
imp	'RtlImageRvaToVa'					RtlImageRvaToVa						ntdll		0
imp	'RtlImpersonateSelf'					RtlImpersonateSelf					ntdll		0
imp	'RtlImpersonateSelfEx'					RtlImpersonateSelfEx					ntdll		0
imp	'RtlIncrementCorrelationVector'				RtlIncrementCorrelationVector				ntdll		0
imp	'RtlInitAnsiString'					RtlInitAnsiString					ntdll		0
imp	'RtlInitAnsiStringEx'					RtlInitAnsiStringEx					ntdll		0
imp	'RtlInitBarrier'					RtlInitBarrier						ntdll		0
imp	'RtlInitCodePageTable'					RtlInitCodePageTable					ntdll		0
imp	'RtlInitEnumerationHashTable'				RtlInitEnumerationHashTable				ntdll		0
imp	'RtlInitMemoryStream'					RtlInitMemoryStream					ntdll		0
imp	'RtlInitNlsTables'					RtlInitNlsTables					ntdll		0
imp	'RtlInitOutOfProcessMemoryStream'			RtlInitOutOfProcessMemoryStream				ntdll		0
imp	'RtlInitString'						RtlInitString						ntdll		0
imp	'RtlInitStringEx'					RtlInitStringEx						ntdll		0
imp	'RtlInitStrongEnumerationHashTable'			RtlInitStrongEnumerationHashTable			ntdll		0
imp	'RtlInitUnicodeString'					RtlInitUnicodeString					ntdll		0	2
imp	'RtlInitUnicodeStringEx'				RtlInitUnicodeStringEx					ntdll		0
imp	'RtlInitWeakEnumerationHashTable'			RtlInitWeakEnumerationHashTable				ntdll		0
imp	'RtlInitializeAtomPackage'				RtlInitializeAtomPackage				ntdll		0
imp	'RtlInitializeBitMap'					RtlInitializeBitMap					ntdll		0
imp	'RtlInitializeBitMapEx'					RtlInitializeBitMapEx					ntdll		0
imp	'RtlInitializeConditionVariable'			RtlInitializeConditionVariable				ntdll		0
imp	'RtlInitializeContext'					RtlInitializeContext					ntdll		0
imp	'RtlInitializeCorrelationVector'			RtlInitializeCorrelationVector				ntdll		0
imp	'RtlInitializeCriticalSection'				RtlInitializeCriticalSection				ntdll		0	1
imp	'RtlInitializeCriticalSectionAndSpinCount'		RtlInitializeCriticalSectionAndSpinCount		ntdll		0
imp	'RtlInitializeCriticalSectionEx'			RtlInitializeCriticalSectionEx				ntdll		0
imp	'RtlInitializeExtendedContext'				RtlInitializeExtendedContext				ntdll		0
imp	'RtlInitializeGenericTable'				RtlInitializeGenericTable				ntdll		0
imp	'RtlInitializeGenericTableAvl'				RtlInitializeGenericTableAvl				ntdll		0
imp	'RtlInitializeHandleTable'				RtlInitializeHandleTable				ntdll		0
imp	'RtlInitializeNtUserPfn'				RtlInitializeNtUserPfn					ntdll		0
imp	'RtlInitializeRXact'					RtlInitializeRXact					ntdll		0
imp	'RtlInitializeResource'					RtlInitializeResource					ntdll		0
imp	'RtlInitializeSListHead'				RtlInitializeSListHead					ntdll		0
imp	'RtlInitializeSRWLock'					RtlInitializeSRWLock					ntdll		0
imp	'RtlInitializeSid'					RtlInitializeSid					ntdll		0
imp	'RtlInitializeSidEx'					RtlInitializeSidEx					ntdll		0
imp	'RtlInsertElementGenericTable'				RtlInsertElementGenericTable				ntdll		0
imp	'RtlInsertElementGenericTableAvl'			RtlInsertElementGenericTableAvl				ntdll		0
imp	'RtlInsertElementGenericTableFull'			RtlInsertElementGenericTableFull			ntdll		0
imp	'RtlInsertElementGenericTableFullAvl'			RtlInsertElementGenericTableFullAvl			ntdll		0
imp	'RtlInsertEntryHashTable'				RtlInsertEntryHashTable					ntdll		0
imp	'RtlInstallFunctionTableCallback'			RtlInstallFunctionTableCallback				ntdll		0
imp	'RtlInt64ToUnicodeString'				RtlInt64ToUnicodeString					ntdll		0
imp	'RtlIntegerToChar'					RtlIntegerToChar					ntdll		0
imp	'RtlIntegerToUnicodeString'				RtlIntegerToUnicodeString				ntdll		0
imp	'RtlInterlockedClearBitRun'				RtlInterlockedClearBitRun				ntdll		0
imp	'RtlInterlockedFlushSList'				RtlInterlockedFlushSList				ntdll		0
imp	'RtlInterlockedPopEntrySList'				RtlInterlockedPopEntrySList				ntdll		0
imp	'RtlInterlockedPushEntrySList'				RtlInterlockedPushEntrySList				ntdll		0
imp	'RtlInterlockedPushListSList'				RtlInterlockedPushListSList				ntdll		0
imp	'RtlInterlockedPushListSListEx'				RtlInterlockedPushListSListEx				ntdll		0
imp	'RtlInterlockedSetBitRun'				RtlInterlockedSetBitRun					ntdll		0
imp	'RtlIoDecodeMemIoResource'				RtlIoDecodeMemIoResource				ntdll		0
imp	'RtlIoEncodeMemIoResource'				RtlIoEncodeMemIoResource				ntdll		0
imp	'RtlIpv4AddressToString'				RtlIpv4AddressToStringW					ntdll		0
imp	'RtlIpv4AddressToStringEx'				RtlIpv4AddressToStringExW				ntdll		0
imp	'RtlIpv4StringToAddress'				RtlIpv4StringToAddressW					ntdll		0
imp	'RtlIpv4StringToAddressEx'				RtlIpv4StringToAddressExW				ntdll		0
imp	'RtlIpv6AddressToString'				RtlIpv6AddressToStringW					ntdll		0
imp	'RtlIpv6AddressToStringEx'				RtlIpv6AddressToStringExW				ntdll		0
imp	'RtlIpv6StringToAddress'				RtlIpv6StringToAddressW					ntdll		0
imp	'RtlIpv6StringToAddressEx'				RtlIpv6StringToAddressExW				ntdll		0
imp	'RtlIsActivationContextActive'				RtlIsActivationContextActive				ntdll		0
imp	'RtlIsCapabilitySid'					RtlIsCapabilitySid					ntdll		0
imp	'RtlIsCloudFilesPlaceholder'				RtlIsCloudFilesPlaceholder				ntdll		0
imp	'RtlIsCriticalSectionLocked'				RtlIsCriticalSectionLocked				ntdll		0
imp	'RtlIsCriticalSectionLockedByThread'			RtlIsCriticalSectionLockedByThread			ntdll		0
imp	'RtlIsCurrentProcess'					RtlIsCurrentProcess					ntdll		0
imp	'RtlIsCurrentThread'					RtlIsCurrentThread					ntdll		0
imp	'RtlIsCurrentThreadAttachExempt'			RtlIsCurrentThreadAttachExempt				ntdll		0
imp	'RtlIsDosDeviceName_U'					RtlIsDosDeviceName_U					ntdll		0
imp	'RtlIsElevatedRid'					RtlIsElevatedRid					ntdll		0
imp	'RtlIsGenericTableEmpty'				RtlIsGenericTableEmpty					ntdll		0
imp	'RtlIsGenericTableEmptyAvl'				RtlIsGenericTableEmptyAvl				ntdll		0
imp	'RtlIsMultiSessionSku'					RtlIsMultiSessionSku					ntdll		0
imp	'RtlIsMultiUsersInSessionSku'				RtlIsMultiUsersInSessionSku				ntdll		0
imp	'RtlIsNameInExpression'					RtlIsNameInExpression					ntdll		0
imp	'RtlIsNameInUnUpcasedExpression'			RtlIsNameInUnUpcasedExpression				ntdll		0
imp	'RtlIsNameLegalDOS8Dot3'				RtlIsNameLegalDOS8Dot3					ntdll		0
imp	'RtlIsNonEmptyDirectoryReparsePointAllowed'		RtlIsNonEmptyDirectoryReparsePointAllowed		ntdll		0
imp	'RtlIsNormalizedString'					RtlIsNormalizedString					ntdll		0
imp	'RtlIsPackageSid'					RtlIsPackageSid						ntdll		0
imp	'RtlIsParentOfChildAppContainer'			RtlIsParentOfChildAppContainer				ntdll		0
imp	'RtlIsPartialPlaceholder'				RtlIsPartialPlaceholder					ntdll		0
imp	'RtlIsPartialPlaceholderFileHandle'			RtlIsPartialPlaceholderFileHandle			ntdll		0
imp	'RtlIsPartialPlaceholderFileInfo'			RtlIsPartialPlaceholderFileInfo				ntdll		0
imp	'RtlIsProcessorFeaturePresent'				RtlIsProcessorFeaturePresent				ntdll		0
imp	'RtlIsStateSeparationEnabled'				RtlIsStateSeparationEnabled				ntdll		0
imp	'RtlIsTextUnicode'					RtlIsTextUnicode					ntdll		0
imp	'RtlIsThreadWithinLoaderCallout'			RtlIsThreadWithinLoaderCallout				ntdll		0
imp	'RtlIsUntrustedObject'					RtlIsUntrustedObject					ntdll		0
imp	'RtlIsValidHandle'					RtlIsValidHandle					ntdll		0
imp	'RtlIsValidIndexHandle'					RtlIsValidIndexHandle					ntdll		0
imp	'RtlIsValidLocaleName'					RtlIsValidLocaleName					ntdll		0
imp	'RtlIsValidProcessTrustLabelSid'			RtlIsValidProcessTrustLabelSid				ntdll		0
imp	'RtlKnownExceptionFilter'				RtlKnownExceptionFilter					ntdll		0
imp	'RtlLCIDToCultureName'					RtlLCIDToCultureName					ntdll		0
imp	'RtlLargeIntegerToChar'					RtlLargeIntegerToChar					ntdll		0
imp	'RtlLcidToLocaleName'					RtlLcidToLocaleName					ntdll		0
imp	'RtlLeaveCriticalSection'				RtlLeaveCriticalSection					ntdll		0	1
imp	'RtlLengthRequiredSid'					RtlLengthRequiredSid					ntdll		0
imp	'RtlLengthSecurityDescriptor'				RtlLengthSecurityDescriptor				ntdll		0
imp	'RtlLengthSid'						RtlLengthSid						ntdll		0
imp	'RtlLengthSidAsUnicodeString'				RtlLengthSidAsUnicodeString				ntdll		0
imp	'RtlLoadString'						RtlLoadString						ntdll		0
imp	'RtlLocalTimeToSystemTime'				RtlLocalTimeToSystemTime				ntdll		0
imp	'RtlLocaleNameToLcid'					RtlLocaleNameToLcid					ntdll		0
imp	'RtlLocateExtendedFeature'				RtlLocateExtendedFeature				ntdll		0
imp	'RtlLocateExtendedFeature2'				RtlLocateExtendedFeature2				ntdll		0
imp	'RtlLocateLegacyContext'				RtlLocateLegacyContext					ntdll		0
imp	'RtlLockBootStatusData'					RtlLockBootStatusData					ntdll		0
imp	'RtlLockCurrentThread'					RtlLockCurrentThread					ntdll		0
imp	'RtlLockHeap'						RtlLockHeap						ntdll		0	1
imp	'RtlLockMemoryBlockLookaside'				RtlLockMemoryBlockLookaside				ntdll		0
imp	'RtlLockMemoryStreamRegion'				RtlLockMemoryStreamRegion				ntdll		0
imp	'RtlLockMemoryZone'					RtlLockMemoryZone					ntdll		0
imp	'RtlLockModuleSection'					RtlLockModuleSection					ntdll		0
imp	'RtlLogStackBackTrace'					RtlLogStackBackTrace					ntdll		0
imp	'RtlLookupAtomInAtomTable'				RtlLookupAtomInAtomTable				ntdll		0
imp	'RtlLookupElementGenericTable'				RtlLookupElementGenericTable				ntdll		0
imp	'RtlLookupElementGenericTableAvl'			RtlLookupElementGenericTableAvl				ntdll		0
imp	'RtlLookupElementGenericTableFull'			RtlLookupElementGenericTableFull			ntdll		0
imp	'RtlLookupElementGenericTableFullAvl'			RtlLookupElementGenericTableFullAvl			ntdll		0
imp	'RtlLookupEntryHashTable'				RtlLookupEntryHashTable					ntdll		0
imp	'RtlLookupFirstMatchingElementGenericTableAvl'		RtlLookupFirstMatchingElementGenericTableAvl		ntdll		0
imp	'RtlLookupFunctionEntry'				RtlLookupFunctionEntry					ntdll		0
imp	'RtlLookupFunctionTable'				RtlLookupFunctionTable					ntdll		0
imp	'RtlMakeSelfRelativeSD'					RtlMakeSelfRelativeSD					ntdll		0
imp	'RtlMapGenericMask'					RtlMapGenericMask					ntdll		0
imp	'RtlMapSecurityErrorToNtStatus'				RtlMapSecurityErrorToNtStatus				ntdll		0
imp	'RtlMoveMemory'						RtlMoveMemory						ntdll		0
imp	'RtlMultiAppendUnicodeStringBuffer'			RtlMultiAppendUnicodeStringBuffer			ntdll		0
imp	'RtlMultiByteToUnicodeN'				RtlMultiByteToUnicodeN					ntdll		0
imp	'RtlMultiByteToUnicodeSize'				RtlMultiByteToUnicodeSize				ntdll		0
imp	'RtlMultipleAllocateHeap'				RtlMultipleAllocateHeap					ntdll		0
imp	'RtlMultipleFreeHeap'					RtlMultipleFreeHeap					ntdll		0
imp	'RtlNewInstanceSecurityObject'				RtlNewInstanceSecurityObject				ntdll		0
imp	'RtlNewSecurityGrantedAccess'				RtlNewSecurityGrantedAccess				ntdll		0
imp	'RtlNewSecurityObject'					RtlNewSecurityObject					ntdll		0
imp	'RtlNewSecurityObjectEx'				RtlNewSecurityObjectEx					ntdll		0
imp	'RtlNewSecurityObjectWithMultipleInheritance'		RtlNewSecurityObjectWithMultipleInheritance		ntdll		0
imp	'RtlNormalizeProcessParams'				RtlNormalizeProcessParams				ntdll		0
imp	'RtlNormalizeString'					RtlNormalizeString					ntdll		0
imp	'RtlNtPathNameToDosPathName'				RtlNtPathNameToDosPathName				ntdll		0
imp	'RtlNtStatusToDosError'					RtlNtStatusToDosError					ntdll		0	1
imp	'RtlNtStatusToDosErrorNoTeb'				RtlNtStatusToDosErrorNoTeb				ntdll		0
imp	'RtlNtdllName'						RtlNtdllName						ntdll		0
imp	'RtlNumberGenericTableElements'				RtlNumberGenericTableElements				ntdll		0
imp	'RtlNumberGenericTableElementsAvl'			RtlNumberGenericTableElementsAvl			ntdll		0
imp	'RtlNumberOfClearBits'					RtlNumberOfClearBits					ntdll		0
imp	'RtlNumberOfClearBitsInRange'				RtlNumberOfClearBitsInRange				ntdll		0
imp	'RtlNumberOfSetBits'					RtlNumberOfSetBits					ntdll		0
imp	'RtlNumberOfSetBitsInRange'				RtlNumberOfSetBitsInRange				ntdll		0
imp	'RtlNumberOfSetBitsUlongPtr'				RtlNumberOfSetBitsUlongPtr				ntdll		0
imp	'RtlOemStringToUnicodeSize'				RtlOemStringToUnicodeSize				ntdll		0
imp	'RtlOemStringToUnicodeString'				RtlOemStringToUnicodeString				ntdll		0
imp	'RtlOemToUnicodeN'					RtlOemToUnicodeN					ntdll		0
imp	'RtlOpenCurrentUser'					RtlOpenCurrentUser					ntdll		0
imp	'RtlOsDeploymentState'					RtlOsDeploymentState					ntdll		0
imp	'RtlOwnerAcesPresent'					RtlOwnerAcesPresent					ntdll		0
imp	'RtlPcToFileHeader'					RtlPcToFileHeader					ntdll		0
imp	'RtlPinAtomInAtomTable'					RtlPinAtomInAtomTable					ntdll		0
imp	'RtlPopFrame'						RtlPopFrame						ntdll		0
imp	'RtlPrefixString'					RtlPrefixString						ntdll		0
imp	'RtlPrefixUnicodeString'				RtlPrefixUnicodeString					ntdll		0
imp	'RtlPrepareForProcessCloning'				RtlPrepareForProcessCloning				ntdll		0
imp	'RtlProcessFlsData'					RtlProcessFlsData					ntdll		0
imp	'RtlProtectHeap'					RtlProtectHeap						ntdll		0
imp	'RtlPublishWnfStateData'				RtlPublishWnfStateData					ntdll		0
imp	'RtlPushFrame'						RtlPushFrame						ntdll		0
imp	'RtlQueryActivationContextApplicationSettings'		RtlQueryActivationContextApplicationSettings		ntdll		0
imp	'RtlQueryAtomInAtomTable'				RtlQueryAtomInAtomTable					ntdll		0
imp	'RtlQueryCriticalSectionOwner'				RtlQueryCriticalSectionOwner				ntdll		0
imp	'RtlQueryDepthSList'					RtlQueryDepthSList					ntdll		0
imp	'RtlQueryDynamicTimeZoneInformation'			RtlQueryDynamicTimeZoneInformation			ntdll		0
imp	'RtlQueryElevationFlags'				RtlQueryElevationFlags					ntdll		0
imp	'RtlQueryEnvironmentVariable'				RtlQueryEnvironmentVariable				ntdll		0	3
imp	'RtlQueryEnvironmentVariable_U'				RtlQueryEnvironmentVariable_U				ntdll		0
imp	'RtlQueryHeapInformation'				RtlQueryHeapInformation					ntdll		0
imp	'RtlQueryImageMitigationPolicy'				RtlQueryImageMitigationPolicy				ntdll		0
imp	'RtlQueryInformationAcl'				RtlQueryInformationAcl					ntdll		0
imp	'RtlQueryInformationActivationContext'			RtlQueryInformationActivationContext			ntdll		0
imp	'RtlQueryInformationActiveActivationContext'		RtlQueryInformationActiveActivationContext		ntdll		0
imp	'RtlQueryInterfaceMemoryStream'				RtlQueryInterfaceMemoryStream				ntdll		0
imp	'RtlQueryModuleInformation'				RtlQueryModuleInformation				ntdll		0
imp	'RtlQueryPackageClaims'					RtlQueryPackageClaims					ntdll		0
imp	'RtlQueryPackageIdentity'				RtlQueryPackageIdentity					ntdll		0
imp	'RtlQueryPackageIdentityEx'				RtlQueryPackageIdentityEx				ntdll		0
imp	'RtlQueryPerformanceCounter'				RtlQueryPerformanceCounter				ntdll		0
imp	'RtlQueryPerformanceFrequency'				RtlQueryPerformanceFrequency				ntdll		0
imp	'RtlQueryProcessBackTraceInformation'			RtlQueryProcessBackTraceInformation			ntdll		0
imp	'RtlQueryProcessDebugInformation'			RtlQueryProcessDebugInformation				ntdll		0
imp	'RtlQueryProcessHeapInformation'			RtlQueryProcessHeapInformation				ntdll		0
imp	'RtlQueryProcessLockInformation'			RtlQueryProcessLockInformation				ntdll		0
imp	'RtlQueryProcessPlaceholderCompatibilityMode'		RtlQueryProcessPlaceholderCompatibilityMode		ntdll		0
imp	'RtlQueryProtectedPolicy'				RtlQueryProtectedPolicy					ntdll		0
imp	'RtlQueryRegistryValueWithFallback'			RtlQueryRegistryValueWithFallback			ntdll		0
imp	'RtlQueryRegistryValues'				RtlQueryRegistryValues					ntdll		0
imp	'RtlQueryRegistryValuesEx'				RtlQueryRegistryValuesEx				ntdll		0
imp	'RtlQueryResourcePolicy'				RtlQueryResourcePolicy					ntdll		0
imp	'RtlQuerySecurityObject'				RtlQuerySecurityObject					ntdll		0
imp	'RtlQueryTagHeap'					RtlQueryTagHeap						ntdll		0
imp	'RtlQueryThreadPlaceholderCompatibilityMode'		RtlQueryThreadPlaceholderCompatibilityMode		ntdll		0
imp	'RtlQueryThreadProfiling'				RtlQueryThreadProfiling					ntdll		0
imp	'RtlQueryTimeZoneInformation'				RtlQueryTimeZoneInformation				ntdll		0
imp	'RtlQueryTokenHostIdAsUlong64'				RtlQueryTokenHostIdAsUlong64				ntdll		0
imp	'RtlQueryUmsThreadInformation'				RtlQueryUmsThreadInformation				ntdll		0
imp	'RtlQueryUnbiasedInterruptTime'				RtlQueryUnbiasedInterruptTime				ntdll		0
imp	'RtlQueryValidationRunlevel'				RtlQueryValidationRunlevel				ntdll		0
imp	'RtlQueryWnfMetaNotification'				RtlQueryWnfMetaNotification				ntdll		0
imp	'RtlQueryWnfStateData'					RtlQueryWnfStateData					ntdll		0
imp	'RtlQueryWnfStateDataWithExplicitScope'			RtlQueryWnfStateDataWithExplicitScope			ntdll		0
imp	'RtlQueueWorkItem'					RtlQueueWorkItem					ntdll		0
imp	'RtlRaiseCustomSystemEventTrigger'			RtlRaiseCustomSystemEventTrigger			ntdll		0
imp	'RtlRaiseException'					RtlRaiseException					ntdll		0
imp	'RtlRaiseStatus'					RtlRaiseStatus						ntdll		0
imp	'RtlRandom'						RtlRandom						ntdll		0
imp	'RtlRandomEx'						RtlRandomEx						ntdll		0
imp	'RtlRbInsertNodeEx'					RtlRbInsertNodeEx					ntdll		0
imp	'RtlRbRemoveNode'					RtlRbRemoveNode						ntdll		0
imp	'RtlReAllocateHeap'					RtlReAllocateHeap					ntdll		0	4
imp	'RtlReadMemoryStream'					RtlReadMemoryStream					ntdll		0
imp	'RtlReadOutOfProcessMemoryStream'			RtlReadOutOfProcessMemoryStream				ntdll		0
imp	'RtlReadThreadProfilingData'				RtlReadThreadProfilingData				ntdll		0
imp	'RtlRealPredecessor'					RtlRealPredecessor					ntdll		0
imp	'RtlRealSuccessor'					RtlRealSuccessor					ntdll		0
imp	'RtlRegisterForWnfMetaNotification'			RtlRegisterForWnfMetaNotification			ntdll		0
imp	'RtlRegisterSecureMemoryCacheCallback'			RtlRegisterSecureMemoryCacheCallback			ntdll		0
imp	'RtlRegisterThreadWithCsrss'				RtlRegisterThreadWithCsrss				ntdll		0
imp	'RtlRegisterWait'					RtlRegisterWait						ntdll		0
imp	'RtlReleaseActivationContext'				RtlReleaseActivationContext				ntdll		0
imp	'RtlReleaseMemoryStream'				RtlReleaseMemoryStream					ntdll		0
imp	'RtlReleasePath'					RtlReleasePath						ntdll		0
imp	'RtlReleasePebLock'					RtlReleasePebLock					ntdll		0
imp	'RtlReleasePrivilege'					RtlReleasePrivilege					ntdll		0
imp	'RtlReleaseRelativeName'				RtlReleaseRelativeName					ntdll		0
imp	'RtlReleaseResource'					RtlReleaseResource					ntdll		0
imp	'RtlReleaseSRWLockExclusive'				RtlReleaseSRWLockExclusive				ntdll		0
imp	'RtlReleaseSRWLockShared'				RtlReleaseSRWLockShared					ntdll		0
imp	'RtlRemoteCall'						RtlRemoteCall						ntdll		0
imp	'RtlRemoveEntryHashTable'				RtlRemoveEntryHashTable					ntdll		0
imp	'RtlRemovePrivileges'					RtlRemovePrivileges					ntdll		0
imp	'RtlRemoveVectoredContinueHandler'			RtlRemoveVectoredContinueHandler			ntdll		0
imp	'RtlRemoveVectoredExceptionHandler'			RtlRemoveVectoredExceptionHandler			ntdll		0
imp	'RtlReplaceSidInSd'					RtlReplaceSidInSd					ntdll		0
imp	'RtlReplaceSystemDirectoryInPath'			RtlReplaceSystemDirectoryInPath				ntdll		0
imp	'RtlReportException'					RtlReportException					ntdll		0
imp	'RtlReportExceptionEx'					RtlReportExceptionEx					ntdll		0
imp	'RtlReportSilentProcessExit'				RtlReportSilentProcessExit				ntdll		0
imp	'RtlReportSqmEscalation'				RtlReportSqmEscalation					ntdll		0
imp	'RtlResetMemoryBlockLookaside'				RtlResetMemoryBlockLookaside				ntdll		0
imp	'RtlResetMemoryZone'					RtlResetMemoryZone					ntdll		0
imp	'RtlResetNtUserPfn'					RtlResetNtUserPfn					ntdll		0
imp	'RtlResetRtlTranslations'				RtlResetRtlTranslations					ntdll		0
imp	'RtlRestoreBootStatusDefaults'				RtlRestoreBootStatusDefaults				ntdll		0
imp	'RtlRestoreContext'					RtlRestoreContext					ntdll		0
imp	'RtlRestoreLastWin32Error'				RtlRestoreLastWin32Error				ntdll		0
imp	'RtlRestoreSystemBootStatusDefaults'			RtlRestoreSystemBootStatusDefaults			ntdll		0
imp	'RtlRetrieveNtUserPfn'					RtlRetrieveNtUserPfn					ntdll		0
imp	'RtlRevertMemoryStream'					RtlRevertMemoryStream					ntdll		0
imp	'RtlRunDecodeUnicodeString'				RtlRunDecodeUnicodeString				ntdll		0
imp	'RtlRunEncodeUnicodeString'				RtlRunEncodeUnicodeString				ntdll		0
imp	'RtlRunOnceBeginInitialize'				RtlRunOnceBeginInitialize				ntdll		0
imp	'RtlRunOnceComplete'					RtlRunOnceComplete					ntdll		0
imp	'RtlRunOnceExecuteOnce'					RtlRunOnceExecuteOnce					ntdll		0
imp	'RtlRunOnceInitialize'					RtlRunOnceInitialize					ntdll		0
imp	'RtlSecondsSince1970ToTime'				RtlSecondsSince1970ToTime				ntdll		0
imp	'RtlSecondsSince1980ToTime'				RtlSecondsSince1980ToTime				ntdll		0
imp	'RtlSeekMemoryStream'					RtlSeekMemoryStream					ntdll		0
imp	'RtlSelfRelativeToAbsoluteSD'				RtlSelfRelativeToAbsoluteSD				ntdll		0
imp	'RtlSelfRelativeToAbsoluteSD2'				RtlSelfRelativeToAbsoluteSD2				ntdll		0
imp	'RtlSendMsgToSm'					RtlSendMsgToSm						ntdll		0
imp	'RtlSetAllBits'						RtlSetAllBits						ntdll		0
imp	'RtlSetAttributesSecurityDescriptor'			RtlSetAttributesSecurityDescriptor			ntdll		0
imp	'RtlSetBit'						RtlSetBit						ntdll		0
imp	'RtlSetBits'						RtlSetBits						ntdll		0
imp	'RtlSetControlSecurityDescriptor'			RtlSetControlSecurityDescriptor				ntdll		0
imp	'RtlSetCriticalSectionSpinCount'			RtlSetCriticalSectionSpinCount				ntdll		0
imp	'RtlSetCurrentDirectory_U'				RtlSetCurrentDirectory_U				ntdll		0
imp	'RtlSetCurrentEnvironment'				RtlSetCurrentEnvironment				ntdll		0
imp	'RtlSetCurrentTransaction'				RtlSetCurrentTransaction				ntdll		0
imp	'RtlSetDaclSecurityDescriptor'				RtlSetDaclSecurityDescriptor				ntdll		0
imp	'RtlSetDynamicTimeZoneInformation'			RtlSetDynamicTimeZoneInformation			ntdll		0
imp	'RtlSetEnvironmentStrings'				RtlSetEnvironmentStrings				ntdll		0
imp	'RtlSetEnvironmentVar'					RtlSetEnvironmentVar					ntdll		0
imp	'RtlSetEnvironmentVariable'				RtlSetEnvironmentVariable				ntdll		0
imp	'RtlSetExtendedFeaturesMask'				RtlSetExtendedFeaturesMask				ntdll		0
imp	'RtlSetGroupSecurityDescriptor'				RtlSetGroupSecurityDescriptor				ntdll		0
imp	'RtlSetHeapInformation'					RtlSetHeapInformation					ntdll		0
imp	'RtlSetImageMitigationPolicy'				RtlSetImageMitigationPolicy				ntdll		0
imp	'RtlSetInformationAcl'					RtlSetInformationAcl					ntdll		0
imp	'RtlSetIoCompletionCallback'				RtlSetIoCompletionCallback				ntdll		0
imp	'RtlSetLastWin32Error'					RtlSetLastWin32Error					ntdll		0
imp	'RtlSetLastWin32ErrorAndNtStatusFromNtStatus'		RtlSetLastWin32ErrorAndNtStatusFromNtStatus		ntdll		0
imp	'RtlSetMemoryStreamSize'				RtlSetMemoryStreamSize					ntdll		0
imp	'RtlSetOwnerSecurityDescriptor'				RtlSetOwnerSecurityDescriptor				ntdll		0
imp	'RtlSetPortableOperatingSystem'				RtlSetPortableOperatingSystem				ntdll		0
imp	'RtlSetProcessDebugInformation'				RtlSetProcessDebugInformation				ntdll		0
imp	'RtlSetProcessIsCritical'				RtlSetProcessIsCritical					ntdll		0
imp	'RtlSetProcessPlaceholderCompatibilityMode'		RtlSetProcessPlaceholderCompatibilityMode		ntdll		0
imp	'RtlSetProcessPreferredUILanguages'			RtlSetProcessPreferredUILanguages			ntdll		0
imp	'RtlSetProtectedPolicy'					RtlSetProtectedPolicy					ntdll		0
imp	'RtlSetProxiedProcessId'				RtlSetProxiedProcessId					ntdll		0
imp	'RtlSetSaclSecurityDescriptor'				RtlSetSaclSecurityDescriptor				ntdll		0
imp	'RtlSetSearchPathMode'					RtlSetSearchPathMode					ntdll		0
imp	'RtlSetSecurityDescriptorRMControl'			RtlSetSecurityDescriptorRMControl			ntdll		0
imp	'RtlSetSecurityObject'					RtlSetSecurityObject					ntdll		0
imp	'RtlSetSecurityObjectEx'				RtlSetSecurityObjectEx					ntdll		0
imp	'RtlSetSystemBootStatus'				RtlSetSystemBootStatus					ntdll		0
imp	'RtlSetSystemBootStatusEx'				RtlSetSystemBootStatusEx				ntdll		0
imp	'RtlSetThreadErrorMode'					RtlSetThreadErrorMode					ntdll		0
imp	'RtlSetThreadIsCritical'				RtlSetThreadIsCritical					ntdll		0
imp	'RtlSetThreadPlaceholderCompatibilityMode'		RtlSetThreadPlaceholderCompatibilityMode		ntdll		0
imp	'RtlSetThreadPoolStartFunc'				RtlSetThreadPoolStartFunc				ntdll		0
imp	'RtlSetThreadPreferredUILanguages'			RtlSetThreadPreferredUILanguages			ntdll		0
imp	'RtlSetThreadSubProcessTag'				RtlSetThreadSubProcessTag				ntdll		0
imp	'RtlSetThreadWorkOnBehalfTicket'			RtlSetThreadWorkOnBehalfTicket				ntdll		0
imp	'RtlSetTimeZoneInformation'				RtlSetTimeZoneInformation				ntdll		0
imp	'RtlSetTimer'						RtlSetTimer						ntdll		0
imp	'RtlSetUmsThreadInformation'				RtlSetUmsThreadInformation				ntdll		0
imp	'RtlSetUnhandledExceptionFilter'			RtlSetUnhandledExceptionFilter				ntdll		0
imp	'RtlSetUserFlagsHeap'					RtlSetUserFlagsHeap					ntdll		0
imp	'RtlSetUserValueHeap'					RtlSetUserValueHeap					ntdll		0
imp	'RtlSidDominates'					RtlSidDominates						ntdll		0
imp	'RtlSidDominatesForTrust'				RtlSidDominatesForTrust					ntdll		0
imp	'RtlSidEqualLevel'					RtlSidEqualLevel					ntdll		0
imp	'RtlSidHashInitialize'					RtlSidHashInitialize					ntdll		0
imp	'RtlSidHashLookup'					RtlSidHashLookup					ntdll		0
imp	'RtlSidIsHigherLevel'					RtlSidIsHigherLevel					ntdll		0
imp	'RtlSizeHeap'						RtlSizeHeap						ntdll		0	3
imp	'RtlSleepConditionVariableCS'				RtlSleepConditionVariableCS				ntdll		0
imp	'RtlSleepConditionVariableSRW'				RtlSleepConditionVariableSRW				ntdll		0
imp	'RtlSplay'						RtlSplay						ntdll		0
imp	'RtlStartRXact'						RtlStartRXact						ntdll		0
imp	'RtlStatMemoryStream'					RtlStatMemoryStream					ntdll		0
imp	'RtlStringFromGUID'					RtlStringFromGUID					ntdll		0
imp	'RtlStringFromGUIDEx'					RtlStringFromGUIDEx					ntdll		0
imp	'RtlStronglyEnumerateEntryHashTable'			RtlStronglyEnumerateEntryHashTable			ntdll		0
imp	'RtlSubAuthorityCountSid'				RtlSubAuthorityCountSid					ntdll		0
imp	'RtlSubAuthoritySid'					RtlSubAuthoritySid					ntdll		0
imp	'RtlSubscribeWnfStateChangeNotification'		RtlSubscribeWnfStateChangeNotification			ntdll		0
imp	'RtlSubtreePredecessor'					RtlSubtreePredecessor					ntdll		0
imp	'RtlSubtreeSuccessor'					RtlSubtreeSuccessor					ntdll		0
imp	'RtlSwitchedVVI'					RtlSwitchedVVI						ntdll		0
imp	'RtlSystemTimeToLocalTime'				RtlSystemTimeToLocalTime				ntdll		0
imp	'RtlTestAndPublishWnfStateData'				RtlTestAndPublishWnfStateData				ntdll		0
imp	'RtlTestBit'						RtlTestBit						ntdll		0
imp	'RtlTestBitEx'						RtlTestBitEx						ntdll		0
imp	'RtlTestProtectedAccess'				RtlTestProtectedAccess					ntdll		0
imp	'RtlTimeFieldsToTime'					RtlTimeFieldsToTime					ntdll		0
imp	'RtlTimeToElapsedTimeFields'				RtlTimeToElapsedTimeFields				ntdll		0
imp	'RtlTimeToSecondsSince1970'				RtlTimeToSecondsSince1970				ntdll		0
imp	'RtlTimeToSecondsSince1980'				RtlTimeToSecondsSince1980				ntdll		0
imp	'RtlTimeToTimeFields'					RtlTimeToTimeFields					ntdll		0
imp	'RtlTraceDatabaseAdd'					RtlTraceDatabaseAdd					ntdll		0
imp	'RtlTraceDatabaseCreate'				RtlTraceDatabaseCreate					ntdll		0
imp	'RtlTraceDatabaseDestroy'				RtlTraceDatabaseDestroy					ntdll		0
imp	'RtlTraceDatabaseEnumerate'				RtlTraceDatabaseEnumerate				ntdll		0
imp	'RtlTraceDatabaseFind'					RtlTraceDatabaseFind					ntdll		0
imp	'RtlTraceDatabaseLock'					RtlTraceDatabaseLock					ntdll		0
imp	'RtlTraceDatabaseUnlock'				RtlTraceDatabaseUnlock					ntdll		0
imp	'RtlTraceDatabaseValidate'				RtlTraceDatabaseValidate				ntdll		0
imp	'RtlTryAcquirePebLock'					RtlTryAcquirePebLock					ntdll		0
imp	'RtlTryAcquireSRWLockExclusive'				RtlTryAcquireSRWLockExclusive				ntdll		0
imp	'RtlTryAcquireSRWLockShared'				RtlTryAcquireSRWLockShared				ntdll		0
imp	'RtlTryConvertSRWLockSharedToExclusiveOrRelease'	RtlTryConvertSRWLockSharedToExclusiveOrRelease		ntdll		0
imp	'RtlTryEnterCriticalSection'				RtlTryEnterCriticalSection				ntdll		0	1
imp	'RtlUTF8ToUnicodeN'					RtlUTF8ToUnicodeN					ntdll		0
imp	'RtlUmsThreadYield'					RtlUmsThreadYield					ntdll		0
imp	'RtlUnhandledExceptionFilter'				RtlUnhandledExceptionFilter				ntdll		0
imp	'RtlUnhandledExceptionFilter2'				RtlUnhandledExceptionFilter2				ntdll		0
imp	'RtlUnicodeStringToAnsiSize'				RtlUnicodeStringToAnsiSize				ntdll		0
imp	'RtlUnicodeStringToAnsiString'				RtlUnicodeStringToAnsiString				ntdll		0
imp	'RtlUnicodeStringToCountedOemString'			RtlUnicodeStringToCountedOemString			ntdll		0
imp	'RtlUnicodeStringToInteger'				RtlUnicodeStringToInteger				ntdll		0
imp	'RtlUnicodeStringToOemSize'				RtlUnicodeStringToOemSize				ntdll		0
imp	'RtlUnicodeStringToOemString'				RtlUnicodeStringToOemString				ntdll		0
imp	'RtlUnicodeToCustomCPN'					RtlUnicodeToCustomCPN					ntdll		0
imp	'RtlUnicodeToMultiByteN'				RtlUnicodeToMultiByteN					ntdll		0
imp	'RtlUnicodeToMultiByteSize'				RtlUnicodeToMultiByteSize				ntdll		0
imp	'RtlUnicodeToOemN'					RtlUnicodeToOemN					ntdll		0
imp	'RtlUnicodeToUTF8N'					RtlUnicodeToUTF8N					ntdll		0
imp	'RtlUniform'						RtlUniform						ntdll		0
imp	'RtlUnlockBootStatusData'				RtlUnlockBootStatusData					ntdll		0
imp	'RtlUnlockCurrentThread'				RtlUnlockCurrentThread					ntdll		0
imp	'RtlUnlockHeap'						RtlUnlockHeap						ntdll		0	1
imp	'RtlUnlockMemoryBlockLookaside'				RtlUnlockMemoryBlockLookaside				ntdll		0
imp	'RtlUnlockMemoryStreamRegion'				RtlUnlockMemoryStreamRegion				ntdll		0
imp	'RtlUnlockMemoryZone'					RtlUnlockMemoryZone					ntdll		0
imp	'RtlUnlockModuleSection'				RtlUnlockModuleSection					ntdll		0
imp	'RtlUnsubscribeWnfNotificationWaitForCompletion'	RtlUnsubscribeWnfNotificationWaitForCompletion		ntdll		0
imp	'RtlUnsubscribeWnfNotificationWithCompletionCallback'	RtlUnsubscribeWnfNotificationWithCompletionCallback	ntdll		0
imp	'RtlUnsubscribeWnfStateChangeNotification'		RtlUnsubscribeWnfStateChangeNotification		ntdll		0
imp	'RtlUnwind'						RtlUnwind						ntdll		0
imp	'RtlUnwindEx'						RtlUnwindEx						ntdll		0
imp	'RtlUpcaseUnicodeChar'					RtlUpcaseUnicodeChar					ntdll		0
imp	'RtlUpcaseUnicodeString'				RtlUpcaseUnicodeString					ntdll		0
imp	'RtlUpcaseUnicodeStringToAnsiString'			RtlUpcaseUnicodeStringToAnsiString			ntdll		0
imp	'RtlUpcaseUnicodeStringToCountedOemString'		RtlUpcaseUnicodeStringToCountedOemString		ntdll		0
imp	'RtlUpcaseUnicodeStringToOemString'			RtlUpcaseUnicodeStringToOemString			ntdll		0
imp	'RtlUpcaseUnicodeToCustomCPN'				RtlUpcaseUnicodeToCustomCPN				ntdll		0
imp	'RtlUpcaseUnicodeToMultiByteN'				RtlUpcaseUnicodeToMultiByteN				ntdll		0
imp	'RtlUpcaseUnicodeToOemN'				RtlUpcaseUnicodeToOemN					ntdll		0
imp	'RtlUpdateClonedCriticalSection'			RtlUpdateClonedCriticalSection				ntdll		0
imp	'RtlUpdateClonedSRWLock'				RtlUpdateClonedSRWLock					ntdll		0
imp	'RtlUpdateTimer'					RtlUpdateTimer						ntdll		0
imp	'RtlUpperChar'						RtlUpperChar						ntdll		0
imp	'RtlUpperString'					RtlUpperString						ntdll		0
imp	'RtlUserThreadStart'					RtlUserThreadStart					ntdll		0
imp	'RtlValidAcl'						RtlValidAcl						ntdll		0
imp	'RtlValidProcessProtection'				RtlValidProcessProtection				ntdll		0
imp	'RtlValidRelativeSecurityDescriptor'			RtlValidRelativeSecurityDescriptor			ntdll		0
imp	'RtlValidSecurityDescriptor'				RtlValidSecurityDescriptor				ntdll		0
imp	'RtlValidSid'						RtlValidSid						ntdll		0
imp	'RtlValidateCorrelationVector'				RtlValidateCorrelationVector				ntdll		0
imp	'RtlValidateHeap'					RtlValidateHeap						ntdll		0	3
imp	'RtlValidateProcessHeaps'				RtlValidateProcessHeaps					ntdll		0
imp	'RtlValidateUnicodeString'				RtlValidateUnicodeString				ntdll		0
imp	'RtlVerifyVersionInfo'					RtlVerifyVersionInfo					ntdll		0
imp	'RtlVirtualUnwind'					RtlVirtualUnwind					ntdll		0
imp	'RtlWaitForWnfMetaNotification'				RtlWaitForWnfMetaNotification				ntdll		0
imp	'RtlWaitOnAddress'					RtlWaitOnAddress					ntdll		0
imp	'RtlWakeAddressAll'					RtlWakeAddressAll					ntdll		0
imp	'RtlWakeAddressAllNoFence'				RtlWakeAddressAllNoFence				ntdll		0
imp	'RtlWakeAddressSingle'					RtlWakeAddressSingle					ntdll		0
imp	'RtlWakeAddressSingleNoFence'				RtlWakeAddressSingleNoFence				ntdll		0
imp	'RtlWakeAllConditionVariable'				RtlWakeAllConditionVariable				ntdll		0
imp	'RtlWakeConditionVariable'				RtlWakeConditionVariable				ntdll		0
imp	'RtlWalkFrameChain'					RtlWalkFrameChain					ntdll		0
imp	'RtlWalkHeap'						RtlWalkHeap						ntdll		0	2
imp	'RtlWeaklyEnumerateEntryHashTable'			RtlWeaklyEnumerateEntryHashTable			ntdll		0
imp	'RtlWerpReportException'				RtlWerpReportException					ntdll		0
imp	'RtlWnfCompareChangeStamp'				RtlWnfCompareChangeStamp				ntdll		0
imp	'RtlWnfDllUnloadCallback'				RtlWnfDllUnloadCallback					ntdll		0
imp	'RtlWriteMemoryStream'					RtlWriteMemoryStream					ntdll		0
imp	'RtlWriteNonVolatileMemory'				RtlWriteNonVolatileMemory				ntdll		0
imp	'RtlWriteRegistryValue'					RtlWriteRegistryValue					ntdll		0
imp	'RtlZeroHeap'						RtlZeroHeap						ntdll		0
imp	'RtlZeroMemory'						RtlZeroMemory						ntdll		0
imp	'RtlZombifyActivationContext'				RtlZombifyActivationContext				ntdll		0
imp	'RtlpApplyLengthFunction'				RtlpApplyLengthFunction					ntdll		0
imp	'RtlpCheckDynamicTimeZoneInformation'			RtlpCheckDynamicTimeZoneInformation			ntdll		0
imp	'RtlpCleanupRegistryKeys'				RtlpCleanupRegistryKeys					ntdll		0
imp	'RtlpConvertAbsoluteToRelativeSecurityAttribute'	RtlpConvertAbsoluteToRelativeSecurityAttribute		ntdll		0
imp	'RtlpConvertCultureNamesToLCIDs'			RtlpConvertCultureNamesToLCIDs				ntdll		0
imp	'RtlpConvertLCIDsToCultureNames'			RtlpConvertLCIDsToCultureNames				ntdll		0
imp	'RtlpConvertRelativeToAbsoluteSecurityAttribute'	RtlpConvertRelativeToAbsoluteSecurityAttribute		ntdll		0
imp	'RtlpCreateProcessRegistryInfo'				RtlpCreateProcessRegistryInfo				ntdll		0
imp	'RtlpEnsureBufferSize'					RtlpEnsureBufferSize					ntdll		0
imp	'RtlpExecuteUmsThread'					RtlpExecuteUmsThread					ntdll		0
imp	'RtlpFreezeTimeBias'					RtlpFreezeTimeBias					ntdll		0
imp	'RtlpGetDeviceFamilyInfoEnum'				RtlpGetDeviceFamilyInfoEnum				ntdll		0
imp	'RtlpGetLCIDFromLangInfoNode'				RtlpGetLCIDFromLangInfoNode				ntdll		0
imp	'RtlpGetNameFromLangInfoNode'				RtlpGetNameFromLangInfoNode				ntdll		0
imp	'RtlpGetSystemDefaultUILanguage'			RtlpGetSystemDefaultUILanguage				ntdll		0
imp	'RtlpGetUserOrMachineUILanguage4NLS'			RtlpGetUserOrMachineUILanguage4NLS			ntdll		0
imp	'RtlpInitializeLangRegistryInfo'			RtlpInitializeLangRegistryInfo				ntdll		0
imp	'RtlpIsQualifiedLanguage'				RtlpIsQualifiedLanguage					ntdll		0
imp	'RtlpLoadMachineUIByPolicy'				RtlpLoadMachineUIByPolicy				ntdll		0
imp	'RtlpLoadUserUIByPolicy'				RtlpLoadUserUIByPolicy					ntdll		0
imp	'RtlpMergeSecurityAttributeInformation'			RtlpMergeSecurityAttributeInformation			ntdll		0
imp	'RtlpMuiFreeLangRegistryInfo'				RtlpMuiFreeLangRegistryInfo				ntdll		0
imp	'RtlpMuiRegCreateRegistryInfo'				RtlpMuiRegCreateRegistryInfo				ntdll		0
imp	'RtlpMuiRegFreeRegistryInfo'				RtlpMuiRegFreeRegistryInfo				ntdll		0
imp	'RtlpMuiRegLoadRegistryInfo'				RtlpMuiRegLoadRegistryInfo				ntdll		0
imp	'RtlpNotOwnerCriticalSection'				RtlpNotOwnerCriticalSection				ntdll		0
imp	'RtlpNtCreateKey'					RtlpNtCreateKey						ntdll		0
imp	'RtlpNtEnumerateSubKey'					RtlpNtEnumerateSubKey					ntdll		0
imp	'RtlpNtMakeTemporaryKey'				RtlpNtMakeTemporaryKey					ntdll		0
imp	'RtlpNtOpenKey'						RtlpNtOpenKey						ntdll		0
imp	'RtlpNtQueryValueKey'					RtlpNtQueryValueKey					ntdll		0
imp	'RtlpNtSetValueKey'					RtlpNtSetValueKey					ntdll		0
imp	'RtlpQueryDefaultUILanguage'				RtlpQueryDefaultUILanguage				ntdll		0
imp	'RtlpQueryProcessDebugInformationRemote'		RtlpQueryProcessDebugInformationRemote			ntdll		0
imp	'RtlpRefreshCachedUILanguage'				RtlpRefreshCachedUILanguage				ntdll		0
imp	'RtlpSetInstallLanguage'				RtlpSetInstallLanguage					ntdll		0
imp	'RtlpSetPreferredUILanguages'				RtlpSetPreferredUILanguages				ntdll		0
imp	'RtlpSetUserPreferredUILanguages'			RtlpSetUserPreferredUILanguages				ntdll		0
imp	'RtlpUmsExecuteYieldThreadEnd'				RtlpUmsExecuteYieldThreadEnd				ntdll		0
imp	'RtlpUmsThreadYield'					RtlpUmsThreadYield					ntdll		0
imp	'RtlpUnWaitCriticalSection'				RtlpUnWaitCriticalSection				ntdll		0
imp	'RtlpVerifyAndCommitUILanguageSettings'			RtlpVerifyAndCommitUILanguageSettings			ntdll		0
imp	'RtlpWaitForCriticalSection'				RtlpWaitForCriticalSection				ntdll		0
imp	'RtlxAnsiStringToUnicodeSize'				RtlxAnsiStringToUnicodeSize				ntdll		0
imp	'RtlxOemStringToUnicodeSize'				RtlxOemStringToUnicodeSize				ntdll		0
imp	'RtlxUnicodeStringToAnsiSize'				RtlxUnicodeStringToAnsiSize				ntdll		0
imp	'RtlxUnicodeStringToOemSize'				RtlxUnicodeStringToOemSize				ntdll		0
imp	'SbExecuteProcedure'					SbExecuteProcedure					ntdll		0
imp	'SbSelectProcedure'					SbSelectProcedure					ntdll		0
imp	'ShipAssert'						ShipAssert						ntdll		0
imp	'ShipAssertGetBufferInfo'				ShipAssertGetBufferInfo					ntdll		0
imp	'ShipAssertMsg'						ShipAssertMsgW						ntdll		0
imp	'TpAllocAlpcCompletion'					TpAllocAlpcCompletion					ntdll		0
imp	'TpAllocAlpcCompletionEx'				TpAllocAlpcCompletionEx					ntdll		0
imp	'TpAllocCleanupGroup'					TpAllocCleanupGroup					ntdll		0
imp	'TpAllocIoCompletion'					TpAllocIoCompletion					ntdll		0
imp	'TpAllocJobNotification'				TpAllocJobNotification					ntdll		0
imp	'TpAllocPool'						TpAllocPool						ntdll		0
imp	'TpAllocTimer'						TpAllocTimer						ntdll		0
imp	'TpAllocWait'						TpAllocWait						ntdll		0
imp	'TpAllocWork'						TpAllocWork						ntdll		0
imp	'TpAlpcRegisterCompletionList'				TpAlpcRegisterCompletionList				ntdll		0
imp	'TpAlpcUnregisterCompletionList'			TpAlpcUnregisterCompletionList				ntdll		0
imp	'TpCallbackDetectedUnrecoverableError'			TpCallbackDetectedUnrecoverableError			ntdll		0
imp	'TpCallbackIndependent'					TpCallbackIndependent					ntdll		0
imp	'TpCallbackLeaveCriticalSectionOnCompletion'		TpCallbackLeaveCriticalSectionOnCompletion		ntdll		0
imp	'TpCallbackMayRunLong'					TpCallbackMayRunLong					ntdll		0
imp	'TpCallbackReleaseMutexOnCompletion'			TpCallbackReleaseMutexOnCompletion			ntdll		0
imp	'TpCallbackReleaseSemaphoreOnCompletion'		TpCallbackReleaseSemaphoreOnCompletion			ntdll		0
imp	'TpCallbackSendAlpcMessageOnCompletion'			TpCallbackSendAlpcMessageOnCompletion			ntdll		0
imp	'TpCallbackSendPendingAlpcMessage'			TpCallbackSendPendingAlpcMessage			ntdll		0
imp	'TpCallbackSetEventOnCompletion'			TpCallbackSetEventOnCompletion				ntdll		0
imp	'TpCallbackUnloadDllOnCompletion'			TpCallbackUnloadDllOnCompletion				ntdll		0
imp	'TpCancelAsyncIoOperation'				TpCancelAsyncIoOperation				ntdll		0
imp	'TpCaptureCaller'					TpCaptureCaller						ntdll		0
imp	'TpCheckTerminateWorker'				TpCheckTerminateWorker					ntdll		0
imp	'TpDbgDumpHeapUsage'					TpDbgDumpHeapUsage					ntdll		0
imp	'TpDbgSetLogRoutine'					TpDbgSetLogRoutine					ntdll		0
imp	'TpDisablePoolCallbackChecks'				TpDisablePoolCallbackChecks				ntdll		0
imp	'TpDisassociateCallback'				TpDisassociateCallback					ntdll		0
imp	'TpIsTimerSet'						TpIsTimerSet						ntdll		0
imp	'TpPostWork'						TpPostWork						ntdll		0
imp	'TpQueryPoolStackInformation'				TpQueryPoolStackInformation				ntdll		0
imp	'TpReleaseAlpcCompletion'				TpReleaseAlpcCompletion					ntdll		0
imp	'TpReleaseCleanupGroup'					TpReleaseCleanupGroup					ntdll		0
imp	'TpReleaseCleanupGroupMembers'				TpReleaseCleanupGroupMembers				ntdll		0
imp	'TpReleaseIoCompletion'					TpReleaseIoCompletion					ntdll		0
imp	'TpReleaseJobNotification'				TpReleaseJobNotification				ntdll		0
imp	'TpReleasePool'						TpReleasePool						ntdll		0
imp	'TpReleaseTimer'					TpReleaseTimer						ntdll		0
imp	'TpReleaseWait'						TpReleaseWait						ntdll		0
imp	'TpReleaseWork'						TpReleaseWork						ntdll		0
imp	'TpSetDefaultPoolMaxThreads'				TpSetDefaultPoolMaxThreads				ntdll		0
imp	'TpSetDefaultPoolStackInformation'			TpSetDefaultPoolStackInformation			ntdll		0
imp	'TpSetPoolMaxThreads'					TpSetPoolMaxThreads					ntdll		0
imp	'TpSetPoolMaxThreadsSoftLimit'				TpSetPoolMaxThreadsSoftLimit				ntdll		0
imp	'TpSetPoolMinThreads'					TpSetPoolMinThreads					ntdll		0
imp	'TpSetPoolStackInformation'				TpSetPoolStackInformation				ntdll		0
imp	'TpSetPoolThreadBasePriority'				TpSetPoolThreadBasePriority				ntdll		0
imp	'TpSetPoolWorkerThreadIdleTimeout'			TpSetPoolWorkerThreadIdleTimeout			ntdll		0
imp	'TpSetTimer'						TpSetTimer						ntdll		0
imp	'TpSetTimerEx'						TpSetTimerEx						ntdll		0
imp	'TpSetWait'						TpSetWait						ntdll		0
imp	'TpSetWaitEx'						TpSetWaitEx						ntdll		0
imp	'TpSimpleTryPost'					TpSimpleTryPost						ntdll		0
imp	'TpStartAsyncIoOperation'				TpStartAsyncIoOperation					ntdll		0
imp	'TpTimerOutstandingCallbackCount'			TpTimerOutstandingCallbackCount				ntdll		0
imp	'TpTrimPools'						TpTrimPools						ntdll		0
imp	'TpWaitForAlpcCompletion'				TpWaitForAlpcCompletion					ntdll		0
imp	'TpWaitForIoCompletion'					TpWaitForIoCompletion					ntdll		0
imp	'TpWaitForJobNotification'				TpWaitForJobNotification				ntdll		0
imp	'TpWaitForTimer'					TpWaitForTimer						ntdll		0
imp	'TpWaitForWait'						TpWaitForWait						ntdll		0
imp	'TpWaitForWork'						TpWaitForWork						ntdll		0
imp	'VerSetConditionMask'					VerSetConditionMask					ntdll		0
imp	'WerReportExceptionWorker'				WerReportExceptionWorker				ntdll		0
imp	'WerReportSQMEvent'					WerReportSQMEvent					ntdll		0
imp	'WinSqmAddToAverageDWORD'				WinSqmAddToAverageDWORD					ntdll		0
imp	'WinSqmAddToStream'					WinSqmAddToStream					ntdll		0
imp	'WinSqmAddToStreamEx'					WinSqmAddToStreamEx					ntdll		0
imp	'WinSqmCheckEscalationAddToStreamEx'			WinSqmCheckEscalationAddToStreamEx			ntdll		0
imp	'WinSqmCheckEscalationSetDWORD'				WinSqmCheckEscalationSetDWORD				ntdll		0
imp	'WinSqmCheckEscalationSetDWORD64'			WinSqmCheckEscalationSetDWORD64				ntdll		0
imp	'WinSqmCheckEscalationSetString'			WinSqmCheckEscalationSetString				ntdll		0
imp	'WinSqmCommonDatapointDelete'				WinSqmCommonDatapointDelete				ntdll		0
imp	'WinSqmCommonDatapointSetDWORD'				WinSqmCommonDatapointSetDWORD				ntdll		0
imp	'WinSqmCommonDatapointSetDWORD64'			WinSqmCommonDatapointSetDWORD64				ntdll		0
imp	'WinSqmCommonDatapointSetStreamEx'			WinSqmCommonDatapointSetStreamEx			ntdll		0
imp	'WinSqmCommonDatapointSetString'			WinSqmCommonDatapointSetString				ntdll		0
imp	'WinSqmEndSession'					WinSqmEndSession					ntdll		0
imp	'WinSqmEventEnabled'					WinSqmEventEnabled					ntdll		0
imp	'WinSqmEventWrite'					WinSqmEventWrite					ntdll		0
imp	'WinSqmGetEscalationRuleStatus'				WinSqmGetEscalationRuleStatus				ntdll		0
imp	'WinSqmGetInstrumentationProperty'			WinSqmGetInstrumentationProperty			ntdll		0
imp	'WinSqmIncrementDWORD'					WinSqmIncrementDWORD					ntdll		0
imp	'WinSqmIsOptedIn'					WinSqmIsOptedIn						ntdll		0
imp	'WinSqmIsOptedInEx'					WinSqmIsOptedInEx					ntdll		0
imp	'WinSqmIsSessionDisabled'				WinSqmIsSessionDisabled					ntdll		0
imp	'WinSqmSetDWORD'					WinSqmSetDWORD						ntdll		0
imp	'WinSqmSetDWORD64'					WinSqmSetDWORD64					ntdll		0
imp	'WinSqmSetEscalationInfo'				WinSqmSetEscalationInfo					ntdll		0
imp	'WinSqmSetIfMaxDWORD'					WinSqmSetIfMaxDWORD					ntdll		0
imp	'WinSqmSetIfMinDWORD'					WinSqmSetIfMinDWORD					ntdll		0
imp	'WinSqmSetString'					WinSqmSetString						ntdll		0
imp	'WinSqmStartSession'					WinSqmStartSession					ntdll		0
imp	'WinSqmStartSessionForPartner'				WinSqmStartSessionForPartner				ntdll		0
imp	'WinSqmStartSqmOptinListener'				WinSqmStartSqmOptinListener				ntdll		0
imp	'ZwAcceptConnectPort'					ZwAcceptConnectPort					ntdll		0
imp	'ZwAccessCheck'						ZwAccessCheck						ntdll		0
imp	'ZwAccessCheckAndAuditAlarm'				ZwAccessCheckAndAuditAlarm				ntdll		0
imp	'ZwAccessCheckByType'					ZwAccessCheckByType					ntdll		0
imp	'ZwAccessCheckByTypeAndAuditAlarm'			ZwAccessCheckByTypeAndAuditAlarm			ntdll		0
imp	'ZwAccessCheckByTypeResultList'				ZwAccessCheckByTypeResultList				ntdll		0
imp	'ZwAccessCheckByTypeResultListAndAuditAlarm'		ZwAccessCheckByTypeResultListAndAuditAlarm		ntdll		0
imp	'ZwAccessCheckByTypeResultListAndAuditAlarmByHandle'	ZwAccessCheckByTypeResultListAndAuditAlarmByHandle	ntdll		0
imp	'ZwAcquireProcessActivityReference'			ZwAcquireProcessActivityReference			ntdll		0
imp	'ZwAddAtom'						ZwAddAtom						ntdll		0
imp	'ZwAddAtomEx'						ZwAddAtomEx						ntdll		0
imp	'ZwAddBootEntry'					ZwAddBootEntry						ntdll		0
imp	'ZwAddDriverEntry'					ZwAddDriverEntry					ntdll		0
imp	'ZwAdjustGroupsToken'					ZwAdjustGroupsToken					ntdll		0
imp	'ZwAdjustPrivilegesToken'				ZwAdjustPrivilegesToken					ntdll		0
imp	'ZwAdjustTokenClaimsAndDeviceGroups'			ZwAdjustTokenClaimsAndDeviceGroups			ntdll		0
imp	'ZwAlertResumeThread'					ZwAlertResumeThread					ntdll		0
imp	'ZwAlertThread'						ZwAlertThread						ntdll		0
imp	'ZwAlertThreadByThreadId'				ZwAlertThreadByThreadId					ntdll		0
imp	'ZwAllocateLocallyUniqueId'				ZwAllocateLocallyUniqueId				ntdll		0
imp	'ZwAllocateReserveObject'				ZwAllocateReserveObject					ntdll		0
imp	'ZwAllocateUserPhysicalPages'				ZwAllocateUserPhysicalPages				ntdll		0
imp	'ZwAllocateUuids'					ZwAllocateUuids						ntdll		0
imp	'ZwAllocateVirtualMemory'				ZwAllocateVirtualMemory					ntdll		0
imp	'ZwAllocateVirtualMemoryEx'				ZwAllocateVirtualMemoryEx				ntdll		0
imp	'ZwAlpcAcceptConnectPort'				ZwAlpcAcceptConnectPort					ntdll		0
imp	'ZwAlpcCancelMessage'					ZwAlpcCancelMessage					ntdll		0
imp	'ZwAlpcConnectPort'					ZwAlpcConnectPort					ntdll		0
imp	'ZwAlpcConnectPortEx'					ZwAlpcConnectPortEx					ntdll		0
imp	'ZwAlpcCreatePort'					ZwAlpcCreatePort					ntdll		0
imp	'ZwAlpcCreatePortSection'				ZwAlpcCreatePortSection					ntdll		0
imp	'ZwAlpcCreateResourceReserve'				ZwAlpcCreateResourceReserve				ntdll		0
imp	'ZwAlpcCreateSectionView'				ZwAlpcCreateSectionView					ntdll		0
imp	'ZwAlpcCreateSecurityContext'				ZwAlpcCreateSecurityContext				ntdll		0
imp	'ZwAlpcDeletePortSection'				ZwAlpcDeletePortSection					ntdll		0
imp	'ZwAlpcDeleteResourceReserve'				ZwAlpcDeleteResourceReserve				ntdll		0
imp	'ZwAlpcDeleteSectionView'				ZwAlpcDeleteSectionView					ntdll		0
imp	'ZwAlpcDeleteSecurityContext'				ZwAlpcDeleteSecurityContext				ntdll		0
imp	'ZwAlpcDisconnectPort'					ZwAlpcDisconnectPort					ntdll		0
imp	'ZwAlpcImpersonateClientContainerOfPort'		ZwAlpcImpersonateClientContainerOfPort			ntdll		0
imp	'ZwAlpcImpersonateClientOfPort'				ZwAlpcImpersonateClientOfPort				ntdll		0
imp	'ZwAlpcOpenSenderProcess'				ZwAlpcOpenSenderProcess					ntdll		0
imp	'ZwAlpcOpenSenderThread'				ZwAlpcOpenSenderThread					ntdll		0
imp	'ZwAlpcQueryInformation'				ZwAlpcQueryInformation					ntdll		0
imp	'ZwAlpcQueryInformationMessage'				ZwAlpcQueryInformationMessage				ntdll		0
imp	'ZwAlpcRevokeSecurityContext'				ZwAlpcRevokeSecurityContext				ntdll		0
imp	'ZwAlpcSendWaitReceivePort'				ZwAlpcSendWaitReceivePort				ntdll		0
imp	'ZwAlpcSetInformation'					ZwAlpcSetInformation					ntdll		0
imp	'ZwApphelpCacheControl'					ZwApphelpCacheControl					ntdll		0
imp	'ZwAreMappedFilesTheSame'				ZwAreMappedFilesTheSame					ntdll		0	2
imp	'ZwAssignProcessToJobObject'				ZwAssignProcessToJobObject				ntdll		0
imp	'ZwAssociateWaitCompletionPacket'			ZwAssociateWaitCompletionPacket				ntdll		0
imp	'ZwCallEnclave'						ZwCallEnclave						ntdll		0
imp	'ZwCallbackReturn'					ZwCallbackReturn					ntdll		0
imp	'ZwCancelIoFile'					ZwCancelIoFile						ntdll		0
imp	'ZwCancelIoFileEx'					ZwCancelIoFileEx					ntdll		0
imp	'ZwCancelSynchronousIoFile'				ZwCancelSynchronousIoFile				ntdll		0
imp	'ZwCancelTimer'						ZwCancelTimer						ntdll		0
imp	'ZwCancelTimer2'					ZwCancelTimer2						ntdll		0
imp	'ZwCancelWaitCompletionPacket'				ZwCancelWaitCompletionPacket				ntdll		0
imp	'ZwClearEvent'						ZwClearEvent						ntdll		0
imp	'ZwClose'						ZwClose							ntdll		0
imp	'ZwCloseObjectAuditAlarm'				ZwCloseObjectAuditAlarm					ntdll		0
imp	'ZwCommitComplete'					ZwCommitComplete					ntdll		0
imp	'ZwCommitEnlistment'					ZwCommitEnlistment					ntdll		0
imp	'ZwCommitRegistryTransaction'				ZwCommitRegistryTransaction				ntdll		0
imp	'ZwCommitTransaction'					ZwCommitTransaction					ntdll		0
imp	'ZwCompactKeys'						ZwCompactKeys						ntdll		0
imp	'ZwCompareObjects'					ZwCompareObjects					ntdll		0
imp	'ZwCompareSigningLevels'				ZwCompareSigningLevels					ntdll		0
imp	'ZwCompareTokens'					ZwCompareTokens						ntdll		0
imp	'ZwCompleteConnectPort'					ZwCompleteConnectPort					ntdll		0
imp	'ZwCompressKey'						ZwCompressKey						ntdll		0
imp	'ZwConnectPort'						ZwConnectPort						ntdll		0
imp	'ZwContinue'						ZwContinue						ntdll		0
imp	'ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter'	ZwConvertBetweenAuxiliaryCounterAndPerformanceCounter	ntdll		0
imp	'ZwCreateDebugObject'					ZwCreateDebugObject					ntdll		0
imp	'ZwCreateDirectoryObject'				ZwCreateDirectoryObject					ntdll		0
imp	'ZwCreateDirectoryObjectEx'				ZwCreateDirectoryObjectEx				ntdll		0
imp	'ZwCreateEnclave'					ZwCreateEnclave						ntdll		0
imp	'ZwCreateEnlistment'					ZwCreateEnlistment					ntdll		0
imp	'ZwCreateEvent'						ZwCreateEvent						ntdll		0
imp	'ZwCreateEventPair'					ZwCreateEventPair					ntdll		0
imp	'ZwCreateFile'						ZwCreateFile						ntdll		0
imp	'ZwCreateIRTimer'					ZwCreateIRTimer						ntdll		0
imp	'ZwCreateIoCompletion'					ZwCreateIoCompletion					ntdll		0
imp	'ZwCreateJobObject'					ZwCreateJobObject					ntdll		0
imp	'ZwCreateJobSet'					ZwCreateJobSet						ntdll		0
imp	'ZwCreateKey'						ZwCreateKey						ntdll		0
imp	'ZwCreateKeyTransacted'					ZwCreateKeyTransacted					ntdll		0
imp	'ZwCreateKeyedEvent'					ZwCreateKeyedEvent					ntdll		0
imp	'ZwCreateLowBoxToken'					ZwCreateLowBoxToken					ntdll		0
imp	'ZwCreateMailslotFile'					ZwCreateMailslotFile					ntdll		0
imp	'ZwCreateMutant'					ZwCreateMutant						ntdll		0
imp	'ZwCreateNamedPipeFile'					ZwCreateNamedPipeFile					ntdll		0
imp	'ZwCreatePagingFile'					ZwCreatePagingFile					ntdll		0
imp	'ZwCreatePartition'					ZwCreatePartition					ntdll		0
imp	'ZwCreatePort'						ZwCreatePort						ntdll		0
imp	'ZwCreatePrivateNamespace'				ZwCreatePrivateNamespace				ntdll		0
imp	'ZwCreateProcess'					ZwCreateProcess						ntdll		0
imp	'ZwCreateProcessEx'					ZwCreateProcessEx					ntdll		0
imp	'ZwCreateProfile'					ZwCreateProfile						ntdll		0
imp	'ZwCreateProfileEx'					ZwCreateProfileEx					ntdll		0
imp	'ZwCreateRegistryTransaction'				ZwCreateRegistryTransaction				ntdll		0
imp	'ZwCreateResourceManager'				ZwCreateResourceManager					ntdll		0
imp	'ZwCreateSection'					ZwCreateSection						ntdll		0
imp	'ZwCreateSemaphore'					ZwCreateSemaphore					ntdll		0
imp	'ZwCreateSymbolicLinkObject'				ZwCreateSymbolicLinkObject				ntdll		0
imp	'ZwCreateThread'					ZwCreateThread						ntdll		0
imp	'ZwCreateThreadEx'					ZwCreateThreadEx					ntdll		0
imp	'ZwCreateTimer'						ZwCreateTimer						ntdll		0
imp	'ZwCreateTimer2'					ZwCreateTimer2						ntdll		0
imp	'ZwCreateToken'						ZwCreateToken						ntdll		0
imp	'ZwCreateTokenEx'					ZwCreateTokenEx						ntdll		0
imp	'ZwCreateTransaction'					ZwCreateTransaction					ntdll		0
imp	'ZwCreateTransactionManager'				ZwCreateTransactionManager				ntdll		0
imp	'ZwCreateUserProcess'					ZwCreateUserProcess					ntdll		0
imp	'ZwCreateWaitCompletionPacket'				ZwCreateWaitCompletionPacket				ntdll		0
imp	'ZwCreateWaitablePort'					ZwCreateWaitablePort					ntdll		0
imp	'ZwCreateWnfStateName'					ZwCreateWnfStateName					ntdll		0
imp	'ZwCreateWorkerFactory'					ZwCreateWorkerFactory					ntdll		0
imp	'ZwDebugActiveProcess'					ZwDebugActiveProcess					ntdll		0
imp	'ZwDebugContinue'					ZwDebugContinue						ntdll		0
imp	'ZwDelayExecution'					ZwDelayExecution					ntdll		0
imp	'ZwDeleteAtom'						ZwDeleteAtom						ntdll		0
imp	'ZwDeleteBootEntry'					ZwDeleteBootEntry					ntdll		0
imp	'ZwDeleteDriverEntry'					ZwDeleteDriverEntry					ntdll		0
imp	'ZwDeleteFile'						ZwDeleteFile						ntdll		0
imp	'ZwDeleteKey'						ZwDeleteKey						ntdll		0
imp	'ZwDeleteObjectAuditAlarm'				ZwDeleteObjectAuditAlarm				ntdll		0
imp	'ZwDeletePrivateNamespace'				ZwDeletePrivateNamespace				ntdll		0
imp	'ZwDeleteValueKey'					ZwDeleteValueKey					ntdll		0
imp	'ZwDeleteWnfStateData'					ZwDeleteWnfStateData					ntdll		0
imp	'ZwDeleteWnfStateName'					ZwDeleteWnfStateName					ntdll		0
imp	'ZwDeviceIoControlFile'					ZwDeviceIoControlFile					ntdll		0
imp	'ZwDisableLastKnownGood'				ZwDisableLastKnownGood					ntdll		0
imp	'ZwDisplayString'					ZwDisplayString						ntdll		0
imp	'ZwDrawText'						ZwDrawText						ntdll		0
imp	'ZwDuplicateObject'					ZwDuplicateObject					ntdll		0
imp	'ZwDuplicateToken'					ZwDuplicateToken					ntdll		0
imp	'ZwEnableLastKnownGood'					ZwEnableLastKnownGood					ntdll		0
imp	'ZwEnumerateBootEntries'				ZwEnumerateBootEntries					ntdll		0
imp	'ZwEnumerateDriverEntries'				ZwEnumerateDriverEntries				ntdll		0
imp	'ZwEnumerateKey'					ZwEnumerateKey						ntdll		0
imp	'ZwEnumerateSystemEnvironmentValuesEx'			ZwEnumerateSystemEnvironmentValuesEx			ntdll		0
imp	'ZwEnumerateTransactionObject'				ZwEnumerateTransactionObject				ntdll		0
imp	'ZwEnumerateValueKey'					ZwEnumerateValueKey					ntdll		0
imp	'ZwExtendSection'					ZwExtendSection						ntdll		0
imp	'ZwFilterBootOption'					ZwFilterBootOption					ntdll		0
imp	'ZwFilterToken'						ZwFilterToken						ntdll		0
imp	'ZwFilterTokenEx'					ZwFilterTokenEx						ntdll		0
imp	'ZwFindAtom'						ZwFindAtom						ntdll		0
imp	'ZwFlushBuffersFile'					ZwFlushBuffersFile					ntdll		0
imp	'ZwFlushBuffersFileEx'					ZwFlushBuffersFileEx					ntdll		0
imp	'ZwFlushInstallUILanguage'				ZwFlushInstallUILanguage				ntdll		0
imp	'ZwFlushInstructionCache'				ZwFlushInstructionCache					ntdll		0
imp	'ZwFlushKey'						ZwFlushKey						ntdll		0
imp	'ZwFlushProcessWriteBuffers'				ZwFlushProcessWriteBuffers				ntdll		0
imp	'ZwFlushVirtualMemory'					ZwFlushVirtualMemory					ntdll		0
imp	'ZwFlushWriteBuffer'					ZwFlushWriteBuffer					ntdll		0
imp	'ZwFreeUserPhysicalPages'				ZwFreeUserPhysicalPages					ntdll		0
imp	'ZwFreeVirtualMemory'					ZwFreeVirtualMemory					ntdll		0
imp	'ZwFreezeRegistry'					ZwFreezeRegistry					ntdll		0
imp	'ZwFreezeTransactions'					ZwFreezeTransactions					ntdll		0
imp	'ZwFsControlFile'					ZwFsControlFile						ntdll		0
imp	'ZwGetCachedSigningLevel'				ZwGetCachedSigningLevel					ntdll		0
imp	'ZwGetCompleteWnfStateSubscription'			ZwGetCompleteWnfStateSubscription			ntdll		0
imp	'ZwGetContextThread'					ZwGetContextThread					ntdll		0
imp	'ZwGetCurrentProcessorNumber'				ZwGetCurrentProcessorNumber				ntdll		0
imp	'ZwGetCurrentProcessorNumberEx'				ZwGetCurrentProcessorNumberEx				ntdll		0
imp	'ZwGetDevicePowerState'					ZwGetDevicePowerState					ntdll		0
imp	'ZwGetMUIRegistryInfo'					ZwGetMUIRegistryInfo					ntdll		0
imp	'ZwGetNextProcess'					ZwGetNextProcess					ntdll		0
imp	'ZwGetNextThread'					ZwGetNextThread						ntdll		0
imp	'ZwGetNlsSectionPtr'					ZwGetNlsSectionPtr					ntdll		0
imp	'ZwGetNotificationResourceManager'			ZwGetNotificationResourceManager			ntdll		0
imp	'ZwGetWriteWatch'					ZwGetWriteWatch						ntdll		0
imp	'ZwImpersonateAnonymousToken'				ZwImpersonateAnonymousToken				ntdll		0
imp	'ZwImpersonateClientOfPort'				ZwImpersonateClientOfPort				ntdll		0
imp	'ZwImpersonateThread'					ZwImpersonateThread					ntdll		0
imp	'ZwInitializeEnclave'					ZwInitializeEnclave					ntdll		0
imp	'ZwInitializeNlsFiles'					ZwInitializeNlsFiles					ntdll		0
imp	'ZwInitializeRegistry'					ZwInitializeRegistry					ntdll		0
imp	'ZwInitiatePowerAction'					ZwInitiatePowerAction					ntdll		0
imp	'ZwIsProcessInJob'					ZwIsProcessInJob					ntdll		0
imp	'ZwIsSystemResumeAutomatic'				ZwIsSystemResumeAutomatic				ntdll		0
imp	'ZwIsUILanguageComitted'				ZwIsUILanguageComitted					ntdll		0
imp	'ZwListenPort'						ZwListenPort						ntdll		0
imp	'ZwLoadDriver'						ZwLoadDriver						ntdll		0
imp	'ZwLoadEnclaveData'					ZwLoadEnclaveData					ntdll		0
imp	'ZwLoadHotPatch'					ZwLoadHotPatch						ntdll		0
imp	'ZwLoadKey'						ZwLoadKey						ntdll		0
imp	'ZwLoadKey2'						ZwLoadKey2						ntdll		0
imp	'ZwLoadKeyEx'						ZwLoadKeyEx						ntdll		0
imp	'ZwLockFile'						ZwLockFile						ntdll		0
imp	'ZwLockProductActivationKeys'				ZwLockProductActivationKeys				ntdll		0
imp	'ZwLockRegistryKey'					ZwLockRegistryKey					ntdll		0
imp	'ZwLockVirtualMemory'					ZwLockVirtualMemory					ntdll		0
imp	'ZwMakePermanentObject'					ZwMakePermanentObject					ntdll		0
imp	'ZwMakeTemporaryObject'					ZwMakeTemporaryObject					ntdll		0
imp	'ZwManagePartition'					ZwManagePartition					ntdll		0
imp	'ZwMapCMFModule'					ZwMapCMFModule						ntdll		0
imp	'ZwMapUserPhysicalPages'				ZwMapUserPhysicalPages					ntdll		0
imp	'ZwMapUserPhysicalPagesScatter'				ZwMapUserPhysicalPagesScatter				ntdll		0
imp	'ZwMapViewOfSection'					ZwMapViewOfSection					ntdll		0
imp	'ZwMapViewOfSectionEx'					ZwMapViewOfSectionEx					ntdll		0
imp	'ZwModifyBootEntry'					ZwModifyBootEntry					ntdll		0
imp	'ZwModifyDriverEntry'					ZwModifyDriverEntry					ntdll		0
imp	'ZwNotifyChangeDirectoryFile'				ZwNotifyChangeDirectoryFile				ntdll		0
imp	'ZwNotifyChangeDirectoryFileEx'				ZwNotifyChangeDirectoryFileEx				ntdll		0
imp	'ZwNotifyChangeKey'					ZwNotifyChangeKey					ntdll		0
imp	'ZwNotifyChangeMultipleKeys'				ZwNotifyChangeMultipleKeys				ntdll		0
imp	'ZwNotifyChangeSession'					ZwNotifyChangeSession					ntdll		0
imp	'ZwOpenDirectoryObject'					ZwOpenDirectoryObject					ntdll		0
imp	'ZwOpenEnlistment'					ZwOpenEnlistment					ntdll		0
imp	'ZwOpenEvent'						ZwOpenEvent						ntdll		0
imp	'ZwOpenEventPair'					ZwOpenEventPair						ntdll		0
imp	'ZwOpenFile'						ZwOpenFile						ntdll		0
imp	'ZwOpenIoCompletion'					ZwOpenIoCompletion					ntdll		0
imp	'ZwOpenJobObject'					ZwOpenJobObject						ntdll		0
imp	'ZwOpenKey'						ZwOpenKey						ntdll		0
imp	'ZwOpenKeyEx'						ZwOpenKeyEx						ntdll		0
imp	'ZwOpenKeyTransacted'					ZwOpenKeyTransacted					ntdll		0
imp	'ZwOpenKeyTransactedEx'					ZwOpenKeyTransactedEx					ntdll		0
imp	'ZwOpenKeyedEvent'					ZwOpenKeyedEvent					ntdll		0
imp	'ZwOpenMutant'						ZwOpenMutant						ntdll		0
imp	'ZwOpenObjectAuditAlarm'				ZwOpenObjectAuditAlarm					ntdll		0
imp	'ZwOpenPartition'					ZwOpenPartition						ntdll		0
imp	'ZwOpenPrivateNamespace'				ZwOpenPrivateNamespace					ntdll		0
imp	'ZwOpenProcess'						ZwOpenProcess						ntdll		0
imp	'ZwOpenProcessToken'					ZwOpenProcessToken					ntdll		0
imp	'ZwOpenProcessTokenEx'					ZwOpenProcessTokenEx					ntdll		0
imp	'ZwOpenRegistryTransaction'				ZwOpenRegistryTransaction				ntdll		0
imp	'ZwOpenResourceManager'					ZwOpenResourceManager					ntdll		0
imp	'ZwOpenSection'						ZwOpenSection						ntdll		0
imp	'ZwOpenSemaphore'					ZwOpenSemaphore						ntdll		0
imp	'ZwOpenSession'						ZwOpenSession						ntdll		0
imp	'ZwOpenSymbolicLinkObject'				ZwOpenSymbolicLinkObject				ntdll		0
imp	'ZwOpenThread'						ZwOpenThread						ntdll		0
imp	'ZwOpenThreadToken'					ZwOpenThreadToken					ntdll		0
imp	'ZwOpenThreadTokenEx'					ZwOpenThreadTokenEx					ntdll		0
imp	'ZwOpenTimer'						ZwOpenTimer						ntdll		0
imp	'ZwOpenTransaction'					ZwOpenTransaction					ntdll		0
imp	'ZwOpenTransactionManager'				ZwOpenTransactionManager				ntdll		0
imp	'ZwPlugPlayControl'					ZwPlugPlayControl					ntdll		0
imp	'ZwPowerInformation'					ZwPowerInformation					ntdll		0
imp	'ZwPrePrepareComplete'					ZwPrePrepareComplete					ntdll		0
imp	'ZwPrePrepareEnlistment'				ZwPrePrepareEnlistment					ntdll		0
imp	'ZwPrepareComplete'					ZwPrepareComplete					ntdll		0
imp	'ZwPrepareEnlistment'					ZwPrepareEnlistment					ntdll		0
imp	'ZwPrivilegeCheck'					ZwPrivilegeCheck					ntdll		0
imp	'ZwPrivilegeObjectAuditAlarm'				ZwPrivilegeObjectAuditAlarm				ntdll		0
imp	'ZwPrivilegedServiceAuditAlarm'				ZwPrivilegedServiceAuditAlarm				ntdll		0
imp	'ZwPropagationComplete'					ZwPropagationComplete					ntdll		0
imp	'ZwPropagationFailed'					ZwPropagationFailed					ntdll		0
imp	'ZwProtectVirtualMemory'				ZwProtectVirtualMemory					ntdll		0
imp	'ZwPulseEvent'						ZwPulseEvent						ntdll		0
imp	'ZwQueryAttributesFile'					ZwQueryAttributesFile					ntdll		0
imp	'ZwQueryAuxiliaryCounterFrequency'			ZwQueryAuxiliaryCounterFrequency			ntdll		0
imp	'ZwQueryBootEntryOrder'					ZwQueryBootEntryOrder					ntdll		0
imp	'ZwQueryBootOptions'					ZwQueryBootOptions					ntdll		0
imp	'ZwQueryDebugFilterState'				ZwQueryDebugFilterState					ntdll		0
imp	'ZwQueryDefaultLocale'					ZwQueryDefaultLocale					ntdll		0
imp	'ZwQueryDefaultUILanguage'				ZwQueryDefaultUILanguage				ntdll		0
imp	'ZwQueryDirectoryFile'					ZwQueryDirectoryFile					ntdll		0
imp	'ZwQueryDirectoryFileEx'				ZwQueryDirectoryFileEx					ntdll		0
imp	'ZwQueryDirectoryObject'				ZwQueryDirectoryObject					ntdll		0
imp	'ZwQueryDriverEntryOrder'				ZwQueryDriverEntryOrder					ntdll		0
imp	'ZwQueryEaFile'						ZwQueryEaFile						ntdll		0
imp	'ZwQueryEvent'						ZwQueryEvent						ntdll		0
imp	'ZwQueryFullAttributesFile'				ZwQueryFullAttributesFile				ntdll		0
imp	'ZwQueryInformationAtom'				ZwQueryInformationAtom					ntdll		0
imp	'ZwQueryInformationByName'				ZwQueryInformationByName				ntdll		0
imp	'ZwQueryInformationEnlistment'				ZwQueryInformationEnlistment				ntdll		0
imp	'ZwQueryInformationFile'				ZwQueryInformationFile					ntdll		0
imp	'ZwQueryInformationJobObject'				ZwQueryInformationJobObject				ntdll		0
imp	'ZwQueryInformationPort'				ZwQueryInformationPort					ntdll		0
imp	'ZwQueryInformationProcess'				ZwQueryInformationProcess				ntdll		0
imp	'ZwQueryInformationResourceManager'			ZwQueryInformationResourceManager			ntdll		0
imp	'ZwQueryInformationThread'				ZwQueryInformationThread				ntdll		0
imp	'ZwQueryInformationToken'				ZwQueryInformationToken					ntdll		0
imp	'ZwQueryInformationTransaction'				ZwQueryInformationTransaction				ntdll		0
imp	'ZwQueryInformationTransactionManager'			ZwQueryInformationTransactionManager			ntdll		0
imp	'ZwQueryInformationWorkerFactory'			ZwQueryInformationWorkerFactory				ntdll		0
imp	'ZwQueryInstallUILanguage'				ZwQueryInstallUILanguage				ntdll		0
imp	'ZwQueryIntervalProfile'				ZwQueryIntervalProfile					ntdll		0
imp	'ZwQueryIoCompletion'					ZwQueryIoCompletion					ntdll		0
imp	'ZwQueryKey'						ZwQueryKey						ntdll		0
imp	'ZwQueryLicenseValue'					ZwQueryLicenseValue					ntdll		0
imp	'ZwQueryMultipleValueKey'				ZwQueryMultipleValueKey					ntdll		0
imp	'ZwQueryMutant'						ZwQueryMutant						ntdll		0
imp	'ZwQueryObject'						ZwQueryObject						ntdll		0
imp	'ZwQueryOpenSubKeys'					ZwQueryOpenSubKeys					ntdll		0
imp	'ZwQueryOpenSubKeysEx'					ZwQueryOpenSubKeysEx					ntdll		0
imp	'ZwQueryPerformanceCounter'				ZwQueryPerformanceCounter				ntdll		0
imp	'ZwQueryPortInformationProcess'				ZwQueryPortInformationProcess				ntdll		0
imp	'ZwQueryQuotaInformationFile'				ZwQueryQuotaInformationFile				ntdll		0
imp	'ZwQuerySection'					ZwQuerySection						ntdll		0
imp	'ZwQuerySecurityAttributesToken'			ZwQuerySecurityAttributesToken				ntdll		0
imp	'ZwQuerySecurityObject'					ZwQuerySecurityObject					ntdll		0
imp	'ZwQuerySecurityPolicy'					ZwQuerySecurityPolicy					ntdll		0
imp	'ZwQuerySemaphore'					ZwQuerySemaphore					ntdll		0
imp	'ZwQuerySymbolicLinkObject'				ZwQuerySymbolicLinkObject				ntdll		0
imp	'ZwQuerySystemEnvironmentValue'				ZwQuerySystemEnvironmentValue				ntdll		0
imp	'ZwQuerySystemEnvironmentValueEx'			ZwQuerySystemEnvironmentValueEx				ntdll		0
imp	'ZwQuerySystemInformation'				ZwQuerySystemInformation				ntdll		0
imp	'ZwQuerySystemInformationEx'				ZwQuerySystemInformationEx				ntdll		0
imp	'ZwQuerySystemTime'					ZwQuerySystemTime					ntdll		0
imp	'ZwQueryTimer'						ZwQueryTimer						ntdll		0
imp	'ZwQueryTimerResolution'				ZwQueryTimerResolution					ntdll		0
imp	'ZwQueryValueKey'					ZwQueryValueKey						ntdll		0
imp	'ZwQueryVirtualMemory'					ZwQueryVirtualMemory					ntdll		0
imp	'ZwQueryVolumeInformationFile'				ZwQueryVolumeInformationFile				ntdll		0
imp	'ZwQueryWnfStateData'					ZwQueryWnfStateData					ntdll		0
imp	'ZwQueryWnfStateNameInformation'			ZwQueryWnfStateNameInformation				ntdll		0
imp	'ZwQueueApcThread'					ZwQueueApcThread					ntdll		0
imp	'ZwQueueApcThreadEx'					ZwQueueApcThreadEx					ntdll		0
imp	'ZwRaiseException'					ZwRaiseException					ntdll		0
imp	'ZwRaiseHardError'					ZwRaiseHardError					ntdll		0
imp	'ZwReadFile'						ZwReadFile						ntdll		0
imp	'ZwReadFileScatter'					ZwReadFileScatter					ntdll		0
imp	'ZwReadOnlyEnlistment'					ZwReadOnlyEnlistment					ntdll		0
imp	'ZwReadRequestData'					ZwReadRequestData					ntdll		0
imp	'ZwReadVirtualMemory'					ZwReadVirtualMemory					ntdll		0
imp	'ZwRecoverEnlistment'					ZwRecoverEnlistment					ntdll		0
imp	'ZwRecoverResourceManager'				ZwRecoverResourceManager				ntdll		0
imp	'ZwRecoverTransactionManager'				ZwRecoverTransactionManager				ntdll		0
imp	'ZwRegisterProtocolAddressInformation'			ZwRegisterProtocolAddressInformation			ntdll		0
imp	'ZwRegisterThreadTerminatePort'				ZwRegisterThreadTerminatePort				ntdll		0
imp	'ZwReleaseKeyedEvent'					ZwReleaseKeyedEvent					ntdll		0
imp	'ZwReleaseMutant'					ZwReleaseMutant						ntdll		0
imp	'ZwReleaseSemaphore'					ZwReleaseSemaphore					ntdll		0
imp	'ZwReleaseWorkerFactoryWorker'				ZwReleaseWorkerFactoryWorker				ntdll		0
imp	'ZwRemoveIoCompletion'					ZwRemoveIoCompletion					ntdll		0
imp	'ZwRemoveIoCompletionEx'				ZwRemoveIoCompletionEx					ntdll		0
imp	'ZwRemoveProcessDebug'					ZwRemoveProcessDebug					ntdll		0
imp	'ZwRenameKey'						ZwRenameKey						ntdll		0
imp	'ZwRenameTransactionManager'				ZwRenameTransactionManager				ntdll		0
imp	'ZwReplaceKey'						ZwReplaceKey						ntdll		0
imp	'ZwReplacePartitionUnit'				ZwReplacePartitionUnit					ntdll		0
imp	'ZwReplyPort'						ZwReplyPort						ntdll		0
imp	'ZwReplyWaitReceivePort'				ZwReplyWaitReceivePort					ntdll		0
imp	'ZwReplyWaitReceivePortEx'				ZwReplyWaitReceivePortEx				ntdll		0
imp	'ZwReplyWaitReplyPort'					ZwReplyWaitReplyPort					ntdll		0
imp	'ZwRequestPort'						ZwRequestPort						ntdll		0
imp	'ZwRequestWaitReplyPort'				ZwRequestWaitReplyPort					ntdll		0
imp	'ZwResetEvent'						ZwResetEvent						ntdll		0
imp	'ZwResetWriteWatch'					ZwResetWriteWatch					ntdll		0
imp	'ZwRestoreKey'						ZwRestoreKey						ntdll		0
imp	'ZwResumeProcess'					ZwResumeProcess						ntdll		0
imp	'ZwResumeThread'					ZwResumeThread						ntdll		0
imp	'ZwRevertContainerImpersonation'			ZwRevertContainerImpersonation				ntdll		0
imp	'ZwRollbackComplete'					ZwRollbackComplete					ntdll		0
imp	'ZwRollbackEnlistment'					ZwRollbackEnlistment					ntdll		0
imp	'ZwRollbackRegistryTransaction'				ZwRollbackRegistryTransaction				ntdll		0
imp	'ZwRollbackTransaction'					ZwRollbackTransaction					ntdll		0
imp	'ZwRollforwardTransactionManager'			ZwRollforwardTransactionManager				ntdll		0
imp	'ZwSaveKey'						ZwSaveKey						ntdll		0
imp	'ZwSaveKeyEx'						ZwSaveKeyEx						ntdll		0
imp	'ZwSaveMergedKeys'					ZwSaveMergedKeys					ntdll		0
imp	'ZwSecureConnectPort'					ZwSecureConnectPort					ntdll		0
imp	'ZwSerializeBoot'					ZwSerializeBoot						ntdll		0
imp	'ZwSetBootEntryOrder'					ZwSetBootEntryOrder					ntdll		0
imp	'ZwSetBootOptions'					ZwSetBootOptions					ntdll		0
imp	'ZwSetCachedSigningLevel'				ZwSetCachedSigningLevel					ntdll		0
imp	'ZwSetCachedSigningLevel2'				ZwSetCachedSigningLevel2				ntdll		0
imp	'ZwSetContextThread'					ZwSetContextThread					ntdll		0
imp	'ZwSetDebugFilterState'					ZwSetDebugFilterState					ntdll		0
imp	'ZwSetDefaultHardErrorPort'				ZwSetDefaultHardErrorPort				ntdll		0
imp	'ZwSetDefaultLocale'					ZwSetDefaultLocale					ntdll		0
imp	'ZwSetDefaultUILanguage'				ZwSetDefaultUILanguage					ntdll		0
imp	'ZwSetDriverEntryOrder'					ZwSetDriverEntryOrder					ntdll		0
imp	'ZwSetEaFile'						ZwSetEaFile						ntdll		0
imp	'ZwSetEvent'						ZwSetEvent						ntdll		0
imp	'ZwSetEventBoostPriority'				ZwSetEventBoostPriority					ntdll		0
imp	'ZwSetHighEventPair'					ZwSetHighEventPair					ntdll		0
imp	'ZwSetHighWaitLowEventPair'				ZwSetHighWaitLowEventPair				ntdll		0
imp	'ZwSetIRTimer'						ZwSetIRTimer						ntdll		0
imp	'ZwSetInformationDebugObject'				ZwSetInformationDebugObject				ntdll		0
imp	'ZwSetInformationEnlistment'				ZwSetInformationEnlistment				ntdll		0
imp	'ZwSetInformationFile'					ZwSetInformationFile					ntdll		0
imp	'ZwSetInformationJobObject'				ZwSetInformationJobObject				ntdll		0
imp	'ZwSetInformationKey'					ZwSetInformationKey					ntdll		0
imp	'ZwSetInformationObject'				ZwSetInformationObject					ntdll		0
imp	'ZwSetInformationProcess'				ZwSetInformationProcess					ntdll		0
imp	'ZwSetInformationResourceManager'			ZwSetInformationResourceManager				ntdll		0
imp	'ZwSetInformationSymbolicLink'				ZwSetInformationSymbolicLink				ntdll		0
imp	'ZwSetInformationThread'				ZwSetInformationThread					ntdll		0
imp	'ZwSetInformationToken'					ZwSetInformationToken					ntdll		0
imp	'ZwSetInformationTransaction'				ZwSetInformationTransaction				ntdll		0
imp	'ZwSetInformationTransactionManager'			ZwSetInformationTransactionManager			ntdll		0
imp	'ZwSetInformationVirtualMemory'				ZwSetInformationVirtualMemory				ntdll		0
imp	'ZwSetInformationWorkerFactory'				ZwSetInformationWorkerFactory				ntdll		0
imp	'ZwSetIntervalProfile'					ZwSetIntervalProfile					ntdll		0
imp	'ZwSetIoCompletion'					ZwSetIoCompletion					ntdll		0
imp	'ZwSetIoCompletionEx'					ZwSetIoCompletionEx					ntdll		0
imp	'ZwSetLdtEntries'					ZwSetLdtEntries						ntdll		0
imp	'ZwSetLowEventPair'					ZwSetLowEventPair					ntdll		0
imp	'ZwSetLowWaitHighEventPair'				ZwSetLowWaitHighEventPair				ntdll		0
imp	'ZwSetQuotaInformationFile'				ZwSetQuotaInformationFile				ntdll		0
imp	'ZwSetSecurityObject'					ZwSetSecurityObject					ntdll		0
imp	'ZwSetSystemEnvironmentValue'				ZwSetSystemEnvironmentValue				ntdll		0
imp	'ZwSetSystemEnvironmentValueEx'				ZwSetSystemEnvironmentValueEx				ntdll		0
imp	'ZwSetSystemInformation'				ZwSetSystemInformation					ntdll		0
imp	'ZwSetSystemPowerState'					ZwSetSystemPowerState					ntdll		0
imp	'ZwSetSystemTime'					ZwSetSystemTime						ntdll		0
imp	'ZwSetThreadExecutionState'				ZwSetThreadExecutionState				ntdll		0
imp	'ZwSetTimer'						ZwSetTimer						ntdll		0
imp	'ZwSetTimer2'						ZwSetTimer2						ntdll		0
imp	'ZwSetTimerEx'						ZwSetTimerEx						ntdll		0
imp	'ZwSetTimerResolution'					ZwSetTimerResolution					ntdll		0
imp	'ZwSetUuidSeed'						ZwSetUuidSeed						ntdll		0
imp	'ZwSetValueKey'						ZwSetValueKey						ntdll		0
imp	'ZwSetVolumeInformationFile'				ZwSetVolumeInformationFile				ntdll		0
imp	'ZwSetWnfProcessNotificationEvent'			ZwSetWnfProcessNotificationEvent			ntdll		0
imp	'ZwShutdownSystem'					ZwShutdownSystem					ntdll		0
imp	'ZwShutdownWorkerFactory'				ZwShutdownWorkerFactory					ntdll		0
imp	'ZwSignalAndWaitForSingleObject'			ZwSignalAndWaitForSingleObject				ntdll		0
imp	'ZwSinglePhaseReject'					ZwSinglePhaseReject					ntdll		0
imp	'ZwStartProfile'					ZwStartProfile						ntdll		0
imp	'ZwStopProfile'						ZwStopProfile						ntdll		0
imp	'ZwSubscribeWnfStateChange'				ZwSubscribeWnfStateChange				ntdll		0
imp	'ZwSuspendProcess'					ZwSuspendProcess					ntdll		0
imp	'ZwSuspendThread'					ZwSuspendThread						ntdll		0
imp	'ZwSystemDebugControl'					ZwSystemDebugControl					ntdll		0
imp	'ZwTerminateEnclave'					ZwTerminateEnclave					ntdll		0
imp	'ZwTerminateJobObject'					ZwTerminateJobObject					ntdll		0
imp	'ZwTerminateProcess'					ZwTerminateProcess					ntdll		0
imp	'ZwTerminateThread'					ZwTerminateThread					ntdll		0
imp	'ZwTestAlert'						ZwTestAlert						ntdll		0
imp	'ZwThawRegistry'					ZwThawRegistry						ntdll		0
imp	'ZwThawTransactions'					ZwThawTransactions					ntdll		0
imp	'ZwTraceControl'					ZwTraceControl						ntdll		0
imp	'ZwTraceEvent'						ZwTraceEvent						ntdll		0
imp	'ZwTranslateFilePath'					ZwTranslateFilePath					ntdll		0
imp	'ZwUmsThreadYield'					ZwUmsThreadYield					ntdll		0
imp	'ZwUnloadDriver'					ZwUnloadDriver						ntdll		0
imp	'ZwUnloadKey'						ZwUnloadKey						ntdll		0
imp	'ZwUnloadKey2'						ZwUnloadKey2						ntdll		0
imp	'ZwUnloadKeyEx'						ZwUnloadKeyEx						ntdll		0
imp	'ZwUnlockFile'						ZwUnlockFile						ntdll		0
imp	'ZwUnlockVirtualMemory'					ZwUnlockVirtualMemory					ntdll		0
imp	'ZwUnmapViewOfSection'					ZwUnmapViewOfSection					ntdll		0
imp	'ZwUnmapViewOfSectionEx'				ZwUnmapViewOfSectionEx					ntdll		0
imp	'ZwUnsubscribeWnfStateChange'				ZwUnsubscribeWnfStateChange				ntdll		0
imp	'ZwUpdateWnfStateData'					ZwUpdateWnfStateData					ntdll		0
imp	'ZwVdmControl'						ZwVdmControl						ntdll		0
imp	'ZwWaitForAlertByThreadId'				ZwWaitForAlertByThreadId				ntdll		0
imp	'ZwWaitForDebugEvent'					ZwWaitForDebugEvent					ntdll		0
imp	'ZwWaitForKeyedEvent'					ZwWaitForKeyedEvent					ntdll		0
imp	'ZwWaitForMultipleObjects'				ZwWaitForMultipleObjects				ntdll		0
imp	'ZwWaitForMultipleObjects32'				ZwWaitForMultipleObjects32				ntdll		0
imp	'ZwWaitForSingleObject'					ZwWaitForSingleObject					ntdll		0
imp	'ZwWaitForWorkViaWorkerFactory'				ZwWaitForWorkViaWorkerFactory				ntdll		0
imp	'ZwWaitHighEventPair'					ZwWaitHighEventPair					ntdll		0
imp	'ZwWaitLowEventPair'					ZwWaitLowEventPair					ntdll		0
imp	'ZwWorkerFactoryWorkerReady'				ZwWorkerFactoryWorkerReady				ntdll		0
imp	'ZwWriteFile'						ZwWriteFile						ntdll		0
imp	'ZwWriteFileGather'					ZwWriteFileGather					ntdll		0
imp	'ZwWriteRequestData'					ZwWriteRequestData					ntdll		0
imp	'ZwWriteVirtualMemory'					ZwWriteVirtualMemory					ntdll		0
imp	'ZwYieldExecution'					ZwYieldExecution					ntdll		0
