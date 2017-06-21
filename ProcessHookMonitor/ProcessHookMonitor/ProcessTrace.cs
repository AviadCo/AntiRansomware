using System;
using System.Management;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHookMonitor
{
    public delegate void ProcessStartEvent(uint pid, string processName, uint parentID);
    public delegate void ProcessStopEvent(uint pid);

    public class ProcessTrace
    {
        private static ManagementEventWatcher startWatch = null;
        private static ManagementEventWatcher stopWatch = null;
        private static ProcessStartEvent processStartHandler;
        private static ProcessStopEvent processStopHandler;

        public static void listenProcessesCreation(ProcessStartEvent handler)
        {
            processStartHandler = handler;
            ManagementEventWatcher startWatch = new ManagementEventWatcher(
                new WqlEventQuery("SELECT * FROM Win32_ProcessStartTrace"));
            startWatch.EventArrived += new EventArrivedEventHandler(startWatch_EventArrived);
            startWatch.Start();
        }
        public static void listenProcessesTermination(ProcessStopEvent handler)
        {
            processStopHandler = handler;
            ManagementEventWatcher stopWatch = new ManagementEventWatcher(
              new WqlEventQuery("SELECT * FROM Win32_ProcessStopTrace"));
            stopWatch.EventArrived += new EventArrivedEventHandler(stopWatch_EventArrived);
            stopWatch.Start();
        }

        public static void unlistenProcessesCreation()
        {
            if (startWatch != null)
            {
                startWatch.Stop();
                startWatch = null;
            }
                
        }
        public static void unlistenProcessesTermination()
        {
            if (stopWatch != null)
            {
                stopWatch.Stop();
                stopWatch = null;
            }
                
        }

        static void stopWatch_EventArrived(object sender, EventArrivedEventArgs e)
        {
            uint processId = (uint)e.NewEvent.GetPropertyValue("ProcessID");
            processStopHandler(processId);
        }

        static void startWatch_EventArrived(object sender, EventArrivedEventArgs e)
        {
            uint parentId = (uint)e.NewEvent.GetPropertyValue("ParentProcessID");
            uint processId = (uint)e.NewEvent.GetPropertyValue("ProcessID");
            string processName = (string)e.NewEvent.GetPropertyValue("ProcessName");

            processStartHandler(processId, processName, parentId);
        }
    }
}
