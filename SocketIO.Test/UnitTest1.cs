using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;
using System.Threading;
using System.Threading.Tasks;

namespace SocketIO.Test
{
    [TestClass]
    public class UnitTest1 : TestBase
    {   
        private void TestMethod1(TaskCompletionSource<object> tcs)
        {
            using (var client = new Client())
            {
                client.Connect("http://localhost:3000");
                using (Socket socket = client.GetSocket())
                {
                    socket.On("test2", () => Console.WriteLine("Action Called!"));

                    socket.On<string>("", response => tcs.TrySetResult(null));

                    socket.Emit("test");
                }
            }
        }

        #region TestMethod Entry Points

        [TestMethod] public async Task TestMethod1() { await RunAsyncTest(TestMethod1); }

        #endregion
    }

    public abstract class TestBase
    {
        protected async Task RunAsyncTest(Action<TaskCompletionSource<object>> action)
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
