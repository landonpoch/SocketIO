using System;
using Microsoft.VisualStudio.TestTools.UnitTesting;

namespace SocketIO.Test
{
    [TestClass]
    public class UnitTest1
    {
        [TestMethod]
        public void TestMethod1()
        {
            using (var client = new Client())
            {
                client.Connect("");
                using (Socket socket = client.GetSocket())
                {
                    socket.On("", () =>
                    {

                    });

                    socket.On<UnitTest1>("", item =>
                    {

                    });

                    socket.Emit("");
                }
            }
        }
    }
}
