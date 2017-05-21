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

        private IHookServer server = null;
        private static Queue<string> eventsQueue = new Queue<string>();

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
            // Injection is now complete and the server interface is connected
            server.ReportStatus(currentPid, "Hook installed successfully");

            // Install hooks
             
            // CreateFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa363858(v=vs.85).aspx
            var createFileHook = EasyHook.LocalHook.Create(
                EasyHook.LocalHook.GetProcAddress("kernel32.dll", "CreateFileW"),
                new FunctionHooks.CreateFileWDelegate(FunctionHooks.CreateFileWHook),
                this);

            /*
            // ReadFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365467(v=vs.85).aspx
            var readFileHook = EasyHook.LocalHook.Create(
                EasyHook.LocalHook.GetProcAddress("kernel32.dll", "ReadFile"),
                new ReadFile_Delegate(ReadFile_Hook),
                this);

            // WriteFile https://msdn.microsoft.com/en-us/library/windows/desktop/aa365747(v=vs.85).aspx
            var writeFileHook = EasyHook.LocalHook.Create(
                EasyHook.LocalHook.GetProcAddress("kernel32.dll", "WriteFile"),
                new WriteFile_Delegate(WriteFile_Hook),
                this);

            */

            // Activate hooks on all threads except the current thread
            createFileHook.ThreadACL.SetExclusiveACL(new Int32[] { 0 });
            
            /*
            readFileHook.ThreadACL.SetExclusiveACL(new Int32[] { 0 });
            writeFileHook.ThreadACL.SetExclusiveACL(new Int32[] { 0 });
            */

            server.ReportStatus(currentPid, "function hooks installed successfully");
            

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
            
            createFileHook.Dispose();
            /*
            readFileHook.Dispose();
            writeFileHook.Dispose();
            */

            // Finalise cleanup of hooks
            EasyHook.LocalHook.Release();
        }

        public static void enqueueEvent(string hookEvent)
        {
            eventsQueue.Enqueue(hookEvent);
        }

    }
}
