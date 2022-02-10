using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using ScaleApi.Data;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using System.Net.Sockets;
using ScaleApi.Models.Db;

namespace ScaleApi.Models.Server
{
    public class TCPServer : IHostedService
    {
        IConfiguration _configuration;
        IServiceScopeFactory _scopeFactory;
        TcpSocketListener _listener;
        List<ClientHandler> _handlers;

        public TCPServer(IServiceScopeFactory scopeFactory, IConfiguration configuration)
        {
            _scopeFactory = scopeFactory;
            _configuration = configuration;
            _listener = new TcpSocketListener();
            _listener.OnClientAccept += _listener_OnClientAccept;
            _handlers = new List<ClientHandler>();
        }

        private void _listener_OnClientAccept(object sender, Socket e)
        {
            ClientHandler handler = new ClientHandler(e, _scopeFactory);
            _handlers.Add(handler);
        }

        public Task StartAsync(CancellationToken cancellationToken)
        {
            return Task.Run(() => {
                string portStr = _configuration["Listener"];
                _listener.BeginListening(int.Parse(portStr));
            });
        }

        public Task StopAsync(CancellationToken cancellationToken)
        {
            return Task.Run(() => {
                //TODO dispose of all open connections!
            });
        }
    }




}
