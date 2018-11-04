using Uno;
using Uno.Net;
using Uno.Net.Sockets;
using Uno.Text;

namespace UdpApp
{
    public class UdpApp : Application
    {
        public UdpApp()
        {
            byte[] data = new byte[1024];
            IPEndPoint ipep = new IPEndPoint(IPAddress.Any, 30101);
            UdpClient newsock = new UdpClient(ipep);

            debug_log("Waiting for a client...");

            IPEndPoint sender = new IPEndPoint(IPAddress.Any, 0);

            data = newsock.Receive(ref sender);

            debug_log string.Format("Message received from {0}:", sender.ToString());
            debug_log(Utf8.GetString(data));

            string welcome = "Welcome to my test server";
            data = Utf8.GetBytes(welcome);
            newsock.Send(data, data.Length, sender);

            while (true)
            {
                data = newsock.Receive(ref sender);

                debug_log(Utf8.GetString(data));
                newsock.Send(data, data.Length, sender);
            }
        }

        void SendBytes()
        {
            var sendBytes = new byte[] { 0x54, 0xfe, 0x97, 0x0, 0x20, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0xf0, 0xbf, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x5, 0x0, 0x73, 0x63, 0x6f, 0x75, 0x74, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x21, 0xd5, };
            UdpClient udpClientB = new UdpClient();
            udpClientB.Send(sendBytes, sendBytes.Length, "localhost", 6002);
            udpClientB.Send(sendBytes, sendBytes.Length, "localhost", 6002);
            udpClientB.Send(sendBytes, sendBytes.Length, "localhost", 6002);
            udpClientB.Send(sendBytes, sendBytes.Length, "localhost", 6002);
            udpClientB.Send(sendBytes, sendBytes.Length, "localhost", 6002);
            udpClientB.Send(sendBytes, sendBytes.Length, "localhost", 6002);
        }
    }
}
