using Microsoft.Extensions.DependencyInjection;
using System.Text.Json;
using System.Text;
using System.Net.Sockets;
using System;

namespace ScaleApi.Models.Server
{
    public class ClientHandler
    {
        public TcpSocketClient Client { get; }
        IServiceScopeFactory _scopeFactory;
        public ClientHandler(Socket socket, IServiceScopeFactory scopeFactory)
        {
            _scopeFactory = scopeFactory;
            Client = new TcpSocketClient();
            Client.OnDataRecieved += Client_OnDataRecieved;
            Client.SetSocket(socket);
        }

        private void Client_OnDataRecieved(object sender, byte[] e)
        {
            if(sender is TcpSocketClient client)
            {
                try
                {
                    string json = Encoding.ASCII.GetString(e);
                    Command cmd = Command.FromJSON(json);
                    var reply = cmd.Execute(_scopeFactory);
                    string replyJson = JsonSerializer.Serialize(reply);
                    byte[] replyData = Encoding.ASCII.GetBytes(replyJson);
                    Client.SendData(replyData);
                }
                catch(Exception ex)
                {
                    //TODO Send a
                }
                
            }
        }
    }
}
