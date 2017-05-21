using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHookMonitor
{
    class HookComServer : ProcessHookCommunication.IHookServer
    {
        public override void Ping()
        {
            
        }

        public override void ReportEvents(int pid, string[] events)
        {
            for (int i = 0; i < events.Length; i++)
            {
                ProcessHookMonitor.notifyFunctionCall(pid, events[i]);
            }

        }

        public override void ReportException(int pid, Exception e)
        {
            ProcessHookMonitor.reportStatus(pid, e.Message);
        }

        public override void ReportStatus(int pid, string message)
        {
            ProcessHookMonitor.reportStatus(pid, message);
        }
    }
}
