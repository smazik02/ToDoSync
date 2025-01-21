FROM alpine:latest AS build

RUN apk update && \
    apk add --no-cache \
    build-base \
    cmake

WORKDIR /ToDoSync

COPY build_cmd/ ./build_cmd/
COPY include/ ./include/
COPY src/ ./src/
COPY CMakeLists.txt .

WORKDIR /ToDoSync/build_cmd

RUN cmake ../. && \
    make ToDoSync

FROM alpine:latest

RUN apk update && \
    apk add --no-cache \
	libstdc++ \
    libgcc

RUN addgroup -S todosync && adduser -S todosync -G todosync
USER todosync

COPY --chown=todosync:todosync --from=build \
    ./ToDoSync/build_cmd/ToDoSync \
    ./app/

ENTRYPOINT ["./app/ToDoSync", "12345"]