FROM ubuntu:24.04

RUN apt-get update && apt-get install -y \
    gcc \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN make && make install

RUN which mzstk && echo "Compiler installed"
