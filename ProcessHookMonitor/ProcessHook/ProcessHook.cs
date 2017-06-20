using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ProcessHookCommunication;

namespace ProcessHook
{
    public class ProcessHook : EasyHook.IEntryPoint

    {
        private const string ERROR_CANT_FIND_HOOK = "Error. Cant find function: ";
        private const string ERROR_CANT_FIND_DLL = "Error. Cant find DLL: ";

        private static IHookServer server = null;
        private static Queue<string> eventsQueue = new Queue<string>();

        private static int mainThreadId = EasyHook.RemoteHooking.GetCurrentThreadId();
        private static Dictionary<string, EasyHook.LocalHook> hooksMap;

        public static void disableThreadFromHook(string hookFunctionName, int threadID)
        {
            hooksMap[hookFunctionName].ThreadACL.SetExclusiveACL(new Int32[] { mainThreadId, threadID });
        }
        public static void resetThreadHooking(string hookFunctionName)
        {
            hooksMap[hookFunctionName].ThreadACL.SetExclusiveACL(new Int32 [] { mainThreadId });
        }

        /// <summary>
        /// creates function hook objects
        /// </summary>
        /// <returns></returns>
        private List<EasyHook.LocalHook> createFunctionsHooks()
        {
            List<EasyHook.LocalHook> resHooks = new List<EasyHook.LocalHook>();
            hooksMap = new Dictionary<string, EasyHook.LocalHook>();

            // Install hooks

            try
            {
                // CreateFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx
                var createFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.CreateFileWStr),
                    new FunctionHooks.CreateFileW_Delegate(FunctionHooks.CreateFileW_Hook),
                    this);
                resHooks.Add(createFileHook);
                hooksMap.Add(FunctionHooks.CreateFileWStr, createFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.CreateFileWStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "kernel32.dll");
            }

            
            try
            {
                // WriteFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365747(v=vs.85).aspx
                var writeFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.WriteFileStr),
                    new FunctionHooks.WriteFile_Delegate(FunctionHooks.WriteFile_Hook),
                    this);
                resHooks.Add(writeFileHook);
                hooksMap.Add(FunctionHooks.WriteFileStr, writeFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.WriteFileStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "kernel32.dll");
            }
            
            try
            {
                // DeleteFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa363915(v=vs.85).aspx
                var deleteFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.DeleteFileWStr),
                    new FunctionHooks.DeleteFileW_Delegate(FunctionHooks.DeleteFileW_Hook),
                    this);
                resHooks.Add(deleteFileHook);
                hooksMap.Add(FunctionHooks.DeleteFileWStr, deleteFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.DeleteFileWStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "kernel32.dll");
            }

            try
            {
                // MoveFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365239(v=vs.85).aspx
                var moveFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.MoveFileWStr),
                    new FunctionHooks.MoveFileW_Delegate(FunctionHooks.MoveFileW_Hook),
                    this);
                resHooks.Add(moveFileHook);
                hooksMap.Add(FunctionHooks.MoveFileWStr, moveFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.MoveFileWStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "kernel32.dll");
            }
            /*
            try
            {
                // CryptEncrypt https://msdn.microsoft.com/en-us/library/windows/desktop/aa365239(v=vs.85).aspx
                var cryptEncryptHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("Advapi32.dll", FunctionHooks.CryptEncryptStr),
                    new FunctionHooks.CryptEncrypt_Delegate(FunctionHooks.CryptEncrypt_Hook),
                    this);
                resHooks.Add(cryptEncryptHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.CryptEncryptStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "Advapi32.dll");
            }
            */
            try
            {
                // ShellExecuteExW https://msdn.microsoft.com/en-us/library/windows/desktop/bb762154(v=vs.85).aspx
                var shellExecuteExWHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("Shell32.dll", FunctionHooks.ShellExecuteExWStr),
                    new FunctionHooks.ShellExecuteExW_Delegate(FunctionHooks.ShellExecuteExW_Hook),
                    this);
                resHooks.Add(shellExecuteExWHook);
                hooksMap.Add(FunctionHooks.ShellExecuteExWStr, shellExecuteExWHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.ShellExecuteExWStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "Shell32.dll");
            }

            
            try
            {
                // WriteProcessMemory https://msdn.microsoft.com/en-us/library/windows/desktop/ms681674(v=vs.85).aspx
                var WriteProcessMemoryHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("Kernel32.dll", FunctionHooks.WriteProcessMemoryStr),
                    new FunctionHooks.WriteProcessMemory_Delegate(FunctionHooks.WriteProcessMemory_Hook),
                    this);
                resHooks.Add(WriteProcessMemoryHook);
                hooksMap.Add(FunctionHooks.WriteProcessMemoryStr, WriteProcessMemoryHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.WriteProcessMemoryStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "Kernel32.dll");
            }
            
            try
            {
                // CreateProcessW https://msdn.microsoft.com/en-us/library/windows/desktop/ms681674(v=vs.85).aspx
                var CreateProcessWHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("Kernel32.dll", FunctionHooks.CreateProcessWStr),
                    new FunctionHooks.CreateProcessW_Delegate(FunctionHooks.CreateProcessW_Hook),
                    this);
                resHooks.Add(CreateProcessWHook);
                hooksMap.Add(FunctionHooks.CreateProcessWStr, CreateProcessWHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.CreateProcessWStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "Kernel32.dll");
            }

            try
            {
                // CreateRemoteThread https://msdn.microsoft.com/en-us/library/windows/desktop/ms682437(v=vs.85).aspx
                var CreateRemoteThreadHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("Kernel32.dll", FunctionHooks.CreateRemoteThreadStr),
                    new FunctionHooks.CreateRemoteThread_Delegate(FunctionHooks.CreateRemoteThread_Hook),
                    this);
                resHooks.Add(CreateRemoteThreadHook);
                hooksMap.Add(FunctionHooks.CreateRemoteThreadStr, CreateRemoteThreadHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.CreateRemoteThreadStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "Kernel32.dll");
            }

            try
            {
                // CreateRemoteThreadEx https://msdn.microsoft.com/en-us/library/windows/desktop/ms682437(v=vs.85).aspx
                var CreateRemoteThreadExHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("Kernel32.dll", FunctionHooks.CreateRemoteThreadExStr),
                    new FunctionHooks.CreateRemoteThreadEx_Delegate(FunctionHooks.CreateRemoteThreadEx_Hook),
                    this);
                resHooks.Add(CreateRemoteThreadExHook);
                hooksMap.Add(FunctionHooks.CreateRemoteThreadExStr, CreateRemoteThreadExHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.CreateRemoteThreadExStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "Kernel32.dll");
            }

            
            try
            {
                // ReadFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
                var readFileHook = EasyHook.LocalHook.Create(
                EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.ReadFileStr),
                new FunctionHooks.ReadFile_Delegate(FunctionHooks.ReadFile_Hook),
                this);
                resHooks.Add(readFileHook);
                hooksMap.Add(FunctionHooks.ReadFileStr, readFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.ReadFileStr);
            }
            catch (System.DllNotFoundException)
            {
                reportStatus(ERROR_CANT_FIND_DLL + "Kernel32.dll");
            }
            

            return resHooks;
        }


        /// <summary>
        /// c'tor of process hook
        /// </summary>
        /// <param name="context">The RemoteHooking context</param>
        /// <param name="channelName">The name of the IPC channel</param>
        public ProcessHook(
            EasyHook.RemoteHooking.IContext context,
            string channelName)
        {
            // Connect to server object using provided channel name
            server = EasyHook.RemoteHooking.IpcConnectClient<IHookServer>(channelName);

            // If Ping fails then the Run method will be not be called
            server.Ping();
            server.ReportStatus(EasyHook.RemoteHooking.GetCurrentProcessId(), "dll init ok");

        }

        /// <summary>
        /// The main entry point for our logic once injected within the target process. 
        /// This is where the hooks will be created, and a loop will be entered until host process exits.
        /// EasyHook requires a matching Run method for the constructor
        /// </summary>
        /// <param name="context">The RemoteHooking context</param>
        /// <param name="channelName">The name of the IPC channel</param>
        public void Run(
            EasyHook.RemoteHooking.IContext context,
            string channelName)
        {
            int currentPid = EasyHook.RemoteHooking.GetCurrentProcessId();
            List<EasyHook.LocalHook> hooks;

            // Injection is now complete and the server interface is connected
            reportStatus("Hook installed successfully");

            // Install hooks
            hooks = createFunctionsHooks();

            // Activate hooks on all threads except the current thread
            foreach ( EasyHook.LocalHook hook in hooks)
            {
                hook.ThreadACL.SetExclusiveACL(new Int32[] { 0 });
            }


            /*
            createFileHook.ThreadACL.SetExclusiveACL(new Int32[] { 0 });
            writeFileHook.ThreadACL.SetExclusiveACL(new Int32[] { 0 });
            readFileHook.ThreadACL.SetExclusiveACL(new Int32[] { 0 });
            */

            reportStatus(hooks.Count + " function hooks installed successfully");
            

            try
            {
                // Loop until FileMonitor closes (i.e. IPC fails)
                while (true)
                {
                    
                    System.Threading.Thread.Sleep(500);

                    string[] queued = null;

                    lock (eventsQueue)
                    {
                        queued = eventsQueue.ToArray();
                        eventsQueue.Clear();
                    }

                    

                    // Send newly monitored file accesses to FileMonitor
                    if (queued != null && queued.Length > 0)
                    {
                        server.ReportEvents(currentPid, queued);
                    }
                    else
                    {
                        server.Ping();
                    }
                }
            }
            catch
            {
                // Ping() or ReportMessages() will raise an exception if host is unreachable
            }

            // Remove hooks
            foreach (EasyHook.LocalHook hook in hooks)
            {
                hook.Dispose();
            }
            /*
            createFileHook.Dispose();
            writeFileHook.Dispose();
            readFileHook.Dispose();
            */

            // Finalise cleanup of hooks
            EasyHook.LocalHook.Release();
        }

        public static void enqueueEvent(string hookEvent, string param)
        {
            lock (eventsQueue) {
                eventsQueue.Enqueue(hookEvent + IHookServer.demiliter + param);
            }
            
        }

        public static void reportStatus(string status)
        {
            server.ReportStatus(EasyHook.RemoteHooking.GetCurrentProcessId(), status);
        }

    }
}
