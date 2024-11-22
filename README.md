# Re-place

r/place server alternative using c++

# Development

## Cloning the repository

```bash
git clone https://github.com/silentrald/re-place.git
/bin/bash ./submodule.sh
```

## Postgresql

Install `docker` and `docker-compose` in your machine and run the following command in the `sql` folder.

Log in console.
```bash
docker-compose up
```

Detached mode.
```bash
docker-compose up -d
```

[Optional] you can install the postgresql into your machine to use as a client to connect to the dockerized server.

## Valkey

Install [valkey](https://github.com/valkey-io/valkey) by following the instruction in their github repository. After installation, always run the following command to spin up the valkey server.

```
valkey-server
```

## Re-place server

Compile the c++ project with the following commands.

```bash
mkdir build
cd build
cmake ..
make
```

Then run the executable within the build folder.

```bash
./main
```

