using Microsoft.Extensions.DependencyInjection;
using ScaleApi.Models.Server.Commands;
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
        }

        public static Command FromJSON(string json)
        {
            Command cmd = JsonSerializer.Deserialize<Command>(json);
            switch (cmd.CMD)
            {
                case Commands.PostMeasurement:
                    cmd = JsonSerializer.Deserialize<PostMeasurement>(json);
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
