#See https://aka.ms/containerfastmode to understand how Visual Studio uses this Dockerfile to build your images for faster debugging.

FROM mcr.microsoft.com/dotnet/aspnet:5.0 AS base
WORKDIR /app
EXPOSE 80
EXPOSE 443
EXPOSE 1000

FROM mcr.microsoft.com/dotnet/sdk:5.0 AS build
WORKDIR /src
COPY ["ScaleApi/ScaleApi.csproj", "ScaleApi/"]
RUN dotnet restore "ScaleApi/ScaleApi.csproj"
COPY . .
WORKDIR "/src/ScaleApi"
RUN dotnet build "ScaleApi.csproj" -c Release -o /app/build

FROM build AS publish
RUN dotnet publish "ScaleApi.csproj" -c Release -o /app/publish

FROM base AS final
WORKDIR /app
COPY --from=publish /app/publish .
ENTRYPOINT ["dotnet", "ScaleApi.dll"]