package com.example.todosync.network

enum class TcpClientMethod(val v: String) {
    // Auth
    AUTH_LOGIN("AUTH|LOGIN"),

    // Task
    T_GET_ALL("T|GET_ALL"),
    T_CREATE("T|CREATE"),
    T_DELETE("T|DELETE"),

    // Task list
    TL_GET_ALL("TL|GET_ALL"),
    TL_CREATE("TL|CREATE"),
    TL_JOIN("TL|JOIN")
}