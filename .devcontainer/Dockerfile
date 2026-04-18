# Geode Mod Development Container

FROM mcr.microsoft.com/devcontainers/cpp:ubuntu

# Install dependencies
RUN apt-get update && apt-get install -y \
    git \
    cmake \
    ninja-build \
    python3 \
    python3-pip \
    curl \
    && rm -rf /var/lib/apt/lists/*

# Install Geode CLI
RUN pip3 install geode-cli

# Set working directory
WORKDIR /workspace

# Clone Geode SDK
RUN git clone https://github.com/geode-sdk/geode.git /opt/geode-sdk

# Set environment variable
ENV GEODE_SDK=/opt/geode-sdk

CMD ["/bin/bash"]
