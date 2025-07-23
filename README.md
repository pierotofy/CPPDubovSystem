# Swisser

A simple pairing engine server that generates pairings via the FIDE Swiss Dubov system.

## Build

### C++ Version (Original)
```bash
mkdir build && cd build
cmake ..
make -j4
./swisser
```

### Go Version (New)
```bash
go build -o swisser-go .
./swisser-go
```

Both versions support the same command line arguments:
- `--host`: Host to bind to (default: 0.0.0.0)
- `--port`: Port to bind to (default: 8080)

## Usage

Then you can invoke:

```bash
curl -X POST http://localhost:8080/round -d @example.json
```

## API Endpoints

- `GET /` - Returns "Swisser is running"
- `GET /ping` - Returns JSON status: `{"swisser": "running"}`
- `POST /round` - Generates tournament pairings (see example.json for format)

## Testing

For the Go version:
```bash
go test -v
```

## License

Swisser is based on CPPDubovSystem which is licensed under Apache 2.0 (see LICENSE file).

The swisser program is instead licensed under the AGPLv3.
