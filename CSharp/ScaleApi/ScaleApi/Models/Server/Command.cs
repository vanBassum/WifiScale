using Microsoft.Extensions.DependencyInjection;
using ScaleApi.Models.Server.Commands;
using System;
using System.Text.Json;

namespace ScaleApi.Models.Server
{


    class Command
    {
        public Commands CMD { get; set; } = Commands.Unknown;

        public virtual Reply Execute(IServiceScopeFactory scopeFactory)
        {
            return new Reply();
        }

        public enum Commands
        {
            Unknown = 0x00,
            PostMeasurement = 0x01,
            PostPowerMeasurement = 0x02,
        }

        public static Command FromJSON(string json)
        {
            Console.WriteLine($"RX: {json}");
            Command cmd = JsonSerializer.Deserialize<Command>(json);
            switch (cmd.CMD)
            {
                case Commands.PostMeasurement:
                    cmd = JsonSerializer.Deserialize<PostMeasurement>(json);
                    break;
                case Commands.PostPowerMeasurement:
                    cmd = JsonSerializer.Deserialize<PostPower>(json);
                    break;
                default:
                    break;
            }
            return cmd;
        }
    }


    class Reply
    {
        public bool Success { get; set; } = false;
    }
}
