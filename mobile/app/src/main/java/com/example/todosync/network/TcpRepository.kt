package com.example.todosync.network

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.MutableSharedFlow
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.asSharedFlow
import kotlinx.coroutines.flow.asStateFlow
import kotlinx.coroutines.launch

class TcpRepository {

    private val tcpClient = TcpClientSingleton.tcpClient

    private val _serverMessages = MutableSharedFlow<String>()
    val serverMessages = _serverMessages.asSharedFlow()

    private val _connectionState = MutableStateFlow("Disconnected")
    val connectionState = _connectionState.asStateFlow()

    init {
        tcpClient.onMessageReceived = { message ->
            CoroutineScope(Dispatchers.IO).launch {
                _serverMessages.emit(message)
            }
        }

        connect()
    }

    fun connect() {
        CoroutineScope(Dispatchers.IO).launch {
            tcpClient.connect()
            _connectionState.value = "Connected"
        }
    }

    fun sendMessage(message: String) {
        tcpClient.sendMessage(message)
    }

    fun disconnect() {
        tcpClient.disconnect()
        _connectionState.value = "Disconnected"
    }
}