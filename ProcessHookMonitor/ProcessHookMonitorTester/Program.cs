using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using ProcessHookMonitor;

namespace ProcessHookMonitorTester
{
    class Program
    {
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
            ProcessHookMonitor.ProcessHookMonitor.setStatusHandler(new MessageHandler(reportToConsole));
            ProcessHookMonitor.ProcessHookMonitor.inject(12076,
                new FunctionCalledHandler(reportToConsole));
            Console.WriteLine("hello");
            Console.ReadKey();
        }
    }
}
