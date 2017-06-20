using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ProcessHookMonitor;
using System.Threading;
using System.Diagnostics;


namespace ProcessHookMonitorTester
{
    public class InjectTask
    {
        int pid;
        FunctionCalledHandler h;

        public InjectTask(int pid, FunctionCalledHandler h)
        {
            this.h = h;
            this.pid = pid;
        }

        public void run()
        {
            ProcessHookMonitor.ProcessHookMonitor.inject(pid, h);
        }
    }

    class Program
    {

        static void reportToConsole(uint pid)
        {
            Console.WriteLine("stop pid: " + pid);
        }

        static void reportToConsole(uint pid, uint ppid)
        {
            Console.WriteLine("start pid: " +pid + " ppid: " + ppid);
        }

        static void reportToConsole(int pid, string name)
        {
            Console.WriteLine(pid + ": " + name + ", ");
        }

        static void reportToConsole(int pid, string name, string param)
        {
            Console.WriteLine(pid + ": " + name + ", " + param);
        }

        static void Main(string[] args)
        {
            //ProcessHookMonitor.ProcessTrace.listenProcessesCreation(new ProcessHookMonitor.ProcessStartEvent(reportToConsole));
            //ProcessHookMonitor.ProcessTrace.listenProcessesTermination(new ProcessHookMonitor.ProcessStopEvent(reportToConsole));

           
            ProcessHookMonitor.ProcessHookMonitor.initialize();
            ProcessHookMonitor.ProcessHookMonitor.setStatusHandler(new MessageHandler(reportToConsole));
            /*
           Process me = Process.GetCurrentProcess();

           Process[] all = Process.GetProcesses();
           int i = 0;
           int count = 0;
           foreach (Process p in all)
           {
               Console.WriteLine("## " + i);
               i++;
               if (p.Id != me.Id && p.Id != 4676 && !p.ProcessName.Equals("MicrosoftEdgeCP"))
               {
                   //InjectTask inj = new InjectTask(p.Id, new FunctionCalledHandler(reportToConsole));
                   //Thread t = new Thread(inj.run);
                   //t.Start();
                   //t.Join(2000);
                   //if (t.IsAlive)
                   //    t.Abort();
                   //else
                   //    count++;
                   if (ProcessHookMonitor.ProcessHookMonitor.inject(p.Id, new FunctionCalledHandler(reportToConsole)) == 0)
                       count++;
               }
           }*/
            //Console.WriteLine("#######################################################hello " + count);


            ProcessHookMonitor.ProcessHookMonitor.inject(9188,
                new FunctionCalledHandler(reportToConsole));
            Console.WriteLine("hello");
            Console.ReadKey();
            ProcessHookMonitor.ProcessHookMonitor.close();
        }
    }
}
