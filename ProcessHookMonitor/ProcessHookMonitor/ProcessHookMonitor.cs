using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading;
using System.Threading.Tasks;

namespace ProcessHookMonitor
{

    public delegate void FunctionCalledHandler(int pid, string functionName, string param);
    
    public delegate void MessageHandler(int pid, string msg);

    public class ProcessHookMonitor
    {
        public const int PROCESS_HOOK_MONITOR_CODEID = -1;
        private static readonly string appWorkPath = Path.GetTempPath() + "ant.ram.temp";

        private static Dictionary<int, FunctionCalledHandler> processListeners = new Dictionary<int, FunctionCalledHandler>();
        private static MessageHandler messageHanlder = new MessageHandler(doNothingMessageHandler);
        private static string channelName = null;
        private static bool serverUp = false;
        private const string dllInjectionName = "ProcessHook.dll";

        class InjectTask
        {
            int pid;
            FunctionCalledHandler h;
            string injectionLibrary;

            int retVal;
            string ErrorMsg = "";

            public InjectTask(int pid, string injectionLibrary, FunctionCalledHandler h)
            {
                this.h = h;
                this.pid = pid;
                this.injectionLibrary = injectionLibrary;
            }

            public void run()
            {

                try
                {
                    reportStatus(PROCESS_HOOK_MONITOR_CODEID, "Attempting to inject into process " + pid);
                    // Console.WriteLine("Attempting to inject into process {0}", pid);
                    // inject into existing process
                    EasyHook.RemoteHooking.Inject(
                        pid,          // ID of process to inject into
                        injectionLibrary,   // 32-bit library to inject (if target is 32-bit)
                        injectionLibrary,   // 64-bit library to inject (if target is 64-bit)
                        channelName         // the parameters to pass into injected library
                                            // ...
                    );
                        //register listener
                        setFunctionListener(pid, h);
                }
                catch (Exception e)
                {
                    reportStatus(PROCESS_HOOK_MONITOR_CODEID, "There was an error while injecting into target (pid: " + pid + "):\n" + e.Message);
                    //Console.ForegroundColor = ConsoleColor.Red;
                    //Console.WriteLine("There was an error while injecting into target:");
                    //Console.ResetColor();
                    //Console.WriteLine(e.ToString());

                    retVal = -1;
                }

                //successful injection
                retVal = 0;
            }

            public int getRetval()
            {
                return retVal;
            }
        }

        public static void initialize()
        {
            CopyDir("utils", Path.GetTempPath() + "ant.ram.temp\\utils");
        }

        public static string getWorkPath()
        {
            return appWorkPath;
        }

        private static void CopyDir(string sourceDirectory, string targetDirectory)
        {
            DirectoryInfo diSource = new DirectoryInfo(sourceDirectory);
            DirectoryInfo diTarget = new DirectoryInfo(targetDirectory);

            CopyAll(diSource, diTarget);
        }

        private static void CopyAll(DirectoryInfo source, DirectoryInfo target)
        {
            Directory.CreateDirectory(target.FullName);

            // Copy each file into the new directory.
            foreach (FileInfo fi in source.GetFiles())
            {
                //Console.WriteLine(@"Copying {0}\{1}", target.FullName, fi.Name);
                fi.CopyTo(Path.Combine(target.FullName, fi.Name), true);
            }

            // Copy each subdirectory using recursion.
            foreach (DirectoryInfo diSourceSubDir in source.GetDirectories())
            {
                DirectoryInfo nextTargetSubDir =
                    target.CreateSubdirectory(diSourceSubDir.Name);
                CopyAll(diSourceSubDir, nextTargetSubDir);
            }
        }

        public static void setStatusHandler(MessageHandler listener)
        {
            messageHanlder = listener;
        }
        
        public static void setFunctionListener(int pid, FunctionCalledHandler listener)
        {
            processListeners[pid] = listener;
        }

        public static void removeFunctionListener(int pid)
        {
            if (processListeners.ContainsKey(pid))
            {
                processListeners[pid] = new FunctionCalledHandler(doNothingFunctionCalledHandler);
            }
        }

        private static void setupServer()
        {
            if (!serverUp)
            {
                EasyHook.RemoteHooking.IpcCreateServer<HookComServer>(ref channelName, System.Runtime.Remoting.WellKnownObjectMode.Singleton);
            }

            serverUp = true;
        }

        public static int inject(int pid, FunctionCalledHandler listener)
        {
            setupServer();

            // Get the full path to the assembly we want to inject into the target process
            string injectionLibrary = Path.Combine(Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location), dllInjectionName);

            InjectTask injectTask = new InjectTask(pid, injectionLibrary, listener);
            Thread task = new Thread(injectTask.run);
            task.Start();
            task.Join(2000);
            if (task.IsAlive)
            {
                task.Abort();
                return -2;
            }
            else
            {
                return injectTask.getRetval();
            }

            /* ==== SERIAL CODE ===
            try
            {
                reportStatus(PROCESS_HOOK_MONITOR_CODEID, "Attempting to inject into process " + pid);
                // Console.WriteLine("Attempting to inject into process {0}", pid);
                // inject into existing process
                Thread tInject = new Thread(new ThreadStart(() =>
                {
                    EasyHook.RemoteHooking.Inject(
                        pid,          // ID of process to inject into
                        injectionLibrary,   // 32-bit library to inject (if target is 32-bit)
                        injectionLibrary,   // 64-bit library to inject (if target is 64-bit)
                        channelName         // the parameters to pass into injected library
                                            // ...
                    );
                }));

                tInject.Start();
                tInject.Join(2000);
                if (tInject.IsAlive)
                {
                    tInject.Abort();
                    return -2;
                }
                else
                {
                    //register listener
                    setFunctionListener(pid, listener);
                }
            }
            catch (Exception e)
            {
                reportStatus(PROCESS_HOOK_MONITOR_CODEID, "There was an error while injecting into target (pid: " + pid + "):\n" + e.Message);
                //Console.ForegroundColor = ConsoleColor.Red;
                //Console.WriteLine("There was an error while injecting into target:");
                //Console.ResetColor();
                //Console.WriteLine(e.ToString());

                return -1;
            }

            //successful injection
            return 0;
            */
        }

        /*
         * This section is used by the server to notify about events and messages
         */
        public static void notifyFunctionCall(int pid, string functionCalledName, string param)
        {
            if (processListeners.ContainsKey(pid))
            {
                FunctionCalledHandler listener = processListeners[pid];
                listener(pid, functionCalledName, param);
            }
        }

        public static void reportStatus(int pid, string message)
        {
            messageHanlder(pid, message);
        }

        private static void doNothingMessageHandler(int pid, string str)
        {
            //empty on purpose. meant to be default behavior when no listener exist
        }

        private static void doNothingFunctionCalledHandler(int pid, string str, string param)
        {
            //empty on purpose. meant to be default behavior when no listener exist
        }

        public static void close()
        {
            DirectoryInfo target = new DirectoryInfo(Path.GetTempPath() + "ant.ram.temp");
            Directory.Delete(target.FullName, true);
        }
    }

    
}
