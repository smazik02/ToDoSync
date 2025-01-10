package com.example.todosync.network

import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.SupervisorJob
import kotlinx.coroutines.channels.Channel
import kotlinx.coroutines.launch
import java.io.BufferedReader
import java.io.IOException
import java.io.InputStream
import java.io.InputStreamReader
import java.io.OutputStream
import java.net.Socket

object TcpClientSingleton {
    val tcpClient: TcpClient by lazy {
        TcpClient()
    }
}

class TcpClient {

    private var socket: Socket? = null
    private var outputStream: OutputStream? = null
    private var inputStream: InputStream? = null

    private val scope = CoroutineScope(Dispatchers.IO + SupervisorJob())

    var serverIp: String? = null
    var serverPort: Int? = null
    var onMessageReceived: ((String) -> Unit)? = null
    var statusChannel = Channel<ConnectionState>()

    fun connect() {
        if (serverIp == null || serverPort == null)
            throw IOException("No server ip or port!")

        scope.launch {
            try {
                socket = Socket(serverIp, serverPort!!)
                outputStream = socket?.getOutputStream()
                inputStream = socket?.getInputStream()

                statusChannel.send(ConnectionState.CONNECTED)
                listenForMessages()
            } catch (e: IOException) {
                e.printStackTrace()
                statusChannel.send(ConnectionState.DISCONNECTED)
            }
        }
    }

    fun sendMessage(message: String) {
        scope.launch {
            try {
                outputStream?.write(message.toByteArray())
                outputStream?.flush()
            } catch (e: IOException) {
                e.printStackTrace()
            }
        }
    }

    fun listenForMessages() {
        try {
            val reader = BufferedReader(InputStreamReader(inputStream))
            val buffer = StringBuilder()

            while (true) {
                val line = reader.readLine() ?: break

                buffer.append(line).append("\n")

                if (buffer.endsWith("\n\n")) {
                    val fullMessage = buffer.toString().trimEnd('\n')

                    onMessageReceived?.invoke(fullMessage)
                    buffer.clear()
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
        } finally {
            disconnect()
        }
    }

    fun disconnect() {
        try {
            socket?.close()
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }
}