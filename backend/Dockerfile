FROM mcr.microsoft.com/dotnet/sdk:10.0 AS build
WORKDIR /App

COPY *.csproj ./
RUN dotnet restore

COPY . ./
RUN dotnet publish -c Release -o out

FROM mcr.microsoft.com/dotnet/aspnet:10.0 AS runtime
WORKDIR /app
COPY --from=build /App/out ./

COPY ssl/ca.crt      certs/ca.crt
COPY ssl/backend.crt certs/backend.crt
COPY ssl/backend.key certs/backend.key

CMD [ "dotnet", "ArduinoGatekeeperBackend.dll" ]