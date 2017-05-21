using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ProcessHookCommunication
{
    public abstract class IHookServer : MarshalByRefObject
    {
        public const string demiliter = "|? ! P|";

        /// <summary>
        /// Called to confirm that the IPC channel is still open / host application has not closed
        /// </summary>
        public abstract void Ping();

        /// <summary>
        /// Report exception
        /// </summary>
        /// <param name="e"></param>
        public abstract void ReportException(int pid, Exception e);

        /// <summary>
        /// Report about events that occured in the procces 
        /// </summary>
        /// <param name="clientPID"></param>
        /// <param name="messages"></param>
        public abstract void ReportEvents(int pid, string[] events);

        public abstract void ReportStatus(int pid, string message);
    }
}
