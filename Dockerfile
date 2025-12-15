FROM python:3.12 AS firmware_builder

# Install system dependencies including Git, SSH, curl, zip, and protobuf compiler
RUN apt-get update && apt-get install -y \
    git \
    openssh-client \
    curl \
    zip \
    protobuf-compiler \
    && rm -rf /var/lib/apt/lists/*

# Install AWS CLI for S3 operations
RUN curl "https://awscli.amazonaws.com/awscli-exe-linux-x86_64.zip" -o "awscliv2.zip" && \
    unzip awscliv2.zip && \
    ./aws/install && \
    rm -rf awscliv2.zip aws

WORKDIR /builder
COPY . .
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

# prebuild and fetch dependencies for soil_sensor firmware
# Set default environment variables for build (dummy values for pre-build)
# Note: HOST, TOKEN, DEVICE_ID are expected to be base64 encoded
ENV WIFI_SSID=dummy
ENV WIFI_PASSWORD=dummy
ENV HOST='ZHVtbXk='
ENV TOKEN='ZHVtbXk='
ENV DEVICE_ID='ZHVtbXk='
ENV INSECURE=0
ENV SERVER_PORT=443
RUN cd soil_sensor && pio run
RUN cd 4_chan_relay && pio run

# clean env var
ENV WIFI_SSID=''
ENV WIFI_PASSWORD=''
ENV HOST=''
ENV TOKEN=''
ENV DEVICE_ID=''
ENV INSECURE=''
ENV SERVER_PORT=''
