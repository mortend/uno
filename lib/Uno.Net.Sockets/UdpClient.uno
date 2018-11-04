using Uno.Compiler.ExportTargetInterop;

namespace Uno.Net.Sockets
{
    [DotNetType("System.Net.Sockets.UdpClient")]
    public class UdpClient : IDisposable
    {
        const int MaxUdpSize = 0x10000;  

        Socket _clientSocket;
        byte[] _buffer = new byte[MaxUdpSize];
        AddressFamily _family = AddressFamily.InterNetwork;
        bool _active;
        bool _disposed;
        bool _isBroadcast;

        public Socket Client
        {
            get { return _clientSocket; }
            set { _clientSocket = value; }
        }

        public UdpClient()
            : this(AddressFamily.InterNetwork)
        {
        }

        public UdpClient(AddressFamily family)
        {
            if (family != AddressFamily.InterNetwork && family != AddressFamily.InterNetworkV6)
                throw new ArgumentException("UdpClient can only accept InterNetwork or InterNetworkV6 addresses.", nameof(family));

            _family = family;
            Client = new Socket(_family, SocketType.Dgram, ProtocolType.Udp);
        }

        public UdpClient(IPEndPoint localEP)
        {
            if (localEP == null)
                throw new ArgumentNullException("localEP");

            _family = localEP.AddressFamily;
            Client = new Socket(_family, SocketType.Dgram, ProtocolType.Udp);
            Client.Bind(localEP);
        }

        public void Close()
        {
            Dispose(true);
        }

        public void Dispose()
        {
            Dispose(true);
        }

        protected virtual void Dispose(bool disposing)
        {
            if (!disposing)
                return;

            if (_disposed)
                return;

            if (_clientSocket != null)
            {
                _clientSocket.Shutdown(SocketShutdown.Both);
                _clientSocket.Close();
                _clientSocket = null;
            }

            _disposed = true;
        }

        public byte[] Receive(ref IPEndPoint remoteEP)
        {
            if (_disposed)
                throw new ObjectDisposedException(GetType().FullName);

            EndPoint tempRemoteEP;

            if (_family == AddressFamily.InterNetwork)
                tempRemoteEP = IPEndPoint.Any;
            else
                tempRemoteEP = IPEndPoint.IPv6Any;

            int received = Client.ReceiveFrom(_buffer, MaxUdpSize, 0, ref tempRemoteEP);
            remoteEP = (IPEndPoint)tempRemoteEP;

            if (received < MaxUdpSize)
            {
                byte[] newBuffer = new byte[received];
                Array.Copy(_buffer, 0, newBuffer, 0, received);
                return newBuffer;
            }

            return _buffer;
        }

        public int Send(byte[] dgram, int bytes, IPEndPoint endPoint)
        {
            if (_disposed)
                throw new ObjectDisposedException(GetType().FullName);

            if (dgram == null)
                throw new ArgumentNullException(nameof(dgram));

            if (_active && endPoint != null)
                throw new InvalidOperationException("Cannot send packets to an arbitrary host while connected.");

            if (endPoint == null)
                return Client.Send(dgram, 0, bytes, SocketFlags.None);

            CheckForBroadcast(endPoint.Address);
            return Client.SendTo(dgram, 0, bytes, SocketFlags.None, endPoint);
        }

        public int Send(byte[] dgram, int bytes, string hostname, int port)
        {
            if (_disposed)
                throw new ObjectDisposedException(GetType().FullName);

            if (dgram == null)
                throw new ArgumentNullException(nameof("dgram"));

            if (_active && ((hostname != null) || (port != 0)))
                throw new InvalidOperationException("Cannot send packets to an arbitrary host while connected.");

            if (hostname == null || port == 0)
                return Client.Send(dgram, 0, bytes, SocketFlags.None);

            IPAddress[] addresses = Dns.GetHostAddresses(hostname);
            
            int i = 0;
            for (; i < addresses.Length && addresses[i].AddressFamily != _family; i++)
                ;

            if (addresses.Length == 0 || i == addresses.Length)
                throw new ArgumentException("None of the discovered or specified addresses match the socket address family.", nameof(hostname));

            CheckForBroadcast(addresses[i]);
            IPEndPoint ipEndPoint = new IPEndPoint(addresses[i], port);
            return Client.SendTo(dgram, 0, bytes, SocketFlags.None, ipEndPoint);
        }

        void CheckForBroadcast(IPAddress ipAddress)
        {
            if (Client != null && !_isBroadcast && ipAddress.IsBroadcast)
            {
                _isBroadcast = true;
                Client.SetSocketOption(SocketOptionLevel.Socket, SocketOptionName.Broadcast, 1);
            }
        }
    }
}
