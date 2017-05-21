using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHookMonitor
{

    public delegate void FunctionCalledHandler(int pid, string functionName, string param);
    
    public delegate void MessageHandler(int pid, string msg);

    public class ProcessHookMonitor
    {
        private static Dictionary<int, FunctionCalledHandler> processListeners = new Dictionary<int, FunctionCalledHandler>();
        private static MessageHandler messageHanlder = new MessageHandler(doNothingMessageHandler);
        private static string channelName = null;
        private static bool serverUp = false;
        private const string dllInjectionName = "ProcessHook.dll";


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

            try
            {
                Console.WriteLine("Attempting to inject into process {0}", pid);
                // inject into existing process
                EasyHook.RemoteHooking.Inject(
                    pid,          // ID of process to inject into
                    injectionLibrary,   // 32-bit library to inject (if target is 32-bit)
                    injectionLibrary,   // 64-bit library to inject (if target is 64-bit)
                    channelName         // the parameters to pass into injected library
                                        // ...
                );
            }
            catch (Exception e)
            {
                Console.ForegroundColor = ConsoleColor.Red;
                Console.WriteLine("There was an error while injecting into target:");
                Console.ResetColor();
                Console.WriteLine(e.ToString());

                return -1;
            }

            //register listener
            setFunctionListener(pid, listener);

            //successful injection
            return 0;
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
    }

    
}
