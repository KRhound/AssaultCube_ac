#include <ntddk.h>
#include <ntstrsafe.h>

// ���μ��� ������ ������ �� ����ϴ� ���� Ŭ������ �� 5�� ����
#define SystemProcessInformation 5
// ���μ��� ���� ��ȸ�� �ʿ��� ������ ���� (���μ��� ���� ����)
#define PROCESS_QUERY_INFORMATION (0x0400)
// ���μ��� �޸� �б⿡ �ʿ��� ������ ����
#define PROCESS_VM_READ (0x0010)
// ����͸��� ���μ��� ����Ʈ�� ũ�� ���� (5��)
#define ProcessListLength 5
// ���μ��� ���� ������ ����
#define PROCESS_TERMINATE (0x0001)

// �޸� ���� Ŭ������ ����
typedef enum _MEMORY_INFORMATION_CLASS {
    MemoryBasicInformation,   // �⺻ �޸� ����
    MemoryWorkingSetInformation,   // ��ŷ �� ����
    MemorySectionName,   // ���� �̸�
    MemoryBasicVlmInformation   // ���� �޸� ����
} MEMORY_INFORMATION_CLASS;

// �޸� �⺻ ���� ����ü ���� (�� �ʵ�� �޸� ������ ���� ������ ����)
typedef struct _MEMORY_BASIC_INFORMATION {
    PVOID BaseAddress;      // �⺻ �ּ�
    PVOID AllocationBase;   // �Ҵ�� �⺻ �ּ�
    ULONG AllocationProtect; // �Ҵ�� ��ȣ �Ӽ�
    SIZE_T RegionSize;      // �޸� ���� ũ��
    ULONG State;            // �޸� ���� (Ŀ�Ե� �������� ��)
    ULONG Protect;          // ��ȣ �Ӽ�
    ULONG Type;             // �޸� Ÿ�� (�̹���, ������ ��)
} MEMORY_BASIC_INFORMATION, * PMEMORY_BASIC_INFORMATION;

// MEM_IMAGE �÷��� ���� (�޸� Ÿ���� �̹����� ��츦 ��Ÿ��)
#define MEM_IMAGE 0x1000000

// �ý��� ���μ��� ���� ����ü ���� (���μ��� ���� ������ �����ϴ� ����ü)
typedef struct _SYSTEM_PROCESS_INFORMATION {
    ULONG NextEntryOffset;         // ���� ���μ��� �������� ������
    ULONG NumberOfThreads;         // ���μ����� �ִ� ������ ��
    LARGE_INTEGER WorkingSetPrivateSize; // ��ŷ �� �����̺� ũ��
    ULONG HardFaultCount;          // �ϵ� ������ ��Ʈ ��
    ULONG NumberOfThreadsHighWatermark;  // �ִ� ������ ��
    ULONGLONG CycleTime;           // ���μ����� CPU ����Ŭ �ð�
    LARGE_INTEGER CreateTime;      // ���μ��� ���� �ð�
    LARGE_INTEGER UserTime;        // ����� ��忡���� ���� �ð�
    LARGE_INTEGER KernelTime;      // Ŀ�� ��忡���� ���� �ð�
    UNICODE_STRING ImageName;      // ���μ��� �̹��� �̸�
    ULONG BasePriority;            // ���μ��� �켱 ����
    HANDLE UniqueProcessId;        // ������ ���μ��� ID
    HANDLE InheritedFromUniqueProcessId; // ��ӵ� ���μ��� ID
    ULONG HandleCount;             // �ڵ� ����
    ULONG SessionId;               // ���� ID
    ULONG_PTR UniqueProcessKey;    // ������ ���μ��� Ű
    SIZE_T PeakVirtualSize;        // �ִ� ���� �޸� ũ��
    SIZE_T VirtualSize;            // ���� ���� �޸� ũ��
    ULONG PageFaultCount;          // ������ ��Ʈ Ƚ��
    SIZE_T PeakWorkingSetSize;     // �ִ� ��ŷ �� ũ��
    SIZE_T WorkingSetSize;         // ���� ��ŷ �� ũ��
    SIZE_T QuotaPeakPagedPoolUsage; // �������� Ǯ �ִ� ��뷮
    SIZE_T QuotaPagedPoolUsage;    // �������� Ǯ ��뷮
    SIZE_T QuotaPeakNonPagedPoolUsage;  // ���������� Ǯ �ִ� ��뷮
    SIZE_T QuotaNonPagedPoolUsage; // ���������� Ǯ ��뷮
    SIZE_T PagefileUsage;          // ������ ���� ��뷮
    SIZE_T PeakPagefileUsage;      // ������ ���� �ִ� ��뷮
    SIZE_T PrivatePageCount;       // �����̺� ������ ��
    LARGE_INTEGER ReadOperationCount;  // �б� �۾� Ƚ��
    LARGE_INTEGER WriteOperationCount; // ���� �۾� Ƚ��
    LARGE_INTEGER OtherOperationCount; // ��Ÿ �۾� Ƚ��
    LARGE_INTEGER ReadTransferCount;   // �б� ���� Ƚ��
    LARGE_INTEGER WriteTransferCount;  // ���� ���� Ƚ��
    LARGE_INTEGER OtherTransferCount;  // ��Ÿ ���� Ƚ��
} SYSTEM_PROCESS_INFORMATION, * PSYSTEM_PROCESS_INFORMATION;

// ZwQuerySystemInformation: �ý��� ������ �����ϴ� �Լ� (NTSTATUS ��ȯ)
NTSYSCALLAPI
NTSTATUS
ZwQuerySystemInformation(
    ULONG SystemInformationClass,    // �ý��� ���� Ŭ����
    PVOID SystemInformation,         // �ý��� ���� ����
    ULONG SystemInformationLength,   // �ý��� ���� ���� ũ��
    PULONG ReturnLength              // ��ȯ�Ǵ� ���� ����
);

// ZwQueryInformationProcess: ���μ��� ������ �����ϴ� �Լ�
NTSYSCALLAPI
NTSTATUS
ZwQueryInformationProcess(
    HANDLE ProcessHandle,            // ���μ��� �ڵ�
    PROCESSINFOCLASS ProcessInformationClass, // ���μ��� ���� Ŭ����
    PVOID ProcessInformation,        // ���μ��� ���� ����
    ULONG ProcessInformationLength,  // ���� ũ��
    PULONG ReturnLength              // ��ȯ�Ǵ� ���� ����
);

// ZwQueryVirtualMemory: ���μ��� ���� �޸� ������ �����ϴ� �Լ�
NTSYSCALLAPI
NTSTATUS
ZwQueryVirtualMemory(
    HANDLE ProcessHandle,            // ���μ��� �ڵ�
    PVOID BaseAddress,               // ���� �ּ�
    MEMORY_INFORMATION_CLASS MemoryInformationClass, // �޸� ���� Ŭ����
    PVOID MemoryInformation,         // �޸� ���� ����
    SIZE_T MemoryInformationLength,  // ���� ũ��
    PSIZE_T ReturnLength             // ��ȯ�Ǵ� ���� ����
);


VOID DriverUnload(PDRIVER_OBJECT pDriverObject);
DWORD GetProcessIDByName(PCWSTR szProcessName);
BOOLEAN IsDllInWhitelist(UNICODE_STRING* dllName);
INT ListDllsInProcess(HANDLE hProcess);
VOID ProcessListKill(DWORD AC_PID);
NTSTATUS TerminateProcessByPID(DWORD dwPID);
VOID PrintDllListByPID(DWORD dwPID);
VOID PeriodicTask(PVOID context);
NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath);

VOID DriverUnload(PDRIVER_OBJECT pDriverObject)
{
    UNREFERENCED_PARAMETER(pDriverObject); // pDriverObject�� ������ ������ ����Ͽ� ��� �����մϴ�.
    DbgPrint("Driver Unload\n"); // ����� ������� ����̹��� ��ε�ȴٴ� �޽����� ����մϴ�.
}

DWORD GetProcessIDByName(PCWSTR szProcessName)
{
    NTSTATUS status; // NTSTATUS ������ �����Ͽ� ZwQuerySystemInformation�� ��� ���¸� ������ ���Դϴ�.
    ULONG bufferSize = 0x10000; // ���� ũ�⸦ 64KB�� �����մϴ�. �̴� ���μ��� ������ �����ϱ� �����Դϴ�.
    PVOID buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, bufferSize, 'proc'); // ������¡ �޸� Ǯ���� 64KB ũ���� �޸𸮸� �Ҵ��մϴ�.
    DWORD dwPID = 0; // ã�� ���μ����� PID�� ������ �����Դϴ�. �ʱⰪ�� 0�Դϴ�.

    if (!buffer) {
        DbgPrint("Failed to allocate buffer\n"); // �޸� �Ҵ翡 �����ϸ�, ����� ������� ���� �޽����� ����ϰ� �Լ� ����
        return 0;
    }

    status = ZwQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, &bufferSize);
    // �ý����� ��� ���μ��� ������ �����ɴϴ�.

    if (status == STATUS_INFO_LENGTH_MISMATCH) {
        // ���� ���۰� ������� �ʴٸ�, ���ο� ���� ũ�⸦ �Ҵ��� �ٽ� �õ��մϴ�.
        ExFreePool(buffer); // ���� ���۸� �����մϴ�.
        buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, bufferSize, 'proc'); // ���ο� ũ���� ���۸� �Ҵ��մϴ�.
        if (!buffer) {
            DbgPrint("Failed to allocate new buffer\n"); // ���� �Ҵ� ���� �� ���� �޽����� ����ϰ� �Լ� ����
            return 0;
        }
        status = ZwQuerySystemInformation(SystemProcessInformation, buffer, bufferSize, &bufferSize);
        // �� ���� ũ��� �ٽ� ���μ��� ������ ��û�մϴ�.
    }

    if (NT_SUCCESS(status)) { // ZwQuerySystemInformation ȣ���� �����ϸ�
        PSYSTEM_PROCESS_INFORMATION processInfo = (PSYSTEM_PROCESS_INFORMATION)buffer; // ��ȯ�� ���۸� ���μ��� ���� ����ü�� ĳ����

        while (TRUE) { // ��� ���μ����� ��ȸ�ϸ鼭 Ž��
            if (processInfo->ImageName.Buffer != NULL &&
                _wcsicmp(processInfo->ImageName.Buffer, szProcessName) == 0) {
                // ���μ��� �̸��� ��ġ�ϴ��� Ȯ�� (��ҹ��� ����)
                dwPID = (DWORD)(ULONG_PTR)processInfo->UniqueProcessId; // ��ġ�ϸ� �ش� ���μ����� PID�� ����
                DbgPrint("Found process: %ws with PID: %d\n", processInfo->ImageName.Buffer, dwPID);
                // ���μ��� �̸��� PID�� ���
                break; // ���μ����� ã�����Ƿ� ���� ����
            }

            if (processInfo->NextEntryOffset == 0) {
                // �� �̻� ���μ��� ������ ������ ������ �����մϴ�.
                break;
            }

            processInfo = (PSYSTEM_PROCESS_INFORMATION)((PUCHAR)processInfo + processInfo->NextEntryOffset);
            // ���� ���μ��� ������ �̵��մϴ�.
        }
    }

    ExFreePool(buffer); // ���۸� �����մϴ�.
    return dwPID; // ã�� ���μ����� PID�� ��ȯ�մϴ�. ã�� ������ ��� 0�� ��ȯ�մϴ�.
}

BOOLEAN IsDllInWhitelist(UNICODE_STRING* dllName)
{
    static const wchar_t* whitelistedDlls[] = {
        // ���� DLL ���ϵ��� ��� ����� �����մϴ�. 
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\ac_client.exe",
        L"\\Device\\HarddiskVolume3\\Windows\\SYSTEM32\\ntdll.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\System32\\wow64.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\System32\\wow64win.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\System32\\wow64cpu.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\ntdll.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\KERNEL32.DLL",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\KERNELBASE.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\WS2_32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\RPCRT4.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\USER32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\win32u.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\GDI32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\gdi32full.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\msvcp_win.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\ucrtbase.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\ADVAPI32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\msvcrt.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\sechost.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\bcrypt.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\SHELL32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\OpenAL32.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\zlib1.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\OPENGL32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\combase.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\SDL2.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\IMM32.DLL",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\ole32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\OLEAUT32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\SETUPAPI.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\cfgmgr32.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\SDL2_image.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\WINMM.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\dbghelp.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\GLU32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\VERSION.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\libvorbisfile.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\ogg.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\libvorbis.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\shcore.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\uxtheme.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\MSCTF.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\kernel.appcore.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\bcryptPrimitives.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\clbcatq.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\vm3dgl.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\vm3dglhelper.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\IME\\IMEKR\\imkrtip.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\PROPSYS.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\IME\\shared\\imetip.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\OLEACC.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\DUI70.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\textinputframework.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\CoreMessaging.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\CoreUIComponents.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\ntmarta.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\wintypes.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\SHLWAPI.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\IME\\IMEKR\\imkrapi.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\policymanager.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\msvcp110_win.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\IME\\shared\\imjkapi.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\libpng16-16.dll",
        L"\\Device\\HarddiskVolume3\\Program Files (x86)\\AssaultCube 1.3.0.2\\bin_win32\\libjpeg-9.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\dwmapi.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\dxgi.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\d3d11.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\vm3dum_loader.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\vm3dum_10.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\dxcore.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\winmmbase.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\MMDevAPI.DLL",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\DEVOBJ.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\wdmaud.drv",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\ksuser.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\AVRT.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\AUDIOSES.DLL",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\powrprof.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\UMPDC.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\msacm32.drv",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\MSACM32.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\midimap.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\dsound.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\resourcepolicyclient.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\wrap_oal.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\Windows.UI.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\WindowManagementAPI.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\InputHost.dll",
        L"\\Device\\HarddiskVolume3\\Windows\\SysWOW64\\twinapi.appcore.dll"
    };

    for (INT i = 0; i < sizeof(whitelistedDlls) / sizeof(whitelistedDlls[0]); i++) {
        // ���� DLL ����� ��ȸ�ϸ鼭
        if (_wcsicmp(dllName->Buffer, whitelistedDlls[i]) == 0) {
            // ���޵� DLL ��ο� ��ġ�ϴ� ���� DLL�� ������ TRUE�� ��ȯ�մϴ�.
            return TRUE;
        }
    }
    return FALSE; // ���� DLL ��Ͽ� ������ FALSE�� ��ȯ�մϴ�.
}

INT ListDllsInProcess(HANDLE hProcess)
{
    NTSTATUS status; // NTSTATUS ������ �����Ͽ� ZwQueryVirtualMemory�� ���¸� ������ ���Դϴ�.
    PVOID baseAddress = 0; // ���μ��� ���� ���� �޸� ���̽� �ּҸ� 0���� �ʱ�ȭ�մϴ�.
    MEMORY_BASIC_INFORMATION memInfo; // �޸��� �⺻ ������ ������ ����ü�� �����մϴ�.
    ULONG resultLength; // ZwQueryVirtualMemory ȣ���� ��� ���̸� ������ ����
    INT Alert_CNT = 0; // ������ ���� DLL�� ������ ������ ����

    while (NT_SUCCESS(ZwQueryVirtualMemory(hProcess, baseAddress, MemoryBasicInformation, &memInfo, sizeof(memInfo), &resultLength))) {
        // ���μ����� �޸� ������ ��ȸ�մϴ�. �����ϸ� �ݺ�
        if (memInfo.Type == MEM_IMAGE) {
            // �ش� �޸� ������ �̹���(��, DLL)���� Ȯ���մϴ�.
            PVOID buffer = ExAllocatePool2(POOL_FLAG_NON_PAGED, 512, 'dlls');
            // DLL ��� ������ ������ 512����Ʈ ũ���� �޸� ���۸� �Ҵ��մϴ�.
            if (!buffer) {
                DbgPrint("Failed to allocate buffer\n"); // ���� �Ҵ� ���� �� ���� �޽����� ����ϰ� �Լ� ����
                return;
            }

            status = ZwQueryVirtualMemory(hProcess, memInfo.AllocationBase, MemorySectionName, buffer, 512, &resultLength);
            // �ش� �޸� ������ ���� �̸�(��, DLL ���)�� ��ȸ�մϴ�.
            if (NT_SUCCESS(status)) {
                UNICODE_STRING* dllName = (UNICODE_STRING*)buffer;
                // ���۸� UNICODE_STRING Ÿ������ ĳ�����Ͽ� DLL �̸��� Ȯ���մϴ�.
                if (dllName->Buffer != NULL) {
                    if (!IsDllInWhitelist(dllName)) {
                        DbgPrint("Alert! Unrecognized DLL loaded: %wZ\n", dllName);
                        // ������ ���� DLL�� �ε�Ǿ����� ��� �޽����� ����ϰ� ī��Ʈ ����
                        Alert_CNT += 1;
                    }
                    else {
                        DbgPrint("Loaded DLL: %wZ\n", dllName); // ���� DLL�� �ε� �޽����� ���
                    }
                }
            }
            ExFreePool(buffer); // ����� ���۸� �����մϴ�.
        }
        baseAddress = (PVOID)((ULONG_PTR)memInfo.BaseAddress + memInfo.RegionSize);
        // ���� �޸� �������� �̵��Ͽ� ��� �˻��մϴ�.
    }
    return Alert_CNT; // ������ ���� DLL�� ������ ��ȯ�մϴ�.
}

VOID ProcessListKill(DWORD AC_PID)
{
    PCWSTR szProcessNameList[ProcessListLength] = {
        L"cheatengine-x86_64-SSE4-AVX2.exe",
        L"x32dbg.exe",
        L"x64dbg.exe",
        L"x32dbg-unsigned.exe",
        L"x64dbg-unsigned.exe"
    };
    // ������ ��� ���μ��� �̸����� ����� �����մϴ�.
    int KillProcessCnt = 0; // ������ ���μ����� ���� ī��Ʈ�� �����Դϴ�.
    for (int i = 0; i < ProcessListLength; i++) {
        DWORD dwPID = 0; // ���μ��� ID�� ������ �����Դϴ�.
        dwPID = GetProcessIDByName(szProcessNameList[i]);
        // ���ǵ� �� ���μ��� �̸��� ������� PID�� ã���ϴ�.
        if (dwPID != 0) {
            TerminateProcessByPID(dwPID);
            // PID�� ��ȿ�ϸ� �ش� ���μ����� �����մϴ�.
            DbgPrint("Debugging Process Name:%ws\nKill Process ID : %d\n", szProcessNameList[i], dwPID);
            // ����� ������� ������ ���μ��� �̸��� PID�� ����մϴ�.
            KillProcessCnt++;
        }
    }
    if (KillProcessCnt != 0) {
        // ���� ����� ���μ����� ������ ���� ���μ����� �����մϴ�.
        TerminateProcessByPID(AC_PID);
        DbgPrint("Main Process Name:ac_client.exe\nKill Process ID : %d\n", AC_PID);
    }
    return;
}

NTSTATUS TerminateProcessByPID(DWORD dwPID)
{
    NTSTATUS status;  // �Լ��� ��ȯ ���� �ڵ� ����
    HANDLE hProcess;  // ���μ��� �ڵ� ����
    OBJECT_ATTRIBUTES objAttrs;  // ������Ʈ �Ӽ� ����ü
    CLIENT_ID clientId;  // Ŭ���̾�Ʈ ID ����ü�� ���μ��� �� ������ ID ����

    // ������Ʈ �Ӽ� �ʱ�ȭ
    InitializeObjectAttributes(&objAttrs, NULL, 0, NULL, NULL);
    clientId.UniqueProcess = (HANDLE)(ULONG_PTR)dwPID;  // PID�� Ŭ���̾�Ʈ ID�� ����
    clientId.UniqueThread = NULL;  // ������ ID�� ������� ����

    // PROCESS_TERMINATE �������� ���μ��� �ڵ��� ����
    status = ZwOpenProcess(&hProcess, PROCESS_TERMINATE, &objAttrs, &clientId);

    // ���μ����� ���������� �������� Ȯ��
    if (NT_SUCCESS(status)) {
        // ���μ����� ����
        status = ZwTerminateProcess(hProcess, STATUS_SUCCESS);

        // ���ῡ �����ߴ��� Ȯ��
        if (NT_SUCCESS(status)) {
            DbgPrint("Successfully terminated process with PID: %d\n", dwPID);  // ���� �޽��� ���
        }
        else {
            DbgPrint("Failed to terminate process with PID: %d\n", dwPID);  // ���� �޽��� ���
        }

        // ���μ��� �ڵ� �ݱ�
        ZwClose(hProcess);
    }
    else {
        DbgPrint("Failed to open process with PID: %d\n", dwPID);  // ���μ����� ���� ������ �� �޽��� ���
    }

    // �Լ��� ���� ���¸� ��ȯ
    return status;
}

VOID PrintDllListByPID(DWORD dwPID)
{
    HANDLE hProcess;  // ���μ��� �ڵ� ����
    OBJECT_ATTRIBUTES objAttrs;  // ������Ʈ �Ӽ� ����ü
    CLIENT_ID clientId;  // Ŭ���̾�Ʈ ID ����ü
    NTSTATUS status;  // �Լ��� ��ȯ ���� �ڵ� ����
    INT Alert_CNT = 0;  // ��� ī���� ���� �ʱ�ȭ

    // ������Ʈ �Ӽ� �ʱ�ȭ
    InitializeObjectAttributes(&objAttrs, NULL, 0, NULL, NULL);
    clientId.UniqueProcess = (HANDLE)(ULONG_PTR)dwPID;  // PID�� Ŭ���̾�Ʈ ID�� ����
    clientId.UniqueThread = NULL;  // ������ ID�� ������� ����

    // PROCESS_QUERY_INFORMATION �� PROCESS_VM_READ �������� ���μ��� �ڵ��� ����
    status = ZwOpenProcess(&hProcess, PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, &objAttrs, &clientId);

    // ���μ����� ���������� �������� Ȯ��
    if (NT_SUCCESS(status)) {
        // �ش� ���μ������� �ε�� DLL ����� ����ϰ� ��� ���� �޾ƿ�
        Alert_CNT = ListDllsInProcess(hProcess);

        // ��� �߻��� ��� ���μ��� ����
        if (Alert_CNT != 0) {
            TerminateProcessByPID(dwPID);
        }

        // ���μ��� �ڵ� �ݱ�
        ZwClose(hProcess);
    }
    else {
        DbgPrint("Failed to open process with PID: %d\n", dwPID);  // ���μ����� ���� ������ �� �޽��� ���
    }
}

VOID PeriodicTask(PVOID context)
{
    UNREFERENCED_PARAMETER(context);  // �Ű������� ������� �����Ƿ� ����

    while (TRUE) {  // ���� ����
        // Ư�� ���μ��� �̸��� ���� ���μ����� PID�� ���
        DWORD dwPID = GetProcessIDByName(L"ac_client.exe");
        DbgPrint("Process ID: %d\n", dwPID);  // PID ���

        // PID�� 0�� �ƴϸ� (��, �ش� ���μ����� �����ϸ�) ó�� ����
        if (dwPID != 0) {
            ProcessListKill(dwPID);  // �ش� PID�� ���μ����� ����
            PrintDllListByPID(dwPID);  // �ش� PID�� ���μ������� DLL ����� ����ϰ� �ʿ��� ��� ����
        }

        // 30�� ��� (100ns ������ 30��)
        LARGE_INTEGER interval;
        interval.QuadPart = -300000000LL;  // ���� ���� ������� ��⸦ �ǹ�
        KeDelayExecutionThread(KernelMode, FALSE, &interval);  // 30�� ���
    }
}

NTSTATUS DriverEntry(PDRIVER_OBJECT pDriverObject, PUNICODE_STRING pRegistryPath)
{
    UNREFERENCED_PARAMETER(pRegistryPath);  // �Ű������� ������� �����Ƿ� ����

    DbgPrint("DriverEntry\n");  // ����̹��� �ε�Ǿ����� ���

    pDriverObject->DriverUnload = DriverUnload;  // ����̹� ��ε� �Լ� ����

    HANDLE threadHandle;  // �ý��� ������ �ڵ� ����
    // �ý��� �����带 �����ϰ� PeriodicTask �Լ��� ����ǵ��� ����
    NTSTATUS status = PsCreateSystemThread(&threadHandle, THREAD_ALL_ACCESS, NULL, NULL, NULL, PeriodicTask, NULL);

    // ������ ������ �����ߴ��� Ȯ��
    if (!NT_SUCCESS(status)) {
        DbgPrint("Failed to create system thread\n");  // ���� �޽��� ���
        return status;  // ���� ���� ��ȯ
    }

    // ������ �ڵ� �ݱ�
    ZwClose(threadHandle);

    // ����̹� �ʱ�ȭ ���� ��ȯ
    return STATUS_SUCCESS;
}