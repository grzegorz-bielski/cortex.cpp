FROM ubuntu:22.04 as base

FROM base as build

ARG CORTEX_CPP_VERSION=latest

ARG CMAKE_EXTRA_FLAGS=""

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    wget \
    jq \
    tar \
    openmpi-bin \
    libopenmpi-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    uuid-dev \
    lsb-release \
    software-properties-common \
    gpg \
    zip \
    unzip \
    gcc \
    g++ \
    ninja-build \
    pkg-config \
    openssl && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

RUN wget -O - https://apt.kitware.com/keys/kitware-archive-latest.asc 2>/dev/null | gpg --dearmor - | tee /etc/apt/trusted.gpg.d/kitware.gpg >/dev/null && \
    apt-add-repository "deb https://apt.kitware.com/ubuntu/ $(lsb_release -cs) main" && \
    apt-get update && \
    apt-get install -y cmake && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY ./engine /app/engine

COPY ./docs/static/openapi/cortex.json /app/docs/static/openapi/cortex.json

RUN cd engine && make configure-vcpkg && make build CMAKE_EXTRA_FLAGS="-DCORTEX_CPP_VERSION=${CORTEX_CPP_VERSION} -DCMAKE_BUILD_TEST=OFF -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake ${CMAKE_EXTRA_FLAGS}"

FROM base as runtime

ENV DEBIAN_FRONTEND=noninteractive

# Install dependencies
RUN apt-get update && apt-get install -y --no-install-recommends \
    ca-certificates \
    curl \
    wget \
    jq \
    tar \
    openmpi-bin \
    libopenmpi-dev && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

ARG CORTEX_LLAMACPP_VERSION=latest

COPY --from=build /app/engine/build/cortex /usr/local/bin/cortex
COPY --from=build /app/engine/build/cortex-server /usr/local/bin/cortex-server

COPY ./docker/download-cortex.llamacpp.sh /tmp/download-cortex.llamacpp.sh

# Get the latest version of the Cortex Llama
RUN chmod +x /tmp/download-cortex.llamacpp.sh && /bin/bash /tmp/download-cortex.llamacpp.sh ${CORTEX_LLAMACPP_VERSION}

# Copy the entrypoint script
COPY ./docker/entrypoint.sh /usr/local/bin/entrypoint.sh

RUN chmod +x /usr/local/bin/entrypoint.sh

EXPOSE 39281

HEALTHCHECK --interval=300s --timeout=30s --start-period=10s --retries=3 \
    CMD curl -f http://127.0.0.1:39281/healthz || exit 1
  
ENTRYPOINT ["/usr/local/bin/entrypoint.sh"]
