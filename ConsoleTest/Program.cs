using SocketIO;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace ConsoleTest
{
    class Program
    {
        static void Main(string[] args)
        {
            try
            {
                new Program().Run();
            }
            catch (Exception e)
            {
                Debug.WriteLine(e);
            }
        }

        private void Run()
        {
            using (var client = new Client())
            {
                client.Connect("http://localhost:3000");
                using (Socket socket = client.GetSocket())
                {
                    RunAsyncTest(tcs =>
                    {
                        socket.On("test2", () => Console.WriteLine("Action Called!"));
                        socket.On<string>("test3", response => tcs.TrySetResult(null));
                        socket.Emit("test");
                    })
                    .Wait();
                }
            }    
        }

        private async Task RunAsyncTest(Action<TaskCompletionSource<object>> action)
        {
            var tcs = new TaskCompletionSource<object>();
            try
            {
                action(tcs);
            }
            catch (Exception e)
            {
                tcs.TrySetResult(null);
            }
            await tcs.Task;
        }
    }
}
