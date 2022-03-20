using System;
using Microsoft.EntityFrameworkCore.Migrations;
using MySql.EntityFrameworkCore.Metadata;

namespace ScaleApi.Migrations
{
    public partial class powerMeas : Migration
    {
        protected override void Up(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.CreateTable(
                name: "PowerMeasurements",
                columns: table => new
                {
                    Id = table.Column<int>(type: "int", nullable: false)
                        .Annotation("MySQL:ValueGenerationStrategy", MySQLValueGenerationStrategy.IdentityColumn),
                    ReceivedTimestamp = table.Column<DateTime>(type: "datetime", nullable: false),
                    TimeStamp = table.Column<DateTime>(type: "datetime", nullable: false),
                    TotalConsumption1 = table.Column<float>(type: "float", nullable: true),
                    TotalConsumption2 = table.Column<float>(type: "float", nullable: true),
                    TotalProduction1 = table.Column<float>(type: "float", nullable: true),
                    TotalProduction2 = table.Column<float>(type: "float", nullable: true),
                    ActualConsumption = table.Column<float>(type: "float", nullable: true),
                    ActualProduction = table.Column<float>(type: "float", nullable: true),
                    TotalGasConsumption = table.Column<float>(type: "float", nullable: true),
                    ActualTarrif = table.Column<int>(type: "int", nullable: true)
                },
                constraints: table =>
                {
                    table.PrimaryKey("PK_PowerMeasurements", x => x.Id);
                });
        }

        protected override void Down(MigrationBuilder migrationBuilder)
        {
            migrationBuilder.DropTable(
                name: "PowerMeasurements");
        }
    }
}
