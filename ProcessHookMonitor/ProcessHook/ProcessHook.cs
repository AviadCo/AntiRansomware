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

        private static IHookServer server = null;
        private static Queue<string> eventsQueue = new Queue<string>();

        /// <summary>
        /// creates function hook objects
        /// </summary>
        /// <returns></returns>
        private List<EasyHook.LocalHook> createFunctionsHooks()
        {
            List<EasyHook.LocalHook> resHooks = new List<EasyHook.LocalHook>();

            // Install hooks

            try
            {
                // CreateFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx
                var createFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.CreateFileWStr),
                    new FunctionHooks.CreateFileW_Delegate(FunctionHooks.CreateFileW_Hook),
                    this);
                resHooks.Add(createFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.CreateFileWStr);
            }

            try
            {
                // WriteFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365747(v=vs.85).aspx
                var writeFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.WriteFileStr),
                    new FunctionHooks.WriteFile_Delegate(FunctionHooks.WriteFile_Hook),
                    this);
                resHooks.Add(writeFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.WriteFileStr);
            }

            try
            {
                // DeleteFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa363915(v=vs.85).aspx
                var deleteFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.DeleteFileWStr),
                    new FunctionHooks.DeleteFileW_Delegate(FunctionHooks.DeleteFileW_Hook),
                    this);
                resHooks.Add(deleteFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.DeleteFileWStr);
            }

            try
            {
                // MoveFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365239(v=vs.85).aspx
                var moveFileHook = EasyHook.LocalHook.Create(
                    EasyHook.LocalHook.GetProcAddress("kernel32.dll", FunctionHooks.MoveFileWStr),
                    new FunctionHooks.MoveFileW_Delegate(FunctionHooks.MoveFileW_Hook),
                    this);
                resHooks.Add(moveFileHook);
            }
            catch (System.MissingMethodException)
            {
                reportStatus(ERROR_CANT_FIND_HOOK + FunctionHooks.MoveFileWStr);
            }

            try
            {
                // MoveFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365239(v=vs.85).aspx
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


            /*
            // ReadFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
            var readFileHook = EasyHook.LocalHook.Create(
                EasyHook.LocalHook.GetProcAddress("kernel32.dll", "ReadFile"),
                new ReadFile_Delegate(ReadFile_Hook),
                this);

            */

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

            reportStatus("function hooks installed successfully");
            

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
