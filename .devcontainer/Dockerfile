FROM postgres:17

# Install development tools and libraries
RUN apt-get update && \
    apt-get install -y \
    build-essential \
    postgresql-server-dev-17 \
    postgresql-server-dev-all \
    pkg-config \
    git \
    curl \
    gdb \
    cmake \
    vim \
    sudo \
    && rm -rf /var/lib/apt/lists/*

# Create symlinks to ensure headers are found
RUN ln -s /usr/include/postgresql /usr/local/include/postgresql && \
    ln -s /usr/include/postgresql/server /usr/local/include/postgresql/server

# Create a non-root user for development
ARG USERNAME=vscode
ARG USER_UID=1000
ARG USER_GID=$USER_UID

RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Set up the PostgreSQL server for development
RUN mkdir -p /var/run/postgresql && chown -R postgres:postgres /var/run/postgresql
USER $USERNAME
WORKDIR /workspace

# Set up entrypoint script
COPY entrypoint.sh /entrypoint.sh
RUN sudo chmod +x /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]
CMD ["sleep", "infinity"]